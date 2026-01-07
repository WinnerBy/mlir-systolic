# Polymer 集成改进路线图

**优先级排序** | **预计工作量** | **影响程度**

## 阶段 1: 依赖距离精度 (高优先级)

### 1.1 实现启发式距离估计

**目标**: 用启发式方法取代 INT64_MAX/INT64_MIN 占位符

**现状**:
```cpp
// lib/Analysis/PolymerAnalysis.cpp, Line 580
// 所有距离都是 INT64_MAX/INT64_MIN（无实际值）
for (int i = 0; i < nLoops; ++i) {
  dist.minDistance = INT64_MAX;  // 占位符
  dist.maxDistance = INT64_MIN;
}
```

**改进方案 A: 从 MLIR 启发式**
```cpp
// 扫描 affine.for 约束
for (auto forOp : func.getOps<affine::AffineForOp>()) {
  // 从 getLowerBound()/getUpperBound() 提取步幅
  // 计算迭代空间大小
  // 估计数据重用距离
}
```

**改进方案 B: 从循环体依赖启发式**
```cpp
// 分析 affine.load/store 的索引
// 比较相邻迭代的访问模式
// 估计距离 ∈ {1, loop_bound, ∞}
```

**预计时间**: 2-3 天  
**影响**: 使空间循环选择从保守 → 准确

### 1.2 扩展 ISL 绑定暴露访问关系

**目标**: 获取 Polymer 内部访问关系用于精确计算

**当前障碍**:
```cpp
// Polymer 中 IslScop 定义 (third_party/Polygeist/...)
class IslScop {
private:
  std::vector<IslStmt> islStmts;  // ← 私有！
  isl_union_set *domain;
};

// 我们需要：
// - 访问关系 (Access maps)
// - 可达性分析 (Flow analysis)
// - 依赖关系 (Dependence relations)
```

**实现路径**:
1. 在 Polymer 中添加公开 getter：
   ```cpp
   isl_union_map* IslScop::getAccessMap(AccessType type);
   ```

2. 在 PolymerAnalysis 中实现依赖计算：
   ```cpp
   isl_union_map *PolymerScop::computeDependences() {
     auto scop = static_cast<polymer::IslScop*>(this->scop);
     isl_union_map *reads = scop->getAccessMap(AccessType::Read);
     isl_union_map *writes = scop->getAccessMap(AccessType::Write);
     return computeFlow(writes, reads);
   }
   ```

**预计时间**: 3-4 天  
**影响**: 完全依赖分析，替换当前 nullptr 实现

### 1.3 距离向量提取

**目标**: 从依赖映射提取距离向量 (d0, d1, d2)

**数学基础**:
```
给定:
  S1 -> S2: {[i,j,k] -> [i,j,k+1] : 0≤i,j,k<32}
则距离向量: (0, 0, 1)
```

**实现代码框架**:
```cpp
void extractDistancesFromMap(isl_union_map *depMap, 
                            SmallVectorImpl<LoopDependenceDistance> &distances) {
  // 对每个依赖元组 (src, dst)
  isl_union_map_foreach_map(depMap, [](isl_map *map, void *user) {
    // 对每个整数点
    isl_map_foreach_point(map, [](isl_point *pt, void *user) {
      // 计算 dst - src = 距离
      // 累积最小/最大值
    });
  });
}
```

**预计时间**: 1-2 天  
**影响**: 实现精确的依赖距离计算

---

## 阶段 2: 调度树优化 (中优先级)

### 2.1 基于依赖的调度优化

**目标**: 使用 ISL 调度优化算法而非默认序列调度

**问题**: Polymer 构建的序列调度缺乏循环级别的优化

**解决方案**:
```cpp
isl_schedule *PolymerScop::optimizeSchedule() {
  // 1. 从 SCoP 获取约束
  isl_union_set *domain = scop->getDomain();
  isl_union_map *deps = scop->getDependences();
  
  // 2. 使用 ISL 调度优化
  isl_schedule_constraints *constraints = 
    isl_schedule_constraints_on_domain(domain);
  constraints = isl_schedule_constraints_set_validity(constraints, deps);
  constraints = isl_schedule_constraints_set_proximity(constraints, deps);
  
  // 3. 计算优化的调度
  isl_schedule *optimized = 
    isl_schedule_constraints_compute_schedule(constraints);
  
  return optimized;
}
```

**预计时间**: 2-3 天  
**影响**: 改进调度树结构，更好的循环信息

### 2.2 提取带状节点特性

**目标**: 从优化的调度树中提取每个维度的属性

**需要提取**:
```cpp
struct LoopCharacteristics {
  int loopIndex;
  bool isParallel;        // 可并行化？
  bool isVectorizable;    // 可向量化？
  bool hasCarriedDeps;    // 有循环携带依赖？
  int maxDistance;        // 最大依赖距离
};
```

**实现**:
```cpp
void extractLoopCharacteristics(isl_schedule_node *bandNode,
                               SmallVectorImpl<LoopCharacteristics> &chars) {
  // ISL 提供的接口：
  bool is_parallel = isl_schedule_node_band_member_get_coincident(node, i);
  
  // 从依赖分析推导其他属性
  for (int i = 0; i < nMembers; i++) {
    LoopCharacteristics c;
    c.isParallel = is_parallel;
    c.isVectorizable = checkVectorizability(i);
    c.hasCarriedDeps = checkDependences(i);
    c.maxDistance = computeMaxDist(i);
    chars.push_back(c);
  }
}
```

**预计时间**: 1-2 天  
**影响**: 启用更精细的循环分类

---

## 阶段 3: 空间-时间循环选择优化 (高优先级)

### 3.1 AutoSA 兼容的循环选择

**目标**: 实现符合 AutoSA 标准的 2D/3D 空间循环选择

**当前情况**:
```cpp
// 所有循环都被标记为空间候选（保守）
for (auto &dist : distances) {
  dist.canBeSpaceLoop = true;  // ← 太宽松
}
```

**改进**:
```cpp
// AutoSA 标准：
// 1. 选择 k 个循环作为空间循环（k = sqrt(PE_ARRAY)）
// 2. 优先选择：
//    a) 有界循环（已知大小）
//    b) 无循环携带依赖的循环
//    c) 最内层循环（内存访问效率）

LogicalResult selectSpaceLoops(
    SmallVectorImpl<LoopDependenceDistance> &distances,
    int numPEs,  // 处理元素数量
    SmallVectorImpl<int> &spaceLoopIndices) {
  
  int target = (int)sqrt(numPEs);  // 2D 平铺
  
  // 评分函数：高分 = 好的空间循环候选
  auto score = [](const LoopDependenceDistance &d) {
    int s = 0;
    s += (!d.hasCarriedDeps) ? 100 : 0;
    s += (d.isBounded) ? 50 : 0;
    s += (d.loopIndex) ? 10 : 0;  // 优先内层
    return s;
  };
  
  // 选择得分最高的 target 个循环
  std::vector<std::pair<int, int>> scored;
  for (auto &d : distances) {
    scored.push_back({score(d), d.loopIndex});
  }
  std::sort(scored.rbegin(), scored.rend());
  
  for (int i = 0; i < target && i < scored.size(); i++) {
    spaceLoopIndices.push_back(scored[i].second);
  }
  
  return success();
}
```

**预计时间**: 2-3 天  
**影响**: 从启发式选择 → 自动化 AutoSA 兼容选择

### 3.2 集成到 SpaceTimeAnalysis

**目标**: 将新的循环选择器集成到决策流程

**当前代码**:
```cpp
// lib/Analysis/SpaceTimeAnalysis.cpp
LogicalResult tryPolymerDependenceAnalysis(...) {
  // 获取距离信息
  computeDependenceDistancesWithPolymer(func, distances);
  
  // TODO: 这里添加智能循环选择
  for (auto &dist : distances) {
    dist.canBeSpaceLoop = true;  // ← 需要改进
  }
}
```

**改进**:
```cpp
LogicalResult tryPolymerDependenceAnalysis(...) {
  // 1. 计算依赖距离
  computeDependenceDistancesWithPolymer(func, distances);
  
  // 2. 选择最优空间循环
  SmallVector<int, 2> spaceLoops;
  if (failed(selectSpaceLoops(distances, options.numPEs, spaceLoops))) {
    return failure();
  }
  
  // 3. 标记选中的循环
  for (int i = 0; i < distances.size(); i++) {
    distances[i].canBeSpaceLoop = 
      std::find(spaceLoops.begin(), spaceLoops.end(), i) 
      != spaceLoops.end();
  }
  
  return success();
}
```

**预计时间**: 1 天  
**影响**: 自动化的空间循环选择

---

## 阶段 4: 测试与验证 (中优先级)

### 4.1 扩展测试套件

**现有测试**:
- 3x3x32 矩阵乘法 ✅

**需要新增**:

| 测试用例 | 特征 | 预期行为 |
|---------|------|---------|
| 2x2 矩阵乘法 | 最小情况 | 所有循环标记为空间 |
| 64x64 矩阵乘法 | 大规模 | 仅 2 个最内层循环为空间 |
| 4D 张量收缩 (MTTKRP) | 复杂依赖 | 选择 3 个无携带依赖循环 |
| CNN 卷积 | 非完美嵌套 | 处理模式匹配失败 |
| LU 分解 | 三角形依赖 | 识别向前依赖 |

**创建方式**:
```bash
# 1. 修改 test/matmul_test.mlir 改变大小
# 2. 生成 MTTKRP 内核（从 AutoSA 参考）
# 3. 使用 Polygeist 将 C 代码转换为 MLIR
```

**预计时间**: 2-3 天  
**影响**: 验证系统在多种场景下的正确性

### 4.2 性能基准测试

**测量指标**:
1. 编译时间开销 (与禁用 Polymer 对比)
2. 选择准确性 (与手动标注对比)
3. 生成的 HLS 代码质量

**基准测试框架**:
```bash
#!/bin/bash
for kernel in matmul2 matmul64 mttkrp lu cnn; do
  echo "Testing $kernel..."
  time ./bin/systolic-opt test/${kernel}.mlir --systolic-transform > /tmp/out.mlir
  
  # 计算生成的循环层数
  grep "affine.for" /tmp/out.mlir | wc -l
done
```

**预计时间**: 2 天  
**影响**: 量化性能特征

---

## 阶段 5: 文档与维护 (低优先级)

### 5.1 API 文档

**需要文档化**:
- PolymerAnalysis.h 接口
- LoopDependenceDistance 结构
- 选择算法的数学基础

**格式**: Doxygen 注释

### 5.2 故障排除指南

**常见问题**:
1. "SCoP extraction failed"
   - 原因清单
   - 诊断步骤
   - 解决方案

2. "Loop dimension = 0"
   - 调试技巧
   - 预期输出示例

3. "Dependence analysis returns nullptr"
   - 当前的已知限制
   - 工作区方案

**预计时间**: 1 天  
**影响**: 改进用户体验

---

## 实现优先级建议

### 推荐顺序 (最高效)

```
周 1: 实现启发式距离估计 (1.1)
      + 扩展测试套件 (4.1)
    
周 2: 扩展 ISL 绑定 (1.2)
      + 距离向量提取 (1.3)
    
周 3: 基于依赖的调度优化 (2.1)
      + AutoSA 兼容循环选择 (3.1)
      + 集成到 SpaceTimeAnalysis (3.2)
    
周 4: 性能基准测试 (4.2)
      + 文档与维护 (5.1, 5.2)
```

**总预计工作量**: 4-5 周 (1 人)  
**总影响**: 从基础实现 → 完全优化的生产系统

---

## 技术债务清单

### 现在需要解决的

- [ ] INT64_MAX/INT64_MIN 占位符 (改为启发式)
- [ ] nullptr 依赖计算 (实现或记录为已知限制)
- [ ] 保守的循环选择 (实现智能选择)

### 将来考虑的

- [ ] 多面体并行化分析
- [ ] 向量化感知调度
- [ ] 内存访问模式优化
- [ ] GPU 架构目标

---

## 相关文件与资源

**核心实现**:
- lib/Analysis/PolymerAnalysis.cpp (599 行)
- lib/Analysis/SpaceTimeAnalysis.cpp (~400 行)

**依赖库**:
- third_party/Polygeist/lib/Polymer/
- ISL (Integer Set Library)

**参考资源**:
- AutoSA 源码: https://github.com/UCLA-VAST/AutoSA
- ISL 手册: https://isl.gforge.inria.fr/
- Polymer 论文: "Polymer: A Multilevel Polyhedral Compiler"

---

**维护者**: 编译器团队  
**最后更新**: 2026-01-07  
**版本**: 1.0 初稿

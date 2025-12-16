# Polymer 集成实现状态

> **更新日期**: 2025-12-15  
> **状态**: 🟡 **部分实现 - SCoP 提取框架完成，需要完善细节**

---

## ✅ 已完成的实现

### 1. SCoP 提取基础框架 (`PolymerScop::extract`)

**文件**: `lib/Analysis/PolymerAnalysis.cpp`

**已实现**:
- [x] 创建 `IslScop` 对象
- [x] 初始化符号表 (`initializeSymbolTable`)
- [x] 遍历 affine.for 循环
- [x] 识别语句（load/store 操作）
- [x] 创建 SCoP 语句 (`createStatement`)
- [x] 构建调度树 (`buildSchedule`)

**当前实现**:
```cpp
std::unique_ptr<PolymerScop> PolymerScop::extract(func::FuncOp func) {
  // 1. 创建 IslScop 对象
  polymer::IslScop *scop = new polymer::IslScop();
  scop->initializeSymbolTable(func, nullptr);
  
  // 2. 查找所有 affine.for 循环
  SmallVector<affine::AffineForOp, 8> loops;
  func.walk([&](affine::AffineForOp forOp) {
    loops.push_back(forOp);
  });
  
  // 3. 识别语句并创建 SCoP 语句
  for (Operation *stmt : statements) {
    scop->createStatement();
  }
  
  // 4. 构建调度树
  scop->buildSchedule(scheduleOps);
  
  return std::unique_ptr<PolymerScop>(new PolymerScop(scop));
}
```

### 2. 调度树计算 (`PolymerScop::computeSchedule`)

**已实现**:
- [x] 检查 SCoP 有效性
- [x] 获取已有调度树（在 SCoP 提取时构建）
- [x] 错误处理

**当前实现**:
```cpp
isl_schedule *PolymerScop::computeSchedule() {
  polymer::IslScop *scop = static_cast<polymer::IslScop*>(this->scop);
  isl_schedule *schedule = scop->getSchedule();
  if (schedule) {
    return isl_schedule_copy(schedule);  // 返回副本
  }
  return nullptr;  // 调度树应在提取时构建
}
```

### 3. 依赖计算框架 (`PolymerScop::computeDependences`)

**已实现**:
- [x] 基础框架
- [x] 从调度树获取 domain
- [x] 错误处理

**待完善**:
- [ ] 提取访问关系（read/write relations）
- [ ] 使用 `isl_union_map_compute_flow()` 计算依赖
- [ ] 处理输出依赖（WAW）和反依赖（WAR）

---

## ⚠️ 待完善的功能

### 1. 完善 SCoP 提取 - Domain 和 Access Relations

**问题**: 当前实现只创建了语句，但没有添加 domain 和 access relations。

**需要实现**:

```cpp
// 为每个语句添加 domain relation
void addDomainRelation(int stmtId, 
                       FlatAffineValueConstraints &cst);

// 为每个访问添加 access relation
LogicalResult addAccessRelation(int stmtId, bool isRead, 
                                Value memref,
                                AffineValueMap &vMap,
                                FlatAffineValueConstraints &cst);
```

**实现步骤**:

1. **提取循环边界约束**:
   ```cpp
   // 对于每个循环 for i = lb to ub
   // 构建约束: lb <= i < ub
   FlatAffineValueConstraints domainCst;
   for (auto forOp : enclosingLoops) {
     // 添加循环边界约束
     domainCst.addBound(/* lower bound */);
     domainCst.addBound(/* upper bound */);
   }
   scop->addDomainRelation(stmtId, domainCst);
   ```

2. **提取访问关系**:
   ```cpp
   // 对于 load/store 操作
   AffineValueMap vMap;
   loadOp.getAffineValueMap(&vMap);
   
   FlatAffineValueConstraints accessCst;
   // 从 vMap 构建访问约束
   
   scop->addAccessRelation(stmtId, /* isRead */, memref, 
                           vMap, accessCst);
   ```

**参考**: Polymer 的 `extractScopStmt()` 实现

### 2. 完善依赖计算

**问题**: 当前实现返回 nullptr，需要实际计算依赖关系。

**需要实现**:

```cpp
isl_union_map *PolymerScop::computeDependences() {
  // 1. 从 SCoP 提取所有 read 关系
  isl_union_map *reads = extractReadRelations();
  
  // 2. 从 SCoP 提取所有 write 关系
  isl_union_map *writes = extractWriteRelations();
  
  // 3. 计算 flow dependences (RAW)
  isl_union_map *flow = isl_union_map_compute_flow(
    isl_union_map_copy(reads),
    isl_union_map_copy(writes),
    isl_union_map_copy(writes),
    isl_union_set_copy(domain)
  );
  
  // 4. 计算 output dependences (WAW)
  isl_union_map *output = isl_union_map_compute_flow(
    isl_union_map_copy(writes),
    isl_union_map_copy(writes),
    isl_union_map_copy(reads),
    isl_union_set_copy(domain)
  );
  
  // 5. 合并所有依赖
  isl_union_map *deps = isl_union_map_union(flow, output);
  
  return deps;
}
```

**参考**: AutoSA 的依赖计算实现

### 3. 实现依赖距离计算

**需要实现**:

```cpp
isl_union_map *PolymerScop::computeDependenceDistances(
    isl_union_map *deps) {
  if (!deps) return nullptr;
  
  // 1. 获取调度映射
  isl_schedule *schedule = getSchedule();
  isl_union_map *scheduleMap = isl_schedule_get_map(schedule);
  
  // 2. 计算距离向量
  // 距离 = schedule(target) - schedule(source)
  isl_union_map *distances = isl_union_map_subtract_domain(
    isl_union_map_apply_range(
      isl_union_map_copy(deps),
      isl_union_map_copy(scheduleMap)
    ),
    isl_union_map_apply_domain(
      isl_union_map_copy(deps),
      isl_union_map_copy(scheduleMap)
    )
  );
  
  return distances;
}
```

**参考**: AutoSA 的 `get_dep_dis_at_node` 实现

### 4. 实现调度树分析

**需要实现**:

```cpp
LogicalResult analyzeScheduleTree(isl_schedule *schedule,
                                   ScheduleTreeInfo &info) {
  // 1. 遍历调度树节点
  isl_schedule_node *root = isl_schedule_get_root(schedule);
  
  // 2. 提取每个 band 节点的信息
  traverseScheduleTree(root, [&](isl_schedule_node *node) {
    if (isl_schedule_node_get_type(node) == isl_schedule_node_band) {
      // 提取 band 的维度信息
      // 提取依赖距离
      // 识别空间循环候选
    }
  });
  
  isl_schedule_node_free(root);
  return success();
}
```

**参考**: ISL Schedule Tree API 文档

### 5. 实现依赖距离提取

**需要实现**:

```cpp
// 在 computeDependenceDistancesWithPolymer 中
// Step 5: Extract distances for each loop dimension
for (auto &dep : distanceMap) {
  // 解析距离映射，提取每个循环维度的距离
  // 计算 min/max 距离
  // 判断是否为均匀依赖
  // 判断是否可以作为空间循环 (distance <= 1)
  
  LoopDependenceDistance dist;
  dist.loopIndex = /* loop index */;
  dist.minDistance = /* min */;
  dist.maxDistance = /* max */;
  dist.isUniform = /* check if constant */;
  dist.canBeSpaceLoop = (dist.maxDistance <= 1);
  
  distances.push_back(dist);
}
```

---

## 📋 下一步实现计划

### 优先级 1: 完善 SCoP 提取（Domain 和 Access Relations）

**目标**: 能够正确提取 domain 和 access relations

**步骤**:
1. 研究 Polymer 的 `addDomainRelation` 和 `addAccessRelation` API
2. 实现循环边界约束提取
3. 实现访问映射提取
4. 测试 SCoP 提取的正确性

**预计时间**: 2-3 天

### 优先级 2: 实现依赖计算

**目标**: 能够计算数据依赖关系

**步骤**:
1. 从 SCoP 提取 read/write 关系
2. 使用 ISL 计算 flow 和 output 依赖
3. 测试依赖计算的正确性

**预计时间**: 2-3 天

### 优先级 3: 实现依赖距离计算

**目标**: 能够计算每个循环的依赖距离

**步骤**:
1. 实现距离向量提取
2. 实现 min/max 距离计算
3. 测试距离计算的正确性

**预计时间**: 2-3 天

### 优先级 4: 实现调度树分析

**目标**: 能够从调度树中提取依赖信息

**步骤**:
1. 研究 ISL Schedule Tree API
2. 实现调度树遍历
3. 提取依赖距离和空间循环候选
4. 测试分析结果的正确性

**预计时间**: 2-3 天

---

## 🔍 关键挑战

### 1. Polymer API 文档不完整

**问题**: Polymer 的 API 文档较少，需要阅读源代码。

**解决方案**:
- 参考 Polymer 的测试用例
- 参考 AutoSA 的实现
- 逐步实现，每个功能单独测试

### 2. ISL API 复杂度

**问题**: ISL API 使用约定（ownership, __isl_give/__isl_take）需要仔细处理。

**解决方案**:
- 仔细阅读 ISL 文档
- 参考 AutoSA 的 ISL 使用方式
- 使用 ISL 的内存管理工具

### 3. Domain 和 Access Relation 构建

**问题**: 从 MLIR Affine IR 构建 ISL 约束需要理解多面体模型。

**解决方案**:
- 参考 Polymer 的实现
- 使用 MLIR 的 AffineStructures 工具
- 逐步构建约束

---

## 📚 参考资源

### 代码库
- **Polymer**: `/home/user/work/Polygeist/tools/polymer/`
- **AutoSA**: AutoSA 的依赖计算实现
- **ISL**: ISL 文档和示例

### 关键文件
- `lib/Analysis/PolymerAnalysis.cpp` - 当前实现
- `include/systolic/Analysis/PolymerAnalysis.h` - 接口定义
- `docs/AUTOSA_ANALYSIS.md` - AutoSA 分析文档

---

**文档维护**: 本文档应随着实现进展持续更新。


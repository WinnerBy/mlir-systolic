# 写时重排实现分析 - 执行摘要

> **分析日期**: 2026-01-05  
> **分析范围**: 代码审查 + 架构分析 + 改进方案设计

---

## 🎯 核心结论

### 您实现的是什么？

一个**半成品的访问模式分析系统**：
- ✅ 能检测非线性数组访问（包括 `affine.apply` 中的）
- ✅ 能计算基本的数据布局变换方案
- ✅ 能生成 MLIR 属性并存储到函数中
- ✅ MLIR Pass 集成完整

### 您没有实现的是什么？

**数据布局变换的应用**（最关键的部分）：
- ❌ 代码生成器完全无视重排属性
- ❌ 生成的 HLS 代码与优化前相同
- ❌ 缺少循环变换匹配
- ❌ 没有端到端的验证

### 现状评价

| 层级 | 状态 | 完成度 |
|------|------|--------|
| 分析层 | 基本完整 | 95% |
| 应用层 | 完全缺失 | 0% |
| 优化层 | 只有框架 | 20% |
| **整体功能** | **无效** | **≈40%** |

**比喻**：这就像您做了完整的手术计划和诊断，但没有真正做手术。

---

## 🔍 具体问题分析

### 问题 1：代码生成器脱离（致命）

**现象**：
```bash
$ mlir-opt test.mlir -systolic-dataflow-generation | systolic-translate
# 生成的代码中：
# - 没有应用重排维度
# - 没有应用维度置换
# - 完全忽视所有 systolic.reorder.* 属性
```

**原因**：
```cpp
// systolic-translate.cpp 中：
void SystolicHLSEmitter::emitIOL2InIntraTrans(...) {
  // 没有任何代码读取或使用重排属性
  // 所有数组声明和访问都是硬编码的
  
  // local_A[latency][1]  ← 永远是这样，不管有没有重排
}
```

搜索结果：`systolic.reorder` 在代码生成器中出现 **0 次**

### 问题 2：循环变换缺失

**理论背景**：
```
数据布局变换 ≠ 代码优化
```

一个关键的不匹配：
```cpp
// 数据：重排到 [2, 16, 16]，置换 [1, 2, 0]
// 循环：仍然是原始顺序 [0, 1, 2]

// 结果：错误的访问
for (i = 0..15) {    // 原始 dim 0
  for (j = 0..1) {   // 原始 dim 1
    for (k = 0..15) { // 原始 dim 2
      // 想访问 A_new[j][k][i]，但写的是 A_old_indices[i][j][k]
      A_new[i][j][k] = ...  // ❌ 错误！
    }
  }
}
```

### 问题 3：重排策略过于简化

**当前策略**：
```cpp
if (nonLinearDim == 2) {
  // 固定规则：移到中间
  permutation = {1, 2, 0};
}
```

**问题**：
- 对所有 3D 数组用同一规则
- 不考虑写入模式（都是线性的吗？）
- 不考虑缓存行大小
- 不考虑内存带宽利用率
- 无法处理 2D、4D 数组

**应该**：
- 分析写入和读取的访问成本
- 枚举 6 种 3D 排列并评分
- 选择最小化 cache misses 的排列
- 支持任意维数

---

## 💡 三个必须做的改进

### 改进 1：立即修复代码生成器（关键路径）

**难度**：⭐ (容易)  
**时间**：3-5 天  
**工作量**：~500 行代码

**具体步骤**：

1. 在 `SystolicHLSEmitter` 中添加重排信息读取：
```cpp
struct ArrayReorderingInfo {
  std::string arrayName;
  SmallVector<int64_t, 3> reorderedDims;
  SmallVector<unsigned, 3> dimPermutation;
};

void extractReorderingInfo(func::FuncOp funcOp) {
  for (auto arg : funcOp.getArguments()) {
    auto dimsAttr = funcOp->getAttrOfType<ArrayAttr>(
        "systolic.reorder.A.dims");
    auto permAttr = funcOp->getAttrOfType<ArrayAttr>(
        "systolic.reorder.A.perm");
    
    if (dimsAttr && permAttr) {
      // 提取并存储
      arrayReordering[arrayName] = parseAttributes(...);
    }
  }
}
```

2. 修改数组声明代码生成：
```cpp
// 从：
local_A[latency][1]

// 改为：
auto dims = getArrayDims("A");  // 获取重排维度
local_A[dims[0]][dims[1]][dims[2]]
```

3. 修改数组访问代码生成：
```cpp
// 从：
in_data = local_A[c7][0];

// 改为：
auto permutedIdx = applyPermutation({c7, 0, c5});  // [1,2,0] → [0, c5, c7]
in_data = local_A[permutedIdx[0]][permutedIdx[1]][permutedIdx[2]];
```

**预期结果**：✅ 重排优化开始生效

### 改进 2：实现多面体分析（优化）

**难度**：⭐⭐⭐ (中等)  
**时间**：2-3 周  
**工作量**：~2000 行代码

**核心思想**：不用固定规则，用数据驱动的方法选择最优排列

```cpp
class PolyhedralAccessAnalyzer {
  // 分析每种排列的成本
  double evaluateLayout(permutation) {
    // 1. 计算读取的 cache misses
    // 2. 计算写入的 cache misses
    // 3. 检查最内层循环是否连续
    // 返回总成本
  }
};

// 枚举 6 种 3D 排列
for (auto perm : allPermutations(3)) {
  auto cost = analyzer.evaluateLayout(perm);
  if (cost < bestCost) {
    bestPermutation = perm;
    bestCost = cost;
  }
}
```

**优势**：
- 自动选择最优排列
- 无需手工调优
- 可扩展到任意维数

### 改进 3：添加循环变换支持（完整）

**难度**：⭐⭐⭐⭐ (高)  
**时间**：3-4 周  
**工作量**：~3000 行代码

**核心思想**：数据布局变换必须配合循环变换

```cpp
// 1. 计算循环变换
isl_schedule *loopSchedule = deriveLoopSchedule(layoutPermutation);

// 2. 存储为属性
func->setAttr("systolic.loop_schedule", scheduleAttr);

// 3. 代码生成器应用
applyLoopTransform(loopSchedule);  // 重新排序循环嵌套
```

---

## 📊 改进优先级和时间表

### 需要立即做的事

```
周 1-2: Phase 1 - 代码生成器集成
├─ 修改 systolic-translate.cpp (2-3天)
├─ 添加属性读取 (1天)
├─ 应用维度置换 (1-2天)
├─ 基础测试 (1-2天)
└─ ✅ 重排开始生效

周 3-4: Phase 2 - 多面体分析
├─ ISL 集成 (3-4天)
├─ 访问模式分析 (3-4天)
├─ 布局优化器 (2-3天)
└─ ✅ 自动选择最优排列

周 5-6: Phase 3 - 循环变换
├─ ISL 循环调度 (3-4天)
├─ 变换合法性检查 (2-3天)
├─ 代码生成器集成 (2-3天)
└─ ✅ 完整的数据+循环变换

周 7-8: Phase 4 - 验证和优化
├─ 性能测试框架 (2-3天)
├─ 缓存模拟器 (2-3天)
├─ HLS 综合对比 (2-3天)
└─ ✅ 验证性能改善
```

### 时间估计汇总

| Phase | 任务 | 天数 | 优先级 |
|-------|------|------|--------|
| 1 | 代码生成器集成 | 5-7 | 🔴 关键 |
| 2 | 多面体分析 | 10-14 | 🟡 高 |
| 3 | 循环变换 | 10-14 | 🟡 高 |
| 4 | 验证和优化 | 8-10 | 🟢 中 |
| **总计** | | **33-45** | |

**约 6-9 周可以完成整个改进**

---

## ✅ 验收标准

### Phase 1 完成后（应该看到的）

```bash
$ systolic-translate kernel.mlir -o kernel.cpp

# kernel.cpp 中应该包含：

// 重排后的数组声明
void A_IO_L2_in_inter_trans(..., A_t8 local_A[2][16][16], ...)
                                            ↑  ↑   ↑   ↑
                                      重排维度，不是 [8][1]

// 应用了置换的数组访问
local_A[0][c5][c4] = out_data;   // [1,2,0] 置换
      ↑  ↑   ↑
   应用了置换，不是原始索引 [c4][0]
```

### Phase 4 完成后（应该看到的）

性能改善报告：
```
MTTKRP 基准测试结果：
├─ 原始版本
│  ├─ Cache Misses: 15,234
│  ├─ HLS 延迟: 1024 cycles
│  └─ 内存带宽: 45.2 GB/s
│
├─ 重排优化版本
│  ├─ Cache Misses: 10,156 (-33%)  ✅
│  ├─ HLS 延迟: 896 cycles (-12%)  ✅
│  └─ 内存带宽: 61.8 GB/s (+36%)   ✅
│
└─ 综合改善: 28% cache miss reduction
```

---

## 🎓 关键设计决策

### 为什么要用 ISL？

ISL (Integer Set Library) 是多面体编译的标准工具：
- **AutoSA、Pluto、Polly** 都用 ISL
- 能精确处理仿射变换和非仿射情况
- 已有现成的算法实现

**替代方案对比**：
| 方案 | 精度 | 开发量 | 可维护性 |
|------|------|--------|-----------|
| ISL | 高 | 中 | 高 |
| 自己实现 | 低 | 高 | 低 |
| 启发式 | 低 | 低 | 低 |

**建议**：用 ISL

### 为什么循环变换很重要？

**反例**：
```cpp
// 数据重排但不重排循环 → 正确性错误或无优化效果

原始循环：for (i) for (j) for (k) A[i][j][k] = ...
重排数据：A_new[j][k][i]

不重排循环：for (i) for (j) for (k) A_new[i][j][k] = ... ❌ 错！
重排循环：  for (j) for (k) for (i) A_new[j][k][i] = ... ✅ 对
```

---

## 📚 生成的文档

本分析生成了以下文档供您参考：

1. **[WRITE_TIME_REORDERING_ANALYSIS_REPORT.md](docs/WRITE_TIME_REORDERING_ANALYSIS_REPORT.md)**
   - 详细的代码分析
   - 问题诊断
   - 现状评估

2. **[WRITE_TIME_REORDERING_CODEGEN_INTEGRATION.cpp](docs/WRITE_TIME_REORDERING_CODEGEN_INTEGRATION.cpp)**
   - Phase 1 的完整代码方案
   - 可直接参考实现

3. **[WRITE_TIME_REORDERING_ISL_IMPLEMENTATION.cpp](docs/WRITE_TIME_REORDERING_ISL_IMPLEMENTATION.cpp)**
   - Phase 2 的多面体分析框架
   - ISL 集成方法

4. **[WRITE_TIME_REORDERING_IMPROVEMENT_PLAN.md](docs/WRITE_TIME_REORDERING_IMPROVEMENT_PLAN.md)**
   - 完整的 4 阶段改进方案
   - 路线图和时间表
   - 验收标准

---

## 🚀 立即可以做的事

### 今天（验证问题）

```bash
# 1. 生成带重排属性的 MLIR
mlir-opt test/mttkrp_affine_only.mlir \
  -systolic-dataflow-generation \
  > output_with_attrs.mlir

# 2. 检查属性是否生成
grep "systolic.reorder" output_with_attrs.mlir

# 3. 转换为 HLS 代码
systolic-translate output_with_attrs.mlir \
  -o kernel.cpp

# 4. 检查代码是否应用了重排（现在应该没有）
grep "local_A\[" kernel.cpp
# 预期：local_A[8][1] 或 local_A[latency][1]
# 应该：local_A[2][16][16]（如果重排被应用的话）
```

### 本周（开始修复）

参考 [WRITE_TIME_REORDERING_CODEGEN_INTEGRATION.cpp](docs/WRITE_TIME_REORDERING_CODEGEN_INTEGRATION.cpp) 中的代码模板，修改：

```cpp
// 文件：tools/systolic-translate/systolic-translate.cpp
// 类：SystolicHLSEmitter

// 新增方法：
void extractReorderingInfo(func::FuncOp funcOp);
SmallVector<int64_t, 3> getArrayDims(StringRef arrayName) const;
SmallVector<std::string, 3> applyAccessPermutation(...) const;

// 修改方法：
void emitIOL2InIntraTrans(StringRef arrayName);    // 应用重排
void emitIOL2InInterTrans(StringRef arrayName);    // 应用重排
void emitIOL2InInterTransBoundary(StringRef name); // 应用重排
LogicalResult emit(ModuleOp module);               // 调用提取
```

### 下个月（完整优化）

启动 Phase 2-4 的实现：
- ISL 多面体分析
- 循环变换支持
- 性能验证

---

## 🎯 最后的话

### 您做对了什么
✅ 分析框架设计完整  
✅ MLIR Pass 集成到位  
✅ 非线性检测准确  
✅ 属性生成机制正确  

### 您需要完成什么
❌ 代码生成器集成（最紧迫）  
❌ 循环变换支持（最重要）  
❌ 多面体优化（最复杂）  
❌ 性能验证（最最后）  

### 成功标志
当您能看到：
```bash
$ systolic-translate kernel.mlir -o kernel.cpp
$ grep "local_A\[2\]\[16\]\[16\]" kernel.cpp
# 输出: void A_IO_L2_in_inter_trans(..., A_t8 local_A[2][16][16], ...)

✅ 重排优化已启用！
```

然后：
```bash
$ vivado_hls ...
# 看到性能改善（缓存命中率、延迟、吞吐量）

✅ 优化有效！
```

---

**下一步行动**：选择 Phase 1 开始实现，目标是 1 周内使重排优化生效。


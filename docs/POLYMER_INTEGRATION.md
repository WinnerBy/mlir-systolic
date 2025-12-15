# Polymer 集成方案

> **状态**: 待实现 ⚠️  
> **优先级**: **最高** ⭐⭐⭐  
> **最后更新**: 2025-12-10

---

## 为什么需要 Polymer

### 核心问题

**Polymer 是 mlir-systolic 的核心依赖，不是可选的**。

1. **AutoSA 基于多面体模型**：
   - AutoSA 使用 ISL (Integer Set Library) 进行依赖分析和循环变换
   - 依赖 ISL Schedule Tree 来确定硬件模块的边界
   - 没有调度树，就无法正确分解 task 成多个独立的 module 函数

2. **ScaleHLS 的失败教训**：
   - ScaleHLS 使用的 Polygeist 版本较旧，没有 Polymer
   - Affine Dialect 只有少部分多面体能力，无法获取调度树
   - **结果**：生成的代码只有一个函数，在 Vitis 中可能被合并，无法预估硬件结构

3. **mlir-systolic 的目标**：
   - 利用 Polymer 获取完整的调度树
   - 基于调度树正确分解 task 成多个独立函数
   - 确保每个硬件模块（IO/PE/Drain）都生成独立的函数

### 调度树与 Task 分解的关系

**AutoSA 的工作流程**：
```
ISL Schedule Tree
    ↓
根据调度树节点确定模块边界
    ↓
生成多个独立的函数：
- A_IO_L3_in()
- A_IO_L2_in()
- PE_0_0()
- PE_0_1()
- ...
- C_drain_IO_L2_out()
```

**ScaleHLS 的问题**：
```
Affine IR (没有调度树)
    ↓
无法确定模块边界
    ↓
生成单个函数：
- kernel() { 所有逻辑都在这里 }
```

**mlir-systolic 的解决方案**：
```
Polymer → ISL Schedule Tree
    ↓
基于调度树节点分解
    ↓
SystolicDataflow Dialect (每个模块一个操作)
    ↓
降级到 HLS Dialect (每个模块一个 Task)
    ↓
生成多个独立函数
```

---

## Polymer 集成方案

### 1. 架构设计

```
Affine IR
    ↓
Polymer: extractScop() → IslScop
    ↓
Polymer: computeSchedule() → isl_schedule (调度树)
    ↓
基于调度树分析：
- 依赖距离
- 空间循环选择
- 模块边界确定
    ↓
SystolicTransform Pass (使用分析结果)
    ↓
SystolicDataflowGeneration Pass (基于调度树分解)
```

### 2. 关键接口

#### 2.1 提取 SCoP

```cpp
#include "polymer/Support/Scop.h"

// 从 MLIR Function 提取 SCoP
polymer::IslScop scop = polymer::extractScop(func);

// 获取 ISL 上下文和域
isl_ctx *ctx = scop.getIslCtx();
isl_union_set *domain = scop.getDomain();
isl_union_map *schedule = scop.getSchedule();
```

#### 2.2 计算调度树

```cpp
#include "polymer/Support/ScheduleTree.h"

// 计算调度树
isl_schedule *scheduleTree = polymer::computeSchedule(scop);

// 遍历调度树节点
// 每个节点对应一个硬件模块（L3 IO, L2 IO, PE, Drain）
```

#### 2.3 依赖距离分析

```cpp
// 使用 ISL 计算依赖距离
isl_union_map *deps = polymer::computeDependences(scop);
isl_union_map *depDistances = polymer::computeDependenceDistances(deps);

// 分析每个循环维度的依赖距离
// 依赖距离 ≤ 1 的维度可以作为空间循环
```

### 3. 实现步骤

#### Step 1: 集成 Polymer 库

**CMakeLists.txt** (已部分完成):
```cmake
# 当前状态：已添加 include 目录
include_directories(${POLYGEIST_BUILD_DIR}/../tools/polymer/include)

# 需要添加：链接 Polymer 库
target_link_libraries(SystolicAnalysis
  ${POLYGEIST_BUILD_DIR}/lib/libPolymerSupport.a
)
```

#### Step 2: 创建 Polymer 分析接口

**lib/Analysis/PolymerAnalysis.cpp** (新建):
```cpp
#include "polymer/Support/Scop.h"
#include "polymer/Support/ScheduleTree.h"

namespace mlir {
namespace systolic {

/// 使用 Polymer 提取 SCoP
polymer::IslScop extractScopWithPolymer(func::FuncOp func);

/// 计算调度树
isl_schedule *computeScheduleTree(polymer::IslScop &scop);

/// 分析依赖距离
SmallVector<DependenceDistance> analyzeDependenceDistances(
    polymer::IslScop &scop);

/// 基于调度树确定模块边界
SmallVector<ModuleBoundary> determineModuleBoundaries(
    isl_schedule *scheduleTree);

} // namespace systolic
} // namespace mlir
```

#### Step 3: 在 SystolicTransform Pass 中使用

**lib/Transforms/SystolicTransform.cpp**:
```cpp
void SystolicTransformPass::runOnOperation() {
  func::FuncOp func = getOperation();
  
  // 使用 Polymer 提取 SCoP
  polymer::IslScop scop = polymer::extractScop(func);
  
  // 计算调度树
  isl_schedule *scheduleTree = polymer::computeSchedule(scop);
  
  // 分析依赖距离（使用 ISL）
  auto depDistances = analyzeDependenceDistances(scop);
  
  // 自动选择空间循环（依赖距离 ≤ 1）
  auto spaceLoops = selectSpaceLoops(depDistances);
  
  // 基于调度树进行变换
  // ...
}
```

#### Step 4: 在 SystolicDataflowGeneration Pass 中使用

**lib/Transforms/SystolicDataflowGeneration.cpp**:
```cpp
void SystolicDataflowGenerationPass::runOnOperation() {
  func::FuncOp func = getOperation();
  
  // 获取调度树
  polymer::IslScop scop = polymer::extractScop(func);
  isl_schedule *scheduleTree = polymer::computeSchedule(scop);
  
  // 基于调度树确定模块边界
  auto boundaries = determineModuleBoundaries(scheduleTree);
  
  // 为每个模块边界生成独立的 SystolicDataflow 操作
  for (auto &boundary : boundaries) {
    // 生成 IOModuleOp, PEArrayOp, DrainModuleOp
    // 每个操作对应调度树的一个节点
  }
}
```

#### Step 5: 在降级 Pass 中生成独立函数

**lib/Transforms/SystolicDataflowToHLS.cpp**:
```cpp
// 将每个 SystolicDataflow 操作降级为独立的 HLS Task
// 每个 Task 最终生成一个独立的 C++ 函数

void lowerIOModule(IOModuleOp op) {
  // 生成独立的函数
  func::FuncOp ioFunc = createFunction(op.getName());
  // 而不是内联到主函数中
}
```

---

## 与 AutoSA 的对应

| AutoSA | MLIR-Systolic (使用 Polymer) |
|--------|----------------------------|
| `isl_schedule` | `isl_schedule *` (通过 Polymer) |
| `isl_union_map *deps` | `polymer::computeDependences()` |
| `get_dep_dis_at_node()` | `analyzeDependenceDistances()` |
| `sa_space_time_transform()` | 基于调度树和依赖距离 |
| `sa_io_construct_optimize()` | 基于调度树节点确定 IO 模块 |
| `generate_hw_modules()` | 基于调度树节点生成独立函数 |

---

## 当前状态

### ✅ 已完成

- [x] CMakeLists.txt 中添加 Polymer include 目录
- [x] 代码框架中预留了 Polymer 集成接口
- [x] 明确了 Polymer 的必要性和集成方案

### ⚠️ 待实现

- [ ] 链接 Polymer 库
- [ ] 实现 `PolymerAnalysis.cpp`
- [ ] 在 `SystolicTransform` Pass 中集成
- [ ] 在 `SystolicDataflowGeneration` Pass 中集成
- [ ] 基于调度树分解 task
- [ ] 测试验证

---

## 测试计划

### 测试用例

1. **MatMul 64×64**：
   - 验证调度树提取
   - 验证依赖距离分析
   - 验证空间循环选择
   - 验证模块分解（L3 IO, L2 IO, PE, Drain）

2. **与 AutoSA 对比**：
   - 生成的函数数量应该匹配
   - 每个函数的功能应该对应

### 验证指标

- ✅ 调度树成功提取
- ✅ 依赖距离分析正确
- ✅ 空间循环自动选择正确
- ✅ 生成多个独立函数（不是单个函数）
- ✅ 函数数量与 AutoSA 匹配

---

## 参考资源

- [Polymer 文档](https://github.com/llvm/Polygeist/tree/main/tools/polymer)
- [ISL 文档](https://libisl.sourceforge.io/)
- AutoSA 源代码：`autosa_utils.cpp` (依赖距离分析)
- ScaleHLS 文档：`scalehls/docs/MLIR_AutoSA_Implementation.md`

---

## 总结

**Polymer 集成是 mlir-systolic 项目的核心工作**，没有 Polymer：
- ❌ 无法获取调度树
- ❌ 无法正确分解 task
- ❌ 无法生成多个独立函数
- ❌ 无法预估硬件结构

**有了 Polymer**：
- ✅ 可以获取完整的调度树
- ✅ 可以正确分解 task
- ✅ 可以生成多个独立函数
- ✅ 可以预估硬件结构
- ✅ 实现完整的 AutoSA 功能

**下一步**：优先实现 Polymer 集成，这是所有后续工作的基础。



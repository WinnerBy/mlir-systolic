# 分支多面体能力检查报告

> **检查日期**: 2025-12-10  
> **分支**: 当前分支（声称使用 Polymer 的版本）  
> **结论**: ⚠️ **已配置但未实现** - 有 Polymer 配置，但代码中未实际使用

---

## 检查结果总结

### ✅ 代码声明 vs ❌ 实际实现

| 声明位置 | 声明内容 | 实际状态 |
|---------|---------|---------|
| `README.md` | "调用 Polymer/ISL 计算依赖距离" | ❌ 未实现 |
| `README.md` | "Polymer 的多面体分析能力" | ❌ 未使用 |
| `SystolicTransform.cpp:148` | "This is a simplified version - full implementation would use Polymer/ISL" | ⚠️ 仍然是简化版本 |

---

## 详细检查

### 1. 代码实现检查

#### SystolicTransform.cpp

**依赖距离分析** (`analyzeDependenceDistances`, line 149-231):
```cpp
/// This is a simplified version - full implementation would use Polymer/ISL.
static LogicalResult analyzeDependenceDistances(...) {
  // 使用简化的启发式方法：
  // - 检查循环变量是否出现在 load/store 中
  // - 固定假设依赖距离为 0
  // ❌ 没有调用 Polymer/ISL
}
```

**空间循环选择** (`selectSpaceLoops`, line 242-319):
```cpp
// 基于用户指定的 spaceTimeMode 参数
// ❌ 没有基于依赖距离自动选择
// ❌ 没有使用调度树
```

#### SpaceTimeAnalysis.cpp

**依赖距离计算** (`computeDependenceDistances`, line 163-244):
```cpp
// (Simplified - full version would use ISL dependence analysis)
static LogicalResult computeDependenceDistances(...) {
  // 简化的启发式方法
  // ❌ 没有使用 ISL 计算依赖距离
}
```

### 2. 构建配置检查

#### 根目录 CMakeLists.txt

**✅ 有 Polymer 配置**:
```cmake
option(SYSTOLIC_ENABLE_POLYMER "Enable Polymer integration for polyhedral analysis" ON)

if(SYSTOLIC_ENABLE_POLYMER)
  # 添加 Polymer include 目录
  include_directories(${POLYGEIST_BUILD_DIR}/../tools/polymer/include)
  include_directories(${POLYGEIST_BUILD_DIR}/tools/polymer/include)
endif()
```

**⚠️ 但只添加了 include 目录，没有链接库**

#### lib/Analysis/CMakeLists.txt

```cmake
LINK_LIBS PUBLIC
  MLIRAffineAnalysis
  MLIRAffineDialect
  MLIRAffineUtils
  MLIRMemRefDialect
  MLIRIR
  MLIRSupport
  # ❌ 没有 PolymerSupport
  # ❌ 没有 ISL
```

#### lib/Transforms/CMakeLists.txt

```cmake
LINK_LIBS PUBLIC
  SystolicAnalysis
  SystolicHLSDialect
  MLIRAffineDialect
  # ... 其他 MLIR 库
  # ❌ 没有 PolymerSupport
  # ❌ 没有 ISL
```

### 3. 头文件包含检查

**头文件中的声明**（`SpaceTimeAnalysis.h`）:
```cpp
// This module provides polyhedral analysis for systolic array generation,
// using Polymer/ISL for dependence analysis and space loop selection.

/// 2. Computes dependence distances using Polymer/ISL
```

**⚠️ 但实际实现中没有包含 Polymer 头文件**

**搜索结果**：
- ❌ 没有找到 `#include "polymer/..."`
- ❌ 没有找到 `#include "isl/..."`
- ❌ 没有找到 `using namespace polymer`
- ❌ 没有找到 `using namespace isl`

**实际包含的头文件**：
```cpp
// 只有 MLIR 标准库
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/Analysis/AffineAnalysis.h"
// ... 没有 Polymer/ISL
```

### 4. 函数调用检查

**搜索结果**：
- ❌ 没有找到 `polymer::` 函数调用
- ❌ 没有找到 `isl_` 函数调用
- ❌ 没有找到 `extractScop`
- ❌ 没有找到 `computeSchedule`
- ❌ 没有找到调度树相关操作

---

## 当前实现方法

### 依赖距离分析

**当前方法**（简化启发式）：
1. 收集所有 `AffineLoadOp` 和 `AffineStoreOp`
2. 检查循环变量是否出现在 load/store 的 map operands 中
3. 如果同一个 memref 的 store 和 load 都使用同一个 IV → 假设依赖距离为 0
4. 固定假设所有依赖距离 ≤ 1，因此所有循环都可以是空间循环

**问题**：
- ❌ 无法处理复杂的依赖关系（如 `A[i+1] = A[i]` 的距离为 1）
- ❌ 无法处理非均匀依赖
- ❌ 无法精确计算依赖距离向量

### 空间循环选择

**当前方法**：
- 基于用户指定的 `spaceTimeMode` 参数（0-5）
- 不基于依赖距离自动选择
- 不验证选择的循环是否真的适合作为空间循环

**问题**：
- ❌ 需要用户手动指定，无法自动分析
- ❌ 可能选择不合适的循环（依赖距离 > 1）

---

## 与 README 声明的对比

### README 声明

```
Pass 1: SystolicTransform
  • 依赖分析 (Polymer/ISL)                          ❌ 未实现
  • 空间循环选择 (依赖距离 ≤ 1)                     ⚠️ 部分实现（基于用户参数）
  • 循环置换 (空间循环外移)                         ✅ 已实现
  • 多级分块 (array_part + latency)                 ✅ 已实现
```

### 实际状态

| 功能 | README 声明 | 实际状态 | 差距 |
|------|------------|---------|------|
| 依赖分析 | Polymer/ISL | 简化启发式 | ❌ 未使用多面体 |
| 空间循环选择 | 自动（基于依赖距离） | 手动（用户指定） | ❌ 无法自动选择 |
| 循环置换 | ✅ | ✅ | ✅ 已实现 |
| 多级分块 | ✅ | ✅ | ✅ 已实现 |

---

## 结论

### ⚠️ 当前分支：已配置但未实现

**状态总结**：
1. ✅ **CMakeLists.txt 中有 Polymer 配置**（添加了 include 目录）
2. ✅ **头文件中有注释说明会使用 Polymer/ISL**
3. ❌ **但实际实现代码中没有使用 Polymer/ISL**
4. ❌ **没有链接 Polymer 库**
5. ❌ **所有分析都是简化的启发式方法**

**这是一个"准备使用但尚未实现"的状态**。

### 📋 配置 vs 实现

| 项目 | 状态 | 说明 |
|------|------|------|
| CMake 配置 | ✅ 已配置 | 添加了 Polymer include 目录 |
| 头文件注释 | ✅ 已声明 | 说明会使用 Polymer/ISL |
| 实际代码 | ❌ 未实现 | 仍然是简化版本 |
| 库链接 | ❌ 未链接 | 没有链接 PolymerSupport |
| 函数调用 | ❌ 无 | 没有调用任何 Polymer 函数 |

### ⚠️ README 与实际不符

README 中声明使用 Polymer/ISL，但实际代码未实现。这可能是：
- 计划中的功能（但尚未实现）
- 文档与代码不同步
- 配置已就绪，等待实现

### 📋 需要完成的工作

要真正使用 Polymer，需要：

1. **链接 Polymer 库**
   ```cmake
   target_link_libraries(SystolicAnalysis
     ${POLYGEIST_BUILD_DIR}/lib/libPolymerSupport.a
   )
   ```

2. **实现真正的多面体分析**
   ```cpp
   #include "polymer/Support/Scop.h"
   #include "polymer/Support/ScheduleTree.h"
   
   polymer::IslScop scop = polymer::extractScop(func);
   isl_schedule *scheduleTree = polymer::computeSchedule(scop);
   // 使用调度树进行依赖分析
   ```

3. **基于调度树选择空间循环**
   - 使用 ISL 计算依赖距离
   - 基于依赖距离自动选择空间循环
   - 验证选择的循环是否合法

---

## 建议

1. **更新 README**：明确标注哪些功能已实现，哪些是计划中的
2. **添加 TODO 注释**：在代码中标记需要 Polymer 集成的部分
3. **创建集成计划**：制定详细的 Polymer 集成路线图
4. **分阶段实现**：先实现基础的 Polymer 调用，再逐步完善

---

**总结**：当前分支**未使用 Polymer/ISL 多面体能力**，所有分析都是简化版本。README 中的声明与实际实现不符。


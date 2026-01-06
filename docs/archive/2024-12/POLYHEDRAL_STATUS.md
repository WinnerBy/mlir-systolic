# 多面体能力使用状态

> **最后更新**: 2025-12-10  
> **状态**: ⚠️ **尚未使用多面体能力**

---

## 当前状态：❌ 未使用多面体能力

### 检查结果

经过代码检查，**mlir-systolic 目前尚未真正使用 Polymer/ISL 多面体能力**。

#### 1. 代码中的证据

**所有分析都是简化版本**：

```cpp
// lib/Transforms/SystolicTransform.cpp:148
/// This is a simplified version - full implementation would use Polymer/ISL.

// lib/Analysis/SpaceTimeAnalysis.cpp:91
// (Simplified version - full implementation would use Polymer)

// lib/Analysis/SpaceTimeAnalysis.cpp:149
// (Simplified - full version would use ISL dependence analysis)

// lib/Transforms/SystolicDataflowGeneration.cpp:184
// TODO: More sophisticated analysis using Polymer/ISL
```

#### 2. 当前实现方法

**依赖距离分析** (`SystolicTransform.cpp`):
- ❌ 没有使用 ISL 计算依赖距离
- ✅ 使用简化的启发式方法：检查循环变量是否出现在 load/store 中
- ✅ 固定假设依赖距离为 0（对于 MatMul 模式）

**空间循环选择** (`SystolicTransform.cpp`):
- ❌ 没有使用 ISL 调度树
- ✅ 基于用户指定的 `spaceTimeMode` 参数
- ✅ 没有自动分析依赖距离来选择空间循环

**IO 层级分析** (`SystolicDataflowGeneration.cpp`):
- ❌ 没有使用调度树确定模块边界
- ✅ 使用简单的启发式：固定为 L2
- ✅ 基于访问模式（只读/只写）分类

#### 3. 构建配置

**CMakeLists.txt**:
```cmake
# 当前状态：只添加了 include 目录
include_directories(${POLYGEIST_BUILD_DIR}/../tools/polymer/include)

# ❌ 没有链接 Polymer 库
# ❌ 没有链接 ISL 库
```

**lib/Analysis/CMakeLists.txt**:
```cmake
# 当前链接的库（没有 Polymer）
LINK_LIBS PUBLIC
  MLIRAffineAnalysis
  MLIRAffineDialect
  MLIRAffineUtils
  # ❌ 没有 PolymerSupport
  # ❌ 没有 ISL
```

#### 4. 代码搜索

**搜索结果**：
- ❌ 没有找到 `polymer::` 函数调用
- ❌ 没有找到 `isl_` 函数调用
- ❌ 没有找到 `#include "polymer/..."`
- ❌ 没有找到 `#include "isl/..."`

---

## 这意味着什么

### 当前能力

✅ **可以工作的功能**：
- 基础的循环变换（分块、置换）
- 基于用户参数的配置（spaceTimeMode, arrayPart, latency）
- 基础的数组引用分析（启发式方法）
- SystolicDataflow Dialect 框架

❌ **无法工作的功能**：
- **自动依赖距离分析**（需要 ISL）
- **自动空间循环选择**（需要依赖距离分析）
- **基于调度树的 task 分解**（需要调度树）
- **精确的 IO 层级判断**（需要调度树信息）

### 与 AutoSA 的差距

| 功能 | AutoSA | mlir-systolic (当前) | 需要 |
|------|--------|---------------------|------|
| 依赖距离分析 | ✅ ISL | ❌ 简化启发式 | Polymer/ISL |
| 空间循环选择 | ✅ 自动（基于依赖距离） | ⚠️ 手动（用户指定） | Polymer/ISL |
| 调度树 | ✅ ISL Schedule Tree | ❌ 无 | Polymer |
| Task 分解 | ✅ 基于调度树 | ❌ 无法分解 | Polymer |
| 多函数生成 | ✅ 多个独立函数 | ❌ 可能单个函数 | Polymer |

---

## 为什么需要 Polymer

### 核心问题

**没有 Polymer，就无法解决 ScaleHLS 的核心问题**：

1. **无法获取调度树** → 无法确定模块边界 → 无法分解 task
2. **无法精确分析依赖** → 无法自动选择空间循环
3. **无法预估硬件结构** → 生成的代码可能被 Vitis 合并

### 具体影响

**当前状态下的问题**：

```cpp
// 当前可能生成的代码（问题）
void kernel() {
  // 所有逻辑在一个函数内
  // 无法确定哪些部分对应哪些硬件模块
  // Vitis 可能进行不可控的优化
}

// 需要生成的代码（目标）
void A_IO_L3_in() { ... }    // 独立函数
void A_IO_L2_in() { ... }    // 独立函数
void PE_0_0() { ... }       // 独立函数
void kernel() {
  A_IO_L3_in();
  A_IO_L2_in();
  PE_0_0();
}
```

**没有调度树，就无法知道在哪里切分函数**。

---

## 下一步：集成 Polymer

### 必须完成的工作

1. **链接 Polymer 库**
   ```cmake
   target_link_libraries(SystolicAnalysis
     ${POLYGEIST_BUILD_DIR}/lib/libPolymerSupport.a
   )
   ```

2. **实现真正的多面体分析**
   ```cpp
   // 替换简化版本
   polymer::IslScop scop = polymer::extractScop(func);
   isl_schedule *scheduleTree = polymer::computeSchedule(scop);
   // 使用调度树进行分析
   ```

3. **基于调度树分解 task**
   ```cpp
   // 基于调度树节点确定模块边界
   // 每个节点生成一个独立的 SystolicDataflow 操作
   ```

### 优先级

**⭐⭐⭐ 最高优先级** - 这是项目的核心目标，必须完成。

---

## 总结

| 项目 | 状态 |
|------|------|
| **是否使用多面体能力** | ❌ **否** |
| **当前实现** | 简化的启发式方法 |
| **Polymer 集成** | ⚠️ 框架已准备，但未实现 |
| **调度树获取** | ❌ 无法获取 |
| **Task 分解** | ❌ 无法分解 |
| **下一步** | ⭐⭐⭐ **集成 Polymer（最高优先级）** |

**结论**：mlir-systolic 目前**尚未使用多面体能力**，所有分析都是简化版本。**集成 Polymer 是最高优先级的工作**，这是实现完整 AutoSA 功能的关键。



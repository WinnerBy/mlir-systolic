# Spacetime 动态枚举使用指南

> **最后更新**: 2026-01-06  
> **状态**: ✅ 核心功能已实现并可用

---

## 功能概述

mlir-systolic 现在支持动态枚举所有可能的 spacetime 配置，类似于 AutoSA 的行为。系统会根据循环数量自动生成所有可能的配置，并允许用户选择或自动选择。

---

## 使用方式

### 1. 自动选择（默认）

默认情况下，系统会自动选择配置：
- 优先选择 ST3（如果存在）
- 否则选择第一个配置

```bash
systolic-opt input.mlir --systolic-transform
```

### 2. 列出所有配置

要查看所有可用的 spacetime 配置，可以在代码中设置 `listConfigs = true`，或者通过 `SystolicConfig` 传递：

```cpp
SystolicConfig config;
// ... 设置其他选项 ...
auto pass = createSpaceTimeTransformPass(config);
```

在 Pass 内部，如果 `listConfigs` 为 true，会输出类似以下的信息：

```
[Systolic] Found 6 spacetime configurations:
  [0] ST0: space=[0], time=[1,2]
  [1] ST1: space=[1], time=[0,2]
  [2] ST2: space=[2], time=[0,1]
  [3] ST3: space=[0,1], time=[2]
  [4] ST4: space=[0,2], time=[1]
  [5] ST5: space=[1,2], time=[0]
```

### 3. 选择特定配置

通过 `SystolicConfig` 或直接修改 `SystolicTransformOptions` 来指定配置索引：

```cpp
SystolicConfig config;
// 选择配置索引 3 (ST3)
// 注意：需要通过 SystolicTransformOptions 来设置
```

或者修改 Pass 代码中的 `options.spaceTimeMode`：

```cpp
options.spaceTimeMode = 3;  // 选择索引 3 的配置
```

---

## 配置选项

### SystolicTransformOptions

```cpp
struct SystolicTransformOptions {
  /// Space-time mapping mode (index into enumerated configurations)
  /// -1: 自动选择（默认）
  /// >= 0: 选择对应索引的配置
  int spaceTimeMode = -1;
  
  /// Maximum systolic array dimension (1, 2, or 3)
  /// 控制要探索的 PE 阵列维度
  unsigned maxSADim = 2;  // 默认 2D
  
  /// List all spacetime configurations instead of generating code
  bool listConfigs = false;
  
  // ... 其他选项 ...
};
```

---

## 不同循环数量的配置数量

### 3 循环 (MM - Matrix Multiplication)

- **1D 阵列**: 3 种（ST0, ST1, ST2）
- **2D 阵列**: 3 种（ST3, ST4, ST5）
- **总计**: 6 种配置

### 4 循环 (MTTKRP, TTMC)

- **1D 阵列**: 4 种
- **2D 阵列**: 6 种
- **3D 阵列**: 4 种（如果 maxSADim >= 3）
- **总计**: 10-14 种配置

### 5 循环 (CNN)

- **1D 阵列**: 5 种
- **2D 阵列**: 10 种
- **3D 阵列**: 10 种（如果 maxSADim >= 3）
- **总计**: 25 种配置

---

## 实现细节

### 枚举算法

系统使用组合数学来枚举所有可能的配置：

```cpp
// 1D: C(N,1) = N 种
// 2D: C(N,2) = N*(N-1)/2 种
// 3D: C(N,3) = N*(N-1)*(N-2)/6 种
```

### Space Loop 候选选择

只有满足以下条件的循环才能作为 space loop：
- 依赖距离 ≤ 1
- 通过 Polymer 依赖分析验证

### 循环置换

选择配置后，系统会自动执行循环置换，确保：
- Space loops 位于最外层
- Time loops 位于内层

这类似于 AutoSA 的 `loop_interchange_at_node` 功能。

---

## 示例

### 示例 1: 3 循环 MM

输入循环：`for i, for j, for k`

枚举结果：
- [0] ST0: space=[i], time=[j,k]
- [1] ST1: space=[j], time=[i,k]
- [2] ST2: space=[k], time=[i,j]
- [3] ST3: space=[i,j], time=[k] ← 默认选择
- [4] ST4: space=[i,k], time=[j]
- [5] ST5: space=[j,k], time=[i]

### 示例 2: 4 循环 MTTKRP

输入循环：`for i, for j, for k, for l`

枚举结果（maxSADim=2）：
- 1D: [0-3] 4 种配置
- 2D: [4-9] 6 种配置
- 总计: 10 种配置

---

## 与 AutoSA 的对应关系

### 3 循环情况

| mlir-systolic 索引 | AutoSA space_time_id | 配置 |
|-------------------|---------------------|------|
| 0 | 0 | ST0: space=[0] |
| 1 | 1 | ST1: space=[1] |
| 2 | 2 | ST2: space=[2] |
| 3 | 3 | ST3: space=[0,1] |
| 4 | 4 | ST4: space=[0,2] |
| 5 | 5 | ST5: space=[1,2] |

**注意**: 对于 4+ 循环，索引对应关系可能不同，因为 AutoSA 也会动态枚举。

---

## 调试信息

启用调试输出（`-debug` 或 `LLVM_DEBUG`）可以查看：

1. **枚举过程**:
```
[Systolic] Enumerating spacetime configs:
  numLoops: 3
  maxSADim: 2
  Exploring 1D arrays...
    [0] 1D: space=[0], time=[1,2]
    ...
  Exploring 2D arrays...
    [3] 2D: space=[0,1], time=[2]
    ...
```

2. **选择结果**:
```
[Systolic] Selected spacetime config [3]: ST3
[Systolic] Selected configuration:
  Space loops: 0 1
  Time loops: 2
```

3. **循环置换**:
```
[Systolic] Permuting loops: 0 1 2
[Systolic] Loop permutation successful
```

---

## 未来改进

1. **命令行选项**: 添加 `--spacetime-mode`, `--max-sa-dim`, `--list-configs` 选项
2. **启发式选择**: 实现更智能的自动选择算法
3. **配置验证**: 添加配置合法性检查
4. **性能分析**: 为每个配置生成性能估计

---

## 参考

- [SPACETIME_INDEX.md](SPACETIME_INDEX.md) - 文档索引
- [SPACETIME_COMPARISON.md](SPACETIME_COMPARISON.md) - 与 AutoSA 的详细对比
- [features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md](features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md) - 实现计划
- `SPACETIME_IMPLEMENTATION_STATUS.md` - 实现状态

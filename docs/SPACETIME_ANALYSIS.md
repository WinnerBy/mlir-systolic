# Spacetime 映射分析

> **最后更新**: 2026-01-06  
> **目的**: 分析 AutoSA 的 spacetime 映射逻辑，理解不同循环数量下的 spacetime 划分

---

## 核心理解

### Spacetime 的动态性

**重要发现**: AutoSA 的 spacetime 映射是**动态的**，不是固定的 ST0-ST5 对应关系。

#### 对于 3 循环 kernel (MM)

对于矩阵乘法（3 个循环：i, j, k），AutoSA 生成 **6 种** spacetime 配置：

- **ST0**: space=[i], time=[j,k] - 1D row array
- **ST1**: space=[j], time=[i,k] - 1D column array  
- **ST2**: space=[k], time=[i,j] - 1D reduction array
- **ST3**: space=[i,j], time=[k] - 2D output-stationary (最常用)
- **ST4**: space=[i,k], time=[j] - 2D weight-stationary
- **ST5**: space=[j,k], time=[i] - 2D activation-stationary

#### 对于 4 循环 kernel (MTTKRP, TTMC)

对于 4 循环 kernel（例如 i, j, k, l），AutoSA 会生成**更多** spacetime 配置：

- 1D 阵列：C(4,1) = 4 种（选择 1 个作为 space）
- 2D 阵列：C(4,2) = 6 种（选择 2 个作为 space）
- 3D 阵列：C(4,3) = 4 种（选择 3 个作为 space）

**总计**: 最多 14 种 spacetime 配置

从 AutoSA 文档示例：
- **TTMC**: 使用 `space_time[4]`，说明至少有 5 种配置（索引 0-4）
- **MTTKRP**: 使用 `space_time[3]`，说明至少有 4 种配置

#### 对于 5+ 循环 kernel (CNN)

对于 5 循环 kernel，spacetime 配置数量会更多：
- 1D: C(5,1) = 5
- 2D: C(5,2) = 10
- 3D: C(5,3) = 10
- 4D: C(5,4) = 5

**总计**: 最多 30 种配置

---

## 当前实现状态

### ParametricSpaceTime 框架

我们的 `ParametricSpaceTime` 框架**已经支持**动态配置：

```cpp
class ParametricSpaceTime {
  SmallVector<SpaceDimConfig> spaceDimConfigs;  // 可配置多个空间维度
  TimeDimConfig timeDimConfig;                  // 可配置多个时间维度
  ReductionDimConfig reductionDimConfig;       // Reduction 维度
};
```

**关键点**:
- ✅ 框架本身支持任意数量的空间/时间维度
- ✅ `createFromMode()` 目前只预定义了 MM 的 ST0-ST5
- 🟡 需要扩展以支持 4+ 循环 kernel 的 spacetime 配置

---

## 需要解决的问题

### 1. Spacetime 索引的动态生成

**问题**: 不同循环数量的 kernel，spacetime 索引含义不同

**解决方案**:
1. **运行时枚举**: 在 `SystolicTransform` 中，根据循环数量动态枚举所有可能的 spacetime 配置
2. **配置映射**: 建立循环数量 → spacetime 配置列表的映射
3. **用户选择**: 提供类似 AutoSA 的交互式选择机制

### 2. 测试覆盖

**当前状态**:
- ✅ MM (3 循环): ST0-ST5 已实现框架支持
- 🟡 MTTKRP (4 循环): 框架支持，但未测试
- 🟡 TTMC (4 循环): 框架支持，但未测试
- 🟡 CNN (5 循环): 框架支持，但未测试

**需要**:
- 为每个 kernel 类型枚举所有可能的 spacetime 配置
- 测试每个配置的代码生成
- 与 AutoSA 输出对比验证

---

## 实现建议

### 阶段 1: 动态 Spacetime 枚举

```cpp
// 在 SystolicTransform 中
LogicalResult enumerateSpaceTimeConfigs(
    const SmallVector<AffineForOp> &loops,
    SmallVector<ParametricSpaceTime> &configs) {
  
  unsigned numLoops = loops.size();
  
  // 枚举所有可能的 space 维度组合
  for (unsigned numSpaceDims = 1; numSpaceDims <= std::min(2u, numLoops); ++numSpaceDims) {
    // 生成 C(numLoops, numSpaceDims) 种组合
    // 每种组合创建一个 ParametricSpaceTime 配置
  }
  
  return success();
}
```

### 阶段 2: 交互式选择（可选）

类似 AutoSA 的 manual 模式：
1. 枚举所有 spacetime 配置
2. 输出配置列表和描述
3. 用户选择（通过命令行参数或交互式提示）
4. 继续后续优化步骤

### 阶段 3: 测试脚本

为每个 kernel 类型生成测试矩阵：
- 所有可能的 spacetime 配置
- 不同的 array_part 组合
- 不同的 latency 组合
- 不同的 simd 组合

---

## 与 AutoSA 的对比

| 特性 | AutoSA | mlir-systolic (当前) | mlir-systolic (目标) |
|------|--------|---------------------|---------------------|
| **3 循环 spacetime** | ST0-ST5 (6种) | ✅ ST0-ST5 框架支持 | ✅ 完整支持 |
| **4 循环 spacetime** | 动态生成 (10+种) | 🟡 框架支持但未枚举 | 🟡 需要实现枚举 |
| **5+ 循环 spacetime** | 动态生成 (20+种) | 🟡 框架支持但未枚举 | 🟡 需要实现枚举 |
| **交互式选择** | ✅ Manual 模式 | ❌ 无 | 🟡 可选实现 |
| **自动选择** | ✅ Auto 模式 | 🟡 默认 ST3 | 🟡 需要实现启发式 |

---

## 下一步工作

1. **扩展 spacetime 枚举**: 实现动态生成所有可能的 spacetime 配置
2. **测试脚本**: 为每个 kernel 类型生成完整的测试矩阵
3. **交互模式** (可选): 实现类似 AutoSA 的 manual 模式
4. **验证**: 与 AutoSA 输出对比，确保配置对应关系正确

---

**参考**: 
- AutoSA 文档: `third_party/AutoSA/docs/examples/`
- AutoSA 源码: `third_party/AutoSA/src/autosa_trans/` (spacetime 变换逻辑)

# 交互模式分析

> **最后更新**: 2026-01-06  
> **目的**: 分析是否需要实现类似 AutoSA 的交互式编译模式

---

## AutoSA 的交互模式

### Manual 模式特点

从 `getting_started.rst` 可以看到，AutoSA 的 manual 模式提供：

1. **逐步提示**: 每个优化步骤完成后，输出下一步的候选选项
2. **tuning.json**: 生成包含候选选项的 JSON 文件
3. **Verbose 输出**: `--AutoSA-verbose` 输出详细的调度树信息
4. **用户选择**: 用户通过 `--sa-sizes` 参数指定选择

### 示例流程

```bash
# Step 1: Space-time transformation
./autosa kernel.c --config=config.json --target=autosa_hls_c
# 输出: [AutoSA] 6 systolic arrays generated.
# 生成 tuning.json: {"space_time": {"n_kernel": 6}}
# 用户选择: --sa-sizes="{kernel[]->space_time[3]}"

# Step 2: Array partitioning
./autosa kernel.c ... --sa-sizes="{kernel[]->space_time[3]}"
# 输出: [AutoSA] Apply array partitioning.
# 生成 tuning.json: {"array_part": {"tilable_loops": [64, 64, 64]}}
# 用户选择: --sa-sizes="{...;kernel[]->array_part[16,16,16]}"

# Step 3: Latency hiding
# 继续类似流程...
```

---

## 我们项目的需求分析

### 是否需要交互模式？

#### 支持交互模式的理由 ✅

1. **不同 kernel 的复杂性**
   - 不同 kernel（MM, MTTKRP, CNN）有不同的 spacetime 选项
   - 不同 spacetime 对后续优化步骤有不同要求
   - 用户需要了解可选项才能做出合理选择

2. **参数调优的复杂性**
   - array_part、latency、simd 的组合空间很大
   - 不同组合对性能影响不同
   - 需要逐步探索和验证

3. **调试和开发**
   - 开发阶段需要了解每个步骤的选项
   - 便于理解编译流程
   - 便于定位问题

#### 不支持交互模式的理由 ❌

1. **自动化目标**
   - 我们的目标是自动化生成，而不是手动调优
   - 交互模式增加了使用复杂度

2. **实现成本**
   - 需要实现候选选项枚举
   - 需要实现提示信息生成
   - 需要维护状态和上下文

3. **使用场景**
   - 如果主要用于测试和验证，可以预先定义配置
   - 如果用于生产，应该使用自动选择或配置文件

---

## 建议方案

### 方案 1: 简化版交互模式（推荐）✅

**实现内容**:
1. **枚举模式**: `--list-spacetime-configs` 列出所有可能的 spacetime 配置
2. **详细信息**: `--verbose` 输出每个步骤的详细信息
3. **配置文件**: 支持从配置文件读取完整参数（避免逐步输入）

**优点**:
- 实现简单
- 满足调试需求
- 不强制交互

**示例**:
```bash
# 列出所有 spacetime 配置
systolic-opt input.mlir --list-spacetime-configs

# 输出:
# Available spacetime configurations for 3-loop kernel:
#   ST0: space=[0], time=[1,2] - 1D row array
#   ST1: space=[1], time=[0,2] - 1D column array
#   ST2: space=[2], time=[0,1] - 1D reduction array
#   ST3: space=[0,1], time=[2] - 2D output-stationary
#   ST4: space=[0,2], time=[1] - 2D weight-stationary
#   ST5: space=[1,2], time=[0] - 2D activation-stationary

# 使用配置文件
systolic-opt input.mlir --config=config.json

# config.json:
# {
#   "spacetime": 3,
#   "array_part": [16, 16, 16],
#   "latency": [8, 8],
#   "simd": [2]
# }
```

### 方案 2: 完整交互模式（可选）🟡

**实现内容**:
1. 类似 AutoSA 的逐步提示
2. 生成 `tuning.json` 文件
3. 支持 `--interactive` 模式

**优点**:
- 完全兼容 AutoSA 的工作流
- 便于学习和调试

**缺点**:
- 实现复杂
- 使用繁琐

### 方案 3: 仅自动模式（不推荐）❌

**实现内容**:
- 只支持自动选择，不提供交互

**缺点**:
- 调试困难
- 无法探索参数空间
- 不符合开发需求

---

## 实现优先级

### 高优先级 ✅

1. **Spacetime 枚举**: 实现动态枚举所有可能的 spacetime 配置
2. **Verbose 输出**: 实现详细的调试信息输出
3. **配置文件支持**: 支持从 JSON/YAML 读取完整配置

### 中优先级 🟡

4. **候选选项输出**: 在 manual 模式下输出下一步的候选选项
5. **tuning.json 生成**: 生成包含候选选项的 JSON 文件

### 低优先级（可选）

6. **交互式提示**: 实现真正的交互式命令行界面
7. **自动调优**: 实现启发式自动选择最佳配置

---

## 具体实现建议

### 1. Spacetime 枚举命令

```cpp
// 在 systolic-opt 中添加
static cl::opt<bool> listSpacetimeConfigs(
    "list-spacetime-configs",
    cl::desc("List all available spacetime configurations"),
    cl::init(false));

// 实现
if (listSpacetimeConfigs) {
  enumerateSpaceTimeConfigs(loops, configs);
  printSpacetimeConfigs(configs);
  return;
}
```

### 2. Verbose 输出

```cpp
// 在 SystolicTransform 中
if (verbose) {
  llvm::outs() << "[Systolic] Available spacetime configurations:\n";
  for (auto &config : configs) {
    llvm::outs() << "  " << config.toString() << "\n";
  }
}
```

### 3. 配置文件支持

```json
{
  "spacetime": 3,
  "array_part": [16, 16, 16],
  "latency": [8, 8],
  "simd": [2],
  "kernel_type": "mm"
}
```

---

## 结论

**推荐**: 实现**方案 1（简化版交互模式）**

**理由**:
1. 满足调试和开发需求
2. 实现成本低
3. 不强制交互，保持自动化特性
4. 可以逐步扩展为完整交互模式

**不推荐**: 立即实现完整交互模式

**理由**:
1. 实现成本高
2. 使用场景有限
3. 可以先通过配置文件解决大部分需求

---

**下一步**: 
1. 实现 spacetime 枚举功能
2. 增强 verbose 输出
3. 添加配置文件支持
4. 根据实际需求决定是否实现完整交互模式

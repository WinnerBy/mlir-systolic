# Spacetime 动态枚举实现计划

> **最后更新**: 2026-01-06  
> **目的**: 详细说明如何实现类似 AutoSA 的动态 spacetime 枚举

---

## 实现目标

1. **动态枚举**: 根据循环数量自动生成所有可能的 spacetime 配置
2. **兼容性**: 保持与现有代码的兼容性（向后兼容预定义模式）
3. **扩展性**: 支持 1D、2D、3D 阵列（可配置）
4. **正确性**: 确保与 AutoSA 的配置对应关系一致

---

## 实现步骤

### 步骤 1: 扩展 ParametricSpaceTime 框架

#### 1.1 添加配置 ID

**文件**: `include/systolic/Analysis/ParametricSpaceTime.h`

```cpp
class ParametricSpaceTime {
private:
    unsigned configId = 0;  // 新增：配置 ID（对应 AutoSA 的 space_time_id）
    
public:
    /// Set configuration ID (for dynamic enumeration)
    void setConfigId(unsigned id) { configId = id; }
    
    /// Get configuration ID
    unsigned getConfigId() const { return configId; }
    
    /// Create from loop indices (for dynamic enumeration)
    static ParametricSpaceTime createFromLoopIndices(
        const SmallVector<unsigned> &spaceLoopIndices,
        const SmallVector<unsigned> &timeLoopIndices,
        const SmallVector<StringRef> &loopNames);
};
```

#### 1.2 实现 createFromLoopIndices

**文件**: `lib/Analysis/ParametricSpaceTime.cpp`

```cpp
ParametricSpaceTime ParametricSpaceTime::createFromLoopIndices(
    const SmallVector<unsigned> &spaceLoopIndices,
    const SmallVector<unsigned> &timeLoopIndices,
    const SmallVector<StringRef> &loopNames) {
    
    ParametricSpaceTime config;
    
    // 添加空间维度
    for (unsigned idx : spaceLoopIndices) {
        StringRef name = (idx < loopNames.size()) ? loopNames[idx] : "unknown";
        config.addSpaceDim(idx, name);
    }
    
    // 添加时间维度
    for (unsigned idx : timeLoopIndices) {
        StringRef name = (idx < loopNames.size()) ? loopNames[idx] : "unknown";
        config.addTimeDim(idx, name);
    }
    
    return config;
}
```

---

### 步骤 2: 实现动态枚举函数

#### 2.1 在 SystolicTransform 中添加枚举函数

**文件**: `lib/Transforms/SystolicTransform.cpp`

```cpp
/// Enumerate all possible spacetime configurations
/// Similar to AutoSA's sa_space_time_transform()
static LogicalResult enumerateSpaceTimeConfigs(
    const SmallVector<AffineForOp> &loops,
    const SmallVector<LoopDepInfo> &depInfos,
    unsigned maxSADim,  // Maximum systolic array dimension (default: 2)
    SmallVector<ParametricSpaceTime> &configs) {
    
    configs.clear();
    unsigned numLoops = loops.size();
    unsigned configId = 0;
    
    // Collect loop names
    SmallVector<StringRef> loopNames;
    for (auto loop : loops) {
        StringRef name = loop.getName();
        if (name.empty()) {
            // Generate default name
            name = StringRef("loop" + std::to_string(loopNames.size()));
        }
        loopNames.push_back(name);
    }
    
    // 1. Enumerate 1D arrays
    if (maxSADim >= 1 && numLoops >= 1) {
        for (unsigned i = 0; i < numLoops; ++i) {
            if (depInfos[i].canBeSpaceLoop) {
                SmallVector<unsigned> spaceLoops = {i};
                SmallVector<unsigned> timeLoops;
                for (unsigned j = 0; j < numLoops; ++j) {
                    if (j != i) {
                        timeLoops.push_back(j);
                    }
                }
                
                ParametricSpaceTime config = 
                    ParametricSpaceTime::createFromLoopIndices(
                        spaceLoops, timeLoops, loopNames);
                config.setConfigId(configId++);
                configs.push_back(config);
            }
        }
    }
    
    // 2. Enumerate 2D arrays
    if (maxSADim >= 2 && numLoops >= 2) {
        for (unsigned i = 0; i < numLoops; ++i) {
            if (depInfos[i].canBeSpaceLoop) {
                for (unsigned j = i + 1; j < numLoops; ++j) {
                    if (depInfos[j].canBeSpaceLoop) {
                        SmallVector<unsigned> spaceLoops = {i, j};
                        SmallVector<unsigned> timeLoops;
                        for (unsigned k = 0; k < numLoops; ++k) {
                            if (k != i && k != j) {
                                timeLoops.push_back(k);
                            }
                        }
                        
                        ParametricSpaceTime config = 
                            ParametricSpaceTime::createFromLoopIndices(
                                spaceLoops, timeLoops, loopNames);
                        config.setConfigId(configId++);
                        configs.push_back(config);
                    }
                }
            }
        }
    }
    
    // 3. Enumerate 3D arrays (optional)
    if (maxSADim >= 3 && numLoops >= 3) {
        for (unsigned i = 0; i < numLoops; ++i) {
            if (depInfos[i].canBeSpaceLoop) {
                for (unsigned j = i + 1; j < numLoops; ++j) {
                    if (depInfos[j].canBeSpaceLoop) {
                        for (unsigned k = j + 1; k < numLoops; ++k) {
                            if (depInfos[k].canBeSpaceLoop) {
                                SmallVector<unsigned> spaceLoops = {i, j, k};
                                SmallVector<unsigned> timeLoops;
                                for (unsigned l = 0; l < numLoops; ++l) {
                                    if (l != i && l != j && l != k) {
                                        timeLoops.push_back(l);
                                    }
                                }
                                
                                ParametricSpaceTime config = 
                                    ParametricSpaceTime::createFromLoopIndices(
                                        spaceLoops, timeLoops, loopNames);
                                config.setConfigId(configId++);
                                configs.push_back(config);
                            }
                        }
                    }
                }
            }
        }
    }
    
    LLVM_DEBUG({
        llvm::dbgs() << "[Systolic] Enumerated " << configs.size() 
                     << " spacetime configurations:\n";
        for (const auto &config : configs) {
            llvm::dbgs() << "  [" << config.getConfigId() << "] " 
                         << config.getSpaceTimeTypeString() << "\n";
        }
    });
    
    return success();
}
```

---

### 步骤 3: 修改主流程使用枚举结果

#### 3.1 修改 SystolicTransformPass::runOnOperation

**文件**: `lib/Transforms/SystolicTransform.cpp`

```cpp
void SystolicTransformPass::runOnOperation() {
    // ... 现有代码 ...
    
    // Step 2.4: Enumerate and select spacetime configuration
    SmallVector<ParametricSpaceTime> allConfigs;
    unsigned maxSADim = options.maxSADim.value_or(2);  // Default to 2D
    
    if (failed(enumerateSpaceTimeConfigs(loops, depInfos, 
                                          maxSADim, allConfigs))) {
        LLVM_DEBUG(llvm::dbgs() << "Failed to enumerate spacetime configs\n");
        return;
    }
    
    if (allConfigs.empty()) {
        LLVM_DEBUG(llvm::dbgs() << "No valid spacetime configurations found\n");
        return;
    }
    
    // Select configuration
    ParametricSpaceTime selectedConfig;
    if (options.spaceTimeMode >= 0 && 
        options.spaceTimeMode < allConfigs.size()) {
        // Use specified mode
        selectedConfig = allConfigs[options.spaceTimeMode];
        LLVM_DEBUG(llvm::dbgs() 
            << "Selected spacetime config [" << selectedConfig.getConfigId() 
            << "]: " << selectedConfig.getSpaceTimeTypeString() << "\n");
    } else {
        // Default to first config (or use heuristics)
        selectedConfig = allConfigs[0];
        LLVM_DEBUG(llvm::dbgs() 
            << "Using default spacetime config [0]: " 
            << selectedConfig.getSpaceTimeTypeString() << "\n");
    }
    
    // Use selected configuration
    SmallVector<unsigned> spaceLoops;
    SmallVector<unsigned> timeLoops;
    
    if (failed(selectSpaceLoopsParametric(depInfos, selectedConfig,
                                          spaceLoops, timeLoops))) {
        LLVM_DEBUG(llvm::dbgs() << "Space loop selection failed\n");
        return;
    }
    
    // ... 后续处理 ...
}
```

#### 3.2 添加选项支持

**文件**: `lib/Transforms/SystolicTransform.cpp`

```cpp
struct SystolicTransformOptions {
    // ... 现有选项 ...
    
    /// Maximum systolic array dimension (1, 2, or 3)
    std::optional<unsigned> maxSADim;
    
    /// List all spacetime configurations instead of generating code
    bool listConfigs = false;
};
```

---

### 步骤 4: 实现循环置换

#### 4.1 添加循环置换函数

**文件**: `lib/Transforms/SystolicTransform.cpp`

```cpp
/// Permute loops so that space loops are at the outermost positions
/// Similar to AutoSA's loop_interchange
static LogicalResult permuteLoopsForSpaceTime(
    LoopBand &band,
    const SmallVector<unsigned> &spaceLoops) {
    
    if (band.size() != spaceLoops.size() + /* time loops */) {
        return failure();
    }
    
    // Create target order: space loops first, then time loops
    SmallVector<unsigned> targetOrder;
    std::set<unsigned> spaceSet(spaceLoops.begin(), spaceLoops.end());
    
    // Add space loops first
    for (unsigned idx : spaceLoops) {
        targetOrder.push_back(idx);
    }
    
    // Add time loops
    for (unsigned i = 0; i < band.size(); ++i) {
        if (spaceSet.find(i) == spaceSet.end()) {
            targetOrder.push_back(i);
        }
    }
    
    // Perform permutation using MLIR's loop utilities
    // Note: This may require custom implementation or using existing utilities
    // ...
    
    return success();
}
```

---

## 测试计划

### 测试用例 1: 3 循环 (MM)

**输入**: 3 个嵌套循环 (i, j, k)

**预期输出**: 6 种配置
- [0]: 1D, space=[0]
- [1]: 1D, space=[1]
- [2]: 1D, space=[2]
- [3]: 2D, space=[0,1]
- [4]: 2D, space=[0,2]
- [5]: 2D, space=[1,2]

**验证**: 与 AutoSA 的 ST0-ST5 对应

### 测试用例 2: 4 循环 (MTTKRP)

**输入**: 4 个嵌套循环

**预期输出**: 10+ 种配置
- 1D: 4 种
- 2D: 6 种
- 3D: 4 种（如果 maxSADim >= 3）

**验证**: 与 AutoSA 输出对比

### 测试用例 3: 5 循环 (CNN)

**输入**: 5 个嵌套循环

**预期输出**: 25+ 种配置

**验证**: 配置数量正确

---

## 向后兼容性

### 保持预定义模式支持

```cpp
// 如果 spaceTimeMode 在预定义范围内，且循环数量为 3
// 可以使用预定义配置（向后兼容）
if (numLoops == 3 && options.spaceTimeMode >= 0 && 
    options.spaceTimeMode <= 5) {
    // 可以使用 createFromMode() 作为快捷方式
    // 但建议统一使用枚举结果
}
```

---

## 实现优先级

1. **高优先级** ✅
   - 扩展 ParametricSpaceTime 添加 configId
   - 实现 enumerateSpaceTimeConfigs()
   - 修改主流程使用枚举结果

2. **中优先级** 🟡
   - 实现循环置换
   - 添加 --list-spacetime-configs 选项
   - 添加 --max-sa-dim 选项

3. **低优先级** 🟢
   - 实现自动选择启发式
   - 优化枚举性能

---

## 参考

- AutoSA 源码: `third_party/AutoSA/src/autosa_trans.cpp`
  - `sa_space_time_transform()`: 主枚举函数
  - `sa_space_time_transform_at_dim_async()`: 1D/2D/3D 枚举
- 对比分析: `docs/SPACETIME_COMPARISON.md`

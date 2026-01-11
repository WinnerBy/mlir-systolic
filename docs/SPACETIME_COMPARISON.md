# Spacetime 处理对比分析

> **最后更新**: 2026-01-06  
> **目的**: 对比 AutoSA 和 mlir-systolic 的 spacetime 处理方式，找出差异并确定调整方案

---

## AutoSA 的 Spacetime 处理方式

### 核心算法

AutoSA 在 `sa_space_time_transform()` 中实现：

```cpp
// 伪代码
struct autosa_kernel **sa_space_time_transform(schedule, scop, num_sa) {
    sa_list = [];
    n_sa = 0;
    band_w = get_band_width(schedule);  // 循环数量
    
    // 1. 枚举 1D 阵列
    if (max_sa_dim >= 1 && band_w >= 1) {
        for each loop i in [0, band_w):
            if (is_space_loop_candidate(i)) {
                sa = create_kernel_with_space_loop(i);
                sa->space_time_id = n_sa++;
                sa_list.append(sa);
    }
    
    // 2. 枚举 2D 阵列
    if (max_sa_dim >= 2 && band_w >= 2) {
        for each loop i in [0, band_w):
            if (is_space_loop_candidate(i)) {
                for each loop j in [i+1, band_w):
                    if (is_space_loop_candidate(j)) {
                        sa = create_kernel_with_space_loops(i, j);
                        sa->space_time_id = n_sa++;
                        sa_list.append(sa);
                    }
                }
            }
    }
    
    // 3. 枚举 3D 阵列（如果支持）
    if (max_sa_dim >= 3 && band_w >= 3) {
        // 类似的三重循环
    }
    
    return sa_list;
}
```

### 关键特点

1. **动态枚举**: 根据循环数量动态生成所有可能的 spacetime 配置
2. **组合生成**: 通过嵌套循环枚举所有 space loop 组合
3. **循环置换**: 通过 `loop_interchange` 将选中的 space loops 移到最外层
4. **ID 分配**: 每个配置分配一个递增的 `space_time_id`（从 0 开始）
5. **维度支持**: 支持 1D、2D、3D 阵列（由 `max_sa_dim` 控制）

### Space Loop 候选选择

```cpp
// 在 sa_space_time_transform_at_dim_async 中
for (int h = 0; h < band_w; h++) {
    // 检查所有依赖关系
    for (each dependency) {
        dep_dis = get_dep_dis_at_node(dep, band);
        val = dep_dis[h];
        // Space loop 必须满足：依赖距离 <= 1
        if (!(val == 0 || val == 1)) {
            is_space_loop[h] = false;
            break;
        }
    }
    is_space_loop[h] = true;  // 所有依赖都满足条件
}
```

### 配置数量计算

对于 N 个循环：
- **1D 阵列**: C(N,1) = N 种（选择 1 个作为 space）
- **2D 阵列**: C(N,2) = N*(N-1)/2 种（选择 2 个作为 space）
- **3D 阵列**: C(N,3) = N*(N-1)*(N-2)/6 种

**示例**:
- 3 循环 (MM): 1D=3, 2D=3, 总计=6 种 (ST0-ST5)
- 4 循环 (MTTKRP): 1D=4, 2D=6, 3D=4, 总计=14 种
- 5 循环 (CNN): 1D=5, 2D=10, 3D=10, 总计=25 种

---

## mlir-systolic 当前处理方式

### 核心实现

在 `SystolicTransform.cpp` 中：

```cpp
// 当前实现
ParametricSpaceTime parametricConfig = 
    ParametricSpaceTime::createFromMode(options.spaceTimeMode);

if (parametricConfig.isValid()) {
    selectSpaceLoopsParametric(depInfos, parametricConfig, 
                                spaceLoops, timeLoops);
}
```

### 关键特点

1. **预定义配置**: `createFromMode()` 只预定义了 MM 的 ST0-ST5
2. **固定映射**: 模式号到配置的映射是硬编码的
3. **单一选择**: 一次只处理一个 spacetime 配置
4. **框架支持**: `ParametricSpaceTime` 框架本身支持动态配置，但未充分利用

### 当前限制

1. **只支持 MM**: `createFromMode()` 只处理 3 循环的情况
2. **无动态枚举**: 没有实现类似 AutoSA 的枚举逻辑
3. **固定索引**: spacetime 索引含义固定，不随循环数量变化

---

## 关键差异对比

| 特性 | AutoSA | mlir-systolic (当前) | 影响 |
|------|--------|---------------------|------|
| **枚举方式** | 动态枚举所有可能配置 | 从预定义列表选择 | 🔴 无法支持 4+ 循环 kernel |
| **配置生成** | 运行时根据循环数量生成 | 编译时预定义 | 🔴 扩展性差 |
| **索引含义** | 动态分配，从 0 开始递增 | 固定含义（ST0-ST5） | 🔴 不同 kernel 索引不一致 |
| **维度支持** | 1D/2D/3D（可配置） | 主要 2D | 🟡 功能受限 |
| **循环置换** | 自动执行 loop_interchange | 部分支持 | 🟡 需要完善 |

---

## 需要调整的地方

### 1. 实现动态枚举 ✅ 高优先级

**目标**: 实现类似 AutoSA 的动态枚举逻辑

**实现位置**: `lib/Transforms/SystolicTransform.cpp`

**关键函数**:
```cpp
// 需要新增
LogicalResult enumerateSpaceTimeConfigs(
    const SmallVector<AffineForOp> &loops,
    const SmallVector<LoopDepInfo> &depInfos,
    unsigned maxSADim,  // 最大阵列维度（默认 2）
    SmallVector<ParametricSpaceTime> &configs);
```

**算法**:
```cpp
LogicalResult enumerateSpaceTimeConfigs(...) {
    configs.clear();
    unsigned numLoops = loops.size();
    unsigned configId = 0;
    
    // 1. 枚举 1D 阵列
    if (maxSADim >= 1 && numLoops >= 1) {
        for (unsigned i = 0; i < numLoops; ++i) {
            if (depInfos[i].canBeSpaceLoop) {
                ParametricSpaceTime config;
                config.addSpaceDim(i, loops[i].getName().str());
                // 添加时间维度（剩余循环）
                for (unsigned j = 0; j < numLoops; ++j) {
                    if (j != i) {
                        config.addTimeDim(j, loops[j].getName().str());
                    }
                }
                config.setConfigId(configId++);
                configs.push_back(config);
            }
        }
    }
    
    // 2. 枚举 2D 阵列
    if (maxSADim >= 2 && numLoops >= 2) {
        for (unsigned i = 0; i < numLoops; ++i) {
            if (depInfos[i].canBeSpaceLoop) {
                for (unsigned j = i + 1; j < numLoops; ++j) {
                    if (depInfos[j].canBeSpaceLoop) {
                        ParametricSpaceTime config;
                        config.addSpaceDim(i, loops[i].getName().str());
                        config.addSpaceDim(j, loops[j].getName().str());
                        // 添加时间维度
                        for (unsigned k = 0; k < numLoops; ++k) {
                            if (k != i && k != j) {
                                config.addTimeDim(k, loops[k].getName().str());
                            }
                        }
                        config.setConfigId(configId++);
                        configs.push_back(config);
                    }
                }
            }
        }
    }
    
    // 3. 枚举 3D 阵列（可选）
    if (maxSADim >= 3 && numLoops >= 3) {
        // 类似的三重循环
    }
    
    return success();
}
```

### 2. 扩展 ParametricSpaceTime 框架

**需要添加**:
```cpp
class ParametricSpaceTime {
    // 新增：配置 ID（对应 AutoSA 的 space_time_id）
    unsigned configId;
    
    // 新增：设置配置 ID
    void setConfigId(unsigned id) { configId = id; }
    unsigned getConfigId() const { return configId; }
    
    // 新增：从循环索引列表创建（用于动态枚举）
    static ParametricSpaceTime createFromLoopIndices(
        const SmallVector<unsigned> &spaceLoopIndices,
        const SmallVector<unsigned> &timeLoopIndices,
        const SmallVector<AffineForOp> &loops);
};
```

### 3. 修改选择逻辑

**当前**: 从预定义模式创建配置

**调整后**: 
- 如果提供了 `spaceTimeMode`，从枚举列表中选择对应 ID 的配置
- 如果没有提供，枚举所有配置并让用户选择（或自动选择）

```cpp
// 在 SystolicTransform 中
SmallVector<ParametricSpaceTime> allConfigs;
if (failed(enumerateSpaceTimeConfigs(loops, depInfos, 
                                      options.maxSADim, allConfigs))) {
    return failure();
}

// 选择配置
ParametricSpaceTime selectedConfig;
if (options.spaceTimeMode >= 0 && 
    options.spaceTimeMode < allConfigs.size()) {
    selectedConfig = allConfigs[options.spaceTimeMode];
} else {
    // 自动选择或报错
    selectedConfig = allConfigs[0];  // 默认选择第一个
}
```

### 4. 循环置换实现

**需要**: 确保 space loops 被移到最外层

```cpp
// 在 selectSpaceLoopsParametric 之后
LogicalResult permuteLoopsForSpaceTime(
    LoopBand &band,
    const SmallVector<unsigned> &spaceLoops) {
    
    // 将 space loops 移到最外层
    // 类似 AutoSA 的 loop_interchange
    for (unsigned i = 0; i < spaceLoops.size(); ++i) {
        unsigned targetPos = i;
        unsigned currentPos = spaceLoops[i];
        
        // 执行循环置换
        if (currentPos != targetPos) {
            // 使用 MLIR 的循环置换工具
            permuteLoops(band, currentPos, targetPos);
        }
    }
    
    return success();
}
```

---

## 实现计划

### 阶段 1: 动态枚举 ✅

1. 实现 `enumerateSpaceTimeConfigs()` 函数
2. 扩展 `ParametricSpaceTime` 添加 `configId`
3. 修改 `SystolicTransform` 使用枚举结果

### 阶段 2: 循环置换 ✅

1. 实现 `permuteLoopsForSpaceTime()` 函数
2. 确保 space loops 在正确位置

### 阶段 3: 测试验证 ✅

1. 测试 3 循环 (MM): 验证生成 6 种配置
2. 测试 4 循环 (MTTKRP): 验证生成 10+ 种配置
3. 与 AutoSA 输出对比

---

## 代码修改位置

### 需要修改的文件

1. **`include/systolic/Analysis/ParametricSpaceTime.h`**
   - 添加 `configId` 成员
   - 添加 `createFromLoopIndices()` 静态方法

2. **`lib/Analysis/ParametricSpaceTime.cpp`**
   - 实现 `createFromLoopIndices()`

3. **`lib/Transforms/SystolicTransform.cpp`**
   - 实现 `enumerateSpaceTimeConfigs()`
   - 实现 `permuteLoopsForSpaceTime()`
   - 修改主流程使用枚举结果

4. **`lib/Transforms/SystolicTransform.cpp`** (选项)
   - 添加 `--list-spacetime-configs` 选项
   - 添加 `--max-sa-dim` 选项

---

## 总结

**核心问题**: 
- AutoSA **动态枚举**所有可能的 spacetime 配置
- 我们当前**静态选择**预定义的配置

**解决方案**:
1. 实现动态枚举逻辑
2. 扩展 ParametricSpaceTime 框架
3. 修改选择流程使用枚举结果
4. 确保循环置换正确执行

**预期效果**:
- 支持任意循环数量的 kernel
- Spacetime 索引动态分配，与 AutoSA 一致
- 可以枚举和选择所有可能的配置

# MLIR-Systolic 开发指南

## 项目当前状态

已创建项目基础结构，包括：

```
mlir-systolic/
├── CMakeLists.txt                           # 主构建配置
├── README.md                                # 项目说明
├── docs/
│   └── ARCHITECTURE.md                      # 架构设计文档
├── include/systolic/
│   ├── Analysis/
│   │   ├── SpaceTimeAnalysis.h              # 空间-时间分析接口 ✅
│   │   └── SystolicConfig.h                 # 配置参数 ✅
│   ├── Dialect/HLS/
│   │   ├── HLS.td                           # Dialect 定义 ✅
│   │   ├── Types.td                         # 类型定义 ✅
│   │   ├── Ops.td                           # 操作定义 ✅
│   │   └── HLS.h                            # C++ 头文件 ✅
│   └── Transforms/
│       └── Passes.h                         # Pass 声明 ✅
├── lib/
│   ├── Analysis/                            # 待实现
│   ├── Dialect/HLS/                         # 待实现
│   ├── Transforms/                          # 待实现
│   └── Translation/                         # 待实现
├── tools/systolic-opt/                      # 待实现
└── test/matmul/
    └── matmul_64x64.mlir                    # 测试用例 ✅
```

## 下一步工作

### 1. 完成 HLS Dialect 实现

```bash
# 需要实现的文件
lib/Dialect/HLS/HLS.cpp       # Dialect 注册
lib/Dialect/HLS/Ops.cpp       # 操作实现
```

**参考来源**: ScaleHLS `lib/Dialect/HLS/HLS.cpp`

### 2. 实现 Space-Time 分析

这是项目的核心创新点。需要：

```cpp
// lib/Analysis/SpaceTimeAnalysis.cpp

LogicalResult analyzeSpaceTime(AffineForOp outerLoop, SpaceTimeInfo &result) {
  // 1. 提取循环嵌套
  SmallVector<AffineForOp> loops;
  getLoopNest(outerLoop, loops);
  
  // 2. 调用 Polymer/ISL 计算依赖距离
  // 这里需要集成 Polymer 的能力
  for (unsigned i = 0; i < loops.size(); i++) {
    result.dependenceDistances.push_back(
        computeDependenceDistance(loops[i]));
  }
  
  // 3. 选择空间循环候选
  for (unsigned i = 0; i < loops.size(); i++) {
    if (result.dependenceDistances[i].canBeSpaceLoop()) {
      result.spaceLoopCandidates.push_back(i);
    }
  }
  
  return success();
}
```

**关键问题**: 如何调用 Polymer 的 ISL 接口？

选项 A: 直接调用 ISL C API
```cpp
#include <isl/flow.h>
isl_union_access_info *ai = ...;
isl_union_flow *flow = isl_union_access_info_compute_flow(ai);
```

选项 B: 使用 Polymer 的包装
```cpp
// 使用 Polymer 的 IslScop 类
polymer::IslScop scop = polymer::extractScop(func);
auto deps = scop.computeDependences();
```

### 3. 实现 Space-Time 变换

```cpp
// lib/Transforms/SpaceTimeTransform.cpp

struct SpaceTimeTransformPass : public PassWrapper<...> {
  void runOnOperation() override {
    // 1. 分析
    SpaceTimeInfo info;
    analyzeSpaceTime(outerLoop, info);
    
    // 2. 选择空间循环
    selectSpaceLoops(info, 2);  // 2D array
    
    // 3. 循环置换
    permuteTo(loops, getSpaceTimeOrder(info));
    
    // 4. 分块
    applyTiling(loops, config.arrayPart);
    applyTiling(innerLoops, config.latency);
  }
};
```

### 4. 实现 Stream 生成

复用 ScaleHLS 的 AutoSAMap 中的流生成逻辑：

```cpp
// lib/Transforms/StreamGeneration.cpp

// 生成 Stream 通道
auto streamType = hls::StreamType::get(elementType, depth);
auto streamOp = builder.create<hls::StreamOp>(loc, streamType, depth);

// 生成 Task
auto taskOp = builder.create<hls::TaskOp>(loc, "PE_0_0");
```

### 5. 实现 EmitHLSCpp

从 ScaleHLS 移植 `lib/Translation/EmitHLSCpp.cpp`，简化为模板化输出。

## Polymer 集成方案

### 方案 1: 编译时链接 (推荐)

```cmake
# CMakeLists.txt
find_package(Polymer REQUIRED)
target_link_libraries(systolic-analysis PolymerSupport)
```

需要 Polymer 构建为库。

### 方案 2: 运行时调用

通过 `polymer-opt` 命令行工具提取信息：

```bash
# 导出 ISL schedule
polymer-opt input.mlir -extract-scop-stmt -islexternal-dump-schedules=./

# 解析 schedule 文件
```

### 方案 3: 直接集成 ISL

```cmake
find_package(ISL REQUIRED)
target_link_libraries(systolic-analysis isl)
```

## 验证计划

### 阶段 1: 硬编码验证

先用硬编码的 MatMul 配置验证整个流程：

```cpp
// 硬编码 space_time=3, array_part={16,16,16}, latency={8,8}
SystolicConfig config = createMatMulConfig(64, 64, 64, {16,16,16}, {8,8});
```

### 阶段 2: 自动分析

接入 Polymer 后，实现自动空间循环选择。

### 阶段 3: 与 AutoSA 对比

对比生成的 HLS C++ 代码与 AutoSA 输出。

## 参考资源

1. **ScaleHLS 代码**
   - `/home/user/work/scalehls/lib/Transforms/Dataflow/AutoSAMap.cpp`
   - `/home/user/work/scalehls/lib/Translation/EmitHLSCpp.cpp`

2. **AutoSA 代码**
   - `/home/user/work/AutoSA/src/autosa_trans.cpp`
   - `/home/user/work/AutoSA/src/autosa_codegen.cpp`

3. **Polymer 代码**
   - `/home/user/work/Polygeist/tools/polymer/`

4. **先前文档**
   - `/home/user/work/scalehls/docs/MLIR_AutoSA_Implementation.md`
   - `/home/user/work/scalehls/test/Transforms/Dataflow/AutoSA/POLYMER_INTEGRATION_PLAN.md`

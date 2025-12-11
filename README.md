# MLIR-Systolic

基于 MLIR 和 Polymer 的脉动阵列编译器。

## 项目目标

将 Affine 循环嵌套自动转换为脉动阵列 HLS C++ 代码，结合：
- **Polymer** 的多面体分析能力（依赖距离、空间循环选择）
- **MLIR** 的变换和代码生成能力（参考 ScaleHLS）

## 架构

```
输入: Affine MLIR (来自 Polygeist 或手写)
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│  Pass 1: SystolicTransform                              │
│  ┌───────────────────────────────────────────────────┐  │
│  │ • 依赖分析 (Polymer/ISL)                          │  │
│  │ • 空间循环选择 (依赖距离 ≤ 1)                     │  │
│  │ • 循环置换 (空间循环外移)                         │  │
│  │ • 多级分块 (array_part + latency)                 │  │
│  └───────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
         │
         ▼ 分块后的 Affine IR
┌─────────────────────────────────────────────────────────┐
│  Pass 2: DataflowGeneration                             │
│  ┌───────────────────────────────────────────────────┐  │
│  │ • Stream 通道生成                                 │  │
│  │ • I/O 模块生成 (L1/L2/L3)                         │  │
│  │ • PE 模块生成                                     │  │
│  │ • 双缓冲 (ping-pong)                              │  │
│  └───────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
         │
         ▼ HLS Dialect IR
┌─────────────────────────────────────────────────────────┐
│  Translation: EmitHLSCpp                                │
│  ┌───────────────────────────────────────────────────┐  │
│  │ • HLS C++ 代码生成                                │  │
│  │ • Pragma 插入 (pipeline, array_partition, etc.)   │  │
│  └───────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
         │
         ▼
输出: HLS C++ (Vivado HLS / Vitis 兼容)
```

## 核心实现文件

本项目采用**三个核心文件**的设计：

| 文件 | 功能 | 行数估计 | 对应 AutoSA |
|------|------|---------|-------------|
| `SystolicTransform.cpp` | 分析 + 分块 + 置换 | ~600 | `sa_space_time_transform` + `compute_management` |
| `DataflowGeneration.cpp` | Stream/Task 生成 | ~1000 | `comm_management` + `generate_hw_modules` |
| `EmitHLSCpp.cpp` | HLS C++ 输出 | ~1200 | `print_hw` |

### 详细职责

**SystolicTransform.cpp** (Pass 1: `-systolic-transform`)
- 调用 Polymer/ISL 计算依赖距离
- 选择空间循环（依赖距离 ≤ 1）
- 执行循环置换（空间循环移到外层）
- 执行多级分块（array_part + latency）

**DataflowGeneration.cpp** (Pass 2: `-systolic-dataflow`)
- 创建 `hls.dataflow.dispatch` 区域
- 生成 Stream 通道（L1/L2/L3 层级）
- 生成 I/O Task（含双缓冲）
- 生成 PE Task（脉动数据流）

**EmitHLSCpp.cpp** (Translation: `-emit-hlscpp`)
- 遍历 HLS Dialect IR
- 生成 HLS C++ 代码
- 插入 Pragma（pipeline, array_partition 等）

## 依赖

- LLVM/MLIR 18 (通过 Polygeist)
- Polygeist + Polymer (ISL 模式)
- ISL (Integer Set Library)

## 构建

```bash
# 确保 Polygeist 已构建
export POLYGEIST_BUILD=/home/user/work/Polygeist/build

# 构建本项目
mkdir build && cd build
cmake .. \
  -DMLIR_DIR=$POLYGEIST_BUILD/lib/cmake/mlir \
  -DLLVM_DIR=$POLYGEIST_BUILD/lib/cmake/llvm \
  -GNinja
ninja
```

## 使用示例

```bash
# 1. C 代码转换为 MLIR (使用 Polygeist)
cgeist matmul.c -function=matmul -S -memref-fullrank > matmul.mlir

# 2. 应用脉动阵列变换
systolic-opt matmul.mlir \
  -systolic-transform="array-part=16,16,16 latency=8,8" \
  -systolic-dataflow \
  -o matmul_systolic.mlir

# 3. 生成 HLS C++
systolic-translate matmul_systolic.mlir -emit-hlscpp > matmul_hls.cpp
```

## 与 AutoSA 的对比

| 功能 | AutoSA | mlir-systolic |
|------|--------|---------------|
| 前端 | PET (C解析) | Polygeist (C→MLIR) |
| 依赖分析 | ISL | Polymer (ISL) |
| 调度 | ISL schedule tree | MLIR Affine + Polymer |
| 变换 | ISL AST build | MLIR transforms |
| 代码生成 | 自定义 C printer | EmitHLSCpp |

## 目录结构

```
mlir-systolic/
├── include/systolic/
│   ├── Dialect/HLS/           # HLS Dialect 定义
│   ├── Analysis/              # 分析接口
│   └── Transforms/            # Pass 声明
├── lib/
│   ├── Dialect/HLS/           # HLS.cpp (~300 行)
│   ├── Analysis/              # SpaceTimeAnalysis.cpp (~400 行)
│   ├── Transforms/
│   │   ├── SystolicTransform.cpp   # (~600 行) ⭐核心
│   │   └── DataflowGeneration.cpp  # (~1000 行) ⭐核心
│   └── Translation/
│       └── EmitHLSCpp.cpp          # (~1200 行) ⭐核心
├── tools/systolic-opt/        # 主工具
├── test/matmul/               # 测试用例
└── docs/                      # 文档
```

## 开发路线

### Phase 1: 基础设施 ✅
- [x] 项目结构
- [x] HLS Dialect 定义 (TableGen)
- [x] 分析接口设计

### Phase 2: 核心实现 (进行中)
- [ ] HLS.cpp - Dialect 实现
- [ ] SystolicTransform.cpp - 变换 Pass
- [ ] DataflowGeneration.cpp - 数据流生成
- [ ] EmitHLSCpp.cpp - 代码生成

### Phase 3: Polymer 集成
- [ ] 依赖距离分析
- [ ] 空间循环自动选择

### Phase 4: 验证
- [ ] MatMul 端到端测试
- [ ] 与 AutoSA 输出对比

## 参考项目

- [ScaleHLS](https://github.com/UIUC-ChenLab/scalehls) - HLS Dialect 和代码生成
- [AutoSA](https://github.com/UCLA-VAST/AutoSA) - 脉动阵列生成算法
- [Polygeist/Polymer](https://github.com/llvm/Polygeist) - 多面体分析

## License

MIT License

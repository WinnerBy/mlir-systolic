# 统一构建指南（推荐方式）

## 概述

统一构建方式可以一次性构建 LLVM、MLIR、Polly、Polygeist、Polymer 和 mlir-systolic，类似于 ScaleHLS 的方式。

**优势：**
- ✅ 一次性构建所有依赖
- ✅ 版本兼容性有保障
- ✅ 配置更简单
- ✅ 减少内存占用（共享构建目录）

## 前提条件

- ✅ Polygeist 已拉取（包含 llvm-project submodule）
- ✅ 路径：`third_party/Polygeist`
- ✅ llvm-project 路径：`third_party/Polygeist/llvm-project`

## 构建步骤

### 步骤 1: 清理旧构建（可选）

```bash
cd /home/user/work/mlir-systolic
rm -rf build
```

### 步骤 2: 配置统一构建

```bash
cd /home/user/work/mlir-systolic
mkdir -p build
cd build

# 统一构建配置
cmake -G Ninja ../third_party/Polygeist/llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist;mlir-systolic" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=../third_party/Polygeist \
  -DLLVM_EXTERNAL_MLIR_SYSTOLIC_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1 \
  -DSYSTOLIC_ENABLE_POLYMER=1
```

**关键配置说明：**
- `-DLLVM_EXTERNAL_PROJECTS="polygeist;mlir-systolic"` - 将 Polygeist 和 mlir-systolic 都作为外部项目
- `-DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=../third_party/Polygeist` - Polygeist 源码目录
- `-DLLVM_EXTERNAL_MLIR_SYSTOLIC_SOURCE_DIR=..` - mlir-systolic 源码目录（相对于 build 目录）
- `-DPOLYGEIST_ENABLE_POLYMER=1` - 启用 Polymer
- `-DPOLYGEIST_POLYMER_ENABLE_ISL=1` - 使用 ISL（需要 polly）
- `-DSYSTOLIC_ENABLE_POLYMER=1` - 启用 mlir-systolic 的 Polymer 集成

### 步骤 3: 构建（限制并行度防止内存溢出）

```bash
# 根据内存选择并行度
# 内存 < 8GB: -j1
# 内存 8-16GB: -j2（推荐）
# 内存 > 16GB: -j4

# 先构建 Polymer 库（mlir-systolic 依赖它）
ninja -j2 PolymerSupport PolymerTargetISL PolymerTransforms

# 然后构建 mlir-systolic
ninja -j2 systolic-opt

# 或者构建所有组件
# ninja -j2
```

### 步骤 4: 验证构建

```bash
# 检查 Polymer 库
ls -la lib/libPolymer*.a

# 检查 mlir-systolic 工具
ls -la bin/systolic-opt

# 测试
./bin/systolic-opt test/matmul/matmul.mlir --systolic-transform
```

## 目录结构

统一构建后的目录结构：

```
mlir-systolic/
├── build/                    # 统一构建目录
│   ├── bin/                  # 所有工具（包括 systolic-opt）
│   ├── lib/                  # 所有库（包括 Polymer 库）
│   └── ...
├── third_party/
│   └── Polygeist/
│       └── llvm-project/     # LLVM/MLIR 源码
└── ...
```

## 与 ScaleHLS 的对比

ScaleHLS 的方式：
- Polygeist 在项目根目录（不是 third_party）
- 使用 `LLVM_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Polygeist/llvm-project/llvm`

我们的方式：
- Polygeist 在 `third_party/Polygeist`
- 使用 `LLVM_EXTERNAL_PROJECTS` 将 Polygeist 和 mlir-systolic 都作为外部项目

两种方式都可以，我们的方式更符合常见的项目结构。

## 故障排除

### 问题 1: CMake 找不到 Polygeist

确保路径正确：
```bash
ls -la third_party/Polygeist/CMakeLists.txt
ls -la third_party/Polygeist/llvm-project/llvm/CMakeLists.txt
```

### 问题 2: Polymer 库未找到

确保在 CMake 配置中启用了 Polymer：
```bash
# 检查 CMake 输出中是否有：
# -- POLYGEIST_ENABLE_POLYMER:BOOL=1
# -- POLYGEIST_POLYMER_ENABLE_ISL:BOOL=1
```

### 问题 3: WSL 卡死

使用更少的并行度：
```bash
ninja -j1 PolymerTransforms
```

### 问题 4: 构建时间过长

这是正常的，首次构建可能需要 30 分钟到数小时。可以：
- 使用 `-j2` 或 `-j4` 增加并行度（如果内存充足）
- 使用 `lld` 链接器加速（如果可用）

## 后续开发

统一构建后，每次修改代码只需：

```bash
cd build
ninja -j2 systolic-opt  # 只构建 mlir-systolic
```

不需要重新构建 LLVM/MLIR/Polygeist。


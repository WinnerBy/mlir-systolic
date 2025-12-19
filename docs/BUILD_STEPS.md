# mlir-systolic 构建指南

## 概述

本项目使用 **统一构建方式**（参考 [Polygeist README Option 2](third_party/Polygeist/README.md#option-2-using-unified-llvm-mlir-clang-and-polygeist-build)），一次性构建 LLVM/MLIR/Polly/Polygeist/Polymer 和 mlir-systolic。

## 快速开始

```bash
# 1. 初始化 submodule
git submodule update --init --recursive

# 2. 构建 Polygeist + Polymer
./scripts/build-polygeist.sh

# 3. 构建 mlir-systolic
./scripts/build-systolic.sh
```

## 详细步骤

### 步骤 1: 初始化 Submodule

```bash
git submodule update --init --recursive
```

这会拉取：
- `third_party/Polygeist` - Polygeist 框架
- `third_party/Polygeist/llvm-project` - LLVM/MLIR 源码（Polygeist 的 submodule）

### 步骤 2: 构建 Polygeist + Polymer

使用提供的脚本（推荐）：

```bash
./scripts/build-polygeist.sh
```

#### 脚本功能

- ✅ 自动检测系统内存并选择合适的并行度（防止 WSL 卡死）
- ✅ 使用统一构建方式（参考 Polygeist README Option 2）
- ✅ 构建 Polymer 库和 mlir-systolic 需要的 MLIR 库

#### 手动构建（如果脚本不适用）

```bash
cd third_party/Polygeist
mkdir -p build && cd build

# 配置 CMake（统一构建方式）
cmake -G Ninja ../llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1

# 构建 Polymer 库和必要的 MLIR 库
# ⚠️ 重要：根据内存选择并行度
# 内存 < 8GB: 使用 -j1（单线程，最安全）
# 内存 8-16GB: 使用 -j2（推荐）
# 内存 > 16GB: 可以使用 -j4 或更多
ninja -j2 PolymerSupport PolymerTargetISL PolymerTransforms \
  MLIRArithTransforms MLIRArithValueBoundsOpInterfaceImpl MLIROptLib
```

**关键配置说明：**
- `-DLLVM_ENABLE_PROJECTS="clang;mlir;polly"` - 包含 polly（ISL 需要）
- `-DLLVM_EXTERNAL_PROJECTS="polygeist"` - 将 Polygeist 作为外部项目
- `-DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=..` - Polygeist 源码目录
- `-DPOLYGEIST_ENABLE_POLYMER=1` - 启用 Polymer
- `-DPOLYGEIST_POLYMER_ENABLE_ISL=1` - 使用 ISL（需要 polly）

#### 验证构建

构建完成后，检查库文件：

```bash
ls third_party/Polygeist/build/lib/libPolymer*.a
ls third_party/Polygeist/build/lib/libMLIRArith*.a
ls third_party/Polygeist/build/lib/libMLIROptLib.a
```

应该看到：
- ✅ `libPolymerSupport.a`
- ✅ `libPolymerTargetISL.a`
- ✅ `libPolymerTransforms.a` ← **必需**
- ✅ `libMLIRArithTransforms.a`
- ✅ `libMLIRArithValueBoundsOpInterfaceImpl.a`
- ✅ `libMLIROptLib.a`

### 步骤 3: 构建 mlir-systolic

使用提供的脚本（推荐）：

```bash
./scripts/build-systolic.sh
```

#### 脚本功能

- ✅ 自动检测 Polygeist 构建目录
- ✅ 自动配置 CMake
- ✅ 自动检测系统内存并选择合适的并行度

#### 手动构建（如果脚本不适用）

```bash
mkdir -p build && cd build

cmake .. \
  -DMLIR_DIR=../third_party/Polygeist/build/lib/cmake/mlir \
  -DLLVM_DIR=../third_party/Polygeist/build/lib/cmake/llvm \
  -DPOLYGEIST_BUILD=../third_party/Polygeist/build

# ⚠️ 重要：根据内存选择并行度
cmake --build . -j2
```

#### 验证构建

构建完成后，检查工具：

```bash
./build/bin/systolic-opt --help
./build/bin/systolic-translate --help
```

## 故障排除

### 问题 1: CMake 找不到 MLIR_DIR

**症状：**
```
Could not find a package configuration file provided by "MLIR"
```

**解决方案：**
确保 Polygeist 已构建完成，并且路径正确：
```bash
ls third_party/Polygeist/build/lib/cmake/mlir/MLIRConfig.cmake
```

如果文件不存在，重新运行 `./scripts/build-polygeist.sh`。

### 问题 2: 找不到 Polymer 库

**症状：**
```
Required Polymer libraries not found: libPolymerTransforms.a
```

**解决方案：**
确保构建了 Polymer 库：
```bash
ls third_party/Polygeist/build/lib/libPolymer*.a
```

如果库不存在，重新运行 `./scripts/build-polygeist.sh`，并确保：
- `-DPOLYGEIST_ENABLE_POLYMER=1` 已设置
- `-DPOLYGEIST_POLYMER_ENABLE_ISL=1` 已设置
- `polly` 包含在 `LLVM_ENABLE_PROJECTS` 中

### 问题 3: 构建时 WSL 卡死

**症状：**
WSL 在构建过程中变得无响应。

**解决方案：**
1. 使用更少的并行任务：
   ```bash
   ninja -j1  # 单线程，最安全
   ```

2. 使用 Release 模式（已在脚本中设置）：
   ```bash
   -DCMAKE_BUILD_TYPE=Release
   -DLLVM_ENABLE_ASSERTIONS=OFF
   ```

3. 检查系统内存：
   ```bash
   free -h
   ```

### 问题 4: 找不到 MLIR 库（如 MLIRArithTransforms）

**症状：**
```
No rule to make target 'libMLIRArithTransforms.a'
```

**解决方案：**
在构建 Polygeist 时，确保构建了所有需要的 MLIR 库：
```bash
cd third_party/Polygeist/build
ninja -j2 MLIRArithTransforms MLIRArithValueBoundsOpInterfaceImpl MLIROptLib
```

或者重新运行 `./scripts/build-polygeist.sh`，脚本会自动构建这些库。

### 问题 5: ISL headers 警告

**症状：**
```
CMake Warning: ISL headers not found. Polymer may not compile correctly.
```

**解决方案：**
这个警告通常可以忽略。如果编译失败，确保 `polly` 包含在 `LLVM_ENABLE_PROJECTS` 中。

## 参考

- [Polygeist README](third_party/Polygeist/README.md) - Polygeist 构建说明
- [Polymer README](third_party/Polygeist/tools/polymer/README.md) - Polymer 使用说明

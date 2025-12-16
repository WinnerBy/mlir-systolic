# 构建 Polygeist Submodule

本文档说明如何在 mlir-systolic 项目中构建 Polygeist submodule 以启用 Polymer 支持。

---

## 📋 前置要求

1. **MLIR/LLVM 已构建**
   - 需要 MLIR 和 LLVM 的构建目录
   - 通常位于 `/path/to/llvm-project/build`

2. **GMP 库**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libgmp-dev
   
   # macOS
   brew install gmp
   ```

3. **CMake 和 Ninja**
   ```bash
   sudo apt-get install cmake ninja-build
   ```

---

## 🔧 构建步骤

### 步骤 1: 进入 Polygeist 目录

```bash
cd third_party/Polygeist
```

### 步骤 2: 创建构建目录

```bash
mkdir build && cd build
```

### 步骤 3: 配置 CMake

```bash
cmake .. \
  -DMLIR_DIR=<path-to-mlir>/lib/cmake/mlir \
  -DLLVM_DIR=<path-to-llvm>/lib/cmake/llvm \
  -DPOLYGEIST_ENABLE_POLYMER=ON \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=ON \
  -GNinja
```

**示例**（如果 MLIR/LLVM 在 `/home/user/work/Polygeist/llvm-project/build`）:
```bash
cmake .. \
  -DMLIR_DIR=/home/user/work/Polygeist/llvm-project/build/lib/cmake/mlir \
  -DLLVM_DIR=/home/user/work/Polygeist/llvm-project/build/lib/cmake/llvm \
  -DPOLYGEIST_ENABLE_POLYMER=ON \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=ON \
  -GNinja
```

### 步骤 4: 构建 Polymer 库

```bash
ninja PolymerSupport PolymerTargetISL
```

或者构建所有 Polygeist 目标：
```bash
ninja
```

### 步骤 5: 验证构建

检查库文件是否存在：
```bash
ls -la lib/libPolymer*.a
```

应该看到：
- `libPolymerSupport.a`
- `libPolymerTargetISL.a`

---

## 🔗 配置 mlir-systolic

### 方式 1: 使用环境变量（推荐）

```bash
export POLYGEIST_BUILD=$(pwd)  # 在 third_party/Polygeist/build 目录下
cd ../../..  # 回到 mlir-systolic 根目录
cd build
cmake .. -DMLIR_DIR=... -DLLVM_DIR=...
```

### 方式 2: 使用 CMake 变量

```bash
cd mlir-systolic/build
cmake .. \
  -DMLIR_DIR=... \
  -DLLVM_DIR=... \
  -DPOLYGEIST_BUILD=../third_party/Polygeist/build
```

### 方式 3: 自动检测

如果 Polygeist 构建在 `third_party/Polygeist/build`，CMake 会自动检测。

---

## ✅ 验证集成

### 1. 检查 CMake 配置

运行 `cmake ..` 后，应该看到：
```
-- Found Polygeist submodule at: ...
-- Found Polymer libraries:
--   - .../libPolymerSupport.a
--   - .../libPolymerTargetISL.a
-- Found GMP: ...
-- Found ISL headers at: ...
-- Polymer integration enabled successfully
```

### 2. 检查编译定义

```bash
cat lib/Analysis/CMakeFiles/obj.SystolicAnalysis.dir/flags.make | grep SYSTOLIC
```

应该看到 `-DSYSTOLIC_ENABLE_POLYMER=1`

### 3. 编译测试

```bash
ninja SystolicAnalysis
```

应该成功编译，没有关于 Polymer 或 ISL 的错误。

---

## 🐛 常见问题

### 问题 1: GMP 未找到

**错误**: `Could not find GMP`

**解决**:
```bash
sudo apt-get install libgmp-dev
# 或
brew install gmp
```

### 问题 2: ISL 头文件未找到

**错误**: `ISL headers not found`

**解决**: 
- 确保 LLVM 构建时启用了 Polly
- 检查 `${LLVM_BINARY_DIR}/tools/polly/lib/External/isl/include` 是否存在

### 问题 3: Polymer 库未找到

**错误**: `Polymer libraries not found`

**解决**:
- 确保已构建 Polymer 库：`ninja PolymerSupport PolymerTargetISL`
- 检查 `POLYGEIST_BUILD` 环境变量或 CMake 变量是否正确设置

---

## 📝 完整构建脚本示例

```bash
#!/bin/bash
set -e

# 设置路径（根据实际情况修改）
MLIR_DIR="/home/user/work/Polygeist/llvm-project/build/lib/cmake/mlir"
LLVM_DIR="/home/user/work/Polygeist/llvm-project/build/lib/cmake/llvm"

# 构建 Polygeist
cd third_party/Polygeist
mkdir -p build && cd build

cmake .. \
  -DMLIR_DIR=${MLIR_DIR} \
  -DLLVM_DIR=${LLVM_DIR} \
  -DPOLYGEIST_ENABLE_POLYMER=ON \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=ON \
  -GNinja

ninja PolymerSupport PolymerTargetISL

# 设置环境变量
export POLYGEIST_BUILD=$(pwd)

# 回到项目根目录并配置
cd ../../..
mkdir -p build && cd build

cmake .. \
  -DMLIR_DIR=${MLIR_DIR} \
  -DLLVM_DIR=${LLVM_DIR} \
  -GNinja

ninja
```

---

**注意**: 确保 MLIR/LLVM 的版本与 Polygeist 兼容。


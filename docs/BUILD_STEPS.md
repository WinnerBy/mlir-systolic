# mlir-systolic 构建步骤（完整版）

## ⚠️ 重要提示：防止 WSL 卡死

**如果之前构建时 WSL 卡死，请使用优化版构建步骤：**
- 查看 `docs/BUILD_STEPS_OPTIMIZED.md` 获取内存优化建议
- **关键优化**：使用 `-j2` 限制并行度，使用 Release 模式

## 前提条件

- ✅ Polygeist 已拉取（包含 llvm-project submodule）
- ✅ 路径：`third_party/Polygeist`
- ✅ llvm-project 路径：`third_party/Polygeist/llvm-project`
- ✅ **内存建议**：至少 8GB（如果不足，请使用优化版步骤）

## 构建策略

根据 Polygeist README，有两种方式。我们推荐**方式 2（统一构建）**，因为：
- LLVM/MLIR 是 Polygeist 的 submodule，可以一同构建
- 配置更简单，依赖关系更清晰
- 确保版本兼容性

## 方式 1: 分步构建（如果 LLVM 已单独构建）

### 步骤 1.1: 构建 LLVM/MLIR/Polly

```bash
cd /home/user/work/mlir-systolic/third_party/Polygeist/llvm-project
mkdir -p build
cd build

cmake -G Ninja ../llvm \
  -DLLVM_ENABLE_PROJECTS="mlir;clang;polly" \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DCMAKE_BUILD_TYPE=Release

ninja
```

### 步骤 1.2: 构建 Polygeist（启用 Polymer + ISL）

```bash
cd /home/user/work/mlir-systolic/third_party/Polygeist
mkdir -p build
cd build

cmake -G Ninja .. \
  -DMLIR_DIR=$PWD/../llvm-project/build/lib/cmake/mlir \
  -DCLANG_DIR=$PWD/../llvm-project/build/lib/cmake/clang \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1

ninja PolymerTransforms
```

## 方式 2: 统一构建（推荐）⭐

这是最简单的方式，一次性构建所有组件：

```bash
cd /home/user/work/mlir-systolic/third_party/Polygeist
mkdir -p build
cd build

cmake -G Ninja ../llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1

# ⚠️ 重要：限制并行度防止内存溢出
# 如果内存 < 8GB，使用 -j1（单线程，最安全）
# 如果内存 8-16GB，使用 -j2（推荐）
# 如果内存 > 16GB，可以使用 -j4 或更多
ninja -j2 PolymerTransforms

# 或者构建所有组件（同样限制并行度）
# ninja -j2
```

**关键配置说明：**
- `-DLLVM_ENABLE_PROJECTS="clang;mlir;polly"` - 包含 polly（ISL 需要）
- `-DLLVM_EXTERNAL_PROJECTS="polygeist"` - 将 Polygeist 作为外部项目
- `-DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=..` - Polygeist 源码目录
- `-DPOLYGEIST_ENABLE_POLYMER=1` - 启用 Polymer
- `-DPOLYGEIST_POLYMER_ENABLE_ISL=1` - 使用 ISL（需要 polly）

## 验证构建

构建完成后，检查库文件：

```bash
ls -la /home/user/work/mlir-systolic/third_party/Polygeist/build/lib/libPolymer*
```

应该看到：
- ✅ `libPolymerSupport.a`
- ✅ `libPolymerTargetISL.a`
- ✅ `libPolymerTransforms.a` ← **必需**

## 配置 mlir-systolic

### 步骤 1: 设置环境变量

```bash
export POLYGEIST_BUILD=/home/user/work/mlir-systolic/third_party/Polygeist/build
```

或者添加到 `~/.bashrc`：

```bash
echo 'export POLYGEIST_BUILD=/home/user/work/mlir-systolic/third_party/Polygeist/build' >> ~/.bashrc
source ~/.bashrc
```

### 步骤 2: 重新配置 mlir-systolic

```bash
cd /home/user/work/mlir-systolic/build
rm -f CMakeCache.txt
cmake ..
```

应该看到：
```
-- Linked Polymer Transforms library to SystolicTransforms: /path/to/libPolymerTransforms.a
```

### 步骤 3: 构建 mlir-systolic

```bash
cmake --build . -j$(nproc)
```

### 步骤 4: 测试

```bash
./build/bin/systolic-opt test/matmul/matmul.mlir --systolic-transform
```

## 故障排除

### 问题 1: CMake 找不到 MLIR_DIR

如果使用方式 1，确保 LLVM 已构建完成，并且路径正确。

### 问题 2: 找不到 polly

确保 `polly` 包含在 `LLVM_ENABLE_PROJECTS` 中。

### 问题 3: PolymerTransforms 构建失败

检查：
1. 是否启用了 `POLYGEIST_ENABLE_POLYMER=1`
2. 是否启用了 `POLYGEIST_POLYMER_ENABLE_ISL=1`
3. 是否包含了 `polly` 在 `LLVM_ENABLE_PROJECTS` 中

### 问题 4: mlir-systolic CMake 找不到 libPolymerTransforms.a

确保：
1. `POLYGEIST_BUILD` 环境变量已设置
2. 库文件确实存在于 `${POLYGEIST_BUILD}/lib/` 目录
3. 重新运行 `cmake ..` 清除缓存

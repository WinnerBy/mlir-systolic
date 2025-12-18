# 开发模式构建指南（快速迭代）

## 概述

在开发阶段，推荐使用**分步构建**方式：
1. **先单独构建 Polygeist + Polymer**（只构建需要的库）
2. **然后单独构建 mlir-systolic**（使用已构建的 Polygeist）

**优势：**
- ✅ 构建速度快（不需要每次都构建整个 LLVM/MLIR）
- ✅ 迭代开发效率高
- ✅ 更容易排查问题
- ✅ 节省内存和时间

## 前提条件

- ✅ Polygeist 已拉取（包含 llvm-project submodule）
- ✅ LLVM/MLIR 已构建（或者使用 Polygeist 统一构建中的 LLVM/MLIR）

## 步骤 1: 构建 Polygeist + Polymer（统一构建方式）

根据 Polygeist README Option 2，Polygeist 可以使用统一构建方式，**不需要预先构建 LLVM/MLIR**。
它会使用 Polygeist 的 `llvm-project` submodule 和系统编译器（clang 或 gcc）来统一构建。

### 方式 1: 使用快速构建脚本（推荐）

```bash
cd /home/user/work/mlir-systolic
./scripts/build-polygeist-dev.sh
```

### 方式 2: 手动执行（参考 Polygeist README Option 2）

```bash
cd /home/user/work/mlir-systolic/third_party/Polygeist
rm -rf build  # 清理旧构建（可选）
mkdir -p build && cd build

# 统一构建方式：从 llvm-project/llvm 开始配置
# 不需要预先构建 LLVM/MLIR，使用系统编译器编译
cmake -G Ninja ../llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1

# 只构建 Polymer 需要的库（快速）
# 注意：首次构建可能需要较长时间，因为需要先构建 LLVM/MLIR 基础库
ninja -j2 PolymerSupport PolymerTargetISL PolymerTransforms

# 验证
ls -la lib/libPolymer*.a
```

**关键点：**
- ✅ 使用 `../llvm-project/llvm` 作为配置起点（不是 `..`）
- ✅ 使用 `-DLLVM_EXTERNAL_PROJECTS="polygeist"` 将 Polygeist 作为外部项目
- ✅ 不需要预先构建 LLVM/MLIR，系统编译器会自动编译
- ✅ 首次构建需要较长时间（需要构建 LLVM/MLIR 基础库）

## 步骤 2: 构建 mlir-systolic（使用已构建的 Polygeist）

```bash
cd /home/user/work/mlir-systolic
rm -rf build  # 清理旧构建
mkdir -p build && cd build

# 设置 Polygeist 构建目录
export POLYGEIST_BUILD=/home/user/work/mlir-systolic/third_party/Polygeist/build

# 配置（使用 Polygeist 统一构建中的 LLVM/MLIR）
# 脚本会自动检测并使用 Polygeist 构建目录中的 LLVM/MLIR
cmake .. \
  -DMLIR_DIR=${POLYGEIST_BUILD}/lib/cmake/mlir \
  -DLLVM_DIR=${POLYGEIST_BUILD}/lib/cmake/llvm \
  -DPOLYGEIST_BUILD=${POLYGEIST_BUILD}

# 构建 mlir-systolic（快速）
cmake --build . -j2

# 验证
ls -la bin/systolic-opt
```

**或者使用脚本（自动检测路径）：**

```bash
./scripts/build-systolic-dev.sh
```

## 开发工作流

### 典型开发循环

1. **修改 mlir-systolic 代码**
2. **重新构建 mlir-systolic**（快速，只需几秒到几分钟）
   ```bash
   cd build
   cmake --build . -j2
   ```
3. **测试**
   ```bash
   ./bin/systolic-opt test/matmul/matmul.mlir --systolic-transform
   ```

### 如果修改了 Polymer 相关代码

如果修改了 `lib/Analysis/PolymerAnalysis.cpp` 等文件，只需要重新构建 mlir-systolic：

```bash
cd build
cmake --build . -j2
```

### 如果需要修改 Polygeist/Polymer 本身

1. 修改 Polygeist/Polymer 代码
2. 重新构建 Polygeist（只构建 Polymer 库）：
   ```bash
   cd third_party/Polygeist/build
   ninja -j2 PolymerSupport PolymerTargetISL PolymerTransforms
   ```
3. 重新构建 mlir-systolic：
   ```bash
   cd ../../build
   cmake --build . -j2
   ```

## 工作原理

### Polygeist 统一构建方式

根据 Polygeist README Option 2，Polygeist 支持统一构建方式：

1. **使用 Polygeist 的 `llvm-project` submodule**
   - Polygeist 包含完整的 LLVM/MLIR 源码（作为 submodule）
   - 路径：`third_party/Polygeist/llvm-project`

2. **从 `llvm-project/llvm` 开始配置**
   - 使用 `cmake ../llvm-project/llvm` 而不是 `cmake ..`
   - 将 Polygeist 作为外部项目：`-DLLVM_EXTERNAL_PROJECTS="polygeist"`

3. **使用系统编译器**
   - 不需要预先构建 LLVM/MLIR
   - 系统编译器（clang 或 gcc）会自动编译所有需要的组件

4. **一次性构建所有组件**
   - LLVM、MLIR、Polly、Polygeist、Polymer 都在同一个构建目录中
   - 版本兼容性有保障

## 快速构建脚本

已创建以下脚本方便开发：

1. **`scripts/build-polygeist-dev.sh`** - 快速构建 Polygeist + Polymer
2. **`scripts/build-systolic-dev.sh`** - 快速构建 mlir-systolic

## 故障排除

### 问题 1: CMake 找不到 MLIR_DIR

确保 LLVM/MLIR 已构建，并设置正确的路径：
```bash
# 检查路径
ls -la /path/to/mlir/lib/cmake/mlir/MLIRConfig.cmake
```

### 问题 2: 找不到 Polymer 库

确保 Polygeist 已构建，并设置了 POLYGEIST_BUILD：
```bash
export POLYGEIST_BUILD=/home/user/work/mlir-systolic/third_party/Polygeist/build
ls -la ${POLYGEIST_BUILD}/lib/libPolymer*.a
```

### 问题 3: 链接错误

确保 mlir-systolic 的 CMake 配置正确找到了 Polymer 库：
```bash
cd build
cmake ..  # 重新配置，查看输出中的 Polymer 相关信息
```

## 与统一构建的对比

| 特性 | 统一构建 | 开发模式（分步构建） |
|------|---------|-------------------|
| 首次构建时间 | 30分钟-数小时 | 30分钟-数小时 |
| 后续构建时间 | 几分钟-数十分钟 | 几秒-几分钟 ⭐ |
| 内存占用 | 高 | 低 ⭐ |
| 适合场景 | 完整构建、CI/CD | 日常开发 ⭐ |
| 版本兼容性 | 最好 | 需要手动管理 |

**推荐：**
- **首次构建**：使用统一构建
- **日常开发**：使用开发模式（分步构建）


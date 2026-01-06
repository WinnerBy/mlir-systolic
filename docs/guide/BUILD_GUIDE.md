# mlir-systolic 构建指南

**最后更新**: 2026-01-06

---

## 目录

1. [概述](#概述)
2. [快速开始](#快速开始)
3. [详细步骤](#详细步骤)
4. [必需的库](#必需的库)
5. [脚本说明](#脚本说明)
6. [故障排除](#故障排除)

---

## 概述

本项目使用 **统一构建方式**（参考 [Polygeist README Option 2](../../third_party/Polygeist/README.md#option-2-using-unified-llvm-mlir-clang-and-polygeist-build)），一次性构建 LLVM/MLIR/Polly/Polygeist/Polymer 和 mlir-systolic。

### 构建流程

```
1. 初始化 Submodule (LLVM/MLIR/Polygeist)
   ↓
2. 构建 Polygeist + Polymer (统一构建)
   ↓
3. 构建 mlir-systolic (链接到 Polygeist 构建)
```

---

## 快速开始

```bash
# 1. 初始化 submodule
git submodule update --init --recursive

# 2. 构建 Polygeist + Polymer
./scripts/build-polygeist.sh

# 3. 构建 mlir-systolic
./scripts/build-systolic.sh

# 4. 验证
./build/bin/systolic-opt --help
./build/bin/systolic-translate --help
```

---

## 详细步骤

### 步骤 1: 初始化 Submodule

```bash
git submodule update --init --recursive
```

这会拉取：
- `third_party/Polygeist` - Polygeist 框架
- `third_party/Polygeist/llvm-project` - LLVM/MLIR 源码（Polygeist 的 submodule）

---

### 步骤 2: 构建 Polygeist + Polymer

#### 使用脚本（推荐）

```bash
./scripts/build-polygeist.sh
```

**脚本功能**:
- ✅ 自动检测系统内存并选择合适的并行度（防止 WSL 卡死）
- ✅ 使用统一构建方式（参考 Polygeist README Option 2）
- ✅ 构建 Polymer 库和 mlir-systolic 需要的 MLIR 库

#### 手动构建（高级用户）

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

# 构建必需的库
# ⚠️ 重要：根据内存选择并行度
# 内存 < 8GB: 使用 -j1（单线程，最安全）
# 内存 8-16GB: 使用 -j2（推荐）
# 内存 > 16GB: 可以使用 -j4 或更多
ninja -j2 PolymerSupport PolymerTargetISL PolymerTransforms \
  MLIRArithTransforms MLIRArithValueBoundsOpInterfaceImpl MLIROptLib
```

**关键配置说明**:
| 配置项 | 说明 |
|--------|------|
| `-DLLVM_ENABLE_PROJECTS="clang;mlir;polly"` | 包含 polly（ISL 依赖） |
| `-DLLVM_EXTERNAL_PROJECTS="polygeist"` | 将 Polygeist 作为外部项目 |
| `-DPOLYGEIST_ENABLE_POLYMER=1` | 启用 Polymer |
| `-DPOLYGEIST_POLYMER_ENABLE_ISL=1` | 使用 ISL（需要 polly） |

#### 验证 Polygeist 构建

构建完成后，检查库文件：

```bash
ls third_party/Polygeist/build/lib/libPolymer*.a
ls third_party/Polygeist/build/lib/libMLIRArith*.a
```

应该看到：
- ✅ `libPolymerSupport.a`
- ✅ `libPolymerTargetISL.a`
- ✅ `libPolymerTransforms.a` ← **最关键**
- ✅ `libMLIRArithTransforms.a`
- ✅ `libMLIRArithValueBoundsOpInterfaceImpl.a`
- ✅ `libMLIROptLib.a`

---

### 步骤 3: 构建 mlir-systolic

#### 使用脚本（推荐）

```bash
./scripts/build-systolic.sh
```

**脚本功能**:
- ✅ 自动检测 Polygeist 构建目录
- ✅ 自动配置 CMake
- ✅ 自动检测系统内存并选择合适的并行度

#### 手动构建（高级用户）

```bash
mkdir -p build && cd build

cmake .. \
  -DMLIR_DIR=../third_party/Polygeist/build/lib/cmake/mlir \
  -DLLVM_DIR=../third_party/Polygeist/build/lib/cmake/llvm \
  -DPOLYGEIST_BUILD=../third_party/Polygeist/build

# ⚠️ 重要：根据内存选择并行度
cmake --build . -j2
```

#### 验证 mlir-systolic 构建

构建完成后，检查工具：

```bash
./build/bin/systolic-opt --help
./build/bin/systolic-translate --help
```

成功的输出应包含可用的 passes 列表，如：
- `--systolic-transform`
- `--array-partitioning`
- `--write-time-reordering`

---

## 必需的库

### Polymer 库

根据项目代码分析，需要构建以下 **3 个 Polymer 库**：

#### 1. PolymerSupport ✅
- **库文件**: `libPolymerSupport.a`
- **用途**: 
  - 提供 `IslScop` 类（SCoP 的核心数据结构）
  - 提供 `ScopStmt` 等基础支持
- **使用位置**: `lib/Analysis/` (SystolicAnalysis)

#### 2. PolymerTargetISL ✅
- **库文件**: `libPolymerTargetISL.a`
- **用途**:
  - 提供 `createIslFromFuncOp()` 函数（SCoP 提取）
  - 提供 ISL 相关的转换功能
- **使用位置**: `lib/Analysis/` (SystolicAnalysis)

#### 3. PolymerTransforms ✅ ⭐ 最关键
- **库文件**: `libPolymerTransforms.a`
- **用途**:
  - 提供 `ExtractScopStmt` Pass（将 affine.for 转换为 scop.stmt 格式）
  - 提供 `createExtractScopStmtPass()` 函数
  - **这是 SystolicTransform Pass 运行 ExtractScopStmt 所必需的**
- **使用位置**: 
  - `lib/Analysis/` (SystolicAnalysis)
  - `lib/Transforms/` (SystolicTransforms) - **必需**

### MLIR 库

项目还需要以下 MLIR 库：
- `MLIRArithTransforms`
- `MLIRArithValueBoundsOpInterfaceImpl`
- `MLIROptLib`

### 依赖关系

```
PolymerTransforms
  ├── PolymerSupport (自动依赖)
  └── PolymerTargetISL (自动依赖)

💡 构建 PolymerTransforms 会自动构建其他两个库
```

### 最小构建命令

```bash
cd third_party/Polygeist/build
ninja PolymerTransforms MLIRArithTransforms MLIRArithValueBoundsOpInterfaceImpl MLIROptLib
```

---

## 脚本说明

### 构建脚本

#### `build-systolic.sh`
**用途**: 构建 mlir-systolic 项目

**使用方法**:
```bash
./scripts/build-systolic.sh
```

**功能**:
- 配置 CMake
- 编译项目
- 安装到指定目录

---

#### `build-polygeist.sh`
**用途**: 构建 Polygeist + Polymer + 依赖库

**使用方法**:
```bash
./scripts/build-polygeist.sh
```

**功能**:
- 下载和构建 Polygeist
- 配置并构建 Polymer
- 构建所有必需的 MLIR 库
- 配置为 mlir-systolic 的依赖

---

### AutoSA 相关脚本

#### `generate_autosa_tests.sh`
**用途**: 生成 AutoSA 参考 samples

**功能**:
- 生成不同 spacetime 值的矩阵乘法（MM）测试用例（0, 1, 2, 3, 4, 5）
- 生成其他 kernel 类型的测试用例（CNN, MTTKRP, TTMc, TTM, LU 等）
- 自动创建 AutoSA 要求的目录结构
- 自动收集 HLS 文件到 `collected_hls_files/` 目录

**使用方法**:
```bash
# 设置环境变量（可选）
export AUTOSA_ROOT=/home/user/work/AutoSA
export OUTPUT_DIR=./autosa_reference_samples

# 运行脚本
./scripts/generate_autosa_tests.sh
```

---

#### `organize_reference_samples.sh`
**用途**: 整理 AutoSA 生成的 samples 到 `test/reference-samples/` 目录

**功能**:
- 从 `autosa_reference_samples/collected_hls_files/` 提取 `kernel_kernel.cpp` 文件
- 重命名为符合命名规则的格式：`{kernel}_st{ST}_..._kernel.cpp`
- 排除无效配置（代码生成失败或参数不合理）
- 自动跳过空文件

---

### 测试脚本

#### `compare_effective_code.sh`
**用途**: 比较有效代码（去除空白和注释）

#### `detailed_code_diff.sh`
**用途**: 生成详细的代码差异报告

#### `check_random_access.sh`
**用途**: 检查代码中的随机访问模式

#### `verify_effective_code.sh`
**用途**: 验证有效代码的正确性

---

## 故障排除

### 问题 1: CMake 找不到 MLIR_DIR

**症状**:
```
Could not find a package configuration file provided by "MLIR"
```

**解决方案**:
确保 Polygeist 已构建完成，并且路径正确：
```bash
ls third_party/Polygeist/build/lib/cmake/mlir/MLIRConfig.cmake
```

如果文件不存在，重新运行：
```bash
./scripts/build-polygeist.sh
```

---

### 问题 2: 找不到 Polymer 库

**症状**:
```
Required Polymer libraries not found: libPolymerTransforms.a
```

**解决方案**:
确保构建了 Polymer 库：
```bash
ls third_party/Polygeist/build/lib/libPolymer*.a
```

如果库不存在，手动构建：
```bash
cd third_party/Polygeist/build
ninja PolymerTransforms
```

或重新运行 `./scripts/build-polygeist.sh`，并确保：
- `-DPOLYGEIST_ENABLE_POLYMER=1` 已设置
- `-DPOLYGEIST_POLYMER_ENABLE_ISL=1` 已设置
- `polly` 包含在 `LLVM_ENABLE_PROJECTS` 中

---

### 问题 3: 构建时 WSL 卡死

**症状**:
WSL 在构建过程中变得无响应。

**解决方案**:

1. **使用更少的并行任务**:
   ```bash
   ninja -j1  # 单线程，最安全
   ```

2. **使用 Release 模式**（已在脚本中设置）:
   ```bash
   -DCMAKE_BUILD_TYPE=Release
   -DLLVM_ENABLE_ASSERTIONS=OFF
   ```

3. **检查系统内存**:
   ```bash
   free -h
   ```
   
4. **分阶段构建**:
   ```bash
   # 先构建 Polymer 库
   ninja -j1 PolymerTransforms
   
   # 再构建 MLIR 库
   ninja -j1 MLIRArithTransforms MLIRArithValueBoundsOpInterfaceImpl MLIROptLib
   ```

---

### 问题 4: 找不到 MLIR 库（如 MLIRArithTransforms）

**症状**:
```
No rule to make target 'libMLIRArithTransforms.a'
```

**解决方案**:
在构建 Polygeist 时，确保构建了所有需要的 MLIR 库：
```bash
cd third_party/Polygeist/build
ninja -j2 MLIRArithTransforms MLIRArithValueBoundsOpInterfaceImpl MLIROptLib
```

或者重新运行：
```bash
./scripts/build-polygeist.sh
```

---

### 问题 5: ISL headers 警告

**症状**:
```
CMake Warning: ISL headers not found. Polymer may not compile correctly.
```

**解决方案**:
这个警告通常可以忽略。如果编译失败，确保 `polly` 包含在 `LLVM_ENABLE_PROJECTS` 中：
```bash
-DLLVM_ENABLE_PROJECTS="clang;mlir;polly"
```

---

### 问题 6: Git submodule 更新失败

**症状**:
```
fatal: No url found for submodule path '...'
```

**解决方案**:
```bash
# 同步 submodule 配置
git submodule sync --recursive

# 重新更新
git submodule update --init --recursive
```

---

## 参考资源

- [Polygeist README](../../third_party/Polygeist/README.md) - Polygeist 构建说明
- [Polymer README](../../third_party/Polygeist/tools/polymer/README.md) - Polymer 使用说明
- [LLVM CMake 文档](https://llvm.org/docs/CMake.html) - LLVM 构建配置

---

## 构建时间参考

| 组件 | 单线程 (-j1) | 双线程 (-j2) | 四线程 (-j4) |
|------|-------------|-------------|-------------|
| Polygeist + Polymer | ~3-4 小时 | ~2-2.5 小时 | ~1-1.5 小时 |
| mlir-systolic | ~5-10 分钟 | ~3-5 分钟 | ~2-3 分钟 |

💡 **建议**: 首次构建使用 `-j2`，后续增量构建可以使用更高并行度。

---

**最后更新**: 2026-01-06

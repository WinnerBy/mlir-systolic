# Scripts 说明文档

> **最后更新**: 2024-12  
> **目的**: 说明 `scripts/` 目录下各个脚本的用途和使用方法

---

## 脚本列表

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
**用途**: 构建 Polygeist（MLIR 的 C/C++ 前端）

**使用方法**:
```bash
./scripts/build-polygeist.sh
```

**功能**:
- 下载和构建 Polygeist
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

**输出**:
- 每个测试用例在 `OUTPUT_DIR/` 下创建独立目录
- 所有 HLS C++ 文件收集到 `OUTPUT_DIR/collected_hls_files/` 目录
- 生成的代码自包含头文件信息（已删除 `--hls` 参数）

**生成的 Samples**:
- 矩阵乘法：不同 spacetime、不同问题规模（I32, I64, I128）、不同参数组合
- 其他 kernel：CNN, MTTKRP, TTMc, TTM, LU

---

#### `organize_reference_samples.sh`
**用途**: 整理 AutoSA 生成的 samples 到 `test/reference-samples/` 目录

**功能**:
- 从 `autosa_reference_samples/collected_hls_files/` 提取 `kernel_kernel.cpp` 文件
- 重命名为符合命名规则的格式：`{kernel}_st{ST}_..._kernel.cpp`
- 排除无效配置（代码生成失败或参数不合理）
- 自动跳过空文件

**使用方法**:
```bash
./scripts/organize_reference_samples.sh
```

**输出**:
- 整理后的 samples 保存到 `test/reference-samples/`
- 每个文件都是自包含的 HLS C++ 代码，包含必要的头文件

**排除的配置**:
- `mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8` - array part 比问题规模大
- `mm_st3_I64_J64_K64_ap32_lat11_32_simd64` - 代码生成失败
- `dnn_ops_st4_ap8_8_4_8_lat4_4_4_simd1_1_1_2` - 代码生成失败

---

#### `check_random_access.sh`
**用途**: 检查生成的代码中的随机读取问题

**功能**:
- 检查 `local_*[...][...][...]` 访问中的非线性索引
- 查找包含 `*`, `/`, `%` 的索引表达式
- 识别可能导致性能问题的访问模式

**使用方法**:
```bash
# 检查单个文件
./scripts/check_random_access.sh /path/to/kernel.cpp

# 检查目录
./scripts/check_random_access.sh /path/to/autosa/output/src

# 详细分析 MTTKRP
./scripts/check_random_access.sh /path/to/mttkrp_kernel.cpp --analyze-mttkrp
```

**问题特征**:
- 非线性索引表达式：`4 * c6 / 16`, `c6 / 4`, `c6 % 4`
- 出现在 `IO_L2_in_intra_trans` 函数中
- 可能导致非顺序访问模式，影响 HLS 性能

---

## 工作流程

### 生成和整理 Reference Samples

```bash
# 1. 生成 AutoSA samples
./scripts/generate_autosa_tests.sh

# 2. 整理 samples 到 test/reference-samples/
./scripts/organize_reference_samples.sh

# 3. 检查随机读取问题（可选）
./scripts/check_random_access.sh test/reference-samples/mttkrp_*.cpp
```

### 构建项目

```bash
# 1. 构建 Polygeist（如果需要）
./scripts/build-polygeist.sh

# 2. 构建 mlir-systolic
./scripts/build-systolic.sh
```

---

## 相关文档

- **Reference Samples 说明**: `test/reference-samples/README.md`
- **AutoSA 分析**: `docs/AUTOSA_ANALYSIS.md`
- **随机读取问题**: `docs/RANDOM_ACCESS_ISSUE_ANALYSIS.md`


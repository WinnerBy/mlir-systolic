# Scripts 使用指南

本目录包含项目的各类脚本，用于构建、生成参考代码和维护工作。

---

## 📋 脚本概览

| 脚本 | 类型 | 功能 | 状态 |
|------|------|------|------|
| `build-polygeist.sh` | 构建 | 构建 Polygeist + Polymer（LLVM/MLIR依赖） | ✅ 核心 |
| `build-systolic.sh` | 构建 | 构建 mlir-systolic 项目 | ✅ 核心 |
| `generate_autosa_reference.py` | 生成 | 生成 AutoSA HLS C 参考代码库 | ✅ 核心 |
| `summarize_autosa_refs.py` | 工具 | 从参考代码生成参数统计表格 | ✅ 核心 |

---

## 🔨 构建脚本

### build-polygeist.sh

**功能**：一次性构建 LLVM/MLIR/Polly/Polygeist/Polymer

**使用**：
```bash
./scripts/build-polygeist.sh
```

**说明**：
- 使用 Polygeist 的统一构建方式（Option 2）
- 自动检测系统内存，选择合适的并行度
- 构建完成后生成 Polymer 库和必要的 MLIR 库
- 输出目录：`third_party/Polygeist/build/`

**手动构建**（如脚本不适用）：
```bash
cd third_party/Polygeist && mkdir -p build && cd build

cmake -G Ninja ../llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1

ninja -j2 PolymerSupport PolymerTargetISL PolymerTransforms \
  MLIRArithTransforms MLIRArithValueBoundsOpInterfaceImpl MLIROptLib
```

---

### build-systolic.sh

**功能**：构建 mlir-systolic 项目

**使用**：
```bash
./scripts/build-systolic.sh
```

**说明**：
- 在项目根目录构建 `build/` 目录
- 自动检测 Polygeist 构建目录
- 必须先完成 Polygeist 构建

**手动构建**：
```bash
mkdir -p build && cd build

cmake .. \
  -DMLIR_DIR=../third_party/Polygeist/build/lib/cmake/mlir \
  -DLLVM_DIR=../third_party/Polygeist/build/lib/cmake/llvm \
  -DPOLYGEIST_BUILD=../third_party/Polygeist/build

cmake --build . -j2
```

**验证**：
```bash
./build/bin/systolic-opt --help
./build/bin/systolic-translate --help
```

---

## 🎯 生成参考代码脚本

### generate_autosa_reference.py

**功能**：自动化生成各种 AutoSA 内核的 HLS C 参考代码

**基本用法**：

#### 1. 仅生成指定 spacetime（不含参数）
```bash
python3 scripts/generate_autosa_reference.py --kernels mm ttmc
```

#### 2. 带参数生成（推荐）
```bash
python3 scripts/generate_autosa_reference.py --kernels mm ttmc mttkrp lu cnn --with-params
```

#### 3. 生成所有内核
```bash
python3 scripts/generate_autosa_reference.py --with-params
```

**选项说明**：
- `--kernels <kernel1> <kernel2> ...` - 指定要生成的内核（mm, ttmc, mttkrp, lu, cnn）
- `--with-params` - 使用预定义参数组合生成完整代码（包括 array_part、latency、simd）
- `--autosa-root <path>` - 指定 AutoSA 根目录（默认：`third_party/AutoSA`）

**脚本特性**：

1. **自动交互处理**：向 AutoSA 自动输入 `y` 确认归约循环提示
2. **simd_info 支持**：自动附加 `--simd-info` 参数以减少交互提示
3. **host-serialize**：默认启用，改善 DRAM burst 长度
4. **命令归档**：生成的每个 HLS C 文件头包含完整的 AutoSA 命令，便于复现
5. **增量生成**：存在的文件自动跳过，支持中途中断后重新运行

**预定义参数组合**：

| 内核 | Spacetime | 参数组合数 | 总文件数 |
|------|-----------|----------|--------|
| MM | 0-5 | 3-4 个/spacetime | ~18 个 |
| TTMC | 0-9 | 2-3 个/spacetime | ~29 个 |
| MTTKRP | 0-5 | 2-4 个/spacetime | ~22 个 |
| LU | 0-5 | 1-2 个/spacetime | ~8 个 |
| CNN | 0-7 | 2-3 个/spacetime | ~20 个 |

**完整参数示例**：

**TTMC**：
```
spacetime: 0-9
array_part: [16,64,16,32], [32,32,32,32], [48,48,48,48]
latency: [1,8,8], [2,4,4], [4,2,2]
simd: [4,1], [2,2], [1,4]
```

**MTTKRP**：
```
spacetime: 0-5
array_part: [64,64,2], [128,128,4], [96,96,3]
latency: [8,4], [16,8], [4,4]
simd: [4,1], [8,1], [2,2]
```

**输出结构**：

```
autosa_hls_refs/
├── mm/
│   ├── st0_ap16_lat8_simd4/
│   │   ├── kernel_kernel.cpp      ← HLS 内核代码（关键文件）
│   │   ├── kernel_host.cpp
│   │   ├── kernel_modules.cpp
│   │   └── ...
│   └── ...
├── ttmc/
│   ├── st0_ap32_lat2_simd2/
│   │   ├── kernel_kernel.cpp      ← HLS 内核代码（关键文件）
│   │   └── ...
│   └── ...
├── mttkrp/
├── lu/
├── cnn/
└── generation_summary.json         ← 元数据（用于参数统计）
```

**文件头示例**：

```cpp
// AutoSA Command:
// /workspaces/mlir-systolic/third_party/AutoSA/autosa ... \
//   --sa-sizes={kernel[]->space_time[0];kernel[]->array_part[16,64,16,32];...} \
//   --host-serialize --simd-info=...
#include <ap_int.h>
#include <hls_stream.h>
...
```

**已知限制**：

1. **某些 spacetime 不可行**：特定内核的某些 spacetime 配置可能不生成完整代码（取决于 AutoSA 兼容性）
2. **LU 无 simd_info**：LU 缺少 `simd_info.json`，可能仍有交互提示
3. **运行时间**：完整生成所有内核约需 10-20 分钟

**故障排除**：

| 问题 | 解决方案 |
|------|--------|
| `FileNotFoundError: ./src/autosa` | 确保正确的 `--autosa-root` 或从项目根目录运行 |
| `tuning.json 不存在` | 检查 AutoSA 执行是否出错（查看日志） |
| `kernel_kernel.cpp 未生成` | 该 spacetime/参数组合对该内核不可行，尝试其他组合 |

---

### summarize_autosa_refs.py

**功能**：从生成的 AutoSA 参考代码提取参数信息，生成统计表格

**基本用法**：

```bash
python3 scripts/summarize_autosa_refs.py
```

**输出**：

1. **Markdown 格式表格**（`docs/AUTOSA_REFERENCE_TABLES.md`）
   - 按 kernel 分组
   - 包含 spacetime、参数、文件路径等信息

2. **CSV 格式表格**（`docs/AUTOSA_REFERENCE_TABLES.csv`）
   - 平铺格式，便于数据分析
   - 包含所有元数据

**使用场景**：

- 查阅参考代码的参数配置
- 统计生成覆盖范围
- 用于论文或报告中的数据表格

**依赖**：

- 必须先运行 `generate_autosa_reference.py` 生成 `autosa_hls_refs/generation_summary.json`

---

## 🔄 工作流程

### 完整参考代码生成流程

```bash
# 1. 构建依赖（仅需一次）
./scripts/build-polygeist.sh
./scripts/build-systolic.sh

# 2. 生成所有参考代码
python3 scripts/generate_autosa_reference.py --with-params

# 3. 生成统计表格
python3 scripts/summarize_autosa_refs.py

# 4. 查看结果
cat docs/AUTOSA_REFERENCE_TABLES.md
```

### 增量生成流程

```bash
# 生成后新增了参数组合？重新运行生成脚本
python3 scripts/generate_autosa_reference.py --kernels mm ttmc --with-params

# 重新生成统计表格
python3 scripts/summarize_autosa_refs.py
```

---

## 📊 生成统计

**当前生成规模**（`--with-params` 模式）：

```
总参考代码数: 97 个 kernel_kernel.cpp
├── Matrix Multiply (MM)      : 18 个
├── Tensor Train Matrix Chain (TTMC): 29 个
├── MTTKRP                     : 22 个
├── LU Decomposition (LU)      : 8 个
└── Convolutional Neural Network (CNN): 20 个
```

---

## 📝 笔记

- **构建脚本** (`build-*.sh`)：通常只需运行一次，除非更新依赖
- **生成脚本** (`generate_autosa_reference.py`)：可多次运行，支持增量生成
- **工具脚本** (`summarize_autosa_refs.py`)：依赖生成脚本的输出

---

## 相关文档

- **生成详情**：[AutoSA 参考覆盖状态](../docs/reference/autosa/AUTOSA_REFERENCE_STATUS.md)
- **参数说明**：[AutoSA 参数表详细](../docs/reference/autosa/AUTOSA_REFERENCE_TABLES.md)
- **快速参考**：[AutoSA 快速查阅](../docs/reference/autosa/AUTOSA_QUICK_REFERENCE.md)
- **构建指南**：[项目构建指南](../docs/guide/BUILD_GUIDE.md)


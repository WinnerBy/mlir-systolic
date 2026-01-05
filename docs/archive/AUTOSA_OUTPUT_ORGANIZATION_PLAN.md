# AutoSA 输出代码整理计划

> **创建时间**: 2026-01-05  
> **目的**: 整理和验证 `autosa_hls_output/` 中的 AutoSA 生成代码，按照项目命名规范归档

---

## 工作概述

### 当前状态

1. **已修复问题**：
   - ✅ kernel0 函数重复定义问题已修复
   - ✅ 已按照 L2 级别混合方案重新实现数据重用

2. **待验证工作**：
   - ⚠️ 验证 `autosa_cmd.md` 中的 AutoSA 命令是否正确
   - ⚠️ 按照 `test/reference-samples/` 的命名规范整理代码
   - ⚠️ 验证生成的代码与命令参数是否匹配

3. **文件位置**：
   - 当前代码：`autosa_hls_output/` 目录
   - 目标位置：`test/reference-samples/` 目录
   - 命令记录：`autosa_hls_output/autosa_cmd.md`

---

## 文件清单与参数提取

### 1. kernel_cnn_large.cpp

**命令**：
```bash
./autosa ./autosa_tests/large/cnn/kernel.c \
  --config=./autosa_config/autosa_config.json \
  --target=autosa_hls_c \
  --output-dir=./autosa.tmp/output \
  --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[64,56,14,64];kernel[]->latency[4,4,7];kernel[]->simd[1,1,8]}" \
  --simd-info=./autosa_tests/large/cnn/simd_info.json
```

**参数提取**：
- Kernel 类型: `cnn` (large)
- Space-time: `4`
- Array part: `64,56,14,64` → `ap64_56_14_64`
- Latency: `4,4,7` → `lat4_4_7`
- SIMD: `1,1,8` → `simd1_1_8`
- 无 `--host-serialize` 参数

**预期文件名**：
```
cnn_large_st4_ap64_56_14_64_lat4_4_7_simd1_1_8_kernel.cpp
```

**验证要点**：
- [ ] 检查代码中是否有 SIMD 打包类型（`ap_uint<64>` 等）
- [ ] 如果没有打包类型，可能需要添加 `--host-serialize`
- [ ] 验证 array_part 参数数量（4个）是否与代码结构匹配

---

### 2. kernel_cnn_small.cpp

**命令**：
```bash
./autosa ./autosa_tests/cnn/kernel.c \
  --config=./autosa_config/autosa_config.json \
  --target=autosa_hls_c \
  --output-dir=./autosa.tmp/output \
  --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,2,4];kernel[]->simd[1,1,1,2]}" \
  --simd-info=./autosa_tests/cnn/simd_info.json \
  --host-serialize
```

**参数提取**：
- Kernel 类型: `cnn` (small)
- Space-time: `4`
- Array part: `8,8,4,8` → `ap8_8_4_8`
- Latency: `4,2,4` → `lat4_2_4`
- SIMD: `1,1,1,2` → `simd1_1_1_2`
- ✅ 有 `--host-serialize` 参数

**预期文件名**：
```
cnn_st4_ap8_8_4_8_lat4_2_4_simd1_1_1_2_kernel.cpp
```

**验证要点**：
- [ ] 检查代码中是否有 SIMD 打包类型（`ap_uint<64>` 对应 simd=2）
- [ ] 对比 `test/reference-samples/cnn_st4_ap8_8_4_8_lat4_2_4_simd1_1_1_2_kernel.cpp`（如果存在）
- [ ] 验证参数是否与现有参考样本一致

**注意**：此文件与 `test/reference-samples/` 中已有的 `cnn_st4_ap8_8_4_8_lat4_2_4_simd1_1_1_2_kernel.cpp` 应该一致

---

### 3. kernel_lu.cpp

**命令**：
```bash
./autosa ./autosa_tests/lu/kernel.c \
  --config=./autosa_config/autosa_config.json \
  --target=autosa_hls_c \
  --output-dir=./autosa.tmp/output \
  --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[-1,-1,-1];kernel[]->latency[]}" \
  --simd-info=./autosa_tests/lu/simd_info.json \
  --use-cplusplus-template \
  --no-reschedule \
  --live-range-reordering
```

**参数提取**：
- Kernel 类型: `lu`
- Space-time: `3`
- Array part: `-1,-1,-1` → **自动计算** → `ap_auto`
- Latency: `[]` → **自动计算** → `lat_auto`
- SIMD: 未指定（可能为默认值 1）
- ✅ 有 `--use-cplusplus-template`（使用 C++ 模板）
- ✅ 有 `--no-reschedule`
- ✅ 有 `--live-range-reordering`

**预期文件名**：
```
lu_st3_ap_auto_lat_auto_kernel.cpp
```

**验证要点**：
- [ ] 检查代码中是否使用了 C++ 模板（`template<int p0, int p1>`）
- [ ] 对比 `test/reference-samples/lu_st3_ap_auto_lat_auto_kernel.cpp`（如果存在）
- [ ] 验证 array_part 和 latency 是否真的是自动计算的
- [ ] 检查是否有 SIMD 参数（如果没有，可能需要添加 `simd1` 或省略）

**注意**：此文件与 `test/reference-samples/` 中已有的 `lu_st3_ap_auto_lat_auto_kernel.cpp` 应该一致

---

### 4. kernel_mttkrp.cpp

**命令**：
```bash
./autosa ./autosa_tests/large/mttkrp/kernel.c \
  --config=./autosa_config/autosa_config.json \
  --target=autosa_hls_c \
  --output-dir=./autosa.tmp/output \
  --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[128,128,2];kernel[]->latency[16,8];kernel[]->simd[8,1]}" \
  --simd-info=./autosa_tests/large/mttkrp/simd_info.json \
  --host-serialize
```

**参数提取**：
- Kernel 类型: `mttkrp` (large)
- Space-time: `3`
- Array part: `128,128,2` → `ap128_128_2`
- Latency: `16,8` → `lat16_8`
- SIMD: `8,1` → `simd8_1`
- ✅ 有 `--host-serialize` 参数

**预期文件名**：
```
mttkrp_st3_ap128_128_2_lat16_8_simd8_1_kernel.cpp
```

**验证要点**：
- [ ] 检查代码中是否有 SIMD 打包类型（`ap_uint<256>` 对应 simd=8）
- [ ] 对比 `test/reference-samples/mttkrp_st3_ap128_128_2_lat16_8_simd8_1_kernel.cpp`（如果存在）
- [ ] 验证参数是否与现有参考样本一致

**注意**：此文件与 `test/reference-samples/` 中已有的 `mttkrp_st3_ap128_128_2_lat16_8_simd8_1_kernel.cpp` 应该一致

---

### 5. kernel_ttmc.cpp

**命令**：
```bash
./autosa ./autosa_tests/large/ttmc/kernel.c \
  --config=./autosa_config/autosa_config.json \
  --target=autosa_hls_c \
  --output-dir=./autosa.tmp/output \
  --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[16,64,16,32];kernel[]->latency[1,8,8];kernel[]->simd[8,1]}" \
  --simd-info=./autosa_tests/large/ttmc/simd_info.json \
  --host-serialize
```

**参数提取**：
- Kernel 类型: `ttmc` (large)
- Space-time: `4`
- Array part: `16,64,16,32` → `ap16_64_16_32`
- Latency: `1,8,8` → `lat1_8_8`
- SIMD: `8,1` → `simd8_1`
- ✅ 有 `--host-serialize` 参数

**预期文件名**：
```
ttmc_st4_ap16_64_16_32_lat1_8_8_simd8_1_kernel.cpp
```

**验证要点**：
- [ ] 检查代码中是否有 SIMD 打包类型（`ap_uint<256>` 对应 simd=8）
- [ ] 对比 `test/reference-samples/ttmc_st4_ap16_64_16_32_lat1_8_8_simd8_1_kernel.cpp`（如果存在）
- [ ] 验证参数是否与现有参考样本一致

**注意**：此文件与 `test/reference-samples/` 中已有的 `ttmc_st4_ap16_64_16_32_lat1_8_8_simd8_1_kernel.cpp` 应该一致

---

## 验证步骤

### 步骤 1: 验证命令参数正确性

对于每个文件，需要验证：

1. **参数数量匹配**：
   - `array_part` 参数数量应该与 kernel 的维度数匹配
   - `latency` 参数数量应该比 `array_part` 少 1（对应空间维度）
   - `simd` 参数数量应该与 `array_part` 匹配

2. **参数值合理性**：
   - `array_part` 应该 ≤ 问题规模
   - `latency` 应该 > 0
   - `simd` 应该是 2 的幂（1, 2, 4, 8, 16, ...）

3. **特殊参数**：
   - `--host-serialize`：当 `simd > 1` 时通常需要
   - `--use-cplusplus-template`：LU 分解等特殊情况需要

### 步骤 2: 验证代码与参数匹配

对于每个文件，检查：

1. **类型定义**：
   - 如果 `simd=2`，应该有 `ap_uint<64>` 类型
   - 如果 `simd=4`，应该有 `ap_uint<128>` 类型
   - 如果 `simd=8`，应该有 `ap_uint<256>` 类型

2. **循环边界**：
   - 检查 `serialize` 函数的循环边界是否与 `array_part` 匹配
   - 检查 PE 函数的循环结构是否与参数匹配

3. **PE 阵列大小**：
   - 验证 `numPE = array_part / latency` 是否与代码中的 PE 数量匹配

### 步骤 3: 对比现有参考样本

对于每个文件：

1. **检查是否已存在**：
   - 在 `test/reference-samples/` 中查找同名文件
   - 如果存在，进行 diff 对比

2. **如果不存在**：
   - 按照命名规范重命名
   - 移动到 `test/reference-samples/` 目录

3. **如果存在但不同**：
   - 记录差异
   - 确定哪个版本是正确的
   - 可能需要重新生成

---

## 命名规范总结

根据 `test/reference-samples/README.md`，命名规则为：

### 矩阵乘法 (MM)
```
mm_st{ST}_I{I}_J{J}_K{K}_ap{AP}_lat{LAT}_simd{SIMD}_kernel.cpp
```

### 其他 Kernel
```
{kernel_type}_st{ST}_ap{AP}_lat{LAT}_simd{SIMD}_kernel.cpp
```

**参数格式**：
- 多个值用下划线连接：`ap8_8_4_8`
- 自动计算用 `_auto`：`ap_auto`
- 如果只有一个值，直接写：`ap8`

**特殊情况**：
- `cnn_large` vs `cnn`：需要区分大小版本
- `mttkrp` vs `ttmc`：不同的 kernel 类型

---

## 具体执行计划

### 阶段 1: 验证命令（优先级：高）

1. **验证 kernel_cnn_large.cpp**
   - [ ] 检查是否需要 `--host-serialize`
   - [ ] 验证参数数量是否正确
   - [ ] 检查代码中的类型定义

2. **验证其他文件**
   - [ ] 逐个验证命令参数
   - [ ] 对比现有参考样本（如果存在）

### 阶段 2: 修正命令（如果需要）

1. **重新生成代码**（如果命令有误）
   - [ ] 修正 `autosa_cmd.md` 中的命令
   - [ ] 重新运行 AutoSA 生成代码
   - [ ] 验证生成的代码

### 阶段 3: 归档整理（优先级：中）

1. **重命名文件**
   - [ ] 按照命名规范重命名所有文件
   - [ ] 更新 `autosa_cmd.md` 记录正确的命令

2. **移动到目标目录**
   - [ ] 将文件移动到 `test/reference-samples/`
   - [ ] 更新 `test/reference-samples/README.md`

3. **创建命令记录**
   - [ ] 在 `docs/` 中创建 `AUTOSA_GENERATION_COMMANDS.md`
   - [ ] 记录所有正确的生成命令

### 阶段 4: 验证 L2 数据重用实现（优先级：高）

1. **代码验证**
   - [ ] 检查 L2 级别的数据重用逻辑是否正确实现
   - [ ] 验证循环边界和数据流是否匹配
   - [ ] 对比 AutoSA 参考代码的实现方式

2. **功能测试**
   - [ ] 运行功能测试验证正确性
   - [ ] 检查是否有数据流不匹配问题

---

## 参考资源

### AutoSA 文档
- `docs/AUTOSA_ANALYSIS.md` - AutoSA 详细分析
- `docs/AUTOSA_ARCHITECTURE.md` - AutoSA 架构说明
- `docs/AUTOSA_SPACETIME_ANALYSIS.md` - Space-time 分析

### 数据重用相关
- `docs/ST3_DETAILED_CODE_ANALYSIS.md` - ST3 详细代码分析（包含数据重用分析）
- `docs/DATA_REUSE_STRATEGY.md` - 数据重用策略分析

### 测试相关
- `test/reference-samples/README.md` - 参考样本说明
- `docs/ST3_FUNCTION_DIFF_ANALYSIS.md` - 函数差异分析

---

## 问题记录

### 待解决问题

1. **kernel_cnn_large.cpp 缺少 --host-serialize**
   - 问题：命令中没有 `--host-serialize`，但 `simd[1,1,8]` 中有 simd=8
   - 需要验证：代码中是否有对应的打包类型

2. **参数数量验证**
   - 需要确认：`array_part`、`latency`、`simd` 的参数数量是否与 kernel 维度匹配
   - 参考：AutoSA 文档中的参数说明

3. **LU kernel 的特殊参数**
   - `--use-cplusplus-template`、`--no-reschedule`、`--live-range-reordering`
   - 需要确认：这些参数是否影响生成的代码结构

---

## 更新日志

- **2026-01-05**: 创建整理计划文档
  - 提取所有命令参数
  - 生成预期文件名
  - 制定验证和归档计划


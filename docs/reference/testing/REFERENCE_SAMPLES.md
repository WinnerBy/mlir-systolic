# Reference Samples 说明

> **最后更新**: 2024-12  
> **目的**: 说明 `test/reference-samples/` 目录中的 AutoSA 参考 samples

---

## 概述

`test/reference-samples/` 目录包含使用 AutoSA 生成的 HLS C++ kernel 代码，用于与 mlir-systolic 生成的代码进行对比验证。

**特点**:
- ✅ 所有文件都是**自包含**的，包含必要的头文件信息
- ✅ 已排除无效配置（代码生成失败或参数不合理）
- ✅ 所有文件都经过验证，包含有效内容

---

## Samples 统计

**矩阵乘法 (MM)**: 17 个
- Space-time=3: 12 个（不同问题规模和参数组合）
- 其他 Space-time: 5 个（st0, st1, st2, st4, st5）

**其他 Kernel**: 5 个
- CNN, MTTKRP, TTM, TTMc, LU

**总计**: 22 个有效的 samples

---

## 排除的配置

以下配置已被排除：
- `mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8` - array part 比问题规模大
- `mm_st3_I64_J64_K64_ap32_lat11_32_simd64` - 代码生成失败
- `dnn_ops_st4_ap8_8_4_8_lat4_4_4_simd1_1_1_2` - 代码生成失败

---

## 生成和整理

**生成 samples**:
```bash
./scripts/generate_autosa_tests.sh
```

**整理 samples**:
```bash
./scripts/organize_reference_samples.sh
```

详细说明请参考: `docs/SCRIPTS.md`

---

## 相关文档

- **详细说明**: `test/reference-samples/README.md`
- **Scripts 说明**: `docs/SCRIPTS.md`
- **参数配置**: `docs/AUTOSA_TEST_GENERATION_GUIDE.md`


#!/bin/bash
# compare_with_autosa.sh - 详细对比 mlir-systolic 和 AutoSA 生成的代码

MLIR_FILE="/home/user/work/mlir-systolic/test/mm_32x32_kernel_v2.cpp"
AUTOSA_FILE="/home/user/work/scalehls/test/Transforms/Dataflow/AutoSA/autosa-reference-samples/mm_st3_I32_J32_K32_ap8_lat4_simd1_kernel.cpp"

echo "=============================================="
echo "mlir-systolic vs AutoSA HLS C++ 代码对比"
echo "=============================================="
echo ""

# 文件行数
echo "=== 文件行数 ==="
echo -n "mlir-systolic: "
wc -l < "$MLIR_FILE"
echo -n "AutoSA:        "
wc -l < "$AUTOSA_FILE"
echo ""

# 数据类型对比
echo "=== 数据类型定义 ==="
echo "--- mlir-systolic ---"
grep "typedef" "$MLIR_FILE"
echo ""
echo "--- AutoSA ---"
grep "typedef" "$AUTOSA_FILE"
echo ""

# PE 模块对比
echo "=== PE 模块对比 (核心计算) ==="
echo "--- mlir-systolic ---"
grep -A 30 "void PE.int idx" "$MLIR_FILE" | head -35
echo ""
echo "--- AutoSA ---"
grep -A 30 "void PE.int idx" "$AUTOSA_FILE" | head -35
echo ""

# 循环边界对比
echo "=== 循环边界对比 ==="
echo "--- mlir-systolic ---"
grep "c0 <= \|c1 <= \|c2 <= \|c5 <= \|c6 <= \|c7 <= " "$MLIR_FILE" | sort | uniq
echo ""
echo "--- AutoSA ---"
grep "c0 <= \|c1 <= \|c2 <= \|c5 <= \|c6 <= \|c7 <= " "$AUTOSA_FILE" | sort | uniq
echo ""

# HLS Pragma 对比
echo "=== HLS Pragma 统计 ==="
echo "mlir-systolic:"
echo -n "  PIPELINE II=1: "
grep -c "PIPELINE II=1" "$MLIR_FILE"
echo -n "  INLINE OFF:    "
grep -c "INLINE OFF" "$MLIR_FILE"
echo -n "  DATAFLOW:      "
grep -c "DATAFLOW" "$MLIR_FILE"
echo -n "  FIFO_SRL:      "
grep -c "FIFO_SRL" "$MLIR_FILE"
echo ""
echo "AutoSA:"
echo -n "  PIPELINE II=1: "
grep -c "PIPELINE II=1" "$AUTOSA_FILE"
echo -n "  INLINE OFF:    "
grep -c "INLINE OFF" "$AUTOSA_FILE"
echo -n "  DATAFLOW:      "
grep -c "DATAFLOW" "$AUTOSA_FILE"
echo -n "  FIFO_SRL:      "
grep -c "FIFO_SRL" "$AUTOSA_FILE"
echo ""

# 模块统计
echo "=== 模块统计 ==="
echo "mlir-systolic modules:"
grep "^void " "$MLIR_FILE" | wc -l
grep "^void " "$MLIR_FILE"
echo ""
echo "AutoSA modules:"
grep "^void " "$AUTOSA_FILE" | wc -l
grep "^void " "$AUTOSA_FILE"
echo ""

# kernel0 接口对比
echo "=== kernel0 接口 Pragma ==="
echo "--- mlir-systolic ---"
grep "pragma HLS INTERFACE" "$MLIR_FILE"
echo ""
echo "--- AutoSA ---"
grep "pragma HLS INTERFACE" "$AUTOSA_FILE"
echo ""

echo "=============================================="
echo "对比完成"
echo "=============================================="

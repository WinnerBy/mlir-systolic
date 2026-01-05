#!/bin/bash
# 详细对比两个HLS C文件的每一行代码

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REF_FILE="$PROJECT_ROOT/test/reference-samples/mm_st3_I32_J32_K32_ap8_lat4_simd1_kernel.cpp"
GEN_FILE="$PROJECT_ROOT/test/output/test_I32_J32_K32_ap8_lat4_simd1_kernel.cpp"

if [ ! -f "$REF_FILE" ] || [ ! -f "$GEN_FILE" ]; then
    echo -e "${RED}错误: 文件不存在${NC}"
    exit 1
fi

echo "=========================================="
echo "详细代码差异分析"
echo "=========================================="
echo ""
echo "参考文件: $(basename $REF_FILE)"
echo "生成文件: $(basename $GEN_FILE)"
echo ""

# 1. 基本统计
echo "=== 1. 基本统计 ==="
ref_lines=$(wc -l < "$REF_FILE")
gen_lines=$(wc -l < "$GEN_FILE")
echo "参考代码行数: $ref_lines"
echo "生成代码行数: $gen_lines"
echo "差异: $((gen_lines - ref_lines)) 行"
echo ""

# 2. 去除注释和空行后的有效代码行数
echo "=== 2. 有效代码行数（去除注释和空行）==="
ref_effective=$(grep -v '^[[:space:]]*//' "$REF_FILE" | grep -v '^[[:space:]]*/\*' | grep -v '^[[:space:]]*\*' | grep -v '^[[:space:]]*$' | wc -l)
gen_effective=$(grep -v '^[[:space:]]*//' "$GEN_FILE" | grep -v '^[[:space:]]*/\*' | grep -v '^[[:space:]]*\*' | grep -v '^[[:space:]]*$' | wc -l)
echo "参考代码有效行数: $ref_effective"
echo "生成代码有效行数: $gen_effective"
echo "差异: $((gen_effective - ref_effective)) 行"
echo ""

# 3. 关键HLS指令统计
echo "=== 3. 关键HLS指令统计 ==="
for pragma in "PIPELINE" "ARRAY_PARTITION" "RESOURCE" "STREAM" "INTERFACE" "DATAFLOW" "INLINE"; do
    ref_count=$(grep -c "#pragma HLS $pragma" "$REF_FILE" 2>/dev/null || echo "0")
    gen_count=$(grep -c "#pragma HLS $pragma" "$GEN_FILE" 2>/dev/null || echo "0")
    if [ "$ref_count" != "$gen_count" ]; then
        echo -e "${YELLOW}$pragma: 参考=$ref_count, 生成=$gen_count${NC}"
    else
        echo "$pragma: $ref_count (一致)"
    fi
done
echo ""

# 4. 函数定义对比
echo "=== 4. 函数定义对比 ==="
ref_funcs=$(grep -c "^void " "$REF_FILE" || echo "0")
gen_funcs=$(grep -c "^void " "$GEN_FILE" || echo "0")
echo "参考代码函数数: $ref_funcs"
echo "生成代码函数数: $gen_funcs"
echo ""

# 5. 数据类型定义对比
echo "=== 5. 数据类型定义对比 ==="
echo "参考代码的类型定义:"
grep -E "^typedef|^#define" "$REF_FILE" | head -10
echo ""
echo "生成代码的类型定义:"
grep -E "^typedef|^#define" "$GEN_FILE" | head -10
echo ""

# 6. 使用diff进行逐行对比（忽略空白和注释）
echo "=== 6. 代码结构差异（前20个差异）==="
diff -u "$REF_FILE" "$GEN_FILE" | head -100
echo ""

# 7. 检查kernel0函数的差异
echo "=== 7. kernel0函数调用顺序对比 ==="
echo "参考代码的kernel0函数调用:"
sed -n '/^void kernel0(/,/^}$/p' "$REF_FILE" | grep -E "^\s*[A-Z_]+\(|^\s*/\* Module Call" | head -30
echo ""
echo "生成代码的kernel0函数调用:"
sed -n '/^void kernel0(/,/^}$/p' "$GEN_FILE" | grep -E "^\s*[A-Z_]+\(|^\s*/\* Module Call" | head -30
echo ""


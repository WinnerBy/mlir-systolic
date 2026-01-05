#!/bin/bash
# 对比生成代码和参考代码的有效代码部分（忽略注释、空行、格式）

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# 配置
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REF_DIR="$PROJECT_ROOT/test/reference-samples"
OUTPUT_DIR="$PROJECT_ROOT/test/output"

# 清理函数：移除注释、空行、多余空格，保留有效代码结构
clean_code() {
    local file=$1
    # 使用更精确的方法清理代码
    # 1. 移除单行注释（// 开头的，但保留字符串中的）
    # 2. 移除多行注释（/* ... */）
    # 3. 移除空行
    # 4. 规范化空格和换行
    # 5. 移除多余的空格
    python3 -c "
import re
import sys

with open('$file', 'r') as f:
    content = f.read()

# 移除多行注释
content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)

# 移除单行注释（但保留字符串中的）
lines = []
in_string = False
for line in content.split('\n'):
    new_line = ''
    i = 0
    while i < len(line):
        if line[i] == '\"' and (i == 0 or line[i-1] != '\\\\'):
            in_string = not in_string
            new_line += line[i]
        elif not in_string and line[i:i+2] == '//':
            break
        else:
            new_line += line[i]
        i += 1
    lines.append(new_line)

content = '\n'.join(lines)

# 移除空行
lines = [line for line in content.split('\n') if line.strip()]

# 规范化空格（保留必要的空格）
normalized = []
for line in lines:
    # 移除行首行尾空格
    line = line.strip()
    if not line:
        continue
    # 规范化多个空格为单个空格（但保留操作符周围的空格）
    line = re.sub(r'[ \t]+', ' ', line)
    normalized.append(line)

# 输出（每行一个有效代码行）
print('\n'.join(normalized))
" 2>/dev/null || {
        # 如果 Python 不可用，使用 sed 方法（简化版）
        sed 's|//.*$||' "$file" | \
        sed ':a;N;$!ba;s|/\*[^*]*\*/||g' | \
        sed '/^[[:space:]]*$/d' | \
        sed 's/[[:space:]]\+/ /g' | \
        sed 's/^[[:space:]]*//' | \
        sed 's/[[:space:]]*$//'
    }
}

# 对比两个文件的有效代码
compare_files() {
    local ref_file=$1
    local gen_file=$2
    
    if [ ! -f "$ref_file" ]; then
        echo -e "${RED}参考文件不存在: $ref_file${NC}"
        return 1
    fi
    
    if [ ! -f "$gen_file" ]; then
        echo -e "${RED}生成文件不存在: $gen_file${NC}"
        return 1
    fi
    
    echo -e "${YELLOW}对比文件:${NC}"
    echo "  参考: $(basename $ref_file)"
    echo "  生成: $(basename $gen_file)"
    
    # 清理代码
    local ref_clean=$(mktemp)
    local gen_clean=$(mktemp)
    
    clean_code "$ref_file" > "$ref_clean"
    clean_code "$gen_file" > "$gen_clean"
    
    # 对比
    if diff -q "$ref_clean" "$gen_clean" > /dev/null; then
        echo -e "${GREEN}✅ 有效代码一致${NC}"
        rm -f "$ref_clean" "$gen_clean"
        return 0
    else
        echo -e "${RED}❌ 有效代码有差异${NC}"
        echo ""
        echo "差异统计:"
        diff -u "$ref_clean" "$gen_clean" | head -20
        rm -f "$ref_clean" "$gen_clean"
        return 1
    fi
}

# 主函数
main() {
    echo "=========================================="
    echo "有效代码一致性验证"
    echo "=========================================="
    echo ""
    
    # 检查目录
    if [ ! -d "$REF_DIR" ]; then
        echo -e "${RED}错误: 参考代码目录不存在: $REF_DIR${NC}"
        exit 1
    fi
    
    if [ ! -d "$OUTPUT_DIR" ]; then
        echo -e "${RED}错误: 生成代码目录不存在: $OUTPUT_DIR${NC}"
        echo "请先运行测试生成代码"
        exit 1
    fi
    
    # 查找所有参考文件
    local ref_files=$(find "$REF_DIR" -name "mm_st3_*.cpp" | sort)
    
    if [ -z "$ref_files" ]; then
        echo -e "${RED}错误: 没有找到参考代码文件${NC}"
        exit 1
    fi
    
    local total=0
    local passed=0
    local failed=0
    
    echo "开始对比..."
    echo ""
    
    for ref_file in $ref_files; do
        local basename=$(basename "$ref_file" .cpp)
        local gen_file="$OUTPUT_DIR/test_${basename#mm_}_kernel.cpp"
        
        # 如果生成文件不存在，尝试其他命名
        if [ ! -f "$gen_file" ]; then
            # 尝试不同的命名模式
            local alt_name=$(echo "$basename" | sed 's/mm_st3_/test_st3_/')
            gen_file="$OUTPUT_DIR/${alt_name}_kernel.cpp"
        fi
        
        if [ ! -f "$gen_file" ]; then
            echo -e "${YELLOW}⚠️  跳过: 生成文件不存在 - $(basename $ref_file)${NC}"
            continue
        fi
        
        total=$((total + 1))
        
        if compare_files "$ref_file" "$gen_file"; then
            passed=$((passed + 1))
        else
            failed=$((failed + 1))
        fi
        echo ""
    done
    
    echo "=========================================="
    echo "验证结果汇总"
    echo "=========================================="
    echo "总计: $total"
    echo -e "${GREEN}通过: $passed${NC}"
    if [ $failed -gt 0 ]; then
        echo -e "${RED}失败: $failed${NC}"
    else
        echo -e "${GREEN}失败: $failed${NC}"
    fi
    echo ""
    
    if [ $failed -eq 0 ] && [ $total -gt 0 ]; then
        echo -e "${GREEN}✅ 所有有效代码一致！${NC}"
        exit 0
    else
        echo -e "${RED}❌ 存在有效代码差异${NC}"
        exit 1
    fi
}

main "$@"


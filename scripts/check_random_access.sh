#!/bin/bash
# check_random_access.sh - 检查生成的代码中的随机读取问题

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 检查单个文件
check_file() {
    local file=$1
    local kernel_name=$2
    
    if [ ! -f "$file" ]; then
        echo -e "${RED}File not found: $file${NC}"
        return 1
    fi
    
    echo "Checking $kernel_name: $file"
    
    # 查找 local buffer 访问中的非线性索引
    # 模式：local_*[...][...][...] 其中索引包含 *, /, %
    local issues=$(grep -n "local_.*\[.*\]\[.*\]\[.*\]" "$file" 2>/dev/null | \
        grep -E "\[.*\*.*\]|\[.*/.*\]|\[.*%.*\]" || true)
    
    if [ -n "$issues" ]; then
        echo -e "  ${YELLOW}⚠️  Found potential random access issues:${NC}"
        echo "$issues" | sed 's/^/    /'
        
        # 提取具体的访问模式
        local patterns=$(echo "$issues" | grep -oE "local_[A-Za-z]*\[[^]]*\]\[[^]]*\]\[[^]]*\]" | sort -u)
        if [ -n "$patterns" ]; then
            echo -e "  ${YELLOW}Access patterns:${NC}"
            echo "$patterns" | sed 's/^/    /'
        fi
        
        return 1
    else
        echo -e "  ${GREEN}✅ No random access issues found${NC}"
        return 0
    fi
}

# 检查目录中的所有 kernel 文件
check_directory() {
    local dir=$1
    
    if [ ! -d "$dir" ]; then
        echo -e "${RED}ERROR: Directory not found: $dir${NC}"
        return 1
    fi
    
    # 查找所有 kernel 文件
    local kernel_files=$(find "$dir" -name "kernel*.cpp" -o -name "kernel*.c" 2>/dev/null | head -20)
    
    if [ -z "$kernel_files" ]; then
        echo -e "${YELLOW}No kernel files found in $dir${NC}"
        return 1
    fi
    
    local total=0
    local issues=0
    
    for file in $kernel_files; do
        ((total++))
        local kernel_name=$(basename "$(dirname "$file")")
        if ! check_file "$file" "$kernel_name"; then
            ((issues++))
        fi
        echo ""
    done
    
    echo "=========================================="
    echo "Summary: $issues/$total files have potential random access issues"
    echo "=========================================="
    
    return $issues
}

# 检查特定的访问模式
check_pattern() {
    local file=$1
    local pattern=$2
    
    if grep -q "$pattern" "$file" 2>/dev/null; then
        echo -e "${YELLOW}Found pattern: $pattern${NC}"
        grep -n "$pattern" "$file" | head -5
        return 1
    fi
    return 0
}

# 详细分析 MTTKRP 问题
analyze_mttkrp_issue() {
    local file=$1
    
    echo "=========================================="
    echo "MTTKRP Random Access Issue Analysis"
    echo "=========================================="
    echo ""
    
    # 查找 IO_L2_in_intra_trans 函数
    echo "Checking IO_L2_in_intra_trans functions..."
    local intra_trans=$(grep -A 30 "IO_L2_in_intra_trans" "$file" | grep "local_.*\[.*\]\[.*\]\[.*\]" | head -5)
    
    if [ -n "$intra_trans" ]; then
        echo -e "${YELLOW}Found local buffer accesses in intra_trans:${NC}"
        echo "$intra_trans" | sed 's/^/  /'
        echo ""
        
        # 检查是否有非线性索引
        local nonlinear=$(echo "$intra_trans" | grep -E "\[.*\*.*\]|\[.*/.*\]|\[.*%.*\]" || true)
        if [ -n "$nonlinear" ]; then
            echo -e "${RED}⚠️  Found nonlinear index expressions:${NC}"
            echo "$nonlinear" | sed 's/^/  /'
        fi
    fi
    
    # 查找 IO_L2_in_inter_trans 函数
    echo ""
    echo "Checking IO_L2_in_inter_trans functions..."
    local inter_trans=$(grep -A 30 "IO_L2_in_inter_trans" "$file" | grep "local_.*\[.*\]\[.*\]\[.*\]" | head -5)
    
    if [ -n "$inter_trans" ]; then
        echo -e "${YELLOW}Found local buffer accesses in inter_trans:${NC}"
        echo "$inter_trans" | sed 's/^/  /'
    fi
}

# 主函数
main() {
    if [ $# -eq 0 ]; then
        echo "Usage: $0 <directory_or_file> [--analyze-mttkrp]"
        echo ""
        echo "Examples:"
        echo "  $0 /path/to/autosa/output/src"
        echo "  $0 /path/to/kernel.cpp --analyze-mttkrp"
        exit 1
    fi
    
    local target=$1
    local analyze_mttkrp=false
    
    if [ "$2" = "--analyze-mttkrp" ]; then
        analyze_mttkrp=true
    fi
    
    if [ -f "$target" ]; then
        # 检查单个文件
        local kernel_name=$(basename "$target")
        check_file "$target" "$kernel_name"
        
        if [ "$analyze_mttkrp" = true ]; then
            echo ""
            analyze_mttkrp_issue "$target"
        fi
    elif [ -d "$target" ]; then
        # 检查目录
        check_directory "$target"
    else
        echo -e "${RED}ERROR: $target is not a file or directory${NC}"
        exit 1
    fi
}

main "$@"



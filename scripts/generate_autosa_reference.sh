#!/bin/bash
# 生成 AutoSA HLS C 参考代码的脚本
# 用途: 为测试提供可重现的 AutoSA 参考代码，确保参数与代码对应

set -e

# 配置
AUTOSA_ROOT="${AUTOSA_ROOT:-$(pwd)/third_party/AutoSA}"
OUTPUT_DIR="${OUTPUT_DIR:-$(pwd)/test/autosa_hls_refs_generated}"
KERNEL_DIR="${KERNEL_DIR:-$AUTOSA_ROOT/autosa_tests}"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 打印函数
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查 AutoSA 是否可用
check_autosa() {
    if [ ! -f "$AUTOSA_ROOT/autosa" ]; then
        print_error "AutoSA 可执行文件不存在: $AUTOSA_ROOT/autosa"
        print_info "请先构建 AutoSA 或设置 AUTOSA_ROOT 环境变量"
        exit 1
    fi
    
    if [ ! -x "$AUTOSA_ROOT/autosa" ]; then
        print_error "AutoSA 可执行文件没有执行权限"
        exit 1
    fi
    
    print_info "AutoSA 路径: $AUTOSA_ROOT/autosa"
}

# 生成单个配置的参考代码
generate_reference() {
    local kernel=$1
    local kernel_file=$2
    local spacetime=$3
    local array_part=$4
    local latency=$5
    local simd=$6
    local simd_info=$7
    local extra_args=$8
    
    # 构建输出文件名
    local filename="${kernel}_st${spacetime}"
    
    if [ -n "$array_part" ]; then
        filename="${filename}_ap${array_part//,/_}"
    fi
    
    if [ -n "$latency" ]; then
        filename="${filename}_lat${latency//,/_}"
    fi
    
    if [ -n "$simd" ]; then
        filename="${filename}_simd${simd//,/_}"
    fi
    
    filename="${filename}_kernel.cpp"
    local output_file="$OUTPUT_DIR/$filename"
    
    # 构建 AutoSA 命令
    local cmd="$AUTOSA_ROOT/autosa"
    cmd="$cmd $kernel_file"
    cmd="$cmd --config=$AUTOSA_ROOT/autosa_config/autosa_config.json"
    cmd="$cmd --target=autosa_hls_c"
    cmd="$cmd --output-dir=$OUTPUT_DIR/tmp"
    cmd="$cmd --sa-sizes=\"{kernel[]->space_time[${spacetime}]"
    
    if [ -n "$array_part" ]; then
        cmd="$cmd;kernel[]->array_part[${array_part}]"
    fi
    
    if [ -n "$latency" ]; then
        cmd="$cmd;kernel[]->latency[${latency}]"
    fi
    
    if [ -n "$simd" ]; then
        cmd="$cmd;kernel[]->simd[${simd}]"
    fi
    
    cmd="$cmd}\""
    
    if [ -n "$simd_info" ] && [ -f "$simd_info" ]; then
        cmd="$cmd --simd-info=$simd_info"
    fi
    
    if [ -n "$extra_args" ]; then
        cmd="$cmd $extra_args"
    fi
    
    # 执行命令
    print_info "生成: $filename"
    print_info "命令: $cmd"
    
    if eval "$cmd" > "$OUTPUT_DIR/tmp/${filename}.log" 2>&1; then
        # 复制生成的 kernel 文件
        if [ -f "$OUTPUT_DIR/tmp/src/kernel_kernel.cpp" ]; then
            cp "$OUTPUT_DIR/tmp/src/kernel_kernel.cpp" "$output_file"
            
            # 生成元数据文件
            cat > "${output_file%.cpp}.meta.json" <<EOF
{
  "kernel": "$kernel",
  "kernel_file": "$kernel_file",
  "spacetime": $spacetime,
  "array_part": "$array_part",
  "latency": "$latency",
  "simd": "$simd",
  "simd_info": "$simd_info",
  "extra_args": "$extra_args",
  "generated_at": "$(date -Iseconds)",
  "autosa_version": "$($AUTOSA_ROOT/autosa --version 2>&1 || echo 'unknown')"
}
EOF
            print_info "✓ 成功生成: $filename"
        else
            print_error "生成失败: kernel_kernel.cpp 不存在"
            return 1
        fi
    else
        print_error "AutoSA 执行失败，查看日志: $OUTPUT_DIR/tmp/${filename}.log"
        return 1
    fi
    
    # 清理临时文件
    rm -rf "$OUTPUT_DIR/tmp"
}

# 生成 MM 参考代码
generate_mm_references() {
    local kernel_file="$KERNEL_DIR/mm/kernel.c"
    local simd_info="$KERNEL_DIR/mm/simd_info.json"
    
    if [ ! -f "$kernel_file" ]; then
        print_warn "MM kernel 文件不存在: $kernel_file"
        return
    fi
    
    print_info "生成 MM (Matrix Multiplication) 参考代码..."
    
    # ST3 配置（最常用）
    generate_reference "mm" "$kernel_file" 3 "16,16,16" "8,8" "2" "$simd_info" "--host-serialize"
    
    # 其他 ST 配置示例
    # generate_reference "mm" "$kernel_file" 0 "32,32,32" "8,8" "2" "$simd_info" "--host-serialize"
    # generate_reference "mm" "$kernel_file" 1 "32,32,32" "8,8" "2" "$simd_info" "--host-serialize"
    # generate_reference "mm" "$kernel_file" 2 "4,32,32" "8,8" "2" "$simd_info" "--host-serialize"
    # generate_reference "mm" "$kernel_file" 4 "32,4,32" "16,16" "2" "$simd_info" "--host-serialize"
    # generate_reference "mm" "$kernel_file" 5 "32,4,32" "16,16" "2" "$simd_info" "--host-serialize"
}

# 生成 MTTKRP 参考代码
generate_mttkrp_references() {
    local kernel_file="$KERNEL_DIR/large/mttkrp/kernel.c"
    local simd_info="$KERNEL_DIR/large/mttkrp/simd_info.json"
    
    if [ ! -f "$kernel_file" ]; then
        print_warn "MTTKRP kernel 文件不存在: $kernel_file"
        return
    fi
    
    print_info "生成 MTTKRP 参考代码..."
    
    # 从 AutoSA 文档示例
    generate_reference "mttkrp" "$kernel_file" 3 "128,128,2" "16,8" "8,1" "$simd_info" ""
}

# 生成 TTMC 参考代码
generate_ttmc_references() {
    local kernel_file="$KERNEL_DIR/large/ttmc/kernel.c"
    local simd_info="$KERNEL_DIR/large/ttmc/simd_info.json"
    
    if [ ! -f "$kernel_file" ]; then
        print_warn "TTMC kernel 文件不存在: $kernel_file"
        return
    fi
    
    print_info "生成 TTMC 参考代码..."
    
    # 从 AutoSA 文档示例
    generate_reference "ttmc" "$kernel_file" 4 "16,64,16,32" "1,8,8" "8,1" "$simd_info" ""
}

# 生成 CNN 参考代码
generate_cnn_references() {
    local kernel_file="$KERNEL_DIR/cnn/kernel.c"
    local simd_info="$KERNEL_DIR/cnn/simd_info.json"
    
    if [ ! -f "$kernel_file" ]; then
        print_warn "CNN kernel 文件不存在: $kernel_file"
        return
    fi
    
    print_info "生成 CNN 参考代码..."
    
    # 从 AutoSA 文档示例
    generate_reference "cnn" "$kernel_file" 4 "8,8,4,8" "4,4,4" "1,1,1,2" "$simd_info" ""
}

# 主函数
main() {
    print_info "开始生成 AutoSA HLS C 参考代码..."
    print_info "输出目录: $OUTPUT_DIR"
    
    # 检查 AutoSA
    check_autosa
    
    # 创建输出目录
    mkdir -p "$OUTPUT_DIR"
    
    # 生成参考代码
    generate_mm_references
    generate_mttkrp_references
    generate_ttmc_references
    generate_cnn_references
    
    # 生成索引文件
    print_info "生成索引文件..."
    cat > "$OUTPUT_DIR/INDEX.md" <<EOF
# AutoSA HLS C 参考代码索引

生成时间: $(date)

## 文件列表

EOF
    
    for file in "$OUTPUT_DIR"/*.cpp; do
        if [ -f "$file" ]; then
            echo "- \`$(basename "$file")\`" >> "$OUTPUT_DIR/INDEX.md"
            if [ -f "${file%.cpp}.meta.json" ]; then
                echo "  - 元数据: \`$(basename "${file%.cpp}.meta.json")\`" >> "$OUTPUT_DIR/INDEX.md"
            fi
        fi
    done
    
    print_info "✓ 完成！参考代码已生成到: $OUTPUT_DIR"
    print_info "查看索引: $OUTPUT_DIR/INDEX.md"
}

# 运行主函数
main "$@"

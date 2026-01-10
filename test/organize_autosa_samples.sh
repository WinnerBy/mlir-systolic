#!/bin/bash
# organize_autosa_samples.sh - Organize and index AutoSA reference samples

set -e

SAMPLES_DIR="/workspaces/mlir-systolic/autosa_reference_samples"
COLLECTED_DIR="/workspaces/mlir-systolic/autosa_reference_samples/collected_hls_files"

echo "========================================"
echo "Organizing AutoSA Reference Samples"
echo "========================================"
echo ""

if [ ! -d "$SAMPLES_DIR" ]; then
    echo "Error: Samples directory not found: $SAMPLES_DIR"
    exit 1
fi

# Create collected directory
mkdir -p "$COLLECTED_DIR"

# Generate index
INDEX_FILE="$SAMPLES_DIR/INDEX.md"
STATS_FILE="$SAMPLES_DIR/STATISTICS.csv"

cat > "$INDEX_FILE" << 'EOF'
# AutoSA Reference Samples Index

这是一个全面的AutoSA生成样本集合，包含多种kernel和参数配置。

## 目录结构

每个样本目录包含：
- `kernel.cpp` - 生成的HLS C++代码
- `metadata.json` - 配置和统计信息
- `autosa.log` - 完整的生成日志
- `src/` - 完整的源文件目录

## 样本列表

EOF

# Create CSV header
echo "Kernel,Variant,SpaceTime,ArrayPart,Latency,SIMD,CodeLines,PEFunctions,IOModules,Pragmas,FIFOs" > "$STATS_FILE"

# Process all samples
total_samples=0
total_lines=0

for category in mm cnn mttkrp ttmc; do
    echo "### ${category^^} Kernels" >> "$INDEX_FILE"
    echo "" >> "$INDEX_FILE"
    
    count=0
    for dir in "$SAMPLES_DIR"/${category}_*; do
        if [ -d "$dir" ]; then
            variant=$(basename "$dir")
            
            if [ -f "$dir/metadata.json" ]; then
                # Extract info from metadata using grep and sed
                desc=$(grep '"description"' "$dir/metadata.json" | sed 's/.*: "\(.*\)",*/\1/')
                st=$(grep '"space_time"' "$dir/metadata.json" | sed 's/.*: \([0-9]*\).*/\1/')
                ap=$(grep '"array_part"' "$dir/metadata.json" | sed 's/.*: "\(.*\)",*/\1/')
                lat=$(grep '"latency"' "$dir/metadata.json" | sed 's/.*: "\(.*\)",*/\1/')
                simd=$(grep '"simd"' "$dir/metadata.json" | sed 's/.*: "\(.*\)",*/\1/')
                lines=$(grep '"code_lines"' "$dir/metadata.json" | sed 's/.*: \([0-9]*\).*/\1/')
                pe=$(grep '"pe_functions"' "$dir/metadata.json" | sed 's/.*: \([0-9]*\).*/\1/')
                io=$(grep '"io_modules"' "$dir/metadata.json" | sed 's/.*: \([0-9]*\).*/\1/')
                pragma=$(grep '"hls_pragmas"' "$dir/metadata.json" | sed 's/.*: \([0-9]*\).*/\1/')
                fifo=$(grep '"fifo_channels"' "$dir/metadata.json" | sed 's/.*: \([0-9]*\).*/\1/')
                
                [ -z "$lines" ] && lines=0
                
                echo "- **$variant**: $desc" >> "$INDEX_FILE"
                echo "  - Space-Time: $st | Array: [$ap] | Latency: [$lat] | SIMD: [$simd]" >> "$INDEX_FILE"
                echo "  - Code: $lines lines | PE: $pe | I/O: $io | Pragmas: $pragma | FIFOs: $fifo" >> "$INDEX_FILE"
                echo "" >> "$INDEX_FILE"
                
                # Add to CSV
                echo "$category,$variant,$st,\"$ap\",\"$lat\",\"$simd\",$lines,$pe,$io,$pragma,$fifo" >> "$STATS_FILE"
                
                # Copy to collected directory
                if [ -f "$dir/kernel.cpp" ]; then
                    cp "$dir/kernel.cpp" "$COLLECTED_DIR/${variant}.cpp"
                    cp "$dir/metadata.json" "$COLLECTED_DIR/${variant}.json"
                fi
                
                count=$((count + 1))
                total_lines=$((total_lines + lines))
            fi
        fi
    done
    
    echo "  - Total: $count variants" >> "$INDEX_FILE"
    echo "" >> "$INDEX_FILE"
    total_samples=$((total_samples + count))
done

# Add summary
cat >> "$INDEX_FILE" << EOF

## 总体统计

- **总样本数**: $total_samples
- **总代码行数**: $total_lines
- **平均每样本**: $((total_lines / total_samples)) 行

## 按类别分组

### Matrix Multiplication (MM)
不同的space-time配置(ST0-ST5)和参数变体：
- ST0-ST2: 1D systolic arrays
- ST3-ST5: 2D systolic arrays
- 变体包括不同的array partition、latency hiding、SIMD vectorization

### Convolutional Neural Networks (CNN)
多种dataflow配置(ST0-ST9)：
- Output-stationary arrays
- Input-reduction arrays
- 2D配置优化

### Tensor Kernels (MTTKRP, TTMC)
高维张量运算：
- MTTKRP: 张量-矩阵乘积
- TTMC: 张量-张量矩阵链

## 使用方法

### 查看特定样本
\`\`\`bash
cd autosa_reference_samples/mm_st3
cat metadata.json      # 查看配置
less kernel.cpp        # 查看生成代码
\`\`\`

### 统计分析
\`\`\`bash
cat STATISTICS.csv | column -t -s,   # 表格形式查看
\`\`\`

### 按代码行数排序
\`\`\`bash
sort -t, -k7 -nr STATISTICS.csv | head -10   # 最大的10个
\`\`\`

## 重新生成

使用脚本重新生成所有样本：
\`\`\`bash
cd /workspaces/mlir-systolic/test
./generate_autosa_variants.sh --clean
\`\`\`

生成特定kernel的变体：
\`\`\`bash
./generate_autosa_variants.sh --kernel mm
\`\`\`
EOF

echo "✓ Generated INDEX.md with $total_samples samples"
echo "✓ Generated STATISTICS.csv"
echo "✓ Collected ${total_samples} HLS files to collected_hls_files/"
echo ""
echo "Summary:"
echo "  Total samples: $total_samples"
echo "  Total code lines: $total_lines"
echo "  Average lines/sample: $((total_lines / total_samples))"
echo ""
echo "Output directory: $SAMPLES_DIR"

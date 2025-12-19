#!/bin/bash
# 构建 mlir-systolic（使用已构建的 Polygeist）
# 使用方法: ./scripts/build-systolic.sh

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}构建 mlir-systolic${NC}"
echo -e "${GREEN}========================================${NC}"

# 进入项目根目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

# 检查 Polygeist 构建目录
POLYGEIST_BUILD="$PROJECT_ROOT/third_party/Polygeist/build"
if [ ! -d "$POLYGEIST_BUILD" ]; then
    echo -e "${RED}错误: 找不到 Polygeist 构建目录: $POLYGEIST_BUILD${NC}"
    echo -e "${YELLOW}请先构建 Polygeist: ./scripts/build-polygeist.sh${NC}"
    exit 1
fi

# 验证 Polymer 库是否存在
if [ ! -f "$POLYGEIST_BUILD/lib/libPolymerTransforms.a" ]; then
    echo -e "${RED}错误: 找不到 Polymer 库${NC}"
    echo -e "${YELLOW}请先构建 Polygeist: ./scripts/build-polygeist.sh${NC}"
    exit 1
fi

echo -e "${GREEN}使用 Polygeist 构建目录: $POLYGEIST_BUILD${NC}"

# 检查内存并选择并行度
echo -e "${YELLOW}检查系统内存...${NC}"
AVAIL_MEM=$(free -g | awk '/^Mem:/{print $7}')
echo -e "${YELLOW}可用内存: ${AVAIL_MEM}GB${NC}"

if [ "$AVAIL_MEM" -lt 4 ]; then
    JOBS=1
elif [ "$AVAIL_MEM" -lt 8 ]; then
    JOBS=1
elif [ "$AVAIL_MEM" -lt 16 ]; then
    JOBS=2
else
    JOBS=4
fi

echo -e "${GREEN}使用 $JOBS 个并行任务${NC}"

# 清理旧构建（可选）
if [ -d "build" ]; then
    read -p "是否清理旧构建？(y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo -e "${YELLOW}清理旧构建...${NC}"
        rm -rf build
    fi
fi

# 创建构建目录
mkdir -p build
cd build

# 配置 CMake
echo -e "${GREEN}配置 CMake...${NC}"
cmake .. \
  -DMLIR_DIR="$POLYGEIST_BUILD/lib/cmake/mlir" \
  -DLLVM_DIR="$POLYGEIST_BUILD/lib/cmake/llvm" \
  -DPOLYGEIST_BUILD="$POLYGEIST_BUILD"

# 构建 mlir-systolic
echo -e "${GREEN}构建 mlir-systolic（使用 $JOBS 个并行任务）...${NC}"
cmake --build . -j$JOBS

# 验证构建
echo -e "${GREEN}验证构建结果...${NC}"
if [ -f "bin/systolic-opt" ] && [ -f "bin/systolic-translate" ]; then
    echo -e "${GREEN}✅ mlir-systolic 构建成功！${NC}"
    ls -lh bin/systolic-opt bin/systolic-translate
    echo ""
    echo -e "${GREEN}可以使用以下命令测试:${NC}"
    echo -e "  ./bin/systolic-opt test/matmul/matmul.mlir --systolic-transform"
else
    echo -e "${RED}❌ mlir-systolic 构建失败${NC}"
    exit 1
fi


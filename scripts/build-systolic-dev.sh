#!/bin/bash
# 开发模式：快速构建 mlir-systolic（使用已构建的 Polygeist）
# 使用方法: ./scripts/build-systolic-dev.sh

set -e  # 遇到错误立即退出

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}mlir-systolic 开发模式构建脚本${NC}"
echo -e "${GREEN}========================================${NC}"

# 进入项目根目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

# 检查 Polygeist 构建目录
POLYGEIST_BUILD=""
if [ -n "$POLYGEIST_BUILD" ]; then
    POLYGEIST_BUILD="$POLYGEIST_BUILD"
elif [ -d "$PROJECT_ROOT/third_party/Polygeist/build" ]; then
    POLYGEIST_BUILD="$PROJECT_ROOT/third_party/Polygeist/build"
else
    echo -e "${RED}错误: 找不到 Polygeist 构建目录${NC}"
    echo -e "${YELLOW}请先构建 Polygeist: ./scripts/build-polygeist-dev.sh${NC}"
    exit 1
fi

# 验证 Polymer 库是否存在
if [ ! -f "$POLYGEIST_BUILD/lib/libPolymerTransforms.a" ]; then
    echo -e "${RED}错误: 找不到 Polymer 库${NC}"
    echo -e "${YELLOW}请先构建 Polygeist: ./scripts/build-polygeist-dev.sh${NC}"
    exit 1
fi

echo -e "${GREEN}使用 Polygeist 构建目录: $POLYGEIST_BUILD${NC}"

# 检查是否有 Polygeist 统一构建的 LLVM/MLIR（优先使用）
POLYGEIST_BUILD_DIR="$PROJECT_ROOT/third_party/Polygeist/build"
if [ -d "$POLYGEIST_BUILD_DIR" ] && [ -f "$POLYGEIST_BUILD_DIR/lib/cmake/mlir/MLIRConfig.cmake" ]; then
    echo -e "${GREEN}找到 Polygeist 统一构建的 LLVM/MLIR，使用统一构建的路径${NC}"
    MLIR_DIR="$POLYGEIST_BUILD_DIR/lib/cmake/mlir"
    LLVM_DIR="$POLYGEIST_BUILD_DIR/lib/cmake/llvm"
    USE_POLYGEIST_BUILD=1
elif [ -d "$PROJECT_ROOT/build" ] && [ -f "$PROJECT_ROOT/build/lib/cmake/mlir/MLIRConfig.cmake" ]; then
    # 检查是否有 mlir-systolic 统一构建的 LLVM/MLIR
    echo -e "${GREEN}找到 mlir-systolic 统一构建的 LLVM/MLIR，使用统一构建的路径${NC}"
    MLIR_DIR="$PROJECT_ROOT/build/lib/cmake/mlir"
    LLVM_DIR="$PROJECT_ROOT/build/lib/cmake/llvm"
    USE_POLYGEIST_BUILD=0
else
    # 尝试从环境变量获取
    if [ -n "$MLIR_DIR" ] && [ -n "$LLVM_DIR" ]; then
        echo -e "${GREEN}使用环境变量中的 LLVM/MLIR 路径${NC}"
        USE_POLYGEIST_BUILD=0
    else
        echo -e "${YELLOW}未找到已构建的 LLVM/MLIR${NC}"
        echo -e "${YELLOW}请先运行: ./scripts/build-polygeist-dev.sh${NC}"
        echo -e "${YELLOW}或者设置环境变量：${NC}"
        echo -e "  export MLIR_DIR=/path/to/mlir/lib/cmake/mlir"
        echo -e "  export LLVM_DIR=/path/to/llvm/lib/cmake/llvm"
        exit 1
    fi
fi

# 检查内存
echo -e "${YELLOW}检查系统内存...${NC}"
free -h

AVAIL_MEM=$(free -g | awk '/^Mem:/{print $7}')
echo -e "${YELLOW}可用内存: ${AVAIL_MEM}GB${NC}"

# 根据内存选择并行度
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

if [ -z "$MLIR_DIR" ] || [ -z "$LLVM_DIR" ]; then
    echo -e "${RED}错误: 需要设置 MLIR_DIR 和 LLVM_DIR${NC}"
    exit 1
fi

cmake .. \
  -DMLIR_DIR="$MLIR_DIR" \
  -DLLVM_DIR="$LLVM_DIR" \
  -DPOLYGEIST_BUILD="$POLYGEIST_BUILD"

# 构建 mlir-systolic
echo -e "${GREEN}构建 mlir-systolic（使用 $JOBS 个并行任务）...${NC}"
cmake --build . -j$JOBS

# 验证构建
echo -e "${GREEN}验证构建结果...${NC}"
if [ -f "bin/systolic-opt" ]; then
    echo -e "${GREEN}✅ mlir-systolic 构建成功！${NC}"
    ls -lh bin/systolic-opt
    echo ""
    echo -e "${GREEN}可以使用以下命令测试:${NC}"
    echo -e "  ./bin/systolic-opt test/matmul/matmul.mlir --systolic-transform"
else
    echo -e "${RED}❌ mlir-systolic 构建失败${NC}"
    exit 1
fi


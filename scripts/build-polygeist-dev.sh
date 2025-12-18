#!/bin/bash
# 开发模式：统一构建 Polygeist + Polymer（使用 Polygeist 的 llvm-project submodule）
# 参考 Polygeist README Option 2: 统一构建方式
# 使用方法: ./scripts/build-polygeist-dev.sh

set -e  # 遇到错误立即退出

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Polygeist 开发模式构建脚本${NC}"
echo -e "${GREEN}（统一构建 LLVM/MLIR/Polly/Polygeist/Polymer）${NC}"
echo -e "${GREEN}（只构建 Polymer 需要的库）${NC}"
echo -e "${GREEN}========================================${NC}"

# 进入项目根目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
POLYGEIST_DIR="$PROJECT_ROOT/third_party/Polygeist"

cd "$PROJECT_ROOT"

# 检查 Polygeist 是否存在
if [ ! -d "$POLYGEIST_DIR" ]; then
    echo -e "${RED}错误: 找不到 Polygeist 目录: $POLYGEIST_DIR${NC}"
    echo -e "${YELLOW}请初始化 submodule: git submodule update --init --recursive${NC}"
    exit 1
fi

# 检查 llvm-project submodule 是否存在
LLVM_PROJECT_DIR="$POLYGEIST_DIR/llvm-project"
if [ ! -d "$LLVM_PROJECT_DIR" ]; then
    echo -e "${RED}错误: 找不到 llvm-project submodule: $LLVM_PROJECT_DIR${NC}"
    echo -e "${YELLOW}请初始化 submodule: git submodule update --init --recursive${NC}"
    exit 1
fi

# 检查内存
echo -e "${YELLOW}检查系统内存...${NC}"
free -h

AVAIL_MEM=$(free -g | awk '/^Mem:/{print $7}')
echo -e "${YELLOW}可用内存: ${AVAIL_MEM}GB${NC}"

# 根据内存选择并行度
if [ "$AVAIL_MEM" -lt 4 ]; then
    JOBS=1
    echo -e "${RED}警告: 内存不足 4GB，使用单线程构建${NC}"
elif [ "$AVAIL_MEM" -lt 8 ]; then
    JOBS=1
    echo -e "${YELLOW}内存不足 8GB，使用单线程构建${NC}"
elif [ "$AVAIL_MEM" -lt 16 ]; then
    JOBS=2
    echo -e "${GREEN}使用 2 个并行任务${NC}"
else
    JOBS=4
    echo -e "${GREEN}内存充足，使用 4 个并行任务${NC}"
fi

# 清理旧构建（可选）
read -p "是否清理旧构建？(y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${YELLOW}清理旧构建...${NC}"
    rm -rf "$POLYGEIST_DIR/build"
fi

# 创建构建目录
cd "$POLYGEIST_DIR"
mkdir -p build
cd build

# 配置 CMake（统一构建方式，参考 Polygeist README Option 2）
echo -e "${GREEN}配置 CMake（统一构建方式）...${NC}"
echo -e "${YELLOW}使用 Polygeist 的 llvm-project submodule 统一构建${NC}"
echo -e "${YELLOW}使用系统编译器（clang 或 gcc）编译 LLVM/MLIR/Polly/Polygeist${NC}"

cmake -G Ninja ../llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1

# 只构建 Polymer 需要的库（加快构建速度）
echo -e "${GREEN}构建 Polymer 库（使用 $JOBS 个并行任务）...${NC}"
echo -e "${YELLOW}只构建需要的库，加快构建速度...${NC}"
echo -e "${YELLOW}注意: 首次构建可能需要较长时间（需要先构建 LLVM/MLIR 基础库）${NC}"

# 先构建一些基础依赖（如果需要）
echo -e "${YELLOW}构建基础依赖...${NC}"
ninja -j$JOBS LLVMSupport LLVMCore || true

# 构建 Polymer 库
ninja -j$JOBS PolymerSupport PolymerTargetISL PolymerTransforms

# 验证构建
echo -e "${GREEN}验证构建结果...${NC}"
if [ -f "lib/libPolymerSupport.a" ] && \
   [ -f "lib/libPolymerTargetISL.a" ] && \
   [ -f "lib/libPolymerTransforms.a" ]; then
    echo -e "${GREEN}✅ Polymer 库构建成功！${NC}"
    ls -lh lib/libPolymer*.a
    echo ""
    echo -e "${GREEN}下一步:${NC}"
    echo -e "  1. 设置环境变量: export POLYGEIST_BUILD=$POLYGEIST_DIR/build"
    echo -e "  2. 构建 mlir-systolic: cd $PROJECT_ROOT && ./scripts/build-systolic-dev.sh"
    echo ""
    echo -e "${GREEN}提示:${NC}"
    echo -e "  - LLVM/MLIR 已构建在: $POLYGEIST_DIR/build"
    echo -e "  - 后续构建 mlir-systolic 可以使用这个构建目录中的 LLVM/MLIR"
else
    echo -e "${RED}❌ Polymer 库构建失败${NC}"
    exit 1
fi


#!/bin/bash
# 统一构建脚本（一次性构建 LLVM/MLIR/Polygeist/Polymer/mlir-systolic）
# 使用方法: ./scripts/build-unified.sh

set -e  # 遇到错误立即退出

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}mlir-systolic 统一构建脚本${NC}"
echo -e "${GREEN}========================================${NC}"

# 检查内存
echo -e "${YELLOW}检查系统内存...${NC}"
free -h

# 获取可用内存（GB）
AVAIL_MEM=$(free -g | awk '/^Mem:/{print $7}')
echo -e "${YELLOW}可用内存: ${AVAIL_MEM}GB${NC}"

# 根据内存选择并行度
if [ "$AVAIL_MEM" -lt 4 ]; then
    JOBS=1
    echo -e "${RED}警告: 内存不足 4GB，使用单线程构建（最慢但最安全）${NC}"
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

# 进入项目根目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

# 检查 Polygeist 是否存在
POLYGEIST_DIR="$PROJECT_ROOT/third_party/Polygeist"
if [ ! -d "$POLYGEIST_DIR" ]; then
    echo -e "${RED}错误: 找不到 Polygeist 目录: $POLYGEIST_DIR${NC}"
    echo -e "${YELLOW}请初始化 submodule: git submodule update --init --recursive${NC}"
    exit 1
fi

# 检查 llvm-project 是否存在
LLVM_PROJECT_DIR="$POLYGEIST_DIR/llvm-project"
if [ ! -d "$LLVM_PROJECT_DIR" ]; then
    echo -e "${RED}错误: 找不到 llvm-project 目录: $LLVM_PROJECT_DIR${NC}"
    echo -e "${YELLOW}请初始化 submodule: git submodule update --init --recursive${NC}"
    exit 1
fi

# 清理旧构建（可选）
read -p "是否清理旧构建？(y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${YELLOW}清理旧构建...${NC}"
    rm -rf build
fi

# 创建构建目录
mkdir -p build
cd build

# 配置 CMake（统一构建）
echo -e "${GREEN}配置 CMake（统一构建模式）...${NC}"
cmake -G Ninja ../third_party/Polygeist/llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist;mlir-systolic" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=../third_party/Polygeist \
  -DLLVM_EXTERNAL_MLIR_SYSTOLIC_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1 \
  -DSYSTOLIC_ENABLE_POLYMER=1

# 构建 Polymer 库（mlir-systolic 依赖它）
echo -e "${GREEN}构建 Polymer 库（使用 $JOBS 个并行任务）...${NC}"
echo -e "${YELLOW}这可能需要较长时间，请耐心等待...${NC}"

ninja -j$JOBS PolymerSupport PolymerTargetISL PolymerTransforms

# 验证 Polymer 库
echo -e "${GREEN}验证 Polymer 库...${NC}"
if [ -f "lib/libPolymerSupport.a" ] && \
   [ -f "lib/libPolymerTargetISL.a" ] && \
   [ -f "lib/libPolymerTransforms.a" ]; then
    echo -e "${GREEN}✅ Polymer 库构建成功！${NC}"
    ls -lh lib/libPolymer*.a
else
    echo -e "${RED}❌ Polymer 库构建失败${NC}"
    exit 1
fi

# 构建 mlir-systolic
echo -e "${GREEN}构建 mlir-systolic（使用 $JOBS 个并行任务）...${NC}"
ninja -j$JOBS systolic-opt

# 验证 mlir-systolic
echo -e "${GREEN}验证 mlir-systolic...${NC}"
if [ -f "bin/systolic-opt" ]; then
    echo -e "${GREEN}✅ mlir-systolic 构建成功！${NC}"
    ls -lh bin/systolic-opt
    echo ""
    echo -e "${GREEN}构建完成！${NC}"
    echo -e "${GREEN}可以使用以下命令测试:${NC}"
    echo -e "  ./bin/systolic-opt test/matmul/matmul.mlir --systolic-transform"
else
    echo -e "${RED}❌ mlir-systolic 构建失败${NC}"
    exit 1
fi


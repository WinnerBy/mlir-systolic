#!/bin/bash
# 构建 Polygeist + Polymer（统一构建方式）
# 参考 Polygeist README Option 2
# 使用方法: ./scripts/build-polygeist.sh

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}构建 Polygeist + Polymer${NC}"
echo -e "${GREEN}（统一构建 LLVM/MLIR/Polly/Polygeist/Polymer）${NC}"
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

# 检查 llvm-project submodule
LLVM_PROJECT_DIR="$POLYGEIST_DIR/llvm-project"
if [ ! -d "$LLVM_PROJECT_DIR" ]; then
    echo -e "${RED}错误: 找不到 llvm-project submodule: $LLVM_PROJECT_DIR${NC}"
    echo -e "${YELLOW}请初始化 submodule: git submodule update --init --recursive${NC}"
    exit 1
fi

# 解析参数：支持 -j/--jobs 指定并行度（优先级：命令行 > 环境变量 > 内存检测）
JOBS_OVERRIDE=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        -j|--jobs)
            JOBS_OVERRIDE="$2"; shift 2 ;;
        --)
            shift; break ;;
        *)
            # 其余参数留给 CMake（目前忽略）
            shift ;;
    esac
done

# 并行度来源：命令行 > 环境变量(NINJA_JOBS/JOBS) > 内存检测
if [[ -n "$JOBS_OVERRIDE" ]]; then
    JOBS="$JOBS_OVERRIDE"; JOBS_SRC="cmdline";
elif [[ -n "${NINJA_JOBS:-}" ]]; then
    JOBS="$NINJA_JOBS"; JOBS_SRC="env(NINJA_JOBS)";
elif [[ -n "${JOBS:-}" ]]; then
    JOBS="$JOBS"; JOBS_SRC="env(JOBS)";
else
    echo -e "${YELLOW}检查系统内存...${NC}"
    AVAIL_MEM=$(free -g | awk '/^Mem:/{print $7}')
    echo -e "${YELLOW}可用内存: ${AVAIL_MEM}GB${NC}"
    if [[ -z "$AVAIL_MEM" || "$AVAIL_MEM" -lt 4 ]]; then
        JOBS=1; JOBS_SRC="mem"; echo -e "${RED}警告: 内存不足 4GB，使用单线程构建${NC}"
    elif [[ "$AVAIL_MEM" -lt 8 ]]; then
        JOBS=1; JOBS_SRC="mem"; echo -e "${YELLOW}内存不足 8GB，使用单线程构建${NC}"
    elif [[ "$AVAIL_MEM" -lt 16 ]]; then
        JOBS=2; JOBS_SRC="mem"; echo -e "${GREEN}使用 2 个并行任务${NC}"
    else
        JOBS=4; JOBS_SRC="mem"; echo -e "${GREEN}内存充足，使用 4 个并行任务${NC}"
    fi
fi

# 校验 JOBS 数值
if ! [[ "$JOBS" =~ ^[0-9]+$ ]] || [[ "$JOBS" -le 0 ]]; then
    echo -e "${YELLOW}无效的并行度: $JOBS，回退为 2${NC}"
    JOBS=2; JOBS_SRC="fallback";
fi
echo -e "${GREEN}并行度: $JOBS (${JOBS_SRC})${NC}"

# 清理旧构建（可选）
if [ -d "$POLYGEIST_DIR/build" ]; then
    read -p "是否清理旧构建？(y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo -e "${YELLOW}清理旧构建...${NC}"
        rm -rf "$POLYGEIST_DIR/build"
    fi
fi

# 创建构建目录
cd "$POLYGEIST_DIR"
mkdir -p build
cd build

# 配置 CMake（统一构建方式，参考 Polygeist README Option 2）
echo -e "${GREEN}配置 CMake（统一构建方式）...${NC}"
cmake -G Ninja ../llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1

# 构建 Polymer 库和必要的 MLIR 库
echo -e "${GREEN}构建 Polymer 库和必要的 MLIR 库（使用 $JOBS 个并行任务）...${NC}"
echo -e "${YELLOW}注意: 首次构建可能需要较长时间（需要先构建 LLVM/MLIR 基础库）${NC}"

# 先构建基础依赖
echo -e "${YELLOW}构建基础依赖...${NC}"
ninja -j"$JOBS" LLVMSupport LLVMCore || true

# 构建 Polymer 库
echo -e "${YELLOW}构建 Polymer 库...${NC}"
ninja -j"$JOBS" PolymerSupport PolymerTargetISL PolymerTransforms

# 构建 mlir-systolic 需要的 MLIR 库
echo -e "${YELLOW}构建必要的 MLIR 库...${NC}"
ninja -j"$JOBS" \
  MLIRArithTransforms \
  MLIRArithValueBoundsOpInterfaceImpl \
  MLIRAffineTransformOps \
  MLIROptLib

# 验证构建
echo -e "${GREEN}验证构建结果...${NC}"
if [ -f "lib/libPolymerSupport.a" ] && \
   [ -f "lib/libPolymerTargetISL.a" ] && \
   [ -f "lib/libPolymerTransforms.a" ] && \
   [ -f "lib/libMLIRArithTransforms.a" ] && \
   [ -f "lib/libMLIRArithValueBoundsOpInterfaceImpl.a" ] && \
   [ -f "lib/libMLIRAffineTransformOps.a" ] && \
   [ -f "lib/libMLIROptLib.a" ]; then
    echo -e "${GREEN}✅ 构建成功！${NC}"
    ls -lh lib/libPolymer*.a lib/libMLIRArith*.a lib/libMLIRAffineTransformOps.a lib/libMLIROptLib.a 2>/dev/null | head -10
    echo ""
    echo -e "${GREEN}下一步:${NC}"
    echo -e "  构建 mlir-systolic: cd $PROJECT_ROOT && ./scripts/build-systolic.sh"
else
    echo -e "${RED}❌ 构建失败：缺少必需的库文件${NC}"
    exit 1
fi


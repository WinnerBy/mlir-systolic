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

# 解析参数：支持 -j/--jobs 指定并行度（优先级：命令行 > 环境变量 > 内存检测）
JOBS_OVERRIDE=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        -j|--jobs)
            JOBS_OVERRIDE="$2"; shift 2 ;;
        --)
            shift; break ;;
        *)
            # 其余参数目前忽略
            shift ;;
    esac
done

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
        JOBS=1; JOBS_SRC="mem";
    elif [[ "$AVAIL_MEM" -lt 8 ]]; then
        JOBS=1; JOBS_SRC="mem";
    elif [[ "$AVAIL_MEM" -lt 16 ]]; then
        JOBS=2; JOBS_SRC="mem";
    else
        JOBS=4; JOBS_SRC="mem";
    fi
fi

if ! [[ "$JOBS" =~ ^[0-9]+$ ]] || [[ "$JOBS" -le 0 ]]; then
    echo -e "${YELLOW}无效的并行度: $JOBS，回退为 2${NC}"
    JOBS=2; JOBS_SRC="fallback";
fi
echo -e "${GREEN}使用 $JOBS 个并行任务（来源: ${JOBS_SRC}）${NC}"

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

# 配置 CMake（使用 Ninja 生成器）
echo -e "${GREEN}配置 CMake（Ninja）...${NC}"
cmake -G Ninja .. \
  -DMLIR_DIR="$POLYGEIST_BUILD/lib/cmake/mlir" \
  -DLLVM_DIR="$POLYGEIST_BUILD/lib/cmake/llvm" \
  -DPOLYGEIST_BUILD="$POLYGEIST_BUILD"

# 构建 mlir-systolic（Ninja）
echo -e "${GREEN}构建 mlir-systolic（使用 $JOBS 个并行任务）...${NC}"
ninja -j"$JOBS"

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


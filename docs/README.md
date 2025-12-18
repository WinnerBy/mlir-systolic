# mlir-systolic 文档

## 核心文档

- **[BUILD_STEPS.md](BUILD_STEPS.md)** - 完整的构建步骤指南（必读）
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - 项目架构说明
- **[DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md)** - 开发指南
- **[AUTOSA_ARCHITECTURE.md](AUTOSA_ARCHITECTURE.md)** - AutoSA 架构参考
- **[comparison_with_autosa.md](comparison_with_autosa.md)** - 与 AutoSA 的对比

## 快速开始

1. 构建 Polygeist（包含 LLVM/MLIR/Polymer）
   ```bash
   cd third_party/Polygeist
   mkdir -p build && cd build
   cmake -G Ninja ../llvm-project/llvm \
     -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
     -DLLVM_EXTERNAL_PROJECTS="polygeist" \
     -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=.. \
     -DCMAKE_BUILD_TYPE=Release \
     -DPOLYGEIST_ENABLE_POLYMER=1 \
     -DPOLYGEIST_POLYMER_ENABLE_ISL=1
   ninja PolymerTransforms
   ```

2. 配置 mlir-systolic
   ```bash
   export POLYGEIST_BUILD=$(pwd)/third_party/Polygeist/build
   cd build
   cmake ..
   cmake --build . -j$(nproc)
   ```

详细步骤请参考 [BUILD_STEPS.md](BUILD_STEPS.md)

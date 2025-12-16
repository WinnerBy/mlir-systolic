# 第三方依赖

本目录包含项目的第三方依赖，通过 Git Submodule 管理。

## Polygeist

Polygeist 是 LLVM 项目的一部分，提供了 C/C++ 到 MLIR 的转换工具，以及 Polymer 多面体分析工具。

### 使用 Submodule

**首次克隆项目**:
```bash
git clone --recursive <repository-url>
```

**如果已经克隆了项目，需要初始化 submodule**:
```bash
git submodule update --init --recursive
```

**更新 submodule 到最新版本**:
```bash
git submodule update --remote third_party/Polygeist
```

**切换到特定版本**:
```bash
cd third_party/Polygeist
git checkout <commit-hash-or-tag>
cd ../..
git add third_party/Polygeist
git commit -m "Update Polygeist to <version>"
```

### 构建 Polygeist

Polygeist 需要先构建才能使用 Polymer 库：

```bash
cd third_party/Polygeist
mkdir build && cd build
cmake .. \
  -DMLIR_DIR=<path-to-mlir>/lib/cmake/mlir \
  -DLLVM_DIR=<path-to-llvm>/lib/cmake/llvm \
  -GNinja
ninja
```

构建完成后，设置环境变量（可选，CMake 会自动检测）：
```bash
export POLYGEIST_BUILD=$(pwd)  # 在 build 目录中
```

### 使用外部 Polygeist

如果不想使用 submodule，也可以使用外部已构建的 Polygeist：

```bash
export POLYGEIST_BUILD=/path/to/Polygeist/build
```

CMake 会优先使用 submodule，如果找不到则使用 `POLYGEIST_BUILD` 环境变量。

### 当前版本

当前使用的 Polygeist commit: `77c04bb2a7a2406ca9480bcc9e729b07d2c8d077`

查看最新版本: https://github.com/llvm/Polygeist


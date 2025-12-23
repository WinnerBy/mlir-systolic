# 项目目录结构

> **最后更新**: 2024-12

---

## 目录结构概览

```
mlir-systolic/
├── CMakeLists.txt              # 主 CMake 配置文件
├── README.md                    # 项目主 README
├── .gitignore                   # Git 忽略规则
│
├── include/                     # 头文件目录
│   └── systolic/
│       ├── Dialect/            # Dialect 定义（HLS, SystolicDataflow）
│       ├── Analysis/           # 分析接口
│       └── Transforms/         # Pass 声明
│
├── lib/                         # 库实现目录
│   ├── Dialect/                # Dialect 实现
│   ├── Analysis/               # 分析实现
│   ├── Transforms/             # Pass 实现
│   └── Translation/            # 代码生成器
│
├── tools/                       # 工具目录
│   ├── systolic-opt/          # MLIR 优化工具
│   └── systolic-translate/     # MLIR 翻译工具
│
├── test/                        # 测试目录
│   ├── matmul/                  # 矩阵乘法测试用例
│   │   ├── matmul.mlir         # 测试输入 MLIR
│   │   └── README.md           # 测试说明
│   ├── reference-samples/      # AutoSA 生成的参考代码
│   │   ├── mm_st3_*.cpp        # 不同配置的参考 HLS C++ 文件
│   │   └── README.md           # 参考样本说明
│   ├── output/                  # 测试输出目录（Git 忽略）
│   │   ├── *.cpp               # 生成的 HLS C++ 代码
│   │   ├── *.mlir              # 中间 MLIR 文件
│   │   └── *.log               # 测试日志
│   ├── *.sh                     # 测试脚本
│   ├── *.mlir                   # 测试输入 MLIR 文件
│   └── TEST_RESULTS.md          # 测试结果文档
│
├── scripts/                     # 脚本目录
│   ├── build-polygeist.sh      # Polygeist 构建脚本
│   ├── build-systolic.sh        # mlir-systolic 构建脚本
│   ├── generate_autosa_tests.sh # AutoSA 测试用例生成脚本
│   ├── generate_other_kernels.sh # 其他 Kernel 生成脚本
│   └── check_random_access.sh  # 随机读取问题检查脚本
│
├── docs/                        # 文档目录
│   ├── README.md                # 文档索引
│   ├── BUILD_STEPS.md           # 构建指南
│   ├── QUICK_REFERENCE.md       # 快速参考
│   ├── PROJECT_STATUS.md        # 项目状态
│   ├── ARCHITECTURE.md          # 架构设计
│   ├── DEVELOPMENT_GUIDE.md     # 开发指南
│   ├── AUTOSA_*.md              # AutoSA 相关文档
│   ├── RANDOM_ACCESS_*.md       # 随机读取问题文档
│   ├── ALLO_*.md                # Allo 集成文档
│   └── archive/                 # 归档文档（过时但保留）
│
├── autosa_hls_output/          # AutoSA 生成的 HLS C 样例（待标注参数）
│   ├── kernel_mttkrp.cpp       # MTTKRP 运算
│   ├── kernel_ttmc.cpp         # TTMc 运算
│   ├── kernel_cnn_small.cpp    # CNN 小规模
│   ├── kernel_cnn_large.cpp    # CNN 大规模
│   └── kernel_lu.cpp           # LU 分解
│
├── build/                       # 构建目录（Git 忽略）
│   └── ...                      # 编译产物
│
└── third_party/                 # 第三方依赖
    └── Polygeist/               # Polygeist submodule
        ├── llvm-project/        # LLVM/MLIR submodule
        └── build/               # Polygeist 构建目录（Git 忽略）
```

---

## 目录说明

### 核心代码目录

- **`include/`**: 所有头文件，按功能分类
- **`lib/`**: 所有实现文件，与 `include/` 结构对应
- **`tools/`**: 命令行工具（systolic-opt, systolic-translate）

### 测试目录 (`test/`)

- **`matmul/`**: 矩阵乘法测试用例的 MLIR 输入文件
- **`reference-samples/`**: AutoSA 生成的参考 HLS C++ 代码，用于对比验证
- **`output/`**: 测试生成的输出文件（自动生成，Git 忽略）
- **`*.sh`**: 测试脚本
- **`*.mlir`**: 测试输入 MLIR 文件（根目录级别）
- **`TEST_RESULTS.md`**: 测试结果总结

### 脚本目录 (`scripts/`)

- **构建脚本**: `build-*.sh` - 用于构建项目
- **测试生成脚本**: `generate_*.sh` - 用于生成 AutoSA 测试用例
- **检查脚本**: `check_*.sh` - 用于检查代码问题

### 文档目录 (`docs/`)

- **核心文档**: 构建、架构、开发指南等
- **AutoSA 相关**: 分析、参数说明、测试生成指南
- **问题分析**: 随机读取问题等
- **归档文档**: `archive/` 目录存放过时但保留的文档

### 其他目录

- **`autosa_hls_output/`**: AutoSA 生成的 HLS C 样例，待标注参数后归档
- **`build/`**: 构建产物（Git 忽略）
- **`third_party/`**: 第三方依赖（submodules）

---

## 文件分类

### 需要保留的文件

- ✅ 所有源代码文件（`include/`, `lib/`, `tools/`）
- ✅ 所有测试用例和参考样本（`test/matmul/`, `test/reference-samples/`）
- ✅ 所有脚本（`scripts/`）
- ✅ 所有文档（`docs/`）
- ✅ 配置文件（`CMakeLists.txt`, `.gitignore`, `README.md`）

### 自动生成的文件（Git 忽略）

- ❌ `build/` 目录及其内容
- ❌ `test/output/` 目录及其内容
- ❌ `*.log` 文件
- ❌ `*.tmp`, `*.temp` 文件
- ❌ 编译产物（`.o`, `.a`, `.so` 等）

### 临时文件（可删除）

- ⚠️ 根目录的临时文档（如 `CLEANUP_SUMMARY.md`）
- ⚠️ `test/` 目录下的测试输出文件（应移到 `test/output/`）

---

## 维护建议

1. **测试输出**: 所有测试生成的 `.cpp`, `.mlir`, `.log` 文件应放在 `test/output/` 目录
2. **文档更新**: 过时文档应移到 `docs/archive/` 而不是直接删除
3. **构建产物**: 确保 `.gitignore` 正确配置，避免提交构建产物
4. **临时文件**: 定期清理根目录和测试目录的临时文件

---

## 相关文档

- [构建指南](docs/BUILD_STEPS.md)
- [开发指南](docs/DEVELOPMENT_GUIDE.md)
- [项目状态](docs/PROJECT_STATUS.md)

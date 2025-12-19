# mlir-systolic 文档索引

> **最后更新**: 2024-12

---

## 📚 快速导航

### 🚀 快速开始
- **[快速参考](QUICK_REFERENCE.md)** ⭐ - 常用命令和参数配置快速查找
- **[构建指南](BUILD_STEPS.md)** ⭐ - 详细的构建步骤和故障排除

### 📖 核心文档
- **[项目进展总结](PROJECT_STATUS.md)** ⭐ - 项目当前状态和最新更新
- **[架构设计](ARCHITECTURE.md)** - 项目架构和设计理念
- **[开发指南](DEVELOPMENT_GUIDE.md)** - 开发流程和代码结构

### 🔬 AutoSA 相关
- **[AutoSA 分析](AUTOSA_ANALYSIS.md)** ⭐ - AutoSA 架构、算法、参数影响分析
- **[AutoSA Spacetime 分析](AUTOSA_SPACETIME_ANALYSIS.md)** - Spacetime 参数详细分析（0-5）
- **[AutoSA 测试生成指南](AUTOSA_TEST_GENERATION_GUIDE.md)** - 生成不同 Spacetime 和参数的测试用例
- **[随机读取问题分析](RANDOM_ACCESS_ISSUE_ANALYSIS.md)** - MTTKRP 等 Kernel 的随机读取问题分析和修复方法
- **[三个问题解决方案](THREE_QUESTIONS_SOLUTION.md)** - 测试用例生成、其他 Kernel 验证、随机读取问题检查

### 🔧 技术文档
- **[AutoSA 架构参考](AUTOSA_ARCHITECTURE.md)** - AutoSA 架构详细分析
- **[与 AutoSA 对比](comparison_with_autosa.md)** - mlir-systolic 与 AutoSA 的功能对比
- **[Allo 集成分析](ALLO_INTEGRATION_ANALYSIS.md)** - Allo 项目集成方案分析
- **[Polymer 库要求](POLYMER_LIBRARIES_REQUIRED.md)** - Polymer 库依赖说明

---

## 📋 文档分类

### 构建和开发
| 文档 | 说明 | 优先级 |
|------|------|--------|
| [BUILD_STEPS.md](BUILD_STEPS.md) | 完整的构建步骤指南 | ⭐⭐⭐ |
| [QUICK_REFERENCE.md](QUICK_REFERENCE.md) | 快速参考指南 | ⭐⭐⭐ |
| [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) | 开发指南 | ⭐⭐ |
| [PROJECT_STATUS.md](PROJECT_STATUS.md) | 项目进展总结 | ⭐⭐⭐ |

### 架构和设计
| 文档 | 说明 | 优先级 |
|------|------|--------|
| [ARCHITECTURE.md](ARCHITECTURE.md) | 项目架构设计 | ⭐⭐⭐ |
| [AUTOSA_ANALYSIS.md](AUTOSA_ANALYSIS.md) | AutoSA 详细分析 | ⭐⭐⭐ |
| [AUTOSA_ARCHITECTURE.md](AUTOSA_ARCHITECTURE.md) | AutoSA 架构参考 | ⭐⭐ |

### AutoSA 参数和测试
| 文档 | 说明 | 优先级 |
|------|------|--------|
| [AUTOSA_SPACETIME_ANALYSIS.md](AUTOSA_SPACETIME_ANALYSIS.md) | Spacetime 参数分析 | ⭐⭐⭐ |
| [AUTOSA_TEST_GENERATION_GUIDE.md](AUTOSA_TEST_GENERATION_GUIDE.md) | 测试用例生成指南 | ⭐⭐⭐ |
| [RANDOM_ACCESS_ISSUE_ANALYSIS.md](RANDOM_ACCESS_ISSUE_ANALYSIS.md) | 随机读取问题分析 | ⭐⭐ |
| [THREE_QUESTIONS_SOLUTION.md](THREE_QUESTIONS_SOLUTION.md) | 三个问题解决方案 | ⭐⭐ |

### 集成和分析
| 文档 | 说明 | 优先级 |
|------|------|--------|
| [ALLO_INTEGRATION_ANALYSIS.md](ALLO_INTEGRATION_ANALYSIS.md) | Allo 集成分析 | ⭐ |
| [comparison_with_autosa.md](comparison_with_autosa.md) | 与 AutoSA 对比 | ⭐⭐ |
| [POLYMER_LIBRARIES_REQUIRED.md](POLYMER_LIBRARIES_REQUIRED.md) | Polymer 库要求 | ⭐ |

---

## 📁 历史文档

过时的文档已移动到 `archive/` 目录，包括：
- 旧的构建指南（已合并到 BUILD_STEPS.md）
- 旧的实现总结（已过时）
- 旧的测试结果（已过时）

如需查看历史文档，请访问 `docs/archive/` 目录。

---

## 🔍 文档查找指南

### 我想...
- **快速开始构建** → [BUILD_STEPS.md](BUILD_STEPS.md)
- **查找常用命令** → [QUICK_REFERENCE.md](QUICK_REFERENCE.md)
- **了解项目状态** → [PROJECT_STATUS.md](PROJECT_STATUS.md)
- **理解架构设计** → [ARCHITECTURE.md](ARCHITECTURE.md)
- **生成测试用例** → [AUTOSA_TEST_GENERATION_GUIDE.md](AUTOSA_TEST_GENERATION_GUIDE.md)
- **了解 Spacetime 参数** → [AUTOSA_SPACETIME_ANALYSIS.md](AUTOSA_SPACETIME_ANALYSIS.md)
- **检查随机读取问题** → [RANDOM_ACCESS_ISSUE_ANALYSIS.md](RANDOM_ACCESS_ISSUE_ANALYSIS.md)
- **了解 AutoSA 架构** → [AUTOSA_ANALYSIS.md](AUTOSA_ANALYSIS.md)

---

## 📝 文档维护

- 文档按功能分类，便于查找
- 过时文档已移动到 `archive/` 目录
- 核心文档保持更新，反映最新状态
- 如有问题或建议，请提交 Issue 或 PR

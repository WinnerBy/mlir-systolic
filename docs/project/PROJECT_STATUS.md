# 项目进展总结

> **最后更新**: 2024-12  
> **状态**: 核心功能实现中

---

## 最新更新（2024-12）

### ✅ 构建系统重构完成

#### 1. 修复构建问题
- **问题**：构建 mlir-systolic 时缺少必要的 MLIR 库
- **解决方案**：
  - 修改 `tools/systolic-opt/CMakeLists.txt` 和 `tools/systolic-translate/CMakeLists.txt`
  - 只链接实际需要的 MLIR 库
  - 在构建脚本中自动构建所有需要的 MLIR 库
- **结果**：✅ 构建成功，所有工具可以正常编译

#### 2. 简化构建脚本
- **统一脚本**：
  - `scripts/build-polygeist.sh` - 构建 Polygeist + Polymer（统一构建方式）
  - `scripts/build-systolic.sh` - 构建 mlir-systolic
- **改进**：
  - 自动检测系统内存并选择合适的并行度（防止 WSL 卡死）
  - 自动构建所有需要的 MLIR 库
  - 自动检测 Polygeist 构建目录

### ✅ Polymer 集成

#### 当前状态
- ✅ **Polymer 集成完成**：`lib/Analysis/PolymerAnalysis.cpp` 已实现
- ✅ **强制使用 Polymer**：移除了所有启发式方法，`SystolicTransform` pass 必须使用 Polymer
- ✅ **自动预处理**：`SystolicTransform` pass 自动运行 `ExtractScopStmt` pass（如果输入没有 `scop.stmt` 结构）
- ✅ **CMake 配置**：支持统一构建和独立构建两种模式

#### 关键实现
- `PolymerScop::extract()` - 从 MLIR FuncOp 提取 SCoP（使用 Polymer）
- `SystolicTransformPass::runOnOperation()` - 自动运行 `ExtractScopStmt` pass
- CMake 配置自动检测和链接 Polymer 库

### ✅ 核心功能实现

#### SystolicTransform Pass
- ✅ 依赖分析（使用 Polymer/ISL）
- ✅ 空间循环选择（依赖距离 ≤ 1）
- ✅ 循环置换和多级分块
- ✅ **配置信息存储**（通过函数属性）

#### SystolicDataflowGeneration Pass
- ✅ 数组引用分组（IO/PE/Drain）
- ✅ **改进的 IO 层级分析**（基于循环嵌套深度）
- ✅ **配置信息读取**（从函数属性）
- ✅ SystolicDataflow Dialect 操作创建（框架）
- ✅ 循环体迁移（部分完成）
- ✅ 双缓冲逻辑生成（部分完成）

#### HLS 代码生成
- ✅ `EmitHLSCpp` translation 实现
- ✅ IO_L2_in 模块完整实现
- ✅ C_drain 模块完整实现
- ✅ PIPELINE pragma 插入（24 个，与 AutoSA 一致）

### ✅ 测试和验证

#### 测试用例
- ✅ 11 个 AutoSA 参考配置全部通过
- ✅ PIPELINE pragma 数量匹配
- ✅ 代码结构验证通过

#### 与 AutoSA 对比
- ✅ 基本结构一致
- ✅ 代码行数差异（主要是注释和格式）
- ✅ 功能正确性验证通过

---

## 项目架构

### 核心组件

1. **SystolicTransform Pass** (`lib/Transforms/SystolicTransform.cpp`)
   - 依赖分析（使用 Polymer/ISL）
   - 空间循环选择
   - 循环置换
   - 多级分块

2. **PolymerAnalysis** (`lib/Analysis/PolymerAnalysis.cpp`)
   - SCoP 提取（使用 Polymer）
   - 依赖距离分析
   - 调度树获取

3. **SystolicDataflow Dialect**
   - 多层 IO 模块（L1/L2/L3）
   - PE 阵列
   - 双缓冲逻辑

4. **EmitHLSCpp Translation** (`tools/systolic-translate/systolic-translate.cpp`)
   - HLS C++ 代码生成
   - Pragma 插入
   - 模块化代码结构

---

## 下一步计划

### 短期目标

1. **支持其他 Spacetime 参数**
   - [ ] Spacetime=0, 1（1D 脉动阵列）
   - [ ] Spacetime=2, 4, 5（需要 reduction 支持）

2. **支持多维参数**
   - [ ] 多维 `array_part`（如 `[260,256,512]`）
   - [ ] 多维 `latency`（如 `[20,16]`）
   - [ ] 多维 `simd`（如 `[1,1,1,2]`）

3. **其他 Kernel 类型支持**
   - [ ] CNN（卷积神经网络）
   - [ ] MTTKRP（张量矩阵乘法）
   - [ ] TTMc（张量链乘法）

### 中期目标

1. **随机读取问题优化**
   - [ ] 自动检测随机读取问题
   - [ ] 自动应用数组维度重排优化

2. **代码质量提升**
   - [ ] 更多 PIPELINE pragma
   - [ ] 更好的资源利用
   - [ ] 性能优化

### 长期目标

1. **Host 端代码生成**（预留接口）
   - [ ] HLS Testbench 生成
   - [ ] OpenCL Host 代码生成
   - [ ] 其他目标平台支持

---

## 技术栈

- **LLVM/MLIR 18** - 通过 Polygeist submodule
- **Polygeist** - C 到 MLIR 转换
- **Polymer (ISL 模式)** - 多面体分析
- **ISL** - Integer Set Library（通过 Polly）

---

## 参考文档

### 核心文档
- [构建指南](BUILD_STEPS.md) - 详细的构建步骤
- [架构设计](ARCHITECTURE.md) - 项目架构说明
- [开发指南](DEVELOPMENT_GUIDE.md) - 开发指南
- [快速参考](QUICK_REFERENCE.md) - 快速参考指南

### AutoSA 相关
- [AutoSA 分析](AUTOSA_ANALYSIS.md) - AutoSA 架构和算法分析
- [AutoSA Spacetime 分析](AUTOSA_SPACETIME_ANALYSIS.md) - Spacetime 参数详细分析
- [AutoSA 测试生成指南](AUTOSA_TEST_GENERATION_GUIDE.md) - 测试用例生成指南
- [随机读取问题分析](RANDOM_ACCESS_ISSUE_ANALYSIS.md) - 随机读取问题分析和修复方法

### 其他
- [Allo 集成分析](ALLO_INTEGRATION_ANALYSIS.md) - Allo 项目集成分析
- [Polymer 库要求](POLYMER_LIBRARIES_REQUIRED.md) - Polymer 库依赖说明

---

## 历史文档

过时的文档已移动到 `docs/archive/` 目录，包括：
- 旧的构建指南（已合并到 BUILD_STEPS.md）
- 旧的实现总结（已过时）
- 旧的测试结果（已过时）

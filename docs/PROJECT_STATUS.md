# 项目进展总结

## 最新更新（2024-12）

### ✅ 构建系统重构完成

#### 1. 修复构建问题
- **问题**：构建 mlir-systolic 时缺少必要的 MLIR 库（如 `MLIRArithTransforms`, `MLIRArithValueBoundsOpInterfaceImpl`, `MLIROptLib`）
- **解决方案**：
  - 修改 `tools/systolic-opt/CMakeLists.txt` 和 `tools/systolic-translate/CMakeLists.txt`
  - 移除 `${dialect_libs}` 和 `${conversion_libs}`，只链接实际需要的 MLIR 库
  - 在构建脚本中自动构建所有需要的 MLIR 库
- **结果**：✅ 构建成功，所有工具可以正常编译

#### 2. 简化构建脚本
- **删除冗余脚本**：
  - `build-polygeist-dev.sh`
  - `build-systolic-dev.sh`
  - `build-unified.sh`
  - `build-polygeist-safe.sh`
- **创建统一脚本**：
  - `scripts/build-polygeist.sh` - 构建 Polygeist + Polymer（统一构建方式）
  - `scripts/build-systolic.sh` - 构建 mlir-systolic
- **改进**：
  - 自动检测系统内存并选择合适的并行度（防止 WSL 卡死）
  - 自动构建所有需要的 MLIR 库
  - 自动检测 Polygeist 构建目录

#### 3. 更新文档
- **README.md**：
  - 更新构建说明，与脚本保持一致
  - 参考 Polygeist README Option 2（统一构建方式）
  - 添加故障排除指南
- **docs/BUILD_STEPS.md**：
  - 完全重写，参考 Polygeist 和 Polymer README
  - 提供详细的构建步骤和故障排除
  - 包含手动构建和脚本构建两种方式

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

### ✅ 构建系统

#### 统一构建方式
- 使用 Polygeist 的 `llvm-project` submodule
- 一次性构建 LLVM/MLIR/Polly/Polygeist/Polymer
- 确保版本兼容性

#### 构建脚本功能
- 自动内存检测和并行度选择
- 自动构建所有依赖库
- 自动路径检测和配置

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

## 下一步计划

### Phase 1: 完善 Polymer 集成
- [ ] 完善依赖距离提取
- [ ] 实现基于调度树的 task 分解
- [ ] 测试各种循环嵌套模式

### Phase 2: 数据流生成
- [ ] 完善 `SystolicDataflowGeneration` pass
- [ ] 实现多层 IO 模块生成
- [ ] 实现双缓冲逻辑

### Phase 3: HLS 代码生成
- [ ] 完善 `EmitHLSCpp` translation
- [ ] 实现 Pragma 插入
- [ ] 端到端测试

## 技术栈

- **LLVM/MLIR 18** - 通过 Polygeist submodule
- **Polygeist** - C 到 MLIR 转换
- **Polymer (ISL 模式)** - 多面体分析
- **ISL** - Integer Set Library（通过 Polly）

## 参考文档

- [构建指南](BUILD_STEPS.md) - 详细的构建步骤
- [Polygeist README](../third_party/Polygeist/README.md) - Polygeist 构建说明
- [Polymer README](../third_party/Polygeist/tools/polymer/README.md) - Polymer 使用说明


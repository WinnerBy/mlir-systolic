# mlir-systolic 当前状态总结

> **最后更新**: 2025-01-XX  
> **总体状态**: 🟡 **Polymer 集成进行中，运行时崩溃待修复**

---

## 📊 项目整体状态

### ✅ 已完成

1. **项目架构设计**
   - ✅ 基于 AutoSA 的架构分析完成
   - ✅ SystolicDataflow Dialect 设计完成
   - ✅ 变换流程设计完成

2. **基础功能实现**
   - ✅ 循环分块（Tiling）实现
   - ✅ PE 阵列大小计算
   - ✅ 基本变换功能正常

3. **Polymer 集成 - 构建和配置**
   - ✅ Polygeist submodule 配置完成
   - ✅ CMake 配置完成（自动检测、库链接）
   - ✅ 编译成功（所有代码编译通过）
   - ✅ ExtractScopStmt 转换集成完成

4. **Polymer 集成 - 代码实现**
   - ✅ SCoP 提取框架完成
   - ✅ 使用 `createIslFromFuncOp` 和 `ExtractScopStmt`
   - ✅ 调度树计算框架完成
   - ✅ 依赖距离计算框架完成

### ⚠️ 当前问题

1. **运行时崩溃**
   - **位置**: `polymer::IslScop::initializeSymbolTable` 内部
   - **堆栈**: `PresburgerSpace::getNumDimAndSymbolVars()` 崩溃
   - **状态**: 即使使用 `ExtractScopStmt` 转换后，`createIslFromFuncOp` 内部仍崩溃
   - **可能原因**: 
     - `FlatAffineValueConstraints` 对象内部状态问题
     - MLIR API 版本兼容性问题
     - Polymer 内部 context 构建问题

2. **依赖计算未完成**
   - 受限于 Polymer API（无法直接访问内部访问关系）
   - 需要扩展 Polymer API 或使用替代方法

3. **距离向量解析未完成**
   - 需要实现 ISL map 迭代逻辑

---

## 🔧 技术实现细节

### Polymer 集成流程

```
affine.for 循环
    ↓
ExtractScopStmt 转换
    ↓
scop.stmt 格式（函数调用）
    ↓
createIslFromFuncOp
    ↓
IslScop 对象
    ↓
[崩溃在 initializeSymbolTable]
```

### 当前实现方式

1. **SCoP 提取** (`PolymerScop::extract`)
   - 检查函数是否有 `scop.stmt` 结构
   - 如果没有，运行 `ExtractScopStmt` 转换
   - 调用 `polymer::createIslFromFuncOp` 构建 SCoP
   - **问题**: 在 `initializeSymbolTable` 时崩溃

2. **调度树计算** (`PolymerScop::computeSchedule`)
   - 从 SCoP 获取已有调度树
   - 框架完成，但受限于 SCoP 提取问题

3. **依赖距离计算** (`PolymerScop::computeDependenceDistances`)
   - 使用 ISL API 计算距离向量
   - 框架完成，但受限于依赖计算未完成

---

## 📋 下一步计划

### 优先级 1: 修复运行时崩溃

**目标**: 让 `createIslFromFuncOp` 能够成功运行

**可能方案**:
1. 深入调试 `initializeSymbolTable` 崩溃原因
2. 检查 `ExtractScopStmt` 生成的函数结构是否正确
3. 查看 Polymer 测试用例，了解正确的使用方式
4. 检查 MLIR 版本兼容性

### 优先级 2: 完善依赖计算

**目标**: 实现完整的依赖分析

**方案**:
- 选项 A: 扩展 Polymer API（推荐）
- 选项 B: 从 MLIR 重建访问关系
- 选项 C: 使用替代方法

### 优先级 3: 完善距离向量解析

**目标**: 从 ISL map 中提取距离向量

**步骤**:
1. 实现 `isl_union_map_foreach_map()` 迭代
2. 提取距离向量
3. 计算 min/max 距离

---

## 📁 代码结构

### 关键文件

- `lib/Analysis/PolymerAnalysis.cpp` - Polymer 集成实现
- `lib/Transforms/SystolicTransform.cpp` - 主变换逻辑
- `include/systolic/Analysis/PolymerAnalysis.h` - Polymer 接口定义
- `CMakeLists.txt` - 构建配置（Polymer 集成）

### 测试文件

- `test/matmul/matmul.mlir` - 矩阵乘法测试用例

---

## 🔍 调试信息

### 编译状态
- ✅ 编译成功
- ✅ 无编译错误
- ✅ Polymer 库正确链接

### 运行时状态
- ❌ `createIslFromFuncOp` 内部崩溃
- ❌ `initializeSymbolTable` 崩溃
- ⚠️ `ExtractScopStmt` 可能未正确运行

### 调试命令
```bash
# 运行测试（会崩溃）
POLYGEIST_BUILD=/home/user/work/mlir-systolic/third_party/Polygeist/build \
  ./build/bin/systolic-opt test/matmul/matmul.mlir --systolic-transform

# 查看调试输出
POLYGEIST_BUILD=... ./build/bin/systolic-opt test/matmul/matmul.mlir \
  --systolic-transform -debug-only=polymer-analysis
```

---

## 📚 参考文档

- `docs/AUTOSA_ANALYSIS.md` - AutoSA 详细分析
- `docs/AUTOSA_ARCHITECTURE.md` - AutoSA 架构说明
- `docs/POLYMER_INTEGRATION.md` - Polymer 集成方案

---

**注意**: 本文档应定期更新以反映最新状态。


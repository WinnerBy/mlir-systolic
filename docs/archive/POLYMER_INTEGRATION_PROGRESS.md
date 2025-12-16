# Polymer 集成进展

> **更新日期**: 2025-12-10  
> **状态**: ⚠️ **基础框架已就绪，待完善实现**

---

## ✅ 已完成的工作

### 1. 创建 Polymer 分析接口

**文件**:
- `include/systolic/Analysis/PolymerAnalysis.h` - 接口定义
- `lib/Analysis/PolymerAnalysis.cpp` - 实现框架

**功能**:
- `PolymerScop` 类：封装 Polymer 的 SCoP 提取
- `ScheduleTreeInfo`：调度树信息结构
- `computeDependenceDistancesWithPolymer()`：使用 Polymer 计算依赖距离
- `isPolymerAvailable()`：检查 Polymer 是否可用

### 2. 链接 Polymer 库

**CMakeLists.txt** (`lib/Analysis/CMakeLists.txt`):
```cmake
# Link Polymer libraries if available
if(SYSTOLIC_ENABLE_POLYMER AND DEFINED POLYGEIST_BUILD_DIR)
  target_link_libraries(SystolicAnalysis PRIVATE
    ${POLYGEIST_BUILD_DIR}/lib/libPolymerSupport.a
    ${POLYGEIST_BUILD_DIR}/lib/libPolymerTargetISL.a
  )
  target_compile_definitions(SystolicAnalysis PRIVATE SYSTOLIC_ENABLE_POLYMER=1)
endif()
```

### 3. 集成到 SystolicTransform Pass

**更新** (`lib/Transforms/SystolicTransform.cpp`):
- `analyzeDependenceDistances()` 现在优先使用 Polymer
- 如果 Polymer 不可用，回退到简化的启发式方法
- 自动检测 Polymer 可用性

---

## ⚠️ 待完善的工作

### 1. 完善 SCoP 提取

**当前状态**: 基础框架已创建，但 SCoP 提取逻辑不完整

**需要实现**:
```cpp
std::unique_ptr<PolymerScop> PolymerScop::extract(func::FuncOp func) {
  polymer::IslScop *scop = new polymer::IslScop();
  scop->initializeSymbolTable(func, nullptr);
  
  // TODO: 需要实现：
  // 1. 遍历 affine.for 循环
  // 2. 为每个语句添加 domain relation
  // 3. 添加 access relations (read/write)
  // 4. 构建调度树
  
  return std::unique_ptr<PolymerScop>(new PolymerScop(scop));
}
```

**参考**: Polymer 的 `extractScopStmt()` 函数

### 2. 实现调度树计算

**当前状态**: 可以获取已有调度树，但无法计算新的调度树

**需要实现**:
```cpp
isl_schedule *PolymerScop::computeSchedule() {
  polymer::IslScop *scop = static_cast<polymer::IslScop*>(this->scop);
  
  // TODO: 如果调度树不存在，需要计算
  // 1. 获取 domain 和 dependences
  // 2. 调用 isl_schedule_compute() 或类似函数
  // 3. 返回计算的调度树
}
```

### 3. 实现依赖距离计算

**当前状态**: 框架已就绪，但依赖距离提取逻辑未实现

**需要实现**:
```cpp
LogicalResult computeDependenceDistancesWithPolymer(...) {
  // Step 1: Extract SCoP ✅
  // Step 2: Compute schedule tree ⚠️ (部分实现)
  // Step 3: Compute dependences ⚠️ (待实现)
  // Step 4: Compute dependence distances ⚠️ (待实现)
  // Step 5: Extract distances for each loop ⚠️ (待实现)
}
```

### 4. 调度树分析

**当前状态**: 框架已就绪，但分析逻辑未实现

**需要实现**:
```cpp
LogicalResult analyzeScheduleTree(isl_schedule *schedule,
                                   ScheduleTreeInfo &info) {
  // TODO: 遍历调度树
  // 1. 提取每个循环维度的依赖距离
  // 2. 识别空间循环候选（距离 <= 1）
  // 3. 填充 ScheduleTreeInfo
}
```

---

## 📋 下一步计划

### 优先级 1: 完善 SCoP 提取

**目标**: 能够从 MLIR function 正确提取 SCoP

**步骤**:
1. 研究 Polymer 的 `extractScopStmt()` API
2. 实现循环遍历和语句提取
3. 添加 domain 和 access relations
4. 测试 SCoP 提取是否正确

### 优先级 2: 实现调度树计算

**目标**: 能够计算 ISL 调度树

**步骤**:
1. 研究 ISL 的调度树计算 API
2. 实现依赖关系计算
3. 调用 ISL 调度树计算函数
4. 验证调度树结构

### 优先级 3: 实现依赖距离提取

**目标**: 从调度树中提取每个循环的依赖距离

**步骤**:
1. 遍历调度树节点
2. 提取距离向量
3. 计算每个循环维度的 min/max 距离
4. 标记空间循环候选

### 优先级 4: 基于调度树分解 task

**目标**: 使用调度树信息分解成多个独立函数

**步骤**:
1. 分析调度树节点边界
2. 为每个节点生成独立的 SystolicDataflow 操作
3. 在 SystolicDataflowGeneration Pass 中使用

---

## 🔍 调试建议

### 检查 Polymer 是否可用

```cpp
if (systolic::isPolymerAvailable()) {
  llvm::dbgs() << "Polymer is available\n";
} else {
  llvm::dbgs() << "Polymer is NOT available\n";
}
```

### 测试 SCoP 提取

```cpp
auto scop = PolymerScop::extract(func);
if (scop && scop->isValid()) {
  llvm::dbgs() << "SCoP extraction successful\n";
} else {
  llvm::dbgs() << "SCoP extraction failed\n";
}
```

### 验证库链接

```bash
# 检查是否链接了 Polymer 库
nm build/lib/libSystolicAnalysis.a | grep -i polymer
```

---

## 📚 参考资源

1. **Polymer 头文件**: `/home/user/work/Polygeist/tools/polymer/include/`
2. **IslScop.h**: Polymer 的 SCoP 类定义
3. **PolymerUtils.h**: 工具函数（如 `extractScopStmt()`）
4. **ISL 文档**: ISL 调度树和依赖分析 API

---

## 总结

**当前状态**: 
- ✅ Polymer 库已链接
- ✅ 分析接口框架已创建
- ✅ 集成到 Transform Pass
- ⚠️ 核心实现逻辑待完善

**下一步**: 优先完善 SCoP 提取，这是所有后续分析的基础。


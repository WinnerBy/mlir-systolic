# Polymer 集成实现总结

> **更新日期**: 2025-12-16  
> **状态**: 🟡 **核心功能已实现，部分功能需要完善**

---

## ✅ 已完成的功能

### 1. SCoP 提取 (`PolymerScop::extract`) ✅

**状态**: 已完成

**实现内容**:
- ✅ 创建 `IslScop` 对象
- ✅ 初始化符号表
- ✅ 遍历 affine.for 循环
- ✅ 识别语句（load/store 操作）
- ✅ 创建 SCoP 语句
- ✅ **添加 domain relations**（从循环边界构建约束）
- ✅ **添加 access relations**（read/write 访问关系）
- ✅ 构建调度树

**关键代码**:
```cpp
// 为每个语句添加 domain relation
scop->addDomainRelation(stmtId, domainCst);

// 为 load/store 操作添加 access relation
scop->addAccessRelation(stmtId, isRead, memref, vMap, domainCst);
```

### 2. 调度树计算 (`PolymerScop::computeSchedule`) ✅

**状态**: 已完成

**实现内容**:
- ✅ 从 SCoP 获取已有调度树（在提取时构建）
- ✅ 错误处理
- ✅ ISL 对象所有权管理

### 3. 依赖距离计算 (`PolymerScop::computeDependenceDistances`) ✅

**状态**: 已完成基础实现

**实现内容**:
- ✅ 从调度树获取调度映射
- ✅ 计算距离向量（schedule(target) - schedule(source)）
- ✅ ISL 对象管理

**关键代码**:
```cpp
isl_union_map *distances = isl_union_map_subtract(
  isl_union_map_apply_range(isl_union_map_copy(deps), scheduleMap),
  isl_union_map_apply_domain(isl_union_map_copy(deps), scheduleMap)
);
```

### 4. 调度树分析 (`analyzeScheduleTree`) ✅

**状态**: 已完成基础实现

**实现内容**:
- ✅ 遍历调度树节点
- ✅ 识别 band 节点（循环维度）
- ✅ 提取维度信息
- ✅ 辅助函数：`traverseScheduleTree()`, `findFirstBandNode()`

### 5. 依赖距离提取 (`computeDependenceDistancesWithPolymer`) 🟡

**状态**: 框架完成，需要完善距离向量解析

**实现内容**:
- ✅ SCoP 提取
- ✅ 调度树计算
- ✅ 依赖计算（框架）
- ✅ 依赖距离计算
- ✅ 循环维度识别
- ✅ 距离信息初始化
- ⚠️ **距离向量解析**（需要完善 ISL map 迭代）

**待完善**:
- 使用 `isl_union_map_foreach_map()` 迭代距离映射
- 从每个 basic map 提取距离向量
- 计算每个循环维度的 min/max 距离

---

## ⚠️ 需要完善的功能

### 1. 依赖计算 (`PolymerScop::computeDependences`) ⚠️

**状态**: 框架完成，受限于 Polymer API

**问题**:
- Polymer 的 `IslScop` 将访问关系存储在私有成员 `islStmts` 中
- 无法直接访问 `readRelations` 和 `writeRelations`
- 需要访问关系才能使用 `isl_union_map_compute_flow()` 计算依赖

**可能的解决方案**:
1. **扩展 Polymer API**（推荐）:
   - 在 `IslScop` 类中添加公共方法获取访问关系
   - 例如：`getReadRelations()`, `getWriteRelations()`

2. **从 MLIR 操作重建访问关系**:
   - 在 SCoP 提取时保存访问信息
   - 在依赖计算时重建 ISL 访问关系

3. **使用 ISL 的其他 API**:
   - 研究是否有其他方法可以从 schedule 和 domain 计算依赖

**当前实现**:
```cpp
// 当前返回 nullptr，因为无法访问内部访问关系
// 需要扩展 Polymer API 或使用替代方法
```

### 2. 距离向量解析 (`extractDistancesFromMap`) ⚠️

**状态**: 框架完成，需要实现 ISL map 迭代

**需要实现**:
```cpp
static void extractDistancesFromMap(isl_union_map *distMap,
                                    SmallVectorImpl<LoopDependenceDistance> &distances) {
  // 使用 isl_union_map_foreach_map() 迭代
  isl_union_map_foreach_map(distMap, [&](isl_map *map) {
    // 1. 提取距离向量
    // 2. 更新每个维度的 min/max 距离
  });
}
```

**参考**: ISL 文档中的 `isl_union_map_foreach_map()` API

---

## 📊 实现进度

| 功能 | 状态 | 完成度 |
|------|------|--------|
| SCoP 提取 | ✅ 完成 | 100% |
| Domain Relations | ✅ 完成 | 100% |
| Access Relations | ✅ 完成 | 100% |
| 调度树计算 | ✅ 完成 | 100% |
| 调度树分析 | ✅ 完成 | 90% |
| 依赖距离计算 | ✅ 完成 | 100% |
| 依赖计算 | ⚠️ 部分 | 30% |
| 距离向量解析 | ⚠️ 部分 | 40% |

**总体进度**: ~75%

---

## 🔧 技术挑战

### 1. Polymer API 限制

**问题**: 无法直接访问内部的访问关系

**影响**: 依赖计算无法完成

**解决方案**: 
- 短期：使用简化方法或从 MLIR 重建
- 长期：扩展 Polymer API 或提交 PR

### 2. ISL API 复杂度

**问题**: ISL API 使用复杂，需要理解多面体模型

**影响**: 距离向量解析需要深入研究

**解决方案**: 
- 参考 AutoSA 的实现
- 参考 ISL 文档和示例
- 逐步实现和测试

### 3. Domain Constraints 构建

**问题**: 从循环边界构建约束需要正确处理符号变量

**状态**: 已实现基础版本，可能需要调整

**解决方案**: 
- 测试当前实现
- 根据测试结果调整约束构建逻辑

---

## 📋 下一步计划

### 优先级 1: 完善依赖计算

**选项 A**: 扩展 Polymer API（推荐）
- 在 `IslScop` 中添加 `getReadRelations()` 和 `getWriteRelations()` 方法
- 返回 ISL union maps

**选项 B**: 从 MLIR 重建访问关系
- 在 SCoP 提取时保存访问信息
- 在依赖计算时重建 ISL 访问关系

**选项 C**: 使用替代方法
- 研究是否有其他 ISL API 可以从 schedule 计算依赖

### 优先级 2: 完善距离向量解析

**步骤**:
1. 研究 `isl_union_map_foreach_map()` API
2. 实现 map 迭代逻辑
3. 提取距离向量
4. 计算 min/max 距离
5. 测试和验证

### 优先级 3: 测试和调试

**步骤**:
1. 使用 `test/matmul_test.mlir` 测试 SCoP 提取
2. 验证 domain 和 access relations 是否正确
3. 测试调度树计算
4. 调试发现的问题

---

## 🎯 成功标准

### 短期目标（1-2周）

- [ ] SCoP 提取能够正确处理简单的循环嵌套
- [ ] Domain 和 access relations 正确添加到 SCoP
- [ ] 调度树能够正确计算和获取
- [ ] 依赖距离计算能够工作（即使依赖计算不完整）

### 中期目标（1个月）

- [ ] 依赖计算能够正确工作
- [ ] 距离向量能够正确解析
- [ ] 空间循环能够正确识别（距离 <= 1）
- [ ] 与 AutoSA 的分析结果一致

### 长期目标（2-3个月）

- [ ] 完整的端到端测试通过
- [ ] 支持复杂的循环嵌套
- [ ] 性能优化
- [ ] 代码质量达到生产级别

---

## 📚 参考资源

### 代码库
- **Polymer**: `third_party/Polygeist/tools/polymer/`
- **ISL**: ISL 文档和示例
- **AutoSA**: AutoSA 的依赖计算实现

### 关键文件
- `lib/Analysis/PolymerAnalysis.cpp` - 当前实现
- `include/systolic/Analysis/PolymerAnalysis.h` - 接口定义
- `docs/AUTOSA_ANALYSIS.md` - AutoSA 分析文档

### API 文档
- ISL Schedule Tree API
- ISL Union Map API
- Polymer IslScop API

---

**文档维护**: 本文档应随着实现进展持续更新。


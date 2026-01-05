# Space-time 扩展实施路线图

> **创建时间**: 2024-12-02  
> **目的**: 详细的实施计划和进度跟踪

---

## 总体目标

实现所有 6 种 Space-time 配置的代码生成，并优化代码质量（PIPELINE 数量和代码行数）。

---

## 阶段 1: Space-time=3 优化 ✅ **当前阶段**

### 目标
- 优化 PIPELINE 数量（当前：24，参考：24 ✅ 已一致）
- 优化代码行数（当前：平均多 66 行，约 4%）
- 分析参考代码的代码生成模式

### 工作项

#### 1.1 分析参考代码的 PIPELINE 插入位置
- [ ] 分析 `test/reference-samples/mm_st3_*.cpp` 中的 PIPELINE 位置
- [ ] 对比生成的代码和参考代码的 PIPELINE 差异
- [ ] 记录 PIPELINE 插入的规则和模式

#### 1.2 分析代码行数差异来源
- [ ] 对比生成的代码和参考代码的结构
- [ ] 识别冗余代码（注释、格式、辅助代码）
- [ ] 分析循环展开程度的差异

#### 1.3 优化代码生成
- [ ] 调整 PIPELINE 插入策略
- [ ] 优化代码格式和注释
- [ ] 减少冗余代码生成

#### 1.4 测试验证
- [ ] 运行所有 Space-time=3 的测试
- [ ] 验证 PIPELINE 数量一致
- [ ] 验证代码行数接近参考代码

### 预期结果
- PIPELINE 数量：24（与参考一致）✅
- 代码行数：减少 30-50 行（接近参考代码）
- 代码质量：结构清晰，无冗余

### 时间估算
- 分析：1-2 天
- 实现：2-3 天
- 测试：1 天
- **总计：4-6 天**

---

## 阶段 2: 实现 1D 脉动阵列（Space-time=0, 1）

### 目标
- 支持 1D output-stationary 脉动阵列
- 实现 Space-time=0（[i]）和 Space-time=1（[j]）

### 工作项

#### 2.1 修改 PE_wrapper 生成
- [ ] 根据 spacetime 参数生成不同的函数签名
- [ ] Space-time=0: B 在 PE 间传递（horizontal）
- [ ] Space-time=1: A 在 PE 间传递（vertical）

#### 2.2 修改 IO 模块生成
- [ ] 简化 IO 层级（L2, L1，无 L3）
- [ ] 根据数据流方向生成不同的 IO 模块
- [ ] 实现 Direct 数据流（A 或 B 直接发送）

#### 2.3 修改数据流生成
- [ ] 实现 Horizontal 数据流（从左到右）
- [ ] 实现 Vertical 数据流（从上到下）
- [ ] 实现边界 PE 的特殊处理

#### 2.4 测试验证
- [ ] 测试 Space-time=0 的代码生成
- [ ] 测试 Space-time=1 的代码生成
- [ ] 验证 PIPELINE 数量（目标：17）
- [ ] 验证代码行数（目标：~1082）

### 预期结果
- Space-time=0: PIPELINE=17, 代码行数~1082
- Space-time=1: PIPELINE=17, 代码行数~1082
- 功能正确性验证通过

### 时间估算
- 实现：3-4 天
- 测试：1-2 天
- **总计：4-6 天**

---

## 阶段 3: 实现 Reduction 支持（Space-time=2）

### 目标
- 支持 1D input-stationary 脉动阵列（reduction）
- 实现 MLIR 自动 Reduction 检测

### 工作项

#### 3.1 实现 Reduction 检测
- [ ] 在 `SystolicTransform` 中检测 reduction 循环
- [ ] 识别累加模式（load → add → store）
- [ ] 确定 reduction 操作符（`+`, `*`, `max`, `min`）

#### 3.2 实现 Reduction 数据流
- [ ] 生成 `fifo_C_in` 和 `fifo_C_out` 参数
- [ ] 在 PE 内累加：`local_C = fifo_C_in.read() + A * B`
- [ ] 在 PE 间传递：`fifo_C_out.write(local_C)`

#### 3.3 修改 IO 模块
- [ ] C 使用 L2_out（不是 drain）
- [ ] A 和 B 使用 broadcast 模式

#### 3.4 测试验证
- [ ] 测试 Space-time=2 的代码生成
- [ ] 验证 Reduction 自动检测
- [ ] 验证 PIPELINE 数量（目标：18）
- [ ] 验证代码行数（目标：~1105）

### 预期结果
- Space-time=2: PIPELINE=18, 代码行数~1105
- Reduction 自动检测和应用
- 功能正确性验证通过

### 时间估算
- 实现：4-5 天
- 测试：1-2 天
- **总计：5-7 天**

---

## 阶段 4: 实现 2D Reduction（Space-time=4, 5）

### 目标
- 支持 2D 脉动阵列（带 reduction）
- 实现 Space-time=4（[i,k]）和 Space-time=5（[j,k]）

### 工作项

#### 4.1 扩展 Reduction 支持
- [ ] 支持 2D 阵列的 reduction
- [ ] C 在水平方向累加（沿 k 方向）

#### 4.2 修改数据流
- [ ] Space-time=4: B 垂直传递，C 水平累加
- [ ] Space-time=5: A 垂直传递，C 水平累加

#### 4.3 修改 PE_wrapper
- [ ] 2D 索引（idx, idy）
- [ ] 根据 spacetime 生成不同的数据流参数

#### 4.4 测试验证
- [ ] 测试 Space-time=4 的代码生成
- [ ] 测试 Space-time=5 的代码生成
- [ ] 验证 PIPELINE 数量（目标：25）
- [ ] 验证代码行数（目标：~1528, ~1520）

### 预期结果
- Space-time=4: PIPELINE=25, 代码行数~1528
- Space-time=5: PIPELINE=25, 代码行数~1520
- 功能正确性验证通过

### 时间估算
- 实现：3-4 天
- 测试：1-2 天
- **总计：4-6 天**

---

## 总体时间估算

| 阶段 | 时间估算 | 状态 |
|------|---------|------|
| 阶段 1: Space-time=3 优化 | 4-6 天 | 🔄 进行中 |
| 阶段 2: 1D 脉动阵列（0, 1） | 4-6 天 | ⏳ 待开始 |
| 阶段 3: Reduction 支持（2） | 5-7 天 | ⏳ 待开始 |
| 阶段 4: 2D Reduction（4, 5） | 4-6 天 | ⏳ 待开始 |
| **总计** | **17-25 天** | |

---

## 当前进度

### ✅ 已完成
- [x] 文档分析和参考代码分析
- [x] 实施计划制定
- [x] Space-time=3 基础实现（PIPELINE 数量已一致）

### 🔄 进行中
- [ ] Space-time=3 代码行数优化
- [ ] PIPELINE 插入位置分析

### ⏳ 待开始
- [ ] 1D 脉动阵列实现
- [ ] Reduction 支持实现
- [ ] 2D Reduction 实现

---

## 关键里程碑

1. **里程碑 1**: Space-time=3 优化完成
   - 代码行数减少 30-50 行
   - 代码质量提升

2. **里程碑 2**: 1D 脉动阵列实现完成
   - Space-time=0, 1 代码生成正确
   - PIPELINE 和代码行数与参考一致

3. **里程碑 3**: Reduction 支持实现完成
   - Space-time=2 代码生成正确
   - MLIR 自动检测 reduction

4. **里程碑 4**: 2D Reduction 实现完成
   - Space-time=4, 5 代码生成正确
   - 所有 6 种 spacetime 支持完成

---

## 风险与应对

### 风险 1: PIPELINE 数量优化困难
- **风险**: 无法准确匹配参考代码的 PIPELINE 位置
- **应对**: 详细分析参考代码，建立 PIPELINE 插入规则表

### 风险 2: 代码行数差异较大
- **风险**: 代码生成策略导致行数差异无法缩小
- **应对**: 分析差异来源，优化代码生成逻辑

### 风险 3: Reduction 检测不准确
- **风险**: MLIR 自动检测 reduction 失败
- **应对**: 实现多种检测策略，提供手动指定选项

### 风险 4: 1D 和 2D 阵列实现差异
- **风险**: 1D 和 2D 阵列的代码生成逻辑差异较大
- **应对**: 抽象公共逻辑，分别实现特殊部分

---

## 相关文档

- **实施计划**: `docs/SPACETIME_IMPLEMENTATION_PLAN.md`
- **实现分析**: `docs/SPACETIME_IMPLEMENTATION_ANALYSIS.md`
- **测试结果**: `docs/SPACETIME_TEST_ANALYSIS.md`
- **AutoSA 文档**: https://autosa.readthedocs.io/en/latest/examples/mm.html

---

## 更新日志

- **2024-12-02**: 创建实施路线图，开始阶段 1（Space-time=3 优化）


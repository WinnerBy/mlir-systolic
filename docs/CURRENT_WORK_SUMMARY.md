# 当前工作进展总结

> **最后更新**: 2024-12  
> **状态**: 继续完善核心功能实现

---

## 本次更新内容

### ✅ 1. 配置信息传递机制

**问题**：`SystolicDataflowGeneration` pass 无法获取 `SystolicTransform` pass 的配置信息（array_part, latency, PE 数组大小等）。

**解决方案**：
- 在 `SystolicTransform` pass 中，将配置信息存储为函数属性：
  - `systolic.array_part` - 数组分块因子
  - `systolic.latency` - 延迟隐藏因子
  - `systolic.pe_array_size` - PE 数组大小
  - `systolic.space_time_mode` - 时空映射模式
- 在 `SystolicDataflowGeneration` pass 中，从函数属性读取配置信息
- 如果属性不存在，回退到从循环结构推断（向后兼容）

**修改文件**：
- `lib/Transforms/SystolicTransform.cpp` - 添加配置信息存储
- `lib/Transforms/SystolicDataflowGeneration.cpp` - 添加配置信息读取

### ✅ 2. 改进 IO 层级分析

**问题**：之前的 IO 层级分析过于简单，所有数组都标记为 L2。

**解决方案**：
- 基于循环嵌套深度进行更智能的分析：
  - **L3**：访问发生在最外层循环（深度 ≥ 4）- 全局内存接口
  - **L2**：访问发生在中间层循环（深度 2-3）- 双缓冲层
  - **L1**：访问发生在内层循环（深度 < 2）- PE 接口
- 根据层级自动设置是否需要双缓冲：
  - L2 通常需要双缓冲（ping-pong）
  - L1 和 L3 通常不需要双缓冲

**修改文件**：
- `lib/Transforms/SystolicDataflowGeneration.cpp` - 改进 `analyzeArrayReferences` 函数

---

## 当前实现状态

### ✅ 已完成

1. **构建系统重构**
   - 统一构建方式（Polygeist + Polymer）
   - 简化的构建脚本
   - 修复链接问题

2. **Polymer 集成**
   - 强制使用 Polymer（无启发式回退）
   - 自动运行 `ExtractScopStmt` pass
   - SCoP 提取和依赖分析

3. **SystolicTransform Pass**
   - 依赖距离分析（使用 Polymer/ISL）
   - 空间循环选择
   - 循环置换和多级分块
   - **配置信息存储**（新增）

4. **SystolicDataflowGeneration Pass（部分完成）**
   - 数组引用分组（IO/PE/Drain）
   - **改进的 IO 层级分析**（新增）
   - **配置信息读取**（新增）
   - SystolicDataflow Dialect 操作创建（框架）

### 🟡 进行中

1. **SystolicDataflowGeneration Pass**
   - ✅ 配置信息传递
   - ✅ IO 层级分析改进
   - ⚠️ 循环体迁移（待实现）
   - ⚠️ 双缓冲逻辑生成（待实现）

### ⚠️ 待实现

1. **循环体迁移**
   - 将 Affine 循环体内容迁移到 SystolicDataflow Dialect 模块中
   - 处理 IO 模块、PE 数组、Drain 模块的内容生成

2. **双缓冲逻辑生成**
   - 在 IO 模块中生成实际的 ping-pong 双缓冲代码
   - 实现 inter-transfer 和 intra-transfer 逻辑

3. **SystolicDataflowToHLS Pass**
   - 将 SystolicDataflow Dialect 降级到 HLS Dialect
   - 大部分实现标记为 TODO

4. **EmitHLSCpp Translation**
   - 从 HLS Dialect 生成 HLS C++ 代码
   - Pragma 插入

---

## 下一步工作计划

### 优先级 1：完善 SystolicDataflowGeneration

1. **实现循环体迁移**
   - 将 Affine 循环体内容迁移到 PE 数组模块
   - 生成 IO 模块的数据传输逻辑
   - 生成 Drain 模块的输出逻辑

2. **实现双缓冲逻辑**
   - 在 L2 IO 模块中生成 `DoubleBufferOp`
   - 实现 ping-pong 切换逻辑
   - 处理 inter-transfer 和 intra-transfer 区域

### 优先级 2：完善代码生成

1. **SystolicDataflowToHLS Pass**
   - 实现 Dialect 降级逻辑
   - 处理双缓冲转换

2. **EmitHLSCpp Translation**
   - 实现 HLS C++ 代码生成
   - 插入 HLS pragma

### 优先级 3：测试和验证

1. **端到端测试**
   - MatMul 测试用例
   - 验证生成的 HLS C++ 代码

2. **与 AutoSA 对比**
   - 对比生成的代码质量
   - 性能分析

---

## 技术细节

### 配置信息传递机制

```cpp
// SystolicTransform.cpp - 存储配置
func->setAttr("systolic.array_part", builder.getI64ArrayAttr(options.arrayPart));
func->setAttr("systolic.latency", builder.getI64ArrayAttr(options.latency));
func->setAttr("systolic.pe_array_size", builder.getI64ArrayAttr(peArraySize));

// SystolicDataflowGeneration.cpp - 读取配置
if (auto peArrayAttr = func->getAttrOfType<ArrayAttr>("systolic.pe_array_size")) {
  // 读取 PE 数组大小
}
```

### IO 层级分析启发式

```cpp
// 基于循环嵌套深度
if (minDepth >= 4) {
  group.ioLevel = 3;  // L3: 全局内存接口
} else if (minDepth >= 2) {
  group.ioLevel = 2;  // L2: 双缓冲层
  group.needsDoubleBuffer = true;
} else {
  group.ioLevel = 1;  // L1: PE 接口
}
```

---

## 相关文件

- `lib/Transforms/SystolicTransform.cpp` - 空间时间变换 Pass
- `lib/Transforms/SystolicDataflowGeneration.cpp` - 数据流生成 Pass
- `lib/Transforms/SystolicDataflowToHLS.cpp` - Dialect 降级 Pass
- `include/systolic/Dialect/SystolicDataflow/` - SystolicDataflow Dialect 定义

---

## 参考文档

- [项目进展总结](PROJECT_STATUS.md) - 项目整体状态
- [构建指南](BUILD_STEPS.md) - 构建说明
- [AutoSA 分析](AUTOSA_ANALYSIS.md) - AutoSA 架构参考


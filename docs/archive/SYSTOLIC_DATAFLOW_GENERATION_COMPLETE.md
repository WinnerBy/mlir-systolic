# SystolicDataflowGeneration Pass 完善总结

> **最后更新**: 2024-12  
> **状态**: 核心功能已实现

---

## 本次更新内容

### ✅ 1. 循环体迁移实现

**功能**：将 Affine 循环体内容实际迁移到 SystolicDataflow Dialect 模块中。

**实现细节**：
- **PE 数组模块**：
  - 找到最内层计算循环（通常是 MatMul 的 k 循环）
  - 从最内层循环向上构建循环嵌套
  - 克隆最内层 1-2 个计算循环到 PE 数组模块中
  - 跳过外层分块循环（由 PE 数组结构处理）
  - 使用 `IRMapping` 正确映射循环归纳变量和操作

- **IO 模块**：
  - L1/L3 模块：生成简单的加载/存储操作（框架已实现，具体逻辑待完善）
  - L2 模块：生成双缓冲逻辑（见下文）

- **Drain 模块**：
  - 生成输出逻辑框架
  - 克隆存储操作到 Drain 模块（框架已实现，具体逻辑待完善）

**关键代码**：
```cpp
// 找到最内层循环并构建循环嵌套
SmallVector<AffineForOp, 4> loopNest;
AffineForOp current = innermostLoop;
while (current && loopNest.size() < 4) {
  loopNest.push_back(current);
  // 向上查找父循环
  ...
}

// 克隆最内层计算循环
IRMapping mapping;
int startIdx = std::max(0, (int)loopNest.size() - 2);  // 克隆最后 2 个循环
for (int i = startIdx; i < (int)loopNest.size(); ++i) {
  // 克隆循环和循环体操作
  ...
}
```

### ✅ 2. 双缓冲逻辑生成

**功能**：在 L2 IO 模块中生成实际的 ping-pong 双缓冲代码。

**实现细节**：
- **缓冲区分配**：
  - 根据 tile size 计算缓冲区形状
  - 分配 ping 和 pong 缓冲区
  - 分配仲裁器（arbiter）和 intra-enable 标志

- **DoubleBufferOp 创建**：
  - 创建 `dataflow::DoubleBufferOp` 操作
  - 设置 inter-transfer 区域（数据加载）
  - 设置 intra-transfer 区域（数据发送到 PE）

- **初始化**：
  - 初始化仲裁器为 false（ping 缓冲区激活）
  - 初始化 intra-enable 为 false

**关键代码**：
```cpp
// 分配缓冲区
auto pingBuffer = builder.create<memref::AllocOp>(loc, pingType);
auto pongBuffer = builder.create<memref::AllocOp>(loc, pongType);
auto arbiter = builder.create<memref::AllocOp>(loc, arbiterType);
auto intraEnable = builder.create<memref::AllocOp>(loc, intraEnableType);

// 创建 DoubleBufferOp
auto doubleBuffer = builder.create<dataflow::DoubleBufferOp>(
    loc, pingBuffer, pongBuffer, arbiter, intraEnable);

// 创建 inter-transfer 和 intra-transfer 区域
Block *interBlock = builder.createBlock(&doubleBuffer.getInterTransfer());
Block *intraBlock = builder.createBlock(&doubleBuffer.getIntraTransfer());
```

---

## 当前实现状态

### ✅ 已完成

1. **配置信息传递**
   - 从 `SystolicTransform` 读取配置信息
   - 支持 PE 数组大小、分块因子、延迟因子等

2. **IO 层级分析**
   - 基于循环嵌套深度的智能分析
   - 自动判断 L1/L2/L3 层级
   - 自动设置双缓冲需求

3. **循环体迁移**
   - PE 数组模块：克隆计算循环体 ✅
   - IO 模块：框架已实现 ⚠️
   - Drain 模块：框架已实现 ⚠️

4. **双缓冲逻辑生成**
   - L2 IO 模块：生成 DoubleBufferOp ✅
   - 缓冲区分配和初始化 ✅
   - inter-transfer 和 intra-transfer 区域创建 ✅

### ⚠️ 待完善

1. **IO 模块内容生成**
   - L1/L3 模块：需要生成具体的加载/存储逻辑
   - L2 模块：需要完善 inter-transfer 和 intra-transfer 的具体逻辑

2. **Drain 模块内容生成**
   - 需要生成具体的输出逻辑
   - 需要正确映射存储操作

3. **值映射和依赖处理**
   - 在克隆操作时，需要正确处理跨模块的值依赖
   - 需要处理 IO 模块和 PE 数组之间的数据流

---

## 技术细节

### 循环体迁移策略

1. **找到最内层循环**：
   - 遍历所有 `AffineForOp`，找到嵌套深度最大的循环

2. **构建循环嵌套**：
   - 从最内层循环向上查找父循环
   - 构建完整的循环嵌套结构

3. **选择克隆范围**：
   - 克隆最内层 1-2 个计算循环
   - 跳过外层分块循环（由 PE 数组结构处理）

4. **克隆操作**：
   - 使用 `IRMapping` 映射循环归纳变量
   - 克隆循环体中的所有操作（排除 yield 和嵌套循环）

### 双缓冲实现策略

1. **缓冲区大小计算**：
   - 根据原始 memref 类型和 tile size 计算缓冲区形状
   - 确保缓冲区大小匹配 tile 大小

2. **仲裁器逻辑**：
   - 使用布尔值标志表示当前激活的缓冲区
   - false = ping 激活，true = pong 激活

3. **区域分离**：
   - inter-transfer：从全局内存加载数据到缓冲区
   - intra-transfer：从缓冲区发送数据到 PE 数组

---

## 下一步工作

### 优先级 1：完善 IO 和 Drain 模块内容

1. **IO 模块内容生成**：
   - 根据数组访问模式生成加载逻辑
   - 实现 inter-transfer 和 intra-transfer 的具体操作

2. **Drain 模块内容生成**：
   - 从 PE 数组收集结果
   - 生成存储操作

### 优先级 2：值映射和依赖处理

1. **跨模块值映射**：
   - 处理 IO 模块和 PE 数组之间的数据流
   - 正确映射 memref 和值

2. **依赖分析**：
   - 确保数据流正确性
   - 处理循环携带依赖

### 优先级 3：测试和验证

1. **端到端测试**：
   - MatMul 测试用例
   - 验证生成的 SystolicDataflow Dialect IR

2. **与 AutoSA 对比**：
   - 对比生成的硬件结构
   - 验证双缓冲逻辑正确性

---

## 相关文件

- `lib/Transforms/SystolicDataflowGeneration.cpp` - 主要实现文件
- `include/systolic/Dialect/SystolicDataflow/SystolicDataflowOps.td` - Dialect 定义

---

## 参考文档

- [当前工作进展总结](CURRENT_WORK_SUMMARY.md)
- [项目进展总结](PROJECT_STATUS.md)
- [AutoSA 分析](AUTOSA_ANALYSIS.md)


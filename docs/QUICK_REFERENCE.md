# MLIR-Systolic 技术方案快速参考

## 核心问题

1. **当前问题**：一对一翻译 AutoSA，无法生成多层双缓冲 IO，难以利用 MLIR 特性
2. **解决方案**：引入 `SystolicDataflow` Dialect 作为中间抽象层

## 关键改进

### 1. 引入 SystolicDataflow Dialect

**为什么**：用 MLIR 的方式表达硬件抽象，而不是直接生成代码

**包含的操作**：
- `systolic.io.module<L3>` - Global Memory 接口
- `systolic.io.module<L2>` - 双缓冲中间层
- `systolic.io.module<L1>` - PE 接口
- `systolic.pe.array` - PE 阵列
- `systolic.drain.module` - 输出模块
- `systolic.double_buffer` - 双缓冲逻辑

### 2. 新的 Pass 流程

```
Affine IR
  ↓ SystolicTransform (分块)
分块后的 Affine IR
  ↓ SystolicDataflowGeneration (生成 Dialect)
SystolicDataflow Dialect ⭐ NEW
  ↓ SystolicDataflowToHLS (降级)
HLS Dialect
  ↓ EmitHLSCpp
HLS C++
```

### 3. 多层双缓冲 IO 实现

**AutoSA 结构**：
```
Global Memory
  ↓ L3 IO (inter-transfer)
L3 Buffer
  ↓ L2 IO (inter-transfer, ping-pong)
L2 Buffer (ping-pong)
  ↓ L1 IO (intra-transfer)
PE Array
```

**MLIR-Systolic 实现**：
- `SystolicDataflowGeneration` Pass 分析数组引用，确定 IO 层级
- 为每个层级生成 `systolic.io.module` 操作
- L2 层级使用 `systolic.double_buffer` 表达 ping-pong 逻辑
- `SystolicDataflowToHLS` Pass 将 Dialect 降级到 HLS Dialect

## 实现步骤

### Phase 1: Dialect 定义
```bash
# 创建 Dialect 定义文件
include/systolic/Dialect/SystolicDataflow/SystolicDataflow.td
```

### Phase 2: 数组引用分析
```cpp
// 在 SystolicDataflowGeneration.cpp 中
SmallVector<ArrayRefGroup> analyzeArrayReferences(FuncOp func);
```

### Phase 3: 数据流生成
```cpp
// 生成多层 IO 模块
void generateMultiLevelIO(OpBuilder &builder, ArrayRefGroup &group);
```

### Phase 4: Dialect 降级
```cpp
// 将 SystolicDataflow 降级到 HLS
class SystolicDataflowToHLSLowering : public ConversionPattern;
```

## 与 AutoSA 的对应

| AutoSA | MLIR-Systolic |
|--------|--------------|
| `autosa_array_ref_group` | `ArrayRefGroup` (分析) |
| `autosa_io_buffer` | `systolic.io.module` |
| `autosa_hw_module` | `systolic.io.module` / `pe.array` |
| `io_level` (1-3) | `IOModuleOp.level` 属性 |
| ping-pong buffer | `systolic.double_buffer` |

## 优势

1. **清晰的抽象层级**：每一层职责明确
2. **易于扩展**：可以添加新的 Dialect 操作或优化 Pass
3. **充分利用 MLIR**：dialect 系统、pass 组合、类型系统
4. **支持多层 IO**：通过 Dialect 清晰表达 L1/L2/L3 结构

## 详细文档

完整的技术方案请参考：[TECHNICAL_REDESIGN.md](TECHNICAL_REDESIGN.md)


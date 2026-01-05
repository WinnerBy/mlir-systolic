# Allo HLS C 代码生成规则

> **最后更新**: 2025-01-XX  
> **目的**: 详细分析 Allo 如何将 MLIR（特别是 Affine Dialect）转换为 HLS C++ 代码

---

## 目录

1. [概述](#1-概述)
2. [代码生成流程](#2-代码生成流程)
3. [Affine 操作转换规则](#3-affine-操作转换规则)
4. [Pragma 插入规则](#4-pragma-插入规则)
5. [数组处理规则](#5-数组处理规则)
6. [Stream 处理规则](#6-stream-处理规则)
7. [与 mlir-systolic 的对比](#7-与-mlir-systolic-的对比)

---

## 1. 概述

Allo 使用 **直接代码生成** 的方式，从 MLIR（包含 Affine Dialect）直接生成 HLS C++ 代码，**不经过中间的 HLS Dialect**。

**关键文件**：
- `allo/mlir/lib/Translation/EmitVivadoHLS.cpp` - Vivado HLS 代码生成器（~2700 行）
- `allo/mlir/lib/Translation/EmitTapaHLS.cpp` - TAPA 代码生成器
- `allo/mlir/lib/Translation/EmitIntelHLS.cpp` - Intel HLS 代码生成器

**核心机制**：
- 使用 Visitor 模式遍历 MLIR 操作
- 对每种操作类型有对应的 `emit` 方法
- 根据操作属性插入 HLS pragma

---

## 2. 代码生成流程

```
MLIR Module (包含 Affine Dialect)
  ↓
ModuleEmitter::emitModule()
  ↓
遍历所有函数
  ↓
ModuleEmitter::emitFunction()
  - 生成函数签名
  - 调用 emitFunctionDirectives() 插入 pragma
  - 调用 emitBlock() 生成函数体
  ↓
StmtVisitor / ExprVisitor
  - 根据操作类型调用对应的 emit 方法
  ↓
HLS C++ 代码
```

**关键代码**（`EmitVivadoHLS.cpp:2237-2248`）：
```cpp
void ModuleEmitter::emitBlock(Block &block) {
  for (auto &op : block) {
    if (ExprVisitor(*this).dispatchVisitor(&op))
      continue;
    if (StmtVisitor(*this).dispatchVisitor(&op))
      continue;
    emitError(&op, "can't be correctly emitted.");
  }
}
```

---

## 3. Affine 操作转换规则

### 3.1 AffineForOp → for 循环

**代码位置**：`EmitVivadoHLS.cpp:872-951`

**转换规则**：
```cpp
affine.for %i = %lb to %ub step %step {
  // body
}
```

转换为：
```cpp
for (int i = lb; i < ub; i += step) {
  // body
}
```

**关键特性**：
1. **循环标签**：如果操作有 `loop_name` 属性，生成带标签的循环
   ```cpp
   l_op_name_loop_name: for (int i = ...) { ... }
   ```

2. **边界表达式**：使用 `AffineExprEmitter` 将 Affine 表达式转换为 C++ 表达式
   - 支持 `max()` 和 `min()` 表达式
   - 支持常量、维度、符号表达式

3. **Pragma 插入**：在循环前调用 `emitLoopDirectives()` 插入 pragma

### 3.2 AffineLoadOp → 数组读取

**代码位置**：`EmitVivadoHLS.cpp:1091-1148`

**转换规则**：
```cpp
%val = affine.load %memref[%i, %j] : memref<MxNxf32>
```

转换为：
```cpp
float val = memref[i][j];
```

**关键特性**：
1. **函数参数线性化**：如果 `memref` 是函数参数且 `linearize_pointers=true`，使用线性化索引
   ```cpp
   // 对于 shape [M, N] 的数组，访问 [i, j] 转换为：
   memref[i * N + j]
   ```

2. **Stream 处理**：如果 `memref` 的 `memory_space` 属性以 `"stream"` 开头，生成 stream 读取
   ```cpp
   stream.read(); // 对于 stream 类型
   ```

3. **属性处理**：
   - `from` 属性：用于注释，标识数据来源
   - `unsigned` 属性：处理无符号类型

### 3.3 AffineStoreOp → 数组写入

**代码位置**：`EmitVivadoHLS.cpp:1150-1207`

**转换规则**：
```cpp
affine.store %val, %memref[%i, %j] : memref<MxNxf32>
```

转换为：
```cpp
memref[i][j] = val;
```

**关键特性**：
1. **函数参数线性化**：与 `AffineLoadOp` 相同
2. **Stream 处理**：生成 `stream.write(val)`
3. **属性处理**：
   - `to` 属性：用于注释，标识数据目标

### 3.4 AffineIfOp → if 语句

**代码位置**：`EmitVivadoHLS.cpp:953-1002`

**转换规则**：
```cpp
affine.if %set(%i, %j) {
  // then block
} else {
  // else block
}
```

转换为：
```cpp
if (constraint1 >= 0 && constraint2 == 0) {
  // then block
} else {
  // else block
}
```

**关键特性**：
- 将 `IntegerSet` 约束转换为 C++ 条件表达式
- 等式约束转换为 `== 0`，不等式约束转换为 `>= 0`

---

## 4. Pragma 插入规则

### 4.1 循环 Pragma

**代码位置**：`EmitVivadoHLS.cpp:2250-2281`

#### Pipeline Pragma

**触发条件**：操作有 `pipeline_ii` 属性

**生成代码**：
```cpp
#pragma HLS pipeline II=<value>
```

**特殊处理**：
- 如果有 `rewind` 属性，添加 `rewind` 关键字
  ```cpp
  #pragma HLS pipeline II=1 rewind
  ```

#### Unroll Pragma

**触发条件**：操作有 `unroll` 属性

**生成代码**：
```cpp
// factor = 0: 完全展开
#pragma HLS unroll

// factor > 0: 部分展开
#pragma HLS unroll factor=<value>
```

#### Dataflow Pragma

**触发条件**：操作有 `dataflow` 属性

**生成代码**：
```cpp
#pragma HLS dataflow
```

### 4.2 数组 Pragma

**代码位置**：`EmitVivadoHLS.cpp:2283-2372`

#### Array Partition Pragma

**触发条件**：`MemRefType` 有 `layout_map` 属性

**生成规则**：

1. **Block Partition**：
   ```cpp
   #pragma HLS array_partition variable=<name> block dim=<dim> factor=<factor>
   ```
   - 当 `layout_map` 的对应维度是 `FloorDiv` 表达式时使用

2. **Cyclic Partition**：
   ```cpp
   #pragma HLS array_partition variable=<name> cyclic dim=<dim> factor=<factor>
   ```
   - 其他情况使用

3. **Complete Partition**：
   ```cpp
   #pragma HLS array_partition variable=<name> complete dim=<dim>
   ```
   - 当数组在该维度完全分区时使用

#### Stream Pragma

**触发条件**：`MemRefType` 的 `memory_space` 属性以 `"stream"` 开头

**生成代码**：
```cpp
#pragma HLS stream variable=<name> depth=<depth>
```

**注意**：如果数组是 Stream 类型，**不会**生成 `array_partition` pragma

### 4.3 函数 Pragma

**代码位置**：`EmitVivadoHLS.cpp:2374-2450`

#### Dataflow Pragma

**触发条件**：函数有 `dataflow` 属性

**生成代码**：
```cpp
#pragma HLS dataflow
```

#### Inline Pragma

**触发条件**：函数有 `inline` 属性

**生成代码**：
```cpp
#pragma HLS inline
```

---

## 5. 数组处理规则

### 5.1 数组声明

**规则**：
- **函数参数**：声明为指针类型 `T*` 或 `T[N]`
- **局部数组**：声明为多维数组 `T[M][N]`

**代码位置**：`EmitVivadoHLS.cpp:emitArrayDecl()`

### 5.2 数组访问

**规则**：
1. **函数参数 + linearize_pointers=true**：
   - 使用线性化索引：`array[i * N + j]`

2. **局部数组**：
   - 使用多维索引：`array[i][j]`

3. **Stream 数组**：
   - 根据 `memory_space` 属性中的 `S`（空间）和 `T`（时间）标记
   - 只对空间维度生成索引：`stream[i].read()`

### 5.3 数组分区

**规则**：
- 从 `layout_map` 属性提取分区信息
- 根据 `AffineExpr` 的类型（`FloorDiv` vs 其他）决定 `block` 或 `cyclic`
- 从 `AffineExpr` 中提取 `factor`

---

## 6. Stream 处理规则

### 6.1 Stream 类型识别

**规则**：`MemRefType` 的 `memory_space` 属性以 `"stream"` 开头

**格式**：`"stream;depth=<depth>;S=<spatial_dims>;T=<temporal_dims>"`

### 6.2 Stream 读取

**代码位置**：`EmitVivadoHLS.cpp:1108-1129`

**转换规则**：
```cpp
%val = affine.load %stream[%i] : memref<Mxstream<f32>>
```

转换为：
```cpp
float val = stream[i].read(); // 只对空间维度生成索引
```

### 6.3 Stream 写入

**代码位置**：`EmitVivadoHLS.cpp:1163-1186`

**转换规则**：
```cpp
affine.store %val, %stream[%i] : memref<Mxstream<f32>>
```

转换为：
```cpp
stream[i].write(val); // 只对空间维度生成索引
```

### 6.4 Stream 声明

**代码位置**：`EmitVivadoHLS.cpp:1669-1694`

**生成代码**：
```cpp
hls::stream<float> stream;
#pragma HLS stream variable=stream depth=<depth>
```

---

## 7. 与 mlir-systolic 的对比

### 7.1 代码生成方式

| 方面 | Allo | mlir-systolic |
|------|------|---------------|
| **抽象层级** | 低（直接 Affine） | 高（HLS Dialect） |
| **代码生成** | 逐句翻译 | 结构化生成 |
| **Pragma 插入** | 根据操作属性 | 根据 Dialect 属性 |
| **可维护性** | 中等（大文件） | 高（分层清晰） |

### 7.2 Pragma 生成对比

**Allo**：
- 直接从操作属性读取（`pipeline_ii`, `unroll`, `dataflow` 等）
- 在代码生成时插入 pragma

**mlir-systolic**：
- 通过 HLS Dialect 表达 pragma 信息
- 在 Dialect 降级时转换为 pragma
- 更结构化和可扩展

### 7.3 数组处理对比

**Allo**：
- 通过 `layout_map` 属性表达分区信息
- 在代码生成时解析并生成 pragma

**mlir-systolic**：
- 通过 HLS Dialect 的 `array_partition` 操作表达
- 在代码生成时直接转换为 pragma
- 更明确的语义

---

## 8. 关键代码片段

### 8.1 AffineForOp 生成

```cpp
void ModuleEmitter::emitAffineFor(AffineForOp op) {
  // 生成循环标签（如果有）
  if (op->hasAttr("loop_name")) {
    os << "l_" << op_name << "_" << loop_name << ": ";
  }
  
  // 生成 for 循环头
  os << "for (";
  emitValue(iterVar);
  os << " = ";
  // 生成下界表达式
  emitAffineExpr(lowerMap.getResult(0));
  os << "; ";
  emitValue(iterVar);
  os << " < ";
  // 生成上界表达式
  emitAffineExpr(upperMap.getResult(0));
  os << "; ";
  emitValue(iterVar);
  os << " += " << step << ") {\n";
  
  // 插入循环 pragma
  emitLoopDirectives(op);
  
  // 生成循环体
  emitBlock(*op.getBody());
  
  os << "}\n";
}
```

### 8.2 Pragma 插入

```cpp
void ModuleEmitter::emitLoopDirectives(Operation *op) {
  // Pipeline pragma
  if (auto ii = getLoopDirective(op, "pipeline_ii")) {
    os << "#pragma HLS pipeline II=" << ii.getValue();
    if (op->hasAttr("rewind"))
      os << " rewind";
    os << "\n";
  }
  
  // Unroll pragma
  if (auto factor = getLoopDirective(op, "unroll")) {
    if (factor.getValue() == 0)
      os << "#pragma HLS unroll\n";
    else
      os << "#pragma HLS unroll factor=" << factor.getValue() << "\n";
  }
  
  // Dataflow pragma
  if (auto dataflow = getLoopDirective(op, "dataflow")) {
    os << "#pragma HLS dataflow\n";
  }
}
```

### 8.3 数组分区 Pragma

```cpp
void ModuleEmitter::emitArrayDirectives(Value memref) {
  auto type = llvm::dyn_cast<MemRefType>(memref.getType());
  auto layoutMap = getLayoutMap(type);
  
  // 获取分区因子
  SmallVector<int64_t, 8> factors;
  getPartitionFactors(type, &factors);
  
  for (int64_t dim = 0; dim < type.getRank(); ++dim) {
    if (factors[dim] != 1) {
      os << "#pragma HLS array_partition";
      os << " variable=" << getName(memref);
      
      // 根据 layout_map 决定 block 或 cyclic
      if (layoutMap.getResult(dim).getKind() == AffineExprKind::FloorDiv)
        os << " block";
      else
        os << " cyclic";
      
      os << " dim=" << dim + 1;
      os << " factor=" << factors[dim] << "\n";
    }
  }
}
```

---

## 9. 总结

### 9.1 Allo 的代码生成特点

1. **直接翻译**：从 MLIR 操作直接生成 C++ 代码，没有中间抽象
2. **属性驱动**：通过操作属性控制 pragma 插入
3. **逐句生成**：每个 MLIR 操作对应一段 C++ 代码
4. **灵活但复杂**：代码生成器约 2700 行，处理各种边界情况

### 9.2 对 mlir-systolic 的启示

1. **保持 HLS Dialect**：使用 Dialect 表达硬件结构，比属性更清晰
2. **结构化生成**：通过 Dialect 降级，代码生成更简单
3. **可扩展性**：Dialect 系统便于添加新的硬件结构

### 9.3 集成建议

- **阶段 1**：完成 mlir-systolic 的核心功能，使用 HLS Dialect 和结构化生成
- **阶段 2**：如果需要，可以降级回 Affine 并使用 Allo 的代码生成器（作为备选方案）

---

## 10. 参考资料

- `allo/mlir/lib/Translation/EmitVivadoHLS.cpp` - 主要代码生成器
- `allo/allo/backend/hls.py` - HLS 后端接口
- [Allo 集成分析](./ALLO_INTEGRATION_ANALYSIS.md)


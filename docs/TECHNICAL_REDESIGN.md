# MLIR-Systolic 技术方案重新设计

## 问题分析

### 当前问题

1. **一对一翻译问题**：当前实现更像 AutoSA 的直接翻译，没有充分利用 MLIR 的优势
2. **缺少多层双缓冲 IO**：无法生成 AutoSA 的多层（L1/L2/L3）双缓冲 IO 模块
3. **难以扩展**：当前设计难以利用 MLIR 的 dialect 系统、pass 组合等特性进行后续改进

### AutoSA 核心架构分析

AutoSA 的核心在于**分层的数据流管理**：

```
Global Memory
    ↓ (L3 IO Module - Inter-transfer)
L3 Buffer (ping-pong)
    ↓ (L2 IO Module - Inter-transfer)
L2 Buffer (ping-pong)  
    ↓ (L1 IO Module - Intra-transfer)
PE Array (Systolic Flow)
    ↑ (Drain Modules - reverse flow)
Output Memory
```

关键数据结构：
- `autosa_array_ref_group`: 数组引用分组（IO/PE/Drain）
- `autosa_io_buffer`: 每个 IO 层级的缓冲区（L1/L2/L3）
- `autosa_hw_module`: 硬件模块（PE/IO/Drain）
- `io_level`: IO 层级（1-3）
- `inter_transfer` / `intra_transfer`: 数据传输类型

## 改进方案：基于 MLIR Dialect 的分层设计

### 核心思想

**不再直接翻译 AutoSA，而是用 MLIR 的方式重新表达 AutoSA 的语义**，利用 MLIR 的 dialect 系统、pass 组合和变换能力。

### 架构设计

```
┌─────────────────────────────────────────────────────────────┐
│ Phase 1: 多面体分析 (Polymer/ISL)                            │
│ - 依赖距离分析                                               │
│ - 空间循环选择                                               │
│ - 调度树构建                                                 │
└─────────────────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────────────────┐
│ Phase 2: 空间-时间变换 (SystolicTransform Pass)              │
│ - 循环置换（空间循环外移）                                    │
│ - 多级分块（array_part + latency）                           │
│ - 输出: 分块后的 Affine IR                                   │
└─────────────────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────────────────┐
│ Phase 3: 数据流抽象 (SystolicDataflow Dialect) ⭐ NEW        │
│                                                              │
│ 定义新的 Dialect 来表达脉动阵列的硬件抽象：                   │
│                                                              │
│ - systolic.io.module<L3> { ... }  // L3 IO 模块             │
│ - systolic.io.module<L2> { ... }  // L2 IO 模块（双缓冲）   │
│ - systolic.io.module<L1> { ... }  // L1 IO 模块             │
│ - systolic.pe.array { ... }      // PE 阵列                 │
│ - systolic.drain.module { ... }   // Drain 模块             │
│                                                              │
│ 每个模块包含：                                                │
│ - 缓冲区声明（ping-pong）                                     │
│ - 数据传输操作（inter/intra）                                 │
│ - 调度信息（schedule）                                        │
└─────────────────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────────────────┐
│ Phase 4: 数据流生成 (SystolicDataflowGeneration Pass)        │
│                                                              │
│ 将 Affine IR 转换为 SystolicDataflow Dialect：              │
│                                                              │
│ 1. 数组引用分组（类似 AutoSA 的 autosa_array_ref_group）     │
│ 2. IO 层级分析（确定 L1/L2/L3）                              │
│ 3. 双缓冲生成（ping-pong buffer）                             │
│ 4. 模块实例化（IO/PE/Drain）                                  │
└─────────────────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────────────────┐
│ Phase 5: 数据流优化 (可选 Pass)                              │
│ - 缓冲区合并优化                                              │
│ - 流通道优化                                                  │
│ - 调度优化                                                    │
└─────────────────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────────────────┐
│ Phase 6: 降级到 HLS Dialect                                  │
│                                                              │
│ 将 SystolicDataflow Dialect 降级到现有的 HLS Dialect：      │
│                                                              │
│ - systolic.io.module → hls.dataflow.task                     │
│ - systolic.pe.array → hls.dataflow.task                     │
│ - 双缓冲逻辑 → Affine loops + SCF if                         │
│ - Stream 通道 → hls.dataflow.stream                          │
└─────────────────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────────────────┐
│ Phase 7: HLS C++ 生成 (EmitHLSCpp)                          │
│ - 生成 HLS C++ 代码                                          │
│ - 插入 Pragma                                                │
└─────────────────────────────────────────────────────────────┘
```

## 详细设计

### 1. SystolicDataflow Dialect 定义

创建新的 Dialect 来抽象脉动阵列的硬件结构：

```tablegen
// include/systolic/Dialect/SystolicDataflow/SystolicDataflow.td

def SystolicDataflow_Dialect : Dialect {
  let name = "systolic.dataflow";
  let cppNamespace = "::mlir::systolic::dataflow";
  let summary = "Systolic array dataflow abstraction";
}

// IO 模块操作
def SystolicIOModule : SystolicDataflow_Op<"io.module"> {
  let summary = "IO module at a specific level";
  
  let arguments = (ins
    I32Attr:$level,        // 1, 2, or 3
    StrAttr:$direction,    // "in" or "out"
    StrAttr:$arrayName,    // Array name (e.g., "A", "B", "C")
    OptionalAttr<I64ArrayAttr>:$bufferShape,  // Buffer dimensions
    OptionalAttr<BoolAttr>:$doubleBuffer,     // Enable ping-pong
    Region:$body
  );
  
  let results = (outs);
  
  let description = [{
    Represents an IO module at level L1, L2, or L3.
    Contains buffer allocation and data transfer logic.
  }];
}

// PE 阵列操作
def SystolicPEArray : SystolicDataflow_Op<"pe.array"> {
  let summary = "Processing Element array";
  
  let arguments = (ins
    I64ArrayAttr:$arraySize,      // [rows, cols]
    I64ArrayAttr:$tileSize,        // Tile dimensions
    Region:$body
  );
  
  let results = (outs);
  
  let description = [{
    Represents a 2D PE array with systolic data flow.
  }];
}

// 双缓冲操作
def SystolicDoubleBuffer : SystolicDataflow_Op<"double_buffer"> {
  let summary = "Double buffer (ping-pong)";
  
  let arguments = (ins
    MemRefType:$pingBuffer,
    MemRefType:$pongBuffer,
    I1:$arbiter,              // Current active buffer
    Region:$interTransfer,    // Inter-transfer logic
    Region:$intraTransfer     // Intra-transfer logic
  );
  
  let results = (outs);
}

// 数据传输操作
def SystolicDataTransfer : SystolicDataflow_Op<"data_transfer"> {
  let summary = "Data transfer between levels";
  
  let arguments = (ins
    StrAttr:$type,           // "inter" or "intra"
    AnyType:$source,
    AnyType:$destination
  );
  
  let results = (outs);
}
```

### 2. 数组引用分组分析

在 `SystolicDataflowGeneration` Pass 中实现：

```cpp
// lib/Transforms/SystolicDataflowGeneration.cpp

/// 数组引用分组（类似 AutoSA 的 autosa_array_ref_group）
struct ArrayRefGroup {
  Value memref;
  std::string name;
  SmallVector<AffineLoadOp, 8> loads;
  SmallVector<AffineStoreOp, 8> stores;
  
  // IO 分类
  enum GroupType { IO_GROUP, PE_GROUP, DRAIN_GROUP } type;
  
  // IO 层级（1-3）
  int ioLevel = 0;
  
  // 是否需要双缓冲
  bool needsDoubleBuffer = false;
  
  // 缓冲区形状
  SmallVector<int64_t, 3> bufferShape;
};

/// 分析函数中的数组引用并分组
SmallVector<ArrayRefGroup> analyzeArrayReferences(FuncOp func) {
  // 1. 收集所有 load/store 操作
  // 2. 按 memref 分组
  // 3. 分析访问模式确定 IO/PE/Drain 类型
  // 4. 确定 IO 层级（基于循环嵌套深度）
  // 5. 确定是否需要双缓冲（基于数据重用）
}
```

### 3. 多层 IO 模块生成

```cpp
/// 生成多层 IO 模块结构
void generateMultiLevelIO(OpBuilder &builder, ArrayRefGroup &group) {
  Location loc = builder.getUnknownLoc();
  
  // 根据 ioLevel 生成不同层级的模块
  if (group.ioLevel >= 3) {
    // L3 IO Module: Global Memory <-> L2 Buffer
    auto l3Module = builder.create<systolic::dataflow::IOModuleOp>(
        loc, /*level=*/3, /*direction=*/"in",
        /*arrayName=*/group.name,
        /*doubleBuffer=*/false  // L3 通常不需要双缓冲
    );
    
    // 在 L3 模块内生成内存访问逻辑
    OpBuilder::InsertionGuard guard(builder);
    builder.setInsertionPointToStart(l3Module.getBody());
    // ... 生成 affine.load/store ...
  }
  
  if (group.ioLevel >= 2) {
    // L2 IO Module: L2 Buffer <-> L1 Buffer (带双缓冲)
    auto l2Module = builder.create<systolic::dataflow::IOModuleOp>(
        loc, /*level=*/2, /*direction=*/"in",
        /*arrayName=*/group.name,
        /*doubleBuffer=*/true,  // L2 需要双缓冲
        /*bufferShape=*/group.bufferShape
    );
    
    // 在 L2 模块内生成双缓冲逻辑
    OpBuilder::InsertionGuard guard(builder);
    builder.setInsertionPointToStart(l2Module.getBody());
    generateDoubleBufferLogic(builder, group);
  }
  
  if (group.ioLevel >= 1) {
    // L1 IO Module: L1 Buffer <-> PE Array
    auto l1Module = builder.create<systolic::dataflow::IOModuleOp>(
        loc, /*level=*/1, /*direction=*/"in",
        /*arrayName=*/group.name,
        /*doubleBuffer=*/false  // L1 通常不需要双缓冲
    );
    
    // 在 L1 模块内生成到 PE 的数据流
    // ...
  }
}
```

### 4. 双缓冲逻辑生成

```cpp
/// 生成双缓冲（ping-pong）逻辑
void generateDoubleBufferLogic(OpBuilder &builder, ArrayRefGroup &group) {
  Location loc = builder.getUnknownLoc();
  
  // 1. 分配 ping-pong 缓冲区
  auto pingBuffer = builder.create<memref::AllocaOp>(
      loc, MemRefType::get(group.bufferShape, elementType));
  auto pongBuffer = builder.create<memref::AllocaOp>(
      loc, MemRefType::get(group.bufferShape, elementType));
  
  // 2. 创建 arbiter（选择当前活跃缓冲区）
  auto arbiterType = MemRefType::get({}, builder.getI1Type());
  auto arbiter = builder.create<memref::AllocaOp>(loc, arbiterType);
  builder.create<memref::StoreOp>(loc, 
      builder.create<arith::ConstantIntOp>(loc, 0, 1), arbiter);
  
  // 3. 创建 intra-transfer enable flag
  auto intraEnable = builder.create<memref::AllocaOp>(loc, arbiterType);
  builder.create<memref::StoreOp>(loc,
      builder.create<arith::ConstantIntOp>(loc, 0, 1), intraEnable);
  
  // 4. 在循环嵌套中生成双缓冲逻辑
  // Inter-transfer: 从上级流读取数据到当前缓冲区
  // Intra-transfer: 从当前缓冲区发送数据到下级流
  // 使用 scf.if 根据 arbiter 选择 ping 或 pong
  
  // 5. 切换 arbiter
  // arbiter = !arbiter
}
```

### 5. Dialect 降级 Pass

将 SystolicDataflow Dialect 降级到 HLS Dialect：

```cpp
// lib/Transforms/SystolicDataflowToHLS.cpp

class SystolicDataflowToHLSLowering : public ConversionPattern {
public:
  void matchAndRewrite(
      systolic::dataflow::IOModuleOp op,
      OpAdaptor adaptor,
      ConversionPatternRewriter &rewriter) const override {
    
    // 将 systolic.io.module 转换为 hls.dataflow.task
    auto taskOp = rewriter.create<hls::dataflow::TaskOp>(
        op.getLoc(), op.getName());
    
    // 将双缓冲逻辑转换为 Affine loops + SCF if
    if (adaptor.getDoubleBuffer()) {
      // 生成 ping-pong 缓冲区分配
      // 生成 arbiter 逻辑
      // 生成 inter/intra transfer 循环
    }
    
    rewriter.eraseOp(op);
  }
};
```

## 实现路线图

### Phase 1: Dialect 定义（1-2 周）
- [ ] 定义 `SystolicDataflow` Dialect
- [ ] 实现核心操作：`IOModuleOp`, `PEArrayOp`, `DoubleBufferOp`
- [ ] 编写 TableGen 定义
- [ ] 实现基本的构建器方法

### Phase 2: 数组引用分析（1 周）
- [ ] 实现 `analyzeArrayReferences()` 函数
- [ ] 实现 IO/PE/Drain 分类逻辑
- [ ] 实现 IO 层级分析（L1/L2/L3）
- [ ] 实现双缓冲需求分析

### Phase 3: 数据流生成（2-3 周）
- [ ] 实现 `SystolicDataflowGeneration` Pass
- [ ] 实现多层 IO 模块生成
- [ ] 实现双缓冲逻辑生成
- [ ] 实现 PE 阵列生成
- [ ] 实现 Drain 模块生成

### Phase 4: Dialect 降级（1-2 周）
- [ ] 实现 `SystolicDataflowToHLS` Pass
- [ ] 将 IO 模块降级到 HLS Task
- [ ] 将双缓冲降级到 Affine/SCF
- [ ] 将 Stream 操作映射到 HLS Stream

### Phase 5: 测试与验证（1-2 周）
- [ ] MatMul 端到端测试
- [ ] 与 AutoSA 输出对比
- [ ] 多层双缓冲 IO 验证
- [ ] 性能对比

## 优势

### 1. 充分利用 MLIR 特性

- **Dialect 系统**：用专门的 Dialect 表达脉动阵列语义，而不是直接生成 HLS 代码
- **Pass 组合**：每个阶段是独立的 Pass，可以灵活组合和优化
- **类型系统**：利用 MLIR 的类型系统表达缓冲区、流等概念
- **变换能力**：可以在不同抽象层级之间转换和优化

### 2. 易于扩展

- **新硬件结构**：可以添加新的 Dialect 操作来表达不同的硬件结构
- **优化 Pass**：可以在 SystolicDataflow 层级添加优化 Pass，而不影响代码生成
- **多后端支持**：可以添加不同的降级 Pass 支持不同的 HLS 工具

### 3. 清晰的抽象层级

```
Affine IR (循环嵌套)
    ↓ SystolicDataflowGeneration
SystolicDataflow Dialect (硬件抽象)
    ↓ SystolicDataflowToHLS
HLS Dialect (数据流)
    ↓ EmitHLSCpp
HLS C++ (最终代码)
```

每一层都有清晰的职责，便于理解和维护。

### 4. 支持多层双缓冲 IO

通过 `SystolicDataflow` Dialect 的 `IOModuleOp` 和 `DoubleBufferOp`，可以清晰地表达：
- L3 IO 模块（Global Memory 接口）
- L2 IO 模块（带双缓冲）
- L1 IO 模块（PE 接口）
- 各层级之间的数据流

## 与 AutoSA 的对应关系

| AutoSA 概念 | MLIR-Systolic 实现 |
|------------|-------------------|
| `autosa_array_ref_group` | `ArrayRefGroup` (分析阶段) |
| `autosa_io_buffer` | `systolic.dataflow.io.module` 操作 |
| `autosa_hw_module` | `systolic.dataflow.io.module` / `pe.array` |
| `io_level` (1-3) | `IOModuleOp` 的 `level` 属性 |
| `inter_transfer` / `intra_transfer` | `DoubleBufferOp` 的 region |
| `ping-pong buffer` | `DoubleBufferOp` 操作 |
| `autosa_pe_module` | `systolic.dataflow.pe.array` |

## 总结

这个方案的核心是**用 MLIR 的方式重新表达 AutoSA 的语义**，而不是直接翻译。通过引入 `SystolicDataflow` Dialect，我们：

1. **清晰地表达了多层 IO 结构**，解决了当前无法生成多层双缓冲 IO 的问题
2. **充分利用了 MLIR 的特性**，使后续扩展和优化更容易
3. **保持了与 AutoSA 的语义对应**，确保生成的代码功能正确

这个设计既实现了 AutoSA 的核心功能，又为后续利用 MLIR 特性进行改进打下了基础。


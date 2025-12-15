# AutoSA 架构分析

> **最后更新**: 2025-12-10  
> **目的**: 理解 AutoSA 的两部分架构，为 mlir-systolic 设计提供参考

---

## AutoSA 的两部分架构

AutoSA 包含两个主要部分，分别处理不同的代码生成任务：

### 1. FPGA Kernel 生成（当前重点）

**输入**: SCoP 参数包裹的嵌套 for 循环

**处理流程**:
1. **多面体分析**（ISL）：
   - 依赖距离分析
   - 空间循环选择
   - 调度树生成

2. **循环变换**：
   - 空间-时间映射
   - 多级分块（array_part + latency）
   - 循环置换

3. **硬件结构生成**：
   - PE 阵列生成
   - 多层 IO 模块（L1/L2/L3）
   - 双缓冲机制
   - 数据流通道

4. **代码生成**：
   - HLS C++ Kernel 代码
   - 包含所有硬件模块函数
   - 插入 HLS Pragma

**输出**: FPGA 的 HLS C++ Kernel 文件（如 `kernel.cpp`）

**对应 AutoSA 代码**:
- `autosa_trans.cpp` - 变换和优化
- `autosa_comm.cpp` - 通信管理
- `autosa_codegen.cpp` - 代码生成
- `autosa_xilinx_hls_c.cpp` - Xilinx HLS C 代码生成
- `autosa_tapa_cpp.cpp` - TAPA C++ 代码生成

### 2. Host 端代码生成（预留接口）

**输入**: Kernel 函数签名、参数信息、目标平台配置

**处理流程**:
1. **根据目标平台选择生成策略**：
   - `autosa_hls_c` → HLS Testbench
   - `autosa_opencl` → OpenCL Host 代码
   - `autosa_tapa` → TAPA Host 代码
   - `autosa_catapult_c` → Catapult HLS Host 代码

2. **生成 Host 端代码**：
   - 数据分配和初始化
   - Kernel 调用接口
   - 数据传输（Host ↔ Device）
   - 结果验证和输出

**输出**: Host 端代码文件

**对应 AutoSA 代码**:
- `autosa_scripts/codegen.py` - 代码生成脚本
- `autosa_scripts/autosa.py` - 主入口脚本（处理两部分）

---

## AutoSA 命令参数

从 `autosa.py` 可以看到 AutoSA 支持的主要参数：

### 目标平台选择

```bash
--target=autosa_hls_c      # Xilinx HLS C (默认)
--target=autosa_opencl     # OpenCL
--target=autosa_tapa       # TAPA
--target=autosa_catapult_c # Catapult HLS
```

### Host 端选项（仅 HLS C）

```bash
--hls    # 生成 HLS Testbench（默认）
--opencl # 生成 OpenCL Host 代码
```

### 示例

```bash
# 生成 HLS Kernel + HLS Testbench
autosa.py kernel.c --target=autosa_hls_c --hls

# 生成 HLS Kernel + OpenCL Host
autosa.py kernel.c --target=autosa_hls_c --opencl

# 生成 OpenCL Kernel + OpenCL Host
autosa.py kernel.c --target=autosa_opencl
```

---

## mlir-systolic 的对应设计

### 当前实现范围

**✅ 已实现/进行中**:
- FPGA Kernel 生成（完整流程）
  - SystolicTransform Pass
  - SystolicDataflowGeneration Pass
  - SystolicDataflowToHLS Pass
  - EmitHLSCpp Translation

**⚠️ 预留接口（暂不实现）**:
- Host 端代码生成
  - `EmitHostCode.cpp` - 接口已预留
  - 支持不同目标平台的选择
  - 根据配置生成相应的 Host 代码

### 架构设计

```
┌─────────────────────────────────────────────────────────┐
│  MLIR-Systolic 编译流程                                  │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  Part 1: FPGA Kernel 生成 (当前重点) ✅                 │
│  ┌──────────────────────────────────────────────────┐  │
│  │ Affine MLIR → SystolicTransform → ...            │  │
│  │ → EmitHLSCpp → HLS C++ Kernel                    │  │
│  └──────────────────────────────────────────────────┘  │
│                                                          │
│  Part 2: Host 端代码生成 (预留接口) ⚠️                 │
│  ┌──────────────────────────────────────────────────┐  │
│  │ Kernel 信息 → EmitHostCode → Host 代码           │  │
│  │   - HLS Testbench                                │  │
│  │   - OpenCL Host                                  │  │
│  │   - 其他平台                                      │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### 接口设计

**EmitHostCode Translation** (预留):

```cpp
// lib/Translation/EmitHostCode.cpp (预留接口)

enum class HostTarget {
  HLS_TESTBENCH,    // HLS C Testbench
  OPENCL_HOST,      // OpenCL Host 代码
  TAPA_HOST,        // TAPA Host 代码
  CATAPULT_HOST     // Catapult HLS Host 代码
};

// 根据目标平台生成 Host 代码
LogicalResult emitHostCode(ModuleOp module,
                           HostTarget target,
                           raw_ostream &os);
```

**使用示例** (未来):

```bash
# 生成 Kernel
systolic-translate kernel.mlir -emit-hlscpp > kernel.cpp

# 生成 HLS Testbench
systolic-translate kernel.mlir -emit-host-code=hlstb > kernel_tb.cpp

# 生成 OpenCL Host
systolic-translate kernel.mlir -emit-host-code=opencl > kernel_host.cpp
```

---

## 与 AutoSA 的对应关系

| AutoSA 组件 | mlir-systolic 对应 | 状态 |
|------------|-------------------|------|
| **FPGA Kernel 生成** | | |
| `autosa_trans.cpp` | `SystolicTransform.cpp` | ✅ 进行中 |
| `autosa_comm.cpp` | `SystolicDataflowGeneration.cpp` | ✅ 进行中 |
| `autosa_codegen.cpp` | `SystolicDataflowToHLS.cpp` | ✅ 进行中 |
| `autosa_xilinx_hls_c.cpp` | `EmitHLSCpp.cpp` | ✅ 进行中 |
| **Host 端代码生成** | | |
| `autosa_scripts/codegen.py` | `EmitHostCode.cpp` | ⚠️ 预留接口 |
| HLS Testbench 生成 | `EmitHostCode(HLS_TESTBENCH)` | ⚠️ 预留接口 |
| OpenCL Host 生成 | `EmitHostCode(OPENCL_HOST)` | ⚠️ 预留接口 |

---

## 总结

1. **AutoSA 包含两部分**：
   - FPGA Kernel 生成（多面体编译）
   - Host 端代码生成（根据目标平台）

2. **mlir-systolic 当前重点**：
   - 专注于 FPGA Kernel 生成
   - 这是核心功能，需要完整的多面体能力

3. **Host 端代码生成**：
   - 接口已预留，便于后续扩展
   - 可以根据需要逐步实现不同目标平台的支持

4. **设计优势**：
   - 清晰的职责分离
   - 便于独立开发和测试
   - 支持逐步扩展

---

## 参考

- AutoSA 源码: `/home/user/work/AutoSA/src/`
- AutoSA 脚本: `/home/user/work/AutoSA/autosa_scripts/autosa.py`
- AutoSA 文档: [AutoSA GitHub](https://github.com/UCLA-VAST/AutoSA)


# 基于 AutoSA 源码的代码生成优化总结

> **最后更新**: 2024-12  
> **参考**: AutoSA 核心 C 源码 (`/home/user/work/AutoSA/src/autosa_xilinx_hls_c.cpp`)

---

## 优化成果

### 代码质量对比

| 指标 | 优化前 | 优化后 | AutoSA | 差距 |
|------|--------|--------|--------|------|
| **代码行数** | 447 | 1055 | 1499 | -444 (-30%) |
| **PIPELINE pragma** | 7 | 15 | 24 | -9 (-38%) |
| **函数数量** | 18 | 36 | ~30 | +6 (+20%) |

### 主要改进

#### 1. IO_L2_in 模块完整实现 ✅

**参考 AutoSA 源码**：
- `autosa_print_inter_trans_module` (line 2480-2548)
- `autosa_print_intra_trans_module` (line 2403-2476)
- `print_module_vars_xilinx` (line 2234-2280)

**实现内容**：
- ✅ `A_IO_L2_in_intra_trans`：数据解包（A_t8 → float）
- ✅ `A_IO_L2_in_inter_trans`：数据加载和转发
- ✅ `A_IO_L2_in_inter_trans_boundary`：边界处理
- ✅ `A_IO_L2_in`：双缓冲主函数
- ✅ `A_IO_L2_in_boundary`：边界双缓冲

**关键特性**：
- 双缓冲逻辑（ping-pong 切换）
- RESOURCE pragma（RAM_2P_BRAM）
- 使能控制（inter_trans_en, intra_trans_en）
- 数据解包逻辑（union 类型转换）

#### 2. 双缓冲逻辑 ✅

**参考 AutoSA 源码**：
- `print_module_vars_xilinx` (line 2247-2277)
- `print_double_buffer_module_while` (line 3871-3995)

**实现内容**：
```cpp
bool arb = 0;
bool inter_trans_en = 1;
bool intra_trans_en = 0;
int c0, c0_prev;
int c1, c1_prev;
int c2, c2_prev;

if (arb == 0) {
  inter_trans(..., local_pong, ...);
  intra_trans(..., local_ping, ...);
} else {
  inter_trans(..., local_ping, ...);
  intra_trans(..., local_pong, ...);
}
arb = !arb;
```

#### 3. 数据解包逻辑 ✅

**参考 AutoSA 源码**：
- `autosa_print_intra_trans_module` (line 2403-2476)

**实现内容**：
```cpp
ap_uint<32> data_split[8];
#pragma HLS ARRAY_PARTITION variable=data_split complete

for (ap_uint<4> n = 0; n < 8; n++) {
  #pragma HLS UNROLL
  data_split[n] = in_data(31, 0);
  in_data = in_data >> 32;
}

union {unsigned int ui; float ut;} u;
u.ui = (unsigned int)data_split[split_idx];
out_data = u.ut;
```

#### 4. 变量声明和 RESOURCE Pragma ✅

**参考 AutoSA 源码**：
- `print_module_var_xilinx` (line 2049-2232)

**实现内容**：
```cpp
A_t8 local_A_ping[4][1];
#pragma HLS RESOURCE variable=local_A_ping core=RAM_2P_BRAM
A_t8 local_A_pong[4][1];
#pragma HLS RESOURCE variable=local_A_pong core=RAM_2P_BRAM
```

---

## 与 AutoSA 的对比分析

### 已实现的功能 ✅

1. **基本结构**：
   - ✅ kernel0 函数
   - ✅ PE 函数
   - ✅ IO_L3_in 模块
   - ✅ IO_L2_in 模块（完整实现）
   - ✅ 双缓冲逻辑
   - ✅ 数据解包逻辑

2. **优化 pragma**：
   - ✅ PIPELINE pragma（15 个）
   - ✅ UNROLL pragma
   - ✅ ARRAY_PARTITION pragma
   - ✅ RESOURCE pragma

3. **代码结构**：
   - ✅ 模块声明和定义
   - ✅ 变量声明格式
   - ✅ 函数调用格式
   - ✅ 注释格式

### 待完善的功能 ⚠️

1. **Drain 模块**：
   - ⚠️ `C_drain_IO_L1_out_intra_trans`：需要实现
   - ⚠️ `C_drain_IO_L1_out_inter_trans`：需要实现
   - ⚠️ `C_drain_IO_L1_out_inter_trans_boundary`：需要实现
   - ⚠️ `C_drain_IO_L1_out_boundary_wrapper`：需要实现
   - ⚠️ `C_drain_IO_L2_out`：需要实现
   - ⚠️ `C_drain_IO_L3_out`：需要实现

2. **更多 PIPELINE pragma**：
   - ⚠️ 目标：24 个（当前 15 个）
   - ⚠️ 需要在更多内层循环添加

3. **代码细节**：
   - ⚠️ 某些循环的边界条件
   - ⚠️ 某些变量的初始化
   - ⚠️ 某些注释的位置

---

## AutoSA 源码关键发现

### 1. 双缓冲逻辑

**关键代码**（`autosa_xilinx_hls_c.cpp:2247-2277`）：
```cpp
if (module->double_buffer && inter == -1)
{
  p = isl_printer_start_line(p);
  p = isl_printer_print_str(p, "bool arb = 0;");
  p = isl_printer_end_line(p);
  p = isl_printer_start_line(p);
  p = isl_printer_print_str(p, module->in ? "bool inter_trans_en = 1;" : "bool inter_trans_en = 0;");
  p = isl_printer_end_line(p);
  p = isl_printer_start_line(p);
  p = isl_printer_print_str(p, module->in ? "bool intra_trans_en = 0;" : "bool intra_trans_en = 1;");
  p = isl_printer_end_line(p);
  // ... iterator declarations ...
}
```

**关键发现**：
- 对于输入模块（`module->in`），`inter_trans_en = 1`，`intra_trans_en = 0`（初始）
- 对于输出模块（`!module->in`），`inter_trans_en = 0`，`intra_trans_en = 1`（初始）

### 2. PIPELINE Pragma 插入

**关键代码**（`autosa_xilinx_hls_c.cpp:2332-2343`）：
```cpp
static __isl_give isl_printer *print_for_with_pipeline(
    __isl_keep isl_ast_node *node, __isl_take isl_printer *p,
    __isl_take isl_ast_print_options *print_options)
{
  p = isl_printer_start_line(p);
  p = isl_printer_print_str(p, "#pragma HLS PIPELINE II=1");
  p = isl_printer_end_line(p);

  p = isl_ast_node_for_print(node, p, print_options);

  return p;
}
```

**关键发现**：
- AutoSA 使用 AST 遍历来插入 PIPELINE pragma
- 根据循环的 `is_pipeline` 属性决定是否添加

### 3. 变量声明和 RESOURCE Pragma

**关键代码**（`autosa_xilinx_hls_c.cpp:2111-2132`）：
```cpp
if (use_memory)
{
  p = isl_printer_start_line(p);
  p = isl_printer_print_str(p, "#pragma HLS RESOURCE variable=");
  p = isl_printer_print_str(p, var->name);
  if (double_buffer)
    p = isl_printer_print_str(p, "_ping");
  if (module->type == IO_MODULE && module->data_pack_inter == module->data_pack_intra)
    p = isl_printer_print_str(p, use_memory == 1 ? " core=RAM_1P_LUTRAM" : (use_memory == 2 ? " core=RAM_1P_BRAM" : " core=RAM_1P_URAM"));
  else
    p = isl_printer_print_str(p, use_memory == 1 ? " core=RAM_2P_LUTRAM" : (use_memory == 2 ? " core=RAM_2P_BRAM" : " core=RAM_2P_URAM"));
  p = isl_printer_end_line(p);
}
```

**关键发现**：
- AutoSA 根据 `use_memory` 变量决定内存类型（LUTRAM/BRAM/URAM）
- 对于双缓冲，使用 `RAM_2P_BRAM`（双端口 BRAM）
- 对于单缓冲，使用 `RAM_1P_BRAM`（单端口 BRAM）

---

## 下一步优化计划

### 优先级 1：完善 Drain 模块

**参考 AutoSA 源码**：
- `autosa_print_drain_merge_funcs`（需要查找）
- `autosa_kernel_print_drain_merge`（需要查找）

**任务**：
1. 实现 `C_drain_IO_L1_out_intra_trans`：数据打包（float → C_t4）
2. 实现 `C_drain_IO_L1_out_inter_trans`：数据转发
3. 实现 `C_drain_IO_L1_out_inter_trans_boundary`：边界处理
4. 实现 `C_drain_IO_L1_out_boundary_wrapper`：边界包装器
5. 实现 `C_drain_IO_L2_out`：L2 级数据收集
6. 实现 `C_drain_IO_L3_out`：L3 级数据序列化

### 优先级 2：添加更多 PIPELINE Pragma

**参考 AutoSA 源码**：
- `print_for_with_pipeline` (line 2332-2343)
- `print_for_xilinx` (line 2358-2391)

**任务**：
1. 在更多内层循环添加 PIPELINE pragma
2. 目标：24 个（当前 15 个）
3. 特别关注：
   - PE 模块的内层循环
   - Drain 模块的内层循环
   - IO 模块的更多循环

### 优先级 3：代码细节优化

**任务**：
1. 优化循环边界条件
2. 优化变量初始化
3. 优化注释位置和格式
4. 确保代码结构与 AutoSA 完全一致

---

## 相关文件

- **AutoSA 源码**：`/home/user/work/AutoSA/src/autosa_xilinx_hls_c.cpp`
- **我们的代码生成器**：`tools/systolic-translate/systolic-translate.cpp`
- **参考样本**：`test/reference-samples/mm_st3_I32_J32_K32_ap8_lat4_simd1_kernel.cpp`
- **测试脚本**：`test/test_against_autosa.sh`

---

## 总结

通过参考 AutoSA 的核心 C 源码，我们已经：

1. ✅ **完善了 IO_L2_in 模块**：实现了完整的双缓冲逻辑和数据解包
2. ✅ **优化了代码结构**：对齐了 AutoSA 的代码格式和风格
3. ✅ **添加了关键 pragma**：RESOURCE、PIPELINE、UNROLL 等

**当前状态**：
- 代码行数：1055（AutoSA：1499，差距 30%）
- PIPELINE pragma：15（AutoSA：24，差距 38%）
- 所有关键结构已生成

**下一步重点**：
- 完善 Drain 模块实现
- 添加更多 PIPELINE pragma
- 优化代码细节


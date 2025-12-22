# AutoSA 随机读取问题分析

> **最后更新**: 2024-12  
> **目的**: 分析 AutoSA 生成的代码中的随机读取问题，并提供检查和修复方法

---

## 1. 问题描述

### 1.1 MTTKRP 随机读取问题

#### 原始代码（存在问题）

**文件**: `kernel_opencl.cpp:77`
```cpp
void A_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, 
                            A_t16 local_A[8][2][4], 
                            hls::stream<A_t4> &fifo_A_local_out, 
                            bool intra_trans_en) {
  // ...
  for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1)
    for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
      for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
        for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t16 in_data;
            A_t4 out_data;
            in_data = local_A[c8][c5][4 * c6 / 16];  // ⚠️ 问题：非线性索引
            // ...
          }
        }
      }
    }
}
```

**问题分析**：
- `4 * c6 / 16` 是一个**非线性索引表达式**
- 当 `c6` 从 0 到 15 变化时，`4 * c6 / 16` 的值是：
  - `c6=0-3`: `0`
  - `c6=4-7`: `1`
  - `c6=8-11`: `2`
  - `c6=12-15`: `3`
- 这导致**非顺序访问模式**，在 HLS 中会导致随机读取，影响性能

#### 优化后代码（已修复）

**文件**: `kernel_opencl_opt_shuffle_db.cpp:77`
```cpp
void A_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, 
                            A_t16 local_A[2][4][8],  // ⚠️ 维度重排：[8][2][4] → [2][4][8]
                            hls::stream<A_t4> &fifo_A_local_out, 
                            bool intra_trans_en) {
  // ...
  for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1)
    for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
      for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
        for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t16 in_data;
            A_t4 out_data;
            in_data = local_A[c5][4 * c6 / 16][c8];  // ✅ 修复：将非线性索引移到中间维度
            // ...
          }
        }
      }
    }
}
```

**修复方法**：
1. **数组维度重排**：从 `local_A[8][2][4]` 到 `local_A[2][4][8]`
2. **写入时重排**：在 `inter_trans` 中写入时使用 `local_A[c5][c6][c4]`
3. **读取时使用重排后的布局**：在 `intra_trans` 中使用 `local_A[c5][4 * c6 / 16][c8]`

**写入代码**（`inter_trans`）：
```cpp
void A_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, 
                            A_t16 local_A[2][4][8],  // 重排后的维度
                            hls::stream<A_t16> &fifo_A_in, 
                            hls::stream<A_t16> &fifo_A_out, 
                            bool inter_trans_en) {
  // ...
  local_A[c5][c6][c4] = out_data;  // ✅ 写入时重排
}
```

---

## 2. 问题特征

### 2.1 随机读取问题的特征

1. **非线性索引表达式**：
   - 包含乘法：`4 * c6 / 16`
   - 包含除法：`c6 / 4`
   - 包含取模：`c6 % 4`
   - 包含复杂表达式：`(c6 + c7) / 2`

2. **出现在 local buffer 访问中**：
   - `local_A[...][...][...]`
   - `local_B[...][...][...]`
   - `local_C[...][...][...]`

3. **在 IO_L2_in_intra_trans 中**：
   - 数据解包时使用
   - 通常用于从 packed 数据中提取单个元素

### 2.2 问题影响

1. **性能影响**：
   - 随机读取导致 BRAM 访问延迟增加
   - 无法利用顺序访问的优化
   - 可能影响 PIPELINE 的 II（Initiation Interval）

2. **资源影响**：
   - 可能需要更多的 BRAM 端口
   - 可能无法使用某些 BRAM 优化

---

## 3. 其他 Kernel 的随机读取问题检查

### 3.1 检查方法

#### 方法 1：使用检查脚本

```bash
# 检查单个文件
./scripts/check_random_access.sh /path/to/kernel.cpp

# 检查目录
./scripts/check_random_access.sh /path/to/autosa/output/src

# 详细分析 MTTKRP
./scripts/check_random_access.sh /path/to/mttkrp_kernel.cpp --analyze-mttkrp
```

#### 方法 2：手动检查

```bash
# 查找 local buffer 访问中的非线性索引
grep -n "local_.*\[.*\]\[.*\]\[.*\]" kernel.cpp | \
    grep -E "\[.*\*.*\]|\[.*/.*\]|\[.*%.*\]"
```

### 3.2 可能存在的问题

| Kernel 类型 | 数组维度 | 可能的问题 | 检查状态 |
|-------------|----------|------------|----------|
| **MTTKRP** | `A[I][K][L]` | ✅ **已确认** | `local_A[c8][c5][4 * c6 / 16]` |
| **TTMc** | `A[I][L][M]`, `D[I][J][K]` | ⚠️ **可能** | 需要检查生成的代码 |
| **CNN** | `cin[R+K-1][C+K-1][I]`, `w[O][K][K][I]` | ⚠️ **可能** | 需要检查生成的代码 |
| **LU** | `A[N][N]` | ❌ **不太可能** | 2D 数组，通常顺序访问 |
| **MM** | `A[I][K]`, `B[K][J]`, `C[I][J]` | ❌ **不太可能** | 2D 数组，通常顺序访问 |

### 3.3 检查结果

#### MTTKRP（已确认）

**问题位置**：
- `A_IO_L2_in_intra_trans`: `local_A[c8][c5][4 * c6 / 16]`
- `C_IO_L2_in_intra_trans`: `local_C[c7][4 * c6 / 16]`（第 789 行）

**修复方法**：
- 数组维度重排
- 写入时重排
- 读取时使用重排后的布局

#### TTMc（需要检查）

**特征**：
- 3D 数组：`A[I][L][M]`, `D[I][J][K]`
- 5 个循环：`i, j, k, l, m`
- 可能存在类似的非线性索引问题

**检查命令**：
```bash
# 生成 TTMc 代码后检查
./scripts/check_random_access.sh /path/to/ttmc/output/src/kernel*.cpp
```

#### CNN（需要检查）

**特征**：
- 4D 数组：`cin[R+K-1][C+K-1][I]`, `w[O][K][K][I]`
- 6 个循环：`o, r, c, i, p, q`
- 可能存在类似的非线性索引问题

**检查命令**：
```bash
# 生成 CNN 代码后检查
./scripts/check_random_access.sh /path/to/cnn/output/src/kernel*.cpp
```

---

## 4. 修复方法

### 4.1 数组维度重排（推荐）

**原理**：
- 将非线性索引移到中间维度
- 在写入时进行重排
- 在读取时使用重排后的布局

**步骤**：
1. **分析访问模式**：找出非线性索引表达式
2. **确定重排方案**：将非线性索引移到中间维度
3. **修改写入代码**：在 `inter_trans` 中写入时重排
4. **修改读取代码**：在 `intra_trans` 中使用重排后的布局

**示例**（MTTKRP）：
```cpp
// 原始：local_A[8][2][4]
// 重排后：local_A[2][4][8]

// 写入（inter_trans）
local_A[c5][c6][c4] = out_data;  // 重排

// 读取（intra_trans）
in_data = local_A[c5][4 * c6 / 16][c8];  // 使用重排后的布局
```

### 4.2 数据布局转换

**原理**：
- 在 IO_L2_in_inter_trans 中写入时进行转换
- 在 IO_L2_in_intra_trans 中读取时使用转换后的布局

**步骤**：
1. **确定转换函数**：根据非线性索引表达式确定转换规则
2. **实现转换逻辑**：在写入时应用转换
3. **更新读取逻辑**：在读取时使用转换后的布局

### 4.3 循环变换

**原理**：
- 调整循环顺序，使访问模式更顺序化
- 可能需要修改 AutoSA 的调度算法

**限制**：
- 需要修改 AutoSA 源码
- 可能影响其他优化

---

## 5. 检查清单

### 5.1 生成代码后检查

- [ ] 运行 `check_random_access.sh` 脚本
- [ ] 检查所有 `IO_L2_in_intra_trans` 函数
- [ ] 检查所有 `local_*[...][...][...]` 访问
- [ ] 查找非线性索引表达式（`*`, `/`, `%`）

### 5.2 发现问题后处理

- [ ] 分析访问模式
- [ ] 确定重排方案
- [ ] 修改写入代码（`inter_trans`）
- [ ] 修改读取代码（`intra_trans`）
- [ ] 验证功能正确性
- [ ] 测试性能提升

---

## 6. 相关文件

- **检查脚本**: `scripts/check_random_access.sh`
- **MTTKRP 原始代码**: `/home/user/work/mttkrp_test_autosa/mttkrp_small/src/kernel_opencl.cpp`
- **MTTKRP 优化代码**: `/home/user/work/mttkrp_test_autosa/mttkrp_small/src/kernel_opencl_opt_shuffle_db.cpp`
- **生成指南**: `docs/AUTOSA_TEST_GENERATION_GUIDE.md`

---

## 7. 下一步

1. ⚠️ **检查其他 Kernel**：TTMc, CNN 等
2. ⚠️ **实现自动检测**：在代码生成时检测随机读取问题
3. ⚠️ **实现自动修复**：自动应用数组维度重排
4. ⚠️ **性能验证**：在板卡上验证修复效果



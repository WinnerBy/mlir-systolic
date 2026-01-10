# AutoSA Multi-Kernel 测试结果

测试时间: 2026-01-10

## 测试结果汇总

| Kernel | 状态 | 代码行数 | PE数 | I/O模块 | Pragma数 | 备注 |
|--------|------|---------|------|---------|---------|------|
| cnn_small | ✅ | 1,845 | 10 | 213 | 192 | CNN单层(小型) |
| cnn_large | ✅ | 9,795 | 226 | 1,725 | 2,047 | CNN单层(大型) |
| lu | ❌ | - | - | - | - | LU分解(AutoSA失败) |
| mttkrp | ✅ | 7,775 | 130 | 1,252 | 1,607 | 张量分解 |
| ttmc | ✅ | 7,548 | 130 | 1,132 | 1,543 | 张量矩阵乘 |

## 成功率

- **总计**: 5个kernel
- **成功**: 4个 (80%)
- **失败**: 1个 (20%)

## 各Kernel详细信息

### 1. CNN Small (成功)

**配置参数:**
- Space-Time: 4
- Array Partition: [8,8,4,8]
- Latency: [4,2,4]
- SIMD: [1,1,1,2]
- 额外标志: `--host-serialize --no-reverse-order`

**生成结果:**
- 代码行数: 1,845
- PE Wrappers: 10
- I/O 模块: 213
- HLS Pragmas: 192

### 2. CNN Large (成功)

**配置参数:**
- Space-Time: 4
- Array Partition: [64,56,14,64]
- Latency: [4,4,7]
- SIMD: [1,1,8]

**生成结果:**
- 代码行数: 9,795 (5.3x vs small)
- PE Wrappers: 226 (22.6x vs small)
- I/O 模块: 1,725 (8.1x vs small)
- HLS Pragmas: 2,047 (10.7x vs small)

### 3. LU Decomposition (失败)

**配置参数:**
- Space-Time: 3
- Array Partition: [-1,-1,-1] (自动推断)
- 额外标志: `--use-cplusplus-template --no-reschedule`

**失败原因:**
- AutoSA内部错误，未给出详细信息
- 可能与ISL调度器或C++模板生成相关
- 需要进一步调试

### 4. MTTKRP (成功)

**配置参数:**
- Space-Time: 3
- Array Partition: [128,128,2]
- Latency: [16,8]
- SIMD: [8,1]
- 额外标志: `--local-reduce --reduce-op=+`

**生成结果:**
- 代码行数: 7,775
- PE Wrappers: 130
- I/O 模块: 1,252
- HLS Pragmas: 1,607

### 5. TTMC (成功)

**配置参数:**
- Space-Time: 4
- Array Partition: [16,64,16,32]
- Latency: [1,8,8]
- SIMD: [8,1]

**生成结果:**
- 代码行数: 7,548
- PE Wrappers: 130
- I/O 模块: 1,132
- HLS Pragmas: 1,543

## 对比分析

### 代码量级别
1. **小型kernel** (CNN Small): ~2K行
2. **中型kernel** (MTTKRP, TTMC): ~7-8K行
3. **大型kernel** (CNN Large): ~10K行

### 复杂度指标
- **CNN Large** 是最复杂的设计：
  - 226个PE wrappers
  - 1,725个I/O模块
  - 2,047个HLS pragma

- **MTTKRP/TTMC** 复杂度相近：
  - 130个PE wrappers
  - 1,100-1,250个I/O模块
  - 1,500-1,600个pragma

## 输出文件位置

所有生成的参考代码位于:
```
test/output/autosa_multi_kernel/
├── cnn_small/
│   ├── kernel.cpp
│   ├── metadata.json
│   └── src/
├── cnn_large/
├── lu/
├── mttkrp/
└── ttmc/
```

## 下一步工作

1. ❌ **修复LU kernel生成问题**
   - 调试AutoSA LU生成
   - 可能需要调整ISL调度参数
   
2. ⏳ **扩展mlir-systolic支持**
   - 当前mlir-systolic主要测试矩阵乘法
   - 需要测试CNN、MTTKRP等复杂kernel
   
3. ⏳ **性能对比**
   - 比较mlir-systolic与AutoSA的代码质量
   - 分析不同kernel类型的生成效率

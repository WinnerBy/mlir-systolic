# AutoSA HLS参考实现

本目录包含从AutoSA生成的26个HLS C++参考实现，用于对比和验证mlir-systolic的输出。

## 文件命名规则

文件名格式：`{kernel}_{variant}.cpp`

例如：
- `mm_st3.cpp`: 矩阵乘法，space-time模式3
- `cnn_st4.cpp`: CNN，space-time模式4
- `mttkrp_st3_ap128_128_2_lat16_8_simd8_1.cpp`: MTTKRP，详细参数配置

## 样本统计

- **总样本数**: 26个
- **总代码行数**: 75,235行
- **平均行数**: 2,893行/样本

### 按Kernel类型分类

| Kernel类型 | 样本数 | 代码行数范围 |
|-----------|--------|-------------|
| MM (矩阵乘法) | 12 | 1,123-2,007行 |
| CNN (卷积网络) | 8 | 1,176-1,875行 |
| MTTKRP (张量分解) | 3 | 7,705-7,775行 |
| TTMC (张量链) | 3 | 4,860-7,548行 |

## 参数变化

### Space-Time模式
- **ST0-ST2**: 1D systolic arrays
- **ST3-ST5**: 2D systolic arrays  
- **ST6-ST9**: 高维/特殊配置

### Array Partition大小
- 小: [4,16,16] or [8,8,4,8]
- 中: [16,16,16] or [16,64,16,32]
- 大: [32,32,32] or [128,128,2]

### Latency Hiding因子
- 低: [4,4] or [1,8,8]
- 中: [8,8] or [4,2,4]
- 高: [16,8] or [16,16]

### SIMD向量化
- 无: simd[1]
- 低: simd[2]
- 中: simd[4]
- 高: simd[8]
- 多维: simd[1,1,1,2]

## 使用说明

这些文件用于：
1. **对比验证**: 与mlir-systolic输出进行功能和性能对比
2. **参考学习**: 理解AutoSA的代码生成策略
3. **测试基准**: 作为正确性验证的golden reference

## 生成详情

- **生成工具**: AutoSA v1.0
- **生成时间**: 2026年1月
- **生成脚本**: `test/generate_autosa_variants.sh`
- **详细日志**: 保存在autosa_reference_samples目录（本地）

## 相关文档

- [AutoSA调试指南](../autosa_debugging_guide.md)
- [变体生成脚本](../generate_autosa_variants.sh)
- [完整统计数据](../../autosa_reference_samples/STATISTICS.csv)（本地）

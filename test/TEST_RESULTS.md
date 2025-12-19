# mlir-systolic vs AutoSA 测试结果

> **最后更新**: 2024-12  
> **测试脚本**: `test/test_against_autosa.sh`

---

## 测试概述

本测试对比 mlir-systolic 生成的 HLS C++ 代码与 AutoSA 生成的参考代码，确保我们的工具与 AutoSA 保持一致。

### 测试配置

测试使用 AutoSA 参考样本目录中的不同参数配置：

| 配置 | 矩阵尺寸 | array_part | latency | simd | PE 阵列 |
|------|----------|------------|---------|------|---------|
| I32_J32_K32_ap8_lat4_simd1 | 32×32 | 8 | 4 | 1 | 2×2 |
| I32_J32_K32_ap8_lat4_simd2 | 32×32 | 8 | 4 | 2 | 2×2 |
| I64_J64_K64_ap16_lat8_simd1 | 64×64 | 16 | 8 | 1 | 2×2 |
| I64_J64_K64_ap16_lat8_simd2 | 64×64 | 16 | 8 | 2 | 2×2 |
| I64_J64_K64_ap16_lat8_simd4 | 64×64 | 16 | 8 | 4 | 2×2 |

---

## 测试流程

1. **生成 MLIR 输入**：根据配置参数生成矩阵乘法的 MLIR 代码
2. **运行 SystolicTransform**：应用空间时间变换、分块和置换
3. **生成 HLS C++**：使用 `systolic-translate` 生成 HLS C++ 代码
4. **对比参考代码**：与 AutoSA 生成的参考代码进行对比

---

## 当前测试结果

### 配置：I32_J32_K32_ap8_lat4_simd1

**基本对比**：
- ✅ kernel0 函数存在
- ✅ PE 函数存在
- ✅ IO 模块存在
- ✅ 数据类型定义存在

**统计信息**：
- 我们的代码行数：447
- AutoSA 代码行数：1499
- PIPELINE pragma：我们的=7，AutoSA=24

**分析**：
- 我们的代码更简洁，但可能缺少一些细节实现
- PIPELINE pragma 数量较少，可能需要添加更多流水线优化
- 基本结构正确，但需要完善细节

---

## 关键差异分析

### 1. 代码行数差异

**原因**：
- AutoSA 生成了完整的 IO 模块实现（包括 inter-transfer 和 intra-transfer）
- AutoSA 包含了详细的边界处理逻辑
- AutoSA 包含了完整的数据打包/解包逻辑

**我们的状态**：
- 基本结构已生成
- IO 模块框架已创建
- 需要完善具体实现细节

### 2. PIPELINE Pragma 差异

**原因**：
- AutoSA 在每个内层循环都添加了 PIPELINE pragma
- AutoSA 在 IO 模块的各个层级都添加了流水线优化

**我们的状态**：
- PE 模块的 PIPELINE pragma 已添加
- IO 模块的 PIPELINE pragma 需要补充

### 3. 模块实现完整性

**AutoSA 包含**：
- 完整的 inter-transfer 和 intra-transfer 逻辑
- 双缓冲的 ping-pong 切换逻辑
- 边界模块的特殊处理
- 数据打包/解包的详细实现

**我们的状态**：
- 模块框架已创建
- 双缓冲结构已生成
- 需要完善具体逻辑实现

---

## 下一步改进

### 优先级 1：完善 IO 模块实现

1. **实现 inter-transfer 逻辑**：
   - 从全局内存加载数据到缓冲区
   - 处理数据打包和序列化

2. **实现 intra-transfer 逻辑**：
   - 从缓冲区发送数据到 PE 数组
   - 处理数据解包

3. **完善双缓冲逻辑**：
   - 实现 ping-pong 切换
   - 处理仲裁器逻辑

### 优先级 2：添加更多 PIPELINE Pragma

1. **IO 模块流水线**：
   - 在 L2/L3 IO 模块中添加 PIPELINE pragma
   - 优化数据传输流水线

2. **边界处理**：
   - 在边界模块中添加流水线优化

### 优先级 3：完善代码生成细节

1. **数据打包/解包**：
   - 实现完整的 SIMD 打包逻辑
   - 实现解包逻辑

2. **边界模块**：
   - 生成边界处理模块
   - 处理 PE 数组边界情况

---

## 运行测试

```bash
# 运行完整测试套件
./test/test_against_autosa.sh

# 查看输出
ls -la test/output/

# 对比特定配置
diff test/output/matmul_I32_J32_K32_ap8_lat4_simd1_kernel.cpp \
     test/reference-samples/mm_st3_I32_J32_K32_ap8_lat4_simd1_kernel.cpp
```

---

## 参考文档

- [AutoSA 参考样本 README](reference-samples/README.md)
- [测试脚本](test_against_autosa.sh)
- [项目进展总结](../docs/PROJECT_STATUS.md)


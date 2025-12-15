# 矩阵乘测试用例

## 测试文件

- `matmul.mlir` - 简单的矩阵乘测试用例（32×32）

## 对应 AutoSA 的 SCoP 区域

这个测试用例对应 AutoSA 中 `kernel.c` 的 SCoP 区域（第 16-24 行）：

```c
#pragma scop
for (int i = 0; i < I; i++)
  for (int j = 0; j < J; j++) {
    for (int k = 0; k < K; k++) {        
      C[i][j] = C[i][j] + A[i][k] * B[k][j];
    }
  }
#pragma endscop
```

## 测试假设

**当前简化假设**：
- 只处理单个 `affine.for` 循环嵌套（对应一个 SCoP 区域）
- 不处理 C 文件导入（后续使用 Polygeist）
- 直接使用 MLIR Affine Dialect 编写的测试用例

## 运行测试

```bash
# 应用 SystolicTransform Pass
systolic-opt test/matmul/matmul.mlir \
  -systolic-transform="array-part=16,16,16 latency=8,8" \
  -o matmul_transformed.mlir

# 应用 SystolicDataflowGeneration Pass
systolic-opt matmul_transformed.mlir \
  -systolic-dataflow-generation \
  -o matmul_dataflow.mlir

# 生成 HLS C++
systolic-translate matmul_dataflow.mlir \
  -emit-hlscpp \
  > matmul_kernel.cpp
```

## 参考文件

AutoSA 生成的参考 HLS C++ 文件位于：
- `../reference-samples/` - 不同参数的 AutoSA 输出参考

这些文件展示了 AutoSA 如何将 SCoP 区域转换为 HLS C++ 代码，可以作为 mlir-systolic 输出的参考。


# SCoP 区域说明

> **最后更新**: 2025-12-10  
> **目的**: 说明 AutoSA 如何处理 SCoP 区域，以及 mlir-systolic 的对应设计

---

## AutoSA 的 SCoP 区域处理

### 什么是 SCoP 区域

AutoSA **只处理被 `#pragma scop` / `#pragma endscop` 包裹的嵌套循环**，而不是整个函数中的所有循环。

**原因**：
- 在 FPGA 上很难为所有循环进行加速
- 通常只有一个关键循环是计算瓶颈
- SCoP 区域定义了可以进行多面体分析的循环嵌套

### 示例

在 AutoSA 的测试用例 `kernel.c` 中：

```c
int main(int argc, char **argv) {
  data_t A[I][K], B[J][K], C[I][J], C_golden[I][J];  
  
  // 初始化代码（不被处理）
  for (int i = 0; i < I; i++) 
    for (int k = 0; k < K; k++) {
      A[i][k] = (data_t)rand() / RAND_MAX;
    }

  // SCoP 区域（被处理）⭐
  #pragma scop
  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++) {
      for (int k = 0; k < K; k++) {        
        C[i][j] = C[i][j] + A[i][k] * B[k][j];
      }
    }
  #pragma endscop

  // 验证代码（不被处理）
  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++) {
      C_golden[i][j] = 0;
      for (int k = 0; k < K; k++) {
        C_golden[i][j] = C_golden[i][j] + A[i][k] * B[k][j];
      }
    }
  
  // ...
}
```

**AutoSA 只处理第 16-24 行的 SCoP 区域**，将其转换为 HLS C++ Kernel 代码。

---

## mlir-systolic 的对应设计

### 当前简化假设

**测试阶段**：
- 暂时不考虑 C 文件导入（后续使用 Polygeist 处理）
- **只考虑只有一个 `affine.for` 循环的情况**
- 直接使用 MLIR Affine Dialect 编写的测试用例

### 测试用例结构

**示例** (`test/matmul/matmul.mlir`):

```mlir
module {
  func.func @matmul(%A: memref<32x32xf32>, 
                    %B: memref<32x32xf32>, 
                    %C: memref<32x32xf32>) {
    // 对应 SCoP 区域的嵌套循环
    affine.for %i = 0 to 32 {
      affine.for %j = 0 to 32 {
        affine.for %k = 0 to 32 {
          %a = affine.load %A[%i, %k] : memref<32x32xf32>
          %b = affine.load %B[%k, %j] : memref<32x32xf32>
          %c = affine.load %C[%i, %j] : memref<32x32xf32>
          %mul = arith.mulf %a, %b : f32
          %add = arith.addf %c, %mul : f32
          affine.store %add, %C[%i, %j] : memref<32x32xf32>
        }
      }
    }
    return
  }
}
```

这个 MLIR 文件直接对应 AutoSA 中 SCoP 区域的内容。

### 未来扩展

**Phase 1: 当前（简化）**：
- ✅ 直接使用 MLIR Affine Dialect 测试用例
- ✅ 只处理单个 `affine.for` 循环嵌套
- ✅ 不处理 C 文件导入

**Phase 2: 完整支持**：
- ⚠️ 使用 Polygeist 从 C 文件导入
- ⚠️ 识别 `#pragma scop` / `#pragma endscop` 区域
- ⚠️ 提取 SCoP 区域对应的循环嵌套
- ⚠️ 处理多个 SCoP 区域的情况

---

## 参考文件

AutoSA 生成的参考 HLS C++ 文件位于：
- `test/reference-samples/` - 不同参数的 AutoSA 输出参考

这些文件展示了 AutoSA 如何将 SCoP 区域转换为 HLS C++ 代码，可以作为 mlir-systolic 输出的参考。

---

## 总结

1. **AutoSA 只处理 SCoP 区域**：被 `#pragma scop` / `#pragma endscop` 包裹的循环
2. **mlir-systolic 当前简化**：直接使用 MLIR 测试用例，只处理单个循环嵌套
3. **未来扩展**：使用 Polygeist 处理 C 文件，自动识别 SCoP 区域


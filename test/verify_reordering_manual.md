# 写时重排手动验证指南

> **创建时间**: 2026-01-05  
> **目的**: 手动验证写时重排功能是否正确工作

---

## 验证步骤

### 1. 编译项目

```bash
cd /workspaces/mlir-systolic
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### 2. 运行自动化测试脚本

```bash
cd /workspaces/mlir-systolic
./test/test_write_time_reordering.sh
```

### 3. 手动验证步骤

#### 3.1 检查分析器是否正确检测非线性访问

**步骤**:
1. 创建一个包含非线性访问的 MLIR 文件
2. 运行 `systolic-dataflow-generation` pass
3. 检查函数属性中是否有 `systolic.reorder.*` 属性

**示例 MLIR**:
```mlir
func.func @kernel0(%A: memref<16x2x16xf32>) {
  affine.for %i = 0 to 16 {
    affine.for %j = 0 to 2 {
      affine.for %k = 0 to 32 {
        // 非线性访问: 8 * k / 16
        %idx = affine.apply affine_map<(d0) -> (d0 * 8 floordiv 16)>(%k)
        %val = affine.load %A[%i, %j, %idx] : memref<16x2x16xf32>
      }
    }
  }
  return
}
```

**运行分析**:
```bash
mlir-opt test.mlir \
  -systolic-dataflow-generation \
  -o output.mlir
```

**检查属性**:
```bash
grep "systolic.reorder" output.mlir
```

**预期输出**:
```mlir
func.func @kernel0(...) attributes {
  systolic.reorder.A.dims = [2, 16, 16]
  systolic.reorder.A.perm = [1, 2, 0]
} {
  ...
}
```

#### 3.2 检查代码生成器是否正确应用重排

**步骤**:
1. 使用包含重排属性的 MLIR 文件
2. 运行 `systolic-translate` 生成 HLS C++ 代码
3. 检查生成的代码中数组声明和访问模式

**生成代码**:
```bash
systolic-translate output.mlir \
  -array-part=16 \
  -latency=8 \
  -simd=1 \
  -size=32 \
  -o kernel.cpp
```

**检查数组声明**:
```bash
grep "local_A\[" kernel.cpp
```

**预期输出**:
```cpp
// 重排后的数组声明
void A_IO_L2_in_intra_trans(..., A_t16 local_A[2][16][16], ...);
void A_IO_L2_in_inter_trans(..., A_t16 local_A[2][16][16], ...);
```

**检查访问模式**:
```bash
grep -A 2 "local_A\[" kernel.cpp | head -20
```

**预期输出**:
```cpp
// 写入时: 重排后的索引
local_A[c5][c6][c4] = out_data;

// 读取时: 非线性索引移到中间
in_data = local_A[c5][8 * c6 / 16][c8];
```

#### 3.3 对比 AutoSA 参考代码

**步骤**:
1. 查看 AutoSA 生成的 MTTKRP kernel
2. 对比数组声明和访问模式
3. 验证重排是否正确

**参考文件**:
```bash
grep "local_A\[" autosa_hls_output/kernel_mttkrp.cpp | head -10
```

**预期参考代码**:
```cpp
// AutoSA 生成的代码（已应用重排）
void A_IO_L2_in_intra_trans(..., A_t16 local_A[16][2][16], ...) {
  // 读取: local_A[c8][c5][8 * c6 / 16]
  in_data = local_A[c8][c5][8 * c6 / 16];
}

// 注意: AutoSA 可能使用不同的重排策略
// 我们的实现: [16][2][16] -> [2][16][16]
// 需要验证哪种策略更优
```

---

## 验证检查清单

### 分析阶段验证

- [ ] `WriteTimeReorderingAnalyzer` 能正确检测非线性索引
- [ ] 重排信息正确存储到函数属性
- [ ] 属性格式正确: `systolic.reorder.<arrayName>.dims` 和 `.perm`

### 代码生成阶段验证

- [ ] 代码生成器能正确读取重排属性
- [ ] 数组声明使用重排后的维度
- [ ] 写入索引正确映射到重排后的布局
- [ ] 读取索引正确映射到重排后的布局

### 功能验证

- [ ] 生成的代码能正确编译（HLS 工具）
- [ ] 重排后的访问模式是顺序的（无随机访问）
- [ ] 功能正确性（与原始代码等价）

---

## 调试技巧

### 1. 启用调试输出

在代码中添加调试输出:
```cpp
LLVM_DEBUG(llvm::dbgs() << "Detected non-linear access in dimension " 
                        << pattern.nonLinearDim << "\n");
```

运行时启用:
```bash
mlir-opt test.mlir -systolic-dataflow-generation -debug-only=write-time-reordering-analysis
```

### 2. 检查中间结果

```bash
# 检查分析后的 MLIR
mlir-opt test.mlir -systolic-dataflow-generation | grep -A 5 "systolic.reorder"

# 检查生成的 C++ 代码
systolic-translate output.mlir -o kernel.cpp
grep -n "local_A" kernel.cpp | head -20
```

### 3. 对比原始和重排后的代码

```bash
# 生成原始代码（不应用重排）
# 需要临时禁用重排功能

# 生成重排后的代码
# 对比两者
diff original.cpp reordered.cpp
```

---

## 已知问题和限制

1. **当前仅支持 3 维数组**
   - 对于其他维度，需要扩展重排逻辑

2. **重排策略较简单**
   - 当前策略: 将非线性维度移到中间
   - 未来可以使用 ISL 计算更优策略

3. **需要验证功能正确性**
   - 确保重排后的代码与原始代码功能等价

---

## 相关文档

- [写时重排实现进度](WRITE_TIME_REORDERING_IMPLEMENTATION.md)
- [基于多面体模型的实现](WRITE_TIME_REORDERING_POLYHEDRAL.md)
- [随机读取问题分析](RANDOM_ACCESS_ANALYSIS.md)



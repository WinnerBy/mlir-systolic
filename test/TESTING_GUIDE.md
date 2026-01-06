# mlir-systolic 测试指南

> **最后更新**: 2026-01-06  
> **用途**: 如何运行和添加测试

---

## 快速开始

### 编译项目

```bash
# 构建 Polygeist 和 Polymer
./scripts/build-polygeist.sh

# 构建 mlir-systolic
./scripts/build-systolic.sh
```

### 运行所有测试

```bash
cd test

# 运行所有 spacetime=3 配置测试
./test_all_configs.sh

# 测试写时重排功能
./test_write_time_reordering.sh

# 测试其他 spacetime 配置 (部分实现)
./test_other_spacetime.sh
```

---

## 测试组织

### 目录结构

```
test/
├── matmul/                    # MatMul kernel 测试
│   ├── matmul.mlir           # 基础 MatMul MLIR
│   └── README.md             # MatMul 测试说明
│
├── reference-samples/         # AutoSA 生成的参考代码
│   ├── mm_st3_*.cpp          # 不同配置的参考
│   └── README.md             # 参考样本说明
│
├── output/                    # 测试输出 (Git 忽略)
│   ├── *.cpp                 # 生成的 HLS C++ 代码
│   └── *.mlir                # 中间 MLIR 文件
│
├── *.mlir                     # 测试输入文件
│   ├── matmul_test.mlir      # MatMul 测试
│   ├── mm_32x32.mlir         # 32x32 MatMul
│   ├── mttkrp_*.mlir         # MTTKRP 测试
│   └── ...
│
├── *.sh                       # 测试脚本
│   ├── test_all_configs.sh   # 所有配置测试
│   ├── test_against_autosa.sh # 与 AutoSA 对比
│   └── ...
│
└── *.md                       # 测试文档
    ├── TEST_STATUS.md        # 当前测试状态
    ├── TEST_RESULTS.md       # 测试结果总结
    └── REORDERING_TEST_RESULTS.md # 重排测试结果
```

---

## 当前测试覆盖

### 已完成的测试

| Kernel | Spacetime | 配置数 | 状态 | 测试脚本 |
|--------|-----------|--------|------|---------|
| MM (Matrix Multiply) | ST3 | 11 | ✅ 全部通过 | test_all_configs.sh |
| MTTKRP | - | 1 | ⚠️ 部分实现 | test_mttkrp_reordering.sh |

### 待测试的配置

| Kernel | Spacetime | 状态 | 优先级 |
|--------|-----------|------|--------|
| MM | ST0 ([i]) | ❌ 未实现 | P1 |
| MM | ST1 ([j]) | ❌ 未实现 | P1 |
| MM | ST2 ([k]) | ❌ 未实现 | P2 |
| MM | ST4 ([i,k]) | ❌ 未实现 | P2 |
| MM | ST5 ([j,k]) | ❌ 未实现 | P2 |
| CNN | ST3 | ❌ 未实现 | P2 |
| LU | ST3 | ❌ 未实现 | P3 |

---

## 测试脚本说明

### test_all_configs.sh

测试所有 spacetime=3 的 MM 配置。

**用法**:
```bash
./test_all_configs.sh
```

**测试的配置**:
- 不同的 PE 数量: 16, 32, 64
- 不同的 array partitioning
- 不同的 latency hiding
- 不同的 SIMD 宽度

**预期结果**: 所有 11 个配置都应该编译通过，PIPELINE 数量与 AutoSA 一致。

### test_against_autosa.sh

将生成的代码与 AutoSA 参考代码对比。

**用法**:
```bash
./test_against_autosa.sh <config_name>
```

**对比项**:
- PIPELINE pragma 数量
- 函数数量
- 代码结构
- 关键循环

### test_write_time_reordering.sh

测试写时重排功能。

**用法**:
```bash
./test_write_time_reordering.sh
```

**验证**:
- 检测非线性访问
- 生成重排属性
- 正确应用重排

---

## 添加新测试

### 添加新的 Kernel 测试

1. **创建 MLIR 文件**:
   ```bash
   # 例如: test/new_kernel.mlir
   func.func @new_kernel(%A: memref<...>, ...) {
     // kernel 实现
   }
   ```

2. **创建测试脚本**:
   ```bash
   # test/test_new_kernel.sh
   #!/bin/bash
   
   # 运行 systolic-opt
   ../build/bin/systolic-opt \
     --systolic-transform \
     --systolic-dataflow-generation \
     new_kernel.mlir -o output/new_kernel_transformed.mlir
   
   # 生成 HLS 代码
   ../build/bin/systolic-translate \
     --emit-hls \
     output/new_kernel_transformed.mlir \
     -o output/new_kernel.cpp
   ```

3. **添加到测试套件**:
   - 更新 `test_all_configs.sh` 或创建新的测试脚本
   - 更新本文档的测试覆盖表

### 添加新的 Spacetime 测试

1. **准备 MLIR 输入** (与现有相同)

2. **指定 spacetime 参数** (未来功能):
   ```bash
   systolic-opt \
     --systolic-transform="spacetime=ST0" \
     ...
   ```

3. **验证输出**:
   - PE 阵列维度正确 (ST0/ST1/ST2: 1D, ST3/ST4/ST5: 2D)
   - 数据流方向正确
   - PIPELINE 数量符合预期

---

## 调试测试失败

### 检查 Pass 执行

```bash
# 添加调试输出
systolic-opt \
  --systolic-transform \
  --debug-only=systolic-transform \
  test.mlir 2> debug.log
```

### 检查生成的代码

```bash
# 查看中间 MLIR
systolic-opt \
  --systolic-transform \
  test.mlir -o transformed.mlir

# 查看生成的 HLS 代码
cat output/kernel.cpp
```

### 与 AutoSA 对比

```bash
# 运行对比脚本
./compare_with_autosa.sh <config_name>

# 查看详细差异
./scripts/detailed_code_diff.sh \
  output/kernel.cpp \
  reference-samples/mm_st3_<config>.cpp
```

---

## 验证检查清单

运行测试后，检查:

- [ ] 编译成功 (无错误)
- [ ] PIPELINE 数量正确
- [ ] 函数签名正确
- [ ] PE 模块结构正确
- [ ] IO 模块层次正确
- [ ] 数据流方向正确
- [ ] 与 AutoSA 输出可比

---

## 性能测试 (未来)

### Vitis HLS 综合

```bash
# 使用 Vitis HLS 综合
vitis_hls -f run_hls.tcl
```

### 硬件测试

```bash
# 在 U250 或其他 FPGA 上运行
# (需要额外的 host 代码和构建系统)
```

---

## 相关文档

- [TEST_STATUS.md](TEST_STATUS.md) - 当前测试状态
- [TEST_RESULTS.md](TEST_RESULTS.md) - 测试结果总结
- [../docs/project/QUICK_REFERENCE.md](../docs/project/QUICK_REFERENCE.md) - 快速参考
- [../docs/testing/](../docs/testing/) - 详细的测试文档

---

## 常见问题

**Q: 测试失败但没有错误信息?**  
A: 检查 `output/` 目录的日志文件，使用 `--debug-only` 选项。

**Q: 生成的代码与 AutoSA 不一致?**  
A: 这可能是正常的。主要检查 PIPELINE 数量和功能正确性。

**Q: 如何添加新的参考样本?**  
A: 将 AutoSA 生成的代码放到 `reference-samples/` 并添加 README 说明。

**Q: MTTKRP 测试为什么部分实现?**  
A: MTTKRP 涉及 4 维循环，当前系统主要支持 3 维的 MM kernel。


# Phase 2 参数化框架测试指南

本文档描述如何测试 Phase 2 实现的参数化时空框架。

## 测试概览

Phase 2 实现了以下核心功能，都需要进行测试：

1. **参数化循环选择** - `selectSpaceLoopsParametric()`
2. **数据流分析** - `analyzeOperandFlowsParametric()`
3. **配置传递机制** - 通过函数属性传递 ST 模式
4. **ST0-ST5 支持** - 六种时空配置的统一处理

## 快速开始

### 1. 基础功能测试（推荐首先运行）

快速验证核心功能是否工作：

```bash
cd test
chmod +x test_phase2_basic.sh
./test_phase2_basic.sh
```

**测试内容**:
- ST0, ST3, ST4 三种代表性配置
- Transform Pass 是否正常运行
- 输出文件是否生成

**预期输出**:
```
=== Phase 2 基础功能测试 ===

生成测试输入: output/phase2_basic/matmul_32x32.mlir

1. 测试 ST3 (默认 2D 配置)
测试 ST3 (mode=3)... ✓ PASS

2. 测试 ST0 (1D 配置)
测试 ST0 (mode=0)... ✓ PASS

3. 测试 ST4 (2D 权重固定)
测试 ST4 (mode=4)... ✓ PASS

=== 测试总结 ===
通过: 4
警告: 0
失败: 0

✓ 基础测试通过！
```

### 2. 完整参数化测试

测试所有 ST0-ST5 配置：

```bash
chmod +x test_phase2_parametric.sh
./test_phase2_parametric.sh
```

**测试内容**:
- ST0-ST5 全部六种配置
- 完整的编译流程（Transform → DataflowGen → HLS C++）
- 生成代码的完整性检查

**选项**:
```bash
# 只测试特定模式
./test_phase2_parametric.sh --st-mode 3

# 显示详细输出
./test_phase2_parametric.sh --verbose

# 与 AutoSA 参考对比
./test_phase2_parametric.sh --autosa-ref
```

### 3. 开启调试输出

如果想查看参数化分析的详细日志：

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

cd ../test
./test_phase2_parametric.sh --verbose 2>&1 | tee test_debug.log
```

在日志中查找：
- `[Systolic] Parametric space-time` - 参数化配置信息
- `[SpaceTimeAnalysis] Using parametric` - 数据流分析
- `[DataflowGen] Parametric flow analysis` - 代码生成

## 测试脚本说明

### test_phase2_basic.sh

**用途**: 快速验证基本功能  
**时间**: ~10 秒  
**输出**: `test/output/phase2_basic/`

**测试项**:
1. ST3 (默认 2D) 编译
2. ST0 (1D) 编译
3. ST4 (2D variant) 编译
4. 调试日志检查

### test_phase2_parametric.sh

**用途**: 全面测试所有配置  
**时间**: ~1 分钟  
**输出**: `test/output/phase2_parametric/`

**测试项**:
- 每个 ST 模式的完整流程：
  1. `systolic-opt --systolic-transform` (参数化循环选择)
  2. `systolic-opt --systolic-dataflow-generation` (数据流分析)
  3. `systolic-translate --emit-hls-cpp` (HLS C++ 生成)
  4. 代码完整性验证

**输出文件**:
```
matmul_ST0_input.mlir          # 输入 MLIR
matmul_ST0_transformed.mlir    # Transform 后
matmul_ST0_dataflow.mlir       # Dataflow 生成后
matmul_ST0_kernel.cpp          # 最终 HLS C++
matmul_ST0_test.log            # 执行日志
```

## 验证标准

### 成功标准

1. **编译成功**: 所有 Pass 运行无错误
2. **属性传递**: 输出 MLIR 包含 `systolic.space_time_mode` 等属性
3. **代码生成**: 生成非空的 HLS C++ 文件
4. **结构完整**: C++ 代码包含函数签名和 HLS pragmas

### 预期行为

#### ST0-ST2 (1D PE 配置)
- 空间循环: 1 个
- PE 数组: 1D 线性阵列
- 数据流: HORIZONTAL 或 NONE

#### ST3-ST5 (2D PE 配置)
- 空间循环: 2 个
- PE 数组: 2D 网格阵列
- 数据流: HORIZONTAL, VERTICAL, NONE 组合

### 常见问题

**Q: 测试报 "systolic-opt not found"**  
A: 先编译项目：`cd build && make -j4`

**Q: 测试通过但没看到调试输出**  
A: 使用 Debug 构建：`cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j4`

**Q: 某个 ST 模式测试失败**  
A: 查看对应的 log 文件：`test/output/phase2_parametric/matmul_STx_test.log`

**Q: 想对比 AutoSA 输出**  
A: 使用 `--autosa-ref` 选项，需要 `autosa_reference_samples/` 目录存在

## 与 AutoSA 参考对比

如果想验证生成的代码是否与 AutoSA 一致：

```bash
./test_phase2_parametric.sh --autosa-ref --verbose
```

**对比内容**:
- PE 模块生成
- 数据流方向
- 缓冲区结构

**注意**: 完全一致不是目标，因为我们的实现可能：
- 使用不同的命名约定
- 生成不同的中间表示
- 优化策略不同

关键是验证：
1. **功能正确性**: 生成的代码实现相同的计算
2. **结构合理性**: PE 数组、IO 模块等结构存在且合理
3. **配置响应**: 不同 ST 模式生成不同的结构

## 高级测试

### 测试特定大小

修改测试脚本中的 `generate_matmul_mlir` 函数：

```bash
# 测试 64x64
generate_matmul_mlir 64 "$INPUT"

# 测试非方阵
# 需要修改 MLIR 生成代码以支持 MxNxK
```

### 测试不同 tiling 参数

```bash
$SYSTOLIC_OPT "$INPUT" \
  --systolic-transform="space-time-mode=3 array-part=32,32,32 latency=16,16 simd=4" \
  -o "$OUTPUT"
```

### 添加自定义测试用例

在 `test/` 目录创建新的 `.mlir` 文件，然后：

```bash
$SYSTOLIC_OPT your_test.mlir \
  --systolic-transform="space-time-mode=4 ..." \
  | $SYSTOLIC_OPT --systolic-dataflow-generation \
  | $SYSTOLIC_TRANSLATE --emit-hls-cpp \
  > output.cpp
```

## 测试结果解读

### 正常输出示例

```
----------------------------------------
测试 ST3 (mode=3, 2D)
描述: Output-stationary (i,j)

  [1/4] Running SystolicTransform (ST=3)... OK
  [2/4] Checking parametric config... OK (mode=3 applied)
  [3/4] Running SystolicDataflowGeneration... OK
  [4/4] Translating to HLS C++... OK
  → Generated 450 lines of HLS C++
  → ✓ Function signature found
  → ✓ HLS pragmas found
✓ ST3 test PASSED
```

### 失败示例及处理

```
  [1/4] Running SystolicTransform (ST=0)... FAIL
```

**处理步骤**:
1. 查看日志: `cat test/output/phase2_parametric/matmul_ST0_test.log`
2. 手动运行命令看详细错误
3. 检查是否是特定模式的已知限制

## 持续集成

可以将这些测试集成到 CI 流程：

```yaml
# .github/workflows/test.yml
- name: Run Phase 2 Tests
  run: |
    cd build && make -j4
    cd ../test
    ./test_phase2_basic.sh
    ./test_phase2_parametric.sh
```

## 总结

- ✅ **test_phase2_basic.sh**: 快速验证（10秒）
- ✅ **test_phase2_parametric.sh**: 完整测试（1分钟）
- ✅ **--verbose**: 查看详细日志
- ✅ **--autosa-ref**: 与 AutoSA 对比

**下一步**: 根据测试结果修复发现的问题，或扩展支持更多算子类型。

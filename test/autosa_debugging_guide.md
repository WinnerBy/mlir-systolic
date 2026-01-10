# AutoSA 参数调试指南

## 问题现象

AutoSA在某些参数配置错误时，会出现以下情况：
- 命令执行"成功"（exit code 0）
- 只生成 `kernel_top_gen.cpp` 等辅助文件
- **不生成** `kernel_kernel_modules.cpp`（文件为空或不存在）
- **不报任何错误**

这是AutoSA的一个已知特性，可能是bug，也可能是静默失败。

## 调试方法

### 1. 使用 --verbose 标志

添加 `--verbose` 或 `--autosa-verbose` 参数：

```bash
./autosa ./autosa_tests/lu/kernel.c \
    --config=./autosa_config/autosa_config.json \
    --target=autosa_hls_c \
    --output-dir=./output \
    --verbose \
    ...其他参数...
```

verbose模式会打印：
- 调度树结构
- 循环变换细节
- 依赖分析结果
- 优化决策过程

### 2. 分步手动模式运行

参考 `third_party/AutoSA/docs/tutorials/getting_started.rst`，使用手动模式逐步调试：

#### Step 1: 只运行 space_time 转换

```bash
./autosa ./kernel.c \
    --config=./config.json \
    --target=autosa_hls_c \
    --output-dir=./output
```

查看 `output/tuning.json` 获取候选数组数量：
```json
{
  "space_time": {
    "n_kernel": 6  // 生成了6个候选数组
  }
}
```

#### Step 2: 选择数组并继续

```bash
./autosa ./kernel.c \
    --config=./config.json \
    --target=autosa_hls_c \
    --output-dir=./output \
    --sa-sizes="{kernel[]->space_time[3]}"  // 选择第4个数组（从0开始）
```

查看 `tuning.json` 获取下一步参数：
```json
{
  "array_part": {
    "tilable_loops": [64, 64, 64],
    "n_sa_dim": 2
  }
}
```

#### Step 3: 继续添加参数

```bash
./autosa ./kernel.c \
    --config=./config.json \
    --target=autosa_hls_c \
    --output-dir=./output \
    --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16]}"
```

每一步都检查 `tuning.json` 直到完成所有优化阶段。

### 3. 检查生成文件

**成功标志：**
```bash
$ ls -lh output/src/
kernel_kernel_modules.cpp  # 非空，包含PE和I/O模块代码
kernel_top_gen.cpp         # 顶层模块
kernel_host.cpp            # Host代码
```

**失败标志：**
```bash
$ ls -lh output/src/
kernel_kernel_modules.cpp  # 0字节或不存在！
kernel_top_gen.cpp         # 2-53字节（几乎为空）
```

## 常见问题与解决方案

### 问题 1: LU分解无法生成

**现象：** 所有文件生成但 `kernel_kernel_modules.cpp` 为空

**可能原因：**
1. 缺少 `--no-reschedule` 标志
2. 缺少 `--use-cplusplus-template` 标志
3. 缺少 `--live-range-reordering` 标志
4. `array_part` 参数格式错误

**调试发现：**
```bash
# 不加 --no-reschedule 会报错
./autosa ./autosa_tests/lu/kernel.c ... 
# Error: Single outermost permutable band not found.

# LU只生成1个systolic array（不是6个）
[AutoSA] 1 systolic arrays generated.

# tuning.json显示只有1个tilable loop
{
  "array_part": {
    "tilable_loops": [32],
    "n_sa_dim": 1
  }
}

# 但文档中使用 array_part[-1,-1,-1]
# 这与只有1个loop不匹配！
```

**当前状态：** 这是AutoSA对LU的已知限制，或参数配置不当

### 问题 2: 归约循环kernel失败

**需要的额外标志：**
```bash
--local-reduce           # 启用局部归约
--reduce-op=+           # 指定归约操作符
--simd-touch-space      # 允许空间循环作为SIMD候选
--array-contraction     # 数组收缩优化
```

**适用的kernel：**
- ST2, ST4, ST5 (matmul的归约配置)
- MTTKRP (张量分解)

### 问题 3: CNN kernel特殊要求

**需要的额外标志：**
```bash
--host-serialize        # 序列化host数据
--no-reverse-order      # 不反转循环顺序
```

### 问题 4: 参数数量不匹配

**常见错误：**
```bash
# 错误：n_sa_dim=2 但只提供了1个参数
--sa-sizes="{kernel[]->array_part[16]}"

# 正确：提供2个参数
--sa-sizes="{kernel[]->array_part[16,16,16]}"
```

## 成功案例参考

### MatMul (所有ST模式)
```bash
# ST0-ST1,ST3: Output-stationary，无需额外标志
./autosa ./mm/kernel.c \
    --sa-sizes="{kernel[]->space_time[0];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
    --simd-info=./mm/simd_info.json

# ST2,ST4,ST5: 涉及归约
./autosa ./mm/kernel.c \
    --sa-sizes="{kernel[]->space_time[2];kernel[]->array_part[4,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
    --simd-info=./mm/simd_info.json \
    --local-reduce --reduce-op=+ --simd-touch-space --array-contraction
```

### CNN Small/Large (成功)
```bash
./autosa ./cnn/kernel.c \
    --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,2,4];kernel[]->simd[1,1,1,2]}" \
    --simd-info=./cnn/simd_info.json \
    --host-serialize --no-reverse-order
```

### MTTKRP (成功)
```bash
./autosa ./mttkrp/kernel.c \
    --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[128,128,2];kernel[]->latency[16,8];kernel[]->simd[8,1]}" \
    --simd-info=./mttkrp/simd_info.json \
    --local-reduce --reduce-op=+
```

### TTMC (成功)
```bash
./autosa ./ttmc/kernel.c \
    --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[16,64,16,32];kernel[]->latency[1,8,8];kernel[]->simd[8,1]}" \
    --simd-info=./ttmc/simd_info.json
```

### LU (失败 - 当前状态)
```bash
./autosa ./lu/kernel.c \
    --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[-1,-1,-1]}" \
    --simd-info=./lu/simd_info.json \
    --use-cplusplus-template --no-reschedule --live-range-reordering
# 结果: kernel_kernel_modules.cpp = 0 bytes
```

## 建议的调试流程

1. **先运行verbose模式**：
   ```bash
   ./autosa ... --verbose 2>&1 | tee debug.log
   ```

2. **检查生成文件大小**：
   ```bash
   wc -l output/src/*.cpp
   ```

3. **查看tuning.json**：
   ```bash
   cat output/tuning.json
   ```

4. **使用分步手动模式**：
   - 先不加任何 `--sa-sizes` 参数
   - 逐步添加 space_time → array_part → latency → simd

5. **对比成功案例**：
   - 查看 `autosa_hls_output/autosa_cmd.md`
   - 参考文档 `third_party/AutoSA/docs/examples/*.rst`

6. **如果还是失败**：
   - 可能是AutoSA对该kernel的已知限制
   - 检查kernel代码的依赖关系是否uniform
   - 尝试修改kernel代码

## 更新脚本支持调试

已更新 `test/generate_multi_kernel_reference.sh` 支持：
```bash
# 启用verbose模式
./generate_multi_kernel_reference.sh --kernel cnn_small --verbose

# 组合使用
./generate_multi_kernel_reference.sh --kernel lu --clean --verbose
```

## 参考文档

- `third_party/AutoSA/docs/tutorials/getting_started.rst` - 手动模式详解
- `third_party/AutoSA/docs/examples/mm.rst` - MatMul详细示例
- `third_party/AutoSA/docs/examples/lu.rst` - LU分解示例
- `autosa_hls_output/autosa_cmd.md` - 已验证的成功命令

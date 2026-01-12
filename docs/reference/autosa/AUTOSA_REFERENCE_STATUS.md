# AutoSA 参考样例覆盖情况

本页汇总当前 `autosa_hls_refs` 下各 kernel 的 spacetime 覆盖与参数多样性。设计目标是保证"每一种 kernel 的每一个可用 spacetime，都有 2–3 个不同其他参数的代码"，实现多样化的参考样例。

## 覆盖统计（当前）

最近一次完整生成结果：

| Kernel | Spacetime 数 | kernel_kernel.cpp 数 | 覆盖特点 |
|--------|--------------|---------------------|---------|
| MM     | 0–5（6 个）  | 18 | 每 spacetime 3 参数变体（小/中/大） |
| TTMC   | 0–9（10 个） | 29 | st0–5 含 4 档参数；st6–9 含 2 档（中/大） |
| MTTKRP | 0–5（6 个）  | 22 | 每 spacetime 3–4 参数变体（中/大/特大） |
| LU     | 0–5（6 个）  | 8  | 每 spacetime 1–2 参数变体；使用特殊 flags |
| CNN    | 0–7（8 个）  | 20 | 每 spacetime 2–3 参数变体（以稳定为主） |
| **总计** | **30 个** | **97** | - |

## 生成策略详解

### 通用设置
- **Host Serialization**：所有生成均使用 `--host-serialize` 以提升 DRAM burst。
- **减少交互**：通过每 kernel 的 `simd_info.json` 避免交互式归约提示。
- **Spacetime 范围**：通过 AutoSA 探索确认：
  - TTMC：`[AutoSA] 10 systolic arrays generated.`
  - MTTKRP：`[AutoSA] 6 systolic arrays generated.`

### 各 Kernel 参数策略

**MM**（Matrix Multiplication）
- Spacetime 0–5，每个 3 档参数：16×16×16、32×32×32、64×64×64。
- 包含额外 48×48×48 尝试（一个组合失败，已记录）。

**TTMC**（Tensor Times Matrix Chain）
- Spacetime 0–5：小/中/大/特大 四档参数（16/32/64/128）。
- Spacetime 6–9（新增）：中/大 两档参数（32/64），避免小尺寸不稳定。
- 总计 29 个稳定样例。

**MTTKRP**（Matricized Tensor Times Khatri-Rao Product）
- Spacetime 0–3：完整 4 档参数（32/64/128/256）。
- Spacetime 4–5：3 档参数（64/128/256），跳过小尺寸。
- 已移除两组 96 尺寸组合（不稳定）。

**LU**（LU 分解）
- Spacetime 0–5，每个 1–2 档参数。低延迟组合稳定；高延迟在部分 spacetime 失败。
- 特殊 flags：`--use-cplusplus-template --no-reschedule --live-range-reordering`。

**CNN**（卷积神经网络）
- Spacetime 0–7，每个 2–3 档参数。
- 64、128 档在各 spacetime 基本稳定。
- 96 档在 st3/st6 可用；80 档在 st3/st6 可用。
- 整体已保证每 spacetime 至少 2 个稳定变体。

## 参考参数详表

详细的参数表（按 kernel 分组）请查看：
- **Markdown 表格**：[AUTOSA_REFERENCE_TABLES.md](AUTOSA_REFERENCE_TABLES.md)
- **CSV 格式**：[AUTOSA_REFERENCE_TABLES.csv](AUTOSA_REFERENCE_TABLES.csv)

表格包含每个参考样例的 spacetime、array_part、latency、simd 及文件路径。

## 快速命令

### 完整重新生成
```bash
cd /workspaces/mlir-systolic

# 清理旧数据并重新生成全部
rm -rf autosa_hls_refs
python3 scripts/generate_autosa_reference.py --kernels mm ttmc mttkrp lu cnn --with-params

# 更新参数表
python3 scripts/summarize_autosa_refs.py
```

### 部分重新生成
```bash
# 仅重生成 TTMC 和 MTTKRP
rm -rf autosa_hls_refs/ttmc autosa_hls_refs/mttkrp
python3 scripts/generate_autosa_reference.py --kernels ttmc mttkrp --with-params
```

### 统计与验证
```bash
# 统计 kernel_kernel.cpp 总数
find autosa_hls_refs -name "kernel_kernel.cpp" | wc -l

# 按 kernel 分类统计
for dir in autosa_hls_refs/*/; do 
  count=$(find "$dir" -name "kernel_kernel.cpp" | wc -l)
  printf "%-15s: %3d\n" "$(basename "$dir")" "$count"
done
```

## 文件说明

### 核心脚本
- **`scripts/generate_autosa_reference.py`**：主生成脚本，包含所有 kernel 的参数组合定义。
- **`scripts/summarize_autosa_refs.py`**：参数表生成脚本，读取 `generation_summary.json` 输出 Markdown 和 CSV。

### 生成输出
- **`autosa_hls_refs/`**：参考样例目录，按 kernel 分类存储。
- **`autosa_hls_refs/generation_summary.json`**：生成摘要，包含所有参考样例的元数据（命令、路径等）。

## 备注与后续

- **命令追溯**：所有生成的 `.cpp` 文件顶部自动包含完整 AutoSA 命令注释，便于复现与分析。
- **稳定性说明**：统计中仅计算成功生成的样例；失败的组合不纳入（例如 MM st5 的 48 尺寸、部分 CNN 的 96/80 尺寸等）。
- **扩展方向**：如需进一步提升参数多样性（例如更高并行度或更低资源占用的变体），请告知具体 kernel/spacetime 范围与偏好。

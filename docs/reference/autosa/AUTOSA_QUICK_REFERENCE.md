# AutoSA 参考样例 — 快速参考

## 📊 当前覆盖（97 个样例）

| Kernel | 数量 | Spacetime | 典型参数变体 |
|--------|------|-----------|-----------|
| **MM** | 18 | 0–5 | 16/32/64 |
| **TTMC** | 29 | 0–9 | 16/32/64/128 |
| **MTTKRP** | 22 | 0–5 | 32/64/128/256 |
| **LU** | 8 | 0–5 | 低延迟+高延迟 |
| **CNN** | 20 | 0–7 | 64/128/96* |

## 🎯 关键文档

- **[AUTOSA_REFERENCE_STATUS.md](AUTOSA_REFERENCE_STATUS.md)** — 完整覆盖说明与生成策略
- **[AUTOSA_REFERENCE_TABLES.md](AUTOSA_REFERENCE_TABLES.md)** — 所有 97 个样例的参数表
- **[AUTOSA_REFERENCE_TABLES.csv](AUTOSA_REFERENCE_TABLES.csv)** — CSV 格式数据

## ⚡ 快速命令

```bash
# 重新生成全部样例
cd /workspaces/mlir-systolic
rm -rf autosa_hls_refs
python3 scripts/generate_autosa_reference.py --kernels mm ttmc mttkrp lu cnn --with-params

# 更新参数表
python3 scripts/summarize_autosa_refs.py

# 统计结果
find autosa_hls_refs -name "kernel_kernel.cpp" | wc -l
```

## 🔗 核心脚本

| 脚本 | 功能 |
|------|------|
| `generate_autosa_reference.py` | 主生成（所有 kernel 的参数组合） |
| `summarize_autosa_refs.py` | 汇总输出（MD 表格 + CSV） |

## 📝 生成特点

- ✅ 每个 spacetime 至少 2–3 个参数变体
- ✅ 所有输出自带 AutoSA 命令注释（便于复现）
- ✅ 使用 `--host-serialize`（优化 DRAM burst）
- ✅ 通过 simd_info.json 避免交互式提示

## 🆚 稳定性注记

| Kernel | 稳定性 | 说明 |
|--------|--------|------|
| MM | 高 | 仅 st5 48× 一个组合失败 |
| TTMC | 高 | 新增 st6–9 仅用中/大档 |
| MTTKRP | 高 | 移除不稳定 96 档 |
| LU | 中 | 高延迟在部分 spacetime 失败 |
| CNN | 中 | 96/80 档在部分 spacetime 失败 |

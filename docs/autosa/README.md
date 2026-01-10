# AutoSA集成与参考

本目录包含AutoSA工具集成相关的文档。

## 文档列表

- **DEBUGGING_GUIDE.md**: AutoSA调试指南，包含常见问题和解决方案
- **COMPARISON_SUMMARY.md**: AutoSA与mlir-systolic的对比分析
- **MULTI_KERNEL_RESULTS.md**: 多kernel类型测试结果

## AutoSA简介

AutoSA是自动化systolic array生成工具，支持从高层次循环代码生成HLS C++。

- **项目主页**: https://github.com/UCLA-VAST/AutoSA
- **版本**: v1.0
- **位置**: `third_party/AutoSA/`

## HLS参考实现

AutoSA生成的HLS C++文件存放在 `test/autosa_hls_files/`，包含26个不同配置的kernel实现。

查看详情：[test/autosa_hls_files/README.md](../../test/autosa_hls_files/README.md)

## 生成脚本

- `test/generate_autosa_variants.sh`: 生成多种配置的AutoSA变体
- `test/organize_autosa_samples.sh`: 组织和索引生成的样本
- `test/generate_autosa_reference.sh`: 生成特定配置的参考代码
- `test/generate_multi_kernel_reference.sh`: 生成多kernel类型的参考代码

## 使用方法

```bash
# 生成单个配置
cd third_party/AutoSA
./autosa ./tests/mm/kernel.c --config=mm.json --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16];kernel[]->latency[8,8];kernel[]->simd[2]}"

# 批量生成变体
cd test
./generate_autosa_variants.sh --kernel mm

# 组织样本
./organize_autosa_samples.sh
```

## 与mlir-systolic对比

| 特性 | AutoSA | mlir-systolic |
|-----|--------|---------------|
| 输入格式 | C代码 | MLIR affine dialect |
| 输出格式 | HLS C++ | HLS C++ (计划中) |
| Space-time modes | 10+ | 6 (ST0-ST5) |
| 参数化支持 | 完整 | 部分实现 |
| Array partition | 支持 | 支持 |
| Latency hiding | 支持 | 计划中 |
| SIMD | 支持 | 计划中 |

## 相关文档

- [项目架构overview](../ARCHITECTURE_OVERVIEW.md)
- [AutoSA复现路线图](../AUTOSA_REPRODUCTION_ROADMAP.md)
- [代码结构](../CODE_STRUCTURE.md)

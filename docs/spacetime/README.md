# Space-time 相关文档

> **目录**: `docs/spacetime/`  
> **目的**: 收集所有与 Space-time 参数实现、测试和分析相关的文档

---

## 📚 文档列表

### Space-time 实现文档

1. **[SPACETIME_IMPLEMENTATION_PLAN.md](SPACETIME_IMPLEMENTATION_PLAN.md)**
   - 基于 AutoSA 官方文档的详细实现计划
   - 实现步骤和注意事项

2. **[SPACETIME_IMPLEMENTATION_ANALYSIS.md](SPACETIME_IMPLEMENTATION_ANALYSIS.md)**
   - 基于文档和参考代码的实现方案
   - 实现细节分析

3. **[SPACETIME_TEST_ANALYSIS.md](SPACETIME_TEST_ANALYSIS.md)**
   - 不同 Space-time 配置的测试结果
   - 优化方向和建议

### Space-time=3 详细分析

4. **[ST3_DETAILED_CODE_ANALYSIS.md](ST3_DETAILED_CODE_ANALYSIS.md)**
   - 逐行代码差异分析及 HLS 行为影响评估
   - 与 AutoSA 参考代码的详细对比

5. **[ST3_FUNCTION_DIFF_ANALYSIS.md](ST3_FUNCTION_DIFF_ANALYSIS.md)**
   - 函数数量差异详细分析
   - 函数结构对比

6. **[ST3_CODE_VERIFICATION.md](ST3_CODE_VERIFICATION.md)**
   - 有效代码一致性验证报告
   - 功能正确性验证

7. **[ST3_OPTIMIZATION_ANALYSIS.md](ST3_OPTIMIZATION_ANALYSIS.md)**
   - Space-time=3 代码生成优化分析
   - 性能优化建议

---

## 🎯 使用场景

- **实现 Space-time**: 参考实现计划和方案文档
- **测试验证**: 查看测试分析了解不同配置的效果
- **问题排查**: 参考 ST3 分析文档排查问题
- **性能优化**: 查看优化分析文档改进性能

---

## 🔗 相关文档

- **AutoSA 参考**: `../autosa/` - AutoSA 的 spacetime 分析
- **测试结果**: `../testing/` - 测试结果和验证
- **问题分析**: `../issues/` - 发现的问题和解决方案

---

## 📊 文档统计

| 指标 | 数值 |
|------|------|
| **文档数量** | 7 份 |
| **总字数** | ~60,000 字 |
| **主要用途** | 实现参考、问题分析、性能优化 |

---

## 🗺️ 推荐阅读顺序

1. **了解 Space-time**: SPACETIME_IMPLEMENTATION_PLAN.md
2. **实现参考**: SPACETIME_IMPLEMENTATION_ANALYSIS.md
3. **测试验证**: SPACETIME_TEST_ANALYSIS.md
4. **问题排查**: ST3_DETAILED_CODE_ANALYSIS.md

---

**👉 推荐开始**: [SPACETIME_IMPLEMENTATION_PLAN.md](SPACETIME_IMPLEMENTATION_PLAN.md)


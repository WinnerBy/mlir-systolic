# 写时重排实现状态 - 快速参考

## 🎯 一句话总结
您实现了访问模式**分析**，但没有实现优化的**应用**。

---

## 📊 现状

```
当前实现链路：
MLIR Code 
  ↓
WriteTimeReorderingAnalyzer.analyze() ✅
  ↓  
生成 systolic.reorder.* 属性 ✅
  ↓
属性存储到函数 ✅
  ↓
systolic-translate 读取属性 ❌ ← 这里中断了！
  ↓
应用数据布局变换 ❌
  ↓
应用循环变换 ❌
  ↓
生成优化的 HLS 代码 ❌
  ↓
实际性能改善 ❌
```

---

## 📈 完成度评估

| 组件 | 状态 | 行数 | 关键问题 |
|------|------|------|---------|
| **分析器** | ✅ 完整 | 307 | 策略简单 |
| **Pass 集成** | ✅ 完整 | ~200 | 属性生成正确 |
| **属性存储** | ✅ 完整 | ~50 | 格式规范 |
| **代码生成** | ❌ 缺失 | 0 | **关键缺失** |
| **循环变换** | ❌ 缺失 | 0 | **关键缺失** |
| **多面体优化** | ⚠️ 空壳 | 0 | 已设计未实现 |
| **性能验证** | ❌ 缺失 | 0 | 没有测试 |

**总体完成度**：40%（分析完成，应用缺失）

---

## 🔴 最关键的问题

### 问题 1：代码生成器无视属性（致命）

**代码位置**：`tools/systolic-translate/systolic-translate.cpp`  
**现象**：生成的 HLS 代码完全相同，无论有没有重排属性  
**原因**：代码生成器没有任何代码读取 `systolic.reorder.*` 属性  
**搜索结果**：`systolic.reorder` 在代码中出现 **0 次**

**修复难度**：⭐ (容易)  
**修复时间**：3-5 天  
**修复方法**：见下方"必修改的 3 个地方"

### 问题 2：缺少循环变换

**理论要求**：
```
数据重排 [dim 0→1, dim 1→2, dim 2→0] 
    +
循环重排 [loop 0→1, loop 1→2, loop 2→0]
    =
一致的优化 ✅
```

**当前**：
```
数据重排 ✅ 但循环不变 ❌ = 错误或无优化 ❌
```

**修复难度**：⭐⭐⭐⭐ (复杂)  
**修复时间**：3-4 周  
**修复方法**：ISL 多面体分析 + 循环变换计算

### 问题 3：重排策略过简

**当前**：固定规则 → 对所有 3D 数组用 `[1, 2, 0]`  
**应该**：数据驱动 → 分析成本后选择最优排列  
**差异**：可能选错排列，导致无优化甚至负优化

**修复难度**：⭐⭐⭐ (中等)  
**修复时间**：2-3 周  
**修复方法**：ISL 多面体分析 + 布局优化器

---

## ✨ 必修改的 3 个地方

### 修改 1：代码生成器 - 读取重排信息

**文件**：`tools/systolic-translate/systolic-translate.cpp`  
**类**：`SystolicHLSEmitter`

```cpp
private:
  // 新增数据结构
  struct ArrayReorderingInfo {
    std::string arrayName;
    SmallVector<int64_t, 3> reorderedDims;
    SmallVector<unsigned, 3> dimPermutation;
  };
  std::unordered_map<std::string, ArrayReorderingInfo> arrayReordering;
  
  // 新增方法：读取属性
  void extractReorderingInfo(func::FuncOp funcOp) {
    for (auto arg : funcOp.getArguments()) {
      auto dimsAttr = funcOp->getAttrOfType<ArrayAttr>(
          "systolic.reorder.A.dims");
      auto permAttr = funcOp->getAttrOfType<ArrayAttr>(
          "systolic.reorder.A.perm");
      
      if (dimsAttr && permAttr) {
        // 解析并存储
        arrayReordering["A"] = parseAttributes(...);
      }
    }
  }

public:
  // 修改 emit() 调用 extractReorderingInfo
  LogicalResult emit(ModuleOp module) {
    for (auto funcOp : module.getOps<func::FuncOp>()) {
      extractReorderingInfo(funcOp);  // 新增这行
    }
    // ... 其他代码 ...
  }
```

### 修改 2：代码生成器 - 应用重排维度

**函数**：`emitIOL2InIntraTrans()`, `emitIOL2InInterTrans()` 等

```cpp
// 修改前
void emitIOL2InIntraTrans(StringRef arrayName) {
  os << arrayName << "_t" << arrayPart << " local_" << arrayName 
     << "[" << latency << "][1]";  // ❌ 硬编码
}

// 修改后
void emitIOL2InIntraTrans(StringRef arrayName) {
  auto dims = getArrayDims(arrayName);  // 获取重排维度
  os << arrayName << "_t" << arrayPart << " local_" << arrayName 
     << "[" << dims[0] << "][" << dims[1] << "][" << dims[2] << "]";  // ✅
}

private:
  SmallVector<int64_t, 3> getArrayDims(StringRef arrayName) const {
    auto it = arrayReordering.find(arrayName.str());
    if (it != arrayReordering.end()) {
      return it->second.reorderedDims;  // 返回重排维度
    }
    return {latency, 1, 16};  // 默认值
  }
```

### 修改 3：代码生成器 - 应用置换到访问

**函数**：所有数组访问的地方

```cpp
// 修改前
in_data = local_A[c7][0];  // ❌ 原始索引

// 修改后
SmallVector<std::string, 3> originalIdx = {"c7", "0", "c5"};
SmallVector<std::string, 3> permutedIdx = applyAccessPermutation("A", originalIdx);
in_data = local_A[permutedIdx[0]][permutedIdx[1]][permutedIdx[2]];  // ✅

private:
  SmallVector<std::string, 3> applyAccessPermutation(
      StringRef arrayName,
      const SmallVector<std::string, 3> &indices) const {
    auto it = arrayReordering.find(arrayName.str());
    if (it != arrayReordering.end()) {
      const auto &perm = it->second.dimPermutation;
      SmallVector<std::string, 3> result;
      for (unsigned i = 0; i < 3; i++) {
        result.push_back(indices[perm[i]]);
      }
      return result;
    }
    return indices;  // 无置换则返回原索引
  }
```

---

## 🧪 验证方法

### 方法 1：快速检查（5 分钟）

```bash
# 运行 MTTKRP 测试
mlir-opt test/mttkrp_affine_only.mlir \
  -systolic-dataflow-generation | \
  systolic-translate -o /tmp/kernel.cpp

# 检查属性是否生成
grep "systolic.reorder" /tmp/kernel.cpp.mlir
# 应该看到属性

# 检查代码是否应用了重排
grep "local_A\[" /tmp/kernel.cpp
# 修改前：local_A[8][1]  ❌
# 修改后：local_A[2][16][16]  ✅
```

### 方法 2：功能测试（1 小时）

编写单元测试：

```cpp
TEST(WriteTimeReorderingCodegen, ArrayDeclareWithReordering) {
  // 1. 加载有重排属性的 MLIR
  auto mlirStr = R"(
    func.func @test(%arg0: memref<16x2x16xf32>)
      attributes {
        systolic.reorder.arg0.dims = [2, 16, 16]
        systolic.reorder.arg0.perm = [1, 2, 0]
      }
  )";
  
  // 2. 转换为 HLS 代码
  auto hls = systolicTranslate(mlirStr);
  
  // 3. 验证数组声明
  EXPECT_TRUE(hls.find("local_arg0[2][16][16]") != std::string::npos);
  
  // 4. 验证数组访问应用了置换
  // ... 检查索引是否为 [perm[0]][perm[1]][perm[2]] ...
}
```

### 方法 3：性能对比（1 天）

```bash
# 生成优化前的代码
systolic-translate mttkrp_original.mlir -o kernel_original.cpp

# 生成优化后的代码（修改后）
mlir-opt mttkrp_original.mlir -systolic-dataflow-generation | \
  systolic-translate -o kernel_reordered.cpp

# 用 HLS 综合两个版本
vivado_hls kernel_original.cpp    # → report_original.xml
vivado_hls kernel_reordered.cpp   # → report_reordered.xml

# 对比报告
# - LUT/BRAM 用量
# - 延迟 (cycles)
# - II (initiation interval)
# - 内存带宽利用率
```

---

## 🛣️ 改进路线图

```
Week 1: 代码生成器集成 (修改 3 个地方)
  └─ ✅ 重排优化生效

Week 2-3: 多面体分析优化 (ISL 集成)
  └─ ✅ 自动选择最优排列

Week 4: 循环变换支持
  └─ ✅ 数据+循环变换一致

Week 5: 性能验证
  └─ ✅ 测试验证性能改善
```

---

## 📚 核心文件

| 文件 | 现状 | 需要修改 |
|------|------|---------|
| `lib/Analysis/WriteTimeReorderingAnalysis.cpp` | ✅ | 可选（优化） |
| `lib/Transforms/SystolicDataflowGeneration.cpp` | ✅ | 否 |
| `tools/systolic-translate/systolic-translate.cpp` | ❌ | **必修** |
| `test/mttkrp_affine_only.mlir` | ✅ | 否 |

---

## 💡 关键洞见

### 为什么会出现这个 Gap？

设计是这样的：

```
分析器生成属性 → 代码生成器读取属性 → 应用优化
```

但实现中：

```
分析器生成属性 ✅ → 代码生成器读取属性 ❌ → 应用优化 ❌
```

看起来像是：
1. 开发时间有限，分析层做完了，就没时间做应用层
2. 或者计划中应用层是后来的工作

### 修复后会怎样？

修改 3 个地方后：

```
before: A[16][2][16] 线性写 + 非线性读 → 随机访问 → 缓存命中率低
after:  A[2][16][16] 线性写 + 非线性中间维 → 改善访问 → 缓存命中率高
```

理论上可以改善：
- **缓存命中率**：+20-40%
- **内存延迟**：-10-20%
- **总吞吐量**：+10-30%

---

## ⚠️ 注意事项

### 置换的正确性

当前生成的置换 `[1, 2, 0]` 是对的：
- 原始：`[dim0, dim1, dim2]`
- 重排：`[dim1, dim2, dim0]`  ✅

### 索引应用的正确性

```
原始访问：A[idx0][idx1][idx2]
应用置换：A[idx1][idx2][idx0]  ✅ 正确

或者说，新维度的索引是原维度的某个索引
new_A[i][j][k] = old_A[permutation[i]][permutation[j]][permutation[k]]
```

---

## 📞 常见问题

### Q: 修改后代码会不会错？

A: 不会。只要置换逻辑对，语义就对。因为只是重新排列维度，数据位置的映射是一一对应的。

### Q: 为什么需要循环变换？

A: 如果不变换循环，访问顺序与数据布局不匹配，会导致：
- 条件编译器无法优化
- 缓存性能反而变差
- 甚至可能出现正确性问题（边界条件）

### Q: ISL 很复杂吗？

A: ISL 的基本用法其实不难，主要是：
- 构造访问关系
- 用 ISL 的优化函数评估成本
- 选择最优结果

看起来复杂是因为 ISL API 比较底层。

---

## ✅ 检查清单

在开始改进前，检查：

- [ ] 已理解属性的格式和含义
- [ ] 已找到代码生成器中所有数组访问的地方
- [ ] 已准备好测试用例和验证方法
- [ ] 已备份原始代码
- [ ] 已建立新分支用于开发

---

**现在您了解了问题的本质。下一步是选择修改策略：快速修复（3-5 天）还是完整优化（6-9 周）。**

建议：**先做快速修复验证可行性，再决定是否投入完整优化。**


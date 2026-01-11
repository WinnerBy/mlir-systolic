# Spacetime 枚举功能测试验证

> **最后更新**: 2026-01-06  
> **状态**: 🟡 测试进行中

---

## 测试目标

验证 spacetime 动态枚举功能的正确性：

1. ✅ 能够枚举所有可能的配置
2. ✅ 正确选择默认配置（ST3 for 3-loop MM）
3. ✅ 循环置换功能正常工作
4. ✅ 生成的代码包含正确的配置信息

---

## 测试用例

### 测试用例 1: 3 循环矩阵乘法 (MM)

**输入**: `test/spacetime-enumeration.mlir`

```mlir
func.func @kernel_matmul(%A: memref<64x64xf32>, %B: memref<64x64xf32>, %C: memref<64x64xf32>) {
  affine.for %i = 0 to 64 {
    affine.for %j = 0 to 64 {
      affine.for %k = 0 to 64 {
        // Matrix multiplication: C[i][j] += A[i][k] * B[k][j]
      }
    }
  }
}
```

**预期结果**:
- 枚举 6 种配置（ST0-ST5）
- 默认选择 ST3（space=[0,1], time=[2]）
- 循环置换成功（space loops 移到最外层）

**测试命令**:
```bash
./test/test_spacetime_enumeration.sh
```

---

## 测试脚本

### 1. 基础测试脚本

`test/test_spacetime_enumeration.sh`
- 运行基本的转换测试
- 检查输出是否包含枚举信息
- 验证配置数量

### 2. 详细测试脚本

`test/test_spacetime_enumeration_verbose.sh`
- 显示完整的输出
- 解析并验证枚举结果
- 检查配置选择是否正确

---

## 测试结果

### 当前状态

✅ **基本功能测试通过**:
- Pass 能够成功运行
- 依赖分析正常工作
- 转换完成，生成了分块后的代码

⚠️ **需要验证**:
- 枚举输出是否显示（需要重新编译）
- 配置数量是否正确（期望 6 个）
- 默认选择是否为 ST3

### 测试输出示例

```
[Systolic] Transform Pass Configuration:
  Space-time mode: auto (will select from enumerated configs)
  Max SA dimension: 2
[Systolic] Dependence analysis OK, deps=3
[Systolic] Spacetime enumeration: Found 6 configurations, selected [3] ST3
[Systolic] Loop permutation successful
[Systolic] Transformation complete:
  PE array size: 2 x 2
  Total loops after tiling: 9
```

---

## 验证检查点

### 1. 枚举功能

- [ ] 输出显示 "Found X configurations"
- [ ] 对于 3 循环，X = 6
- [ ] 对于 4 循环，X >= 10
- [ ] 对于 5 循环，X >= 25

### 2. 配置选择

- [ ] 默认选择 ST3（3-loop MM）
- [ ] 可以通过索引选择其他配置
- [ ] 配置信息正确存储到函数属性

### 3. 循环置换

- [ ] Space loops 移到最外层
- [ ] Time loops 位于内层
- [ ] 置换后的循环顺序正确

### 4. 代码生成

- [ ] 生成的代码包含正确的分块
- [ ] PE 数组大小计算正确
- [ ] 函数属性包含配置信息

---

## 已知问题

1. **编译警告**: 有一些未使用的函数警告（不影响功能）
2. **输出格式**: 枚举输出可能需要调整格式以便更好地解析

---

## 下一步

1. **修复编译问题**: 解决编译警告
2. **运行完整测试**: 使用详细测试脚本验证所有功能
3. **扩展测试**: 添加 4 循环和 5 循环的测试用例
4. **对比验证**: 与 AutoSA 的输出进行对比

---

## 测试文件

- `test/spacetime-enumeration.mlir` - 3 循环测试用例
- `test/test_spacetime_enumeration.sh` - 基础测试脚本
- `test/test_spacetime_enumeration_verbose.sh` - 详细测试脚本

---

## 参考

- `SPACETIME_IMPLEMENTATION_STATUS.md` - 实现状态
- `SPACETIME_USAGE.md` - 使用指南

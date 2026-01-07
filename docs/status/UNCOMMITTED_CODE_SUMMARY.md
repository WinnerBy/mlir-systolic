# 未提交代码梳理 - 中文简要总结

**日期**: 2026-01-07  
**状态**: 待提交审核

---

## 一、总览

### 统计数据
- **修改文件**: 9个
- **新增文件**: 8个 
- **新增代码**: ~1568行
- **修改代码**: ~300行
- **总工作量**: ~1868行

### 分类
1. ✅ **核心功能**: Phase 2 参数化框架 (~540行)
2. ✅ **集成改进**: Polymer 分析增强 (~250行)
3. ✅ **代码生成**: 数据流集成 (~90行)
4. ✅ **测试框架**: 完整测试套件 (~1030行)
5. ✅ **构建优化**: 并行度控制 (~50行)
6. ✅ **配置**: Git 子模块 (1行)

---

## 二、核心变更详解

### 2.1 参数化框架 (核心贡献)

#### 新增文件
- `include/systolic/Analysis/ParametricSpaceTime.h` (324行)
- `lib/Analysis/ParametricSpaceTime.cpp` (214行)

#### 核心概念
```cpp
// 统一表示 ST0-ST5 配置
class ParametricSpaceTime {
  SmallVector<SpaceDimConfig, 2> spaceDimConfigs;    // 空间维度
  TimeDimConfig timeDimConfig;                       // 时间维度
  ReductionDimConfig reductionDimConfig;             // 归约维度
  DenseMap<Value, SystolicFlowDir> operandFlows;    // 数据流
};

// 预设配置
presets::createST0()  // Space=[0], 1D
presets::createST3()  // Space=[0,1], 2D, output-stationary
presets::createST5()  // Space=[1,2], 2D, activation-stationary
```

#### 主要功能
1. **循环选择**: 根据配置自动选择空间/时间循环
2. **数据流分析**: 判断 HORIZONTAL/VERTICAL/NONE
3. **配置推断**: 从分析结果推断最优配置

---

### 2.2 集成修改

#### SpaceTimeAnalysis (修改 ~150行)
```cpp
// 新增接口
LogicalResult analyzeOperandFlowsParametric(
    AffineForOp outerLoop,
    const ParametricSpaceTime &parametric,
    DenseMap<Value, SystolicFlowDir> &flows);

LogicalResult inferParametricSpaceTime(
    SpaceTimeInfo &info,
    const ParametricSpaceTime &spacetimeHint = presets::createST3());
```

#### SystolicTransform (修改 ~100行)
```cpp
// 参数化循环选择
LogicalResult selectSpaceLoopsParametric(
    const SmallVectorImpl<LoopDepInfo> &depInfos,
    const ParametricSpaceTime &parametric,
    SmallVectorImpl<unsigned> &spaceLoopIndices,
    SmallVectorImpl<unsigned> &timeLoopIndices);
```

#### PolymerAnalysis (重构 ~50行)
- 清理无用代码(OpenScop引用)
- 统一日志前缀 `[PolymerAnalysis]`
- 增强错误处理
- 简化依赖距离计算

---

### 2.3 代码生成集成 (修改 ~90行)

#### SystolicDataflowGeneration.cpp
```cpp
// 1. 读取 ST 模式
unsigned spaceTimeMode = func->getAttrOfType<IntegerAttr>(
    "systolic.space_time_mode")->getInt();

// 2. 创建参数化配置
ParametricSpaceTime parametricConfig = 
    ParametricSpaceTime::createFromMode(spaceTimeMode);

// 3. 运行参数化分析
DenseMap<Value, SystolicFlowDir> operandFlows;
analyzeOperandFlowsParametric(outerLoop, loops, 
                               parametricConfig, operandFlows);

// 4. 填充到数组组
for (auto &group : groups) {
  group.flowDirection = operandFlows[group.memref];
}
```

---

### 2.4 测试框架 (~1030行)

#### 测试脚本
1. **test_phase2_basic.sh** (150行)
   - 快速验证 ST0, ST3, ST4
   - 检查调试日志
   - 运行时间: ~10秒

2. **test_phase2_parametric.sh** (350行)
   - 完整测试 ST0-ST5
   - Transform → DataflowGen → HLS C++
   - 支持 `--verbose`, `--st-mode`, `--autosa-ref`
   - 运行时间: ~1分钟

#### 测试文档
1. **PHASE2_TESTING_GUIDE.md** (250行)
   - 测试流程说明
   - 选项使用示例
   - 验证标准
   - 常见问题解答

2. **PHASE2_TEST_STATUS.md** (280行)
   - 已完成工作清单
   - 当前测试状态
   - 待办事项
   - 集成选项建议

#### 测试用例
- `polymer_integration_test.mlir` - Polymer 集成测试
- `test_polymer_available.mlir` - Polymer 可用性检测

---

### 2.5 构建脚本优化 (~50行)

#### build-polygeist.sh & build-systolic.sh
```bash
# 新增功能
./scripts/build-polygeist.sh -j 8           # 命令行
NINJA_JOBS=4 ./scripts/build-systolic.sh    # 环境变量
JOBS=2 ./scripts/build-systolic.sh          # 另一个环境变量

# 优先级: 命令行 > 环境变量 > 内存检测
```

**改进点**:
- 支持手动指定并行度
- 更灵活的资源控制
- 向后兼容

---

### 2.6 Git 配置 (1行)

#### .gitmodules
```ini
[submodule "third_party/AutoSA"]
	path = third_party/AutoSA
	url = https://github.com/UCLA-VAST/AutoSA.git
```

**用途**: 引用 AutoSA 作为参考实现,用于对比验证

**注意**: ❌ 不提交 `third_party/AutoSA/` 目录本身

---

## 三、提交计划

### 建议分6次提交

#### Commit 1: 核心框架
```bash
git add include/systolic/Analysis/ParametricSpaceTime.h
git add lib/Analysis/ParametricSpaceTime.cpp
git add include/systolic/Analysis/SpaceTimeAnalysis.h
git add lib/Analysis/SpaceTimeAnalysis.cpp
git add lib/Analysis/CMakeLists.txt
git commit -m "feat: Add Phase 2 parametric space-time framework

- Add ParametricSpaceTime class for unified ST0-ST5 configuration
- Implement parametric data flow analysis
- Add configuration inference
- Preserve backward compatibility"
```

#### Commit 2: Polymer 集成
```bash
git add lib/Analysis/PolymerAnalysis.cpp
git add lib/Transforms/SystolicTransform.cpp
git commit -m "refactor: Improve Polymer integration and error handling

- Clean up unused OpenScop references
- Add comprehensive debug logging
- Enhance error diagnostics
- Implement selectSpaceLoopsParametric"
```

#### Commit 3: 代码生成
```bash
git add lib/Transforms/SystolicDataflowGeneration.cpp
git commit -m "feat: Integrate parametric framework into code generation

- Add flowDirection field to ArrayRefGroup
- Run parametric data flow analysis
- Read space_time_mode from function attributes"
```

#### Commit 4: 构建脚本
```bash
git add scripts/build-polygeist.sh
git add scripts/build-systolic.sh
git commit -m "build: Add parallel build control to build scripts

- Support -j/--jobs option and NINJA_JOBS/JOBS env vars
- Priority: cmdline > env var > memory detection
- Backward compatible"
```

#### Commit 5: 测试框架
```bash
git add test/PHASE2_TESTING_GUIDE.md
git add test/PHASE2_TEST_STATUS.md
git add test/test_phase2_basic.sh
git add test/test_phase2_parametric.sh
git add test/polymer_integration_test.mlir
git add test/test_polymer_available.mlir
git commit -m "test: Add Phase 2 parametric framework test suite

- Add comprehensive testing guide and status report
- Add basic and full test scripts
- Cover ST0-ST5 all configurations
- Support verbose mode and AutoSA comparison"
```

#### Commit 6: Git 配置
```bash
git add .gitmodules
git commit -m "chore: Add AutoSA submodule for reference

Add AutoSA as submodule for code comparison.
Initialize with: git submodule update --init third_party/AutoSA"
```

---

## 四、代码质量

### ✅ 优点
1. **架构清晰**: 统一抽象,易于扩展
2. **向后兼容**: 保留旧接口,新功能可选
3. **文档完善**: 注释齐全,测试指南详细
4. **错误处理**: 验证完善,调试友好
5. **测试覆盖**: 基础 + 完整 + 文档

### ⚠️ 注意点
1. **Polymer 依赖**: 
   - SystolicTransform 强制要求 Polymer
   - 建议: 添加 Mock 或 fallback

2. **命令行选项**: 
   - ST 模式当前硬编码
   - 建议: 添加 `--space-time-mode=N`

3. **测试执行**:
   - 需要 Polymer 才能运行完整测试
   - 当前只验证编译

### 📊 编译验证
```
✅ 所有文件编译通过
✅ 0 Errors
✅ 0 Warnings
✅ 类型检查通过
```

---

## 五、下一步

### 立即可做 (无需 Polymer)
1. ✅ 代码审查
2. ✅ 分批提交
3. ✅ 更新主 README

### 需要 Polymer
1. ⏳ 运行 `test_phase2_basic.sh`
2. ⏳ 运行 `test_phase2_parametric.sh`
3. ⏳ 与 AutoSA 对比

### 后续改进
1. 💡 添加 `--space-time-mode` Pass 选项
2. 💡 实现 Mock Polymer 用于 CI
3. 💡 扩展到更多算子

---

## 六、关键文件速查

### 核心实现
- `include/systolic/Analysis/ParametricSpaceTime.h` - 参数化框架头文件
- `lib/Analysis/ParametricSpaceTime.cpp` - 实现
- `lib/Analysis/SpaceTimeAnalysis.cpp` - 参数化分析集成

### 集成点
- `lib/Transforms/SystolicTransform.cpp` - 循环选择
- `lib/Transforms/SystolicDataflowGeneration.cpp` - 代码生成

### 测试
- `test/test_phase2_basic.sh` - 快速测试
- `test/test_phase2_parametric.sh` - 完整测试
- `test/PHASE2_TESTING_GUIDE.md` - 测试指南

### 文档
- `docs/status/UNCOMMITTED_CODE_REVIEW.md` - 完整梳理(英文)
- `docs/status/UNCOMMITTED_CODE_SUMMARY.md` - 本文件(中文简要)

---

**梳理完成时间**: 2026-01-07  
**审核状态**: ✅ 待审核  
**建议操作**: 审核后分批提交,先核心框架,后测试支持

# 项目需要的 Polymer 库

## 必需的 Polymer 库

根据项目代码分析，需要构建以下 **3 个 Polymer 库**：

### 1. PolymerSupport ✅ 必需
- **库文件**: `libPolymerSupport.a`
- **用途**: 
  - 提供 `IslScop` 类（SCoP 的核心数据结构）
  - 提供 `ScopStmt` 等基础支持
- **链接位置**: `lib/Analysis/CMakeLists.txt` (SystolicAnalysis)

### 2. PolymerTargetISL ✅ 必需
- **库文件**: `libPolymerTargetISL.a`
- **用途**:
  - 提供 `createIslFromFuncOp()` 函数（SCoP 提取）
  - 提供 ISL 相关的转换功能
- **链接位置**: `lib/Analysis/CMakeLists.txt` (SystolicAnalysis)

### 3. PolymerTransforms ✅ 必需（最关键）
- **库文件**: `libPolymerTransforms.a`
- **用途**:
  - 提供 `ExtractScopStmt` Pass（将 affine.for 转换为 scop.stmt 格式）
  - 提供 `createExtractScopStmtPass()` 函数
  - **这是 SystolicTransform Pass 运行 ExtractScopStmt 所必需的**
- **链接位置**: 
  - `lib/Analysis/CMakeLists.txt` (SystolicAnalysis)
  - `lib/Transforms/CMakeLists.txt` (SystolicTransforms) - **必需**

## 构建命令

```bash
cd third_party/Polygeist/build

# 构建所有必需的 Polymer 库
ninja PolymerSupport PolymerTargetISL PolymerTransforms

# 或者只构建 PolymerTransforms（会自动构建依赖）
ninja PolymerTransforms
```

## 验证

构建完成后，检查库文件：

```bash
ls -la third_party/Polygeist/build/lib/libPolymer*.a
```

应该看到：
- ✅ `libPolymerSupport.a`
- ✅ `libPolymerTargetISL.a`
- ✅ `libPolymerTransforms.a` ← **最关键**

## 代码中的使用

### SystolicAnalysis 使用：
- `polymer::createIslFromFuncOp()` - 来自 PolymerTargetISL
- `polymer::IslScop` - 来自 PolymerSupport
- `polymer::createExtractScopStmtPass()` - 来自 PolymerTransforms

### SystolicTransforms 使用：
- `polymer::createExtractScopStmtPass()` - 来自 PolymerTransforms（必需）

## 依赖关系

```
PolymerTransforms
  ├── PolymerSupport (依赖)
  └── PolymerTargetISL (依赖)

所以构建 PolymerTransforms 会自动构建其他两个库
```

## 总结

**最小构建命令**（推荐）：
```bash
ninja PolymerTransforms
```

这会自动构建所有必需的依赖库。

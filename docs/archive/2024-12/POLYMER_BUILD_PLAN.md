# Polymer 集成构建计划

> **目标**: 使用 mlir-systolic 下的 Polygeist submodule 重新构建和集成 Polymer

---

## 📋 当前状态分析

### 1. Polygeist Submodule 结构

```
third_party/Polygeist/
├── tools/polymer/
│   ├── include/polymer/          # Polymer 头文件
│   ├── lib/                      # Polymer 库源码
│   └── CMakeLists.txt            # Polymer 构建配置
└── CMakeLists.txt                # Polygeist 主配置
```

### 2. Polymer 依赖关系

根据 `tools/polymer/CMakeLists.txt` 分析：

**必需依赖**:
- ✅ **GMP**: `find_package(GMP REQUIRED)`
- ✅ **MLIR/LLVM**: 已通过主项目提供
- ✅ **ISL**: 从 Polly 构建目录获取
  - 路径: `${LLVM_BINARY_DIR}/tools/polly/lib/External/isl/include`
  - 或: `${MLIR_MAIN_SRC_DIR}/../polly/lib/External/isl/include`

**构建目标**:
- `PolymerSupport`: 核心支持库
- `PolymerTargetISL`: ISL 后端库（我们需要这个）

### 3. ISL 头文件位置

Polymer 期望从以下位置找到 ISL 头文件：
1. `${LLVM_BINARY_DIR}/tools/polly/lib/External/isl/include`
2. `${MLIR_MAIN_SRC_DIR}/../polly/lib/External/isl/include`

---

## 🎯 构建策略

### 方案 A: 在 mlir-systolic 中构建 Polygeist（推荐）

**优点**:
- 统一管理依赖
- 版本一致
- 简化配置

**步骤**:
1. 在 `third_party/Polygeist/build` 构建 Polygeist
2. 启用 Polymer 和 ISL 后端
3. 在 mlir-systolic 的 CMakeLists.txt 中引用构建产物

### 方案 B: 使用已构建的 Polygeist

**优点**:
- 快速开始
- 不需要重新构建

**缺点**:
- 版本可能不匹配
- 路径配置复杂

---

## 🔧 实施计划

### 阶段 1: 构建 Polygeist Submodule

```bash
cd third_party/Polygeist
mkdir build && cd build
cmake .. \
  -DMLIR_DIR=<mlir-cmake-path> \
  -DLLVM_DIR=<llvm-cmake-path> \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=ON \
  -DPOLYGEIST_ENABLE_POLYMER=ON \
  -GNinja
ninja PolymerSupport PolymerTargetISL
```

### 阶段 2: 更新 mlir-systolic CMakeLists.txt

需要配置：
1. **Polymer 头文件路径**: `third_party/Polygeist/tools/polymer/include`
2. **ISL 头文件路径**: 从 LLVM 构建目录获取
3. **Polymer 库路径**: `third_party/Polygeist/build/lib`
4. **GMP 依赖**: 需要找到 GMP

### 阶段 3: 更新 PolymerAnalysis.cpp

1. 修复 ISL 头文件包含
2. 修复 MLIR API 调用
3. 移除异常处理

---

## 📝 CMakeLists.txt 重构计划

### 主 CMakeLists.txt 需要：

1. **检测 Polygeist submodule**
2. **检测 Polygeist 构建目录**
3. **添加 Polymer 头文件路径**
4. **添加 ISL 头文件路径**（从 LLVM 构建目录）
5. **链接 Polymer 库**
6. **处理 GMP 依赖**

### lib/Analysis/CMakeLists.txt 需要：

1. **条件编译定义**: `SYSTOLIC_ENABLE_POLYMER`
2. **链接 Polymer 库**: `PolymerSupport`, `PolymerTargetISL`
3. **链接 GMP**（如果需要）

---

## 🔍 需要检查的依赖

### 1. GMP
```cmake
find_package(GMP REQUIRED)
target_link_libraries(SystolicAnalysis PRIVATE GMP::GMP)
```

### 2. ISL 头文件
- 检查 LLVM 构建目录中是否有 ISL
- 如果没有，需要单独构建 ISL 或使用系统 ISL

### 3. MLIR/LLVM 版本兼容性
- 确保 Polygeist 与当前 MLIR 版本兼容

---

## ✅ 验证步骤

1. **构建 Polygeist**:
   ```bash
   cd third_party/Polygeist/build
   ninja PolymerSupport PolymerTargetISL
   ```

2. **检查构建产物**:
   ```bash
   ls -la lib/libPolymer*.a
   ```

3. **配置 mlir-systolic**:
   ```bash
   cd build
   cmake .. -DSYSTOLIC_ENABLE_POLYMER=ON
   ```

4. **检查编译定义**:
   ```bash
   cat lib/Analysis/CMakeFiles/obj.SystolicAnalysis.dir/flags.make | grep SYSTOLIC
   ```

5. **编译测试**:
   ```bash
   ninja SystolicAnalysis
   ```

---

## 🚀 下一步行动

1. ✅ 分析 Polygeist 构建配置
2. ⏳ 构建 Polygeist submodule
3. ⏳ 重构 CMakeLists.txt
4. ⏳ 修复代码兼容性问题
5. ⏳ 测试验证


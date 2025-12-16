# MLIR-Systolic 实现进度

## ✅ Phase 1 & 2: SystolicDataflow Dialect 完成

### 已完成的工作

1. **Dialect 定义 (TableGen)**
   - ✅ `SystolicDataflow.td` - Dialect 基础定义
   - ✅ `SystolicDataflowOps.td` - 操作定义
     - `IOModuleOp` - IO 模块（L1/L2/L3）
     - `PEArrayOp` - PE 阵列
     - `DoubleBufferOp` - 双缓冲操作
     - `DrainModuleOp` - 输出模块
   - ✅ CMakeLists.txt 配置

2. **C++ 实现**
   - ✅ `SystolicDataflowDialect.cpp` - Dialect 注册和验证
   - ✅ `SystolicDataflow.h` - 头文件
   - ✅ 操作验证器实现
   - ✅ 辅助方法实现

3. **工具集成**
   - ✅ 在 `systolic-opt` 中注册 Dialect
   - ✅ CMakeLists.txt 链接配置

### 文件结构

```
include/systolic/Dialect/SystolicDataflow/
├── SystolicDataflow.td          # Dialect 定义
├── SystolicDataflowOps.td       # 操作定义
├── SystolicDataflow.h           # 头文件
└── CMakeLists.txt               # TableGen 配置

lib/Dialect/SystolicDataflow/
├── SystolicDataflowDialect.cpp  # 实现
└── CMakeLists.txt               # 构建配置
```

### 下一步

现在可以开始 Phase 3：实现数组引用分析，为 `SystolicDataflowGeneration` Pass 做准备。

## 📋 待实现

### Phase 3: 数组引用分析
- [ ] 实现 `ArrayRefGroup` 数据结构
- [ ] 实现数组引用收集和分析
- [ ] 实现 IO/PE/Drain 分类逻辑
- [ ] 实现 IO 层级分析（L1/L2/L3）

### Phase 4: SystolicDataflowGeneration Pass
- [ ] 实现 Pass 框架
- [ ] 实现多层 IO 模块生成
- [ ] 实现双缓冲逻辑生成
- [ ] 实现 PE 阵列生成

### Phase 5: SystolicDataflowToHLS 降级
- [ ] 实现降级 Pass
- [ ] 实现 Dialect 转换模式

## 🔧 构建和测试

```bash
# 构建项目
cd build
cmake .. -DMLIR_DIR=$POLYGEIST_BUILD/lib/cmake/mlir -GNinja
ninja

# 测试 Dialect（需要先创建测试文件）
ninja check-systolic
```

## 📝 注意事项

1. **TableGen 生成**：确保在构建前运行 TableGen 生成头文件
2. **Dialect 注册**：确保在工具中正确注册 Dialect
3. **类型系统**：目前没有定义自定义类型，使用 MLIR 内置类型


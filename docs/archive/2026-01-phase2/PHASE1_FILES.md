# Phase 1 实施文件清单

## 创建的新文件

### 1. 头文件
- **`include/systolic/Analysis/ParametricSpaceTime.h`**
  - 行数：330 行
  - 内容：
    - `ParametricSpaceTime` 主类定义
    - `SpaceDimConfig` 结构体（空间维配置）
    - `TimeDimConfig` 结构体（时间维配置）
    - `ReductionDimConfig` 结构体（约化维配置）
    - `SystolicFlowDir` 枚举（数据流方向）
    - 预设工厂函数声明（`createST0()` - `createST5()`）
    - 验证、序列化、调试方法

### 2. 实现文件
- **`lib/Analysis/ParametricSpaceTime.cpp`**
  - 行数：190 行
  - 内容：
    - `ParametricSpaceTime` 类实现
    - 所有配置访问方法
    - 验证逻辑
    - Spacetime 类型字符串推导
    - 6 个 ST 预设实现（ST0–ST5）
    - 调试输出（dump、print、toString）

### 3. 文档
- **`docs/AUTOSA_REPRODUCTION_ROADMAP.md`** (3500 行)
  - 完整的技术路线文档
  - 包含目标、架构、设计、实施细节
  
- **`docs/PHASE1_COMPLETION_REPORT.md`** (200 行)
  - 第一阶段完成报告
  - 包含成就、指标、下一步

## 修改的现有文件

### 1. 分析头文件
- **`include/systolic/Analysis/SpaceTimeAnalysis.h`**
  - 添加：`#include "systolic/Analysis/ParametricSpaceTime.h"`
  - 删除：重复的 `SystolicFlowDir` 枚举定义
  - 添加：`SpaceTimeInfo::parametric` 字段
  - 添加：`inferParametricSpaceTime()` 函数声明
  - 改动：3 处（包含、删除、新增）

### 2. 分析实现文件
- **`lib/Analysis/SpaceTimeAnalysis.cpp`**
  - 添加：`inferParametricSpaceTime()` 函数实现
  - 行数增加：~30 行
  - 特点：初版保留 ST3 默认（向后兼容）

### 3. 构建配置
- **`lib/Analysis/CMakeLists.txt`**
  - 添加：`ParametricSpaceTime.cpp` 到 `add_mlir_library()` 源列表
  - 位置：第 4 行（作为第一个源文件）
  - 目的：确保编译新的实现文件

## 编译验证清单

```
✅ lib/Analysis/ParametricSpaceTime.cpp
   - 编译成功，无错误
   - 警告：0 个

✅ lib/Analysis/SpaceTimeAnalysis.cpp
   - 编译成功，无新增错误
   - 包含链无循环

✅ SystolicAnalysis 库
   - 构建成功
   - 目标：lib/libSystolicAnalysis.a

✅ SystolicTransforms 库
   - 构建成功
   - 无新增编译错误

✅ systolic-opt 工具
   - 链接成功
   - 目标：bin/systolic-opt

✅ systolic-translate 工具
   - 链接成功
   - 目标：bin/systolic-translate
```

## 关键变更统计

| 指标 | 值 |
|------|-----|
| 新增代码行数 | ~520 |
| 修改现有代码行数 | ~40 |
| 新增文件 | 5 |
| 修改文件 | 3 |
| 编译错误（新增） | 0 |
| 编译警告（新增） | 0 |
| 编译目标（成功） | 4/4 |

## 功能清单

### ParametricSpaceTime 核心功能
- ✅ 支持 1D（ST0/ST1/ST2）和 2D（ST3/ST4/ST5）PE 配置
- ✅ 灵活的配置构建接口（`addSpaceDim()`, `addTimeDim()`, etc.)
- ✅ 6 个完整的预设工厂函数
- ✅ 自动验证与合法性检查
- ✅ Spacetime 类型字符串推导
- ✅ 数据流方向映射（`operandFlows`）
- ✅ 约化维显式支持
- ✅ 详细的调试输出接口

### SpaceTimeAnalysis 改进
- ✅ `SpaceTimeInfo` 与 `ParametricSpaceTime` 集成
- ✅ `inferParametricSpaceTime()` 函数实现
- ✅ 模块化的配置推导接口
- ✅ 向后兼容性保证

## 依赖关系

```
ParametricSpaceTime.h
  ├─ mlir/IR/Value.h
  ├─ llvm/ADT/DenseMap.h
  ├─ llvm/ADT/SmallVector.h
  ├─ llvm/ADT/StringRef.h
  └─ <string>

ParametricSpaceTime.cpp
  ├─ ParametricSpaceTime.h
  ├─ llvm/Support/Debug.h
  ├─ llvm/Support/raw_ostream.h
  ├─ llvm/ADT/StringRef.h
  └─ <sstream>

SpaceTimeAnalysis.h (改动)
  ├─ ParametricSpaceTime.h (新增)
  ├─ mlir/Dialect/Affine/IR/AffineOps.h
  ├─ mlir/IR/Value.h
  ├─ llvm/ADT/DenseMap.h
  └─ llvm/ADT/SmallVector.h

SpaceTimeAnalysis.cpp (改动)
  └─ (无新增外部依赖)
```

## 向后兼容性验证

- ✅ 现有 `SpaceTimeInfo` 字段完全保留
- ✅ 新增 `parametric` 字段不强制使用
- ✅ 现有函数签名未修改
- ✅ 新增函数不破坏现有调用
- ✅ ST3 默认行为不变
- ✅ 所有现有代码可正常编译

## 下一步提示

### Phase 2 即将开始的改动文件
1. **`lib/Transforms/SystolicTransform.cpp`**
   - 替换硬编码 `[0, 1]` 和 `[2..]`
   - 使用 `info.parametric`

2. **`lib/Analysis/SpaceTimeAnalysis.cpp`**
   - 完善 `analyzeDataFlow()` 实现
   - 添加访问投影分析

3. **`lib/Transforms/SystolicDataflowGeneration.cpp`**
   - 替换数组名判断
   - 使用 `info.parametric.operandFlows`

### 预期改动范围
- 文件数：3 个
- 改动行数：~200-300 行
- 新增函数：2-3 个
- 预期工期：1-2 周

---

**生成时间**: 2026-01-07  
**版本**: Phase 1 完成版  
**状态**: ✅ 就绪进入 Phase 2

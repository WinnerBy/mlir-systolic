# mlir-systolic 构建步骤（优化版 - 防止 WSL 卡死）

## 问题分析

WSL 卡死通常是因为：
1. **内存不足**：LLVM/MLIR 构建需要大量内存（建议至少 8GB）
2. **并行度过高**：默认 `-j$(nproc)` 可能使用过多并行任务
3. **构建类型**：Release 模式比 Debug 模式内存占用更少

## 优化策略

根据 Polymer README 和实际经验，我们采用以下优化：

### 1. 使用 Release 模式（减少内存占用）
### 2. 限制并行度（避免内存溢出）
### 3. 分阶段构建（先构建依赖，再构建目标）

## 构建步骤（优化版）

### 步骤 1: 清理旧构建（如果存在）

```bash
cd /home/user/work/mlir-systolic/third_party/Polygeist
rm -rf build
```

### 步骤 2: 配置 CMake（统一构建方式）

```bash
cd /home/user/work/mlir-systolic/third_party/Polygeist
mkdir -p build
cd build

# 使用 Release 模式，减少内存占用
# 使用 lld 链接器加速（如果可用）
cmake -G Ninja ../llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1 \
  -DLLVM_USE_LINKER=lld
```

**关键优化：**
- `-DCMAKE_BUILD_TYPE=Release` - Release 模式，内存占用更少
- `-DLLVM_ENABLE_ASSERTIONS=OFF` - 关闭断言，减少内存占用
- `-DLLVM_USE_LINKER=lld` - 使用 lld 链接器（如果可用，加速构建）

### 步骤 3: 分阶段构建（避免内存溢出）

**阶段 1: 先构建 LLVM 核心库（限制并行度）**

```bash
# 限制并行度为 2，避免内存溢出
# 如果内存充足（>16GB），可以增加到 4
ninja -j2 LLVMSupport LLVMCore LLVMBitReader LLVMBitWriter
```

**阶段 2: 构建 MLIR 核心库**

```bash
ninja -j2 MLIRSupport MLIRIR MLIRAffineDialect MLIRAffineAnalysis
```

**阶段 3: 构建 Polly（ISL 需要）**

```bash
ninja -j2 PollyISL Polly
```

**阶段 4: 构建 Polymer 库（我们需要的）**

```bash
# 构建 Polymer 库，会自动构建依赖
ninja -j2 PolymerSupport PolymerTargetISL PolymerTransforms
```

**或者，如果内存非常有限（<8GB）：**

```bash
# 单线程构建（最慢但最安全）
ninja -j1 PolymerSupport PolymerTargetISL PolymerTransforms
```

### 步骤 4: 验证构建

```bash
ls -la lib/libPolymer*.a
```

应该看到：
- ✅ `libPolymerSupport.a`
- ✅ `libPolymerTargetISL.a`
- ✅ `libPolymerTransforms.a`

## 内存优化建议

### 如果内存 < 8GB

1. **使用单线程构建**：
   ```bash
   ninja -j1 PolymerTransforms
   ```

2. **增加 swap 空间**：
   ```bash
   # 检查当前 swap
   free -h
   
   # 如果 swap 不足，可以增加（需要 root）
   sudo fallocate -l 4G /swapfile
   sudo chmod 600 /swapfile
   sudo mkswap /swapfile
   sudo swapon /swapfile
   ```

3. **关闭其他程序**，释放内存

### 如果内存 8-16GB

使用 `-j2` 或 `-j4`：
```bash
ninja -j2 PolymerTransforms
```

### 如果内存 > 16GB

可以使用更多并行度：
```bash
ninja -j4 PolymerTransforms
# 或
ninja -j$(($(nproc) / 2)) PolymerTransforms
```

## 完整构建命令（一键执行）

如果内存充足（>16GB），可以一次性构建：

```bash
cd /home/user/work/mlir-systolic/third_party/Polygeist
rm -rf build
mkdir -p build && cd build

cmake -G Ninja ../llvm-project/llvm \
  -DLLVM_ENABLE_PROJECTS="clang;mlir;polly" \
  -DLLVM_EXTERNAL_PROJECTS="polygeist" \
  -DLLVM_EXTERNAL_POLYGEIST_SOURCE_DIR=.. \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DPOLYGEIST_ENABLE_POLYMER=1 \
  -DPOLYGEIST_POLYMER_ENABLE_ISL=1 \
  -DLLVM_USE_LINKER=lld

# 限制并行度构建
ninja -j2 PolymerTransforms
```

## 配置 mlir-systolic

构建完成后：

```bash
# 1. 设置环境变量
export POLYGEIST_BUILD=/home/user/work/mlir-systolic/third_party/Polygeist/build

# 2. 配置 mlir-systolic
cd /home/user/work/mlir-systolic
mkdir -p build && cd build
rm -f CMakeCache.txt
cmake ..

# 3. 构建（也限制并行度）
cmake --build . -j2
```

## 故障排除

### 问题 1: WSL 卡死

**解决方案：**
1. 减少并行度：使用 `-j1` 或 `-j2`
2. 增加 swap 空间
3. 使用 Release 模式（已包含在配置中）
4. 关闭其他程序

### 问题 2: 内存不足错误

```bash
# 检查内存使用
free -h

# 如果内存不足，增加 swap 或减少并行度
```

### 问题 3: 构建中断

可以继续构建，ninja 会跳过已完成的：
```bash
ninja -j2 PolymerTransforms
```

### 问题 4: lld 链接器不可用

如果 `-DLLVM_USE_LINKER=lld` 报错，可以移除这个选项：
```bash
cmake ... # 不包含 -DLLVM_USE_LINKER=lld
```

## 参考

- Polymer README: `third_party/Polygeist/tools/polymer/README.md`
- Polygeist README: `third_party/Polygeist/README.md`


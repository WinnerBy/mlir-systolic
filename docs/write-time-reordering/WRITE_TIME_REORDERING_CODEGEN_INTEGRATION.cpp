// ============================================================================
// 写时重排代码生成器集成方案
// ============================================================================
// 这个文件展示如何在 systolic-translate.cpp 中集成重排支持
// ============================================================================

#include <unordered_map>
#include <optional>

// ============================================================================
// 第 1 部分：数据结构
// ============================================================================

namespace {

/// 表示数组的布局重排信息
struct ArrayReorderingInfo {
  std::string arrayName;
  
  // 原始维度和重排后维度
  SmallVector<int64_t, 3> originalDims;
  SmallVector<int64_t, 3> reorderedDims;
  
  // 维度置换：new_dim[i] = originalDims[permutation[i]]
  SmallVector<unsigned, 3> dimPermutation;
  
  // 是否需要重排
  bool needsReordering() const {
    return originalDims != reorderedDims;
  }
  
  // 应用置换到索引
  // 输入索引数组：[idx0, idx1, idx2]
  // 输出索引数组：[idx_perm[0], idx_perm[1], idx_perm[2]]
  SmallVector<std::string, 3> applyPermutation(
      const SmallVector<std::string, 3> &indices) const {
    SmallVector<std::string, 3> result(3);
    for (unsigned i = 0; i < 3; i++) {
      result[i] = indices[dimPermutation[i]];
    }
    return result;
  }
};

} // namespace

// ============================================================================
// 第 2 部分：扩展 SystolicHLSEmitter 类
// ============================================================================

class SystolicHLSEmitter {
private:
  // 新增：存储所有数组的重排信息
  std::unordered_map<std::string, ArrayReorderingInfo> arrayReordering;
  
  // 新增：读取函数属性中的重排信息
  void extractReorderingInfo(func::FuncOp funcOp) {
    // 遍历所有函数参数，查找重排属性
    for (size_t argIdx = 0; argIdx < funcOp.getNumArguments(); ++argIdx) {
      auto arg = funcOp.getArgument(argIdx);
      
      if (auto memrefType = arg.getType().dyn_cast<MemRefType>()) {
        // 获取数组名称
        std::string arrayName = "A";  // 默认值
        if (auto nameAttr = funcOp.getArgAttrOfType<StringAttr>(
                argIdx, "mlir.name")) {
          arrayName = nameAttr.getValue().str();
        }
        
        // 查找重排维度属性
        std::string dimsAttrName = "systolic.reorder." + arrayName + ".dims";
        auto dimsAttr = funcOp->getAttrOfType<ArrayAttr>(dimsAttrName);
        
        // 查找重排置换属性
        std::string permAttrName = "systolic.reorder." + arrayName + ".perm";
        auto permAttr = funcOp->getAttrOfType<ArrayAttr>(permAttrName);
        
        // 如果两个属性都存在，提取信息
        if (dimsAttr && permAttr) {
          ArrayReorderingInfo info;
          info.arrayName = arrayName;
          
          // 原始维度
          for (int64_t dim : memrefType.getShape()) {
            info.originalDims.push_back(dim);
          }
          
          // 重排后维度
          for (auto attr : dimsAttr) {
            if (auto intAttr = attr.dyn_cast<IntegerAttr>()) {
              info.reorderedDims.push_back(intAttr.getInt());
            }
          }
          
          // 维度置换
          for (auto attr : permAttr) {
            if (auto intAttr = attr.dyn_cast<IntegerAttr>()) {
              info.dimPermutation.push_back(intAttr.getInt());
            }
          }
          
          arrayReordering[arrayName] = info;
          
          LLVM_DEBUG({
            llvm::dbgs() << "Extracted reordering info for " << arrayName << ":\n"
                         << "  Original: [" << info.originalDims[0] << ", "
                         << info.originalDims[1] << ", "
                         << info.originalDims[2] << "]\n"
                         << "  Reordered: [" << info.reorderedDims[0] << ", "
                         << info.reorderedDims[1] << ", "
                         << info.reorderedDims[2] << "]\n"
                         << "  Permutation: [" << info.dimPermutation[0] << ", "
                         << info.dimPermutation[1] << ", "
                         << info.dimPermutation[2] << "]\n";
          });
        }
      }
    }
  }
  
  // 新增：获取数组的实际声明维度
  SmallVector<int64_t, 3> getArrayDims(StringRef arrayName) const {
    auto it = arrayReordering.find(arrayName.str());
    if (it != arrayReordering.end() && it->second.needsReordering()) {
      // 如果有重排，使用重排后的维度
      return it->second.reorderedDims;
    } else {
      // 否则使用原始维度（从代码生成器参数推断）
      // 在实际实现中，应该从某处获取原始维度信息
      return {latency, 1, 16};  // 示例：MTTKRP 的 L2 缓冲区大小
    }
  }
  
  // 新增：应用维度置换到访问索引
  SmallVector<std::string, 3> applyAccessPermutation(
      StringRef arrayName,
      const SmallVector<std::string, 3> &originalIndices) const {
    auto it = arrayReordering.find(arrayName.str());
    if (it != arrayReordering.end() && it->second.needsReordering()) {
      return it->second.applyPermutation(originalIndices);
    } else {
      // 无重排，返回原始索引
      return originalIndices;
    }
  }
  
public:
  LogicalResult emit(ModuleOp module) {
    // ... 现有的 emit 代码 ...
    
    // 新增：在处理函数之前，提取重排信息
    for (auto funcOp : module.getOps<func::FuncOp>()) {
      extractReorderingInfo(funcOp);
    }
    
    // ... 继续处理 ...
    return success();
  }

private:
  // 新增：修改数组声明代码生成
  void emitIOL2InIntraTrans(StringRef arrayName) {
    // 获取声明维度（考虑重排）
    auto dims = getArrayDims(arrayName);
    
    os << "/* Module Definition */\n";
    os << "void " << arrayName << "_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, "
       << arrayName << "_t" << arrayPart << " local_" << arrayName 
       << "[" << dims[0] << "][" << dims[1] << "][" << dims[2] << "], "  // ✨ 应用重排后的维度
       << "hls::stream<float> &fifo_" << arrayName << "_local_out, "
       << "bool intra_trans_en) {\n";
    os << "#pragma HLS INLINE OFF\n";
    os << "  /* Variable Declaration */\n";
    os << "  int p0 = idx; // module id\n";
    os << "  ap_uint<32> data_split[" << arrayPart << "];\n";
    os << "  #pragma HLS ARRAY_PARTITION variable=data_split complete\n";
    os << "  /* Variable Declaration */\n\n";
    os << "  if (!intra_trans_en) return;\n\n";
    
    // 新增：应用维度置换到循环索引
    os << "  // Reading from reordered array\n";
    os << "  for (ap_uint<4> c5 = 0; c5 <= " << (arrayPart / simd - 1) << "; c5 += 1) {\n";
    os << "    for (ap_uint<3> c6 = 0; c6 <= " << (latency - 1) << "; c6 += 1) {\n";
    os << "      for (ap_uint<3> c7 = 0; c7 <= " << (latency - 1) << "; c7 += 1) {\n";
    os << "      #pragma HLS PIPELINE II=1\n";
    os << "        {\n";
    os << "          " << arrayName << "_t" << arrayPart << " in_data;\n";
    os << "          " << arrayName << "_t1 out_data;\n";
    
    // ✨ 关键修改：应用置换到数组访问
    SmallVector<std::string, 3> originalIdx = {"c7", "0", "c5"};
    SmallVector<std::string, 3> permutedIdx = applyAccessPermutation(arrayName, originalIdx);
    
    os << "          in_data = local_" << arrayName << "[" << permutedIdx[0] << "]["
       << permutedIdx[1] << "][" << permutedIdx[2] << "];\n";  // 使用置换后的索引
    
    os << "          for (ap_uint<4> n = 0; n < " << arrayPart << "; n++) {\n";
    os << "          #pragma HLS UNROLL\n";
    os << "            data_split[n] = in_data(31, 0);\n";
    os << "            in_data = in_data >> 32;\n";
    os << "          }\n";
    os << "          int split_idx = (c5) % " << arrayPart << ";\n";
    os << "          union {unsigned int ui; float ut;} u;\n";
    os << "          u.ui = (unsigned int)data_split[split_idx];\n";
    os << "          out_data = u.ut;\n";
    os << "          fifo_" << arrayName << "_local_out.write(out_data);\n";
    os << "        }\n";
    os << "      }\n";
    os << "    }\n";
    os << "  }\n";
    os << "}\n";
    os << "/* Module Definition */\n\n";
  }
  
  // 类似地修改 emitIOL2InInterTrans
  void emitIOL2InInterTrans(StringRef arrayName) {
    auto dims = getArrayDims(arrayName);
    
    os << "/* Module Definition */\n";
    os << "void " << arrayName << "_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, "
       << arrayName << "_t" << arrayPart << " local_" << arrayName 
       << "[" << dims[0] << "][" << dims[1] << "][" << dims[2] << "], "  // ✨ 应用重排后的维度
       << "hls::stream<" << arrayName << "_t" << arrayPart << "> &fifo_" << arrayName << "_in, "
       << "hls::stream<" << arrayName << "_t" << arrayPart << "> &fifo_" << arrayName << "_out, "
       << "bool inter_trans_en) {\n";
    os << "#pragma HLS INLINE OFF\n";
    os << "  /* Variable Declaration */\n";
    os << "  int p0 = idx; // module id\n";
    os << "  /* Variable Declaration */\n\n";
    os << "  if (!inter_trans_en) return;\n\n";
    
    os << "  for (ap_uint<2> c3 = p0; c3 <= " << (numPE - 1) << "; c3 += 1) {\n";
    os << "    if (c3 == p0) {\n";
    os << "      for (ap_uint<3> c4 = 0; c4 <= " << (latency - 1) << "; c4 += 1) {\n";
    os << "      #pragma HLS PIPELINE II=1\n";
    os << "        {\n";
    os << "          " << arrayName << "_t" << arrayPart << " in_data;\n";
    os << "          " << arrayName << "_t" << arrayPart << " out_data;\n";
    os << "          in_data = fifo_" << arrayName << "_in.read();\n";
    os << "          out_data = in_data;\n";
    
    // ✨ 关键修改：应用置换到写入索引
    // 写入时的索引是 [c4, 0, ...]，需要应用置换
    SmallVector<std::string, 3> writeIdx = {"c4", "0", "0"};
    SmallVector<std::string, 3> permutedWriteIdx = applyAccessPermutation(arrayName, writeIdx);
    
    os << "          local_" << arrayName << "[" << permutedWriteIdx[0] << "]["
       << permutedWriteIdx[1] << "][" << permutedWriteIdx[2] << "] = out_data;\n";
    
    os << "        }\n";
    os << "      }\n";
    os << "    } else {\n";
    os << "      for (ap_uint<3> c4 = 0; c4 <= " << (latency - 1) << "; c4 += 1) {\n";
    os << "      #pragma HLS PIPELINE II=1\n";
    os << "        {\n";
    os << "          " << arrayName << "_t" << arrayPart << " in_data;\n";
    os << "          " << arrayName << "_t" << arrayPart << " out_data;\n";
    os << "          in_data = fifo_" << arrayName << "_in.read();\n";
    os << "          out_data = in_data;\n";
    os << "          fifo_" << arrayName << "_out.write(out_data);\n";
    os << "        }\n";
    os << "      }\n";
    os << "    }\n";
    os << "  }\n";
    os << "}\n";
    os << "/* Module Definition */\n\n";
  }
};

// ============================================================================
// 第 3 部分：使用示例
// ============================================================================

/*
使用方式：

1. 在 SystolicDataflowGeneration pass 中生成重排属性：
   func.func @kernel_mttkrp(%A: memref<16x2x16xf32>) 
     attributes {
       systolic.reorder.A.dims = [2, 16, 16]
       systolic.reorder.A.perm = [1 : i32, 2 : i32, 0 : i32]
     }

2. 在 systolic-translate 中提取并应用重排：
   auto emitter = SystolicHLSEmitter(...);
   emitter.emit(module);  // 会自动调用 extractReorderingInfo
   
   生成的代码会：
   - 使用重排后的维度 [2, 16, 16] 声明数组
   - 应用置换 [1, 2, 0] 到所有数组访问
   
3. 结果：
   原始访问：local_A[c7][0][c5]
   重排后：  local_A[0][c5][c7]  (应用置换 [1, 2, 0])

示例：MTTKRP
- 原始：读取 local_A[c8][c5][8*c6/16]（非线性在dim2）
- 重排：读取 local_A_reordered[c5][8*c6/16][c8]
- 优势：非线性索引现在在第二维，而不是第三维
*/


//===----------------------------------------------------------------------===//
//
// systolic-translate - Systolic Array Translation Tool
//
// This tool translates MLIR to HLS C++ code for systolic arrays.
//
//===----------------------------------------------------------------------===//

#include "systolic/Dialect/HLS/HLS.h"

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Parser/Parser.h"
#include "mlir/Support/FileUtilities.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <sstream>
#include <unordered_map>
#include <optional>
#include "llvm/ADT/SmallVector.h"

using namespace mlir;

//===----------------------------------------------------------------------===//
// Command Line Options
//===----------------------------------------------------------------------===//

static llvm::cl::opt<std::string> inputFilename(
    llvm::cl::Positional, llvm::cl::desc("<input file>"), llvm::cl::init("-"));

static llvm::cl::opt<std::string> outputFilename(
    "o", llvm::cl::desc("Output filename"), llvm::cl::value_desc("filename"),
    llvm::cl::init("-"));

static llvm::cl::opt<unsigned> arrayPartSize(
    "array-part", llvm::cl::desc("Array partition size (default: 8)"),
    llvm::cl::init(8));

static llvm::cl::opt<unsigned> latencySize(
    "latency", llvm::cl::desc("Latency hiding factor (default: 4)"),
    llvm::cl::init(4));

static llvm::cl::opt<unsigned> simdFactor(
    "simd", llvm::cl::desc("SIMD factor (default: 1)"),
    llvm::cl::init(1));

static llvm::cl::opt<unsigned> problemSize(
    "size", llvm::cl::desc("Problem size N for NxN matrix (default: 32)"),
    llvm::cl::init(32));

//===----------------------------------------------------------------------===//
// HLS C++ Emitter
//===----------------------------------------------------------------------===//

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
    for (unsigned i = 0; i < 3 && i < dimPermutation.size(); i++) {
      unsigned origIdx = dimPermutation[i];
      if (origIdx < indices.size()) {
        result[i] = indices[origIdx];
      } else {
        result[i] = "0";  // 默认值
      }
    }
    return result;
  }
};

class SystolicHLSEmitter {
public:
  SystolicHLSEmitter(raw_ostream &os, unsigned arrayPart, unsigned latency, 
                     unsigned simd, unsigned size)
      : os(os), arrayPart(arrayPart), latency(latency), simd(simd), size(size),
        numPE(arrayPart / latency),
        tileSize(latency * numPE),  // Each tile is latency * numPE
        numTiles(size / tileSize)   // Number of tile iterations per dimension
        {}

  LogicalResult emit(ModuleOp module);

private:
  raw_ostream &os;
  unsigned arrayPart;
  unsigned latency;
  unsigned simd;
  unsigned size;
  unsigned numPE;
  unsigned tileSize;  // = latency * numPE
  unsigned numTiles;  // = size / tileSize
  unsigned indentLevel = 0;
  unsigned valueCounter = 0;
  
  llvm::DenseMap<Value, std::string> valueNames;
  
  // 写时重排信息：存储所有数组的重排信息
  std::unordered_map<std::string, ArrayReorderingInfo> arrayReordering;
  
  // Indentation helpers
  raw_ostream &indent() { indentLevel++; return os; }
  raw_ostream &dedent() { if (indentLevel > 0) indentLevel--; return os; }
  raw_ostream &emitIndent() {
    for (unsigned i = 0; i < indentLevel; i++) os << "  ";
    return os;
  }
  
  // Type helpers
  std::string getTypeName(Type type);
  std::string getName(Value value);
  
  // Write-time reordering methods
  void extractReorderingInfo(func::FuncOp funcOp);
  SmallVector<int64_t, 3> getArrayDims(StringRef arrayName) const;
  SmallVector<std::string, 3> applyAccessPermutation(
      StringRef arrayName,
      const SmallVector<std::string, 3> &originalIndices) const;
  
  // Emission methods
  void emitFileHeader();
  void emitTypeDefinitions();
  void emitModuleDeclarations();
  void emitIOL3InSerialize(StringRef arrayName, StringRef typeName, 
                           unsigned totalSize);
  void emitIOL3In(StringRef arrayName, StringRef typeName);
  void emitIOL2InIntraTrans(StringRef arrayName);
  void emitIOL2InInterTrans(StringRef arrayName);
  void emitIOL2InInterTransBoundary(StringRef arrayName);
  void emitIOL2In(StringRef arrayName);
  void emitIOL2InBoundary(StringRef arrayName);
  void emitPE();
  void emitPEWrapper();
  void emitDummyModules();
  void emitDrainIOL1(StringRef arrayName);
  void emitDrainIOL2(StringRef arrayName);
  void emitDrainIOL3(StringRef arrayName);
  void emitDrainSerialize(StringRef arrayName, unsigned totalSize);
  void emitTopKernel(func::FuncOp funcOp);
  
  LogicalResult emitFunc(func::FuncOp funcOp);
};

} // namespace

std::string SystolicHLSEmitter::getTypeName(Type type) {
  if (type.isF32()) return "float";
  if (type.isF64()) return "double";
  if (type.isInteger(32)) return "int";
  if (type.isInteger(64)) return "long long";
  if (type.isInteger(16)) return "short";
  if (type.isInteger(8)) return "char";
  return "float";
}

std::string SystolicHLSEmitter::getName(Value value) {
  auto it = valueNames.find(value);
  if (it != valueNames.end()) return it->second;
  std::string name = "v" + std::to_string(valueCounter++);
  valueNames[value] = name;
  return name;
}

void SystolicHLSEmitter::emitFileHeader() {
  os << "//===----------------------------------------------------------------------===//\n";
  os << "// Generated by mlir-systolic (systolic-translate)\n";
  os << "// Configuration: array_part=" << arrayPart << ", latency=" << latency 
     << ", simd=" << simd << "\n";
  os << "// PE Array: " << numPE << " x " << numPE << "\n";
  os << "//===----------------------------------------------------------------------===//\n\n";
  
  os << "#include <ap_int.h>\n";
  os << "#include <hls_stream.h>\n\n";
  
  os << "#define min(x,y) ((x < y) ? x : y)\n";
  os << "#define max(x,y) ((x > y) ? x : y)\n\n";
}

void SystolicHLSEmitter::emitTypeDefinitions() {
  os << "/* Data Type */\n";
  os << "typedef float A_t1;\n";
  os << "typedef float B_t1;\n";
  os << "typedef float C_t1;\n";
  
  // Packed types based on array_part
  unsigned packBits = arrayPart * 32;
  os << "typedef ap_uint<512> A_t16;\n";
  os << "typedef ap_uint<" << (arrayPart * 32) << "> A_t" << arrayPart << ";\n";
  os << "typedef ap_uint<512> B_t16;\n";
  os << "typedef ap_uint<" << (arrayPart * 32) << "> B_t" << arrayPart << ";\n";
  os << "typedef ap_uint<512> C_t16;\n";
  os << "typedef ap_uint<" << (latency * 32) << "> C_t" << latency << ";\n";
  os << "/* Data Type */\n\n";
}

void SystolicHLSEmitter::emitModuleDeclarations() {
  os << "/* Module Declarations */\n";
    os << "void A_IO_L3_in(hls::stream<A_t" << arrayPart << "> &fifo_A_in, "
      << "hls::stream<A_t" << arrayPart << "> &fifo_A_local_out);\n";
  os << "void A_IO_L3_in_serialize(A_t16 *A, hls::stream<A_t" << arrayPart 
     << "> &fifo_A_local_out);\n";
  os << "void A_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, A_t" << arrayPart 
     << " local_A[" << latency << "][1], hls::stream<float> &fifo_A_local_out, bool intra_trans_en);\n";
  os << "void A_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, A_t" << arrayPart 
     << " local_A[" << latency << "][1], hls::stream<A_t" << arrayPart 
     << "> &fifo_A_in, hls::stream<A_t" << arrayPart << "> &fifo_A_out, bool inter_trans_en);\n";
  os << "void A_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, A_t" << arrayPart 
     << " local_A[" << latency << "][1], hls::stream<A_t" << arrayPart << "> &fifo_A_in, bool inter_trans_en);\n";
  os << "void A_IO_L2_in(int idx, hls::stream<A_t" << arrayPart << "> &fifo_A_in, "
     << "hls::stream<A_t" << arrayPart << "> &fifo_A_out, "
     << "hls::stream<float> &fifo_A_local_out);\n";
  os << "void A_IO_L2_in_boundary(int idx, hls::stream<A_t" << arrayPart 
     << "> &fifo_A_in, hls::stream<float> &fifo_A_local_out);\n";
  os << "void B_IO_L3_in(hls::stream<B_t" << arrayPart << "> &fifo_B_in, "
     << "hls::stream<B_t" << arrayPart << "> &fifo_B_local_out);\n";
  os << "void B_IO_L3_in_serialize(B_t16 *B, hls::stream<B_t" << arrayPart 
     << "> &fifo_B_local_out);\n";
  os << "void B_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, B_t" << arrayPart 
     << " local_B[" << latency << "][1], hls::stream<float> &fifo_B_local_out, bool intra_trans_en);\n";
  os << "void B_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, B_t" << arrayPart 
     << " local_B[" << latency << "][1], hls::stream<B_t" << arrayPart 
     << "> &fifo_B_in, hls::stream<B_t" << arrayPart << "> &fifo_B_out, bool inter_trans_en);\n";
  os << "void B_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, B_t" << arrayPart 
     << " local_B[" << latency << "][1], hls::stream<B_t" << arrayPart << "> &fifo_B_in, bool inter_trans_en);\n";
  os << "void B_IO_L2_in(int idx, hls::stream<B_t" << arrayPart << "> &fifo_B_in, "
     << "hls::stream<B_t" << arrayPart << "> &fifo_B_out, "
     << "hls::stream<float> &fifo_B_local_out);\n";
  os << "void B_IO_L2_in_boundary(int idx, hls::stream<B_t" << arrayPart 
     << "> &fifo_B_in, hls::stream<float> &fifo_B_local_out);\n";
  os << "void PE_wrapper(int idx, int idy, "
     << "hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out, "
     << "hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, "
     << "hls::stream<float> &fifo_C_drain_out);\n";
  os << "void C_drain_IO_L1_out_intra_trans(int idx, int idy, int c0, int c1, C_t" << latency 
     << " local_C[" << latency << "][1], hls::stream<float> &fifo_C_drain_local_in);\n";
  os << "void C_drain_IO_L1_out_inter_trans(int idx, int idy, int c0, int c1, C_t" << latency 
     << " local_C[" << latency << "][1], hls::stream<C_t" << latency 
     << "> &fifo_C_drain_in, hls::stream<C_t" << latency << "> &fifo_C_drain_out);\n";
  os << "void C_drain_IO_L1_out_inter_trans_boundary(int idx, int idy, int c0, int c1, C_t" << latency 
     << " local_C[" << latency << "][1], hls::stream<C_t" << latency << "> &fifo_C_drain_out);\n";
  os << "void C_drain_IO_L1_out_wrapper(int idx, int idy, "
     << "hls::stream<C_t" << latency << "> &fifo_C_drain_in, "
     << "hls::stream<C_t" << latency << "> &fifo_C_drain_out, "
     << "hls::stream<float> &fifo_C_drain_local_in);\n";
  os << "void C_drain_IO_L1_out_boundary_wrapper(int idx, int idy, "
     << "hls::stream<C_t" << latency << "> &fifo_C_drain_out, "
     << "hls::stream<float> &fifo_C_drain_local_in);\n";
  os << "void C_drain_IO_L2_out(int idx, hls::stream<C_t" << latency 
     << "> &fifo_C_drain_in, hls::stream<C_t" << latency 
     << "> &fifo_C_drain_out, hls::stream<C_t" << latency << "> &fifo_C_drain_local_in);\n";
  os << "void C_drain_IO_L2_out_boundary(int idx, hls::stream<C_t" << latency 
     << "> &fifo_C_drain_out, hls::stream<C_t" << latency << "> &fifo_C_drain_local_in);\n";
  os << "void C_drain_IO_L3_out(hls::stream<C_t" << latency 
     << "> &fifo_C_drain_out, hls::stream<C_t" << latency << "> &fifo_C_drain_local_in);\n";
  os << "void C_drain_IO_L3_out_serialize(C_t16 *C, hls::stream<C_t" << latency << "> &fifo_C_drain_local_in);\n";
  os << "/* Module Declarations */\n\n";
}

void SystolicHLSEmitter::emitIOL3InSerialize(StringRef arrayName, 
                                              StringRef typeName,
                                              unsigned totalSize) {
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_IO_L3_in_serialize(" << arrayName 
     << "_t16 *" << arrayName << ", hls::stream<" << arrayName << "_t" 
     << arrayPart << "> &fifo_" << arrayName << "_local_out) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  " << arrayName << "_t" << arrayPart << " fifo_data;\n";
  os << "  " << arrayName << "_t16 mem_data;\n";
  
  unsigned iterations = (totalSize * totalSize * 4) / 64;  // 512 bits = 64 bytes
  os << "  for (ap_uint<" << (unsigned)ceil(log2(iterations + 1)) << "> i = 0; i < " 
     << iterations << "; i++) {\n";
  os << "  #pragma HLS PIPELINE II=1\n";
  os << "    mem_data = " << arrayName << "[i];\n";
  os << "    for (ap_uint<2> p = 0; p < 2; p++) {\n";
  os << "      fifo_data = mem_data(" << (arrayPart * 32 - 1) << ", 0);\n";
  os << "      mem_data = mem_data >> " << (arrayPart * 32) << ";\n";
  os << "      fifo_" << arrayName << "_local_out.write(fifo_data);\n";
  os << "    }\n";
  os << "  }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitIOL3In(StringRef arrayName, StringRef typeName) {
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_IO_L3_in(hls::stream<" << arrayName << "_t" 
     << arrayPart << "> &fifo_" << arrayName << "_in, hls::stream<" 
     << arrayName << "_t" << arrayPart << "> &fifo_" << arrayName 
     << "_local_out) {\n";
  os << "#pragma HLS INLINE OFF\n";
  // c0, c1, c2 iterate over tiles: numTiles = size / (latency * numPE)
  os << "  for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "    for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1) {\n";
  os << "      // io_L3\n";
  os << "      for (ap_uint<2> c3 = 0; c3 <= " << (numPE - 1) << "; c3 += 1) {\n";
  os << "        // io_L2\n";
  os << "        for (ap_uint<3> c4 = 0; c4 <= " << (latency - 1) << "; c4 += 1) {\n";
  os << "        #pragma HLS PIPELINE II=1\n";
  os << "          // access_coalesce\n";
  os << "          // access_serialize\n";
  os << "          {\n";
  os << "            " << arrayName << "_t" << arrayPart << " in_data;\n";
  os << "            " << arrayName << "_t" << arrayPart << " out_data;\n";
  os << "            in_data = fifo_" << arrayName << "_in.read();\n";
  os << "            out_data = in_data;\n";
  os << "            fifo_" << arrayName << "_local_out.write(out_data);\n";
  os << "          }\n";
  os << "        }\n";
  os << "      }\n";
  os << "    }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitIOL2InIntraTrans(StringRef arrayName) {
  unsigned c5Bound = arrayPart / simd;
  
  // 获取声明维度（考虑重排）
  auto dims = getArrayDims(arrayName);
  
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, "
     << arrayName << "_t" << arrayPart << " local_" << arrayName 
     << "[" << dims[0] << "][" << dims[1] << "][" << dims[2] << "], "
     << "hls::stream<float> &fifo_" << arrayName << "_local_out, bool intra_trans_en) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx; // module id\n";
  os << "  ap_uint<32> data_split[" << arrayPart << "];\n";
  os << "  #pragma HLS ARRAY_PARTITION variable=data_split complete\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  if (!intra_trans_en) return;\n\n";
  os << "  // io_L2\n";
  os << "  // io_L1\n";
  os << "  // pe\n";
  os << "  for (ap_uint<4> c5 = 0; c5 <= " << (c5Bound - 1) << "; c5 += 1) {\n";
  os << "    // latency\n";
  os << "    for (ap_uint<3> c6 = 0; c6 <= " << (latency - 1) << "; c6 += 1) {\n";
  os << "      // latency\n";
  os << "      for (ap_uint<3> c7 = 0; c7 <= " << (latency - 1) << "; c7 += 1) {\n";
  os << "      #pragma HLS PIPELINE II=1\n";
  os << "        {\n";
  os << "          " << arrayName << "_t" << arrayPart << " in_data;\n";
  os << "          " << arrayName << "_t1 out_data;\n";
  
  // 应用维度置换到数组访问
  SmallVector<std::string, 3> originalIdx = {"c7", "0", "c5"};
  SmallVector<std::string, 3> permutedIdx = applyAccessPermutation(arrayName, originalIdx);
  os << "          in_data = local_" << arrayName << "[" << permutedIdx[0] << "]["
     << permutedIdx[1] << "][" << permutedIdx[2] << "];\n";
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

void SystolicHLSEmitter::emitIOL2InInterTrans(StringRef arrayName) {
  // 获取声明维度（考虑重排）
  auto dims = getArrayDims(arrayName);
  
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, "
     << arrayName << "_t" << arrayPart << " local_" << arrayName 
     << "[" << dims[0] << "][" << dims[1] << "][" << dims[2] << "], "
     << "hls::stream<" << arrayName << "_t" << arrayPart << "> &fifo_" << arrayName << "_in, "
     << "hls::stream<" << arrayName << "_t" << arrayPart << "> &fifo_" << arrayName << "_out, "
     << "bool inter_trans_en) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx; // module id\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  if (!inter_trans_en) return;\n\n";
  os << "  for (ap_uint<2> c3 = p0; c3 <= " << (numPE - 1) << "; c3 += 1) {\n";
  os << "    // io_L2\n";
  os << "    if (c3 == p0) {\n";
  os << "      for (ap_uint<3> c4 = 0; c4 <= " << (latency - 1) << "; c4 += 1) {\n";
  os << "      #pragma HLS PIPELINE II=1\n";
  os << "        // access_coalesce\n";
  os << "        {\n";
        os << "          " << arrayName << "_t" << arrayPart << " in_data;\n";
        os << "          " << arrayName << "_t" << arrayPart << " out_data;\n";
        os << "          in_data = fifo_" << arrayName << "_in.read();\n";
        os << "          out_data = in_data;\n";
        
        // 应用维度置换到写入索引
        SmallVector<std::string, 3> writeIdx = {"c4", "0", "0"};
        SmallVector<std::string, 3> permutedWriteIdx = applyAccessPermutation(arrayName, writeIdx);
        os << "          local_" << arrayName << "[" << permutedWriteIdx[0] << "]["
           << permutedWriteIdx[1] << "][" << permutedWriteIdx[2] << "] = out_data;\n";
  os << "        }\n";
  os << "      }\n";
  os << "    } else {\n";
  os << "      for (ap_uint<3> c4 = 0; c4 <= " << (latency - 1) << "; c4 += 1) {\n";
  os << "      #pragma HLS PIPELINE II=1\n";
  os << "        // access_coalesce\n";
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

void SystolicHLSEmitter::emitIOL2InInterTransBoundary(StringRef arrayName) {
  // 获取声明维度（考虑重排）
  auto dims = getArrayDims(arrayName);
  
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, "
     << arrayName << "_t" << arrayPart << " local_" << arrayName 
     << "[" << dims[0] << "][" << dims[1] << "][" << dims[2] << "], "
     << "hls::stream<" << arrayName << "_t" << arrayPart << "> &fifo_" << arrayName << "_in, "
     << "bool inter_trans_en) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx; // module id\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  if (!inter_trans_en) return;\n\n";
  os << "  for (ap_uint<2> c3 = p0; c3 <= " << (numPE - 1) << "; c3 += 1)\n";
  os << "    if (c3 == p0) {\n";
  os << "      // io_L2\n";
  os << "      for (ap_uint<3> c4 = 0; c4 <= " << (latency - 1) << "; c4 += 1) {\n";
      os << "      #pragma HLS PIPELINE II=1\n";
      os << "        // access_coalesce\n";
      os << "        {\n";
      os << "          " << arrayName << "_t" << arrayPart << " in_data;\n";
      os << "          " << arrayName << "_t" << arrayPart << " out_data;\n";
      os << "          in_data = fifo_" << arrayName << "_in.read();\n";
      os << "          out_data = in_data;\n";
      
      // 应用维度置换到写入索引
      SmallVector<std::string, 3> writeIdx = {"c4", "0", "0"};
      SmallVector<std::string, 3> permutedWriteIdx = applyAccessPermutation(arrayName, writeIdx);
      os << "          local_" << arrayName << "[" << permutedWriteIdx[0] << "]["
         << permutedWriteIdx[1] << "][" << permutedWriteIdx[2] << "] = out_data;\n";
      os << "        }\n";
      os << "      }\n";
    os << "    }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitIOL2In(StringRef arrayName) {
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_IO_L2_in(int idx, hls::stream<" << arrayName << "_t" << arrayPart 
     << "> &fifo_" << arrayName << "_in, hls::stream<" << arrayName << "_t" << arrayPart 
     << "> &fifo_" << arrayName << "_out, hls::stream<float> &fifo_" << arrayName << "_local_out) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx; // module id\n";
  os << "  " << arrayName << "_t" << arrayPart << " local_" << arrayName << "_ping[" << latency << "][1];\n";
  os << "  #pragma HLS RESOURCE variable=local_" << arrayName << "_ping core=RAM_2P_BRAM\n";
  os << "  " << arrayName << "_t" << arrayPart << " local_" << arrayName << "_pong[" << latency << "][1];\n";
  os << "  #pragma HLS RESOURCE variable=local_" << arrayName << "_pong core=RAM_2P_BRAM\n";
  os << "  bool arb = 0;\n";
  os << "  bool inter_trans_en = 1;\n";
  os << "  bool intra_trans_en = 1;\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  {\n";
  os << "    for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "      for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1) {\n";
  os << "        // array\n";
  os << "        // io_L3\n";
  os << "        if (arb == 0) {\n";
  os << "          " << arrayName << "_IO_L2_in_inter_trans(\n";
  os << "            /* module id */ idx, \n";
  os << "            /* host iter */ c0, \n";
  os << "            /* host iter */ c1, \n";
  os << "            /* host iter */ 0, \n";
  os << "            /* array */ local_" << arrayName << "_pong, \n";
  os << "            /* fifo */ fifo_" << arrayName << "_in, \n";
  os << "            /* fifo */ fifo_" << arrayName << "_out, \n";
  os << "            /* enable */ inter_trans_en\n";
  os << "          );\n";
  os << "          for (ap_uint<3> c2 = 0; c2 <= " << (numTiles - 1) << "; c2 += 1) {\n";
  os << "            " << arrayName << "_IO_L2_in_intra_trans(\n";
  os << "              /* module id */ idx, \n";
  os << "              /* host iter */ c0, \n";
  os << "              /* host iter */ c1, \n";
  os << "              /* host iter */ c2, \n";
  os << "              /* array */ local_" << arrayName << "_pong, \n";
  os << "              /* fifo */ fifo_" << arrayName << "_local_out, \n";
  os << "              /* enable */ intra_trans_en\n";
  os << "            );\n";
  os << "          }\n";
  os << "        } else {\n";
  os << "          " << arrayName << "_IO_L2_in_inter_trans(\n";
  os << "            /* module id */ idx, \n";
  os << "            /* host iter */ c0, \n";
  os << "            /* host iter */ c1, \n";
  os << "            /* host iter */ 0, \n";
  os << "            /* array */ local_" << arrayName << "_ping, \n";
  os << "            /* fifo */ fifo_" << arrayName << "_in, \n";
  os << "            /* fifo */ fifo_" << arrayName << "_out, \n";
  os << "            /* enable */ inter_trans_en\n";
  os << "          );\n";
  os << "          for (ap_uint<3> c2 = 0; c2 <= " << (numTiles - 1) << "; c2 += 1) {\n";
  os << "            " << arrayName << "_IO_L2_in_intra_trans(\n";
  os << "              /* module id */ idx, \n";
  os << "              /* host iter */ c0, \n";
  os << "              /* host iter */ c1, \n";
  os << "              /* host iter */ c2, \n";
  os << "              /* array */ local_" << arrayName << "_ping, \n";
  os << "              /* fifo */ fifo_" << arrayName << "_local_out, \n";
  os << "              /* enable */ intra_trans_en\n";
  os << "            );\n";
  os << "          }\n";
  os << "        }\n";
  os << "        arb = !arb;\n";
  os << "      }\n";
  os << "  }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitIOL2InBoundary(StringRef arrayName) {
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_IO_L2_in_boundary(int idx, hls::stream<" << arrayName << "_t" << arrayPart 
     << "> &fifo_" << arrayName << "_in, hls::stream<float> &fifo_" << arrayName << "_local_out) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx; // module id\n";
  os << "  " << arrayName << "_t" << arrayPart << " local_" << arrayName << "_ping[" << latency << "][1];\n";
  os << "  #pragma HLS RESOURCE variable=local_" << arrayName << "_ping core=RAM_2P_BRAM\n";
  os << "  " << arrayName << "_t" << arrayPart << " local_" << arrayName << "_pong[" << latency << "][1];\n";
  os << "  #pragma HLS RESOURCE variable=local_" << arrayName << "_pong core=RAM_2P_BRAM\n";
  os << "  bool arb = 0;\n";
  os << "  bool inter_trans_en = 1;\n";
  os << "  bool intra_trans_en = 1;\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  {\n";
  os << "    for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "      for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1) {\n";
  os << "        // array\n";
  os << "        // io_L3\n";
  os << "        if (arb == 0) {\n";
  os << "          " << arrayName << "_IO_L2_in_inter_trans_boundary(\n";
  os << "            /* module id */ idx, \n";
  os << "            /* host iter */ c0, \n";
  os << "            /* host iter */ c1, \n";
  os << "            /* host iter */ 0, \n";
  os << "            /* array */ local_" << arrayName << "_pong, \n";
  os << "            /* fifo */ fifo_" << arrayName << "_in, \n";
  os << "            /* enable */ inter_trans_en\n";
  os << "          );\n";
  os << "          for (ap_uint<3> c2 = 0; c2 <= " << (numTiles - 1) << "; c2 += 1) {\n";
  os << "            " << arrayName << "_IO_L2_in_intra_trans(\n";
  os << "              /* module id */ idx, \n";
  os << "              /* host iter */ c0, \n";
  os << "              /* host iter */ c1, \n";
  os << "              /* host iter */ c2, \n";
  os << "              /* array */ local_" << arrayName << "_pong, \n";
  os << "              /* fifo */ fifo_" << arrayName << "_local_out, \n";
  os << "              /* enable */ intra_trans_en\n";
  os << "            );\n";
  os << "          }\n";
  os << "        } else {\n";
  os << "          " << arrayName << "_IO_L2_in_inter_trans_boundary(\n";
  os << "            /* module id */ idx, \n";
  os << "            /* host iter */ c0, \n";
  os << "            /* host iter */ c1, \n";
  os << "            /* host iter */ 0, \n";
  os << "            /* array */ local_" << arrayName << "_ping, \n";
  os << "            /* fifo */ fifo_" << arrayName << "_in, \n";
  os << "            /* enable */ inter_trans_en\n";
  os << "          );\n";
  os << "          for (ap_uint<3> c2 = 0; c2 <= " << (numTiles - 1) << "; c2 += 1) {\n";
  os << "            " << arrayName << "_IO_L2_in_intra_trans(\n";
  os << "              /* module id */ idx, \n";
  os << "              /* host iter */ c0, \n";
  os << "              /* host iter */ c1, \n";
  os << "              /* host iter */ c2, \n";
  os << "              /* array */ local_" << arrayName << "_ping, \n";
  os << "              /* fifo */ fifo_" << arrayName << "_local_out, \n";
  os << "              /* enable */ intra_trans_en\n";
  os << "            );\n";
  os << "          }\n";
  os << "        }\n";
  os << "        arb = !arb;\n";
  os << "      }\n";
  os << "  }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitPE() {
  // c5 bound: arrayPart / simd = 8 / 1 = 8 iterations (0 to 7)
  unsigned c5Bound = arrayPart / simd;
  
  os << "/* Module Definition */\n";
  os << "void PE(int idx, int idy, "
     << "hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out, "
     << "hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, "
     << "hls::stream<float> &fifo_C_drain_out) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx, p1 = idy; // module id\n";
  os << "  A_t1 local_A[1][1];\n";
  os << "  #pragma HLS ARRAY_PARTITION variable=local_A dim=0 complete\n";
  os << "  B_t1 local_B[1][1];\n";
  os << "  #pragma HLS ARRAY_PARTITION variable=local_B dim=0 complete\n";
  os << "  C_t1 local_C[" << latency << "][" << latency << "];\n";
  os << "  #pragma HLS RESOURCE variable=local_C core=RAM_2P_BRAM\n";
  os << "  /* Variable Declaration */\n\n";
  
  // Main computation loops - c0, c1, c2 iterate over numTiles
  os << "  for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "    for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1)\n";
  os << "      for (ap_uint<3> c2 = 0; c2 <= " << (numTiles - 1) << "; c2 += 1) {\n";
  os << "        // array\n";
  os << "        // pe\n";
  os << "        for (ap_uint<4> c5 = 0; c5 <= " << (c5Bound - 1) << "; c5 += 1) {\n";
  os << "          // latency\n";
  os << "          for (ap_uint<3> c6 = 0; c6 <= " << (latency - 1) << "; c6 += 1) {\n";
  os << "            // latency\n";
  os << "            for (ap_uint<3> c7 = 0; c7 <= " << (latency - 1) << "; c7 += 1) {\n";
  os << "            #pragma HLS PIPELINE II=1\n";
  os << "              {\n";
  os << "                local_A[0][0] = fifo_A_in.read();\n";
  os << "                local_B[0][0] = fifo_B_in.read();\n";
  os << "                local_C[c7][c6] = (local_C[c7][c6] + (local_A[0][0] * local_B[0][0]));\n";
  os << "                if (c2 == " << (numTiles - 1) << " && c5 == " << (c5Bound - 1) << ")\n";
  os << "                  fifo_C_drain_out.write(local_C[c7][c6]);\n";
  os << "                fifo_B_out.write(local_B[0][0]);\n";
  os << "                fifo_A_out.write(local_A[0][0]);\n";
  os << "              }\n";
  os << "            }\n";
  os << "          }\n";
  os << "        }\n";
  os << "      }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitPEWrapper() {
  os << "/* Module Definition */\n";
  os << "void PE_wrapper(int idx, int idy, "
     << "hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out, "
     << "hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, "
     << "hls::stream<float> &fifo_C_drain_out) {\n";
  os << "  PE(idx, idy, fifo_A_in, fifo_A_out, fifo_B_in, fifo_B_out, "
     << "fifo_C_drain_out);\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitDummyModules() {
  unsigned c5Bound = arrayPart / simd;
  
  // A dummy module
  os << "/* Module Definition */\n";
  os << "void A_PE_dummy_in(int idx, int idy, hls::stream<float> &fifo_A_in) {\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx, p1 = idy; // module id\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "    for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1)\n";
  os << "      for (ap_uint<3> c2 = 0; c2 <= " << (numTiles - 1) << "; c2 += 1) {\n";
  os << "        // array\n";
  os << "        // pe\n";
  os << "        for (ap_uint<4> c5 = 0; c5 <= " << (c5Bound - 1) << "; c5 += 1) {\n";
  os << "          // latency\n";
  os << "          for (ap_uint<3> c6 = 0; c6 <= " << (latency - 1) << "; c6 += 1) {\n";
  os << "            // latency\n";
  os << "            for (ap_uint<3> c7 = 0; c7 <= " << (latency - 1) << "; c7 += 1) {\n";
  os << "            #pragma HLS PIPELINE II=1\n";
  os << "              A_t1 fifo_data;\n";
  os << "              fifo_data = fifo_A_in.read();\n";
  os << "            }\n";
  os << "          }\n";
  os << "        }\n";
  os << "      }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
  
  // B dummy module
  os << "/* Module Definition */\n";
  os << "void B_PE_dummy_in(int idx, int idy, hls::stream<float> &fifo_B_in) {\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx, p1 = idy; // module id\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "    for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1)\n";
  os << "      for (ap_uint<3> c2 = 0; c2 <= " << (numTiles - 1) << "; c2 += 1) {\n";
  os << "        // array\n";
  os << "        // pe\n";
  os << "        for (ap_uint<4> c5 = 0; c5 <= " << (c5Bound - 1) << "; c5 += 1) {\n";
  os << "          // latency\n";
  os << "          for (ap_uint<3> c6 = 0; c6 <= " << (latency - 1) << "; c6 += 1) {\n";
  os << "            // latency\n";
  os << "            for (ap_uint<3> c7 = 0; c7 <= " << (latency - 1) << "; c7 += 1) {\n";
  os << "            #pragma HLS PIPELINE II=1\n";
  os << "              B_t1 fifo_data;\n";
  os << "              fifo_data = fifo_B_in.read();\n";
  os << "            }\n";
  os << "          }\n";
  os << "        }\n";
  os << "      }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitDrainIOL1(StringRef arrayName) {
  // C_drain_IO_L1_out_intra_trans
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L1_out_intra_trans(int idx, int idy, int c0, int c1, "
     << arrayName << "_t" << latency << " local_" << arrayName << "[" << latency << "][1], "
     << "hls::stream<float> &fifo_" << arrayName << "_drain_local_in) {\n";
  os << "#pragma HLS INLINE\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx, p1 = idy; // module id\n";
  os << "  ap_uint<32> data_split[" << latency << "];\n";
  os << "  #pragma HLS ARRAY_PARTITION variable=data_split complete\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  // io_L1\n";
  os << "  // pe\n";
  os << "  // latency\n";
  os << "  for (ap_uint<3> c6 = 0; c6 <= " << (latency - 1) << "; c6 += 1) {\n";
  os << "    // latency\n";
  os << "    for (ap_uint<3> c7 = 0; c7 <= " << (latency - 1) << "; c7 += 1) {\n";
  os << "    #pragma HLS PIPELINE II=1\n";
  os << "      {\n";
  os << "        " << arrayName << "_t1 in_data;\n";
  os << "        " << arrayName << "_t" << latency << " out_data;\n";
  os << "        in_data = fifo_" << arrayName << "_drain_local_in.read();\n";
  os << "        int split_idx = (c6) % " << latency << ";\n";
  os << "        out_data = local_" << arrayName << "[c7][c6 / " << latency << "];\n";
  os << "        for (ap_uint<3> n = 0; n < " << latency << "; n++) {\n";
  os << "        #pragma HLS UNROLL\n";
  os << "          data_split[n] = out_data(31, 0);\n";
  os << "          out_data = out_data >> 32;\n";
  os << "        }\n";
  os << "        union {unsigned int ui; float ut;} u;\n";
  os << "        u.ut = in_data;\n";
  os << "        data_split[split_idx] = ap_uint<32>(u.ui);\n";
  os << "        out_data = (";
  for (unsigned i = latency - 1; i > 0; i--) {
    os << "data_split[" << i << "], ";
  }
  os << "data_split[0]);\n";
  os << "        local_" << arrayName << "[c7][c6 / " << latency << "] = out_data;\n";
  os << "      }\n";
  os << "    }\n";
  os << "  }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
  
  // C_drain_IO_L1_out_inter_trans
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L1_out_inter_trans(int idx, int idy, int c0, int c1, "
     << arrayName << "_t" << latency << " local_" << arrayName << "[" << latency << "][1], "
     << "hls::stream<" << arrayName << "_t" << latency << "> &fifo_" << arrayName 
     << "_drain_in, hls::stream<" << arrayName << "_t" << latency << "> &fifo_" << arrayName 
     << "_drain_out) {\n";
  os << "#pragma HLS INLINE\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx, p1 = idy; // module id\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<2> c4 = p1; c4 <= " << (numPE - 1) << "; c4 += 1) {\n";
  os << "    // io_L1\n";
  os << "    if (c4 == p1) {\n";
  os << "      for (ap_uint<3> c5 = 0; c5 <= " << (latency - 1) << "; c5 += 1) {\n";
  os << "      #pragma HLS PIPELINE II=1\n";
  os << "        // access_coalesce\n";
  os << "        {\n";
  os << "          " << arrayName << "_t" << latency << " in_data;\n";
  os << "          " << arrayName << "_t" << latency << " out_data;\n";
  os << "          in_data = local_" << arrayName << "[c5][0];\n";
  os << "          out_data = in_data;\n";
  os << "          fifo_" << arrayName << "_drain_out.write(out_data);\n";
  os << "        }\n";
  os << "      }\n";
  os << "    } else {\n";
  os << "      for (ap_uint<3> c5 = 0; c5 <= " << (latency - 1) << "; c5 += 1) {\n";
  os << "      #pragma HLS PIPELINE II=1\n";
  os << "        // access_coalesce\n";
  os << "        {\n";
  os << "          " << arrayName << "_t" << latency << " in_data;\n";
  os << "          " << arrayName << "_t" << latency << " out_data;\n";
  os << "          in_data = fifo_" << arrayName << "_drain_in.read();\n";
  os << "          out_data = in_data;\n";
  os << "          fifo_" << arrayName << "_drain_out.write(out_data);\n";
  os << "        }\n";
  os << "      }\n";
  os << "    }\n";
  os << "  }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
  
  // C_drain_IO_L1_out_inter_trans_boundary
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L1_out_inter_trans_boundary(int idx, int idy, int c0, int c1, "
     << arrayName << "_t" << latency << " local_" << arrayName << "[" << latency << "][1], "
     << "hls::stream<" << arrayName << "_t" << latency << "> &fifo_" << arrayName << "_drain_out) {\n";
  os << "#pragma HLS INLINE\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx, p1 = idy; // module id\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<2> c4 = p1; c4 <= " << (numPE - 1) << "; c4 += 1)\n";
  os << "    if (c4 == p1) {\n";
  os << "      // io_L1\n";
  os << "      for (ap_uint<3> c5 = 0; c5 <= " << (latency - 1) << "; c5 += 1) {\n";
  os << "      #pragma HLS PIPELINE II=1\n";
  os << "        // access_coalesce\n";
  os << "        {\n";
  os << "          " << arrayName << "_t" << latency << " in_data;\n";
  os << "          " << arrayName << "_t" << latency << " out_data;\n";
  os << "          in_data = local_" << arrayName << "[c5][0];\n";
  os << "          out_data = in_data;\n";
  os << "          fifo_" << arrayName << "_drain_out.write(out_data);\n";
  os << "        }\n";
  os << "      }\n";
  os << "    }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
  
  // C_drain_IO_L1_out
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L1_out(int idx, int idy, "
     << "hls::stream<" << arrayName << "_t" << latency << "> &fifo_" << arrayName << "_drain_in, "
     << "hls::stream<" << arrayName << "_t" << latency << "> &fifo_" << arrayName << "_drain_out, "
     << "hls::stream<float> &fifo_" << arrayName << "_drain_local_in) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx, p1 = idy; // module id\n";
  os << "  " << arrayName << "_t" << latency << " local_" << arrayName << "[" << latency << "][1];\n";
  os << "  #pragma HLS RESOURCE variable=local_" << arrayName << " core=RAM_2P_BRAM\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "    for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1) {\n";
  os << "      // array\n";
  os << "      // io_L3\n";
  os << "      // io_L2\n";
  os << "      " << arrayName << "_drain_IO_L1_out_intra_trans(\n";
  os << "        /* module id */ idx, \n";
  os << "        /* module id */ idy, \n";
  os << "        /* host iter */ c0, \n";
  os << "        /* host iter */ c1, \n";
  os << "        /* array */ local_" << arrayName << ", \n";
  os << "        /* fifo */ fifo_" << arrayName << "_drain_local_in\n";
  os << "      );\n";
  os << "      " << arrayName << "_drain_IO_L1_out_inter_trans(\n";
  os << "        /* module id */ idx, \n";
  os << "        /* module id */ idy, \n";
  os << "        /* host iter */ c0, \n";
  os << "        /* host iter */ c1, \n";
  os << "        /* array */ local_" << arrayName << ", \n";
  os << "        /* fifo */ fifo_" << arrayName << "_drain_in, \n";
  os << "        /* fifo */ fifo_" << arrayName << "_drain_out\n";
  os << "      );\n";
  os << "    }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
  
  // C_drain_IO_L1_out_wrapper
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L1_out_wrapper(int idx, int idy, "
     << "hls::stream<" << arrayName << "_t" << latency << "> &fifo_" << arrayName << "_drain_in, "
     << "hls::stream<" << arrayName << "_t" << latency << "> &fifo_" << arrayName << "_drain_out, "
     << "hls::stream<float> &fifo_" << arrayName << "_drain_local_in) {\n";
  os << "  " << arrayName << "_drain_IO_L1_out(\n";
  os << "    /* module id */ idx, \n";
  os << "    /* module id */ idy, \n";
  os << "    /* fifo */ fifo_" << arrayName << "_drain_in, \n";
  os << "    /* fifo */ fifo_" << arrayName << "_drain_out, \n";
  os << "    /* fifo */ fifo_" << arrayName << "_drain_local_in);\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
  
  // C_drain_IO_L1_out_boundary
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L1_out_boundary(int idx, int idy, "
     << "hls::stream<" << arrayName << "_t" << latency << "> &fifo_" << arrayName << "_drain_out, "
     << "hls::stream<float> &fifo_" << arrayName << "_drain_local_in) {\n";
  os << "#pragma HLS INLINE\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx, p1 = idy; // module id\n";
  os << "  " << arrayName << "_t" << latency << " local_" << arrayName << "[" << latency << "][1];\n";
  os << "  #pragma HLS RESOURCE variable=local_" << arrayName << " core=RAM_2P_BRAM\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "    for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1) {\n";
  os << "      // array\n";
  os << "      // io_L3\n";
  os << "      " << arrayName << "_drain_IO_L1_out_intra_trans(\n";
  os << "        /* module id */ idx, \n";
  os << "        /* module id */ idy, \n";
  os << "        /* host iter */ c0, \n";
  os << "        /* host iter */ c1, \n";
  os << "        /* array */ local_" << arrayName << ", \n";
  os << "        /* fifo */ fifo_" << arrayName << "_drain_local_in\n";
  os << "      );\n";
  os << "      " << arrayName << "_drain_IO_L1_out_inter_trans_boundary(\n";
  os << "        /* module id */ idx, \n";
  os << "        /* module id */ idy, \n";
  os << "        /* host iter */ c0, \n";
  os << "        /* host iter */ c1, \n";
  os << "        /* array */ local_" << arrayName << ", \n";
  os << "        /* fifo */ fifo_" << arrayName << "_drain_out\n";
  os << "      );\n";
  os << "    }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
  
  // C_drain_IO_L1_out_boundary_wrapper
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L1_out_boundary_wrapper(int idx, int idy, "
     << "hls::stream<" << arrayName << "_t" << latency << "> &fifo_" << arrayName << "_drain_out, "
     << "hls::stream<float> &fifo_" << arrayName << "_drain_local_in) {\n";
  os << "  " << arrayName << "_drain_IO_L1_out_boundary(\n";
  os << "    /* module id */ idx, \n";
  os << "    /* module id */ idy, \n";
  os << "    /* fifo */ fifo_" << arrayName << "_drain_out, \n";
  os << "    /* fifo */ fifo_" << arrayName << "_drain_local_in);\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitDrainIOL2(StringRef arrayName) {
  // C_drain_IO_L2_out
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L2_out(int idx, hls::stream<" << arrayName << "_t" << latency 
     << "> &fifo_" << arrayName << "_drain_in, hls::stream<" << arrayName << "_t" << latency 
     << "> &fifo_" << arrayName << "_drain_out, hls::stream<" << arrayName << "_t" << latency 
     << "> &fifo_" << arrayName << "_drain_local_in) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx; // module id\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "    for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1) {\n";
  os << "      // array\n";
  os << "      // io_L3\n";
  os << "      for (ap_uint<2> c3 = p0; c3 <= " << (numPE - 1) << "; c3 += 1) {\n";
  os << "        // io_L2\n";
  os << "        if (c3 == p0) {\n";
  os << "          for (ap_uint<2> c4 = 0; c4 <= " << (numPE - 1) << "; c4 += 1) {\n";
  os << "            // io_L1\n";
  os << "            for (ap_uint<3> c5 = 0; c5 <= " << (latency - 1) << "; c5 += 1) {\n";
  os << "            #pragma HLS PIPELINE II=1\n";
  os << "              // access_coalesce\n";
  os << "              {\n";
  os << "                " << arrayName << "_t" << latency << " in_data;\n";
  os << "                " << arrayName << "_t" << latency << " out_data;\n";
  os << "                in_data = fifo_" << arrayName << "_drain_local_in.read();\n";
  os << "                out_data = in_data;\n";
  os << "                fifo_" << arrayName << "_drain_out.write(out_data);\n";
  os << "              }\n";
  os << "            }\n";
  os << "          }\n";
  os << "        } else {\n";
  os << "          for (ap_uint<2> c4 = 0; c4 <= " << (numPE - 1) << "; c4 += 1) {\n";
  os << "            // io_L1\n";
  os << "            for (ap_uint<3> c5 = 0; c5 <= " << (latency - 1) << "; c5 += 1) {\n";
  os << "            #pragma HLS PIPELINE II=1\n";
  os << "              // access_coalesce\n";
  os << "              {\n";
  os << "                " << arrayName << "_t" << latency << " in_data;\n";
  os << "                " << arrayName << "_t" << latency << " out_data;\n";
  os << "                in_data = fifo_" << arrayName << "_drain_in.read();\n";
  os << "                out_data = in_data;\n";
  os << "                fifo_" << arrayName << "_drain_out.write(out_data);\n";
  os << "              }\n";
  os << "            }\n";
  os << "          }\n";
  os << "        }\n";
  os << "      }\n";
  os << "    }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
  
  // C_drain_IO_L2_out_boundary
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L2_out_boundary(int idx, hls::stream<" << arrayName << "_t" << latency 
     << "> &fifo_" << arrayName << "_drain_out, hls::stream<" << arrayName << "_t" << latency 
     << "> &fifo_" << arrayName << "_drain_local_in) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  int p0 = idx; // module id\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "    for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1) {\n";
  os << "      // array\n";
  os << "      // io_L3\n";
  os << "      for (ap_uint<2> c3 = p0; c3 <= " << (numPE - 1) << "; c3 += 1)\n";
  os << "        if (c3 == p0) {\n";
  os << "          // io_L2\n";
  os << "          for (ap_uint<2> c4 = 0; c4 <= " << (numPE - 1) << "; c4 += 1) {\n";
  os << "            // io_L1\n";
  os << "            for (ap_uint<3> c5 = 0; c5 <= " << (latency - 1) << "; c5 += 1) {\n";
  os << "            #pragma HLS PIPELINE II=1\n";
  os << "              // access_coalesce\n";
  os << "              {\n";
  os << "                " << arrayName << "_t" << latency << " in_data;\n";
  os << "                " << arrayName << "_t" << latency << " out_data;\n";
  os << "                in_data = fifo_" << arrayName << "_drain_local_in.read();\n";
  os << "                out_data = in_data;\n";
  os << "                fifo_" << arrayName << "_drain_out.write(out_data);\n";
  os << "              }\n";
  os << "            }\n";
  os << "          }\n";
  os << "        }\n";
  os << "    }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitDrainIOL3(StringRef arrayName) {
  // C_drain_IO_L3_out
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L3_out(hls::stream<" << arrayName << "_t" << latency 
     << "> &fifo_" << arrayName << "_drain_out, hls::stream<" << arrayName << "_t" << latency 
     << "> &fifo_" << arrayName << "_drain_local_in) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<3> c0 = 0; c0 <= " << (numTiles - 1) << "; c0 += 1)\n";
  os << "    for (ap_uint<3> c1 = 0; c1 <= " << (numTiles - 1) << "; c1 += 1) {\n";
  os << "      // array\n";
  os << "      // io_L3\n";
  os << "      for (ap_uint<2> c3 = 0; c3 <= " << (numPE - 1) << "; c3 += 1) {\n";
  os << "        // io_L2\n";
  os << "        for (ap_uint<2> c4 = 0; c4 <= " << (numPE - 1) << "; c4 += 1) {\n";
  os << "          // io_L1\n";
  os << "          for (ap_uint<3> c5 = 0; c5 <= " << (latency - 1) << "; c5 += 1) {\n";
  os << "          #pragma HLS PIPELINE II=1\n";
  os << "            // access_coalesce\n";
  os << "            // access_serialize\n";
  os << "            {\n";
  os << "              " << arrayName << "_t" << latency << " in_data;\n";
  os << "              " << arrayName << "_t" << latency << " out_data;\n";
  os << "              in_data = fifo_" << arrayName << "_drain_local_in.read();\n";
  os << "              out_data = in_data;\n";
  os << "              fifo_" << arrayName << "_drain_out.write(out_data);\n";
  os << "            }\n";
  os << "          }\n";
  os << "        }\n";
  os << "      }\n";
  os << "    }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitDrainSerialize(StringRef arrayName, unsigned totalSize) {
  // C_drain_IO_L3_out_serialize
  // C_t16 = 512 bits, C_t4 = 128 bits, so 512 / 128 = 4
  unsigned packFactor = 16 / latency;  // 16 / 4 = 4
  unsigned iterations = (totalSize * totalSize * 4) / 64;  // 512 bits = 64 bytes
  os << "/* Module Definition */\n";
  os << "void " << arrayName << "_drain_IO_L3_out_serialize(" << arrayName 
     << "_t16 *" << arrayName << ", hls::stream<" << arrayName << "_t" << latency 
     << "> &fifo_" << arrayName << "_drain_local_in) {\n";
  os << "#pragma HLS INLINE OFF\n";
  os << "  /* Variable Declaration */\n";
  os << "  /* Variable Declaration */\n\n";
  os << "  for (ap_uint<" << (unsigned)ceil(log2(iterations + 1)) << "> i = 0; i < " 
     << iterations << "; i++) {\n";
  os << "  #pragma HLS PIPELINE II=1\n";
  os << "    " << arrayName << "_t" << latency << " fifo_data;\n";
  os << "    " << arrayName << "_t16 mem_data;\n";
  os << "    " << arrayName << "_t" << latency << " mem_data_split[" << packFactor << "];\n";
  os << "    #pragma HLS ARRAY_PARTITION variable=mem_data_split complete\n";
  os << "    for (ap_uint<3> p = 0; p < " << packFactor << "; p++) {\n";
  os << "      fifo_data = fifo_" << arrayName << "_drain_local_in.read();\n";
  os << "      mem_data_split[p] = fifo_data;\n";
  os << "    }\n";
  os << "    mem_data = (";
  for (unsigned i = packFactor - 1; i > 0; i--) {
    os << "mem_data_split[" << i << "], ";
  }
  os << "mem_data_split[0]);\n";
  os << "    " << arrayName << "[i] = mem_data;\n";
  os << "  }\n";
  os << "}\n";
  os << "/* Module Definition */\n\n";
}

void SystolicHLSEmitter::emitTopKernel(func::FuncOp funcOp) {
  os << "extern \"C\" {\n";
  os << "void kernel0(A_t16 *A, B_t16 *B, C_t16 *C) {\n";
  os << "#pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem_A\n";
  os << "#pragma HLS INTERFACE m_axi port=B offset=slave bundle=gmem_B\n";
  os << "#pragma HLS INTERFACE m_axi port=C offset=slave bundle=gmem_C\n";
  os << "#pragma HLS INTERFACE s_axilite port=A bundle=control\n";
  os << "#pragma HLS INTERFACE s_axilite port=B bundle=control\n";
  os << "#pragma HLS INTERFACE s_axilite port=C bundle=control\n";
  os << "#pragma HLS INTERFACE s_axilite port=return bundle=control\n\n";
  os << "#pragma HLS DATAFLOW\n\n";
  
  // FIFO declarations
  os << "  /* FIFO Declaration */\n";
  os << "  hls::stream<A_t" << arrayPart << "> fifo_A_A_IO_L3_in_serialize;\n";
  os << "  #pragma HLS STREAM variable=fifo_A_A_IO_L3_in_serialize depth=2\n";
  os << "  hls::stream<B_t" << arrayPart << "> fifo_B_B_IO_L3_in_serialize;\n";
  os << "  #pragma HLS STREAM variable=fifo_B_B_IO_L3_in_serialize depth=2\n";
  os << "  hls::stream<C_t" << latency << "> fifo_C_drain_C_drain_IO_L3_out_serialize;\n";
  os << "  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L3_out_serialize depth=2\n";
  os << "  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L3_out_serialize core=FIFO_SRL\n\n";
  
  // L2 FIFOs
  for (unsigned i = 0; i <= numPE; i++) {
    os << "  hls::stream<A_t" << arrayPart << "> fifo_A_A_IO_L2_in_" << i << ";\n";
    os << "  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_" << i << " depth=2\n";
    os << "  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_" << i << " core=FIFO_SRL\n";
  }
  for (unsigned i = 0; i <= numPE; i++) {
    os << "  hls::stream<B_t" << arrayPart << "> fifo_B_B_IO_L2_in_" << i << ";\n";
    os << "  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_" << i << " depth=2\n";
    os << "  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_" << i << " core=FIFO_SRL\n";
  }
  
  // PE FIFOs
  for (unsigned i = 0; i < numPE; i++) {
    for (unsigned j = 0; j <= numPE; j++) {
      os << "  hls::stream<float> fifo_A_PE_" << i << "_" << j << ";\n";
      os << "  #pragma HLS STREAM variable=fifo_A_PE_" << i << "_" << j << " depth=2\n";
      os << "  #pragma HLS RESOURCE variable=fifo_A_PE_" << i << "_" << j << " core=FIFO_SRL\n";
    }
  }
  for (unsigned i = 0; i <= numPE; i++) {
    for (unsigned j = 0; j < numPE; j++) {
      os << "  hls::stream<float> fifo_B_PE_" << i << "_" << j << ";\n";
      os << "  #pragma HLS STREAM variable=fifo_B_PE_" << i << "_" << j << " depth=2\n";
      os << "  #pragma HLS RESOURCE variable=fifo_B_PE_" << i << "_" << j << " core=FIFO_SRL\n";
    }
  }
  for (unsigned i = 0; i < numPE; i++) {
    for (unsigned j = 0; j < numPE; j++) {
      os << "  hls::stream<float> fifo_C_drain_PE_" << i << "_" << j << ";\n";
      os << "  #pragma HLS STREAM variable=fifo_C_drain_PE_" << i << "_" << j << " depth=2\n";
      os << "  #pragma HLS RESOURCE variable=fifo_C_drain_PE_" << i << "_" << j << " core=FIFO_SRL\n";
    }
  }
  // Drain L1 FIFOs
  for (unsigned i = 0; i < numPE; i++) {
    for (unsigned j = 0; j <= numPE; j++) {
      os << "  hls::stream<C_t" << latency << "> fifo_C_drain_C_drain_IO_L1_out_" << i << "_" << j << ";\n";
      os << "  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_" << i << "_" << j << " depth=2\n";
      os << "  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_" << i << "_" << j << " core=FIFO_SRL\n";
    }
  }
  // Drain L2 FIFOs
  for (unsigned i = 0; i <= numPE; i++) {
    os << "  hls::stream<C_t" << latency << "> fifo_C_drain_C_drain_IO_L2_out_" << i << ";\n";
    os << "  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L2_out_" << i << " depth=2\n";
    os << "  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L2_out_" << i << " core=FIFO_SRL\n";
  }
  os << "  /* FIFO Declaration */\n\n";
  
  // Module calls
  os << "  /* Module Call */\n";
  os << "  A_IO_L3_in_serialize(A, fifo_A_A_IO_L3_in_serialize);\n";
  os << "  A_IO_L3_in(fifo_A_A_IO_L3_in_serialize, fifo_A_A_IO_L2_in_0);\n";
  for (unsigned i = 0; i < numPE - 1; i++) {
    os << "  A_IO_L2_in(" << i << ", fifo_A_A_IO_L2_in_" << i 
       << ", fifo_A_A_IO_L2_in_" << (i + 1) << ", fifo_A_PE_" << i << "_0);\n";
  }
  os << "  A_IO_L2_in_boundary(" << (numPE - 1) << ", fifo_A_A_IO_L2_in_" 
     << (numPE - 1) << ", fifo_A_PE_" << (numPE - 1) << "_0);\n\n";
  
  os << "  B_IO_L3_in_serialize(B, fifo_B_B_IO_L3_in_serialize);\n";
  os << "  B_IO_L3_in(fifo_B_B_IO_L3_in_serialize, fifo_B_B_IO_L2_in_0);\n";
  for (unsigned j = 0; j < numPE - 1; j++) {
    os << "  B_IO_L2_in(" << j << ", fifo_B_B_IO_L2_in_" << j 
       << ", fifo_B_B_IO_L2_in_" << (j + 1) << ", fifo_B_PE_0_" << j << ");\n";
  }
  os << "  B_IO_L2_in_boundary(" << (numPE - 1) << ", fifo_B_B_IO_L2_in_" 
     << (numPE - 1) << ", fifo_B_PE_0_" << (numPE - 1) << ");\n\n";
  
  // PE array calls
  for (unsigned i = 0; i < numPE; i++) {
    for (unsigned j = 0; j < numPE; j++) {
      os << "  PE_wrapper(" << i << ", " << j << ", "
         << "fifo_A_PE_" << i << "_" << j << ", fifo_A_PE_" << i << "_" << (j + 1) << ", "
         << "fifo_B_PE_" << i << "_" << j << ", fifo_B_PE_" << (i + 1) << "_" << j << ", "
         << "fifo_C_drain_PE_" << i << "_" << j << ");\n";
    }
  }
  os << "\n";
  
  // Dummy modules
  for (unsigned i = 0; i < numPE; i++) {
    os << "  A_PE_dummy_in(" << i << ", " << (numPE - 1) << ", fifo_A_PE_" 
       << i << "_" << numPE << ");\n";
  }
  for (unsigned j = 0; j < numPE; j++) {
    os << "  B_PE_dummy_in(" << (numPE - 1) << ", " << j << ", fifo_B_PE_" 
       << numPE << "_" << j << ");\n";
  }
  os << "\n";
  
  // Drain modules
  os << "  /* C drain modules */\n";
  // L1 drain modules - process by column (j), from rightmost to leftmost
  for (unsigned j = numPE - 1; j > 0; j--) {
    // First boundary wrapper for this column
    for (unsigned i = 0; i < numPE; i++) {
      if (j == numPE - 1) {
        // Rightmost column: boundary wrapper
        os << "  C_drain_IO_L1_out_boundary_wrapper(\n";
        os << "    /* module id */ " << i << ", \n";
        os << "    /* module id */ " << j << ", \n";
        os << "    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_" << i << "_" << j << ", \n";
        os << "    /* fifo */ fifo_C_drain_PE_" << i << "_" << j << "\n";
        os << "  );\n";
        os << "  /* Module Call */\n\n";
      }
      // Regular wrapper for this column
      os << "  C_drain_IO_L1_out_wrapper(\n";
      os << "    /* module id */ " << i << ", \n";
      os << "    /* module id */ " << (j - 1) << ", \n";
      os << "    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_" << i << "_" << j << ", \n";
      os << "    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_" << i << "_" << (j - 1) << ", \n";
      os << "    /* fifo */ fifo_C_drain_PE_" << i << "_" << (j - 1) << "\n";
      os << "  );\n";
      os << "  /* Module Call */\n\n";
    }
  }
  // L2 drain modules - boundary first
  os << "  C_drain_IO_L2_out_boundary(\n";
  os << "    /* module id */ " << (numPE - 1) << ", \n";
  os << "    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_" << (numPE - 1) << ", \n";
  os << "    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_" << (numPE - 1) << "_0\n";
  os << "  );\n";
  os << "  /* Module Call */\n\n";
  // L2 drain modules - regular
  for (unsigned i = 0; i < numPE - 1; i++) {
    os << "  C_drain_IO_L2_out(\n";
    os << "    /* module id */ " << i << ", \n";
    os << "    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_" << (i + 1) << ", \n";
    os << "    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_" << i << ", \n";
    os << "    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_" << i << "_0\n";
    os << "  );\n";
    os << "  /* Module Call */\n\n";
  }
  // L3 drain modules
  os << "  C_drain_IO_L3_out(\n";
  os << "    /* fifo */ fifo_C_drain_C_drain_IO_L3_out_serialize, \n";
  os << "    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_0\n";
  os << "  );\n";
  os << "  /* Module Call */\n\n";
  // L3 serialize
  os << "  C_drain_IO_L3_out_serialize(\n";
  os << "    /* array */ C, \n";
  os << "    /* fifo */ fifo_C_drain_C_drain_IO_L3_out_serialize\n";
  os << "  );\n";
  os << "  /* Module Call */\n\n";
  
  os << "}\n";
  os << "}\n";
}

// Extract reordering information from function attributes
void SystolicHLSEmitter::extractReorderingInfo(func::FuncOp funcOp) {
  // 遍历所有函数参数，查找重排属性
  for (size_t argIdx = 0; argIdx < funcOp.getNumArguments(); ++argIdx) {
    auto arg = funcOp.getArgument(argIdx);
    
    if (auto memrefType = arg.getType().dyn_cast<MemRefType>()) {
      // 获取数组名称
      std::string arrayName = "arg" + std::to_string(argIdx);  // 默认值
      if (auto nameAttr = funcOp.getArgAttrOfType<StringAttr>(
              argIdx, "mlir.name")) {
        arrayName = nameAttr.getValue().str();
      } else {
        // 尝试从函数属性中查找（使用 arg0, arg1 等格式）
        std::string attrName = "systolic.reorder.arg" + std::to_string(argIdx) + ".dims";
        if (funcOp->getAttrOfType<ArrayAttr>(attrName)) {
          arrayName = "arg" + std::to_string(argIdx);
        }
      }
      
      // 查找重排维度属性
      std::string dimsAttrName = "systolic.reorder." + arrayName + ".dims";
      auto dimsAttr = funcOp->getAttrOfType<ArrayAttr>(dimsAttrName);
      
      // 如果使用 arg0 格式，也尝试查找
      if (!dimsAttr) {
        dimsAttrName = "systolic.reorder.arg" + std::to_string(argIdx) + ".dims";
        dimsAttr = funcOp->getAttrOfType<ArrayAttr>(dimsAttrName);
        if (dimsAttr) {
          arrayName = "arg" + std::to_string(argIdx);
        }
      }
      
      // 查找重排置换属性
      std::string permAttrName = "systolic.reorder." + arrayName + ".perm";
      auto permAttr = funcOp->getAttrOfType<ArrayAttr>(permAttrName);
      
      if (!permAttr) {
        permAttrName = "systolic.reorder.arg" + std::to_string(argIdx) + ".perm";
        permAttr = funcOp->getAttrOfType<ArrayAttr>(permAttrName);
      }
      
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
      }
    }
  }
}

// Get array dimensions (considering reordering)
SmallVector<int64_t, 3> SystolicHLSEmitter::getArrayDims(StringRef arrayName) const {
  auto it = arrayReordering.find(arrayName.str());
  if (it != arrayReordering.end() && it->second.needsReordering()) {
    // 如果有重排，使用重排后的维度
    return it->second.reorderedDims;
  } else {
    // 否则使用默认维度（L2 缓冲区大小）
    return {static_cast<int64_t>(latency), 1, static_cast<int64_t>(arrayPart)};
  }
}

// Apply dimension permutation to access indices
SmallVector<std::string, 3> SystolicHLSEmitter::applyAccessPermutation(
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

LogicalResult SystolicHLSEmitter::emitFunc(func::FuncOp funcOp) {
  emitTopKernel(funcOp);
  return success();
}

LogicalResult SystolicHLSEmitter::emit(ModuleOp module) {
  // 在处理函数之前，提取重排信息
  for (auto funcOp : module.getOps<func::FuncOp>()) {
    extractReorderingInfo(funcOp);
  }
  
  emitFileHeader();
  emitTypeDefinitions();
  emitModuleDeclarations();
  
  // Emit module implementations
  emitIOL3InSerialize("A", "float", size);
  emitIOL3In("A", "float");
  emitIOL2InIntraTrans("A");
  emitIOL2InInterTrans("A");
  emitIOL2InInterTransBoundary("A");
  emitIOL2In("A");
  emitIOL2InBoundary("A");
  emitIOL3InSerialize("B", "float", size);
  emitIOL3In("B", "float");
  emitIOL2InIntraTrans("B");
  emitIOL2InInterTrans("B");
  emitIOL2InInterTransBoundary("B");
  emitIOL2In("B");
  emitIOL2InBoundary("B");
  emitPE();
  emitPEWrapper();
  emitDummyModules();
  emitDrainIOL1("C");
  emitDrainIOL2("C");
  emitDrainIOL3("C");
  emitDrainSerialize("C", size);
  
  // Emit top kernel - only emit once (not per function)
  // Note: We only emit one kernel0 regardless of how many FuncOps exist
  auto funcOps = module.getOps<func::FuncOp>();
  if (!funcOps.empty()) {
    if (failed(emitFunc(*funcOps.begin())))
      return failure();
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// Main
//===----------------------------------------------------------------------===//

int main(int argc, char **argv) {
  llvm::InitLLVM y(argc, argv);
  llvm::cl::ParseCommandLineOptions(argc, argv, "Systolic HLS C++ Translator\n");
  
  // Set up context
  MLIRContext context;
  context.getOrLoadDialect<affine::AffineDialect>();
  context.getOrLoadDialect<arith::ArithDialect>();
  context.getOrLoadDialect<func::FuncDialect>();
  context.getOrLoadDialect<memref::MemRefDialect>();
  
  // Parse input
  std::string errorMessage;
  auto file = openInputFile(inputFilename, &errorMessage);
  if (!file) {
    llvm::errs() << errorMessage << "\n";
    return 1;
  }
  
  llvm::SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(file), llvm::SMLoc());
  
  auto module = parseSourceFile<ModuleOp>(sourceMgr, &context);
  if (!module) {
    llvm::errs() << "Failed to parse input file\n";
    return 1;
  }
  
  // Set up output
  auto output = openOutputFile(outputFilename, &errorMessage);
  if (!output) {
    llvm::errs() << errorMessage << "\n";
    return 1;
  }
  
  // Emit HLS C++
  SystolicHLSEmitter emitter(output->os(), arrayPartSize, latencySize, simdFactor, problemSize);
  if (failed(emitter.emit(*module))) {
    llvm::errs() << "Failed to emit HLS C++\n";
    return 1;
  }
  
  output->keep();
  return 0;
}

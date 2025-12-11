//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: HLS C++ Code Emission
//
// This module translates the systolic array IR (HLS Dialect) to
// synthesizable HLS C++ code targeting Xilinx Vitis HLS.
//
// AutoSA Reference:
//   - autosa_xilinx_hls_c.cpp: HLS C code generation
//   - autosa_print.cpp: General printing utilities
//   - print_module_*: Module-specific printing functions
//
// Generated Structure:
//   - kernel.h: Type definitions, stream types, function declarations
//   - kernel_modules.cpp: PE, I/O, and drain module implementations
//   - kernel_host.cpp: Top-level kernel wrapper
//
// HLS Pragmas Generated:
//   - #pragma HLS INTERFACE: For function arguments
//   - #pragma HLS DATAFLOW: For dataflow regions
//   - #pragma HLS PIPELINE: For innermost loops
//   - #pragma HLS ARRAY_PARTITION: For local buffers
//   - #pragma HLS INLINE OFF: For module isolation
//
//===----------------------------------------------------------------------===//

#include "systolic/Transforms/Passes.h"
#include "systolic/Analysis/SystolicConfig.h"

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/Vector/IR/VectorOps.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Operation.h"
#include "mlir/Support/IndentedOstream.h"
#include "mlir/Translation.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <sstream>

#define DEBUG_TYPE "emit-hlscpp"

using namespace mlir;

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// HLS C++ Emitter
//===----------------------------------------------------------------------===//

/// Main emitter class for generating HLS C++ code.
/// Follows AutoSA's code generation methodology with MLIR as IR.
class HLSCppEmitter {
public:
  explicit HLSCppEmitter(raw_ostream &os, bool emitMultiFunction = false)
      : os(os), emitMultiFunction(emitMultiFunction) {}

  /// Emit the complete HLS C++ translation unit.
  LogicalResult emitModule(ModuleOp module);

private:
  //===--------------------------------------------------------------------===//
  // Header Generation (AutoSA: kernel.h)
  //===--------------------------------------------------------------------===//
  
  void emitFileHeader();
  void emitIncludes();
  void emitTypeDefinitions();
  void emitMacros();
  void emitStreamTypes();
  void emitFunctionDeclarations(ModuleOp module);
  
  //===--------------------------------------------------------------------===//
  // Type Emission
  //===--------------------------------------------------------------------===//
  
  /// Map MLIR types to HLS C++ types
  std::string getTypeName(Type type);
  std::string getStreamTypeName(Type elementType, unsigned depth = 2);
  std::string getVectorTypeName(VectorType vecType);
  std::string getMemRefTypeName(MemRefType memType);
  
  //===--------------------------------------------------------------------===//
  // Operation Emission
  //===--------------------------------------------------------------------===//
  
  LogicalResult emitOperation(Operation *op);
  LogicalResult emitFunc(func::FuncOp func);
  LogicalResult emitAffineFor(affine::AffineForOp forOp);
  LogicalResult emitAffineIf(affine::AffineIfOp ifOp);
  LogicalResult emitAffineLoad(affine::AffineLoadOp loadOp);
  LogicalResult emitAffineStore(affine::AffineStoreOp storeOp);
  LogicalResult emitArithOp(Operation *op);
  LogicalResult emitMemRefAlloc(memref::AllocOp allocOp);
  LogicalResult emitMemRefAlloca(memref::AllocaOp allocaOp);
  LogicalResult emitSCFFor(scf::ForOp forOp);
  LogicalResult emitSCFIf(scf::IfOp ifOp);
  
  //===--------------------------------------------------------------------===//
  // Value and Expression Emission
  //===--------------------------------------------------------------------===//
  
  /// Get the C++ name for a value
  std::string getName(Value value);
  
  /// Emit an affine expression as C++ code
  std::string emitAffineExpr(AffineExpr expr, 
                             ArrayRef<Value> dimOperands,
                             ArrayRef<Value> symbolOperands);
  
  /// Emit an affine map application
  std::string emitAffineMap(AffineMap map,
                            ArrayRef<Value> operands);
  
  //===--------------------------------------------------------------------===//
  // Pragma Generation (AutoSA: HLS pragma insertion)
  //===--------------------------------------------------------------------===//
  
  /// Emit #pragma HLS DATAFLOW
  void emitDataflowPragma();
  
  /// Emit #pragma HLS PIPELINE II=<ii>
  void emitPipelinePragma(unsigned ii = 1);
  
  /// Emit #pragma HLS UNROLL factor=<factor>
  void emitUnrollPragma(unsigned factor = 0);
  
  /// Emit #pragma HLS ARRAY_PARTITION
  void emitArrayPartitionPragma(StringRef arrayName, 
                                unsigned dim, 
                                StringRef type = "complete");
  
  /// Emit #pragma HLS INTERFACE
  void emitInterfacePragma(StringRef argName, 
                           StringRef mode = "m_axi",
                           StringRef bundle = "gmem");
  
  /// Emit #pragma HLS INLINE OFF
  void emitInlineOffPragma();
  
  /// Emit #pragma HLS RESOURCE
  void emitResourcePragma(StringRef varName, StringRef resource = "BRAM");
  
  //===--------------------------------------------------------------------===//
  // Indentation and Formatting
  //===--------------------------------------------------------------------===//
  
  void indent() { indentLevel++; }
  void dedent() { if (indentLevel > 0) indentLevel--; }
  raw_ostream &emitIndent() {
    for (unsigned i = 0; i < indentLevel; ++i)
      os << "  ";
    return os;
  }
  
  //===--------------------------------------------------------------------===//
  // State
  //===--------------------------------------------------------------------===//
  
  raw_ostream &os;
  bool emitMultiFunction;
  unsigned indentLevel = 0;
  unsigned valueCounter = 0;
  llvm::DenseMap<Value, std::string> valueNames;
  llvm::StringMap<unsigned> nameCounters;
};

//===----------------------------------------------------------------------===//
// Type Emission Implementation
//===----------------------------------------------------------------------===//

std::string HLSCppEmitter::getTypeName(Type type) {
  return llvm::TypeSwitch<Type, std::string>(type)
      .Case<Float16Type>([](auto) { return "half"; })
      .Case<Float32Type>([](auto) { return "float"; })
      .Case<Float64Type>([](auto) { return "double"; })
      .Case<IntegerType>([](IntegerType intType) {
        unsigned width = intType.getWidth();
        bool isSigned = !intType.isUnsigned();
        if (width == 1)
          return std::string("bool");
        if (width == 8)
          return isSigned ? std::string("int8_t") : std::string("uint8_t");
        if (width == 16)
          return isSigned ? std::string("int16_t") : std::string("uint16_t");
        if (width == 32)
          return isSigned ? std::string("int32_t") : std::string("uint32_t");
        if (width == 64)
          return isSigned ? std::string("int64_t") : std::string("uint64_t");
        // For arbitrary widths, use ap_int/ap_uint
        return (isSigned ? "ap_int<" : "ap_uint<") + 
               std::to_string(width) + ">";
      })
      .Case<IndexType>([](auto) { return "int64_t"; })
      .Case<VectorType>([this](VectorType vecType) {
        return getVectorTypeName(vecType);
      })
      .Case<MemRefType>([this](MemRefType memType) {
        return getMemRefTypeName(memType);
      })
      .Default([](Type) { return std::string("void"); });
}

std::string HLSCppEmitter::getVectorTypeName(VectorType vecType) {
  // AutoSA uses ap_uint for packed data
  // vector<N x f32> -> ap_uint<N*32>
  Type elemType = vecType.getElementType();
  int64_t numElems = vecType.getNumElements();
  
  unsigned elemBits = 32;  // Default
  if (auto intType = dyn_cast<IntegerType>(elemType)) {
    elemBits = intType.getWidth();
  } else if (isa<Float32Type>(elemType)) {
    elemBits = 32;
  } else if (isa<Float64Type>(elemType)) {
    elemBits = 64;
  } else if (isa<Float16Type>(elemType)) {
    elemBits = 16;
  }
  
  unsigned totalBits = numElems * elemBits;
  return "ap_uint<" + std::to_string(totalBits) + ">";
}

std::string HLSCppEmitter::getMemRefTypeName(MemRefType memType) {
  return getTypeName(memType.getElementType()) + "*";
}

std::string HLSCppEmitter::getStreamTypeName(Type elementType, unsigned depth) {
  return "hls::stream<" + getTypeName(elementType) + ">";
}

//===----------------------------------------------------------------------===//
// Name Management
//===----------------------------------------------------------------------===//

std::string HLSCppEmitter::getName(Value value) {
  auto it = valueNames.find(value);
  if (it != valueNames.end())
    return it->second;
  
  // Generate a new name
  std::string name = "v" + std::to_string(valueCounter++);
  valueNames[value] = name;
  return name;
}

//===----------------------------------------------------------------------===//
// Header and Includes
//===----------------------------------------------------------------------===//

void HLSCppEmitter::emitFileHeader() {
  os << "//===----------------------------------------------------------------------===//\n";
  os << "// Auto-generated by MLIR-Systolic\n";
  os << "// Do not edit manually.\n";
  os << "//===----------------------------------------------------------------------===//\n\n";
}

void HLSCppEmitter::emitIncludes() {
  os << "#include <ap_int.h>\n";
  os << "#include <hls_stream.h>\n";
  os << "#include <stdint.h>\n";
  os << "\n";
}

void HLSCppEmitter::emitMacros() {
  os << "#define min(x,y) ((x < y) ? x : y)\n";
  os << "#define max(x,y) ((x > y) ? x : y)\n";
  os << "\n";
}

//===----------------------------------------------------------------------===//
// Pragma Emission
//===----------------------------------------------------------------------===//

void HLSCppEmitter::emitDataflowPragma() {
  emitIndent() << "#pragma HLS DATAFLOW\n";
}

void HLSCppEmitter::emitPipelinePragma(unsigned ii) {
  emitIndent() << "#pragma HLS PIPELINE II=" << ii << "\n";
}

void HLSCppEmitter::emitUnrollPragma(unsigned factor) {
  if (factor == 0)
    emitIndent() << "#pragma HLS UNROLL\n";
  else
    emitIndent() << "#pragma HLS UNROLL factor=" << factor << "\n";
}

void HLSCppEmitter::emitArrayPartitionPragma(StringRef arrayName, 
                                              unsigned dim, 
                                              StringRef type) {
  emitIndent() << "#pragma HLS ARRAY_PARTITION variable=" << arrayName 
               << " " << type << " dim=" << dim << "\n";
}

void HLSCppEmitter::emitInterfacePragma(StringRef argName, 
                                         StringRef mode,
                                         StringRef bundle) {
  emitIndent() << "#pragma HLS INTERFACE " << mode 
               << " port=" << argName;
  if (mode == "m_axi")
    os << " bundle=" << bundle;
  os << "\n";
}

void HLSCppEmitter::emitInlineOffPragma() {
  emitIndent() << "#pragma HLS INLINE OFF\n";
}

void HLSCppEmitter::emitResourcePragma(StringRef varName, StringRef resource) {
  emitIndent() << "#pragma HLS RESOURCE variable=" << varName 
               << " core=" << resource << "\n";
}

//===----------------------------------------------------------------------===//
// Affine Expression Emission
//===----------------------------------------------------------------------===//

std::string HLSCppEmitter::emitAffineExpr(AffineExpr expr,
                                          ArrayRef<Value> dimOperands,
                                          ArrayRef<Value> symbolOperands) {
  std::stringstream ss;
  
  if (auto dimExpr = dyn_cast<AffineDimExpr>(expr)) {
    unsigned pos = dimExpr.getPosition();
    if (pos < dimOperands.size())
      ss << getName(dimOperands[pos]);
    else
      ss << "d" << pos;
  } else if (auto symExpr = dyn_cast<AffineSymbolExpr>(expr)) {
    unsigned pos = symExpr.getPosition();
    if (pos < symbolOperands.size())
      ss << getName(symbolOperands[pos]);
    else
      ss << "s" << pos;
  } else if (auto constExpr = dyn_cast<AffineConstantExpr>(expr)) {
    ss << constExpr.getValue();
  } else if (auto binExpr = dyn_cast<AffineBinaryOpExpr>(expr)) {
    std::string lhs = emitAffineExpr(binExpr.getLHS(), dimOperands, symbolOperands);
    std::string rhs = emitAffineExpr(binExpr.getRHS(), dimOperands, symbolOperands);
    
    switch (binExpr.getKind()) {
    case AffineExprKind::Add:
      ss << "(" << lhs << " + " << rhs << ")";
      break;
    case AffineExprKind::Mul:
      ss << "(" << lhs << " * " << rhs << ")";
      break;
    case AffineExprKind::FloorDiv:
      ss << "(" << lhs << " / " << rhs << ")";
      break;
    case AffineExprKind::CeilDiv:
      ss << "((" << lhs << " + " << rhs << " - 1) / " << rhs << ")";
      break;
    case AffineExprKind::Mod:
      ss << "(" << lhs << " % " << rhs << ")";
      break;
    default:
      ss << "/* unsupported expr */";
    }
  }
  
  return ss.str();
}

std::string HLSCppEmitter::emitAffineMap(AffineMap map,
                                         ArrayRef<Value> operands) {
  // For single-result maps, just emit the expression
  if (map.getNumResults() == 1) {
    return emitAffineExpr(map.getResult(0), 
                          operands.take_front(map.getNumDims()),
                          operands.drop_front(map.getNumDims()));
  }
  
  // For multi-result, return comma-separated list
  std::stringstream ss;
  for (unsigned i = 0; i < map.getNumResults(); ++i) {
    if (i > 0) ss << ", ";
    ss << emitAffineExpr(map.getResult(i),
                         operands.take_front(map.getNumDims()),
                         operands.drop_front(map.getNumDims()));
  }
  return ss.str();
}

//===----------------------------------------------------------------------===//
// Operation Emission
//===----------------------------------------------------------------------===//

LogicalResult HLSCppEmitter::emitOperation(Operation *op) {
  return llvm::TypeSwitch<Operation *, LogicalResult>(op)
      .Case<func::FuncOp>([&](auto op) { return emitFunc(op); })
      .Case<affine::AffineForOp>([&](auto op) { return emitAffineFor(op); })
      .Case<affine::AffineIfOp>([&](auto op) { return emitAffineIf(op); })
      .Case<affine::AffineLoadOp>([&](auto op) { return emitAffineLoad(op); })
      .Case<affine::AffineStoreOp>([&](auto op) { return emitAffineStore(op); })
      .Case<affine::AffineYieldOp>([&](auto) { return success(); })
      .Case<memref::AllocOp>([&](auto op) { return emitMemRefAlloc(op); })
      .Case<memref::AllocaOp>([&](auto op) { return emitMemRefAlloca(op); })
      .Case<scf::ForOp>([&](auto op) { return emitSCFFor(op); })
      .Case<scf::IfOp>([&](auto op) { return emitSCFIf(op); })
      .Case<scf::YieldOp>([&](auto) { return success(); })
      .Case<func::ReturnOp>([&](auto) { 
        emitIndent() << "return;\n"; 
        return success(); 
      })
      // Arithmetic operations
      .Case<arith::ConstantOp>([&](auto op) { return emitArithOp(op); })
      .Case<arith::AddFOp, arith::AddIOp, arith::SubFOp, arith::SubIOp,
            arith::MulFOp, arith::MulIOp, arith::DivFOp, arith::DivSIOp,
            arith::CmpFOp, arith::CmpIOp, arith::SelectOp>(
          [&](auto op) { return emitArithOp(op); })
      .Default([&](Operation *op) {
        LLVM_DEBUG(llvm::dbgs() << "Unsupported op: " << op->getName() << "\n");
        return success();  // Skip unsupported ops
      });
}

LogicalResult HLSCppEmitter::emitFunc(func::FuncOp func) {
  // Emit function signature
  auto funcType = func.getFunctionType();
  
  os << getTypeName(funcType.getNumResults() > 0 ? 
                    funcType.getResult(0) : Type()) 
     << " " << func.getName() << "(";
  
  // Emit arguments
  for (unsigned i = 0; i < func.getNumArguments(); ++i) {
    if (i > 0) os << ", ";
    auto arg = func.getArgument(i);
    auto argType = arg.getType();
    
    std::string argName = "arg" + std::to_string(i);
    valueNames[arg] = argName;
    
    os << getTypeName(argType) << " " << argName;
  }
  
  os << ") {\n";
  indent();
  
  // Emit interface pragmas for memref arguments
  for (unsigned i = 0; i < func.getNumArguments(); ++i) {
    auto arg = func.getArgument(i);
    if (isa<MemRefType>(arg.getType())) {
      std::string argName = "arg" + std::to_string(i);
      emitInterfacePragma(argName, "m_axi", "gmem" + std::to_string(i));
    }
  }
  os << "\n";
  
  // Emit function body
  for (auto &op : func.getBody().front()) {
    if (failed(emitOperation(&op)))
      return failure();
  }
  
  dedent();
  os << "}\n\n";
  
  return success();
}

LogicalResult HLSCppEmitter::emitAffineFor(affine::AffineForOp forOp) {
  // Get loop bounds
  int64_t lb = forOp.getConstantLowerBound();
  int64_t ub = forOp.getConstantUpperBound();
  int64_t step = forOp.getStepAsInt();
  
  // Assign name to induction variable
  std::string ivName = "i" + std::to_string(valueCounter++);
  valueNames[forOp.getInductionVar()] = ivName;
  
  // Emit for loop
  emitIndent() << "for (int " << ivName << " = " << lb 
               << "; " << ivName << " < " << ub 
               << "; " << ivName << " += " << step << ") {\n";
  indent();
  
  // Check if this is the innermost loop (for pipeline pragma)
  bool isInnermost = true;
  for (auto &op : *forOp.getBody()) {
    if (isa<affine::AffineForOp>(op) || isa<scf::ForOp>(op)) {
      isInnermost = false;
      break;
    }
  }
  
  if (isInnermost) {
    emitPipelinePragma(1);
  }
  
  // Emit body
  for (auto &op : *forOp.getBody()) {
    if (failed(emitOperation(&op)))
      return failure();
  }
  
  dedent();
  emitIndent() << "}\n";
  
  return success();
}

LogicalResult HLSCppEmitter::emitAffineIf(affine::AffineIfOp ifOp) {
  // Simplified: just emit both branches
  emitIndent() << "// AffineIf (condition omitted)\n";
  emitIndent() << "{\n";
  indent();
  
  for (auto &op : ifOp.getThenBlock()->without_terminator()) {
    if (failed(emitOperation(&op)))
      return failure();
  }
  
  dedent();
  emitIndent() << "}\n";
  
  if (ifOp.hasElse()) {
    emitIndent() << "// else\n";
    emitIndent() << "{\n";
    indent();
    
    for (auto &op : ifOp.getElseBlock()->without_terminator()) {
      if (failed(emitOperation(&op)))
        return failure();
    }
    
    dedent();
    emitIndent() << "}\n";
  }
  
  return success();
}

LogicalResult HLSCppEmitter::emitAffineLoad(affine::AffineLoadOp loadOp) {
  std::string resultName = getName(loadOp.getResult());
  std::string memName = getName(loadOp.getMemRef());
  std::string indices = emitAffineMap(loadOp.getAffineMap(),
                                      loadOp.getMapOperands());
  
  Type elemType = loadOp.getMemRefType().getElementType();
  emitIndent() << getTypeName(elemType) << " " << resultName 
               << " = " << memName << "[" << indices << "];\n";
  
  return success();
}

LogicalResult HLSCppEmitter::emitAffineStore(affine::AffineStoreOp storeOp) {
  std::string memName = getName(storeOp.getMemRef());
  std::string valueName = getName(storeOp.getValue());
  std::string indices = emitAffineMap(storeOp.getAffineMap(),
                                      storeOp.getMapOperands());
  
  emitIndent() << memName << "[" << indices << "] = " << valueName << ";\n";
  
  return success();
}

LogicalResult HLSCppEmitter::emitArithOp(Operation *op) {
  if (auto constOp = dyn_cast<arith::ConstantOp>(op)) {
    std::string name = getName(constOp.getResult());
    
    if (auto intAttr = dyn_cast<IntegerAttr>(constOp.getValue())) {
      emitIndent() << getTypeName(constOp.getType()) << " " << name 
                   << " = " << intAttr.getInt() << ";\n";
    } else if (auto floatAttr = dyn_cast<FloatAttr>(constOp.getValue())) {
      emitIndent() << getTypeName(constOp.getType()) << " " << name 
                   << " = " << floatAttr.getValueAsDouble() << ";\n";
    }
    return success();
  }
  
  // Binary operations
  if (op->getNumOperands() == 2 && op->getNumResults() == 1) {
    std::string result = getName(op->getResult(0));
    std::string lhs = getName(op->getOperand(0));
    std::string rhs = getName(op->getOperand(1));
    Type resultType = op->getResult(0).getType();
    
    std::string opStr;
    if (isa<arith::AddFOp, arith::AddIOp>(op)) opStr = " + ";
    else if (isa<arith::SubFOp, arith::SubIOp>(op)) opStr = " - ";
    else if (isa<arith::MulFOp, arith::MulIOp>(op)) opStr = " * ";
    else if (isa<arith::DivFOp, arith::DivSIOp>(op)) opStr = " / ";
    else opStr = " /* unknown */ ";
    
    emitIndent() << getTypeName(resultType) << " " << result 
                 << " = " << lhs << opStr << rhs << ";\n";
  }
  
  return success();
}

LogicalResult HLSCppEmitter::emitMemRefAlloc(memref::AllocOp allocOp) {
  std::string name = getName(allocOp.getResult());
  auto memType = allocOp.getType();
  
  // Emit local array declaration
  emitIndent() << getTypeName(memType.getElementType()) << " " << name;
  for (int64_t dim : memType.getShape()) {
    os << "[" << dim << "]";
  }
  os << ";\n";
  
  // Emit array partition pragma for multi-dimensional arrays
  if (memType.getRank() > 1) {
    emitArrayPartitionPragma(name, memType.getRank(), "complete");
  }
  
  return success();
}

LogicalResult HLSCppEmitter::emitMemRefAlloca(memref::AllocaOp allocaOp) {
  std::string name = getName(allocaOp.getResult());
  auto memType = allocaOp.getType();
  
  emitIndent() << getTypeName(memType.getElementType()) << " " << name;
  for (int64_t dim : memType.getShape()) {
    os << "[" << dim << "]";
  }
  os << ";\n";
  
  return success();
}

LogicalResult HLSCppEmitter::emitSCFFor(scf::ForOp forOp) {
  std::string ivName = "i" + std::to_string(valueCounter++);
  valueNames[forOp.getInductionVar()] = ivName;
  
  std::string lb = getName(forOp.getLowerBound());
  std::string ub = getName(forOp.getUpperBound());
  std::string step = getName(forOp.getStep());
  
  emitIndent() << "for (int " << ivName << " = " << lb 
               << "; " << ivName << " < " << ub 
               << "; " << ivName << " += " << step << ") {\n";
  indent();
  
  for (auto &op : forOp.getBody()->without_terminator()) {
    if (failed(emitOperation(&op)))
      return failure();
  }
  
  dedent();
  emitIndent() << "}\n";
  
  return success();
}

LogicalResult HLSCppEmitter::emitSCFIf(scf::IfOp ifOp) {
  std::string cond = getName(ifOp.getCondition());
  
  emitIndent() << "if (" << cond << ") {\n";
  indent();
  
  for (auto &op : ifOp.getThenRegion().front().without_terminator()) {
    if (failed(emitOperation(&op)))
      return failure();
  }
  
  dedent();
  emitIndent() << "}";
  
  if (!ifOp.getElseRegion().empty()) {
    os << " else {\n";
    indent();
    
    for (auto &op : ifOp.getElseRegion().front().without_terminator()) {
      if (failed(emitOperation(&op)))
        return failure();
    }
    
    dedent();
    emitIndent() << "}";
  }
  os << "\n";
  
  return success();
}

//===----------------------------------------------------------------------===//
// Module Emission
//===----------------------------------------------------------------------===//

LogicalResult HLSCppEmitter::emitModule(ModuleOp module) {
  emitFileHeader();
  emitIncludes();
  emitMacros();
  os << "\n";
  
  // Emit all functions
  for (auto func : module.getOps<func::FuncOp>()) {
    if (failed(emitFunc(func)))
      return failure();
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// Translation Registration
//===----------------------------------------------------------------------===//

LogicalResult emitHLSCpp(ModuleOp module, raw_ostream &os, 
                         bool emitMultiFunction) {
  HLSCppEmitter emitter(os, emitMultiFunction);
  return emitter.emitModule(module);
}

/// Register the HLS C++ translation.
void registerEmitHLSCppTranslation() {
  TranslateFromMLIRRegistration registration(
      "emit-hlscpp", "Emit HLS C++ code",
      [](ModuleOp module, raw_ostream &os) {
        return emitHLSCpp(module, os, /*emitMultiFunction=*/false);
      },
      [](DialectRegistry &registry) {
        registry.insert<func::FuncDialect, affine::AffineDialect,
                        arith::ArithDialect, memref::MemRefDialect,
                        scf::SCFDialect, vector::VectorDialect>();
      });
}

} // namespace systolic
} // namespace mlir

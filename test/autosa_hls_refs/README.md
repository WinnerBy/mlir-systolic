# AutoSA HLS Reference Files

This directory contains carefully organized AutoSA-generated HLS C++ kernel reference files with comprehensive parameter encoding in filenames for easy identification and testing.

## Organization

Files are organized with the following naming convention:

```
{kernel}_{problem_size}_{st}{space_time}_ap{array_parts}_lat{latencies}_simd{simd}.cpp
```

### Naming Components

- **kernel**: The computational kernel
  - `mm`: Matrix Multiplication
  - `cnn`: Convolutional Neural Network
  - `mttkrp`: Matricized Tensor Times Kronecker Root Pairing
  - `ttmc`: Tensor-Times-Matrix Chain

- **problem_size**: Input problem size
  - For MM: `512`, `1024` (matrix dimensions)
  - For CNN: `small`, `large`, `default`
  - For MTTKRP/TTMC: `small`, `medium`, `large`, `xlarge`, `default`

- **st[N]**: Space-time (ST) mode parameter
  - ST0-ST2: 1D systolic array (output/input-stationary)
  - ST3-ST5: 2D systolic array
  - ST6+: High-dimensional/special configurations

- **ap[D1xD2x...]**: Array Partitioning (PE dimensions)
  - Example: `ap8x4x4x8` = 8×4×4×8 PE array
  - Comma-separated values converted to 'x' for clarity

- **lat[D1xD2x...]**: Latency hiding factors
  - Controls initiation interval and pipeline depth
  - Example: `lat4x2x4` = [4,2,4] latency factors

- **simd[D1xD2x...]**: SIMD vectorization factors
  - Single dimension (MM): `simd2`, `simd4`, `simd8`
  - Multi-dimensional (CNN, MTTKRP): `simd1x1x1x2`, `simd2x2x2x4`

## File Statistics

- **Total Files**: 26 unique configurations
- **Total Lines of Code**: ~75K lines
- **Breakdown by Kernel**:
  - MM: 12 files (Matrix Multiplication)
  - CNN: 8 files (Convolution)
  - MTTKRP: 3 files (Tensor operations)
  - TTMC: 3 files (Tensor-Times-Matrix Chain)

## Usage Examples

### Example 1: Matrix Multiplication 512×512, ST3 Mode
```
mm_512_st3_ap16x16x16_lat8x8_simd2.cpp
```
- 512×512 matrix multiplication
- Space-time mode 3 (2D output-stationary)
- 16×16×16 PE array
- 8×8 latency factors
- 2× SIMD vectorization

### Example 2: CNN Small, ST4 Mode, Multiple SIMD Variants
```
cnn_small_st4_ap8x8x4x8_lat4x2x4_simd1x1x1x1.cpp  # No vectorization
cnn_small_st4_ap8x8x4x8_lat4x2x4_simd1x1x1x2.cpp  # Partial vectorization
cnn_small_st4_ap8x8x4x8_lat4x2x4_simd2x2x2x4.cpp  # Full vectorization
```

### Example 3: MTTKRP Large Configuration
```
mttkrp_large_st3_ap64x64x2_lat8x4_simd4x1.cpp
```
- Large tensor problem
- Space-time mode 3
- 64×64×2 PE array
- 8×4 latency factors
- 4×1 SIMD vectorization

## Parameter Combinations

The reference set covers diverse configurations:

- **Space-Time Modes**: ST0, ST1, ST2, ST3, ST4, ST5, ST7, ST8
- **Problem Sizes**: 512, 1024, small, medium, large, xlarge, default
- **Array Partitions**: 4-256 in each dimension (scalar or vectorized)
- **Latency Factors**: 1-32 (various combinations for II hiding)
- **SIMD Vectors**: 1-16× (including multi-dimensional factorization)

## File Format

Each `.cpp` file contains:
- HLS-synthesizable C++ kernels
- Systolic array dataflow architecture
- Optimized memory hierarchies
- PE functions with pipeline pragmas
- I/O modules for boundary conditions
- FIFO channels for inter-PE communication

## Extraction Method

These files were automatically extracted from `autosa_reference_samples/` using intelligent metadata parsing:
- Each sample directory contains `metadata.json` with all parameters
- HLS kernels extracted from `src/kernel_kernel.cpp`
- Filenames automatically generated from parameter metadata
- Duplicates eliminated for clean reference set

## Testing & Validation

These reference files can be used to:
1. Validate HLS synthesis results
2. Compare performance characteristics
3. Test different space-time modes
4. Explore parameter-space trade-offs
5. Generate HLS implementations with specific configurations

## Directory Structure

```
autosa_hls_refs/
├── README.md                                           (this file)
├── cnn_default_st0_ap8x4x4x8_lat4x2x4_simd1x1x1x2.cpp
├── cnn_default_st3_ap4x4x4x8_lat2x2x2_simd1x1x1x2.cpp
├── ... (26 files total)
└── ttmc_xlarge_st4_ap32x128x32x64_lat2x16x16_simd16x2.cpp
```

---

**Generated**: January 10, 2025
**Collection Method**: Metadata-driven extraction from autosa_reference_samples/
**Script**: collect_autosa_hls_refs.sh

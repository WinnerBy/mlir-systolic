# AutoSA 参考参数表（按 kernel 分组）

本表格基于 `autosa_hls_refs/generation_summary.json` 自动生成，展示每个参考代码的关键参数与路径。

注：路径为 `kernel_kernel.cpp` 所在文件相对路径。


## cnn（20 条）

| spacetime | array_part | latency | simd | path |

|-----------:|-----------|---------|------|------|

| 0 | 128,112,28,128 | 8,8,14 | 2,2,16 | autosa_hls_refs/cnn/st0_ap128,112,28,128_lat8,8,14_simd2,2,16/cnn_st0_ap128,112,28,128_lat8,8,14_simd2,2,16_kernel_kernel.cpp |
| 0 | 64,56,14,64 | 4,4,7 | 1,1,8 | autosa_hls_refs/cnn/st0_ap64,56,14,64_lat4,4,7_simd1,1,8/cnn_st0_ap64,56,14,64_lat4,4,7_simd1,1,8_kernel_kernel.cpp |
| 1 | 128,112,28,128 | 8,8,14 | 2,2,16 | autosa_hls_refs/cnn/st1_ap128,112,28,128_lat8,8,14_simd2,2,16/cnn_st1_ap128,112,28,128_lat8,8,14_simd2,2,16_kernel_kernel.cpp |
| 1 | 64,56,14,64 | 4,4,7 | 1,1,8 | autosa_hls_refs/cnn/st1_ap64,56,14,64_lat4,4,7_simd1,1,8/cnn_st1_ap64,56,14,64_lat4,4,7_simd1,1,8_kernel_kernel.cpp |
| 2 | 128,112,28,128 | 8,8,14 | 2,2,16 | autosa_hls_refs/cnn/st2_ap128,112,28,128_lat8,8,14_simd2,2,16/cnn_st2_ap128,112,28,128_lat8,8,14_simd2,2,16_kernel_kernel.cpp |
| 2 | 64,56,14,64 | 4,4,7 | 1,1,8 | autosa_hls_refs/cnn/st2_ap64,56,14,64_lat4,4,7_simd1,1,8/cnn_st2_ap64,56,14,64_lat4,4,7_simd1,1,8_kernel_kernel.cpp |
| 3 | 128,112,28,128 | 8,8,14 | 2,2,16 | autosa_hls_refs/cnn/st3_ap128,112,28,128_lat8,8,14_simd2,2,16/cnn_st3_ap128,112,28,128_lat8,8,14_simd2,2,16_kernel_kernel.cpp |
| 3 | 64,56,14,64 | 4,4,7 | 1,1,8 | autosa_hls_refs/cnn/st3_ap64,56,14,64_lat4,4,7_simd1,1,8/cnn_st3_ap64,56,14,64_lat4,4,7_simd1,1,8_kernel_kernel.cpp |
| 3 | 80,70,17,80 | 5,5,8 | 1,1,10 | autosa_hls_refs/cnn/st3_ap80,70,17,80_lat5,5,8_simd1,1,10/cnn_st3_ap80,70,17,80_lat5,5,8_simd1,1,10_kernel_kernel.cpp |
| 3 | 96,84,21,96 | 6,6,10 | 1,1,12 | autosa_hls_refs/cnn/st3_ap96,84,21,96_lat6,6,10_simd1,1,12/cnn_st3_ap96,84,21,96_lat6,6,10_simd1,1,12_kernel_kernel.cpp |
| 4 | 128,112,28,128 | 8,8,14 | 2,2,16 | autosa_hls_refs/cnn/st4_ap128,112,28,128_lat8,8,14_simd2,2,16/cnn_st4_ap128,112,28,128_lat8,8,14_simd2,2,16_kernel_kernel.cpp |
| 4 | 64,56,14,64 | 4,4,7 | 1,1,8 | autosa_hls_refs/cnn/st4_ap64,56,14,64_lat4,4,7_simd1,1,8/cnn_st4_ap64,56,14,64_lat4,4,7_simd1,1,8_kernel_kernel.cpp |
| 5 | 128,112,28,128 | 8,8,14 | 2,2,16 | autosa_hls_refs/cnn/st5_ap128,112,28,128_lat8,8,14_simd2,2,16/cnn_st5_ap128,112,28,128_lat8,8,14_simd2,2,16_kernel_kernel.cpp |
| 5 | 64,56,14,64 | 4,4,7 | 1,1,8 | autosa_hls_refs/cnn/st5_ap64,56,14,64_lat4,4,7_simd1,1,8/cnn_st5_ap64,56,14,64_lat4,4,7_simd1,1,8_kernel_kernel.cpp |
| 6 | 128,112,28,128 | 8,8,14 | 2,2,16 | autosa_hls_refs/cnn/st6_ap128,112,28,128_lat8,8,14_simd2,2,16/cnn_st6_ap128,112,28,128_lat8,8,14_simd2,2,16_kernel_kernel.cpp |
| 6 | 64,56,14,64 | 4,4,7 | 1,1,8 | autosa_hls_refs/cnn/st6_ap64,56,14,64_lat4,4,7_simd1,1,8/cnn_st6_ap64,56,14,64_lat4,4,7_simd1,1,8_kernel_kernel.cpp |
| 6 | 80,70,17,80 | 5,5,8 | 1,1,10 | autosa_hls_refs/cnn/st6_ap80,70,17,80_lat5,5,8_simd1,1,10/cnn_st6_ap80,70,17,80_lat5,5,8_simd1,1,10_kernel_kernel.cpp |
| 6 | 96,84,21,96 | 6,6,10 | 1,1,12 | autosa_hls_refs/cnn/st6_ap96,84,21,96_lat6,6,10_simd1,1,12/cnn_st6_ap96,84,21,96_lat6,6,10_simd1,1,12_kernel_kernel.cpp |
| 7 | 128,112,28,128 | 8,8,14 | 2,2,16 | autosa_hls_refs/cnn/st7_ap128,112,28,128_lat8,8,14_simd2,2,16/cnn_st7_ap128,112,28,128_lat8,8,14_simd2,2,16_kernel_kernel.cpp |
| 7 | 64,56,14,64 | 4,4,7 | 1,1,8 | autosa_hls_refs/cnn/st7_ap64,56,14,64_lat4,4,7_simd1,1,8/cnn_st7_ap64,56,14,64_lat4,4,7_simd1,1,8_kernel_kernel.cpp |

## lu（10 条）

| spacetime | array_part | latency | simd | path |

|-----------:|-----------|---------|------|------|

| 0 | -1,-1,-1 | 4,4 | 1,1 | autosa_hls_refs/lu/st0_ap-1,-1,-1_lat4,4_simd1,1/lu_st0_ap-1,-1,-1_lat4,4_simd1,1_kernel_kernel.cpp |
| 1 | -1,-1,-1 | 4,4 | 1,1 | autosa_hls_refs/lu/st1_ap-1,-1,-1_lat4,4_simd1,1/lu_st1_ap-1,-1,-1_lat4,4_simd1,1_kernel_kernel.cpp |
| 1 | -1,-1,-1 | 8,8 | 2,2 | autosa_hls_refs/lu/st1_ap-1,-1,-1_lat8,8_simd2,2/lu_st1_ap-1,-1,-1_lat8,8_simd2,2_kernel_host.cpp |
| 2 | -1,-1,-1 | 4,4 | 1,1 | autosa_hls_refs/lu/st2_ap-1,-1,-1_lat4,4_simd1,1/lu_st2_ap-1,-1,-1_lat4,4_simd1,1_kernel_kernel.cpp |
| 2 | -1,-1,-1 | 8,8 | 2,2 | autosa_hls_refs/lu/st2_ap-1,-1,-1_lat8,8_simd2,2/lu_st2_ap-1,-1,-1_lat8,8_simd2,2_kernel_kernel.cpp |
| 3 | -1,-1,-1 | 4,4 | 1,1 | autosa_hls_refs/lu/st3_ap-1,-1,-1_lat4,4_simd1,1/lu_st3_ap-1,-1,-1_lat4,4_simd1,1_kernel_kernel.cpp |
| 3 | -1,-1,-1 | 8,8 | 2,2 | autosa_hls_refs/lu/st3_ap-1,-1,-1_lat8,8_simd2,2/lu_st3_ap-1,-1,-1_lat8,8_simd2,2_kernel_kernel.cpp |
| 4 | -1,-1,-1 | 4,4 | 1,1 | autosa_hls_refs/lu/st4_ap-1,-1,-1_lat4,4_simd1,1/lu_st4_ap-1,-1,-1_lat4,4_simd1,1_kernel_kernel.cpp |
| 5 | -1,-1,-1 | 4,4 | 1,1 | autosa_hls_refs/lu/st5_ap-1,-1,-1_lat4,4_simd1,1/lu_st5_ap-1,-1,-1_lat4,4_simd1,1_kernel_kernel.cpp |
| 5 | -1,-1,-1 | 8,8 | 2,2 | autosa_hls_refs/lu/st5_ap-1,-1,-1_lat8,8_simd2,2/lu_st5_ap-1,-1,-1_lat8,8_simd2,2_kernel_host.cpp |

## mm（18 条）

| spacetime | array_part | latency | simd | path |

|-----------:|-----------|---------|------|------|

| 0 | 16,16,16 | 8,8 | 2 | autosa_hls_refs/mm/st0_ap16,16,16_lat8,8_simd2/mm_st0_ap16,16,16_lat8,8_simd2_kernel_kernel.cpp |
| 0 | 32,32,32 | 16,16 | 4 | autosa_hls_refs/mm/st0_ap32,32,32_lat16,16_simd4/mm_st0_ap32,32,32_lat16,16_simd4_kernel_kernel.cpp |
| 0 | 64,64,64 | 32,32 | 8 | autosa_hls_refs/mm/st0_ap64,64,64_lat32,32_simd8/mm_st0_ap64,64,64_lat32,32_simd8_kernel_kernel.cpp |
| 1 | 16,16,16 | 8,8 | 2 | autosa_hls_refs/mm/st1_ap16,16,16_lat8,8_simd2/mm_st1_ap16,16,16_lat8,8_simd2_kernel_kernel.cpp |
| 1 | 32,32,32 | 16,16 | 4 | autosa_hls_refs/mm/st1_ap32,32,32_lat16,16_simd4/mm_st1_ap32,32,32_lat16,16_simd4_kernel_kernel.cpp |
| 1 | 64,64,64 | 32,32 | 8 | autosa_hls_refs/mm/st1_ap64,64,64_lat32,32_simd8/mm_st1_ap64,64,64_lat32,32_simd8_kernel_kernel.cpp |
| 2 | 16,16,16 | 8,8 | 2 | autosa_hls_refs/mm/st2_ap16,16,16_lat8,8_simd2/mm_st2_ap16,16,16_lat8,8_simd2_kernel_kernel.cpp |
| 2 | 32,32,32 | 16,16 | 4 | autosa_hls_refs/mm/st2_ap32,32,32_lat16,16_simd4/mm_st2_ap32,32,32_lat16,16_simd4_kernel_kernel.cpp |
| 2 | 64,64,64 | 32,32 | 8 | autosa_hls_refs/mm/st2_ap64,64,64_lat32,32_simd8/mm_st2_ap64,64,64_lat32,32_simd8_kernel_kernel.cpp |
| 3 | 16,16,16 | 8,8 | 2 | autosa_hls_refs/mm/st3_ap16,16,16_lat8,8_simd2/mm_st3_ap16,16,16_lat8,8_simd2_kernel_kernel.cpp |
| 3 | 32,32,32 | 16,16 | 4 | autosa_hls_refs/mm/st3_ap32,32,32_lat16,16_simd4/mm_st3_ap32,32,32_lat16,16_simd4_kernel_kernel.cpp |
| 3 | 64,64,64 | 32,32 | 8 | autosa_hls_refs/mm/st3_ap64,64,64_lat32,32_simd8/mm_st3_ap64,64,64_lat32,32_simd8_kernel_kernel.cpp |
| 4 | 16,16,16 | 8,8 | 2 | autosa_hls_refs/mm/st4_ap16,16,16_lat8,8_simd2/mm_st4_ap16,16,16_lat8,8_simd2_kernel_kernel.cpp |
| 4 | 32,32,32 | 16,16 | 4 | autosa_hls_refs/mm/st4_ap32,32,32_lat16,16_simd4/mm_st4_ap32,32,32_lat16,16_simd4_kernel_kernel.cpp |
| 4 | 64,64,64 | 32,32 | 8 | autosa_hls_refs/mm/st4_ap64,64,64_lat32,32_simd8/mm_st4_ap64,64,64_lat32,32_simd8_kernel_kernel.cpp |
| 5 | 16,16,16 | 8,8 | 2 | autosa_hls_refs/mm/st5_ap16,16,16_lat8,8_simd2/mm_st5_ap16,16,16_lat8,8_simd2_kernel_kernel.cpp |
| 5 | 32,32,32 | 16,16 | 4 | autosa_hls_refs/mm/st5_ap32,32,32_lat16,16_simd4/mm_st5_ap32,32,32_lat16,16_simd4_kernel_kernel.cpp |
| 5 | 64,64,64 | 32,32 | 8 | autosa_hls_refs/mm/st5_ap64,64,64_lat32,32_simd8/mm_st5_ap64,64,64_lat32,32_simd8_kernel_kernel.cpp |

## mttkrp（22 条）

| spacetime | array_part | latency | simd | path |

|-----------:|-----------|---------|------|------|

| 0 | 128,128,2 | 16,8 | 8,1 | autosa_hls_refs/mttkrp/st0_ap128,128,2_lat16,8_simd8,1/mttkrp_st0_ap128,128,2_lat16,8_simd8,1_kernel_kernel.cpp |
| 0 | 256,256,4 | 32,16 | 16,2 | autosa_hls_refs/mttkrp/st0_ap256,256,4_lat32,16_simd16,2/mttkrp_st0_ap256,256,4_lat32,16_simd16,2_kernel_kernel.cpp |
| 0 | 32,32,2 | 4,2 | 2,1 | autosa_hls_refs/mttkrp/st0_ap32,32,2_lat4,2_simd2,1/mttkrp_st0_ap32,32,2_lat4,2_simd2,1_kernel_kernel.cpp |
| 0 | 64,64,2 | 8,4 | 4,1 | autosa_hls_refs/mttkrp/st0_ap64,64,2_lat8,4_simd4,1/mttkrp_st0_ap64,64,2_lat8,4_simd4,1_kernel_kernel.cpp |
| 1 | 128,128,2 | 16,8 | 8,1 | autosa_hls_refs/mttkrp/st1_ap128,128,2_lat16,8_simd8,1/mttkrp_st1_ap128,128,2_lat16,8_simd8,1_kernel_kernel.cpp |
| 1 | 256,256,4 | 32,16 | 16,2 | autosa_hls_refs/mttkrp/st1_ap256,256,4_lat32,16_simd16,2/mttkrp_st1_ap256,256,4_lat32,16_simd16,2_kernel_kernel.cpp |
| 1 | 32,32,2 | 4,2 | 2,1 | autosa_hls_refs/mttkrp/st1_ap32,32,2_lat4,2_simd2,1/mttkrp_st1_ap32,32,2_lat4,2_simd2,1_kernel_kernel.cpp |
| 1 | 64,64,2 | 8,4 | 4,1 | autosa_hls_refs/mttkrp/st1_ap64,64,2_lat8,4_simd4,1/mttkrp_st1_ap64,64,2_lat8,4_simd4,1_kernel_kernel.cpp |
| 2 | 128,128,2 | 16,8 | 8,1 | autosa_hls_refs/mttkrp/st2_ap128,128,2_lat16,8_simd8,1/mttkrp_st2_ap128,128,2_lat16,8_simd8,1_kernel_kernel.cpp |
| 2 | 256,256,4 | 32,16 | 16,2 | autosa_hls_refs/mttkrp/st2_ap256,256,4_lat32,16_simd16,2/mttkrp_st2_ap256,256,4_lat32,16_simd16,2_kernel_kernel.cpp |
| 2 | 32,32,2 | 4,2 | 2,1 | autosa_hls_refs/mttkrp/st2_ap32,32,2_lat4,2_simd2,1/mttkrp_st2_ap32,32,2_lat4,2_simd2,1_kernel_kernel.cpp |
| 2 | 64,64,2 | 8,4 | 4,1 | autosa_hls_refs/mttkrp/st2_ap64,64,2_lat8,4_simd4,1/mttkrp_st2_ap64,64,2_lat8,4_simd4,1_kernel_kernel.cpp |
| 3 | 128,128,2 | 16,8 | 8,1 | autosa_hls_refs/mttkrp/st3_ap128,128,2_lat16,8_simd8,1/mttkrp_st3_ap128,128,2_lat16,8_simd8,1_kernel_kernel.cpp |
| 3 | 256,256,4 | 32,16 | 16,2 | autosa_hls_refs/mttkrp/st3_ap256,256,4_lat32,16_simd16,2/mttkrp_st3_ap256,256,4_lat32,16_simd16,2_kernel_kernel.cpp |
| 3 | 32,32,2 | 4,2 | 2,1 | autosa_hls_refs/mttkrp/st3_ap32,32,2_lat4,2_simd2,1/mttkrp_st3_ap32,32,2_lat4,2_simd2,1_kernel_kernel.cpp |
| 3 | 64,64,2 | 8,4 | 4,1 | autosa_hls_refs/mttkrp/st3_ap64,64,2_lat8,4_simd4,1/mttkrp_st3_ap64,64,2_lat8,4_simd4,1_kernel_kernel.cpp |
| 4 | 128,128,2 | 16,8 | 8,1 | autosa_hls_refs/mttkrp/st4_ap128,128,2_lat16,8_simd8,1/mttkrp_st4_ap128,128,2_lat16,8_simd8,1_kernel_kernel.cpp |
| 4 | 256,256,4 | 32,16 | 16,2 | autosa_hls_refs/mttkrp/st4_ap256,256,4_lat32,16_simd16,2/mttkrp_st4_ap256,256,4_lat32,16_simd16,2_kernel_kernel.cpp |
| 4 | 64,64,2 | 8,4 | 4,1 | autosa_hls_refs/mttkrp/st4_ap64,64,2_lat8,4_simd4,1/mttkrp_st4_ap64,64,2_lat8,4_simd4,1_kernel_kernel.cpp |
| 5 | 128,128,2 | 16,8 | 8,1 | autosa_hls_refs/mttkrp/st5_ap128,128,2_lat16,8_simd8,1/mttkrp_st5_ap128,128,2_lat16,8_simd8,1_kernel_kernel.cpp |
| 5 | 256,256,4 | 32,16 | 16,2 | autosa_hls_refs/mttkrp/st5_ap256,256,4_lat32,16_simd16,2/mttkrp_st5_ap256,256,4_lat32,16_simd16,2_kernel_kernel.cpp |
| 5 | 64,64,2 | 8,4 | 4,1 | autosa_hls_refs/mttkrp/st5_ap64,64,2_lat8,4_simd4,1/mttkrp_st5_ap64,64,2_lat8,4_simd4,1_kernel_kernel.cpp |

## ttmc（29 条）

| spacetime | array_part | latency | simd | path |

|-----------:|-----------|---------|------|------|

| 0 | 128,128,128,128 | 8,16,16 | 8,8 | autosa_hls_refs/ttmc/st0_ap128,128,128,128_lat8,16,16_simd8,8/ttmc_st0_ap128,128,128,128_lat8,16,16_simd8,8_kernel_kernel.cpp |
| 0 | 16,16,16,16 | 1,2,2 | 1,1 | autosa_hls_refs/ttmc/st0_ap16,16,16,16_lat1,2,2_simd1,1/ttmc_st0_ap16,16,16,16_lat1,2,2_simd1,1_kernel_kernel.cpp |
| 0 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st0_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st0_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 0 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st0_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st0_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
| 1 | 16,16,16,16 | 1,2,2 | 1,1 | autosa_hls_refs/ttmc/st1_ap16,16,16,16_lat1,2,2_simd1,1/ttmc_st1_ap16,16,16,16_lat1,2,2_simd1,1_kernel_kernel.cpp |
| 1 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st1_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st1_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 1 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st1_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st1_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
| 2 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st2_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st2_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 2 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st2_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st2_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
| 3 | 128,128,128,128 | 8,16,16 | 8,8 | autosa_hls_refs/ttmc/st3_ap128,128,128,128_lat8,16,16_simd8,8/ttmc_st3_ap128,128,128,128_lat8,16,16_simd8,8_kernel_kernel.cpp |
| 3 | 16,16,16,16 | 1,2,2 | 1,1 | autosa_hls_refs/ttmc/st3_ap16,16,16,16_lat1,2,2_simd1,1/ttmc_st3_ap16,16,16,16_lat1,2,2_simd1,1_kernel_kernel.cpp |
| 3 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st3_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st3_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 3 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st3_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st3_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
| 4 | 128,128,128,128 | 8,16,16 | 8,8 | autosa_hls_refs/ttmc/st4_ap128,128,128,128_lat8,16,16_simd8,8/ttmc_st4_ap128,128,128,128_lat8,16,16_simd8,8_kernel_kernel.cpp |
| 4 | 16,16,16,16 | 1,2,2 | 1,1 | autosa_hls_refs/ttmc/st4_ap16,16,16,16_lat1,2,2_simd1,1/ttmc_st4_ap16,16,16,16_lat1,2,2_simd1,1_kernel_kernel.cpp |
| 4 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st4_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st4_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 4 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st4_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st4_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
| 5 | 128,128,128,128 | 8,16,16 | 8,8 | autosa_hls_refs/ttmc/st5_ap128,128,128,128_lat8,16,16_simd8,8/ttmc_st5_ap128,128,128,128_lat8,16,16_simd8,8_kernel_kernel.cpp |
| 5 | 16,16,16,16 | 1,2,2 | 1,1 | autosa_hls_refs/ttmc/st5_ap16,16,16,16_lat1,2,2_simd1,1/ttmc_st5_ap16,16,16,16_lat1,2,2_simd1,1_kernel_kernel.cpp |
| 5 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st5_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st5_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 5 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st5_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st5_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
| 6 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st6_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st6_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 6 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st6_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st6_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
| 7 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st7_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st7_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 7 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st7_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st7_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
| 8 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st8_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st8_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 8 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st8_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st8_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
| 9 | 32,32,32,32 | 2,4,4 | 2,2 | autosa_hls_refs/ttmc/st9_ap32,32,32,32_lat2,4,4_simd2,2/ttmc_st9_ap32,32,32,32_lat2,4,4_simd2,2_kernel_kernel.cpp |
| 9 | 64,64,64,64 | 4,8,8 | 4,4 | autosa_hls_refs/ttmc/st9_ap64,64,64,64_lat4,8,8_simd4,4/ttmc_st9_ap64,64,64,64_lat4,8,8_simd4,4_kernel_kernel.cpp |
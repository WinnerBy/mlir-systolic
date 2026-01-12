// AutoSA Command:
// /workspaces/mlir-systolic/third_party/AutoSA/autosa /workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/ttmc/kernel.c --config=/workspaces/mlir-systolic/third_party/AutoSA/autosa_config/autosa_config.json --target=autosa_hls_c --output-dir=/workspaces/mlir-systolic/third_party/AutoSA/autosa.tmp/output --host-serialize --simd-info=/workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/ttmc/simd_info.json --sa-sizes={kernel[]->space_time[0];kernel[]->array_part[128,128,128,128];kernel[]->latency[8,16,16];kernel[]->simd[8,8]}

#include <ap_int.h>
#include <hls_stream.h>

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Data Type */
typedef float A_t1;
typedef float B_t1;
typedef float C_t1;
typedef float D_t1;
typedef ap_uint<512> A_t16;
typedef ap_uint<256> A_t8;
typedef ap_uint<512> B_t16;
typedef ap_uint<512> C_t16;
typedef ap_uint<256> C_t8;
typedef ap_uint<512> D_t16;
typedef ap_uint<128> D_t4;
/* Data Type */

extern "C" {
void kernel0(A_t16 *A, B_t16 *B, C_t16 *C, D_t16 *D);
}
void A_IO_L1_in_intra_trans(int idx, A_t8 local_A[8][128][16], hls::stream<A_t8> &fifo_A_local_out, bool intra_trans_en);
void A_IO_L1_in_inter_trans(int idx, A_t8 local_A[8][128][16], hls::stream<A_t8> &fifo_A_in, hls::stream<A_t8> &fifo_A_out, bool inter_trans_en);
void A_IO_L1_in_inter_trans_boundary(int idx, A_t8 local_A[8][128][16], hls::stream<A_t8> &fifo_A_in, bool inter_trans_en);
void A_IO_L1_in_wrapper(int idx, hls::stream<A_t8> &fifo_A_in, hls::stream<A_t8> &fifo_A_out, hls::stream<A_t8> &fifo_A_local_out);
void A_IO_L1_in_boundary_wrapper(int idx, hls::stream<A_t8> &fifo_A_in, hls::stream<A_t8> &fifo_A_local_out);
void B_IO_L2_in_intra_trans(B_t16 local_B[128][8], hls::stream<float> &fifo_B_local_out, bool intra_trans_en);
void B_IO_L2_in_inter_trans_boundary(B_t16 local_B[128][8], hls::stream<B_t16> &fifo_B_in, bool inter_trans_en);
void C_IO_L2_in_intra_trans(C_t16 local_C[128][8], hls::stream<C_t8> &fifo_C_local_out, bool intra_trans_en);
void C_IO_L2_in_inter_trans_boundary(C_t16 local_C[128][8], hls::stream<C_t16> &fifo_C_in, bool inter_trans_en);
void PE_wrapper(int idx, hls::stream<A_t8> &fifo_A_in, hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, hls::stream<C_t8> &fifo_C_in, hls::stream<C_t8> &fifo_C_out, hls::stream<float> &fifo_D_drain_out);
void D_drain_IO_L1_out_intra_trans(int idx, D_t4 local_D[8][128][32], hls::stream<float> &fifo_D_drain_local_in);
void D_drain_IO_L1_out_inter_trans(int idx, D_t4 local_D[8][128][32], hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out);
void D_drain_IO_L1_out_inter_trans_boundary(int idx, D_t4 local_D[8][128][32], hls::stream<D_t4> &fifo_D_drain_out);
void D_drain_IO_L1_out_wrapper(int idx, hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in);
void D_drain_IO_L1_out_boundary_wrapper(int idx, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in);

/* Module Definition */
void A_IO_L2_in(hls::stream<A_t8> &fifo_A_in, hls::stream<A_t8> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L2
  for (ap_uint<5> c4 = 0; c4 <= 15; c4 += 1) {
    // io_L1
    for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
      for (ap_uint<8> c6 = 0; c6 <= 127; c6 += 1) {
        // access_coalesce
        // access_serialize
        for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t8 in_data;
            A_t8 out_data;
            in_data = fifo_A_in.read();
            out_data = in_data;
            fifo_A_local_out.write(out_data);
          }
        }
      }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in_serialize(A_t16 *A, hls::stream<A_t8> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  A_t8 fifo_data;
  A_t16 mem_data;
  for (ap_uint<18> i = 0; i < 131072; i++) {
  #pragma HLS PIPELINE II=1
    mem_data = A[i];
    for (ap_uint<2> p = 0; p < 2; p++) {
      fifo_data = mem_data(255, 0);
      mem_data = mem_data >> 256;
      fifo_A_local_out.write(fifo_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_intra_trans(int idx, A_t8 local_A[8][128][16], hls::stream<A_t8> &fifo_A_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // array
  // io_L2
  // io_L1
  // pe
  for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
      for (ap_uint<8> c7 = 0; c7 <= 127; c7 += 1)
        for (ap_uint<5> c8 = 0; c8 <= 15; c8 += 1) {
          // latency
          for (ap_uint<5> c9 = 0; c9 <= 15; c9 += 1) {
            // latency
            for (ap_uint<5> c10 = 0; c10 <= 15; c10 += 1) {
              // latency
              for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
              #pragma HLS PIPELINE II=1
                // simd
                {
                  A_t8 in_data;
                  A_t8 out_data;
                  in_data = local_A[c11][c7][c8];
                  out_data = in_data;
                  fifo_A_local_out.write(out_data);
                }
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_inter_trans(int idx, A_t8 local_A[8][128][16], hls::stream<A_t8> &fifo_A_in, hls::stream<A_t8> &fifo_A_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<5> c4 = p0; c4 <= 15; c4 += 1) {
    // io_L1
    if (c4 == p0) {
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
        for (ap_uint<8> c6 = 0; c6 <= 127; c6 += 1) {
          // access_coalesce
          for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
          #pragma HLS PIPELINE II=1
            {
              A_t8 in_data;
              A_t8 out_data;
              in_data = fifo_A_in.read();
              out_data = in_data;
              local_A[c5][c6][c7] = out_data;
            }
          }
        }
    } else {
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
        for (ap_uint<8> c6 = 0; c6 <= 127; c6 += 1) {
          // access_coalesce
          for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
          #pragma HLS PIPELINE II=1
            {
              A_t8 in_data;
              A_t8 out_data;
              in_data = fifo_A_in.read();
              out_data = in_data;
              fifo_A_out.write(out_data);
            }
          }
        }
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_inter_trans_boundary(int idx, A_t8 local_A[8][128][16], hls::stream<A_t8> &fifo_A_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<5> c4 = p0; c4 <= 15; c4 += 1)
    if (c4 == p0) {
      // io_L1
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
        for (ap_uint<8> c6 = 0; c6 <= 127; c6 += 1) {
          // access_coalesce
          for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
          #pragma HLS PIPELINE II=1
            {
              A_t8 in_data;
              A_t8 out_data;
              in_data = fifo_A_in.read();
              out_data = in_data;
              local_A[c5][c6][c7] = out_data;
            }
          }
        }
    }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in(int idx, hls::stream<A_t8> &fifo_A_in, hls::stream<A_t8> &fifo_A_out, hls::stream<A_t8> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t8 local_A_ping[8][128][16];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_1P_BRAM
  A_t8 local_A_pong[8][128][16];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  /* Variable Declaration */

  {
    // array
    // io_L2
    {
      if (arb == 0) {
        A_IO_L1_in_inter_trans(
          /* module id */ idx, 
          /* array */ local_A_pong, 
          /* fifo */ fifo_A_in, 
          /* fifo */ fifo_A_out, 
          /* enable */ inter_trans_en
        );
        A_IO_L1_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_A_ping, 
          /* fifo */ fifo_A_local_out, 
          /* enable */ intra_trans_en
        );
      } else {
        A_IO_L1_in_inter_trans(
          /* module id */ idx, 
          /* array */ local_A_ping, 
          /* fifo */ fifo_A_in, 
          /* fifo */ fifo_A_out, 
          /* enable */ inter_trans_en
        );
        A_IO_L1_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_A_pong, 
          /* fifo */ fifo_A_local_out, 
          /* enable */ intra_trans_en
        );
      }
      intra_trans_en = 1;
      arb = !arb;
    }
    if (arb == 0) {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_A_ping, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_A_pong, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_wrapper(int idx, hls::stream<A_t8> &fifo_A_in, hls::stream<A_t8> &fifo_A_out, hls::stream<A_t8> &fifo_A_local_out)
 {
  A_IO_L1_in(
    /* module id */ idx, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_out, 
    /* fifo */ fifo_A_local_out);
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_boundary(int idx, hls::stream<A_t8> &fifo_A_in, hls::stream<A_t8> &fifo_A_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t8 local_A_ping[8][128][16];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_1P_BRAM
  A_t8 local_A_pong[8][128][16];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  /* Variable Declaration */

  {
    // array
    // io_L2
    {
      if (arb == 0) {
        A_IO_L1_in_inter_trans_boundary(
          /* module id */ idx, 
          /* array */ local_A_pong, 
          /* fifo */ fifo_A_in, 
          /* enable */ inter_trans_en
        );
        A_IO_L1_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_A_ping, 
          /* fifo */ fifo_A_local_out, 
          /* enable */ intra_trans_en
        );
      } else {
        A_IO_L1_in_inter_trans_boundary(
          /* module id */ idx, 
          /* array */ local_A_ping, 
          /* fifo */ fifo_A_in, 
          /* enable */ inter_trans_en
        );
        A_IO_L1_in_intra_trans(
          /* module id */ idx, 
          /* array */ local_A_pong, 
          /* fifo */ fifo_A_local_out, 
          /* enable */ intra_trans_en
        );
      }
      intra_trans_en = 1;
      arb = !arb;
    }
    if (arb == 0) {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_A_ping, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* array */ local_A_pong, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_boundary_wrapper(int idx, hls::stream<A_t8> &fifo_A_in, hls::stream<A_t8> &fifo_A_local_out)
 {
  A_IO_L1_in_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_local_out);
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_intra_trans(B_t16 local_B[128][8], hls::stream<float> &fifo_B_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  ap_uint<32> data_split[16];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
      for (ap_uint<8> c7 = 0; c7 <= 127; c7 += 1)
        for (ap_uint<5> c8 = 0; c8 <= 15; c8 += 1) {
          // latency
          for (ap_uint<5> c9 = 0; c9 <= 15; c9 += 1) {
            // latency
            for (ap_uint<5> c10 = 0; c10 <= 15; c10 += 1) {
              // latency
              for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
              #pragma HLS PIPELINE II=1
                // simd
                {
                  B_t16 in_data;
                  B_t1 out_data;
                  in_data = local_B[c7][(16 * c5 + c10) / 16];
                  for (ap_uint<5> n = 0; n < 16; n++) {
                  #pragma HLS UNROLL
                    data_split[n] = in_data(31, 0);
                    in_data = in_data >> 32;
                  }
                  int split_idx = (c10 + 16*c5) % 16;
                  union {unsigned int ui; float ut;} u;
                  u.ui = (unsigned int)data_split[split_idx];
                  out_data = u.ut;
                  fifo_B_local_out.write(out_data);
                }
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_inter_trans_boundary(B_t16 local_B[128][8], hls::stream<B_t16> &fifo_B_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  if (!inter_trans_en) return;

  // io_L2
  for (ap_uint<8> c4 = 0; c4 <= 127; c4 += 1) {
    // access_coalesce
    // access_serialize
    for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
    #pragma HLS PIPELINE II=1
      {
        B_t16 in_data;
        B_t16 out_data;
        in_data = fifo_B_in.read();
        out_data = in_data;
        local_B[c4][c5] = out_data;
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_boundary(hls::stream<B_t16> &fifo_B_in, hls::stream<float> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  B_t16 local_B_ping[128][8];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_2P_BRAM
  B_t16 local_B_pong[128][8];
  #pragma HLS RESOURCE variable=local_B_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  /* Variable Declaration */

  {
    // array
    {
      if (arb == 0) {
        B_IO_L2_in_inter_trans_boundary(
          /* array */ local_B_pong, 
          /* fifo */ fifo_B_in, 
          /* enable */ inter_trans_en
        );
        B_IO_L2_in_intra_trans(
          /* array */ local_B_ping, 
          /* fifo */ fifo_B_local_out, 
          /* enable */ intra_trans_en
        );
      } else {
        B_IO_L2_in_inter_trans_boundary(
          /* array */ local_B_ping, 
          /* fifo */ fifo_B_in, 
          /* enable */ inter_trans_en
        );
        B_IO_L2_in_intra_trans(
          /* array */ local_B_pong, 
          /* fifo */ fifo_B_local_out, 
          /* enable */ intra_trans_en
        );
      }
      intra_trans_en = 1;
      arb = !arb;
    }
    if (arb == 0) {
      B_IO_L2_in_intra_trans(
        /* array */ local_B_ping, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      B_IO_L2_in_intra_trans(
        /* array */ local_B_pong, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_boundary_serialize(B_t16 *B, hls::stream<B_t16> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<11> i = 0; i < 1024; i++) {
  #pragma HLS PIPELINE II=1
    B_t16 fifo_data;
    fifo_data = B[i];
    fifo_B_local_out.write(fifo_data);
  }
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_in_intra_trans(C_t16 local_C[128][8], hls::stream<C_t8> &fifo_C_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  C_t8 data_split[2];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
      for (ap_uint<8> c7 = 0; c7 <= 127; c7 += 1)
        for (ap_uint<5> c8 = 0; c8 <= 15; c8 += 1) {
          // latency
          for (ap_uint<5> c9 = 0; c9 <= 15; c9 += 1) {
            // latency
            for (ap_uint<5> c10 = 0; c10 <= 15; c10 += 1) {
              // latency
              for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
              #pragma HLS PIPELINE II=1
                // simd
                {
                  C_t16 in_data;
                  C_t8 out_data;
                  in_data = local_C[16*c6 + c9][8 * c8 / 16];
                  for (ap_uint<2> n = 0; n < 2; n++) {
                  #pragma HLS UNROLL
                    data_split[n] = in_data(255, 0);
                    in_data = in_data >> 256;
                  }
                  int split_idx = (c8) % 2;
                  out_data = data_split[split_idx];
                  fifo_C_local_out.write(out_data);
                }
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_in_inter_trans_boundary(C_t16 local_C[128][8], hls::stream<C_t16> &fifo_C_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  if (!inter_trans_en) return;

  // io_L2
  for (ap_uint<8> c4 = 0; c4 <= 127; c4 += 1) {
    // access_coalesce
    // access_serialize
    for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
    #pragma HLS PIPELINE II=1
      {
        C_t16 in_data;
        C_t16 out_data;
        in_data = fifo_C_in.read();
        out_data = in_data;
        local_C[c4][c5] = out_data;
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_in_boundary(hls::stream<C_t16> &fifo_C_in, hls::stream<C_t8> &fifo_C_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  C_t16 local_C_ping[128][8];
  #pragma HLS RESOURCE variable=local_C_ping core=RAM_2P_BRAM
  C_t16 local_C_pong[128][8];
  #pragma HLS RESOURCE variable=local_C_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  /* Variable Declaration */

  {
    // array
    {
      if (arb == 0) {
        C_IO_L2_in_inter_trans_boundary(
          /* array */ local_C_pong, 
          /* fifo */ fifo_C_in, 
          /* enable */ inter_trans_en
        );
        C_IO_L2_in_intra_trans(
          /* array */ local_C_ping, 
          /* fifo */ fifo_C_local_out, 
          /* enable */ intra_trans_en
        );
      } else {
        C_IO_L2_in_inter_trans_boundary(
          /* array */ local_C_ping, 
          /* fifo */ fifo_C_in, 
          /* enable */ inter_trans_en
        );
        C_IO_L2_in_intra_trans(
          /* array */ local_C_pong, 
          /* fifo */ fifo_C_local_out, 
          /* enable */ intra_trans_en
        );
      }
      intra_trans_en = 1;
      arb = !arb;
    }
    if (arb == 0) {
      C_IO_L2_in_intra_trans(
        /* array */ local_C_ping, 
        /* fifo */ fifo_C_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      C_IO_L2_in_intra_trans(
        /* array */ local_C_pong, 
        /* fifo */ fifo_C_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_in_boundary_serialize(C_t16 *C, hls::stream<C_t16> &fifo_C_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<11> i = 0; i < 1024; i++) {
  #pragma HLS PIPELINE II=1
    C_t16 fifo_data;
    fifo_data = C[i];
    fifo_C_local_out.write(fifo_data);
  }
}
/* Module Definition */

/* Module Definition */
void PE(int idx, hls::stream<A_t8> &fifo_A_in, hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, hls::stream<C_t8> &fifo_C_in, hls::stream<C_t8> &fifo_C_out, hls::stream<float> &fifo_D_drain_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t1 local_A[1][1][8];
  #pragma HLS ARRAY_PARTITION variable=local_A dim=0 complete
  B_t1 local_B[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_B dim=0 complete
  C_t1 local_C[1][8];
  #pragma HLS ARRAY_PARTITION variable=local_C dim=0 complete
  D_t1 local_D[8][128][128];
  #pragma HLS RESOURCE variable=local_D core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // pe
  for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
      // latency
      for (ap_uint<5> c8 = 0; c8 <= 15; c8 += 1) {
        // latency
        for (ap_uint<5> c9 = 0; c9 <= 15; c9 += 1) {
          // latency
          for (ap_uint<4> c10 = 0; c10 <= 7; c10 += 1) {
          #pragma HLS PIPELINE II=1
            local_D[c10][16*c5 + c9][16*c6 + c8] = 0;
          }
        }
      }
      for (ap_uint<8> c7 = 0; c7 <= 127; c7 += 1)
        for (ap_uint<5> c8 = 0; c8 <= 15; c8 += 1) {
          // latency
          for (ap_uint<5> c9 = 0; c9 <= 15; c9 += 1) {
            // latency
            for (ap_uint<5> c10 = 0; c10 <= 15; c10 += 1) {
              // latency
              for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
              #pragma HLS PIPELINE II=1
                {
                  {
                    A_t8 fifo_data;
                    fifo_data = fifo_A_in.read();
                    for (ap_uint<4> n = 0; n < 8; n++) {
                    #pragma HLS UNROLL
                      union {unsigned int ui; float ut;} u;
                      u.ui = (unsigned int)fifo_data(31, 0);
                      local_A[0][0][n] = u.ut;
                      fifo_data = fifo_data >> 32;
                    }
                  }
                  local_B[0][0] = fifo_B_in.read();
                  {
                    C_t8 fifo_data;
                    fifo_data = fifo_C_in.read();
                    for (ap_uint<4> n = 0; n < 8; n++) {
                    #pragma HLS UNROLL
                      union {unsigned int ui; float ut;} u;
                      u.ui = (unsigned int)fifo_data(31, 0);
                      local_C[0][n] = u.ut;
                      fifo_data = fifo_data >> 32;
                    }
                  }
                  // simd
                  for (ap_uint<4> c12 = 0; c12 <= 7; c12 += 1) {
                  #pragma HLS UNROLL
                    local_D[c11][c10 + 16*c5][16*c6 + c9] = (local_D[c11][c10 + 16*c5][16*c6 + c9] + ((local_A[0][0][c12] * local_B[0][0]) * local_C[0][c12]));
                  }
                  if (c7 == 127 && c8 == 15)
                    fifo_D_drain_out.write(local_D[c11][c10 + 16*c5][16*c6 + c9]);
                  {
                    C_t8 fifo_data;
                    union {unsigned int ui; float ut;} u7, u6, u5, u4, u3, u2, u1, u0;
                    u7.ut = local_C[0][7];
                    u6.ut = local_C[0][6];
                    u5.ut = local_C[0][5];
                    u4.ut = local_C[0][4];
                    u3.ut = local_C[0][3];
                    u2.ut = local_C[0][2];
                    u1.ut = local_C[0][1];
                    u0.ut = local_C[0][0];
                    fifo_data = (ap_uint<32>(u7.ui), ap_uint<32>(u6.ui), ap_uint<32>(u5.ui), ap_uint<32>(u4.ui), ap_uint<32>(u3.ui), ap_uint<32>(u2.ui), ap_uint<32>(u1.ui), ap_uint<32>(u0.ui));
                    fifo_C_out.write(fifo_data);
                  }
                  fifo_B_out.write(local_B[0][0]);
                }
              }
            }
          }
        }
    }
}
/* Module Definition */

/* Module Definition */
void PE_wrapper(int idx, hls::stream<A_t8> &fifo_A_in, hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, hls::stream<C_t8> &fifo_C_in, hls::stream<C_t8> &fifo_C_out, hls::stream<float> &fifo_D_drain_out)
 {
  PE(
    /* module id */ idx, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_B_in, 
    /* fifo */ fifo_B_out, 
    /* fifo */ fifo_C_in, 
    /* fifo */ fifo_C_out, 
    /* fifo */ fifo_D_drain_out);
}
/* Module Definition */

/* Module Definition */
void B_PE_dummy_in(int idx, hls::stream<float> &fifo_B_in) {
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  {
    // array
    {
    }
    // array
    // pe
    for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
        for (ap_uint<8> c7 = 0; c7 <= 127; c7 += 1)
          for (ap_uint<5> c8 = 0; c8 <= 15; c8 += 1) {
            // latency
            for (ap_uint<5> c9 = 0; c9 <= 15; c9 += 1) {
              // latency
              for (ap_uint<5> c10 = 0; c10 <= 15; c10 += 1) {
                // latency
                for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
                #pragma HLS PIPELINE II=1
                  B_t1 fifo_data;
                  fifo_data = fifo_B_in.read();
                }
              }
            }
          }
  }
}
/* Module Definition */

/* Module Definition */
void C_PE_dummy_in(int idx, hls::stream<C_t8> &fifo_C_in) {
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  {
    // array
    {
    }
    // array
    // pe
    for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
        for (ap_uint<8> c7 = 0; c7 <= 127; c7 += 1)
          for (ap_uint<5> c8 = 0; c8 <= 15; c8 += 1) {
            // latency
            for (ap_uint<5> c9 = 0; c9 <= 15; c9 += 1) {
              // latency
              for (ap_uint<5> c10 = 0; c10 <= 15; c10 += 1) {
                // latency
                for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
                #pragma HLS PIPELINE II=1
                  C_t8 fifo_data;
                  fifo_data = fifo_C_in.read();
                }
              }
            }
          }
  }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_intra_trans(int idx, D_t4 local_D[8][128][32], hls::stream<float> &fifo_D_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */


  // io_L1
  // pe
  for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
      // latency
      for (ap_uint<5> c9 = 0; c9 <= 15; c9 += 1) {
        // latency
        for (ap_uint<5> c10 = 0; c10 <= 15; c10 += 1) {
          // latency
          for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
          #pragma HLS PIPELINE II=1
            // simd
            {
              D_t1 in_data;
              D_t4 out_data;
              in_data = fifo_D_drain_local_in.read();
              int split_idx = (16*c6 + c9) % 4;
              out_data = local_D[c11][c10 + 16*c5][(16 * c6 + c9) / 4];
              for (ap_uint<3> n = 0; n < 4; n++) {
              #pragma HLS UNROLL
                data_split[n] = out_data(31, 0);
                out_data = out_data >> 32;
              }
              union {unsigned int ui; float ut;} u;
              u.ut = in_data;
              data_split[split_idx] = ap_uint<32>(u.ui);
              out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);              local_D[c11][c10 + 16*c5][(16 * c6 + c9) / 4] = out_data;
            }
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_inter_trans(int idx, D_t4 local_D[8][128][32], hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<5> c4 = p0; c4 <= 15; c4 += 1) {
    // io_L1
    if (c4 == p0) {
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
        for (ap_uint<8> c6 = 0; c6 <= 127; c6 += 1) {
          // access_coalesce
          for (ap_uint<6> c7 = 0; c7 <= 31; c7 += 1) {
          #pragma HLS PIPELINE II=1
            {
              D_t4 in_data;
              D_t4 out_data;
              in_data = local_D[c5][c6][c7];
              out_data = in_data;
              fifo_D_drain_out.write(out_data);
            }
          }
        }
    } else {
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
        for (ap_uint<8> c6 = 0; c6 <= 127; c6 += 1) {
          // access_coalesce
          for (ap_uint<6> c7 = 0; c7 <= 31; c7 += 1) {
          #pragma HLS PIPELINE II=1
            {
              D_t4 in_data;
              D_t4 out_data;
              in_data = fifo_D_drain_in.read();
              out_data = in_data;
              fifo_D_drain_out.write(out_data);
            }
          }
        }
    }
  }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_inter_trans_boundary(int idx, D_t4 local_D[8][128][32], hls::stream<D_t4> &fifo_D_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<5> c4 = p0; c4 <= 15; c4 += 1)
    if (c4 == p0) {
      // io_L1
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
        for (ap_uint<8> c6 = 0; c6 <= 127; c6 += 1) {
          // access_coalesce
          for (ap_uint<6> c7 = 0; c7 <= 31; c7 += 1) {
          #pragma HLS PIPELINE II=1
            {
              D_t4 in_data;
              D_t4 out_data;
              in_data = local_D[c5][c6][c7];
              out_data = in_data;
              fifo_D_drain_out.write(out_data);
            }
          }
        }
    }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out(int idx, hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  D_t4 local_D[8][128][32];
  #pragma HLS RESOURCE variable=local_D core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  D_drain_IO_L1_out_intra_trans(
    /* module id */ idx, 
    /* array */ local_D, 
    /* fifo */ fifo_D_drain_local_in
  );
  D_drain_IO_L1_out_inter_trans(
    /* module id */ idx, 
    /* array */ local_D, 
    /* fifo */ fifo_D_drain_in, 
    /* fifo */ fifo_D_drain_out
  );
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_wrapper(int idx, hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in)
 {
  D_drain_IO_L1_out(
    /* module id */ idx, 
    /* fifo */ fifo_D_drain_in, 
    /* fifo */ fifo_D_drain_out, 
    /* fifo */ fifo_D_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_boundary(int idx, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  D_t4 local_D[8][128][32];
  #pragma HLS RESOURCE variable=local_D core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  D_drain_IO_L1_out_intra_trans(
    /* module id */ idx, 
    /* array */ local_D, 
    /* fifo */ fifo_D_drain_local_in
  );
  D_drain_IO_L1_out_inter_trans_boundary(
    /* module id */ idx, 
    /* array */ local_D, 
    /* fifo */ fifo_D_drain_out
  );
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_boundary_wrapper(int idx, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in)
 {
  D_drain_IO_L1_out_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_D_drain_out, 
    /* fifo */ fifo_D_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L2_out(hls::stream<D_t4> &fifo_D_drain_out, hls::stream<D_t4> &fifo_D_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L2
  for (ap_uint<5> c4 = 0; c4 <= 15; c4 += 1) {
    // io_L1
    for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1)
      for (ap_uint<8> c6 = 0; c6 <= 127; c6 += 1) {
        // access_coalesce
        // access_serialize
        for (ap_uint<6> c7 = 0; c7 <= 31; c7 += 1) {
        #pragma HLS PIPELINE II=1
          {
            D_t4 in_data;
            D_t4 out_data;
            in_data = fifo_D_drain_local_in.read();
            out_data = in_data;
            fifo_D_drain_out.write(out_data);
          }
        }
      }
  }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L2_out_serialize(D_t16 *D, hls::stream<D_t4> &fifo_D_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<18> i = 0; i < 131072; i++) {
  #pragma HLS PIPELINE II=1
    D_t4 fifo_data;
    D_t16 mem_data;
    D_t4 mem_data_split[4];
    #pragma HLS ARRAY_PARTITION variable=mem_data_split complete
    for (ap_uint<3> p = 0; p < 4; p++) {
      fifo_data = fifo_D_drain_local_in.read();
      mem_data_split[p] = fifo_data;
    }
    mem_data = (mem_data_split[3], mem_data_split[2], mem_data_split[1], mem_data_split[0]);
    D[i] = mem_data;
  }
}
/* Module Definition */

extern "C" {
void kernel0(A_t16 *A, B_t16 *B, C_t16 *C, D_t16 *D)
{
#pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem_A
#pragma HLS INTERFACE m_axi port=B offset=slave bundle=gmem_B
#pragma HLS INTERFACE m_axi port=C offset=slave bundle=gmem_C
#pragma HLS INTERFACE m_axi port=D offset=slave bundle=gmem_D
#pragma HLS INTERFACE s_axilite port=A bundle=control
#pragma HLS INTERFACE s_axilite port=B bundle=control
#pragma HLS INTERFACE s_axilite port=C bundle=control
#pragma HLS INTERFACE s_axilite port=D bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW

  /* FIFO Declaration */
  /* A_IO_L2_in_serialize fifo */ hls::stream<A_t8> fifo_A_A_IO_L2_in_serialize;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_serialize depth=2
  /* B_IO_L2_in_serialize fifo */ hls::stream<B_t16> fifo_B_B_IO_L2_in_serialize;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_serialize depth=2
  /* C_IO_L2_in_serialize fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_serialize;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_serialize depth=2
  /* D_drain_IO_L2_out_serialize fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_serialize;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_serialize depth=2
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_0;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_0 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_1;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_1 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_2;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_2 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_3;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_3 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_4;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_4 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_5;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_5 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_6;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_6 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_7;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_7 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_8;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_8 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_9;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_9 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_9 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_10;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_10 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_10 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_11;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_11 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_11 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_12;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_12 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_12 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_13;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_13 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_13 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_14;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_14 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_14 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_15;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_15 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_15 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t8> fifo_A_A_IO_L1_in_16;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_16 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_0;
  #pragma HLS STREAM variable=fifo_A_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_1;
  #pragma HLS STREAM variable=fifo_A_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_2;
  #pragma HLS STREAM variable=fifo_A_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_3;
  #pragma HLS STREAM variable=fifo_A_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_4;
  #pragma HLS STREAM variable=fifo_A_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_5;
  #pragma HLS STREAM variable=fifo_A_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_6;
  #pragma HLS STREAM variable=fifo_A_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_7;
  #pragma HLS STREAM variable=fifo_A_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_8;
  #pragma HLS STREAM variable=fifo_A_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_9;
  #pragma HLS STREAM variable=fifo_A_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_10;
  #pragma HLS STREAM variable=fifo_A_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_11;
  #pragma HLS STREAM variable=fifo_A_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_12;
  #pragma HLS STREAM variable=fifo_A_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_13;
  #pragma HLS STREAM variable=fifo_A_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_14;
  #pragma HLS STREAM variable=fifo_A_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t8> fifo_A_PE_15;
  #pragma HLS STREAM variable=fifo_A_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0;
  #pragma HLS STREAM variable=fifo_B_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1;
  #pragma HLS STREAM variable=fifo_B_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2;
  #pragma HLS STREAM variable=fifo_B_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3;
  #pragma HLS STREAM variable=fifo_B_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4;
  #pragma HLS STREAM variable=fifo_B_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_5;
  #pragma HLS STREAM variable=fifo_B_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_6;
  #pragma HLS STREAM variable=fifo_B_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_7;
  #pragma HLS STREAM variable=fifo_B_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_8;
  #pragma HLS STREAM variable=fifo_B_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_9;
  #pragma HLS STREAM variable=fifo_B_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_10;
  #pragma HLS STREAM variable=fifo_B_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_11;
  #pragma HLS STREAM variable=fifo_B_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_12;
  #pragma HLS STREAM variable=fifo_B_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_13;
  #pragma HLS STREAM variable=fifo_B_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_14;
  #pragma HLS STREAM variable=fifo_B_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_15;
  #pragma HLS STREAM variable=fifo_B_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_16;
  #pragma HLS STREAM variable=fifo_B_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_0;
  #pragma HLS STREAM variable=fifo_C_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_1;
  #pragma HLS STREAM variable=fifo_C_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_2;
  #pragma HLS STREAM variable=fifo_C_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_3;
  #pragma HLS STREAM variable=fifo_C_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_4;
  #pragma HLS STREAM variable=fifo_C_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_5;
  #pragma HLS STREAM variable=fifo_C_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_6;
  #pragma HLS STREAM variable=fifo_C_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_7;
  #pragma HLS STREAM variable=fifo_C_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_8;
  #pragma HLS STREAM variable=fifo_C_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_9;
  #pragma HLS STREAM variable=fifo_C_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_10;
  #pragma HLS STREAM variable=fifo_C_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_11;
  #pragma HLS STREAM variable=fifo_C_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_12;
  #pragma HLS STREAM variable=fifo_C_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_13;
  #pragma HLS STREAM variable=fifo_C_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_14;
  #pragma HLS STREAM variable=fifo_C_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_15;
  #pragma HLS STREAM variable=fifo_C_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t8> fifo_C_PE_16;
  #pragma HLS STREAM variable=fifo_C_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_0;
  #pragma HLS STREAM variable=fifo_D_drain_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_1;
  #pragma HLS STREAM variable=fifo_D_drain_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_2;
  #pragma HLS STREAM variable=fifo_D_drain_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_3;
  #pragma HLS STREAM variable=fifo_D_drain_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_4;
  #pragma HLS STREAM variable=fifo_D_drain_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_5;
  #pragma HLS STREAM variable=fifo_D_drain_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_6;
  #pragma HLS STREAM variable=fifo_D_drain_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_7;
  #pragma HLS STREAM variable=fifo_D_drain_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_8;
  #pragma HLS STREAM variable=fifo_D_drain_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_9;
  #pragma HLS STREAM variable=fifo_D_drain_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_10;
  #pragma HLS STREAM variable=fifo_D_drain_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_11;
  #pragma HLS STREAM variable=fifo_D_drain_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_12;
  #pragma HLS STREAM variable=fifo_D_drain_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_13;
  #pragma HLS STREAM variable=fifo_D_drain_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_14;
  #pragma HLS STREAM variable=fifo_D_drain_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_15;
  #pragma HLS STREAM variable=fifo_D_drain_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_15 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_8 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_9;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_9 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_9 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_10;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_10 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_10 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_11;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_11 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_11 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_12;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_12 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_12 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_13;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_13 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_13 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_14;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_14 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_14 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_15;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_15 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_15 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_16;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_16 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_16 core=FIFO_SRL
  /* FIFO Declaration */

  /* Module Call */
  A_IO_L2_in_serialize(
    /* array */ A,
    /* fifo */ fifo_A_A_IO_L2_in_serialize
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* fifo */ fifo_A_A_IO_L2_in_serialize,
    /* fifo */ fifo_A_A_IO_L1_in_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_A_A_IO_L1_in_0,
    /* fifo */ fifo_A_A_IO_L1_in_1,
    /* fifo */ fifo_A_PE_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_A_A_IO_L1_in_1,
    /* fifo */ fifo_A_A_IO_L1_in_2,
    /* fifo */ fifo_A_PE_1
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_A_A_IO_L1_in_2,
    /* fifo */ fifo_A_A_IO_L1_in_3,
    /* fifo */ fifo_A_PE_2
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_A_A_IO_L1_in_3,
    /* fifo */ fifo_A_A_IO_L1_in_4,
    /* fifo */ fifo_A_PE_3
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_A_A_IO_L1_in_4,
    /* fifo */ fifo_A_A_IO_L1_in_5,
    /* fifo */ fifo_A_PE_4
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_A_A_IO_L1_in_5,
    /* fifo */ fifo_A_A_IO_L1_in_6,
    /* fifo */ fifo_A_PE_5
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_A_A_IO_L1_in_6,
    /* fifo */ fifo_A_A_IO_L1_in_7,
    /* fifo */ fifo_A_PE_6
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_A_A_IO_L1_in_7,
    /* fifo */ fifo_A_A_IO_L1_in_8,
    /* fifo */ fifo_A_PE_7
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_A_A_IO_L1_in_8,
    /* fifo */ fifo_A_A_IO_L1_in_9,
    /* fifo */ fifo_A_PE_8
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_A_A_IO_L1_in_9,
    /* fifo */ fifo_A_A_IO_L1_in_10,
    /* fifo */ fifo_A_PE_9
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_A_A_IO_L1_in_10,
    /* fifo */ fifo_A_A_IO_L1_in_11,
    /* fifo */ fifo_A_PE_10
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_A_A_IO_L1_in_11,
    /* fifo */ fifo_A_A_IO_L1_in_12,
    /* fifo */ fifo_A_PE_11
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_A_A_IO_L1_in_12,
    /* fifo */ fifo_A_A_IO_L1_in_13,
    /* fifo */ fifo_A_PE_12
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_A_A_IO_L1_in_13,
    /* fifo */ fifo_A_A_IO_L1_in_14,
    /* fifo */ fifo_A_PE_13
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_A_A_IO_L1_in_14,
    /* fifo */ fifo_A_A_IO_L1_in_15,
    /* fifo */ fifo_A_PE_14
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_boundary_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_A_A_IO_L1_in_15,
    /* fifo */ fifo_A_PE_15
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in_boundary_serialize(
    /* array */ B,
    /* fifo */ fifo_B_B_IO_L2_in_serialize
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in_boundary(
    /* fifo */ fifo_B_B_IO_L2_in_serialize,
    /* fifo */ fifo_B_PE_0
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in_boundary_serialize(
    /* array */ C,
    /* fifo */ fifo_C_C_IO_L2_in_serialize
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in_boundary(
    /* fifo */ fifo_C_C_IO_L2_in_serialize,
    /* fifo */ fifo_C_PE_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_A_PE_0,
    /* fifo */ fifo_B_PE_0,
    /* fifo */ fifo_B_PE_1,
    /* fifo */ fifo_C_PE_0,
    /* fifo */ fifo_C_PE_1,
    /* fifo */ fifo_D_drain_PE_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_A_PE_1,
    /* fifo */ fifo_B_PE_1,
    /* fifo */ fifo_B_PE_2,
    /* fifo */ fifo_C_PE_1,
    /* fifo */ fifo_C_PE_2,
    /* fifo */ fifo_D_drain_PE_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_A_PE_2,
    /* fifo */ fifo_B_PE_2,
    /* fifo */ fifo_B_PE_3,
    /* fifo */ fifo_C_PE_2,
    /* fifo */ fifo_C_PE_3,
    /* fifo */ fifo_D_drain_PE_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_A_PE_3,
    /* fifo */ fifo_B_PE_3,
    /* fifo */ fifo_B_PE_4,
    /* fifo */ fifo_C_PE_3,
    /* fifo */ fifo_C_PE_4,
    /* fifo */ fifo_D_drain_PE_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_A_PE_4,
    /* fifo */ fifo_B_PE_4,
    /* fifo */ fifo_B_PE_5,
    /* fifo */ fifo_C_PE_4,
    /* fifo */ fifo_C_PE_5,
    /* fifo */ fifo_D_drain_PE_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_A_PE_5,
    /* fifo */ fifo_B_PE_5,
    /* fifo */ fifo_B_PE_6,
    /* fifo */ fifo_C_PE_5,
    /* fifo */ fifo_C_PE_6,
    /* fifo */ fifo_D_drain_PE_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_A_PE_6,
    /* fifo */ fifo_B_PE_6,
    /* fifo */ fifo_B_PE_7,
    /* fifo */ fifo_C_PE_6,
    /* fifo */ fifo_C_PE_7,
    /* fifo */ fifo_D_drain_PE_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_A_PE_7,
    /* fifo */ fifo_B_PE_7,
    /* fifo */ fifo_B_PE_8,
    /* fifo */ fifo_C_PE_7,
    /* fifo */ fifo_C_PE_8,
    /* fifo */ fifo_D_drain_PE_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_A_PE_8,
    /* fifo */ fifo_B_PE_8,
    /* fifo */ fifo_B_PE_9,
    /* fifo */ fifo_C_PE_8,
    /* fifo */ fifo_C_PE_9,
    /* fifo */ fifo_D_drain_PE_8
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_A_PE_9,
    /* fifo */ fifo_B_PE_9,
    /* fifo */ fifo_B_PE_10,
    /* fifo */ fifo_C_PE_9,
    /* fifo */ fifo_C_PE_10,
    /* fifo */ fifo_D_drain_PE_9
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_A_PE_10,
    /* fifo */ fifo_B_PE_10,
    /* fifo */ fifo_B_PE_11,
    /* fifo */ fifo_C_PE_10,
    /* fifo */ fifo_C_PE_11,
    /* fifo */ fifo_D_drain_PE_10
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_A_PE_11,
    /* fifo */ fifo_B_PE_11,
    /* fifo */ fifo_B_PE_12,
    /* fifo */ fifo_C_PE_11,
    /* fifo */ fifo_C_PE_12,
    /* fifo */ fifo_D_drain_PE_11
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_A_PE_12,
    /* fifo */ fifo_B_PE_12,
    /* fifo */ fifo_B_PE_13,
    /* fifo */ fifo_C_PE_12,
    /* fifo */ fifo_C_PE_13,
    /* fifo */ fifo_D_drain_PE_12
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_A_PE_13,
    /* fifo */ fifo_B_PE_13,
    /* fifo */ fifo_B_PE_14,
    /* fifo */ fifo_C_PE_13,
    /* fifo */ fifo_C_PE_14,
    /* fifo */ fifo_D_drain_PE_13
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_A_PE_14,
    /* fifo */ fifo_B_PE_14,
    /* fifo */ fifo_B_PE_15,
    /* fifo */ fifo_C_PE_14,
    /* fifo */ fifo_C_PE_15,
    /* fifo */ fifo_D_drain_PE_14
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_A_PE_15,
    /* fifo */ fifo_B_PE_15,
    /* fifo */ fifo_B_PE_16,
    /* fifo */ fifo_C_PE_15,
    /* fifo */ fifo_C_PE_16,
    /* fifo */ fifo_D_drain_PE_15
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 15,
    /* fifo */ fifo_B_PE_16
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_in(
    /* module id */ 15,
    /* fifo */ fifo_C_PE_16
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_15,
    /* fifo */ fifo_D_drain_PE_15
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_15,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_14,
    /* fifo */ fifo_D_drain_PE_14
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_14,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_13,
    /* fifo */ fifo_D_drain_PE_13
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_13,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_12,
    /* fifo */ fifo_D_drain_PE_12
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_12,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_11,
    /* fifo */ fifo_D_drain_PE_11
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_11,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_10,
    /* fifo */ fifo_D_drain_PE_10
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_10,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_9,
    /* fifo */ fifo_D_drain_PE_9
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_9,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_8,
    /* fifo */ fifo_D_drain_PE_8
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_8,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7,
    /* fifo */ fifo_D_drain_PE_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6,
    /* fifo */ fifo_D_drain_PE_6
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5,
    /* fifo */ fifo_D_drain_PE_5
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4,
    /* fifo */ fifo_D_drain_PE_4
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3,
    /* fifo */ fifo_D_drain_PE_3
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2,
    /* fifo */ fifo_D_drain_PE_2
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1,
    /* fifo */ fifo_D_drain_PE_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0,
    /* fifo */ fifo_D_drain_PE_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out(
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_serialize,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out_serialize(
    /* array */ D,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_serialize
  );
  /* Module Call */

}
}

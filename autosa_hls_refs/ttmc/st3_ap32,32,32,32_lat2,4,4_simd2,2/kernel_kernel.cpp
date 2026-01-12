// AutoSA Command:
// /workspaces/mlir-systolic/third_party/AutoSA/autosa /workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/ttmc/kernel.c --config=/workspaces/mlir-systolic/third_party/AutoSA/autosa_config/autosa_config.json --target=autosa_hls_c --output-dir=/workspaces/mlir-systolic/third_party/AutoSA/autosa.tmp/output --host-serialize --simd-info=/workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/ttmc/simd_info.json --sa-sizes={kernel[]->space_time[3];kernel[]->array_part[32,32,32,32];kernel[]->latency[2,4,4];kernel[]->simd[2,2]}

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
typedef ap_uint<64> A_t2;
typedef ap_uint<128> A_t4;
typedef ap_uint<512> B_t16;
typedef ap_uint<128> B_t4;
typedef ap_uint<512> C_t16;
typedef ap_uint<64> C_t2;
typedef ap_uint<128> C_t4;
typedef ap_uint<512> D_t16;
typedef ap_uint<128> D_t4;
/* Data Type */

extern "C" {
void kernel0(A_t16 *A, B_t16 *B, C_t16 *C, D_t16 *D_0, D_t16 *D_1, D_t16 *D_2);
}
void A_IO_L1_in_intra_trans(int idx, int c0, int c1, int c2, int c3, A_t4 local_A[32][1][32], hls::stream<A_t2> &fifo_A_local_out, bool intra_trans_en);
void A_IO_L1_in_inter_trans(int idx, int c0, int c1, int c2, int c3, A_t4 local_A[32][1][32], hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, bool inter_trans_en);
void A_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c2, int c3, A_t4 local_A[32][1][32], hls::stream<A_t4> &fifo_A_in, bool inter_trans_en);
void A_IO_L1_in_wrapper(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<A_t2> &fifo_A_local_out);
void A_IO_L1_in_boundary_wrapper(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t2> &fifo_A_local_out);
void B_IO_L1_in_intra_trans(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[1][8], hls::stream<float> &fifo_B_local_out, bool intra_trans_en);
void B_IO_L1_in_inter_trans(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[1][8], hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, bool inter_trans_en);
void B_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[1][8], hls::stream<B_t4> &fifo_B_in, bool inter_trans_en);
void B_IO_L1_in_wrapper(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, hls::stream<float> &fifo_B_local_out);
void B_IO_L1_in_boundary_wrapper(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<float> &fifo_B_local_out);
void C_IO_L1_in_intra_trans(int idx, int c0, int c1, int c2, C_t4 local_C[32][32], hls::stream<C_t2> &fifo_C_local_out, bool intra_trans_en);
void C_IO_L1_in_inter_trans(int idx, int c0, int c1, int c2, C_t4 local_C[32][32], hls::stream<C_t4> &fifo_C_in, hls::stream<C_t4> &fifo_C_out, bool inter_trans_en);
void C_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c2, C_t4 local_C[32][32], hls::stream<C_t4> &fifo_C_in, bool inter_trans_en);
void C_IO_L1_in_wrapper(int idx, hls::stream<C_t4> &fifo_C_in, hls::stream<C_t4> &fifo_C_out, hls::stream<C_t2> &fifo_C_local_out);
void C_IO_L1_in_boundary_wrapper(int idx, hls::stream<C_t4> &fifo_C_in, hls::stream<C_t2> &fifo_C_local_out);
void D_1_IO_L2_in_intra_trans(int c0, int c1, int c2, int c3, D_t16 *D, D_t16 local_D_1[32][32][2], hls::stream<float> &fifo_D_1_local_out, bool intra_trans_en);
void PE_wrapper(int idx, hls::stream<A_t2> &fifo_A_in, hls::stream<float> &fifo_B_in, hls::stream<C_t2> &fifo_C_in, hls::stream<float> &fifo_D_1_in, hls::stream<float> &fifo_D_1_out, hls::stream<float> &fifo_D_drain_out);
void D_1_IO_L2_out_intra_trans(int c0, int c1, int c2, int c3, D_t16 *D, D_t16 local_D_1[32][32][2], hls::stream<float> &fifo_D_1_local_in, bool intra_trans_en);
void D_1_IO_L2_out_inter_trans_boundary(int c0, int c1, int c2, D_t16 *D, D_t16 local_D_1[32][32][2], hls::stream<D_t16> &fifo_D_1_out, bool inter_trans_en);
void D_drain_IO_L1_out_intra_trans(int idx, int c0, int c1, int c2, D_t4 local_D[32][32][8], hls::stream<float> &fifo_D_drain_local_in);
void D_drain_IO_L1_out_inter_trans(int idx, int c0, int c1, int c2, D_t4 local_D[32][32][8], hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out);
void D_drain_IO_L1_out_inter_trans_boundary(int idx, int c0, int c1, int c2, D_t4 local_D[32][32][8], hls::stream<D_t4> &fifo_D_drain_out);
void D_drain_IO_L1_out_wrapper(int idx, hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in);
void D_drain_IO_L1_out_boundary_wrapper(int idx, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in);

/* Module Definition */
void A_IO_L2_in(A_t16 *A, hls::stream<A_t4> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  A_t4 data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
        for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
          // array
          // io_L2
          for (ap_uint<6> c4 = 0; c4 <= 31; c4 += 1) {
            // io_L1
            for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1) {
              // access_coalesce
              for (ap_uint<6> c7 = 0; c7 <= 31; c7 += 1) {
              #pragma HLS PIPELINE II=1
                {
                  A_t16 in_data;
                  A_t4 out_data;
                  if (c7 % 4 == 0) {
                    in_data = A[(((32 * c0 + c5) * 128 + (32 * c3 + c4)) * 128 + 4 * c7) / 16];
                    for (ap_uint<3> n = 0; n < 4; n++) {
                    #pragma HLS UNROLL
                      data_split[n] = in_data(127, 0);
                      in_data = in_data >> 128;
                    }
                  }
                  int split_idx = (131072*c0 + 1024*c3 + 32*c4 + 4096*c5 + c7) % 4;
                  out_data = data_split[split_idx];
                  fifo_A_local_out.write(out_data);
                }
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_intra_trans(int idx, int c0, int c1, int c2, int c3, A_t4 local_A[32][1][32], hls::stream<A_t2> &fifo_A_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t2 data_split[2];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L1
  // pe
  for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
        for (ap_uint<7> c8 = 0; c8 <= 63; c8 += 1) {
          // latency
          for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
            // latency
            for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
              // latency
              for (ap_uint<2> c11 = 0; c11 <= 1; c11 += 1) {
              #pragma HLS PIPELINE II=1
                // simd
                {
                  A_t4 in_data;
                  A_t2 out_data;
                  in_data = local_A[c11 + 2*c5][0][2 * c8 / 4];
                  for (ap_uint<2> n = 0; n < 2; n++) {
                  #pragma HLS UNROLL
                    data_split[n] = in_data(63, 0);
                    in_data = in_data >> 64;
                  }
                  int split_idx = (c8) % 2;
                  out_data = data_split[split_idx];
                  fifo_A_local_out.write(out_data);
                }
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_inter_trans(int idx, int c0, int c1, int c2, int c3, A_t4 local_A[32][1][32], hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<6> c4 = p0; c4 <= 31; c4 += 1) {
    // io_L1
    if (c4 == p0) {
      for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1) {
        // access_coalesce
        for (ap_uint<6> c7 = 0; c7 <= 31; c7 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t4 in_data;
            A_t4 out_data;
            in_data = fifo_A_in.read();
            out_data = in_data;
            local_A[c5][0][c7] = out_data;
          }
        }
      }
    } else {
      for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1) {
        // access_coalesce
        for (ap_uint<6> c7 = 0; c7 <= 31; c7 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t4 in_data;
            A_t4 out_data;
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
void A_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c2, int c3, A_t4 local_A[32][1][32], hls::stream<A_t4> &fifo_A_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<6> c4 = p0; c4 <= 31; c4 += 1)
    if (c4 == p0) {
      // io_L1
      for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1) {
        // access_coalesce
        for (ap_uint<6> c7 = 0; c7 <= 31; c7 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t4 in_data;
            A_t4 out_data;
            in_data = fifo_A_in.read();
            out_data = in_data;
            local_A[c5][0][c7] = out_data;
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<A_t2> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t4 local_A_ping[32][1][32];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_2P_BRAM
  A_t4 local_A_pong[32][1][32];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
          for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
            // array
            // io_L2
            {
              if (arb == 0) {
                A_IO_L1_in_inter_trans(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_A_pong, 
                  /* fifo */ fifo_A_in, 
                  /* fifo */ fifo_A_out, 
                  /* enable */ inter_trans_en
                );
                A_IO_L1_in_intra_trans(
                  /* module id */ idx, 
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ local_A_ping, 
                  /* fifo */ fifo_A_local_out, 
                  /* enable */ intra_trans_en
                );
              } else {
                A_IO_L1_in_inter_trans(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_A_ping, 
                  /* fifo */ fifo_A_in, 
                  /* fifo */ fifo_A_out, 
                  /* enable */ inter_trans_en
                );
                A_IO_L1_in_intra_trans(
                  /* module id */ idx, 
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ local_A_pong, 
                  /* fifo */ fifo_A_local_out, 
                  /* enable */ intra_trans_en
                );
              }
              intra_trans_en = 1;
              arb = !arb;
              c0_prev = c0;
              c1_prev = c1;
              c2_prev = c2;
              c3_prev = c3;
            }
          }
    if (arb == 0) {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_A_ping, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_A_pong, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_wrapper(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<A_t2> &fifo_A_local_out)
 {
  A_IO_L1_in(
    /* module id */ idx, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_out, 
    /* fifo */ fifo_A_local_out);
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_boundary(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t2> &fifo_A_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t4 local_A_ping[32][1][32];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_2P_BRAM
  A_t4 local_A_pong[32][1][32];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
          for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
            // array
            // io_L2
            {
              if (arb == 0) {
                A_IO_L1_in_inter_trans_boundary(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_A_pong, 
                  /* fifo */ fifo_A_in, 
                  /* enable */ inter_trans_en
                );
                A_IO_L1_in_intra_trans(
                  /* module id */ idx, 
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ local_A_ping, 
                  /* fifo */ fifo_A_local_out, 
                  /* enable */ intra_trans_en
                );
              } else {
                A_IO_L1_in_inter_trans_boundary(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_A_ping, 
                  /* fifo */ fifo_A_in, 
                  /* enable */ inter_trans_en
                );
                A_IO_L1_in_intra_trans(
                  /* module id */ idx, 
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ local_A_pong, 
                  /* fifo */ fifo_A_local_out, 
                  /* enable */ intra_trans_en
                );
              }
              intra_trans_en = 1;
              arb = !arb;
              c0_prev = c0;
              c1_prev = c1;
              c2_prev = c2;
              c3_prev = c3;
            }
          }
    if (arb == 0) {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_A_ping, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_A_pong, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_boundary_wrapper(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t2> &fifo_A_local_out)
 {
  A_IO_L1_in_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_local_out);
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in(B_t16 *B, hls::stream<B_t4> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  B_t4 data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
        for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
          // array
          // io_L2
          for (ap_uint<6> c4 = 0; c4 <= 31; c4 += 1) {
            // io_L1
            // access_coalesce
            for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
            #pragma HLS PIPELINE II=1
              {
                B_t16 in_data;
                B_t4 out_data;
                if (c6 % 4 == 0) {
                  in_data = B[((32 * c3 + c4) * 128 + (32 * c1 + 4 * c6)) / 16];
                  for (ap_uint<3> n = 0; n < 4; n++) {
                  #pragma HLS UNROLL
                    data_split[n] = in_data(127, 0);
                    in_data = in_data >> 128;
                  }
                }
                int split_idx = (8*c1 + 1024*c3 + 32*c4 + c6) % 4;
                out_data = data_split[split_idx];
                fifo_B_local_out.write(out_data);
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void B_IO_L1_in_intra_trans(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[1][8], hls::stream<float> &fifo_B_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L1
  // pe
  for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
        for (ap_uint<7> c8 = 0; c8 <= 63; c8 += 1) {
          // latency
          for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
            // latency
            for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
              // latency
              for (ap_uint<2> c11 = 0; c11 <= 1; c11 += 1) {
              #pragma HLS PIPELINE II=1
                // simd
                {
                  B_t4 in_data;
                  B_t1 out_data;
                  in_data = local_B[0][(4 * c6 + c10) / 4];
                  for (ap_uint<3> n = 0; n < 4; n++) {
                  #pragma HLS UNROLL
                    data_split[n] = in_data(31, 0);
                    in_data = in_data >> 32;
                  }
                  int split_idx = (c10 + 4*c6) % 4;
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
void B_IO_L1_in_inter_trans(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[1][8], hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<6> c4 = p0; c4 <= 31; c4 += 1) {
    // io_L1
    if (c4 == p0) {
      // access_coalesce
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
      #pragma HLS PIPELINE II=1
        {
          B_t4 in_data;
          B_t4 out_data;
          in_data = fifo_B_in.read();
          out_data = in_data;
          local_B[0][c6] = out_data;
        }
      }
    } else {
      // access_coalesce
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
      #pragma HLS PIPELINE II=1
        {
          B_t4 in_data;
          B_t4 out_data;
          in_data = fifo_B_in.read();
          out_data = in_data;
          fifo_B_out.write(out_data);
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[1][8], hls::stream<B_t4> &fifo_B_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<6> c4 = p0; c4 <= 31; c4 += 1)
    if (c4 == p0) {
      // io_L1
      // access_coalesce
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
      #pragma HLS PIPELINE II=1
        {
          B_t4 in_data;
          B_t4 out_data;
          in_data = fifo_B_in.read();
          out_data = in_data;
          local_B[0][c6] = out_data;
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void B_IO_L1_in(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, hls::stream<float> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  B_t4 local_B_ping[1][8];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_2P_BRAM
  B_t4 local_B_pong[1][8];
  #pragma HLS RESOURCE variable=local_B_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
          for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
            // array
            // io_L2
            {
              if (arb == 0) {
                B_IO_L1_in_inter_trans(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_B_pong, 
                  /* fifo */ fifo_B_in, 
                  /* fifo */ fifo_B_out, 
                  /* enable */ inter_trans_en
                );
                B_IO_L1_in_intra_trans(
                  /* module id */ idx, 
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ local_B_ping, 
                  /* fifo */ fifo_B_local_out, 
                  /* enable */ intra_trans_en
                );
              } else {
                B_IO_L1_in_inter_trans(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_B_ping, 
                  /* fifo */ fifo_B_in, 
                  /* fifo */ fifo_B_out, 
                  /* enable */ inter_trans_en
                );
                B_IO_L1_in_intra_trans(
                  /* module id */ idx, 
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ local_B_pong, 
                  /* fifo */ fifo_B_local_out, 
                  /* enable */ intra_trans_en
                );
              }
              intra_trans_en = 1;
              arb = !arb;
              c0_prev = c0;
              c1_prev = c1;
              c2_prev = c2;
              c3_prev = c3;
            }
          }
    if (arb == 0) {
      B_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_B_ping, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      B_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_B_pong, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L1_in_wrapper(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, hls::stream<float> &fifo_B_local_out)
 {
  B_IO_L1_in(
    /* module id */ idx, 
    /* fifo */ fifo_B_in, 
    /* fifo */ fifo_B_out, 
    /* fifo */ fifo_B_local_out);
}
/* Module Definition */

/* Module Definition */
void B_IO_L1_in_boundary(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<float> &fifo_B_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  B_t4 local_B_ping[1][8];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_2P_BRAM
  B_t4 local_B_pong[1][8];
  #pragma HLS RESOURCE variable=local_B_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
          for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
            // array
            // io_L2
            {
              if (arb == 0) {
                B_IO_L1_in_inter_trans_boundary(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_B_pong, 
                  /* fifo */ fifo_B_in, 
                  /* enable */ inter_trans_en
                );
                B_IO_L1_in_intra_trans(
                  /* module id */ idx, 
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ local_B_ping, 
                  /* fifo */ fifo_B_local_out, 
                  /* enable */ intra_trans_en
                );
              } else {
                B_IO_L1_in_inter_trans_boundary(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_B_ping, 
                  /* fifo */ fifo_B_in, 
                  /* enable */ inter_trans_en
                );
                B_IO_L1_in_intra_trans(
                  /* module id */ idx, 
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ local_B_pong, 
                  /* fifo */ fifo_B_local_out, 
                  /* enable */ intra_trans_en
                );
              }
              intra_trans_en = 1;
              arb = !arb;
              c0_prev = c0;
              c1_prev = c1;
              c2_prev = c2;
              c3_prev = c3;
            }
          }
    if (arb == 0) {
      B_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_B_ping, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      B_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_B_pong, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L1_in_boundary_wrapper(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<float> &fifo_B_local_out)
 {
  B_IO_L1_in_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_B_in, 
    /* fifo */ fifo_B_local_out);
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_in(C_t16 *C, hls::stream<C_t4> &fifo_C_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  C_t4 data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
        // array
        // io_L2
        for (ap_uint<6> c4 = 0; c4 <= 31; c4 += 1) {
          // io_L1
          for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1) {
            // access_coalesce
            for (ap_uint<6> c6 = 0; c6 <= 31; c6 += 1) {
            #pragma HLS PIPELINE II=1
              {
                C_t16 in_data;
                C_t4 out_data;
                if (c6 % 4 == 0) {
                  in_data = C[((32 * c2 + c5) * 128 + 4 * c6) / 16];
                  for (ap_uint<3> n = 0; n < 4; n++) {
                  #pragma HLS UNROLL
                    data_split[n] = in_data(127, 0);
                    in_data = in_data >> 128;
                  }
                }
                int split_idx = (1024*c2 + 32*c5 + c6) % 4;
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
void C_IO_L1_in_intra_trans(int idx, int c0, int c1, int c2, C_t4 local_C[32][32], hls::stream<C_t2> &fifo_C_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t2 data_split[2];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
    // array
    // io_L2
    // io_L1
    // pe
    for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1)
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
          for (ap_uint<7> c8 = 0; c8 <= 63; c8 += 1) {
            // latency
            for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
              // latency
              for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
                // latency
                for (ap_uint<2> c11 = 0; c11 <= 1; c11 += 1) {
                #pragma HLS PIPELINE II=1
                  // simd
                  {
                    C_t4 in_data;
                    C_t2 out_data;
                    in_data = local_C[4*c7 + c9][2 * c8 / 4];
                    for (ap_uint<2> n = 0; n < 2; n++) {
                    #pragma HLS UNROLL
                      data_split[n] = in_data(63, 0);
                      in_data = in_data >> 64;
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
}
/* Module Definition */

/* Module Definition */
void C_IO_L1_in_inter_trans(int idx, int c0, int c1, int c2, C_t4 local_C[32][32], hls::stream<C_t4> &fifo_C_in, hls::stream<C_t4> &fifo_C_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<6> c4 = p0; c4 <= 31; c4 += 1) {
    // io_L1
    if (c4 == p0) {
      for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1) {
        // access_coalesce
        for (ap_uint<6> c6 = 0; c6 <= 31; c6 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t4 in_data;
            C_t4 out_data;
            in_data = fifo_C_in.read();
            out_data = in_data;
            local_C[c5][c6] = out_data;
          }
        }
      }
    } else {
      for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1) {
        // access_coalesce
        for (ap_uint<6> c6 = 0; c6 <= 31; c6 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t4 in_data;
            C_t4 out_data;
            in_data = fifo_C_in.read();
            out_data = in_data;
            fifo_C_out.write(out_data);
          }
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c2, C_t4 local_C[32][32], hls::stream<C_t4> &fifo_C_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<6> c4 = p0; c4 <= 31; c4 += 1)
    if (c4 == p0) {
      // io_L1
      for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1) {
        // access_coalesce
        for (ap_uint<6> c6 = 0; c6 <= 31; c6 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t4 in_data;
            C_t4 out_data;
            in_data = fifo_C_in.read();
            out_data = in_data;
            local_C[c5][c6] = out_data;
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void C_IO_L1_in(int idx, hls::stream<C_t4> &fifo_C_in, hls::stream<C_t4> &fifo_C_out, hls::stream<C_t2> &fifo_C_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t4 local_C_ping[32][32];
  #pragma HLS RESOURCE variable=local_C_ping core=RAM_2P_BRAM
  C_t4 local_C_pong[32][32];
  #pragma HLS RESOURCE variable=local_C_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  /* Variable Declaration */

  {
    for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
          // array
          // io_L2
          {
            if (arb == 0) {
              C_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_C_pong, 
                /* fifo */ fifo_C_in, 
                /* fifo */ fifo_C_out, 
                /* enable */ inter_trans_en
              );
              C_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
                /* array */ local_C_ping, 
                /* fifo */ fifo_C_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              C_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_C_ping, 
                /* fifo */ fifo_C_in, 
                /* fifo */ fifo_C_out, 
                /* enable */ inter_trans_en
              );
              C_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
                /* array */ local_C_pong, 
                /* fifo */ fifo_C_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c1_prev = c1;
            c2_prev = c2;
          }
        }
    if (arb == 0) {
      C_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_C_ping, 
        /* fifo */ fifo_C_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      C_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_C_pong, 
        /* fifo */ fifo_C_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_IO_L1_in_wrapper(int idx, hls::stream<C_t4> &fifo_C_in, hls::stream<C_t4> &fifo_C_out, hls::stream<C_t2> &fifo_C_local_out)
 {
  C_IO_L1_in(
    /* module id */ idx, 
    /* fifo */ fifo_C_in, 
    /* fifo */ fifo_C_out, 
    /* fifo */ fifo_C_local_out);
}
/* Module Definition */

/* Module Definition */
void C_IO_L1_in_boundary(int idx, hls::stream<C_t4> &fifo_C_in, hls::stream<C_t2> &fifo_C_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t4 local_C_ping[32][32];
  #pragma HLS RESOURCE variable=local_C_ping core=RAM_2P_BRAM
  C_t4 local_C_pong[32][32];
  #pragma HLS RESOURCE variable=local_C_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  /* Variable Declaration */

  {
    for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
          // array
          // io_L2
          {
            if (arb == 0) {
              C_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_C_pong, 
                /* fifo */ fifo_C_in, 
                /* enable */ inter_trans_en
              );
              C_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
                /* array */ local_C_ping, 
                /* fifo */ fifo_C_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              C_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_C_ping, 
                /* fifo */ fifo_C_in, 
                /* enable */ inter_trans_en
              );
              C_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
                /* array */ local_C_pong, 
                /* fifo */ fifo_C_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c1_prev = c1;
            c2_prev = c2;
          }
        }
    if (arb == 0) {
      C_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_C_ping, 
        /* fifo */ fifo_C_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      C_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_C_pong, 
        /* fifo */ fifo_C_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_IO_L1_in_boundary_wrapper(int idx, hls::stream<C_t4> &fifo_C_in, hls::stream<C_t2> &fifo_C_local_out)
 {
  C_IO_L1_in_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_C_in, 
    /* fifo */ fifo_C_local_out);
}
/* Module Definition */

/* Module Definition */
void D_1_IO_L2_in_intra_trans(int c0, int c1, int c2, int c3, D_t16 *D, D_t16 local_D_1[32][32][2], hls::stream<float> &fifo_D_1_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  ap_uint<32> data_split[16];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
        // latency
        for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
          // latency
          for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
            // latency
            for (ap_uint<2> c11 = 0; c11 <= 1; c11 += 1) {
            #pragma HLS PIPELINE II=1
              // simd
              {
                D_t16 in_data;
                D_t1 out_data;
                in_data = local_D_1[c11 + 2*c5][c10 + 4*c6][(4 * c7 + c9) / 16];
                for (ap_uint<5> n = 0; n < 16; n++) {
                #pragma HLS UNROLL
                  data_split[n] = in_data(31, 0);
                  in_data = in_data >> 32;
                }
                int split_idx = (4*c7 + c9) % 16;
                union {unsigned int ui; float ut;} u;
                u.ui = (unsigned int)data_split[split_idx];
                out_data = u.ut;
                fifo_D_1_local_out.write(out_data);
              }
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void D_1_IO_L2_in_boundary(D_t16 *D, hls::stream<float> &fifo_D_1_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  D_t16 local_D_1_ping[32][32][2];
  #pragma HLS RESOURCE variable=local_D_1_ping core=RAM_2P_BRAM
  D_t16 local_D_1_pong[32][32][2];
  #pragma HLS RESOURCE variable=local_D_1_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
          for (ap_uint<3> c3 = 1; c3 <= 3; c3 += 1) {
            // array
            {
              if (arb == 0) {
                D_1_IO_L2_in_inter_trans_boundary(
                  /* array */ D, 
                  /* array */ local_D_1_pong, 
                  /* fifo */ fifo_D_1_in, 
                  /* enable */ inter_trans_en
                );
                D_1_IO_L2_in_intra_trans(
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ D, 
                  /* array */ local_D_1_ping, 
                  /* fifo */ fifo_D_1_local_out, 
                  /* enable */ intra_trans_en
                );
              } else {
                D_1_IO_L2_in_inter_trans_boundary(
                  /* array */ D, 
                  /* array */ local_D_1_ping, 
                  /* fifo */ fifo_D_1_in, 
                  /* enable */ inter_trans_en
                );
                D_1_IO_L2_in_intra_trans(
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* host iter */ c3_prev, 
                  /* array */ D, 
                  /* array */ local_D_1_pong, 
                  /* fifo */ fifo_D_1_local_out, 
                  /* enable */ intra_trans_en
                );
              }
              intra_trans_en = 1;
              arb = !arb;
              c0_prev = c0;
              c1_prev = c1;
              c2_prev = c2;
              c3_prev = c3;
            }
          }
    if (arb == 0) {
      D_1_IO_L2_in_intra_trans(
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ D, 
        /* array */ local_D_1_ping, 
        /* fifo */ fifo_D_1_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      D_1_IO_L2_in_intra_trans(
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ D, 
        /* array */ local_D_1_pong, 
        /* fifo */ fifo_D_1_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void PE(int idx, hls::stream<A_t2> &fifo_A_in, hls::stream<float> &fifo_B_in, hls::stream<C_t2> &fifo_C_in, hls::stream<float> &fifo_D_1_in, hls::stream<float> &fifo_D_1_out, hls::stream<float> &fifo_D_drain_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t1 local_A[1][1][2];
  #pragma HLS ARRAY_PARTITION variable=local_A dim=0 complete
  B_t1 local_B[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_B dim=0 complete
  C_t1 local_C[1][2];
  #pragma HLS ARRAY_PARTITION variable=local_C dim=0 complete
  D_t1 local_D[32][32][32];
  #pragma HLS RESOURCE variable=local_D core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
        for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
          // array
          // pe
          for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1)
            for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
              for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
                if (p0 == 0 && c3 == 0) {
                  // latency
                  for (ap_uint<3> c8 = 0; c8 <= 3; c8 += 1) {
                    // latency
                    for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
                      // latency
                      for (ap_uint<2> c10 = 0; c10 <= 1; c10 += 1) {
                      #pragma HLS PIPELINE II=1
                        local_D[c10 + 2*c5][4*c6 + c9][4*c7 + c8] = 0;
                      }
                    }
                  }
                }
                for (ap_uint<7> c8 = 0; c8 <= 63; c8 += 1) {
                  // latency
                  for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
                    // latency
                    for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
                      // latency
                      for (ap_uint<2> c11 = 0; c11 <= 1; c11 += 1) {
                      #pragma HLS PIPELINE II=1
                        {
                          {
                            A_t2 fifo_data;
                            fifo_data = fifo_A_in.read();
                            for (ap_uint<2> n = 0; n < 2; n++) {
                            #pragma HLS UNROLL
                              union {unsigned int ui; float ut;} u;
                              u.ui = (unsigned int)fifo_data(31, 0);
                              local_A[0][0][n] = u.ut;
                              fifo_data = fifo_data >> 32;
                            }
                          }
                          local_B[0][0] = fifo_B_in.read();
                          {
                            C_t2 fifo_data;
                            fifo_data = fifo_C_in.read();
                            for (ap_uint<2> n = 0; n < 2; n++) {
                            #pragma HLS UNROLL
                              union {unsigned int ui; float ut;} u;
                              u.ui = (unsigned int)fifo_data(31, 0);
                              local_C[0][n] = u.ut;
                              fifo_data = fifo_data >> 32;
                            }
                          }
                          if (p0 + 32 * c3 >= 1 && c8 == 0)
                            local_D[c11 + 2*c5][c10 + 4*c6][4*c7 + c9] = fifo_D_1_in.read();
                          // simd
                          for (ap_uint<2> c12 = 0; c12 <= 1; c12 += 1) {
                          #pragma HLS UNROLL
                            local_D[c11 + 2*c5][c10 + 4*c6][4*c7 + c9] = (local_D[c11 + 2*c5][c10 + 4*c6][4*c7 + c9] + ((local_A[0][0][c12] * local_B[0][0]) * local_C[0][c12]));
                          }
                          if (p0 == 31 && c3 == 3 && c8 == 63) {
                            fifo_D_drain_out.write(local_D[c11 + 2*c5][c10 + 4*c6][4*c7 + c9]);
                          } else if (p0 + 32 * c3 <= 126 && c8 == 63) {
                            fifo_D_1_out.write(local_D[c11 + 2*c5][c10 + 4*c6][4*c7 + c9]);
                          }
                        }
                      }
                    }
                  }
                }
              }
        }
}
/* Module Definition */

/* Module Definition */
void PE_wrapper(int idx, hls::stream<A_t2> &fifo_A_in, hls::stream<float> &fifo_B_in, hls::stream<C_t2> &fifo_C_in, hls::stream<float> &fifo_D_1_in, hls::stream<float> &fifo_D_1_out, hls::stream<float> &fifo_D_drain_out)
 {
  PE(
    /* module id */ idx, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_B_in, 
    /* fifo */ fifo_C_in, 
    /* fifo */ fifo_D_1_in, 
    /* fifo */ fifo_D_1_out, 
    /* fifo */ fifo_D_drain_out);
}
/* Module Definition */

/* Module Definition */
void D_1_PE_dummy_out(int idx, hls::stream<float> &fifo_D_1_out) {
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
        // array
        {
        }
        for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1)
          if (p0 + 32 * c3 >= 1) {
            // array
            // pe
            for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1)
              for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
                for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
                  for (ap_uint<7> c8 = 0; c8 <= 63; c8 += 1)
                    if (c8 == 0) {
                      // latency
                      for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
                        // latency
                        for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
                          // latency
                          for (ap_uint<2> c11 = 0; c11 <= 1; c11 += 1) {
                          #pragma HLS PIPELINE II=1
                            D_t1 fifo_data = 0;
                            fifo_D_1_out.write(fifo_data);
                          }
                        }
                      }
                    }
          }
      }
}
/* Module Definition */

/* Module Definition */
void D_1_IO_L2_out_intra_trans(int c0, int c1, int c2, int c3, D_t16 *D, D_t16 local_D_1[32][32][2], hls::stream<float> &fifo_D_1_local_in, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  ap_uint<32> data_split[16];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
        // latency
        for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
          // latency
          for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
            // latency
            for (ap_uint<2> c11 = 0; c11 <= 1; c11 += 1) {
            #pragma HLS PIPELINE II=1
              // simd
              {
                D_t1 in_data;
                D_t16 out_data;
                in_data = fifo_D_1_local_in.read();
                int split_idx = (4*c7 + c9) % 16;
                out_data = local_D_1[c11 + 2*c5][c10 + 4*c6][(4 * c7 + c9) / 16];
                for (ap_uint<5> n = 0; n < 16; n++) {
                #pragma HLS UNROLL
                  data_split[n] = out_data(31, 0);
                  out_data = out_data >> 32;
                }
                union {unsigned int ui; float ut;} u;
                u.ut = in_data;
                data_split[split_idx] = ap_uint<32>(u.ui);
                out_data = (data_split[15], data_split[14], data_split[13], data_split[12], data_split[11], data_split[10], data_split[9], data_split[8], data_split[7], data_split[6], data_split[5], data_split[4], data_split[3], data_split[2], data_split[1], data_split[0]);                local_D_1[c11 + 2*c5][c10 + 4*c6][(4 * c7 + c9) / 16] = out_data;
              }
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void D_1_IO_L2_out_inter_trans_boundary(int c0, int c1, int c2, D_t16 *D, D_t16 local_D_1[32][32][2], hls::stream<D_t16> &fifo_D_1_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  if (!inter_trans_en) return;

  // io_L2
  for (ap_uint<6> c4 = 0; c4 <= 31; c4 += 1)
    for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1) {
      // access_coalesce
      for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1) {
      #pragma HLS PIPELINE II=1
        {
          D_t16 in_data;
          D_t16 out_data;
          in_data = local_D_1[c4][c5][c6];
          out_data = in_data;
          D[32768*c0 + 256*c1 + 2*c2 + 1024*c4 + 8*c5 + c6] = out_data;
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void D_1_IO_L2_out_boundary(D_t16 *D, hls::stream<float> &fifo_D_1_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  D_t16 local_D_1_ping[32][32][2];
  #pragma HLS RESOURCE variable=local_D_1_ping core=RAM_2P_BRAM
  D_t16 local_D_1_pong[32][32][2];
  #pragma HLS RESOURCE variable=local_D_1_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 0;
  bool intra_trans_en = 1;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  /* Variable Declaration */

  {
    for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
          for (ap_uint<3> c3 = 0; c3 <= 2; c3 += 1) {
            // array
            {
              if (arb == 0) {
                D_1_IO_L2_out_intra_trans(
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ D, 
                  /* array */ local_D_1_ping, 
                  /* fifo */ fifo_D_1_local_in, 
                  /* enable */ intra_trans_en
                );
                D_1_IO_L2_out_inter_trans_boundary(
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* array */ D, 
                  /* array */ local_D_1_pong, 
                  /* fifo */ fifo_D_1_out, 
                  /* enable */ inter_trans_en
                );
              } else {
                D_1_IO_L2_out_intra_trans(
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ D, 
                  /* array */ local_D_1_pong, 
                  /* fifo */ fifo_D_1_local_in, 
                  /* enable */ intra_trans_en
                );
                D_1_IO_L2_out_inter_trans_boundary(
                  /* host iter */ c0_prev, 
                  /* host iter */ c1_prev, 
                  /* host iter */ c2_prev, 
                  /* array */ D, 
                  /* array */ local_D_1_ping, 
                  /* fifo */ fifo_D_1_out, 
                  /* enable */ inter_trans_en
                );
              }
              inter_trans_en = 1;
              arb = !arb;
              c0_prev = c0;
              c1_prev = c1;
              c2_prev = c2;
            }
          }
    if (arb == 0) {
      D_1_IO_L2_out_inter_trans_boundary(
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ D, 
        /* array */ local_D_1_pong, 
        /* fifo */ fifo_D_1_out, 
        /* enable */ inter_trans_en
      );
    } else {
      D_1_IO_L2_out_inter_trans_boundary(
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ D, 
        /* array */ local_D_1_ping, 
        /* fifo */ fifo_D_1_out, 
        /* enable */ inter_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_intra_trans(int idx, int c0, int c1, int c2, D_t4 local_D[32][32][8], hls::stream<float> &fifo_D_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */


  // io_L1
  // pe
  for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1)
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
        // latency
        for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
          // latency
          for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
            // latency
            for (ap_uint<2> c11 = 0; c11 <= 1; c11 += 1) {
            #pragma HLS PIPELINE II=1
              // simd
              {
                D_t1 in_data;
                D_t4 out_data;
                in_data = fifo_D_drain_local_in.read();
                int split_idx = (4*c7 + c9) % 4;
                out_data = local_D[c11 + 2*c5][c10 + 4*c6][(4 * c7 + c9) / 4];
                for (ap_uint<3> n = 0; n < 4; n++) {
                #pragma HLS UNROLL
                  data_split[n] = out_data(31, 0);
                  out_data = out_data >> 32;
                }
                union {unsigned int ui; float ut;} u;
                u.ut = in_data;
                data_split[split_idx] = ap_uint<32>(u.ui);
                out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);                local_D[c11 + 2*c5][c10 + 4*c6][(4 * c7 + c9) / 4] = out_data;
              }
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_inter_trans(int idx, int c0, int c1, int c2, D_t4 local_D[32][32][8], hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  // io_L1
  if (p0 == 31) {
    for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1)
      for (ap_uint<6> c6 = 0; c6 <= 31; c6 += 1) {
        // access_coalesce
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
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
    for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1)
      for (ap_uint<6> c6 = 0; c6 <= 31; c6 += 1) {
        // access_coalesce
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
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
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_inter_trans_boundary(int idx, int c0, int c1, int c2, D_t4 local_D[32][32][8], hls::stream<D_t4> &fifo_D_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  // io_L1
  for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1)
    for (ap_uint<6> c6 = 0; c6 <= 31; c6 += 1) {
      // access_coalesce
      for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
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
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out(int idx, hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  D_t4 local_D[32][32][8];
  #pragma HLS RESOURCE variable=local_D core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
        // array
        // io_L2
        D_drain_IO_L1_out_intra_trans(
          /* module id */ idx, 
          /* host iter */ c0, 
          /* host iter */ c1, 
          /* host iter */ c2, 
          /* array */ local_D, 
          /* fifo */ fifo_D_drain_local_in
        );
        D_drain_IO_L1_out_inter_trans(
          /* module id */ idx, 
          /* host iter */ c0, 
          /* host iter */ c1, 
          /* host iter */ c2, 
          /* array */ local_D, 
          /* fifo */ fifo_D_drain_in, 
          /* fifo */ fifo_D_drain_out
        );
      }
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
  D_t4 local_D[32][32][8];
  #pragma HLS RESOURCE variable=local_D core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
        // array
        // io_L2
        D_drain_IO_L1_out_intra_trans(
          /* module id */ idx, 
          /* host iter */ c0, 
          /* host iter */ c1, 
          /* host iter */ c2, 
          /* array */ local_D, 
          /* fifo */ fifo_D_drain_local_in
        );
        D_drain_IO_L1_out_inter_trans_boundary(
          /* module id */ idx, 
          /* host iter */ c0, 
          /* host iter */ c1, 
          /* host iter */ c2, 
          /* array */ local_D, 
          /* fifo */ fifo_D_drain_out
        );
      }
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
void D_drain_IO_L2_out(D_t16 *D, hls::stream<D_t4> &fifo_D_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  D_t4 data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
        // array
        // io_L2
        // io_L1
        for (ap_uint<6> c5 = 0; c5 <= 31; c5 += 1)
          for (ap_uint<6> c6 = 0; c6 <= 31; c6 += 1) {
            // access_coalesce
            for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
            #pragma HLS PIPELINE II=1
              {
                D_t4 in_data;
                D_t16 out_data;
                in_data = fifo_D_drain_local_in.read();
                int split_idx = (131072*c0 + 1024*c1 + 8*c2 + 4096*c5 + 32*c6 + c7) % 4;
                data_split[split_idx] = in_data;
                out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);                if (c7 % 4 == 4 - 1 || c7 == 7) {
                  D[(((32 * c0 + c5) * 128 + (32 * c1 + c6)) * 128 + (32 * c2 + 4 * c7)) / 16] = out_data;
                }
              }
            }
          }
      }
}
/* Module Definition */

extern "C" {
void kernel0(A_t16 *A, B_t16 *B, C_t16 *C, D_t16 *D_0, D_t16 *D_1, D_t16 *D_2)
{
#pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem_A
#pragma HLS INTERFACE m_axi port=B offset=slave bundle=gmem_B
#pragma HLS INTERFACE m_axi port=C offset=slave bundle=gmem_C
#pragma HLS INTERFACE m_axi port=D_0 offset=slave bundle=gmem_D_0
#pragma HLS INTERFACE m_axi port=D_1 offset=slave bundle=gmem_D_1
#pragma HLS INTERFACE m_axi port=D_2 offset=slave bundle=gmem_D_2
#pragma HLS INTERFACE s_axilite port=A bundle=control
#pragma HLS INTERFACE s_axilite port=B bundle=control
#pragma HLS INTERFACE s_axilite port=C bundle=control
#pragma HLS INTERFACE s_axilite port=D_0 bundle=control
#pragma HLS INTERFACE s_axilite port=D_1 bundle=control
#pragma HLS INTERFACE s_axilite port=D_2 bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW

  /* FIFO Declaration */
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_0;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_0 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_1;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_1 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_2;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_2 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_3;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_3 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_4;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_4 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_5;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_5 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_6;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_6 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_7;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_7 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_8;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_8 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_9;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_9 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_9 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_10;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_10 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_10 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_11;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_11 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_11 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_12;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_12 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_12 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_13;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_13 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_13 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_14;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_14 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_14 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_15;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_15 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_15 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_16;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_16 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_16 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_17;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_17 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_17 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_18;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_18 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_18 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_19;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_19 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_19 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_20;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_20 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_20 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_21;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_21 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_21 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_22;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_22 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_22 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_23;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_23 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_23 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_24;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_24 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_24 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_25;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_25 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_25 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_26;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_26 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_26 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_27;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_27 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_27 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_28;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_28 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_28 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_29;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_29 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_29 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_30;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_30 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_30 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_31;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_31 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_31 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t4> fifo_A_A_IO_L1_in_32;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_32 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_32 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_0;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_0 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_1;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_1 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_2;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_2 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_3;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_3 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_4;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_4 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_5;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_5 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_6;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_6 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_7;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_7 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_8;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_8 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_9;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_9 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_9 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_10;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_10 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_10 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_11;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_11 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_11 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_12;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_12 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_12 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_13;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_13 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_13 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_14;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_14 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_14 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_15;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_15 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_15 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_16;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_16 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_16 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_17;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_17 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_17 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_18;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_18 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_18 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_19;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_19 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_19 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_20;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_20 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_20 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_21;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_21 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_21 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_22;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_22 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_22 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_23;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_23 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_23 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_24;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_24 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_24 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_25;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_25 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_25 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_26;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_26 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_26 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_27;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_27 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_27 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_28;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_28 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_28 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_29;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_29 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_29 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_30;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_30 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_30 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_31;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_31 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_31 core=FIFO_SRL
  /* B_IO_L1_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L1_in_32;
  #pragma HLS STREAM variable=fifo_B_B_IO_L1_in_32 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L1_in_32 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_0;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_0 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_1;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_1 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_2;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_2 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_3;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_3 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_4;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_4 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_5;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_5 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_6;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_6 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_7;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_7 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_8;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_8 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_9;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_9 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_9 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_10;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_10 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_10 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_11;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_11 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_11 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_12;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_12 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_12 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_13;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_13 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_13 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_14;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_14 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_14 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_15;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_15 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_15 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_16;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_16 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_16 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_17;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_17 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_17 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_18;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_18 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_18 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_19;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_19 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_19 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_20;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_20 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_20 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_21;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_21 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_21 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_22;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_22 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_22 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_23;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_23 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_23 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_24;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_24 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_24 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_25;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_25 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_25 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_26;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_26 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_26 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_27;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_27 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_27 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_28;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_28 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_28 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_29;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_29 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_29 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_30;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_30 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_30 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_31;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_31 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_31 core=FIFO_SRL
  /* C_IO_L1_in fifo */ hls::stream<C_t4> fifo_C_C_IO_L1_in_32;
  #pragma HLS STREAM variable=fifo_C_C_IO_L1_in_32 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L1_in_32 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_0;
  #pragma HLS STREAM variable=fifo_A_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_1;
  #pragma HLS STREAM variable=fifo_A_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_2;
  #pragma HLS STREAM variable=fifo_A_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_3;
  #pragma HLS STREAM variable=fifo_A_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_4;
  #pragma HLS STREAM variable=fifo_A_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_5;
  #pragma HLS STREAM variable=fifo_A_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_6;
  #pragma HLS STREAM variable=fifo_A_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_7;
  #pragma HLS STREAM variable=fifo_A_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_8;
  #pragma HLS STREAM variable=fifo_A_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_9;
  #pragma HLS STREAM variable=fifo_A_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_10;
  #pragma HLS STREAM variable=fifo_A_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_11;
  #pragma HLS STREAM variable=fifo_A_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_12;
  #pragma HLS STREAM variable=fifo_A_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_13;
  #pragma HLS STREAM variable=fifo_A_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_14;
  #pragma HLS STREAM variable=fifo_A_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_15;
  #pragma HLS STREAM variable=fifo_A_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_16;
  #pragma HLS STREAM variable=fifo_A_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_17;
  #pragma HLS STREAM variable=fifo_A_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_18;
  #pragma HLS STREAM variable=fifo_A_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_19;
  #pragma HLS STREAM variable=fifo_A_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_20;
  #pragma HLS STREAM variable=fifo_A_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_21;
  #pragma HLS STREAM variable=fifo_A_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_22;
  #pragma HLS STREAM variable=fifo_A_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_23;
  #pragma HLS STREAM variable=fifo_A_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_24;
  #pragma HLS STREAM variable=fifo_A_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_25;
  #pragma HLS STREAM variable=fifo_A_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_26;
  #pragma HLS STREAM variable=fifo_A_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_27;
  #pragma HLS STREAM variable=fifo_A_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_28;
  #pragma HLS STREAM variable=fifo_A_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_29;
  #pragma HLS STREAM variable=fifo_A_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_30;
  #pragma HLS STREAM variable=fifo_A_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_31;
  #pragma HLS STREAM variable=fifo_A_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_31 core=FIFO_SRL
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
  /* PE fifo */ hls::stream<float> fifo_B_PE_17;
  #pragma HLS STREAM variable=fifo_B_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_18;
  #pragma HLS STREAM variable=fifo_B_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_19;
  #pragma HLS STREAM variable=fifo_B_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_20;
  #pragma HLS STREAM variable=fifo_B_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_21;
  #pragma HLS STREAM variable=fifo_B_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_22;
  #pragma HLS STREAM variable=fifo_B_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_23;
  #pragma HLS STREAM variable=fifo_B_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_24;
  #pragma HLS STREAM variable=fifo_B_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_25;
  #pragma HLS STREAM variable=fifo_B_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_26;
  #pragma HLS STREAM variable=fifo_B_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_27;
  #pragma HLS STREAM variable=fifo_B_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_28;
  #pragma HLS STREAM variable=fifo_B_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_29;
  #pragma HLS STREAM variable=fifo_B_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_30;
  #pragma HLS STREAM variable=fifo_B_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_31;
  #pragma HLS STREAM variable=fifo_B_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_0;
  #pragma HLS STREAM variable=fifo_C_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_1;
  #pragma HLS STREAM variable=fifo_C_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_2;
  #pragma HLS STREAM variable=fifo_C_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_3;
  #pragma HLS STREAM variable=fifo_C_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_4;
  #pragma HLS STREAM variable=fifo_C_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_5;
  #pragma HLS STREAM variable=fifo_C_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_6;
  #pragma HLS STREAM variable=fifo_C_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_7;
  #pragma HLS STREAM variable=fifo_C_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_8;
  #pragma HLS STREAM variable=fifo_C_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_9;
  #pragma HLS STREAM variable=fifo_C_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_10;
  #pragma HLS STREAM variable=fifo_C_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_11;
  #pragma HLS STREAM variable=fifo_C_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_12;
  #pragma HLS STREAM variable=fifo_C_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_13;
  #pragma HLS STREAM variable=fifo_C_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_14;
  #pragma HLS STREAM variable=fifo_C_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_15;
  #pragma HLS STREAM variable=fifo_C_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_16;
  #pragma HLS STREAM variable=fifo_C_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_17;
  #pragma HLS STREAM variable=fifo_C_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_18;
  #pragma HLS STREAM variable=fifo_C_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_19;
  #pragma HLS STREAM variable=fifo_C_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_20;
  #pragma HLS STREAM variable=fifo_C_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_21;
  #pragma HLS STREAM variable=fifo_C_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_22;
  #pragma HLS STREAM variable=fifo_C_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_23;
  #pragma HLS STREAM variable=fifo_C_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_24;
  #pragma HLS STREAM variable=fifo_C_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_25;
  #pragma HLS STREAM variable=fifo_C_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_26;
  #pragma HLS STREAM variable=fifo_C_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_27;
  #pragma HLS STREAM variable=fifo_C_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_28;
  #pragma HLS STREAM variable=fifo_C_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_29;
  #pragma HLS STREAM variable=fifo_C_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_30;
  #pragma HLS STREAM variable=fifo_C_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t2> fifo_C_PE_31;
  #pragma HLS STREAM variable=fifo_C_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_0;
  #pragma HLS STREAM variable=fifo_D_1_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_1;
  #pragma HLS STREAM variable=fifo_D_1_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_2;
  #pragma HLS STREAM variable=fifo_D_1_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_3;
  #pragma HLS STREAM variable=fifo_D_1_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_4;
  #pragma HLS STREAM variable=fifo_D_1_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_5;
  #pragma HLS STREAM variable=fifo_D_1_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_6;
  #pragma HLS STREAM variable=fifo_D_1_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_7;
  #pragma HLS STREAM variable=fifo_D_1_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_8;
  #pragma HLS STREAM variable=fifo_D_1_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_9;
  #pragma HLS STREAM variable=fifo_D_1_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_10;
  #pragma HLS STREAM variable=fifo_D_1_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_11;
  #pragma HLS STREAM variable=fifo_D_1_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_12;
  #pragma HLS STREAM variable=fifo_D_1_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_13;
  #pragma HLS STREAM variable=fifo_D_1_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_14;
  #pragma HLS STREAM variable=fifo_D_1_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_15;
  #pragma HLS STREAM variable=fifo_D_1_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_16;
  #pragma HLS STREAM variable=fifo_D_1_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_17;
  #pragma HLS STREAM variable=fifo_D_1_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_18;
  #pragma HLS STREAM variable=fifo_D_1_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_19;
  #pragma HLS STREAM variable=fifo_D_1_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_20;
  #pragma HLS STREAM variable=fifo_D_1_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_21;
  #pragma HLS STREAM variable=fifo_D_1_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_22;
  #pragma HLS STREAM variable=fifo_D_1_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_23;
  #pragma HLS STREAM variable=fifo_D_1_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_24;
  #pragma HLS STREAM variable=fifo_D_1_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_25;
  #pragma HLS STREAM variable=fifo_D_1_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_26;
  #pragma HLS STREAM variable=fifo_D_1_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_27;
  #pragma HLS STREAM variable=fifo_D_1_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_28;
  #pragma HLS STREAM variable=fifo_D_1_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_29;
  #pragma HLS STREAM variable=fifo_D_1_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_30;
  #pragma HLS STREAM variable=fifo_D_1_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_31;
  #pragma HLS STREAM variable=fifo_D_1_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_1_PE_32;
  #pragma HLS STREAM variable=fifo_D_1_PE_32 depth=2
  #pragma HLS RESOURCE variable=fifo_D_1_PE_32 core=FIFO_SRL
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
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_16;
  #pragma HLS STREAM variable=fifo_D_drain_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_17;
  #pragma HLS STREAM variable=fifo_D_drain_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_18;
  #pragma HLS STREAM variable=fifo_D_drain_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_19;
  #pragma HLS STREAM variable=fifo_D_drain_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_20;
  #pragma HLS STREAM variable=fifo_D_drain_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_21;
  #pragma HLS STREAM variable=fifo_D_drain_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_22;
  #pragma HLS STREAM variable=fifo_D_drain_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_23;
  #pragma HLS STREAM variable=fifo_D_drain_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_24;
  #pragma HLS STREAM variable=fifo_D_drain_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_25;
  #pragma HLS STREAM variable=fifo_D_drain_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_26;
  #pragma HLS STREAM variable=fifo_D_drain_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_27;
  #pragma HLS STREAM variable=fifo_D_drain_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_28;
  #pragma HLS STREAM variable=fifo_D_drain_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_29;
  #pragma HLS STREAM variable=fifo_D_drain_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_30;
  #pragma HLS STREAM variable=fifo_D_drain_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_31;
  #pragma HLS STREAM variable=fifo_D_drain_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_31 core=FIFO_SRL
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
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_17;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_17 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_17 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_18;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_18 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_18 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_19;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_19 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_19 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_20;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_20 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_20 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_21;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_21 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_21 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_22;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_22 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_22 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_23;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_23 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_23 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_24;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_24 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_24 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_25;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_25 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_25 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_26;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_26 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_26 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_27;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_27 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_27 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_28;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_28 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_28 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_29;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_29 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_29 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_30;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_30 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_30 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_31;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_31 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_31 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_32;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_32 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_32 core=FIFO_SRL
  /* FIFO Declaration */

  /* Module Call */
  A_IO_L2_in(
    /* array */ A,
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
  A_IO_L1_in_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_A_A_IO_L1_in_15,
    /* fifo */ fifo_A_A_IO_L1_in_16,
    /* fifo */ fifo_A_PE_15
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 16,
    /* fifo */ fifo_A_A_IO_L1_in_16,
    /* fifo */ fifo_A_A_IO_L1_in_17,
    /* fifo */ fifo_A_PE_16
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 17,
    /* fifo */ fifo_A_A_IO_L1_in_17,
    /* fifo */ fifo_A_A_IO_L1_in_18,
    /* fifo */ fifo_A_PE_17
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 18,
    /* fifo */ fifo_A_A_IO_L1_in_18,
    /* fifo */ fifo_A_A_IO_L1_in_19,
    /* fifo */ fifo_A_PE_18
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 19,
    /* fifo */ fifo_A_A_IO_L1_in_19,
    /* fifo */ fifo_A_A_IO_L1_in_20,
    /* fifo */ fifo_A_PE_19
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 20,
    /* fifo */ fifo_A_A_IO_L1_in_20,
    /* fifo */ fifo_A_A_IO_L1_in_21,
    /* fifo */ fifo_A_PE_20
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 21,
    /* fifo */ fifo_A_A_IO_L1_in_21,
    /* fifo */ fifo_A_A_IO_L1_in_22,
    /* fifo */ fifo_A_PE_21
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 22,
    /* fifo */ fifo_A_A_IO_L1_in_22,
    /* fifo */ fifo_A_A_IO_L1_in_23,
    /* fifo */ fifo_A_PE_22
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 23,
    /* fifo */ fifo_A_A_IO_L1_in_23,
    /* fifo */ fifo_A_A_IO_L1_in_24,
    /* fifo */ fifo_A_PE_23
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 24,
    /* fifo */ fifo_A_A_IO_L1_in_24,
    /* fifo */ fifo_A_A_IO_L1_in_25,
    /* fifo */ fifo_A_PE_24
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 25,
    /* fifo */ fifo_A_A_IO_L1_in_25,
    /* fifo */ fifo_A_A_IO_L1_in_26,
    /* fifo */ fifo_A_PE_25
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 26,
    /* fifo */ fifo_A_A_IO_L1_in_26,
    /* fifo */ fifo_A_A_IO_L1_in_27,
    /* fifo */ fifo_A_PE_26
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 27,
    /* fifo */ fifo_A_A_IO_L1_in_27,
    /* fifo */ fifo_A_A_IO_L1_in_28,
    /* fifo */ fifo_A_PE_27
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 28,
    /* fifo */ fifo_A_A_IO_L1_in_28,
    /* fifo */ fifo_A_A_IO_L1_in_29,
    /* fifo */ fifo_A_PE_28
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 29,
    /* fifo */ fifo_A_A_IO_L1_in_29,
    /* fifo */ fifo_A_A_IO_L1_in_30,
    /* fifo */ fifo_A_PE_29
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 30,
    /* fifo */ fifo_A_A_IO_L1_in_30,
    /* fifo */ fifo_A_A_IO_L1_in_31,
    /* fifo */ fifo_A_PE_30
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_boundary_wrapper(
    /* module id */ 31,
    /* fifo */ fifo_A_A_IO_L1_in_31,
    /* fifo */ fifo_A_PE_31
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* array */ B,
    /* fifo */ fifo_B_B_IO_L1_in_0
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_B_B_IO_L1_in_0,
    /* fifo */ fifo_B_B_IO_L1_in_1,
    /* fifo */ fifo_B_PE_0
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_B_B_IO_L1_in_1,
    /* fifo */ fifo_B_B_IO_L1_in_2,
    /* fifo */ fifo_B_PE_1
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_B_B_IO_L1_in_2,
    /* fifo */ fifo_B_B_IO_L1_in_3,
    /* fifo */ fifo_B_PE_2
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_B_B_IO_L1_in_3,
    /* fifo */ fifo_B_B_IO_L1_in_4,
    /* fifo */ fifo_B_PE_3
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_B_B_IO_L1_in_4,
    /* fifo */ fifo_B_B_IO_L1_in_5,
    /* fifo */ fifo_B_PE_4
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_B_B_IO_L1_in_5,
    /* fifo */ fifo_B_B_IO_L1_in_6,
    /* fifo */ fifo_B_PE_5
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_B_B_IO_L1_in_6,
    /* fifo */ fifo_B_B_IO_L1_in_7,
    /* fifo */ fifo_B_PE_6
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_B_B_IO_L1_in_7,
    /* fifo */ fifo_B_B_IO_L1_in_8,
    /* fifo */ fifo_B_PE_7
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_B_B_IO_L1_in_8,
    /* fifo */ fifo_B_B_IO_L1_in_9,
    /* fifo */ fifo_B_PE_8
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_B_B_IO_L1_in_9,
    /* fifo */ fifo_B_B_IO_L1_in_10,
    /* fifo */ fifo_B_PE_9
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_B_B_IO_L1_in_10,
    /* fifo */ fifo_B_B_IO_L1_in_11,
    /* fifo */ fifo_B_PE_10
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_B_B_IO_L1_in_11,
    /* fifo */ fifo_B_B_IO_L1_in_12,
    /* fifo */ fifo_B_PE_11
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_B_B_IO_L1_in_12,
    /* fifo */ fifo_B_B_IO_L1_in_13,
    /* fifo */ fifo_B_PE_12
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_B_B_IO_L1_in_13,
    /* fifo */ fifo_B_B_IO_L1_in_14,
    /* fifo */ fifo_B_PE_13
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_B_B_IO_L1_in_14,
    /* fifo */ fifo_B_B_IO_L1_in_15,
    /* fifo */ fifo_B_PE_14
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_B_B_IO_L1_in_15,
    /* fifo */ fifo_B_B_IO_L1_in_16,
    /* fifo */ fifo_B_PE_15
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 16,
    /* fifo */ fifo_B_B_IO_L1_in_16,
    /* fifo */ fifo_B_B_IO_L1_in_17,
    /* fifo */ fifo_B_PE_16
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 17,
    /* fifo */ fifo_B_B_IO_L1_in_17,
    /* fifo */ fifo_B_B_IO_L1_in_18,
    /* fifo */ fifo_B_PE_17
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 18,
    /* fifo */ fifo_B_B_IO_L1_in_18,
    /* fifo */ fifo_B_B_IO_L1_in_19,
    /* fifo */ fifo_B_PE_18
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 19,
    /* fifo */ fifo_B_B_IO_L1_in_19,
    /* fifo */ fifo_B_B_IO_L1_in_20,
    /* fifo */ fifo_B_PE_19
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 20,
    /* fifo */ fifo_B_B_IO_L1_in_20,
    /* fifo */ fifo_B_B_IO_L1_in_21,
    /* fifo */ fifo_B_PE_20
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 21,
    /* fifo */ fifo_B_B_IO_L1_in_21,
    /* fifo */ fifo_B_B_IO_L1_in_22,
    /* fifo */ fifo_B_PE_21
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 22,
    /* fifo */ fifo_B_B_IO_L1_in_22,
    /* fifo */ fifo_B_B_IO_L1_in_23,
    /* fifo */ fifo_B_PE_22
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 23,
    /* fifo */ fifo_B_B_IO_L1_in_23,
    /* fifo */ fifo_B_B_IO_L1_in_24,
    /* fifo */ fifo_B_PE_23
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 24,
    /* fifo */ fifo_B_B_IO_L1_in_24,
    /* fifo */ fifo_B_B_IO_L1_in_25,
    /* fifo */ fifo_B_PE_24
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 25,
    /* fifo */ fifo_B_B_IO_L1_in_25,
    /* fifo */ fifo_B_B_IO_L1_in_26,
    /* fifo */ fifo_B_PE_25
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 26,
    /* fifo */ fifo_B_B_IO_L1_in_26,
    /* fifo */ fifo_B_B_IO_L1_in_27,
    /* fifo */ fifo_B_PE_26
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 27,
    /* fifo */ fifo_B_B_IO_L1_in_27,
    /* fifo */ fifo_B_B_IO_L1_in_28,
    /* fifo */ fifo_B_PE_27
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 28,
    /* fifo */ fifo_B_B_IO_L1_in_28,
    /* fifo */ fifo_B_B_IO_L1_in_29,
    /* fifo */ fifo_B_PE_28
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 29,
    /* fifo */ fifo_B_B_IO_L1_in_29,
    /* fifo */ fifo_B_B_IO_L1_in_30,
    /* fifo */ fifo_B_PE_29
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_wrapper(
    /* module id */ 30,
    /* fifo */ fifo_B_B_IO_L1_in_30,
    /* fifo */ fifo_B_B_IO_L1_in_31,
    /* fifo */ fifo_B_PE_30
  );
  /* Module Call */

  /* Module Call */
  B_IO_L1_in_boundary_wrapper(
    /* module id */ 31,
    /* fifo */ fifo_B_B_IO_L1_in_31,
    /* fifo */ fifo_B_PE_31
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in(
    /* array */ C,
    /* fifo */ fifo_C_C_IO_L1_in_0
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_C_C_IO_L1_in_0,
    /* fifo */ fifo_C_C_IO_L1_in_1,
    /* fifo */ fifo_C_PE_0
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_C_C_IO_L1_in_1,
    /* fifo */ fifo_C_C_IO_L1_in_2,
    /* fifo */ fifo_C_PE_1
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_C_C_IO_L1_in_2,
    /* fifo */ fifo_C_C_IO_L1_in_3,
    /* fifo */ fifo_C_PE_2
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_C_C_IO_L1_in_3,
    /* fifo */ fifo_C_C_IO_L1_in_4,
    /* fifo */ fifo_C_PE_3
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_C_C_IO_L1_in_4,
    /* fifo */ fifo_C_C_IO_L1_in_5,
    /* fifo */ fifo_C_PE_4
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_C_C_IO_L1_in_5,
    /* fifo */ fifo_C_C_IO_L1_in_6,
    /* fifo */ fifo_C_PE_5
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_C_C_IO_L1_in_6,
    /* fifo */ fifo_C_C_IO_L1_in_7,
    /* fifo */ fifo_C_PE_6
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_C_C_IO_L1_in_7,
    /* fifo */ fifo_C_C_IO_L1_in_8,
    /* fifo */ fifo_C_PE_7
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_C_C_IO_L1_in_8,
    /* fifo */ fifo_C_C_IO_L1_in_9,
    /* fifo */ fifo_C_PE_8
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_C_C_IO_L1_in_9,
    /* fifo */ fifo_C_C_IO_L1_in_10,
    /* fifo */ fifo_C_PE_9
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_C_C_IO_L1_in_10,
    /* fifo */ fifo_C_C_IO_L1_in_11,
    /* fifo */ fifo_C_PE_10
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_C_C_IO_L1_in_11,
    /* fifo */ fifo_C_C_IO_L1_in_12,
    /* fifo */ fifo_C_PE_11
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_C_C_IO_L1_in_12,
    /* fifo */ fifo_C_C_IO_L1_in_13,
    /* fifo */ fifo_C_PE_12
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_C_C_IO_L1_in_13,
    /* fifo */ fifo_C_C_IO_L1_in_14,
    /* fifo */ fifo_C_PE_13
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_C_C_IO_L1_in_14,
    /* fifo */ fifo_C_C_IO_L1_in_15,
    /* fifo */ fifo_C_PE_14
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_C_C_IO_L1_in_15,
    /* fifo */ fifo_C_C_IO_L1_in_16,
    /* fifo */ fifo_C_PE_15
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 16,
    /* fifo */ fifo_C_C_IO_L1_in_16,
    /* fifo */ fifo_C_C_IO_L1_in_17,
    /* fifo */ fifo_C_PE_16
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 17,
    /* fifo */ fifo_C_C_IO_L1_in_17,
    /* fifo */ fifo_C_C_IO_L1_in_18,
    /* fifo */ fifo_C_PE_17
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 18,
    /* fifo */ fifo_C_C_IO_L1_in_18,
    /* fifo */ fifo_C_C_IO_L1_in_19,
    /* fifo */ fifo_C_PE_18
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 19,
    /* fifo */ fifo_C_C_IO_L1_in_19,
    /* fifo */ fifo_C_C_IO_L1_in_20,
    /* fifo */ fifo_C_PE_19
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 20,
    /* fifo */ fifo_C_C_IO_L1_in_20,
    /* fifo */ fifo_C_C_IO_L1_in_21,
    /* fifo */ fifo_C_PE_20
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 21,
    /* fifo */ fifo_C_C_IO_L1_in_21,
    /* fifo */ fifo_C_C_IO_L1_in_22,
    /* fifo */ fifo_C_PE_21
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 22,
    /* fifo */ fifo_C_C_IO_L1_in_22,
    /* fifo */ fifo_C_C_IO_L1_in_23,
    /* fifo */ fifo_C_PE_22
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 23,
    /* fifo */ fifo_C_C_IO_L1_in_23,
    /* fifo */ fifo_C_C_IO_L1_in_24,
    /* fifo */ fifo_C_PE_23
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 24,
    /* fifo */ fifo_C_C_IO_L1_in_24,
    /* fifo */ fifo_C_C_IO_L1_in_25,
    /* fifo */ fifo_C_PE_24
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 25,
    /* fifo */ fifo_C_C_IO_L1_in_25,
    /* fifo */ fifo_C_C_IO_L1_in_26,
    /* fifo */ fifo_C_PE_25
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 26,
    /* fifo */ fifo_C_C_IO_L1_in_26,
    /* fifo */ fifo_C_C_IO_L1_in_27,
    /* fifo */ fifo_C_PE_26
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 27,
    /* fifo */ fifo_C_C_IO_L1_in_27,
    /* fifo */ fifo_C_C_IO_L1_in_28,
    /* fifo */ fifo_C_PE_27
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 28,
    /* fifo */ fifo_C_C_IO_L1_in_28,
    /* fifo */ fifo_C_C_IO_L1_in_29,
    /* fifo */ fifo_C_PE_28
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 29,
    /* fifo */ fifo_C_C_IO_L1_in_29,
    /* fifo */ fifo_C_C_IO_L1_in_30,
    /* fifo */ fifo_C_PE_29
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_wrapper(
    /* module id */ 30,
    /* fifo */ fifo_C_C_IO_L1_in_30,
    /* fifo */ fifo_C_C_IO_L1_in_31,
    /* fifo */ fifo_C_PE_30
  );
  /* Module Call */

  /* Module Call */
  C_IO_L1_in_boundary_wrapper(
    /* module id */ 31,
    /* fifo */ fifo_C_C_IO_L1_in_31,
    /* fifo */ fifo_C_PE_31
  );
  /* Module Call */

  /* Module Call */
  D_1_IO_L2_in_boundary(
    /* array */ D_0,
    /* fifo */ fifo_D_1_PE_0
  );
  /* Module Call */

  /* Module Call */
  D_1_PE_dummy_out(
    /* module id */ 0,
    /* fifo */ fifo_D_1_PE_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_A_PE_0,
    /* fifo */ fifo_B_PE_0,
    /* fifo */ fifo_C_PE_0,
    /* fifo */ fifo_D_1_PE_0,
    /* fifo */ fifo_D_1_PE_1,
    /* fifo */ fifo_D_drain_PE_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_A_PE_1,
    /* fifo */ fifo_B_PE_1,
    /* fifo */ fifo_C_PE_1,
    /* fifo */ fifo_D_1_PE_1,
    /* fifo */ fifo_D_1_PE_2,
    /* fifo */ fifo_D_drain_PE_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_A_PE_2,
    /* fifo */ fifo_B_PE_2,
    /* fifo */ fifo_C_PE_2,
    /* fifo */ fifo_D_1_PE_2,
    /* fifo */ fifo_D_1_PE_3,
    /* fifo */ fifo_D_drain_PE_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_A_PE_3,
    /* fifo */ fifo_B_PE_3,
    /* fifo */ fifo_C_PE_3,
    /* fifo */ fifo_D_1_PE_3,
    /* fifo */ fifo_D_1_PE_4,
    /* fifo */ fifo_D_drain_PE_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_A_PE_4,
    /* fifo */ fifo_B_PE_4,
    /* fifo */ fifo_C_PE_4,
    /* fifo */ fifo_D_1_PE_4,
    /* fifo */ fifo_D_1_PE_5,
    /* fifo */ fifo_D_drain_PE_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_A_PE_5,
    /* fifo */ fifo_B_PE_5,
    /* fifo */ fifo_C_PE_5,
    /* fifo */ fifo_D_1_PE_5,
    /* fifo */ fifo_D_1_PE_6,
    /* fifo */ fifo_D_drain_PE_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_A_PE_6,
    /* fifo */ fifo_B_PE_6,
    /* fifo */ fifo_C_PE_6,
    /* fifo */ fifo_D_1_PE_6,
    /* fifo */ fifo_D_1_PE_7,
    /* fifo */ fifo_D_drain_PE_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_A_PE_7,
    /* fifo */ fifo_B_PE_7,
    /* fifo */ fifo_C_PE_7,
    /* fifo */ fifo_D_1_PE_7,
    /* fifo */ fifo_D_1_PE_8,
    /* fifo */ fifo_D_drain_PE_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_A_PE_8,
    /* fifo */ fifo_B_PE_8,
    /* fifo */ fifo_C_PE_8,
    /* fifo */ fifo_D_1_PE_8,
    /* fifo */ fifo_D_1_PE_9,
    /* fifo */ fifo_D_drain_PE_8
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_A_PE_9,
    /* fifo */ fifo_B_PE_9,
    /* fifo */ fifo_C_PE_9,
    /* fifo */ fifo_D_1_PE_9,
    /* fifo */ fifo_D_1_PE_10,
    /* fifo */ fifo_D_drain_PE_9
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_A_PE_10,
    /* fifo */ fifo_B_PE_10,
    /* fifo */ fifo_C_PE_10,
    /* fifo */ fifo_D_1_PE_10,
    /* fifo */ fifo_D_1_PE_11,
    /* fifo */ fifo_D_drain_PE_10
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_A_PE_11,
    /* fifo */ fifo_B_PE_11,
    /* fifo */ fifo_C_PE_11,
    /* fifo */ fifo_D_1_PE_11,
    /* fifo */ fifo_D_1_PE_12,
    /* fifo */ fifo_D_drain_PE_11
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_A_PE_12,
    /* fifo */ fifo_B_PE_12,
    /* fifo */ fifo_C_PE_12,
    /* fifo */ fifo_D_1_PE_12,
    /* fifo */ fifo_D_1_PE_13,
    /* fifo */ fifo_D_drain_PE_12
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_A_PE_13,
    /* fifo */ fifo_B_PE_13,
    /* fifo */ fifo_C_PE_13,
    /* fifo */ fifo_D_1_PE_13,
    /* fifo */ fifo_D_1_PE_14,
    /* fifo */ fifo_D_drain_PE_13
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_A_PE_14,
    /* fifo */ fifo_B_PE_14,
    /* fifo */ fifo_C_PE_14,
    /* fifo */ fifo_D_1_PE_14,
    /* fifo */ fifo_D_1_PE_15,
    /* fifo */ fifo_D_drain_PE_14
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_A_PE_15,
    /* fifo */ fifo_B_PE_15,
    /* fifo */ fifo_C_PE_15,
    /* fifo */ fifo_D_1_PE_15,
    /* fifo */ fifo_D_1_PE_16,
    /* fifo */ fifo_D_drain_PE_15
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 16,
    /* fifo */ fifo_A_PE_16,
    /* fifo */ fifo_B_PE_16,
    /* fifo */ fifo_C_PE_16,
    /* fifo */ fifo_D_1_PE_16,
    /* fifo */ fifo_D_1_PE_17,
    /* fifo */ fifo_D_drain_PE_16
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 17,
    /* fifo */ fifo_A_PE_17,
    /* fifo */ fifo_B_PE_17,
    /* fifo */ fifo_C_PE_17,
    /* fifo */ fifo_D_1_PE_17,
    /* fifo */ fifo_D_1_PE_18,
    /* fifo */ fifo_D_drain_PE_17
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 18,
    /* fifo */ fifo_A_PE_18,
    /* fifo */ fifo_B_PE_18,
    /* fifo */ fifo_C_PE_18,
    /* fifo */ fifo_D_1_PE_18,
    /* fifo */ fifo_D_1_PE_19,
    /* fifo */ fifo_D_drain_PE_18
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 19,
    /* fifo */ fifo_A_PE_19,
    /* fifo */ fifo_B_PE_19,
    /* fifo */ fifo_C_PE_19,
    /* fifo */ fifo_D_1_PE_19,
    /* fifo */ fifo_D_1_PE_20,
    /* fifo */ fifo_D_drain_PE_19
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 20,
    /* fifo */ fifo_A_PE_20,
    /* fifo */ fifo_B_PE_20,
    /* fifo */ fifo_C_PE_20,
    /* fifo */ fifo_D_1_PE_20,
    /* fifo */ fifo_D_1_PE_21,
    /* fifo */ fifo_D_drain_PE_20
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 21,
    /* fifo */ fifo_A_PE_21,
    /* fifo */ fifo_B_PE_21,
    /* fifo */ fifo_C_PE_21,
    /* fifo */ fifo_D_1_PE_21,
    /* fifo */ fifo_D_1_PE_22,
    /* fifo */ fifo_D_drain_PE_21
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 22,
    /* fifo */ fifo_A_PE_22,
    /* fifo */ fifo_B_PE_22,
    /* fifo */ fifo_C_PE_22,
    /* fifo */ fifo_D_1_PE_22,
    /* fifo */ fifo_D_1_PE_23,
    /* fifo */ fifo_D_drain_PE_22
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 23,
    /* fifo */ fifo_A_PE_23,
    /* fifo */ fifo_B_PE_23,
    /* fifo */ fifo_C_PE_23,
    /* fifo */ fifo_D_1_PE_23,
    /* fifo */ fifo_D_1_PE_24,
    /* fifo */ fifo_D_drain_PE_23
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 24,
    /* fifo */ fifo_A_PE_24,
    /* fifo */ fifo_B_PE_24,
    /* fifo */ fifo_C_PE_24,
    /* fifo */ fifo_D_1_PE_24,
    /* fifo */ fifo_D_1_PE_25,
    /* fifo */ fifo_D_drain_PE_24
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 25,
    /* fifo */ fifo_A_PE_25,
    /* fifo */ fifo_B_PE_25,
    /* fifo */ fifo_C_PE_25,
    /* fifo */ fifo_D_1_PE_25,
    /* fifo */ fifo_D_1_PE_26,
    /* fifo */ fifo_D_drain_PE_25
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 26,
    /* fifo */ fifo_A_PE_26,
    /* fifo */ fifo_B_PE_26,
    /* fifo */ fifo_C_PE_26,
    /* fifo */ fifo_D_1_PE_26,
    /* fifo */ fifo_D_1_PE_27,
    /* fifo */ fifo_D_drain_PE_26
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 27,
    /* fifo */ fifo_A_PE_27,
    /* fifo */ fifo_B_PE_27,
    /* fifo */ fifo_C_PE_27,
    /* fifo */ fifo_D_1_PE_27,
    /* fifo */ fifo_D_1_PE_28,
    /* fifo */ fifo_D_drain_PE_27
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 28,
    /* fifo */ fifo_A_PE_28,
    /* fifo */ fifo_B_PE_28,
    /* fifo */ fifo_C_PE_28,
    /* fifo */ fifo_D_1_PE_28,
    /* fifo */ fifo_D_1_PE_29,
    /* fifo */ fifo_D_drain_PE_28
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 29,
    /* fifo */ fifo_A_PE_29,
    /* fifo */ fifo_B_PE_29,
    /* fifo */ fifo_C_PE_29,
    /* fifo */ fifo_D_1_PE_29,
    /* fifo */ fifo_D_1_PE_30,
    /* fifo */ fifo_D_drain_PE_29
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 30,
    /* fifo */ fifo_A_PE_30,
    /* fifo */ fifo_B_PE_30,
    /* fifo */ fifo_C_PE_30,
    /* fifo */ fifo_D_1_PE_30,
    /* fifo */ fifo_D_1_PE_31,
    /* fifo */ fifo_D_drain_PE_30
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 31,
    /* fifo */ fifo_A_PE_31,
    /* fifo */ fifo_B_PE_31,
    /* fifo */ fifo_C_PE_31,
    /* fifo */ fifo_D_1_PE_31,
    /* fifo */ fifo_D_1_PE_32,
    /* fifo */ fifo_D_drain_PE_31
  );
  /* Module Call */

  /* Module Call */
  D_1_IO_L2_out_boundary(
    /* array */ D_1,
    /* fifo */ fifo_D_1_PE_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 31,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_31,
    /* fifo */ fifo_D_drain_PE_31
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 30,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_31,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_30,
    /* fifo */ fifo_D_drain_PE_30
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 29,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_30,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_29,
    /* fifo */ fifo_D_drain_PE_29
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 28,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_29,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_28,
    /* fifo */ fifo_D_drain_PE_28
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 27,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_28,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_27,
    /* fifo */ fifo_D_drain_PE_27
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 26,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_27,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_26,
    /* fifo */ fifo_D_drain_PE_26
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 25,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_26,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_25,
    /* fifo */ fifo_D_drain_PE_25
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 24,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_25,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_24,
    /* fifo */ fifo_D_drain_PE_24
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 23,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_24,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_23,
    /* fifo */ fifo_D_drain_PE_23
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 22,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_23,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_22,
    /* fifo */ fifo_D_drain_PE_22
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 21,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_22,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_21,
    /* fifo */ fifo_D_drain_PE_21
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 20,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_21,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_20,
    /* fifo */ fifo_D_drain_PE_20
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 19,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_20,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_19,
    /* fifo */ fifo_D_drain_PE_19
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 18,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_19,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_18,
    /* fifo */ fifo_D_drain_PE_18
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 17,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_18,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_17,
    /* fifo */ fifo_D_drain_PE_17
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 16,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_17,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_16,
    /* fifo */ fifo_D_drain_PE_16
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_16,
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
    /* array */ D_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0
  );
  /* Module Call */

}
}

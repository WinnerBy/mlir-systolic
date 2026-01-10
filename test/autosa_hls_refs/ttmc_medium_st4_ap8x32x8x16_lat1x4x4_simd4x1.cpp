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
typedef ap_uint<128> A_t4;
typedef ap_uint<128> B_t4;
typedef ap_uint<512> C_t16;
typedef ap_uint<128> C_t4;
typedef ap_uint<256> D_t8;
typedef ap_uint<128> D_t4;
/* Data Type */

extern "C" {
void kernel0(A_t16 *A, B_t4 *B, C_t16 *C, D_t8 *D);
}
void A_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, int c3, A_t16 local_A[1][16][8], hls::stream<A_t4> &fifo_A_local_out, bool intra_trans_en);
void A_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, int c3, A_t16 local_A[1][16][8], hls::stream<A_t16> &fifo_A_in, hls::stream<A_t16> &fifo_A_out, bool inter_trans_en);
void A_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, int c3, A_t16 local_A[1][16][8], hls::stream<A_t16> &fifo_A_in, bool inter_trans_en);
void B_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[16][1], hls::stream<float> &fifo_B_local_out, bool intra_trans_en);
void B_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[16][1], hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, bool inter_trans_en);
void B_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[16][1], hls::stream<B_t4> &fifo_B_in, bool inter_trans_en);
void C_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, C_t16 local_C[8][8], hls::stream<C_t4> &fifo_C_local_out, bool intra_trans_en);
void C_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, C_t16 local_C[8][8], hls::stream<C_t16> &fifo_C_in, hls::stream<C_t16> &fifo_C_out, bool inter_trans_en);
void C_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, C_t16 local_C[8][8], hls::stream<C_t16> &fifo_C_in, bool inter_trans_en);
void PE_wrapper(int idx, int idy, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, hls::stream<C_t4> &fifo_C_in, hls::stream<C_t4> &fifo_C_out, hls::stream<float> &fifo_D_drain_out);
void D_drain_IO_L1_out_intra_trans(int idx, int idy, int c0, int c1, int c2, D_t4 local_D[1][4][2], hls::stream<float> &fifo_D_drain_local_in);
void D_drain_IO_L1_out_inter_trans(int idx, int idy, int c0, int c1, int c2, D_t4 local_D[1][4][2], hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out);
void D_drain_IO_L1_out_inter_trans_boundary(int idx, int idy, int c0, int c1, int c2, D_t4 local_D[1][4][2], hls::stream<D_t4> &fifo_D_drain_out);
void D_drain_IO_L1_out_wrapper(int idx, int idy, hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in);
void D_drain_IO_L1_out_boundary_wrapper(int idx, int idy, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in);

/* Module Definition */
void A_IO_L3_in(A_t16 *A, hls::stream<A_t16> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
          // array
          // io_L3
          for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
            // io_L2
            for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
              // access_coalesce
              for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
              #pragma HLS PIPELINE II=1
                {
                  A_t16 in_data;
                  A_t16 out_data;
                  in_data = A[8192*c0 + 128*c3 + 1024*c4 + 8*c6 + c7];
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
void A_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, int c3, A_t16 local_A[1][16][8], hls::stream<A_t4> &fifo_A_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t4 data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
    for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1)
      for (ap_uint<6> c8 = 0; c8 <= 31; c8 += 1) {
        // latency
        for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
          // latency
          for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
          #pragma HLS PIPELINE II=1
            // simd
            {
              A_t16 in_data;
              A_t4 out_data;
              in_data = local_A[0][c7][4 * c8 / 16];
              for (ap_uint<3> n = 0; n < 4; n++) {
              #pragma HLS UNROLL
                data_split[n] = in_data(127, 0);
                in_data = in_data >> 128;
              }
              int split_idx = (c8) % 4;
              out_data = data_split[split_idx];
              fifo_A_local_out.write(out_data);
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, int c3, A_t16 local_A[1][16][8], hls::stream<A_t16> &fifo_A_in, hls::stream<A_t16> &fifo_A_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c4 = p0; c4 <= 7; c4 += 1) {
    // io_L2
    if (c4 == p0) {
      for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
        // access_coalesce
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t16 in_data;
            A_t16 out_data;
            in_data = fifo_A_in.read();
            out_data = in_data;
            local_A[0][c6][c7] = out_data;
          }
        }
      }
    } else {
      for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
        // access_coalesce
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t16 in_data;
            A_t16 out_data;
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
void A_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, int c3, A_t16 local_A[1][16][8], hls::stream<A_t16> &fifo_A_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c4 = p0; c4 <= 7; c4 += 1)
    if (c4 == p0) {
      // io_L2
      for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
        // access_coalesce
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t16 in_data;
            A_t16 out_data;
            in_data = fifo_A_in.read();
            out_data = in_data;
            local_A[0][c6][c7] = out_data;
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in(int idx, hls::stream<A_t16> &fifo_A_in, hls::stream<A_t16> &fifo_A_out, hls::stream<A_t4> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t16 local_A_ping[1][16][8];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_2P_BRAM
  A_t16 local_A_pong[1][16][8];
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
    for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1)
          for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
            // array
            // io_L3
            {
              if (arb == 0) {
                A_IO_L2_in_inter_trans(
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
                A_IO_L2_in_intra_trans(
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
                A_IO_L2_in_inter_trans(
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
                A_IO_L2_in_intra_trans(
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
      A_IO_L2_in_intra_trans(
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
      A_IO_L2_in_intra_trans(
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
void A_IO_L2_in_boundary(int idx, hls::stream<A_t16> &fifo_A_in, hls::stream<A_t4> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t16 local_A_ping[1][16][8];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_2P_BRAM
  A_t16 local_A_pong[1][16][8];
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
    for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1)
          for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
            // array
            // io_L3
            {
              if (arb == 0) {
                A_IO_L2_in_inter_trans_boundary(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_A_pong, 
                  /* fifo */ fifo_A_in, 
                  /* enable */ inter_trans_en
                );
                A_IO_L2_in_intra_trans(
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
                A_IO_L2_in_inter_trans_boundary(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_A_ping, 
                  /* fifo */ fifo_A_in, 
                  /* enable */ inter_trans_en
                );
                A_IO_L2_in_intra_trans(
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
      A_IO_L2_in_intra_trans(
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
      A_IO_L2_in_intra_trans(
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
void B_IO_L3_in(B_t4 *B, hls::stream<B_t4> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
          // array
          // io_L3
          for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
            // io_L2
            for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
            #pragma HLS PIPELINE II=1
              // access_coalesce
              {
                B_t4 in_data;
                B_t4 out_data;
                in_data = B[8*c1 + 512*c3 + c4 + 32*c5];
                out_data = in_data;
                fifo_B_local_out.write(out_data);
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[16][1], hls::stream<float> &fifo_B_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
    for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1)
      for (ap_uint<6> c8 = 0; c8 <= 31; c8 += 1) {
        // latency
        for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
          // latency
          for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
          #pragma HLS PIPELINE II=1
            // simd
            {
              B_t4 in_data;
              B_t1 out_data;
              in_data = local_B[c7][c10 / 4];
              for (ap_uint<3> n = 0; n < 4; n++) {
              #pragma HLS UNROLL
                data_split[n] = in_data(31, 0);
                in_data = in_data >> 32;
              }
              int split_idx = (c10) % 4;
              union {unsigned int ui; float ut;} u;
              u.ui = (unsigned int)data_split[split_idx];
              out_data = u.ut;
              fifo_B_local_out.write(out_data);
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[16][1], hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c4 = p0; c4 <= 7; c4 += 1) {
    // io_L2
    if (c4 == p0) {
      for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          B_t4 in_data;
          B_t4 out_data;
          in_data = fifo_B_in.read();
          out_data = in_data;
          local_B[c5][0] = out_data;
        }
      }
    } else {
      for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
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
void B_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, int c3, B_t4 local_B[16][1], hls::stream<B_t4> &fifo_B_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c4 = p0; c4 <= 7; c4 += 1)
    if (c4 == p0) {
      // io_L2
      for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          B_t4 in_data;
          B_t4 out_data;
          in_data = fifo_B_in.read();
          out_data = in_data;
          local_B[c5][0] = out_data;
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<B_t4> &fifo_B_out, hls::stream<float> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  B_t4 local_B_ping[16][1];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_2P_BRAM
  B_t4 local_B_pong[16][1];
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
    for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1)
          for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
            // array
            // io_L3
            {
              if (arb == 0) {
                B_IO_L2_in_inter_trans(
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
                B_IO_L2_in_intra_trans(
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
                B_IO_L2_in_inter_trans(
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
                B_IO_L2_in_intra_trans(
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
      B_IO_L2_in_intra_trans(
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
      B_IO_L2_in_intra_trans(
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
void B_IO_L2_in_boundary(int idx, hls::stream<B_t4> &fifo_B_in, hls::stream<float> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  B_t4 local_B_ping[16][1];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_2P_BRAM
  B_t4 local_B_pong[16][1];
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
    for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1)
          for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
            // array
            // io_L3
            {
              if (arb == 0) {
                B_IO_L2_in_inter_trans_boundary(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_B_pong, 
                  /* fifo */ fifo_B_in, 
                  /* enable */ inter_trans_en
                );
                B_IO_L2_in_intra_trans(
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
                B_IO_L2_in_inter_trans_boundary(
                  /* module id */ idx, 
                  /* host iter */ c0, 
                  /* host iter */ c1, 
                  /* host iter */ c2, 
                  /* host iter */ c3, 
                  /* array */ local_B_ping, 
                  /* fifo */ fifo_B_in, 
                  /* enable */ inter_trans_en
                );
                B_IO_L2_in_intra_trans(
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
      B_IO_L2_in_intra_trans(
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
      B_IO_L2_in_intra_trans(
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
void C_IO_L3_in(C_t16 *C, hls::stream<C_t16> &fifo_C_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
          // io_L2
          for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
            // access_coalesce
            for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
            #pragma HLS PIPELINE II=1
              {
                C_t16 in_data;
                C_t16 out_data;
                in_data = C[64*c2 + 8*c5 + c6];
                out_data = in_data;
                fifo_C_local_out.write(out_data);
              }
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, C_t16 local_C[8][8], hls::stream<C_t4> &fifo_C_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t4 data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
    // array
    // io_L3
    // io_L2
    // io_L1
    // pe
    for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
      for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1)
        for (ap_uint<6> c8 = 0; c8 <= 31; c8 += 1) {
          // latency
          for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
            // latency
            for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
            #pragma HLS PIPELINE II=1
              // simd
              {
                C_t16 in_data;
                C_t4 out_data;
                in_data = local_C[4*c6 + c9][4 * c8 / 16];
                for (ap_uint<3> n = 0; n < 4; n++) {
                #pragma HLS UNROLL
                  data_split[n] = in_data(127, 0);
                  in_data = in_data >> 128;
                }
                int split_idx = (c8) % 4;
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
void C_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, C_t16 local_C[8][8], hls::stream<C_t16> &fifo_C_in, hls::stream<C_t16> &fifo_C_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c4 = p0; c4 <= 7; c4 += 1) {
    // io_L2
    if (c4 == p0) {
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
        // access_coalesce
        for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t16 in_data;
            C_t16 out_data;
            in_data = fifo_C_in.read();
            out_data = in_data;
            local_C[c5][c6] = out_data;
          }
        }
      }
    } else {
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
        // access_coalesce
        for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t16 in_data;
            C_t16 out_data;
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
void C_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, C_t16 local_C[8][8], hls::stream<C_t16> &fifo_C_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c4 = p0; c4 <= 7; c4 += 1)
    if (c4 == p0) {
      // io_L2
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
        // access_coalesce
        for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t16 in_data;
            C_t16 out_data;
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
void C_IO_L2_in(int idx, hls::stream<C_t16> &fifo_C_in, hls::stream<C_t16> &fifo_C_out, hls::stream<C_t4> &fifo_C_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t16 local_C_ping[8][8];
  #pragma HLS RESOURCE variable=local_C_ping core=RAM_2P_BRAM
  C_t16 local_C_pong[8][8];
  #pragma HLS RESOURCE variable=local_C_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  /* Variable Declaration */

  {
    for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
          // array
          // io_L3
          {
            if (arb == 0) {
              C_IO_L2_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_C_pong, 
                /* fifo */ fifo_C_in, 
                /* fifo */ fifo_C_out, 
                /* enable */ inter_trans_en
              );
              C_IO_L2_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
                /* array */ local_C_ping, 
                /* fifo */ fifo_C_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              C_IO_L2_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_C_ping, 
                /* fifo */ fifo_C_in, 
                /* fifo */ fifo_C_out, 
                /* enable */ inter_trans_en
              );
              C_IO_L2_in_intra_trans(
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
      C_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_C_ping, 
        /* fifo */ fifo_C_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      C_IO_L2_in_intra_trans(
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
void C_IO_L2_in_boundary(int idx, hls::stream<C_t16> &fifo_C_in, hls::stream<C_t4> &fifo_C_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t16 local_C_ping[8][8];
  #pragma HLS RESOURCE variable=local_C_ping core=RAM_2P_BRAM
  C_t16 local_C_pong[8][8];
  #pragma HLS RESOURCE variable=local_C_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  /* Variable Declaration */

  {
    for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
          // array
          // io_L3
          {
            if (arb == 0) {
              C_IO_L2_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_C_pong, 
                /* fifo */ fifo_C_in, 
                /* enable */ inter_trans_en
              );
              C_IO_L2_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
                /* array */ local_C_ping, 
                /* fifo */ fifo_C_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              C_IO_L2_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_C_ping, 
                /* fifo */ fifo_C_in, 
                /* enable */ inter_trans_en
              );
              C_IO_L2_in_intra_trans(
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
      C_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_C_ping, 
        /* fifo */ fifo_C_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      C_IO_L2_in_intra_trans(
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
void PE(int idx, int idy, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, hls::stream<C_t4> &fifo_C_in, hls::stream<C_t4> &fifo_C_out, hls::stream<float> &fifo_D_drain_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  A_t1 local_A[1][1][4];
  #pragma HLS ARRAY_PARTITION variable=local_A dim=0 complete
  B_t1 local_B[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_B dim=0 complete
  C_t1 local_C[1][4];
  #pragma HLS ARRAY_PARTITION variable=local_C dim=0 complete
  D_t1 local_D[1][4][8];
  #pragma HLS RESOURCE variable=local_D core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
          // array
          // pe
          for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1) {
            if (c3 == 0) {
              // latency
              for (ap_uint<3> c8 = 0; c8 <= 3; c8 += 1) {
                // latency
                for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
                #pragma HLS PIPELINE II=1
                  local_D[0][c9][4*c6 + c8] = 0;
                }
              }
            }
            for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1)
              for (ap_uint<6> c8 = 0; c8 <= 31; c8 += 1) {
                // latency
                for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
                  // latency
                  for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
                  #pragma HLS PIPELINE II=1
                    {
                      {
                        A_t4 fifo_data;
                        fifo_data = fifo_A_in.read();
                        for (ap_uint<3> n = 0; n < 4; n++) {
                        #pragma HLS UNROLL
                          union {unsigned int ui; float ut;} u;
                          u.ui = (unsigned int)fifo_data(31, 0);
                          local_A[0][0][n] = u.ut;
                          fifo_data = fifo_data >> 32;
                        }
                      }
                      local_B[0][0] = fifo_B_in.read();
                      {
                        C_t4 fifo_data;
                        fifo_data = fifo_C_in.read();
                        for (ap_uint<3> n = 0; n < 4; n++) {
                        #pragma HLS UNROLL
                          union {unsigned int ui; float ut;} u;
                          u.ui = (unsigned int)fifo_data(31, 0);
                          local_C[0][n] = u.ut;
                          fifo_data = fifo_data >> 32;
                        }
                      }
                      // simd
                      for (ap_uint<3> c11 = 0; c11 <= 3; c11 += 1) {
                      #pragma HLS UNROLL
                        local_D[0][c10][4*c6 + c9] = (local_D[0][c10][4*c6 + c9] + ((local_A[0][0][c11] * local_B[0][0]) * local_C[0][c11]));
                      }
                      if (c3 == 7 && c7 == 15 && c8 == 31)
                        fifo_D_drain_out.write(local_D[0][c10][4*c6 + c9]);
                      {
                        C_t4 fifo_data;
                        union {unsigned int ui; float ut;} u3, u2, u1, u0;
                        u3.ut = local_C[0][3];
                        u2.ut = local_C[0][2];
                        u1.ut = local_C[0][1];
                        u0.ut = local_C[0][0];
                        fifo_data = (ap_uint<32>(u3.ui), ap_uint<32>(u2.ui), ap_uint<32>(u1.ui), ap_uint<32>(u0.ui));
                        fifo_C_out.write(fifo_data);
                      }
                      fifo_B_out.write(local_B[0][0]);
                      {
                        A_t4 fifo_data;
                        union {unsigned int ui; float ut;} u3, u2, u1, u0;
                        u3.ut = local_A[0][0][3];
                        u2.ut = local_A[0][0][2];
                        u1.ut = local_A[0][0][1];
                        u0.ut = local_A[0][0][0];
                        fifo_data = (ap_uint<32>(u3.ui), ap_uint<32>(u2.ui), ap_uint<32>(u1.ui), ap_uint<32>(u0.ui));
                        fifo_A_out.write(fifo_data);
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
void PE_wrapper(int idx, int idy, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out, hls::stream<C_t4> &fifo_C_in, hls::stream<C_t4> &fifo_C_out, hls::stream<float> &fifo_D_drain_out)
 {
  PE(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_out, 
    /* fifo */ fifo_B_in, 
    /* fifo */ fifo_B_out, 
    /* fifo */ fifo_C_in, 
    /* fifo */ fifo_C_out, 
    /* fifo */ fifo_D_drain_out);
}
/* Module Definition */

/* Module Definition */
void A_PE_dummy_in(int idx, int idy, hls::stream<A_t4> &fifo_A_in) {
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        {
        }
        for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
          // array
          // pe
          for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
            for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1)
              for (ap_uint<6> c8 = 0; c8 <= 31; c8 += 1) {
                // latency
                for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
                  // latency
                  for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
                  #pragma HLS PIPELINE II=1
                    A_t4 fifo_data;
                    fifo_data = fifo_A_in.read();
                  }
                }
              }
        }
      }
}
/* Module Definition */

/* Module Definition */
void B_PE_dummy_in(int idx, int idy, hls::stream<float> &fifo_B_in) {
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        {
        }
        for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
          // array
          // pe
          for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
            for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1)
              for (ap_uint<6> c8 = 0; c8 <= 31; c8 += 1) {
                // latency
                for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
                  // latency
                  for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
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
void C_PE_dummy_in(int idx, int idy, hls::stream<C_t4> &fifo_C_in) {
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        {
        }
        for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
          // array
          // pe
          for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
            for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1)
              for (ap_uint<6> c8 = 0; c8 <= 31; c8 += 1) {
                // latency
                for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
                  // latency
                  for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
                  #pragma HLS PIPELINE II=1
                    C_t4 fifo_data;
                    fifo_data = fifo_C_in.read();
                  }
                }
              }
        }
      }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_intra_trans(int idx, int idy, int c0, int c1, int c2, D_t4 local_D[1][4][2], hls::stream<float> &fifo_D_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */


  // io_L1
  // pe
  for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1) {
    // latency
    for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
      // latency
      for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
      #pragma HLS PIPELINE II=1
        // simd
        {
          D_t1 in_data;
          D_t4 out_data;
          in_data = fifo_D_drain_local_in.read();
          int split_idx = (4*c6 + c9) % 4;
          out_data = local_D[0][c10][(4 * c6 + c9) / 4];
          for (ap_uint<3> n = 0; n < 4; n++) {
          #pragma HLS UNROLL
            data_split[n] = out_data(31, 0);
            out_data = out_data >> 32;
          }
          union {unsigned int ui; float ut;} u;
          u.ut = in_data;
          data_split[split_idx] = ap_uint<32>(u.ui);
          out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);          local_D[0][c10][(4 * c6 + c9) / 4] = out_data;
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_inter_trans(int idx, int idy, int c0, int c1, int c2, D_t4 local_D[1][4][2], hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<4> c5 = p1; c5 <= 7; c5 += 1) {
    // io_L1
    if (c5 == p1) {
      for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
        // access_coalesce
        for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
        #pragma HLS PIPELINE II=1
          {
            D_t4 in_data;
            D_t4 out_data;
            in_data = local_D[0][c7][c8];
            out_data = in_data;
            fifo_D_drain_out.write(out_data);
          }
        }
      }
    } else {
      for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
        // access_coalesce
        for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
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
void D_drain_IO_L1_out_inter_trans_boundary(int idx, int idy, int c0, int c1, int c2, D_t4 local_D[1][4][2], hls::stream<D_t4> &fifo_D_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<4> c5 = p1; c5 <= 7; c5 += 1)
    if (c5 == p1) {
      // io_L1
      for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
        // access_coalesce
        for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
        #pragma HLS PIPELINE II=1
          {
            D_t4 in_data;
            D_t4 out_data;
            in_data = local_D[0][c7][c8];
            out_data = in_data;
            fifo_D_drain_out.write(out_data);
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out(int idx, int idy, hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  D_t4 local_D[1][4][2];
  #pragma HLS RESOURCE variable=local_D core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        // io_L2
        D_drain_IO_L1_out_intra_trans(
          /* module id */ idx, 
          /* module id */ idy, 
          /* host iter */ c0, 
          /* host iter */ c1, 
          /* host iter */ c2, 
          /* array */ local_D, 
          /* fifo */ fifo_D_drain_local_in
        );
        D_drain_IO_L1_out_inter_trans(
          /* module id */ idx, 
          /* module id */ idy, 
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
void D_drain_IO_L1_out_wrapper(int idx, int idy, hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in)
 {
  D_drain_IO_L1_out(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_D_drain_in, 
    /* fifo */ fifo_D_drain_out, 
    /* fifo */ fifo_D_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L1_out_boundary(int idx, int idy, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  D_t4 local_D[1][4][2];
  #pragma HLS RESOURCE variable=local_D core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        // io_L2
        D_drain_IO_L1_out_intra_trans(
          /* module id */ idx, 
          /* module id */ idy, 
          /* host iter */ c0, 
          /* host iter */ c1, 
          /* host iter */ c2, 
          /* array */ local_D, 
          /* fifo */ fifo_D_drain_local_in
        );
        D_drain_IO_L1_out_inter_trans_boundary(
          /* module id */ idx, 
          /* module id */ idy, 
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
void D_drain_IO_L1_out_boundary_wrapper(int idx, int idy, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<float> &fifo_D_drain_local_in)
 {
  D_drain_IO_L1_out_boundary(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_D_drain_out, 
    /* fifo */ fifo_D_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L2_out(int idx, hls::stream<D_t4> &fifo_D_drain_in, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<D_t4> &fifo_D_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        for (ap_uint<4> c4 = p0; c4 <= 7; c4 += 1) {
          // io_L2
          if (c4 == p0) {
            for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
              // io_L1
              for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
                // access_coalesce
                for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
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
          } else {
            for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
              // io_L1
              for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
                // access_coalesce
                for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
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
      }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L2_out_boundary(int idx, hls::stream<D_t4> &fifo_D_drain_out, hls::stream<D_t4> &fifo_D_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        for (ap_uint<4> c4 = p0; c4 <= 7; c4 += 1)
          if (c4 == p0) {
            // io_L2
            for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
              // io_L1
              for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
                // access_coalesce
                for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
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
      }
}
/* Module Definition */

/* Module Definition */
void D_drain_IO_L3_out(D_t8 *D, hls::stream<D_t4> &fifo_D_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  D_t4 data_split[2];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 15; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
          // io_L2
          for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
            // io_L1
            for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
              // access_coalesce
              for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
              #pragma HLS PIPELINE II=1
                {
                  D_t4 in_data;
                  D_t8 out_data;
                  in_data = fifo_D_drain_local_in.read();
                  int split_idx = (32768*c0 + 1024*c1 + 2*c2 + 128*c4 + 4096*c5 + 32*c7 + c8) % 2;
                  data_split[split_idx] = in_data;
                  out_data = (data_split[1], data_split[0]);                  if (c8 % 2 == 2 - 1 || c8 == 1) {
                    D[(((8 * c0 + c5) * 128 + (32 * c1 + 4 * c4 + c7)) * 128 + (8 * c2 + 4 * c8)) / 8] = out_data;
                  }
                }
              }
            }
          }
        }
      }
}
/* Module Definition */

extern "C" {
void kernel0(A_t16 *A, B_t4 *B, C_t16 *C, D_t8 *D)
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
  /* A_IO_L2_in fifo */ hls::stream<A_t16> fifo_A_A_IO_L2_in_0;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_0 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t16> fifo_A_A_IO_L2_in_1;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_1 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t16> fifo_A_A_IO_L2_in_2;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_2 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t16> fifo_A_A_IO_L2_in_3;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_3 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t16> fifo_A_A_IO_L2_in_4;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_4 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t16> fifo_A_A_IO_L2_in_5;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_5 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t16> fifo_A_A_IO_L2_in_6;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_6 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t16> fifo_A_A_IO_L2_in_7;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_7 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t16> fifo_A_A_IO_L2_in_8;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_8 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_0;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_0 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_1;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_1 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_2;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_2 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_3;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_3 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_4;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_4 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_5;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_5 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_6;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_6 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_7;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_7 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t4> fifo_B_B_IO_L2_in_8;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_8 core=FIFO_SRL
  /* C_IO_L2_in fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_0;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_in_0 core=FIFO_SRL
  /* C_IO_L2_in fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_1;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_in_1 core=FIFO_SRL
  /* C_IO_L2_in fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_2;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_in_2 core=FIFO_SRL
  /* C_IO_L2_in fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_3;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_in_3 core=FIFO_SRL
  /* C_IO_L2_in fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_4;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_in_4 core=FIFO_SRL
  /* C_IO_L2_in fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_5;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_in_5 core=FIFO_SRL
  /* C_IO_L2_in fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_6;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_in_6 core=FIFO_SRL
  /* C_IO_L2_in fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_7;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_in_7 core=FIFO_SRL
  /* C_IO_L2_in fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_in_8;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_in_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_0_0;
  #pragma HLS STREAM variable=fifo_A_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_0_1;
  #pragma HLS STREAM variable=fifo_A_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_0_2;
  #pragma HLS STREAM variable=fifo_A_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_0_3;
  #pragma HLS STREAM variable=fifo_A_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_0_4;
  #pragma HLS STREAM variable=fifo_A_PE_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_0_5;
  #pragma HLS STREAM variable=fifo_A_PE_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_0_6;
  #pragma HLS STREAM variable=fifo_A_PE_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_0_7;
  #pragma HLS STREAM variable=fifo_A_PE_0_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_0_8;
  #pragma HLS STREAM variable=fifo_A_PE_0_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_1_0;
  #pragma HLS STREAM variable=fifo_A_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_1_1;
  #pragma HLS STREAM variable=fifo_A_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_1_2;
  #pragma HLS STREAM variable=fifo_A_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_1_3;
  #pragma HLS STREAM variable=fifo_A_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_1_4;
  #pragma HLS STREAM variable=fifo_A_PE_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_1_5;
  #pragma HLS STREAM variable=fifo_A_PE_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_1_6;
  #pragma HLS STREAM variable=fifo_A_PE_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_1_7;
  #pragma HLS STREAM variable=fifo_A_PE_1_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_1_8;
  #pragma HLS STREAM variable=fifo_A_PE_1_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_2_0;
  #pragma HLS STREAM variable=fifo_A_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_2_1;
  #pragma HLS STREAM variable=fifo_A_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_2_2;
  #pragma HLS STREAM variable=fifo_A_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_2_3;
  #pragma HLS STREAM variable=fifo_A_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_2_4;
  #pragma HLS STREAM variable=fifo_A_PE_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_2_5;
  #pragma HLS STREAM variable=fifo_A_PE_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_2_6;
  #pragma HLS STREAM variable=fifo_A_PE_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_2_7;
  #pragma HLS STREAM variable=fifo_A_PE_2_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_2_8;
  #pragma HLS STREAM variable=fifo_A_PE_2_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_3_0;
  #pragma HLS STREAM variable=fifo_A_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_3_1;
  #pragma HLS STREAM variable=fifo_A_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_3_2;
  #pragma HLS STREAM variable=fifo_A_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_3_3;
  #pragma HLS STREAM variable=fifo_A_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_3_4;
  #pragma HLS STREAM variable=fifo_A_PE_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_3_5;
  #pragma HLS STREAM variable=fifo_A_PE_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_3_6;
  #pragma HLS STREAM variable=fifo_A_PE_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_3_7;
  #pragma HLS STREAM variable=fifo_A_PE_3_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_3_8;
  #pragma HLS STREAM variable=fifo_A_PE_3_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_4_0;
  #pragma HLS STREAM variable=fifo_A_PE_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_4_1;
  #pragma HLS STREAM variable=fifo_A_PE_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_4_2;
  #pragma HLS STREAM variable=fifo_A_PE_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_4_3;
  #pragma HLS STREAM variable=fifo_A_PE_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_4_4;
  #pragma HLS STREAM variable=fifo_A_PE_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_4_5;
  #pragma HLS STREAM variable=fifo_A_PE_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_4_6;
  #pragma HLS STREAM variable=fifo_A_PE_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_4_7;
  #pragma HLS STREAM variable=fifo_A_PE_4_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_4_8;
  #pragma HLS STREAM variable=fifo_A_PE_4_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_5_0;
  #pragma HLS STREAM variable=fifo_A_PE_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_5_1;
  #pragma HLS STREAM variable=fifo_A_PE_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_5_2;
  #pragma HLS STREAM variable=fifo_A_PE_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_5_3;
  #pragma HLS STREAM variable=fifo_A_PE_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_5_4;
  #pragma HLS STREAM variable=fifo_A_PE_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_5_5;
  #pragma HLS STREAM variable=fifo_A_PE_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_5_6;
  #pragma HLS STREAM variable=fifo_A_PE_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_5_7;
  #pragma HLS STREAM variable=fifo_A_PE_5_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_5_8;
  #pragma HLS STREAM variable=fifo_A_PE_5_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_6_0;
  #pragma HLS STREAM variable=fifo_A_PE_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_6_1;
  #pragma HLS STREAM variable=fifo_A_PE_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_6_2;
  #pragma HLS STREAM variable=fifo_A_PE_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_6_3;
  #pragma HLS STREAM variable=fifo_A_PE_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_6_4;
  #pragma HLS STREAM variable=fifo_A_PE_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_6_5;
  #pragma HLS STREAM variable=fifo_A_PE_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_6_6;
  #pragma HLS STREAM variable=fifo_A_PE_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_6_7;
  #pragma HLS STREAM variable=fifo_A_PE_6_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_6_8;
  #pragma HLS STREAM variable=fifo_A_PE_6_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_7_0;
  #pragma HLS STREAM variable=fifo_A_PE_7_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_7_1;
  #pragma HLS STREAM variable=fifo_A_PE_7_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_7_2;
  #pragma HLS STREAM variable=fifo_A_PE_7_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_7_3;
  #pragma HLS STREAM variable=fifo_A_PE_7_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_7_4;
  #pragma HLS STREAM variable=fifo_A_PE_7_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_7_5;
  #pragma HLS STREAM variable=fifo_A_PE_7_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_7_6;
  #pragma HLS STREAM variable=fifo_A_PE_7_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_7_7;
  #pragma HLS STREAM variable=fifo_A_PE_7_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t4> fifo_A_PE_7_8;
  #pragma HLS STREAM variable=fifo_A_PE_7_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_0;
  #pragma HLS STREAM variable=fifo_B_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_0;
  #pragma HLS STREAM variable=fifo_B_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_0;
  #pragma HLS STREAM variable=fifo_B_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_0;
  #pragma HLS STREAM variable=fifo_B_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_0;
  #pragma HLS STREAM variable=fifo_B_PE_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_5_0;
  #pragma HLS STREAM variable=fifo_B_PE_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_5_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_6_0;
  #pragma HLS STREAM variable=fifo_B_PE_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_6_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_7_0;
  #pragma HLS STREAM variable=fifo_B_PE_7_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_7_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_8_0;
  #pragma HLS STREAM variable=fifo_B_PE_8_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_8_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_1;
  #pragma HLS STREAM variable=fifo_B_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_1;
  #pragma HLS STREAM variable=fifo_B_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_1;
  #pragma HLS STREAM variable=fifo_B_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_1;
  #pragma HLS STREAM variable=fifo_B_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_1;
  #pragma HLS STREAM variable=fifo_B_PE_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_5_1;
  #pragma HLS STREAM variable=fifo_B_PE_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_5_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_6_1;
  #pragma HLS STREAM variable=fifo_B_PE_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_6_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_7_1;
  #pragma HLS STREAM variable=fifo_B_PE_7_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_7_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_8_1;
  #pragma HLS STREAM variable=fifo_B_PE_8_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_8_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_2;
  #pragma HLS STREAM variable=fifo_B_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_2;
  #pragma HLS STREAM variable=fifo_B_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_2;
  #pragma HLS STREAM variable=fifo_B_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_2;
  #pragma HLS STREAM variable=fifo_B_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_2;
  #pragma HLS STREAM variable=fifo_B_PE_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_5_2;
  #pragma HLS STREAM variable=fifo_B_PE_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_5_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_6_2;
  #pragma HLS STREAM variable=fifo_B_PE_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_6_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_7_2;
  #pragma HLS STREAM variable=fifo_B_PE_7_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_7_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_8_2;
  #pragma HLS STREAM variable=fifo_B_PE_8_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_8_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_3;
  #pragma HLS STREAM variable=fifo_B_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_3;
  #pragma HLS STREAM variable=fifo_B_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_3;
  #pragma HLS STREAM variable=fifo_B_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_3;
  #pragma HLS STREAM variable=fifo_B_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_3;
  #pragma HLS STREAM variable=fifo_B_PE_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_5_3;
  #pragma HLS STREAM variable=fifo_B_PE_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_5_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_6_3;
  #pragma HLS STREAM variable=fifo_B_PE_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_6_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_7_3;
  #pragma HLS STREAM variable=fifo_B_PE_7_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_7_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_8_3;
  #pragma HLS STREAM variable=fifo_B_PE_8_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_8_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_4;
  #pragma HLS STREAM variable=fifo_B_PE_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_4;
  #pragma HLS STREAM variable=fifo_B_PE_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_4;
  #pragma HLS STREAM variable=fifo_B_PE_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_4;
  #pragma HLS STREAM variable=fifo_B_PE_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_4;
  #pragma HLS STREAM variable=fifo_B_PE_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_5_4;
  #pragma HLS STREAM variable=fifo_B_PE_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_5_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_6_4;
  #pragma HLS STREAM variable=fifo_B_PE_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_6_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_7_4;
  #pragma HLS STREAM variable=fifo_B_PE_7_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_7_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_8_4;
  #pragma HLS STREAM variable=fifo_B_PE_8_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_8_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_5;
  #pragma HLS STREAM variable=fifo_B_PE_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_5;
  #pragma HLS STREAM variable=fifo_B_PE_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_5;
  #pragma HLS STREAM variable=fifo_B_PE_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_5;
  #pragma HLS STREAM variable=fifo_B_PE_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_5;
  #pragma HLS STREAM variable=fifo_B_PE_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_5_5;
  #pragma HLS STREAM variable=fifo_B_PE_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_5_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_6_5;
  #pragma HLS STREAM variable=fifo_B_PE_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_6_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_7_5;
  #pragma HLS STREAM variable=fifo_B_PE_7_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_7_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_8_5;
  #pragma HLS STREAM variable=fifo_B_PE_8_5 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_8_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_6;
  #pragma HLS STREAM variable=fifo_B_PE_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_6;
  #pragma HLS STREAM variable=fifo_B_PE_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_6;
  #pragma HLS STREAM variable=fifo_B_PE_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_6;
  #pragma HLS STREAM variable=fifo_B_PE_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_6;
  #pragma HLS STREAM variable=fifo_B_PE_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_5_6;
  #pragma HLS STREAM variable=fifo_B_PE_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_5_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_6_6;
  #pragma HLS STREAM variable=fifo_B_PE_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_6_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_7_6;
  #pragma HLS STREAM variable=fifo_B_PE_7_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_7_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_8_6;
  #pragma HLS STREAM variable=fifo_B_PE_8_6 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_8_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_0_7;
  #pragma HLS STREAM variable=fifo_B_PE_0_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_1_7;
  #pragma HLS STREAM variable=fifo_B_PE_1_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_2_7;
  #pragma HLS STREAM variable=fifo_B_PE_2_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_3_7;
  #pragma HLS STREAM variable=fifo_B_PE_3_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_4_7;
  #pragma HLS STREAM variable=fifo_B_PE_4_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_5_7;
  #pragma HLS STREAM variable=fifo_B_PE_5_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_5_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_6_7;
  #pragma HLS STREAM variable=fifo_B_PE_6_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_6_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_7_7;
  #pragma HLS STREAM variable=fifo_B_PE_7_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_7_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_B_PE_8_7;
  #pragma HLS STREAM variable=fifo_B_PE_8_7 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_8_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_0_0;
  #pragma HLS STREAM variable=fifo_C_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_1_0;
  #pragma HLS STREAM variable=fifo_C_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_2_0;
  #pragma HLS STREAM variable=fifo_C_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_3_0;
  #pragma HLS STREAM variable=fifo_C_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_4_0;
  #pragma HLS STREAM variable=fifo_C_PE_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_5_0;
  #pragma HLS STREAM variable=fifo_C_PE_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_6_0;
  #pragma HLS STREAM variable=fifo_C_PE_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_7_0;
  #pragma HLS STREAM variable=fifo_C_PE_7_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_8_0;
  #pragma HLS STREAM variable=fifo_C_PE_8_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_0_1;
  #pragma HLS STREAM variable=fifo_C_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_1_1;
  #pragma HLS STREAM variable=fifo_C_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_2_1;
  #pragma HLS STREAM variable=fifo_C_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_3_1;
  #pragma HLS STREAM variable=fifo_C_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_4_1;
  #pragma HLS STREAM variable=fifo_C_PE_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_5_1;
  #pragma HLS STREAM variable=fifo_C_PE_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_6_1;
  #pragma HLS STREAM variable=fifo_C_PE_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_7_1;
  #pragma HLS STREAM variable=fifo_C_PE_7_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_8_1;
  #pragma HLS STREAM variable=fifo_C_PE_8_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_0_2;
  #pragma HLS STREAM variable=fifo_C_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_1_2;
  #pragma HLS STREAM variable=fifo_C_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_2_2;
  #pragma HLS STREAM variable=fifo_C_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_3_2;
  #pragma HLS STREAM variable=fifo_C_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_4_2;
  #pragma HLS STREAM variable=fifo_C_PE_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_5_2;
  #pragma HLS STREAM variable=fifo_C_PE_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_6_2;
  #pragma HLS STREAM variable=fifo_C_PE_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_7_2;
  #pragma HLS STREAM variable=fifo_C_PE_7_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_8_2;
  #pragma HLS STREAM variable=fifo_C_PE_8_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_0_3;
  #pragma HLS STREAM variable=fifo_C_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_1_3;
  #pragma HLS STREAM variable=fifo_C_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_2_3;
  #pragma HLS STREAM variable=fifo_C_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_3_3;
  #pragma HLS STREAM variable=fifo_C_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_4_3;
  #pragma HLS STREAM variable=fifo_C_PE_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_5_3;
  #pragma HLS STREAM variable=fifo_C_PE_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_6_3;
  #pragma HLS STREAM variable=fifo_C_PE_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_7_3;
  #pragma HLS STREAM variable=fifo_C_PE_7_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_8_3;
  #pragma HLS STREAM variable=fifo_C_PE_8_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_0_4;
  #pragma HLS STREAM variable=fifo_C_PE_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_1_4;
  #pragma HLS STREAM variable=fifo_C_PE_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_2_4;
  #pragma HLS STREAM variable=fifo_C_PE_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_3_4;
  #pragma HLS STREAM variable=fifo_C_PE_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_4_4;
  #pragma HLS STREAM variable=fifo_C_PE_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_5_4;
  #pragma HLS STREAM variable=fifo_C_PE_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_6_4;
  #pragma HLS STREAM variable=fifo_C_PE_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_7_4;
  #pragma HLS STREAM variable=fifo_C_PE_7_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_8_4;
  #pragma HLS STREAM variable=fifo_C_PE_8_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_0_5;
  #pragma HLS STREAM variable=fifo_C_PE_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_1_5;
  #pragma HLS STREAM variable=fifo_C_PE_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_2_5;
  #pragma HLS STREAM variable=fifo_C_PE_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_3_5;
  #pragma HLS STREAM variable=fifo_C_PE_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_4_5;
  #pragma HLS STREAM variable=fifo_C_PE_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_5_5;
  #pragma HLS STREAM variable=fifo_C_PE_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_6_5;
  #pragma HLS STREAM variable=fifo_C_PE_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_7_5;
  #pragma HLS STREAM variable=fifo_C_PE_7_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_8_5;
  #pragma HLS STREAM variable=fifo_C_PE_8_5 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_0_6;
  #pragma HLS STREAM variable=fifo_C_PE_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_1_6;
  #pragma HLS STREAM variable=fifo_C_PE_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_2_6;
  #pragma HLS STREAM variable=fifo_C_PE_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_3_6;
  #pragma HLS STREAM variable=fifo_C_PE_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_4_6;
  #pragma HLS STREAM variable=fifo_C_PE_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_5_6;
  #pragma HLS STREAM variable=fifo_C_PE_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_6_6;
  #pragma HLS STREAM variable=fifo_C_PE_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_7_6;
  #pragma HLS STREAM variable=fifo_C_PE_7_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_8_6;
  #pragma HLS STREAM variable=fifo_C_PE_8_6 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_0_7;
  #pragma HLS STREAM variable=fifo_C_PE_0_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_1_7;
  #pragma HLS STREAM variable=fifo_C_PE_1_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_2_7;
  #pragma HLS STREAM variable=fifo_C_PE_2_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_2_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_3_7;
  #pragma HLS STREAM variable=fifo_C_PE_3_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_3_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_4_7;
  #pragma HLS STREAM variable=fifo_C_PE_4_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_4_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_5_7;
  #pragma HLS STREAM variable=fifo_C_PE_5_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_5_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_6_7;
  #pragma HLS STREAM variable=fifo_C_PE_6_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_6_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_7_7;
  #pragma HLS STREAM variable=fifo_C_PE_7_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_7_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<C_t4> fifo_C_PE_8_7;
  #pragma HLS STREAM variable=fifo_C_PE_8_7 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_8_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_0_0;
  #pragma HLS STREAM variable=fifo_D_drain_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_1_0;
  #pragma HLS STREAM variable=fifo_D_drain_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_2_0;
  #pragma HLS STREAM variable=fifo_D_drain_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_3_0;
  #pragma HLS STREAM variable=fifo_D_drain_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_4_0;
  #pragma HLS STREAM variable=fifo_D_drain_PE_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_4_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_5_0;
  #pragma HLS STREAM variable=fifo_D_drain_PE_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_5_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_6_0;
  #pragma HLS STREAM variable=fifo_D_drain_PE_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_6_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_7_0;
  #pragma HLS STREAM variable=fifo_D_drain_PE_7_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_7_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_0_1;
  #pragma HLS STREAM variable=fifo_D_drain_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_1_1;
  #pragma HLS STREAM variable=fifo_D_drain_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_2_1;
  #pragma HLS STREAM variable=fifo_D_drain_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_3_1;
  #pragma HLS STREAM variable=fifo_D_drain_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_4_1;
  #pragma HLS STREAM variable=fifo_D_drain_PE_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_4_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_5_1;
  #pragma HLS STREAM variable=fifo_D_drain_PE_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_5_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_6_1;
  #pragma HLS STREAM variable=fifo_D_drain_PE_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_6_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_7_1;
  #pragma HLS STREAM variable=fifo_D_drain_PE_7_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_7_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_0_2;
  #pragma HLS STREAM variable=fifo_D_drain_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_1_2;
  #pragma HLS STREAM variable=fifo_D_drain_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_2_2;
  #pragma HLS STREAM variable=fifo_D_drain_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_3_2;
  #pragma HLS STREAM variable=fifo_D_drain_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_4_2;
  #pragma HLS STREAM variable=fifo_D_drain_PE_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_4_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_5_2;
  #pragma HLS STREAM variable=fifo_D_drain_PE_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_5_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_6_2;
  #pragma HLS STREAM variable=fifo_D_drain_PE_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_6_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_7_2;
  #pragma HLS STREAM variable=fifo_D_drain_PE_7_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_7_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_0_3;
  #pragma HLS STREAM variable=fifo_D_drain_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_1_3;
  #pragma HLS STREAM variable=fifo_D_drain_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_2_3;
  #pragma HLS STREAM variable=fifo_D_drain_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_3_3;
  #pragma HLS STREAM variable=fifo_D_drain_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_3_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_4_3;
  #pragma HLS STREAM variable=fifo_D_drain_PE_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_4_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_5_3;
  #pragma HLS STREAM variable=fifo_D_drain_PE_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_5_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_6_3;
  #pragma HLS STREAM variable=fifo_D_drain_PE_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_6_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_7_3;
  #pragma HLS STREAM variable=fifo_D_drain_PE_7_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_7_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_0_4;
  #pragma HLS STREAM variable=fifo_D_drain_PE_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_0_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_1_4;
  #pragma HLS STREAM variable=fifo_D_drain_PE_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_1_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_2_4;
  #pragma HLS STREAM variable=fifo_D_drain_PE_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_2_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_3_4;
  #pragma HLS STREAM variable=fifo_D_drain_PE_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_3_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_4_4;
  #pragma HLS STREAM variable=fifo_D_drain_PE_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_4_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_5_4;
  #pragma HLS STREAM variable=fifo_D_drain_PE_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_5_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_6_4;
  #pragma HLS STREAM variable=fifo_D_drain_PE_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_6_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_7_4;
  #pragma HLS STREAM variable=fifo_D_drain_PE_7_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_7_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_0_5;
  #pragma HLS STREAM variable=fifo_D_drain_PE_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_0_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_1_5;
  #pragma HLS STREAM variable=fifo_D_drain_PE_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_1_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_2_5;
  #pragma HLS STREAM variable=fifo_D_drain_PE_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_2_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_3_5;
  #pragma HLS STREAM variable=fifo_D_drain_PE_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_3_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_4_5;
  #pragma HLS STREAM variable=fifo_D_drain_PE_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_4_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_5_5;
  #pragma HLS STREAM variable=fifo_D_drain_PE_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_5_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_6_5;
  #pragma HLS STREAM variable=fifo_D_drain_PE_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_6_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_7_5;
  #pragma HLS STREAM variable=fifo_D_drain_PE_7_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_7_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_0_6;
  #pragma HLS STREAM variable=fifo_D_drain_PE_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_0_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_1_6;
  #pragma HLS STREAM variable=fifo_D_drain_PE_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_1_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_2_6;
  #pragma HLS STREAM variable=fifo_D_drain_PE_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_2_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_3_6;
  #pragma HLS STREAM variable=fifo_D_drain_PE_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_3_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_4_6;
  #pragma HLS STREAM variable=fifo_D_drain_PE_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_4_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_5_6;
  #pragma HLS STREAM variable=fifo_D_drain_PE_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_5_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_6_6;
  #pragma HLS STREAM variable=fifo_D_drain_PE_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_6_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_7_6;
  #pragma HLS STREAM variable=fifo_D_drain_PE_7_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_7_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_0_7;
  #pragma HLS STREAM variable=fifo_D_drain_PE_0_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_0_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_1_7;
  #pragma HLS STREAM variable=fifo_D_drain_PE_1_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_1_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_2_7;
  #pragma HLS STREAM variable=fifo_D_drain_PE_2_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_2_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_3_7;
  #pragma HLS STREAM variable=fifo_D_drain_PE_3_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_3_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_4_7;
  #pragma HLS STREAM variable=fifo_D_drain_PE_4_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_4_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_5_7;
  #pragma HLS STREAM variable=fifo_D_drain_PE_5_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_5_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_6_7;
  #pragma HLS STREAM variable=fifo_D_drain_PE_6_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_6_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_D_drain_PE_7_7;
  #pragma HLS STREAM variable=fifo_D_drain_PE_7_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_PE_7_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0_0 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0_1 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0_2 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0_3 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0_4 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0_5 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0_6 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_0_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_0_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_0_8 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1_0 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1_1 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1_2 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1_3 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1_4 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1_5 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1_6 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_1_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_1_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_1_8 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2_0 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2_1 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2_2 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2_3 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2_4 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2_5 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2_6 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_2_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_2_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_2_8 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3_0 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3_1 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3_2 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3_3 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3_4 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3_5 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3_6 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_3_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_3_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_3_8 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4_0 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4_1 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4_2 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4_3 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4_4 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4_5 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4_6 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_4_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_4_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_4_8 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5_0 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5_1 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5_2 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5_3 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5_4 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5_5 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5_6 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_5_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_5_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_5_8 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6_0 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6_1 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6_2 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6_3 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6_4 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6_5 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6_6 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_6_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_6_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_6_8 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7_0 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7_1 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7_2 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7_3 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7_4 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7_5 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7_6 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7_7 core=FIFO_SRL
  /* D_drain_IO_L1_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L1_out_7_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L1_out_7_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L1_out_7_8 core=FIFO_SRL
  /* D_drain_IO_L2_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_0;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_0 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L2_out_0 core=FIFO_SRL
  /* D_drain_IO_L2_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_1;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_1 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L2_out_1 core=FIFO_SRL
  /* D_drain_IO_L2_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_2;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_2 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L2_out_2 core=FIFO_SRL
  /* D_drain_IO_L2_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_3;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_3 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L2_out_3 core=FIFO_SRL
  /* D_drain_IO_L2_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_4;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_4 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L2_out_4 core=FIFO_SRL
  /* D_drain_IO_L2_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_5;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_5 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L2_out_5 core=FIFO_SRL
  /* D_drain_IO_L2_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_6;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_6 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L2_out_6 core=FIFO_SRL
  /* D_drain_IO_L2_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_7;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_7 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L2_out_7 core=FIFO_SRL
  /* D_drain_IO_L2_out fifo */ hls::stream<D_t4> fifo_D_drain_D_drain_IO_L2_out_8;
  #pragma HLS STREAM variable=fifo_D_drain_D_drain_IO_L2_out_8 depth=2
  #pragma HLS RESOURCE variable=fifo_D_drain_D_drain_IO_L2_out_8 core=FIFO_SRL
  /* FIFO Declaration */

  /* Module Call */
  A_IO_L3_in(
    /* array */ A,
    /* fifo */ fifo_A_A_IO_L2_in_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 0,
    /* fifo */ fifo_A_A_IO_L2_in_0,
    /* fifo */ fifo_A_A_IO_L2_in_1,
    /* fifo */ fifo_A_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 1,
    /* fifo */ fifo_A_A_IO_L2_in_1,
    /* fifo */ fifo_A_A_IO_L2_in_2,
    /* fifo */ fifo_A_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 2,
    /* fifo */ fifo_A_A_IO_L2_in_2,
    /* fifo */ fifo_A_A_IO_L2_in_3,
    /* fifo */ fifo_A_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 3,
    /* fifo */ fifo_A_A_IO_L2_in_3,
    /* fifo */ fifo_A_A_IO_L2_in_4,
    /* fifo */ fifo_A_PE_3_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 4,
    /* fifo */ fifo_A_A_IO_L2_in_4,
    /* fifo */ fifo_A_A_IO_L2_in_5,
    /* fifo */ fifo_A_PE_4_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 5,
    /* fifo */ fifo_A_A_IO_L2_in_5,
    /* fifo */ fifo_A_A_IO_L2_in_6,
    /* fifo */ fifo_A_PE_5_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in(
    /* module id */ 6,
    /* fifo */ fifo_A_A_IO_L2_in_6,
    /* fifo */ fifo_A_A_IO_L2_in_7,
    /* fifo */ fifo_A_PE_6_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in_boundary(
    /* module id */ 7,
    /* fifo */ fifo_A_A_IO_L2_in_7,
    /* fifo */ fifo_A_PE_7_0
  );
  /* Module Call */

  /* Module Call */
  B_IO_L3_in(
    /* array */ B,
    /* fifo */ fifo_B_B_IO_L2_in_0
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 0,
    /* fifo */ fifo_B_B_IO_L2_in_0,
    /* fifo */ fifo_B_B_IO_L2_in_1,
    /* fifo */ fifo_B_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 1,
    /* fifo */ fifo_B_B_IO_L2_in_1,
    /* fifo */ fifo_B_B_IO_L2_in_2,
    /* fifo */ fifo_B_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 2,
    /* fifo */ fifo_B_B_IO_L2_in_2,
    /* fifo */ fifo_B_B_IO_L2_in_3,
    /* fifo */ fifo_B_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 3,
    /* fifo */ fifo_B_B_IO_L2_in_3,
    /* fifo */ fifo_B_B_IO_L2_in_4,
    /* fifo */ fifo_B_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 4,
    /* fifo */ fifo_B_B_IO_L2_in_4,
    /* fifo */ fifo_B_B_IO_L2_in_5,
    /* fifo */ fifo_B_PE_0_4
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 5,
    /* fifo */ fifo_B_B_IO_L2_in_5,
    /* fifo */ fifo_B_B_IO_L2_in_6,
    /* fifo */ fifo_B_PE_0_5
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in(
    /* module id */ 6,
    /* fifo */ fifo_B_B_IO_L2_in_6,
    /* fifo */ fifo_B_B_IO_L2_in_7,
    /* fifo */ fifo_B_PE_0_6
  );
  /* Module Call */

  /* Module Call */
  B_IO_L2_in_boundary(
    /* module id */ 7,
    /* fifo */ fifo_B_B_IO_L2_in_7,
    /* fifo */ fifo_B_PE_0_7
  );
  /* Module Call */

  /* Module Call */
  C_IO_L3_in(
    /* array */ C,
    /* fifo */ fifo_C_C_IO_L2_in_0
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in(
    /* module id */ 0,
    /* fifo */ fifo_C_C_IO_L2_in_0,
    /* fifo */ fifo_C_C_IO_L2_in_1,
    /* fifo */ fifo_C_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in(
    /* module id */ 1,
    /* fifo */ fifo_C_C_IO_L2_in_1,
    /* fifo */ fifo_C_C_IO_L2_in_2,
    /* fifo */ fifo_C_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in(
    /* module id */ 2,
    /* fifo */ fifo_C_C_IO_L2_in_2,
    /* fifo */ fifo_C_C_IO_L2_in_3,
    /* fifo */ fifo_C_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in(
    /* module id */ 3,
    /* fifo */ fifo_C_C_IO_L2_in_3,
    /* fifo */ fifo_C_C_IO_L2_in_4,
    /* fifo */ fifo_C_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in(
    /* module id */ 4,
    /* fifo */ fifo_C_C_IO_L2_in_4,
    /* fifo */ fifo_C_C_IO_L2_in_5,
    /* fifo */ fifo_C_PE_0_4
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in(
    /* module id */ 5,
    /* fifo */ fifo_C_C_IO_L2_in_5,
    /* fifo */ fifo_C_C_IO_L2_in_6,
    /* fifo */ fifo_C_PE_0_5
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in(
    /* module id */ 6,
    /* fifo */ fifo_C_C_IO_L2_in_6,
    /* fifo */ fifo_C_C_IO_L2_in_7,
    /* fifo */ fifo_C_PE_0_6
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_in_boundary(
    /* module id */ 7,
    /* fifo */ fifo_C_C_IO_L2_in_7,
    /* fifo */ fifo_C_PE_0_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_0_0,
    /* fifo */ fifo_A_PE_0_1,
    /* fifo */ fifo_B_PE_0_0,
    /* fifo */ fifo_B_PE_1_0,
    /* fifo */ fifo_C_PE_0_0,
    /* fifo */ fifo_C_PE_1_0,
    /* fifo */ fifo_D_drain_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_0_1,
    /* fifo */ fifo_A_PE_0_2,
    /* fifo */ fifo_B_PE_0_1,
    /* fifo */ fifo_B_PE_1_1,
    /* fifo */ fifo_C_PE_0_1,
    /* fifo */ fifo_C_PE_1_1,
    /* fifo */ fifo_D_drain_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_0_2,
    /* fifo */ fifo_A_PE_0_3,
    /* fifo */ fifo_B_PE_0_2,
    /* fifo */ fifo_B_PE_1_2,
    /* fifo */ fifo_C_PE_0_2,
    /* fifo */ fifo_C_PE_1_2,
    /* fifo */ fifo_D_drain_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_0_3,
    /* fifo */ fifo_A_PE_0_4,
    /* fifo */ fifo_B_PE_0_3,
    /* fifo */ fifo_B_PE_1_3,
    /* fifo */ fifo_C_PE_0_3,
    /* fifo */ fifo_C_PE_1_3,
    /* fifo */ fifo_D_drain_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 4,
    /* fifo */ fifo_A_PE_0_4,
    /* fifo */ fifo_A_PE_0_5,
    /* fifo */ fifo_B_PE_0_4,
    /* fifo */ fifo_B_PE_1_4,
    /* fifo */ fifo_C_PE_0_4,
    /* fifo */ fifo_C_PE_1_4,
    /* fifo */ fifo_D_drain_PE_0_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 5,
    /* fifo */ fifo_A_PE_0_5,
    /* fifo */ fifo_A_PE_0_6,
    /* fifo */ fifo_B_PE_0_5,
    /* fifo */ fifo_B_PE_1_5,
    /* fifo */ fifo_C_PE_0_5,
    /* fifo */ fifo_C_PE_1_5,
    /* fifo */ fifo_D_drain_PE_0_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 6,
    /* fifo */ fifo_A_PE_0_6,
    /* fifo */ fifo_A_PE_0_7,
    /* fifo */ fifo_B_PE_0_6,
    /* fifo */ fifo_B_PE_1_6,
    /* fifo */ fifo_C_PE_0_6,
    /* fifo */ fifo_C_PE_1_6,
    /* fifo */ fifo_D_drain_PE_0_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_0_7,
    /* fifo */ fifo_A_PE_0_8,
    /* fifo */ fifo_B_PE_0_7,
    /* fifo */ fifo_B_PE_1_7,
    /* fifo */ fifo_C_PE_0_7,
    /* fifo */ fifo_C_PE_1_7,
    /* fifo */ fifo_D_drain_PE_0_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_1_0,
    /* fifo */ fifo_A_PE_1_1,
    /* fifo */ fifo_B_PE_1_0,
    /* fifo */ fifo_B_PE_2_0,
    /* fifo */ fifo_C_PE_1_0,
    /* fifo */ fifo_C_PE_2_0,
    /* fifo */ fifo_D_drain_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_1_1,
    /* fifo */ fifo_A_PE_1_2,
    /* fifo */ fifo_B_PE_1_1,
    /* fifo */ fifo_B_PE_2_1,
    /* fifo */ fifo_C_PE_1_1,
    /* fifo */ fifo_C_PE_2_1,
    /* fifo */ fifo_D_drain_PE_1_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_1_2,
    /* fifo */ fifo_A_PE_1_3,
    /* fifo */ fifo_B_PE_1_2,
    /* fifo */ fifo_B_PE_2_2,
    /* fifo */ fifo_C_PE_1_2,
    /* fifo */ fifo_C_PE_2_2,
    /* fifo */ fifo_D_drain_PE_1_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_1_3,
    /* fifo */ fifo_A_PE_1_4,
    /* fifo */ fifo_B_PE_1_3,
    /* fifo */ fifo_B_PE_2_3,
    /* fifo */ fifo_C_PE_1_3,
    /* fifo */ fifo_C_PE_2_3,
    /* fifo */ fifo_D_drain_PE_1_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 4,
    /* fifo */ fifo_A_PE_1_4,
    /* fifo */ fifo_A_PE_1_5,
    /* fifo */ fifo_B_PE_1_4,
    /* fifo */ fifo_B_PE_2_4,
    /* fifo */ fifo_C_PE_1_4,
    /* fifo */ fifo_C_PE_2_4,
    /* fifo */ fifo_D_drain_PE_1_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 5,
    /* fifo */ fifo_A_PE_1_5,
    /* fifo */ fifo_A_PE_1_6,
    /* fifo */ fifo_B_PE_1_5,
    /* fifo */ fifo_B_PE_2_5,
    /* fifo */ fifo_C_PE_1_5,
    /* fifo */ fifo_C_PE_2_5,
    /* fifo */ fifo_D_drain_PE_1_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 6,
    /* fifo */ fifo_A_PE_1_6,
    /* fifo */ fifo_A_PE_1_7,
    /* fifo */ fifo_B_PE_1_6,
    /* fifo */ fifo_B_PE_2_6,
    /* fifo */ fifo_C_PE_1_6,
    /* fifo */ fifo_C_PE_2_6,
    /* fifo */ fifo_D_drain_PE_1_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_1_7,
    /* fifo */ fifo_A_PE_1_8,
    /* fifo */ fifo_B_PE_1_7,
    /* fifo */ fifo_B_PE_2_7,
    /* fifo */ fifo_C_PE_1_7,
    /* fifo */ fifo_C_PE_2_7,
    /* fifo */ fifo_D_drain_PE_1_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_2_0,
    /* fifo */ fifo_A_PE_2_1,
    /* fifo */ fifo_B_PE_2_0,
    /* fifo */ fifo_B_PE_3_0,
    /* fifo */ fifo_C_PE_2_0,
    /* fifo */ fifo_C_PE_3_0,
    /* fifo */ fifo_D_drain_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_2_1,
    /* fifo */ fifo_A_PE_2_2,
    /* fifo */ fifo_B_PE_2_1,
    /* fifo */ fifo_B_PE_3_1,
    /* fifo */ fifo_C_PE_2_1,
    /* fifo */ fifo_C_PE_3_1,
    /* fifo */ fifo_D_drain_PE_2_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_2_2,
    /* fifo */ fifo_A_PE_2_3,
    /* fifo */ fifo_B_PE_2_2,
    /* fifo */ fifo_B_PE_3_2,
    /* fifo */ fifo_C_PE_2_2,
    /* fifo */ fifo_C_PE_3_2,
    /* fifo */ fifo_D_drain_PE_2_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_2_3,
    /* fifo */ fifo_A_PE_2_4,
    /* fifo */ fifo_B_PE_2_3,
    /* fifo */ fifo_B_PE_3_3,
    /* fifo */ fifo_C_PE_2_3,
    /* fifo */ fifo_C_PE_3_3,
    /* fifo */ fifo_D_drain_PE_2_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 4,
    /* fifo */ fifo_A_PE_2_4,
    /* fifo */ fifo_A_PE_2_5,
    /* fifo */ fifo_B_PE_2_4,
    /* fifo */ fifo_B_PE_3_4,
    /* fifo */ fifo_C_PE_2_4,
    /* fifo */ fifo_C_PE_3_4,
    /* fifo */ fifo_D_drain_PE_2_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 5,
    /* fifo */ fifo_A_PE_2_5,
    /* fifo */ fifo_A_PE_2_6,
    /* fifo */ fifo_B_PE_2_5,
    /* fifo */ fifo_B_PE_3_5,
    /* fifo */ fifo_C_PE_2_5,
    /* fifo */ fifo_C_PE_3_5,
    /* fifo */ fifo_D_drain_PE_2_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 6,
    /* fifo */ fifo_A_PE_2_6,
    /* fifo */ fifo_A_PE_2_7,
    /* fifo */ fifo_B_PE_2_6,
    /* fifo */ fifo_B_PE_3_6,
    /* fifo */ fifo_C_PE_2_6,
    /* fifo */ fifo_C_PE_3_6,
    /* fifo */ fifo_D_drain_PE_2_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_2_7,
    /* fifo */ fifo_A_PE_2_8,
    /* fifo */ fifo_B_PE_2_7,
    /* fifo */ fifo_B_PE_3_7,
    /* fifo */ fifo_C_PE_2_7,
    /* fifo */ fifo_C_PE_3_7,
    /* fifo */ fifo_D_drain_PE_2_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_3_0,
    /* fifo */ fifo_A_PE_3_1,
    /* fifo */ fifo_B_PE_3_0,
    /* fifo */ fifo_B_PE_4_0,
    /* fifo */ fifo_C_PE_3_0,
    /* fifo */ fifo_C_PE_4_0,
    /* fifo */ fifo_D_drain_PE_3_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_3_1,
    /* fifo */ fifo_A_PE_3_2,
    /* fifo */ fifo_B_PE_3_1,
    /* fifo */ fifo_B_PE_4_1,
    /* fifo */ fifo_C_PE_3_1,
    /* fifo */ fifo_C_PE_4_1,
    /* fifo */ fifo_D_drain_PE_3_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_3_2,
    /* fifo */ fifo_A_PE_3_3,
    /* fifo */ fifo_B_PE_3_2,
    /* fifo */ fifo_B_PE_4_2,
    /* fifo */ fifo_C_PE_3_2,
    /* fifo */ fifo_C_PE_4_2,
    /* fifo */ fifo_D_drain_PE_3_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_3_3,
    /* fifo */ fifo_A_PE_3_4,
    /* fifo */ fifo_B_PE_3_3,
    /* fifo */ fifo_B_PE_4_3,
    /* fifo */ fifo_C_PE_3_3,
    /* fifo */ fifo_C_PE_4_3,
    /* fifo */ fifo_D_drain_PE_3_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 4,
    /* fifo */ fifo_A_PE_3_4,
    /* fifo */ fifo_A_PE_3_5,
    /* fifo */ fifo_B_PE_3_4,
    /* fifo */ fifo_B_PE_4_4,
    /* fifo */ fifo_C_PE_3_4,
    /* fifo */ fifo_C_PE_4_4,
    /* fifo */ fifo_D_drain_PE_3_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 5,
    /* fifo */ fifo_A_PE_3_5,
    /* fifo */ fifo_A_PE_3_6,
    /* fifo */ fifo_B_PE_3_5,
    /* fifo */ fifo_B_PE_4_5,
    /* fifo */ fifo_C_PE_3_5,
    /* fifo */ fifo_C_PE_4_5,
    /* fifo */ fifo_D_drain_PE_3_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 6,
    /* fifo */ fifo_A_PE_3_6,
    /* fifo */ fifo_A_PE_3_7,
    /* fifo */ fifo_B_PE_3_6,
    /* fifo */ fifo_B_PE_4_6,
    /* fifo */ fifo_C_PE_3_6,
    /* fifo */ fifo_C_PE_4_6,
    /* fifo */ fifo_D_drain_PE_3_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_3_7,
    /* fifo */ fifo_A_PE_3_8,
    /* fifo */ fifo_B_PE_3_7,
    /* fifo */ fifo_B_PE_4_7,
    /* fifo */ fifo_C_PE_3_7,
    /* fifo */ fifo_C_PE_4_7,
    /* fifo */ fifo_D_drain_PE_3_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_4_0,
    /* fifo */ fifo_A_PE_4_1,
    /* fifo */ fifo_B_PE_4_0,
    /* fifo */ fifo_B_PE_5_0,
    /* fifo */ fifo_C_PE_4_0,
    /* fifo */ fifo_C_PE_5_0,
    /* fifo */ fifo_D_drain_PE_4_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_4_1,
    /* fifo */ fifo_A_PE_4_2,
    /* fifo */ fifo_B_PE_4_1,
    /* fifo */ fifo_B_PE_5_1,
    /* fifo */ fifo_C_PE_4_1,
    /* fifo */ fifo_C_PE_5_1,
    /* fifo */ fifo_D_drain_PE_4_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_4_2,
    /* fifo */ fifo_A_PE_4_3,
    /* fifo */ fifo_B_PE_4_2,
    /* fifo */ fifo_B_PE_5_2,
    /* fifo */ fifo_C_PE_4_2,
    /* fifo */ fifo_C_PE_5_2,
    /* fifo */ fifo_D_drain_PE_4_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_4_3,
    /* fifo */ fifo_A_PE_4_4,
    /* fifo */ fifo_B_PE_4_3,
    /* fifo */ fifo_B_PE_5_3,
    /* fifo */ fifo_C_PE_4_3,
    /* fifo */ fifo_C_PE_5_3,
    /* fifo */ fifo_D_drain_PE_4_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 4,
    /* fifo */ fifo_A_PE_4_4,
    /* fifo */ fifo_A_PE_4_5,
    /* fifo */ fifo_B_PE_4_4,
    /* fifo */ fifo_B_PE_5_4,
    /* fifo */ fifo_C_PE_4_4,
    /* fifo */ fifo_C_PE_5_4,
    /* fifo */ fifo_D_drain_PE_4_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 5,
    /* fifo */ fifo_A_PE_4_5,
    /* fifo */ fifo_A_PE_4_6,
    /* fifo */ fifo_B_PE_4_5,
    /* fifo */ fifo_B_PE_5_5,
    /* fifo */ fifo_C_PE_4_5,
    /* fifo */ fifo_C_PE_5_5,
    /* fifo */ fifo_D_drain_PE_4_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 6,
    /* fifo */ fifo_A_PE_4_6,
    /* fifo */ fifo_A_PE_4_7,
    /* fifo */ fifo_B_PE_4_6,
    /* fifo */ fifo_B_PE_5_6,
    /* fifo */ fifo_C_PE_4_6,
    /* fifo */ fifo_C_PE_5_6,
    /* fifo */ fifo_D_drain_PE_4_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_4_7,
    /* fifo */ fifo_A_PE_4_8,
    /* fifo */ fifo_B_PE_4_7,
    /* fifo */ fifo_B_PE_5_7,
    /* fifo */ fifo_C_PE_4_7,
    /* fifo */ fifo_C_PE_5_7,
    /* fifo */ fifo_D_drain_PE_4_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_5_0,
    /* fifo */ fifo_A_PE_5_1,
    /* fifo */ fifo_B_PE_5_0,
    /* fifo */ fifo_B_PE_6_0,
    /* fifo */ fifo_C_PE_5_0,
    /* fifo */ fifo_C_PE_6_0,
    /* fifo */ fifo_D_drain_PE_5_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_5_1,
    /* fifo */ fifo_A_PE_5_2,
    /* fifo */ fifo_B_PE_5_1,
    /* fifo */ fifo_B_PE_6_1,
    /* fifo */ fifo_C_PE_5_1,
    /* fifo */ fifo_C_PE_6_1,
    /* fifo */ fifo_D_drain_PE_5_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_5_2,
    /* fifo */ fifo_A_PE_5_3,
    /* fifo */ fifo_B_PE_5_2,
    /* fifo */ fifo_B_PE_6_2,
    /* fifo */ fifo_C_PE_5_2,
    /* fifo */ fifo_C_PE_6_2,
    /* fifo */ fifo_D_drain_PE_5_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_5_3,
    /* fifo */ fifo_A_PE_5_4,
    /* fifo */ fifo_B_PE_5_3,
    /* fifo */ fifo_B_PE_6_3,
    /* fifo */ fifo_C_PE_5_3,
    /* fifo */ fifo_C_PE_6_3,
    /* fifo */ fifo_D_drain_PE_5_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 4,
    /* fifo */ fifo_A_PE_5_4,
    /* fifo */ fifo_A_PE_5_5,
    /* fifo */ fifo_B_PE_5_4,
    /* fifo */ fifo_B_PE_6_4,
    /* fifo */ fifo_C_PE_5_4,
    /* fifo */ fifo_C_PE_6_4,
    /* fifo */ fifo_D_drain_PE_5_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 5,
    /* fifo */ fifo_A_PE_5_5,
    /* fifo */ fifo_A_PE_5_6,
    /* fifo */ fifo_B_PE_5_5,
    /* fifo */ fifo_B_PE_6_5,
    /* fifo */ fifo_C_PE_5_5,
    /* fifo */ fifo_C_PE_6_5,
    /* fifo */ fifo_D_drain_PE_5_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 6,
    /* fifo */ fifo_A_PE_5_6,
    /* fifo */ fifo_A_PE_5_7,
    /* fifo */ fifo_B_PE_5_6,
    /* fifo */ fifo_B_PE_6_6,
    /* fifo */ fifo_C_PE_5_6,
    /* fifo */ fifo_C_PE_6_6,
    /* fifo */ fifo_D_drain_PE_5_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_5_7,
    /* fifo */ fifo_A_PE_5_8,
    /* fifo */ fifo_B_PE_5_7,
    /* fifo */ fifo_B_PE_6_7,
    /* fifo */ fifo_C_PE_5_7,
    /* fifo */ fifo_C_PE_6_7,
    /* fifo */ fifo_D_drain_PE_5_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_6_0,
    /* fifo */ fifo_A_PE_6_1,
    /* fifo */ fifo_B_PE_6_0,
    /* fifo */ fifo_B_PE_7_0,
    /* fifo */ fifo_C_PE_6_0,
    /* fifo */ fifo_C_PE_7_0,
    /* fifo */ fifo_D_drain_PE_6_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_6_1,
    /* fifo */ fifo_A_PE_6_2,
    /* fifo */ fifo_B_PE_6_1,
    /* fifo */ fifo_B_PE_7_1,
    /* fifo */ fifo_C_PE_6_1,
    /* fifo */ fifo_C_PE_7_1,
    /* fifo */ fifo_D_drain_PE_6_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_6_2,
    /* fifo */ fifo_A_PE_6_3,
    /* fifo */ fifo_B_PE_6_2,
    /* fifo */ fifo_B_PE_7_2,
    /* fifo */ fifo_C_PE_6_2,
    /* fifo */ fifo_C_PE_7_2,
    /* fifo */ fifo_D_drain_PE_6_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_6_3,
    /* fifo */ fifo_A_PE_6_4,
    /* fifo */ fifo_B_PE_6_3,
    /* fifo */ fifo_B_PE_7_3,
    /* fifo */ fifo_C_PE_6_3,
    /* fifo */ fifo_C_PE_7_3,
    /* fifo */ fifo_D_drain_PE_6_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 4,
    /* fifo */ fifo_A_PE_6_4,
    /* fifo */ fifo_A_PE_6_5,
    /* fifo */ fifo_B_PE_6_4,
    /* fifo */ fifo_B_PE_7_4,
    /* fifo */ fifo_C_PE_6_4,
    /* fifo */ fifo_C_PE_7_4,
    /* fifo */ fifo_D_drain_PE_6_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 5,
    /* fifo */ fifo_A_PE_6_5,
    /* fifo */ fifo_A_PE_6_6,
    /* fifo */ fifo_B_PE_6_5,
    /* fifo */ fifo_B_PE_7_5,
    /* fifo */ fifo_C_PE_6_5,
    /* fifo */ fifo_C_PE_7_5,
    /* fifo */ fifo_D_drain_PE_6_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 6,
    /* fifo */ fifo_A_PE_6_6,
    /* fifo */ fifo_A_PE_6_7,
    /* fifo */ fifo_B_PE_6_6,
    /* fifo */ fifo_B_PE_7_6,
    /* fifo */ fifo_C_PE_6_6,
    /* fifo */ fifo_C_PE_7_6,
    /* fifo */ fifo_D_drain_PE_6_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_6_7,
    /* fifo */ fifo_A_PE_6_8,
    /* fifo */ fifo_B_PE_6_7,
    /* fifo */ fifo_B_PE_7_7,
    /* fifo */ fifo_C_PE_6_7,
    /* fifo */ fifo_C_PE_7_7,
    /* fifo */ fifo_D_drain_PE_6_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_7_0,
    /* fifo */ fifo_A_PE_7_1,
    /* fifo */ fifo_B_PE_7_0,
    /* fifo */ fifo_B_PE_8_0,
    /* fifo */ fifo_C_PE_7_0,
    /* fifo */ fifo_C_PE_8_0,
    /* fifo */ fifo_D_drain_PE_7_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_7_1,
    /* fifo */ fifo_A_PE_7_2,
    /* fifo */ fifo_B_PE_7_1,
    /* fifo */ fifo_B_PE_8_1,
    /* fifo */ fifo_C_PE_7_1,
    /* fifo */ fifo_C_PE_8_1,
    /* fifo */ fifo_D_drain_PE_7_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* module id */ 2,
    /* fifo */ fifo_A_PE_7_2,
    /* fifo */ fifo_A_PE_7_3,
    /* fifo */ fifo_B_PE_7_2,
    /* fifo */ fifo_B_PE_8_2,
    /* fifo */ fifo_C_PE_7_2,
    /* fifo */ fifo_C_PE_8_2,
    /* fifo */ fifo_D_drain_PE_7_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* module id */ 3,
    /* fifo */ fifo_A_PE_7_3,
    /* fifo */ fifo_A_PE_7_4,
    /* fifo */ fifo_B_PE_7_3,
    /* fifo */ fifo_B_PE_8_3,
    /* fifo */ fifo_C_PE_7_3,
    /* fifo */ fifo_C_PE_8_3,
    /* fifo */ fifo_D_drain_PE_7_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* module id */ 4,
    /* fifo */ fifo_A_PE_7_4,
    /* fifo */ fifo_A_PE_7_5,
    /* fifo */ fifo_B_PE_7_4,
    /* fifo */ fifo_B_PE_8_4,
    /* fifo */ fifo_C_PE_7_4,
    /* fifo */ fifo_C_PE_8_4,
    /* fifo */ fifo_D_drain_PE_7_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* module id */ 5,
    /* fifo */ fifo_A_PE_7_5,
    /* fifo */ fifo_A_PE_7_6,
    /* fifo */ fifo_B_PE_7_5,
    /* fifo */ fifo_B_PE_8_5,
    /* fifo */ fifo_C_PE_7_5,
    /* fifo */ fifo_C_PE_8_5,
    /* fifo */ fifo_D_drain_PE_7_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* module id */ 6,
    /* fifo */ fifo_A_PE_7_6,
    /* fifo */ fifo_A_PE_7_7,
    /* fifo */ fifo_B_PE_7_6,
    /* fifo */ fifo_B_PE_8_6,
    /* fifo */ fifo_C_PE_7_6,
    /* fifo */ fifo_C_PE_8_6,
    /* fifo */ fifo_D_drain_PE_7_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_7_7,
    /* fifo */ fifo_A_PE_7_8,
    /* fifo */ fifo_B_PE_7_7,
    /* fifo */ fifo_B_PE_8_7,
    /* fifo */ fifo_C_PE_7_7,
    /* fifo */ fifo_C_PE_8_7,
    /* fifo */ fifo_D_drain_PE_7_7
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 0,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_0_8
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 1,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_1_8
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 2,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_2_8
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 3,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_3_8
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 4,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_4_8
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 5,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_5_8
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 6,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_6_8
  );
  /* Module Call */

  /* Module Call */
  A_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 7,
    /* fifo */ fifo_A_PE_7_8
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 0,
    /* fifo */ fifo_B_PE_8_0
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 1,
    /* fifo */ fifo_B_PE_8_1
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 2,
    /* fifo */ fifo_B_PE_8_2
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 3,
    /* fifo */ fifo_B_PE_8_3
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 4,
    /* fifo */ fifo_B_PE_8_4
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 5,
    /* fifo */ fifo_B_PE_8_5
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 6,
    /* fifo */ fifo_B_PE_8_6
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 7,
    /* fifo */ fifo_B_PE_8_7
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 0,
    /* fifo */ fifo_C_PE_8_0
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 1,
    /* fifo */ fifo_C_PE_8_1
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 2,
    /* fifo */ fifo_C_PE_8_2
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 3,
    /* fifo */ fifo_C_PE_8_3
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 4,
    /* fifo */ fifo_C_PE_8_4
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 5,
    /* fifo */ fifo_C_PE_8_5
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 6,
    /* fifo */ fifo_C_PE_8_6
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_in(
    /* module id */ 7,
    /* module id */ 7,
    /* fifo */ fifo_C_PE_8_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 0,
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_7,
    /* fifo */ fifo_D_drain_PE_7_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_6,
    /* fifo */ fifo_D_drain_PE_6_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_5,
    /* fifo */ fifo_D_drain_PE_5_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_4,
    /* fifo */ fifo_D_drain_PE_4_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_3,
    /* fifo */ fifo_D_drain_PE_3_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_2,
    /* fifo */ fifo_D_drain_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_1,
    /* fifo */ fifo_D_drain_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_0,
    /* fifo */ fifo_D_drain_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 1,
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_7,
    /* fifo */ fifo_D_drain_PE_7_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_6,
    /* fifo */ fifo_D_drain_PE_6_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_5,
    /* fifo */ fifo_D_drain_PE_5_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_4,
    /* fifo */ fifo_D_drain_PE_4_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_3,
    /* fifo */ fifo_D_drain_PE_3_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_2,
    /* fifo */ fifo_D_drain_PE_2_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_1,
    /* fifo */ fifo_D_drain_PE_1_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_0,
    /* fifo */ fifo_D_drain_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 2,
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_7,
    /* fifo */ fifo_D_drain_PE_7_2
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_6,
    /* fifo */ fifo_D_drain_PE_6_2
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_5,
    /* fifo */ fifo_D_drain_PE_5_2
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_4,
    /* fifo */ fifo_D_drain_PE_4_2
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_3,
    /* fifo */ fifo_D_drain_PE_3_2
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_2,
    /* fifo */ fifo_D_drain_PE_2_2
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_1,
    /* fifo */ fifo_D_drain_PE_1_2
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_0,
    /* fifo */ fifo_D_drain_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 3,
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_7,
    /* fifo */ fifo_D_drain_PE_7_3
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_6,
    /* fifo */ fifo_D_drain_PE_6_3
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_5,
    /* fifo */ fifo_D_drain_PE_5_3
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_4,
    /* fifo */ fifo_D_drain_PE_4_3
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_3,
    /* fifo */ fifo_D_drain_PE_3_3
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_2,
    /* fifo */ fifo_D_drain_PE_2_3
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_1,
    /* fifo */ fifo_D_drain_PE_1_3
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_0,
    /* fifo */ fifo_D_drain_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 4,
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_7,
    /* fifo */ fifo_D_drain_PE_7_4
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_6,
    /* fifo */ fifo_D_drain_PE_6_4
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_5,
    /* fifo */ fifo_D_drain_PE_5_4
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_4,
    /* fifo */ fifo_D_drain_PE_4_4
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_3,
    /* fifo */ fifo_D_drain_PE_3_4
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_2,
    /* fifo */ fifo_D_drain_PE_2_4
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_1,
    /* fifo */ fifo_D_drain_PE_1_4
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_0,
    /* fifo */ fifo_D_drain_PE_0_4
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 5,
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_7,
    /* fifo */ fifo_D_drain_PE_7_5
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_6,
    /* fifo */ fifo_D_drain_PE_6_5
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_5,
    /* fifo */ fifo_D_drain_PE_5_5
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_4,
    /* fifo */ fifo_D_drain_PE_4_5
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_3,
    /* fifo */ fifo_D_drain_PE_3_5
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_2,
    /* fifo */ fifo_D_drain_PE_2_5
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_1,
    /* fifo */ fifo_D_drain_PE_1_5
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_0,
    /* fifo */ fifo_D_drain_PE_0_5
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 6,
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_7,
    /* fifo */ fifo_D_drain_PE_7_6
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_6,
    /* fifo */ fifo_D_drain_PE_6_6
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_5,
    /* fifo */ fifo_D_drain_PE_5_6
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_4,
    /* fifo */ fifo_D_drain_PE_4_6
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_3,
    /* fifo */ fifo_D_drain_PE_3_6
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_2,
    /* fifo */ fifo_D_drain_PE_2_6
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_1,
    /* fifo */ fifo_D_drain_PE_1_6
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_0,
    /* fifo */ fifo_D_drain_PE_0_6
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 7,
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_7,
    /* fifo */ fifo_D_drain_PE_7_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 7,
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_6,
    /* fifo */ fifo_D_drain_PE_6_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 7,
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_5,
    /* fifo */ fifo_D_drain_PE_5_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 7,
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_4,
    /* fifo */ fifo_D_drain_PE_4_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 7,
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_3,
    /* fifo */ fifo_D_drain_PE_3_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 7,
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_2,
    /* fifo */ fifo_D_drain_PE_2_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 7,
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_1,
    /* fifo */ fifo_D_drain_PE_1_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L1_out_wrapper(
    /* module id */ 7,
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_0,
    /* fifo */ fifo_D_drain_PE_0_7
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out_boundary(
    /* module id */ 7,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_7_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out(
    /* module id */ 6,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_7,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_6_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out(
    /* module id */ 5,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_6,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_5_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out(
    /* module id */ 4,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_5,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_4_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out(
    /* module id */ 3,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_4,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_3_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out(
    /* module id */ 2,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_3,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_2_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out(
    /* module id */ 1,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_2,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_1_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L2_out(
    /* module id */ 0,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_1,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_0,
    /* fifo */ fifo_D_drain_D_drain_IO_L1_out_0_0
  );
  /* Module Call */

  /* Module Call */
  D_drain_IO_L3_out(
    /* array */ D,
    /* fifo */ fifo_D_drain_D_drain_IO_L2_out_0
  );
  /* Module Call */

}
}

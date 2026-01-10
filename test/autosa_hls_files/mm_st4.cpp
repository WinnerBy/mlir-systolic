#include <ap_int.h>
#include <hls_stream.h>

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Data Type */
typedef float A_t1;
typedef float B_t1;
typedef float C_t1;
typedef ap_uint<64> A_t2;
typedef ap_uint<64> B_t2;
typedef ap_uint<512> C_t16;
/* Data Type */

extern "C" {
void kernel0(A_t2 *A, B_t2 *B, C_t16 *C);
}
void A_IO_L1_in_intra_trans(int idx, int idy, int c0, int c1, int c2, A_t2 local_A[16][1], hls::stream<A_t2> &fifo_A_local_out, bool intra_trans_en);
void A_IO_L1_in_inter_trans(int idx, int idy, int c0, int c1, int c2, A_t2 local_A[16][1], hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_out, bool inter_trans_en);
void A_IO_L1_in_inter_trans_boundary(int idx, int idy, int c0, int c1, int c2, A_t2 local_A[16][1], hls::stream<A_t2> &fifo_A_in, bool inter_trans_en);
void A_IO_L1_in_wrapper(int idx, int idy, hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_out, hls::stream<A_t2> &fifo_A_local_out);
void A_IO_L1_in_boundary_wrapper(int idx, int idy, hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_local_out);
void B_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, B_t2 local_B[32][1], hls::stream<B_t2> &fifo_B_local_out, bool intra_trans_en);
void B_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, B_t2 local_B[32][1], hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out, bool inter_trans_en);
void B_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, B_t2 local_B[32][1], hls::stream<B_t2> &fifo_B_in, bool inter_trans_en);
void PE_wrapper(int idx, int idy, hls::stream<A_t2> &fifo_A_in, hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out, hls::stream<float> &fifo_C_in, hls::stream<float> &fifo_C_out);
void C_IO_L2_out_intra_trans(int idx, int c0, int c1, C_t16 local_C[16][2], hls::stream<float> &fifo_C_local_in, bool intra_trans_en);
void C_IO_L2_out_inter_trans(int idx, int c0, int c1, C_t16 local_C[16][2], hls::stream<C_t16> &fifo_C_in, hls::stream<C_t16> &fifo_C_out, bool inter_trans_en);
void C_IO_L2_out_inter_trans_boundary(int idx, int c0, int c1, C_t16 local_C[16][2], hls::stream<C_t16> &fifo_C_out, bool inter_trans_en);

/* Module Definition */
void A_IO_L3_in(A_t2 *A, hls::stream<A_t2> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
    for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        for (ap_uint<2> c3 = 0; c3 <= 1; c3 += 1) {
          // io_L2
          for (ap_uint<2> c4 = 0; c4 <= 1; c4 += 1) {
            // io_L1
            for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
            #pragma HLS PIPELINE II=1
              // access_coalesce
              {
                A_t2 in_data;
                A_t2 out_data;
                in_data = A[1024*c0 + 2*c2 + c3 + 512*c4 + 32*c5];
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
void A_IO_L2_in(int idx, hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_out, hls::stream<A_t2> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
    for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        for (ap_uint<2> c3 = p0; c3 <= 1; c3 += 1) {
          // io_L2
          if (c3 == p0) {
            for (ap_uint<2> c4 = 0; c4 <= 1; c4 += 1) {
              // io_L1
              for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
              #pragma HLS PIPELINE II=1
                // access_coalesce
                {
                  A_t2 in_data;
                  A_t2 out_data;
                  in_data = fifo_A_in.read();
                  out_data = in_data;
                  fifo_A_local_out.write(out_data);
                }
              }
            }
          } else {
            for (ap_uint<2> c4 = 0; c4 <= 1; c4 += 1) {
              // io_L1
              for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
              #pragma HLS PIPELINE II=1
                // access_coalesce
                {
                  A_t2 in_data;
                  A_t2 out_data;
                  in_data = fifo_A_in.read();
                  out_data = in_data;
                  fifo_A_out.write(out_data);
                }
              }
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in_boundary(int idx, hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
    for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        for (ap_uint<2> c3 = p0; c3 <= 1; c3 += 1)
          if (c3 == p0) {
            // io_L2
            for (ap_uint<2> c4 = 0; c4 <= 1; c4 += 1) {
              // io_L1
              for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
              #pragma HLS PIPELINE II=1
                // access_coalesce
                {
                  A_t2 in_data;
                  A_t2 out_data;
                  in_data = fifo_A_in.read();
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
void A_IO_L1_in_intra_trans(int idx, int idy, int c0, int c1, int c2, A_t2 local_A[16][1], hls::stream<A_t2> &fifo_A_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L1
  // pe
  for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
    // latency
    for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
      // latency
      for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
      #pragma HLS PIPELINE II=1
        // simd
        {
          A_t2 in_data;
          A_t2 out_data;
          in_data = local_A[c7][0];
          out_data = in_data;
          fifo_A_local_out.write(out_data);
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_inter_trans(int idx, int idy, int c0, int c1, int c2, A_t2 local_A[16][1], hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<2> c4 = p1; c4 <= 1; c4 += 1) {
    // io_L1
    if (c4 == p1) {
      for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          A_t2 in_data;
          A_t2 out_data;
          in_data = fifo_A_in.read();
          out_data = in_data;
          local_A[c5][0] = out_data;
        }
      }
    } else {
      for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          A_t2 in_data;
          A_t2 out_data;
          in_data = fifo_A_in.read();
          out_data = in_data;
          fifo_A_out.write(out_data);
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_inter_trans_boundary(int idx, int idy, int c0, int c1, int c2, A_t2 local_A[16][1], hls::stream<A_t2> &fifo_A_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<2> c4 = p1; c4 <= 1; c4 += 1)
    if (c4 == p1) {
      // io_L1
      for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          A_t2 in_data;
          A_t2 out_data;
          in_data = fifo_A_in.read();
          out_data = in_data;
          local_A[c5][0] = out_data;
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in(int idx, int idy, hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_out, hls::stream<A_t2> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  A_t2 local_A_ping[16][1];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_1P_BRAM
  A_t2 local_A_pong[16][1];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  /* Variable Declaration */

  {
    for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
      for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
          // array
          // io_L3
          // io_L2
          {
            if (arb == 0) {
              A_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* module id */ idy, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_A_pong, 
                /* fifo */ fifo_A_in, 
                /* fifo */ fifo_A_out, 
                /* enable */ inter_trans_en
              );
              A_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* module id */ idy, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
                /* array */ local_A_ping, 
                /* fifo */ fifo_A_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              A_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* module id */ idy, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_A_ping, 
                /* fifo */ fifo_A_in, 
                /* fifo */ fifo_A_out, 
                /* enable */ inter_trans_en
              );
              A_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* module id */ idy, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
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
          }
        }
    if (arb == 0) {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* module id */ idy, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_A_ping, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* module id */ idy, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_A_pong, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_wrapper(int idx, int idy, hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_out, hls::stream<A_t2> &fifo_A_local_out)
 {
  A_IO_L1_in(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_out, 
    /* fifo */ fifo_A_local_out);
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_boundary(int idx, int idy, hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  A_t2 local_A_ping[16][1];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_1P_BRAM
  A_t2 local_A_pong[16][1];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  /* Variable Declaration */

  {
    for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
      for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
          // array
          // io_L3
          // io_L2
          {
            if (arb == 0) {
              A_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* module id */ idy, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_A_pong, 
                /* fifo */ fifo_A_in, 
                /* enable */ inter_trans_en
              );
              A_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* module id */ idy, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
                /* array */ local_A_ping, 
                /* fifo */ fifo_A_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              A_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* module id */ idy, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_A_ping, 
                /* fifo */ fifo_A_in, 
                /* enable */ inter_trans_en
              );
              A_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* module id */ idy, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
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
          }
        }
    if (arb == 0) {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* module id */ idy, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_A_ping, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      A_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* module id */ idy, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
        /* array */ local_A_pong, 
        /* fifo */ fifo_A_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_boundary_wrapper(int idx, int idy, hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_local_out)
 {
  A_IO_L1_in_boundary(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_local_out);
}
/* Module Definition */

/* Module Definition */
void B_IO_L3_in(B_t2 *B, hls::stream<B_t2> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
    for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // io_L3
        for (ap_uint<2> c3 = 0; c3 <= 1; c3 += 1) {
          // io_L2
          for (ap_uint<6> c4 = 0; c4 <= 31; c4 += 1) {
          #pragma HLS PIPELINE II=1
            // access_coalesce
            {
              B_t2 in_data;
              B_t2 out_data;
              in_data = B[1024*c1 + 2*c2 + c3 + 32*c4];
              out_data = in_data;
              fifo_B_local_out.write(out_data);
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_intra_trans(int idx, int c0, int c1, int c2, B_t2 local_B[32][1], hls::stream<B_t2> &fifo_B_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
    // latency
    for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
      // latency
      for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
      #pragma HLS PIPELINE II=1
        // simd
        {
          B_t2 in_data;
          B_t2 out_data;
          in_data = local_B[16*c5 + c6][0];
          out_data = in_data;
          fifo_B_local_out.write(out_data);
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, B_t2 local_B[32][1], hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<2> c3 = p0; c3 <= 1; c3 += 1) {
    // io_L2
    if (c3 == p0) {
      for (ap_uint<6> c4 = 0; c4 <= 31; c4 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          B_t2 in_data;
          B_t2 out_data;
          in_data = fifo_B_in.read();
          out_data = in_data;
          local_B[c4][0] = out_data;
        }
      }
    } else {
      for (ap_uint<6> c4 = 0; c4 <= 31; c4 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          B_t2 in_data;
          B_t2 out_data;
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
void B_IO_L2_in_inter_trans_boundary(int idx, int c0, int c1, int c2, B_t2 local_B[32][1], hls::stream<B_t2> &fifo_B_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<2> c3 = p0; c3 <= 1; c3 += 1)
    if (c3 == p0) {
      // io_L2
      for (ap_uint<6> c4 = 0; c4 <= 31; c4 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          B_t2 in_data;
          B_t2 out_data;
          in_data = fifo_B_in.read();
          out_data = in_data;
          local_B[c4][0] = out_data;
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in(int idx, hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out, hls::stream<B_t2> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  B_t2 local_B_ping[32][1];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_1P_BRAM
  B_t2 local_B_pong[32][1];
  #pragma HLS RESOURCE variable=local_B_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  /* Variable Declaration */

  {
    for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
      for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
          // array
          // io_L3
          {
            if (arb == 0) {
              B_IO_L2_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
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
          }
        }
    if (arb == 0) {
      B_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
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
        /* array */ local_B_pong, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_boundary(int idx, hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  B_t2 local_B_ping[32][1];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_1P_BRAM
  B_t2 local_B_pong[32][1];
  #pragma HLS RESOURCE variable=local_B_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c2, c2_prev;
  /* Variable Declaration */

  {
    for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
      for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
        for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
          // array
          // io_L3
          {
            if (arb == 0) {
              B_IO_L2_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c2, 
                /* array */ local_B_pong, 
                /* fifo */ fifo_B_in, 
                /* enable */ inter_trans_en
              );
              B_IO_L2_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
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
                /* array */ local_B_ping, 
                /* fifo */ fifo_B_in, 
                /* enable */ inter_trans_en
              );
              B_IO_L2_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c2_prev, 
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
          }
        }
    if (arb == 0) {
      B_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c2_prev, 
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
        /* array */ local_B_pong, 
        /* fifo */ fifo_B_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void PE(int idx, int idy, hls::stream<A_t2> &fifo_A_in, hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out, hls::stream<float> &fifo_C_in, hls::stream<float> &fifo_C_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  A_t1 local_A[1][2];
  #pragma HLS ARRAY_PARTITION variable=local_A dim=0 complete
  B_t1 local_B[1][2];
  #pragma HLS ARRAY_PARTITION variable=local_B dim=0 complete
  C_t1 local_C[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_C dim=0 complete
  /* Variable Declaration */

  for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
    for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // pe
        for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
          // latency
          for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
            // latency
            for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
            #pragma HLS PIPELINE II=1
              {
                {
                  A_t2 fifo_data;
                  fifo_data = fifo_A_in.read();
                  for (ap_uint<2> n = 0; n < 2; n++) {
                  #pragma HLS UNROLL
                    union {unsigned int ui; float ut;} u;
                    u.ui = (unsigned int)fifo_data(31, 0);
                    local_A[0][n] = u.ut;
                    fifo_data = fifo_data >> 32;
                  }
                }
                {
                  B_t2 fifo_data;
                  fifo_data = fifo_B_in.read();
                  for (ap_uint<2> n = 0; n < 2; n++) {
                  #pragma HLS UNROLL
                    union {unsigned int ui; float ut;} u;
                    u.ui = (unsigned int)fifo_data(31, 0);
                    local_B[0][n] = u.ut;
                    fifo_data = fifo_data >> 32;
                  }
                }
                if (p1 + 2 * c2 >= 1)
                  local_C[0][0] = fifo_C_in.read();
                // simd
                for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
                #pragma HLS UNROLL
                  local_C[0][0] = (local_C[0][0] + (local_A[0][c8] * local_B[0][c8]));
                }
                fifo_C_out.write(local_C[0][0]);
                {
                  B_t2 fifo_data;
                  union {unsigned int ui; float ut;} u1, u0;
                  u1.ut = local_B[0][1];
                  u0.ut = local_B[0][0];
                  fifo_data = (ap_uint<32>(u1.ui), ap_uint<32>(u0.ui));
                  fifo_B_out.write(fifo_data);
                }
              }
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void PE_wrapper(int idx, int idy, hls::stream<A_t2> &fifo_A_in, hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out, hls::stream<float> &fifo_C_in, hls::stream<float> &fifo_C_out)
 {
  PE(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_B_in, 
    /* fifo */ fifo_B_out, 
    /* fifo */ fifo_C_in, 
    /* fifo */ fifo_C_out);
}
/* Module Definition */

/* Module Definition */
void B_PE_dummy_in(int idx, int idy, hls::stream<B_t2> &fifo_B_in) {
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
    for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
        // array
        // pe
        for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
          // latency
          for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
            // latency
            for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
            #pragma HLS PIPELINE II=1
              B_t2 fifo_data;
              fifo_data = fifo_B_in.read();
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void C_PE_dummy_out(int idx, int idy, hls::stream<float> &fifo_C_out) {
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
    for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1)
      for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1)
        if (p1 + 2 * c2 >= 1) {
          // array
          // pe
          for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
            // latency
            for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
              // latency
              for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
              #pragma HLS PIPELINE II=1
                C_t1 fifo_data = 0;
                fifo_C_out.write(fifo_data);
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_out_intra_trans(int idx, int c0, int c1, C_t16 local_C[16][2], hls::stream<float> &fifo_C_local_in, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[16];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;

  for (ap_uint<5> c0 = 0; c0 < 16; c0++) {
    for (ap_uint<2> c1 = 0; c1 < 2; c1++) {
    #pragma HLS PIPELINE II=1
      local_C[c0][c1] = 0;
    }
  }

  for (ap_uint<5> c2 = 0; c2 <= 15; c2 += 1) {
    // array
    // io_L3
    // io_L2
    // io_L1
    // pe
    for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
      // latency
      for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
        // latency
        for (ap_uint<5> c7 = 0; c7 <= 15; c7 += 1) {
        #pragma HLS PIPELINE II=1
          // simd
          {
            C_t1 in_data;
            C_t16 out_data;
            in_data = fifo_C_local_in.read();
            int split_idx = (16*c5 + c6) % 16;
            out_data = local_C[c7][(16 * c5 + c6) / 16];
            for (ap_uint<5> n = 0; n < 16; n++) {
            #pragma HLS UNROLL
              data_split[n] = out_data(31, 0);
              out_data = out_data >> 32;
            }
            /* Local Reduction */
            union {unsigned int ui; float ut;} uin_0, uout_0;
            uin_0.ut = in_data;
            uout_0.ui = (unsigned int)data_split[split_idx](31, 0);
            uout_0.ut += uin_0.ut;
            data_split[split_idx] = ((ap_uint<32>)uout_0.ui);
            /* Local Reduction */
            out_data = (data_split[15], data_split[14], data_split[13], data_split[12], data_split[11], data_split[10], data_split[9], data_split[8], data_split[7], data_split[6], data_split[5], data_split[4], data_split[3], data_split[2], data_split[1], data_split[0]);            local_C[c7][(16 * c5 + c6) / 16] = out_data;
          }
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_out_inter_trans(int idx, int c0, int c1, C_t16 local_C[16][2], hls::stream<C_t16> &fifo_C_in, hls::stream<C_t16> &fifo_C_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  // array
  // io_L3
  for (ap_uint<2> c3 = p0; c3 <= 1; c3 += 1) {
    // io_L2
    if (c3 == p0) {
      for (ap_uint<5> c4 = 0; c4 <= 15; c4 += 1) {
        // access_coalesce
        for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t16 in_data;
            C_t16 out_data;
            in_data = local_C[c4][c5];
            out_data = in_data;
            fifo_C_out.write(out_data);
          }
        }
      }
    } else {
      for (ap_uint<5> c4 = 0; c4 <= 15; c4 += 1) {
        // access_coalesce
        for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
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
void C_IO_L2_out_inter_trans_boundary(int idx, int c0, int c1, C_t16 local_C[16][2], hls::stream<C_t16> &fifo_C_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  // array
  // io_L3
  for (ap_uint<2> c3 = p0; c3 <= 1; c3 += 1)
    if (c3 == p0) {
      // io_L2
      for (ap_uint<5> c4 = 0; c4 <= 15; c4 += 1) {
        // access_coalesce
        for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t16 in_data;
            C_t16 out_data;
            in_data = local_C[c4][c5];
            out_data = in_data;
            fifo_C_out.write(out_data);
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_out(int idx, hls::stream<C_t16> &fifo_C_in, hls::stream<C_t16> &fifo_C_out, hls::stream<float> &fifo_C_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t16 local_C_ping[16][2];
  #pragma HLS RESOURCE variable=local_C_ping core=RAM_2P_BRAM
  C_t16 local_C_pong[16][2];
  #pragma HLS RESOURCE variable=local_C_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 0;
  bool intra_trans_en = 1;
  int c0, c0_prev;
  int c1, c1_prev;
  /* Variable Declaration */

  {
    for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
      for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1) {
        if (arb == 0) {
          C_IO_L2_out_intra_trans(
            /* module id */ idx, 
            /* host iter */ c0, 
            /* host iter */ c1, 
            /* array */ local_C_ping, 
            /* fifo */ fifo_C_local_in, 
            /* enable */ intra_trans_en
          );
          C_IO_L2_out_inter_trans(
            /* module id */ idx, 
            /* host iter */ c0_prev, 
            /* host iter */ c1_prev, 
            /* array */ local_C_pong, 
            /* fifo */ fifo_C_in, 
            /* fifo */ fifo_C_out, 
            /* enable */ inter_trans_en
          );
        } else {
          C_IO_L2_out_intra_trans(
            /* module id */ idx, 
            /* host iter */ c0, 
            /* host iter */ c1, 
            /* array */ local_C_pong, 
            /* fifo */ fifo_C_local_in, 
            /* enable */ intra_trans_en
          );
          C_IO_L2_out_inter_trans(
            /* module id */ idx, 
            /* host iter */ c0_prev, 
            /* host iter */ c1_prev, 
            /* array */ local_C_ping, 
            /* fifo */ fifo_C_in, 
            /* fifo */ fifo_C_out, 
            /* enable */ inter_trans_en
          );
        }
        inter_trans_en = 1;
        arb = !arb;
        c0_prev = c0;
        c1_prev = c1;
      }
    if (arb == 0) {
      C_IO_L2_out_inter_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* array */ local_C_pong, 
        /* fifo */ fifo_C_in, 
        /* fifo */ fifo_C_out, 
        /* enable */ inter_trans_en
      );
    } else {
      C_IO_L2_out_inter_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* array */ local_C_ping, 
        /* fifo */ fifo_C_in, 
        /* fifo */ fifo_C_out, 
        /* enable */ inter_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_IO_L2_out_boundary(int idx, hls::stream<C_t16> &fifo_C_out, hls::stream<float> &fifo_C_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t16 local_C_ping[16][2];
  #pragma HLS RESOURCE variable=local_C_ping core=RAM_2P_BRAM
  C_t16 local_C_pong[16][2];
  #pragma HLS RESOURCE variable=local_C_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 0;
  bool intra_trans_en = 1;
  int c0, c0_prev;
  int c1, c1_prev;
  /* Variable Declaration */

  {
    for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
      for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1) {
        if (arb == 0) {
          C_IO_L2_out_intra_trans(
            /* module id */ idx, 
            /* host iter */ c0, 
            /* host iter */ c1, 
            /* array */ local_C_ping, 
            /* fifo */ fifo_C_local_in, 
            /* enable */ intra_trans_en
          );
          C_IO_L2_out_inter_trans_boundary(
            /* module id */ idx, 
            /* host iter */ c0_prev, 
            /* host iter */ c1_prev, 
            /* array */ local_C_pong, 
            /* fifo */ fifo_C_out, 
            /* enable */ inter_trans_en
          );
        } else {
          C_IO_L2_out_intra_trans(
            /* module id */ idx, 
            /* host iter */ c0, 
            /* host iter */ c1, 
            /* array */ local_C_pong, 
            /* fifo */ fifo_C_local_in, 
            /* enable */ intra_trans_en
          );
          C_IO_L2_out_inter_trans_boundary(
            /* module id */ idx, 
            /* host iter */ c0_prev, 
            /* host iter */ c1_prev, 
            /* array */ local_C_ping, 
            /* fifo */ fifo_C_out, 
            /* enable */ inter_trans_en
          );
        }
        inter_trans_en = 1;
        arb = !arb;
        c0_prev = c0;
        c1_prev = c1;
      }
    if (arb == 0) {
      C_IO_L2_out_inter_trans_boundary(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* array */ local_C_pong, 
        /* fifo */ fifo_C_out, 
        /* enable */ inter_trans_en
      );
    } else {
      C_IO_L2_out_inter_trans_boundary(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* array */ local_C_ping, 
        /* fifo */ fifo_C_out, 
        /* enable */ inter_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_IO_L3_out(C_t16 *C, hls::stream<C_t16> &fifo_C_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<2> c0 = 0; c0 <= 1; c0 += 1)
    for (ap_uint<2> c1 = 0; c1 <= 1; c1 += 1) {
      // array
      // io_L3
      for (ap_uint<2> c3 = 0; c3 <= 1; c3 += 1) {
        // io_L2
        for (ap_uint<5> c4 = 0; c4 <= 15; c4 += 1) {
          // access_coalesce
          for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
          #pragma HLS PIPELINE II=1
            {
              C_t16 in_data;
              C_t16 out_data;
              in_data = fifo_C_local_in.read();
              out_data = in_data;
              C[128*c0 + 2*c1 + 64*c3 + 4*c4 + c5] = out_data;
            }
          }
        }
      }
    }
}
/* Module Definition */

extern "C" {
void kernel0(A_t2 *A, B_t2 *B, C_t16 *C)
{
#pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem_A
#pragma HLS INTERFACE m_axi port=B offset=slave bundle=gmem_B
#pragma HLS INTERFACE m_axi port=C offset=slave bundle=gmem_C
#pragma HLS INTERFACE s_axilite port=A bundle=control
#pragma HLS INTERFACE s_axilite port=B bundle=control
#pragma HLS INTERFACE s_axilite port=C bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW

  /* FIFO Declaration */
  /* A_IO_L2_in fifo */ hls::stream<A_t2> fifo_A_A_IO_L2_in_0;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_0 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t2> fifo_A_A_IO_L2_in_1;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_1 core=FIFO_SRL
  /* A_IO_L2_in fifo */ hls::stream<A_t2> fifo_A_A_IO_L2_in_2;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L2_in_2 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t2> fifo_A_A_IO_L1_in_0_0;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_0_0 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t2> fifo_A_A_IO_L1_in_0_1;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_0_1 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t2> fifo_A_A_IO_L1_in_0_2;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_0_2 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t2> fifo_A_A_IO_L1_in_1_0;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_1_0 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t2> fifo_A_A_IO_L1_in_1_1;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_1_1 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<A_t2> fifo_A_A_IO_L1_in_1_2;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_1_2 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t2> fifo_B_B_IO_L2_in_0;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_0 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t2> fifo_B_B_IO_L2_in_1;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_1 core=FIFO_SRL
  /* B_IO_L2_in fifo */ hls::stream<B_t2> fifo_B_B_IO_L2_in_2;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_B_IO_L2_in_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_0_0;
  #pragma HLS STREAM variable=fifo_A_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_1_0;
  #pragma HLS STREAM variable=fifo_A_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_0_1;
  #pragma HLS STREAM variable=fifo_A_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<A_t2> fifo_A_PE_1_1;
  #pragma HLS STREAM variable=fifo_A_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_0_0;
  #pragma HLS STREAM variable=fifo_B_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_1_0;
  #pragma HLS STREAM variable=fifo_B_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_2_0;
  #pragma HLS STREAM variable=fifo_B_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_0_1;
  #pragma HLS STREAM variable=fifo_B_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_1_1;
  #pragma HLS STREAM variable=fifo_B_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_2_1;
  #pragma HLS STREAM variable=fifo_B_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_PE_0_0;
  #pragma HLS STREAM variable=fifo_C_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_PE_0_1;
  #pragma HLS STREAM variable=fifo_C_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_PE_0_2;
  #pragma HLS STREAM variable=fifo_C_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_PE_1_0;
  #pragma HLS STREAM variable=fifo_C_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_PE_1_1;
  #pragma HLS STREAM variable=fifo_C_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_PE_1_2;
  #pragma HLS STREAM variable=fifo_C_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_PE_1_2 core=FIFO_SRL
  /* C_IO_L2_out fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_out_0;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_out_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_out_0 core=FIFO_SRL
  /* C_IO_L2_out fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_out_1;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_out_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_out_1 core=FIFO_SRL
  /* C_IO_L2_out fifo */ hls::stream<C_t16> fifo_C_C_IO_L2_out_2;
  #pragma HLS STREAM variable=fifo_C_C_IO_L2_out_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_C_IO_L2_out_2 core=FIFO_SRL
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
    /* fifo */ fifo_A_A_IO_L1_in_0_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L2_in_boundary(
    /* module id */ 1,
    /* fifo */ fifo_A_A_IO_L2_in_1,
    /* fifo */ fifo_A_A_IO_L1_in_1_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_A_A_IO_L1_in_0_0,
    /* fifo */ fifo_A_A_IO_L1_in_0_1,
    /* fifo */ fifo_A_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_boundary_wrapper(
    /* module id */ 0,
    /* module id */ 1,
    /* fifo */ fifo_A_A_IO_L1_in_0_1,
    /* fifo */ fifo_A_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_A_A_IO_L1_in_1_0,
    /* fifo */ fifo_A_A_IO_L1_in_1_1,
    /* fifo */ fifo_A_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_boundary_wrapper(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_A_A_IO_L1_in_1_1,
    /* fifo */ fifo_A_PE_1_1
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
  B_IO_L2_in_boundary(
    /* module id */ 1,
    /* fifo */ fifo_B_B_IO_L2_in_1,
    /* fifo */ fifo_B_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_out(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_C_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  C_PE_dummy_out(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_C_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_0_0,
    /* fifo */ fifo_B_PE_0_0,
    /* fifo */ fifo_B_PE_1_0,
    /* fifo */ fifo_C_PE_0_0,
    /* fifo */ fifo_C_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_0_1,
    /* fifo */ fifo_B_PE_0_1,
    /* fifo */ fifo_B_PE_1_1,
    /* fifo */ fifo_C_PE_0_1,
    /* fifo */ fifo_C_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_A_PE_1_0,
    /* fifo */ fifo_B_PE_1_0,
    /* fifo */ fifo_B_PE_2_0,
    /* fifo */ fifo_C_PE_1_0,
    /* fifo */ fifo_C_PE_1_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_A_PE_1_1,
    /* fifo */ fifo_B_PE_1_1,
    /* fifo */ fifo_B_PE_2_1,
    /* fifo */ fifo_C_PE_1_1,
    /* fifo */ fifo_C_PE_1_2
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_B_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_B_PE_2_1
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_out_boundary(
    /* module id */ 1,
    /* fifo */ fifo_C_C_IO_L2_out_1,
    /* fifo */ fifo_C_PE_1_2
  );
  /* Module Call */

  /* Module Call */
  C_IO_L2_out(
    /* module id */ 0,
    /* fifo */ fifo_C_C_IO_L2_out_1,
    /* fifo */ fifo_C_C_IO_L2_out_0,
    /* fifo */ fifo_C_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  C_IO_L3_out(
    /* array */ C,
    /* fifo */ fifo_C_C_IO_L2_out_0
  );
  /* Module Call */

}
}

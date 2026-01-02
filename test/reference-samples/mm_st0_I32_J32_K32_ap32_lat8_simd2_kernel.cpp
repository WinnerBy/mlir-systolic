#include <ap_int.h>
#include <hls_stream.h>

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Data Type */
typedef float A_t1;
typedef float B_t1;
typedef float C_t1;
typedef ap_uint<512> A_t16;
typedef ap_uint<64> A_t2;
typedef ap_uint<128> A_t4;
typedef ap_uint<512> B_t16;
typedef ap_uint<64> B_t2;
typedef ap_uint<512> C_t16;
typedef ap_uint<128> C_t4;
/* Data Type */

extern "C" {
void kernel0(A_t16 *A, B_t16 *B, C_t16 *C);
}
void A_IO_L1_in_intra_trans(int idx, A_t4 local_A[8][8], hls::stream<A_t2> &fifo_A_local_out, bool intra_trans_en);
void A_IO_L1_in_inter_trans(int idx, A_t4 local_A[8][8], hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, bool inter_trans_en);
void A_IO_L1_in_inter_trans_boundary(int idx, A_t4 local_A[8][8], hls::stream<A_t4> &fifo_A_in, bool inter_trans_en);
void A_IO_L1_in_wrapper(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, hls::stream<A_t2> &fifo_A_local_out);
void A_IO_L1_in_boundary_wrapper(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t2> &fifo_A_local_out);
void B_IO_L2_in_intra_trans(B_t16 local_B[32][2], hls::stream<B_t2> &fifo_B_local_out, bool intra_trans_en);
void B_IO_L2_in_inter_trans_boundary(B_t16 local_B[32][2], hls::stream<B_t16> &fifo_B_in, bool inter_trans_en);
void PE_wrapper(int idx, hls::stream<A_t2> &fifo_A_in, hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out, hls::stream<float> &fifo_C_drain_out);
void C_drain_IO_L1_out_intra_trans(int idx, C_t4 local_C[8][8], hls::stream<float> &fifo_C_drain_local_in);
void C_drain_IO_L1_out_inter_trans(int idx, C_t4 local_C[8][8], hls::stream<C_t4> &fifo_C_drain_in, hls::stream<C_t4> &fifo_C_drain_out);
void C_drain_IO_L1_out_inter_trans_boundary(int idx, C_t4 local_C[8][8], hls::stream<C_t4> &fifo_C_drain_out);
void C_drain_IO_L1_out_wrapper(int idx, hls::stream<C_t4> &fifo_C_drain_in, hls::stream<C_t4> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in);
void C_drain_IO_L1_out_boundary_wrapper(int idx, hls::stream<C_t4> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in);

/* Module Definition */
void A_IO_L2_in(hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L2
  for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
    // io_L1
    for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
      // access_coalesce
      // access_serialize
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
      #pragma HLS PIPELINE II=1
        {
          A_t4 in_data;
          A_t4 out_data;
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
void A_IO_L2_in_serialize(A_t16 *A, hls::stream<A_t4> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  A_t4 fifo_data;
  A_t16 mem_data;
  for (ap_uint<7> i = 0; i < 64; i++) {
  #pragma HLS PIPELINE II=1
    mem_data = A[i];
    for (ap_uint<3> p = 0; p < 4; p++) {
      fifo_data = mem_data(127, 0);
      mem_data = mem_data >> 128;
      fifo_A_local_out.write(fifo_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_intra_trans(int idx, A_t4 local_A[8][8], hls::stream<A_t2> &fifo_A_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t2 data_split[2];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // array
  // io_L2
  // io_L1
  // pe
  for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1)
    for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      // latency
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
        // latency
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
        #pragma HLS PIPELINE II=1
          // simd
          {
            A_t4 in_data;
            A_t2 out_data;
            in_data = local_A[c7][2 * c5 / 4];
            for (ap_uint<2> n = 0; n < 2; n++) {
            #pragma HLS UNROLL
              data_split[n] = in_data(63, 0);
              in_data = in_data >> 64;
            }
            int split_idx = (c5) % 2;
            out_data = data_split[split_idx];
            fifo_A_local_out.write(out_data);
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void A_IO_L1_in_inter_trans(int idx, A_t4 local_A[8][8], hls::stream<A_t4> &fifo_A_in, hls::stream<A_t4> &fifo_A_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1) {
    // io_L1
    if (c3 == p0) {
      for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
        // access_coalesce
        for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t4 in_data;
            A_t4 out_data;
            in_data = fifo_A_in.read();
            out_data = in_data;
            local_A[c4][c5] = out_data;
          }
        }
      }
    } else {
      for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
        // access_coalesce
        for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
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
void A_IO_L1_in_inter_trans_boundary(int idx, A_t4 local_A[8][8], hls::stream<A_t4> &fifo_A_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1)
    if (c3 == p0) {
      // io_L1
      for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
        // access_coalesce
        for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
        #pragma HLS PIPELINE II=1
          {
            A_t4 in_data;
            A_t4 out_data;
            in_data = fifo_A_in.read();
            out_data = in_data;
            local_A[c4][c5] = out_data;
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
  A_t4 local_A_ping[8][8];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_2P_BRAM
  A_t4 local_A_pong[8][8];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_2P_BRAM
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
  A_t4 local_A_ping[8][8];
  #pragma HLS RESOURCE variable=local_A_ping core=RAM_2P_BRAM
  A_t4 local_A_pong[8][8];
  #pragma HLS RESOURCE variable=local_A_pong core=RAM_2P_BRAM
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
void A_IO_L1_in_boundary_wrapper(int idx, hls::stream<A_t4> &fifo_A_in, hls::stream<A_t2> &fifo_A_local_out)
 {
  A_IO_L1_in_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_local_out);
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_intra_trans(B_t16 local_B[32][2], hls::stream<B_t2> &fifo_B_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  B_t2 data_split[8];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1)
    for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      // latency
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
        // latency
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
        #pragma HLS PIPELINE II=1
          // simd
          {
            B_t16 in_data;
            B_t2 out_data;
            in_data = local_B[8*c4 + c6][2 * c5 / 16];
            for (ap_uint<4> n = 0; n < 8; n++) {
            #pragma HLS UNROLL
              data_split[n] = in_data(63, 0);
              in_data = in_data >> 64;
            }
            int split_idx = (c5) % 8;
            out_data = data_split[split_idx];
            fifo_B_local_out.write(out_data);
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_inter_trans_boundary(B_t16 local_B[32][2], hls::stream<B_t16> &fifo_B_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  if (!inter_trans_en) return;

  // io_L2
  for (ap_uint<6> c3 = 0; c3 <= 31; c3 += 1) {
    // access_coalesce
    // access_serialize
    for (ap_uint<2> c4 = 0; c4 <= 1; c4 += 1) {
    #pragma HLS PIPELINE II=1
      {
        B_t16 in_data;
        B_t16 out_data;
        in_data = fifo_B_in.read();
        out_data = in_data;
        local_B[c3][c4] = out_data;
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void B_IO_L2_in_boundary(hls::stream<B_t16> &fifo_B_in, hls::stream<B_t2> &fifo_B_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  B_t16 local_B_ping[32][2];
  #pragma HLS RESOURCE variable=local_B_ping core=RAM_2P_BRAM
  B_t16 local_B_pong[32][2];
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

  for (ap_uint<7> i = 0; i < 64; i++) {
  #pragma HLS PIPELINE II=1
    B_t16 fifo_data;
    fifo_data = B[i];
    fifo_B_local_out.write(fifo_data);
  }
}
/* Module Definition */

/* Module Definition */
void PE(int idx, hls::stream<A_t2> &fifo_A_in, hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out, hls::stream<float> &fifo_C_drain_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  A_t1 local_A[1][2];
  #pragma HLS ARRAY_PARTITION variable=local_A dim=0 complete
  B_t1 local_B[1][2];
  #pragma HLS ARRAY_PARTITION variable=local_B dim=0 complete
  C_t1 local_C[8][32];
  #pragma HLS RESOURCE variable=local_C core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // pe
  for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1)
    for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      // latency
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
        // latency
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
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
            // simd
            for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
            #pragma HLS UNROLL
              local_C[c7][8*c4 + c6] = (local_C[c7][8*c4 + c6] + (local_A[0][c8] * local_B[0][c8]));
            }
            if (c5 == 15)
              fifo_C_drain_out.write(local_C[c7][8*c4 + c6]);
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
/* Module Definition */

/* Module Definition */
void PE_wrapper(int idx, hls::stream<A_t2> &fifo_A_in, hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out, hls::stream<float> &fifo_C_drain_out)
 {
  PE(
    /* module id */ idx, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_B_in, 
    /* fifo */ fifo_B_out, 
    /* fifo */ fifo_C_drain_out);
}
/* Module Definition */

/* Module Definition */
void B_PE_dummy_in(int idx, hls::stream<B_t2> &fifo_B_in) {
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  // array
  // pe
  for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1)
    for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      // latency
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
        // latency
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
        #pragma HLS PIPELINE II=1
          B_t2 fifo_data;
          fifo_data = fifo_B_in.read();
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_intra_trans(int idx, C_t4 local_C[8][8], hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */


  // io_L1
  // pe
  for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1) {
    // latency
    for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1) {
      // latency
      for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
      #pragma HLS PIPELINE II=1
        // simd
        {
          C_t1 in_data;
          C_t4 out_data;
          in_data = fifo_C_drain_local_in.read();
          int split_idx = (8*c4 + c6) % 4;
          out_data = local_C[c7][(8 * c4 + c6) / 4];
          for (ap_uint<3> n = 0; n < 4; n++) {
          #pragma HLS UNROLL
            data_split[n] = out_data(31, 0);
            out_data = out_data >> 32;
          }
          union {unsigned int ui; float ut;} u;
          u.ut = in_data;
          data_split[split_idx] = ap_uint<32>(u.ui);
          out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);          local_C[c7][(8 * c4 + c6) / 4] = out_data;
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_inter_trans(int idx, C_t4 local_C[8][8], hls::stream<C_t4> &fifo_C_drain_in, hls::stream<C_t4> &fifo_C_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1) {
    // io_L1
    if (c3 == p0) {
      for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
        // access_coalesce
        for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t4 in_data;
            C_t4 out_data;
            in_data = local_C[c4][c5];
            out_data = in_data;
            fifo_C_drain_out.write(out_data);
          }
        }
      }
    } else {
      for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
        // access_coalesce
        for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t4 in_data;
            C_t4 out_data;
            in_data = fifo_C_drain_in.read();
            out_data = in_data;
            fifo_C_drain_out.write(out_data);
          }
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_inter_trans_boundary(int idx, C_t4 local_C[8][8], hls::stream<C_t4> &fifo_C_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<3> c3 = p0; c3 <= 3; c3 += 1)
    if (c3 == p0) {
      // io_L1
      for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
        // access_coalesce
        for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
        #pragma HLS PIPELINE II=1
          {
            C_t4 in_data;
            C_t4 out_data;
            in_data = local_C[c4][c5];
            out_data = in_data;
            fifo_C_drain_out.write(out_data);
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out(int idx, hls::stream<C_t4> &fifo_C_drain_in, hls::stream<C_t4> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t4 local_C[8][8];
  #pragma HLS RESOURCE variable=local_C core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  C_drain_IO_L1_out_intra_trans(
    /* module id */ idx, 
    /* array */ local_C, 
    /* fifo */ fifo_C_drain_local_in
  );
  C_drain_IO_L1_out_inter_trans(
    /* module id */ idx, 
    /* array */ local_C, 
    /* fifo */ fifo_C_drain_in, 
    /* fifo */ fifo_C_drain_out
  );
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_wrapper(int idx, hls::stream<C_t4> &fifo_C_drain_in, hls::stream<C_t4> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in)
 {
  C_drain_IO_L1_out(
    /* module id */ idx, 
    /* fifo */ fifo_C_drain_in, 
    /* fifo */ fifo_C_drain_out, 
    /* fifo */ fifo_C_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_boundary(int idx, hls::stream<C_t4> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  C_t4 local_C[8][8];
  #pragma HLS RESOURCE variable=local_C core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  C_drain_IO_L1_out_intra_trans(
    /* module id */ idx, 
    /* array */ local_C, 
    /* fifo */ fifo_C_drain_local_in
  );
  C_drain_IO_L1_out_inter_trans_boundary(
    /* module id */ idx, 
    /* array */ local_C, 
    /* fifo */ fifo_C_drain_out
  );
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L1_out_boundary_wrapper(int idx, hls::stream<C_t4> &fifo_C_drain_out, hls::stream<float> &fifo_C_drain_local_in)
 {
  C_drain_IO_L1_out_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_C_drain_out, 
    /* fifo */ fifo_C_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L2_out(hls::stream<C_t4> &fifo_C_drain_out, hls::stream<C_t4> &fifo_C_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L2
  for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
    // io_L1
    for (ap_uint<4> c4 = 0; c4 <= 7; c4 += 1) {
      // access_coalesce
      // access_serialize
      for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
      #pragma HLS PIPELINE II=1
        {
          C_t4 in_data;
          C_t4 out_data;
          in_data = fifo_C_drain_local_in.read();
          out_data = in_data;
          fifo_C_drain_out.write(out_data);
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void C_drain_IO_L2_out_serialize(C_t16 *C, hls::stream<C_t4> &fifo_C_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<7> i = 0; i < 64; i++) {
  #pragma HLS PIPELINE II=1
    C_t4 fifo_data;
    C_t16 mem_data;
    C_t4 mem_data_split[4];
    #pragma HLS ARRAY_PARTITION variable=mem_data_split complete
    for (ap_uint<3> p = 0; p < 4; p++) {
      fifo_data = fifo_C_drain_local_in.read();
      mem_data_split[p] = fifo_data;
    }
    mem_data = (mem_data_split[3], mem_data_split[2], mem_data_split[1], mem_data_split[0]);
    C[i] = mem_data;
  }
}
/* Module Definition */

extern "C" {
void kernel0(A_t16 *A, B_t16 *B, C_t16 *C)
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
  /* A_IO_L2_in_serialize fifo */ hls::stream<A_t4> fifo_A_A_IO_L2_in_serialize;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_serialize depth=2
  /* B_IO_L2_in_serialize fifo */ hls::stream<B_t16> fifo_B_B_IO_L2_in_serialize;
  #pragma HLS STREAM variable=fifo_B_B_IO_L2_in_serialize depth=2
  /* C_drain_IO_L2_out_serialize fifo */ hls::stream<C_t4> fifo_C_drain_C_drain_IO_L2_out_serialize;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L2_out_serialize depth=2
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
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_0;
  #pragma HLS STREAM variable=fifo_B_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_1;
  #pragma HLS STREAM variable=fifo_B_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_2;
  #pragma HLS STREAM variable=fifo_B_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_3;
  #pragma HLS STREAM variable=fifo_B_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<B_t2> fifo_B_PE_4;
  #pragma HLS STREAM variable=fifo_B_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_B_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_0;
  #pragma HLS STREAM variable=fifo_C_drain_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_1;
  #pragma HLS STREAM variable=fifo_C_drain_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_2;
  #pragma HLS STREAM variable=fifo_C_drain_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_C_drain_PE_3;
  #pragma HLS STREAM variable=fifo_C_drain_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_PE_3 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<C_t4> fifo_C_drain_C_drain_IO_L1_out_0;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_0 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_0 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<C_t4> fifo_C_drain_C_drain_IO_L1_out_1;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_1 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_1 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<C_t4> fifo_C_drain_C_drain_IO_L1_out_2;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_2 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_2 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<C_t4> fifo_C_drain_C_drain_IO_L1_out_3;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_3 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_3 core=FIFO_SRL
  /* C_drain_IO_L1_out fifo */ hls::stream<C_t4> fifo_C_drain_C_drain_IO_L1_out_4;
  #pragma HLS STREAM variable=fifo_C_drain_C_drain_IO_L1_out_4 depth=2
  #pragma HLS RESOURCE variable=fifo_C_drain_C_drain_IO_L1_out_4 core=FIFO_SRL
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
  A_IO_L1_in_boundary_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_A_A_IO_L1_in_3,
    /* fifo */ fifo_A_PE_3
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
  PE_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_A_PE_0,
    /* fifo */ fifo_B_PE_0,
    /* fifo */ fifo_B_PE_1,
    /* fifo */ fifo_C_drain_PE_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_A_PE_1,
    /* fifo */ fifo_B_PE_1,
    /* fifo */ fifo_B_PE_2,
    /* fifo */ fifo_C_drain_PE_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_A_PE_2,
    /* fifo */ fifo_B_PE_2,
    /* fifo */ fifo_B_PE_3,
    /* fifo */ fifo_C_drain_PE_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_A_PE_3,
    /* fifo */ fifo_B_PE_3,
    /* fifo */ fifo_B_PE_4,
    /* fifo */ fifo_C_drain_PE_3
  );
  /* Module Call */

  /* Module Call */
  B_PE_dummy_in(
    /* module id */ 3,
    /* fifo */ fifo_B_PE_4
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3,
    /* fifo */ fifo_C_drain_PE_3
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_3,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2,
    /* fifo */ fifo_C_drain_PE_2
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_2,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1,
    /* fifo */ fifo_C_drain_PE_1
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_1,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0,
    /* fifo */ fifo_C_drain_PE_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L2_out(
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_serialize,
    /* fifo */ fifo_C_drain_C_drain_IO_L1_out_0
  );
  /* Module Call */

  /* Module Call */
  C_drain_IO_L2_out_serialize(
    /* array */ C,
    /* fifo */ fifo_C_drain_C_drain_IO_L2_out_serialize
  );
  /* Module Call */

}
}

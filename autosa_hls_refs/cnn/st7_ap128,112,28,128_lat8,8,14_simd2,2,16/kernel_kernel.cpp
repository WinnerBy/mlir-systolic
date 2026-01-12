// AutoSA Command:
// /workspaces/mlir-systolic/third_party/AutoSA/autosa /workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/cnn/kernel.c --config=/workspaces/mlir-systolic/third_party/AutoSA/autosa_config/autosa_config.json --target=autosa_hls_c --output-dir=/workspaces/mlir-systolic/third_party/AutoSA/autosa.tmp/output --host-serialize --simd-info=/workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/cnn/simd_info.json --sa-sizes={kernel[]->space_time[7];kernel[]->array_part[128,112,28,128];kernel[]->latency[8,8,14];kernel[]->simd[2,2,16]}

#include <ap_int.h>
#include <hls_stream.h>

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Data Type */
typedef float cin_t1;
typedef float cout_t1;
typedef float w_t1;
typedef ap_uint<512> cin_t16;
typedef ap_uint<512> cout_t16;
typedef ap_uint<128> cout_t4;
typedef ap_uint<512> w_t16;
/* Data Type */

extern "C" {
void kernel0(cin_t16 *cin, cout_t16 *cout, w_t16 *w);
}
void cin_IO_L1_in_intra_trans(int idx, int idy, int c0, int c3, cin_t16 local_cin[10][10][8], hls::stream<cin_t16> &fifo_cin_local_out, bool intra_trans_en);
void cin_IO_L1_in_inter_trans(int idx, int idy, int c0, int c3, cin_t16 local_cin[10][10][8], hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_out, bool inter_trans_en);
void cin_IO_L1_in_inter_trans_boundary(int idx, int idy, int c0, int c3, cin_t16 local_cin[10][10][8], hls::stream<cin_t16> &fifo_cin_in, bool inter_trans_en);
void cin_IO_L1_in_wrapper(int idx, int idy, hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_out, hls::stream<cin_t16> &fifo_cin_local_out);
void cin_IO_L1_in_boundary_wrapper(int idx, int idy, hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_local_out);
void w_IO_L2_in_intra_trans(int idx, int c0, int c3, w_t16 local_w[28][3][3][8], hls::stream<w_t16> &fifo_w_local_out, bool intra_trans_en);
void w_IO_L2_in_inter_trans(int idx, int c0, int c3, w_t16 local_w[28][3][3][8], hls::stream<w_t16> &fifo_w_in, hls::stream<w_t16> &fifo_w_out, bool inter_trans_en);
void w_IO_L2_in_inter_trans_boundary(int idx, int c0, int c3, w_t16 local_w[28][3][3][8], hls::stream<w_t16> &fifo_w_in, bool inter_trans_en);
void PE_wrapper(int idx, int idy, hls::stream<cin_t16> &fifo_cin_in, hls::stream<float> &fifo_cout_drain_out, hls::stream<w_t16> &fifo_w_in, hls::stream<w_t16> &fifo_w_out);
void cout_drain_IO_L1_out_intra_trans(int idx, int idy, int c0, cout_t4 local_cout[8][8][7], hls::stream<float> &fifo_cout_drain_local_in);
void cout_drain_IO_L1_out_inter_trans(int idx, int idy, int c0, cout_t4 local_cout[8][8][7], hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out);
void cout_drain_IO_L1_out_inter_trans_boundary(int idx, int idy, int c0, cout_t4 local_cout[8][8][7], hls::stream<cout_t4> &fifo_cout_drain_out);
void cout_drain_IO_L1_out_wrapper(int idx, int idy, hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in);
void cout_drain_IO_L1_out_boundary_wrapper(int idx, int idy, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in);

/* Module Definition */
void cin_IO_L3_in(hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1)
    for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
      // array
      // io_L3
      for (ap_uint<4> c4 = 0; c4 <= 6; c4 += 1) {
        // io_L2
        for (ap_uint<4> c5 = 0; c5 <= 6; c5 += 1) {
          // io_L1
          for (ap_uint<5> c6 = 0; c6 <= 9; c6 += 1)
            for (ap_uint<5> c7 = 0; c7 <= 9; c7 += 1) {
              // access_coalesce
              // access_serialize
              for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
              #pragma HLS PIPELINE II=1
                {
                  cin_t16 in_data;
                  cin_t16 out_data;
                  in_data = fifo_cin_in.read();
                  out_data = in_data;
                  fifo_cin_local_out.write(out_data);
                }
              }
            }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L3_in_serialize(cin_t16 *cin, hls::stream<cin_t16> &fifo_cin_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<23> i = 0; i < 3606400; i++) {
  #pragma HLS PIPELINE II=1
    cin_t16 fifo_data;
    fifo_data = cin[i];
    fifo_cin_local_out.write(fifo_data);
  }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L2_in(int idx, hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_out, hls::stream<cin_t16> &fifo_cin_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1)
    for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
      // array
      // io_L3
      for (ap_uint<4> c4 = p0; c4 <= 6; c4 += 1) {
        // io_L2
        if (c4 == p0) {
          for (ap_uint<4> c5 = 0; c5 <= 6; c5 += 1) {
            // io_L1
            for (ap_uint<5> c6 = 0; c6 <= 9; c6 += 1)
              for (ap_uint<5> c7 = 0; c7 <= 9; c7 += 1) {
                // access_coalesce
                for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
                #pragma HLS PIPELINE II=1
                  {
                    cin_t16 in_data;
                    cin_t16 out_data;
                    in_data = fifo_cin_in.read();
                    out_data = in_data;
                    fifo_cin_local_out.write(out_data);
                  }
                }
              }
          }
        } else {
          for (ap_uint<4> c5 = 0; c5 <= 6; c5 += 1) {
            // io_L1
            for (ap_uint<5> c6 = 0; c6 <= 9; c6 += 1)
              for (ap_uint<5> c7 = 0; c7 <= 9; c7 += 1) {
                // access_coalesce
                for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
                #pragma HLS PIPELINE II=1
                  {
                    cin_t16 in_data;
                    cin_t16 out_data;
                    in_data = fifo_cin_in.read();
                    out_data = in_data;
                    fifo_cin_out.write(out_data);
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
void cin_IO_L2_in_boundary(int idx, hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1)
    for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
      // array
      // io_L3
      for (ap_uint<4> c4 = p0; c4 <= 6; c4 += 1)
        if (c4 == p0) {
          // io_L2
          for (ap_uint<4> c5 = 0; c5 <= 6; c5 += 1) {
            // io_L1
            for (ap_uint<5> c6 = 0; c6 <= 9; c6 += 1)
              for (ap_uint<5> c7 = 0; c7 <= 9; c7 += 1) {
                // access_coalesce
                for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
                #pragma HLS PIPELINE II=1
                  {
                    cin_t16 in_data;
                    cin_t16 out_data;
                    in_data = fifo_cin_in.read();
                    out_data = in_data;
                    fifo_cin_local_out.write(out_data);
                  }
                }
              }
          }
        }
    }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in_intra_trans(int idx, int idy, int c0, int c3, cin_t16 local_cin[10][10][8], hls::stream<cin_t16> &fifo_cin_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L1
  // pe
  for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
    for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
      for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
        for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
          // latency
          for (int c10 = 0; c10 <= min(13, -28 * c0 - 14 * c6 + 639); c10 += 1) {
            // latency
            for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
              // latency
              for (ap_uint<4> c12 = 0; c12 <= 7; c12 += 1) {
              #pragma HLS PIPELINE II=1
                // simd
                {
                  cin_t16 in_data;
                  cin_t16 out_data;
                  in_data = local_cin[c12 + c8][c11 + c9][c7];
                  out_data = in_data;
                  fifo_cin_local_out.write(out_data);
                }
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in_inter_trans(int idx, int idy, int c0, int c3, cin_t16 local_cin[10][10][8], hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c5 = p1; c5 <= 6; c5 += 1) {
    // io_L1
    if (c5 == p1) {
      for (ap_uint<5> c6 = 0; c6 <= 9; c6 += 1)
        for (ap_uint<5> c7 = 0; c7 <= 9; c7 += 1) {
          // access_coalesce
          for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
          #pragma HLS PIPELINE II=1
            {
              cin_t16 in_data;
              cin_t16 out_data;
              in_data = fifo_cin_in.read();
              out_data = in_data;
              local_cin[c6][c7][c8] = out_data;
            }
          }
        }
    } else {
      for (ap_uint<5> c6 = 0; c6 <= 9; c6 += 1)
        for (ap_uint<5> c7 = 0; c7 <= 9; c7 += 1) {
          // access_coalesce
          for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
          #pragma HLS PIPELINE II=1
            {
              cin_t16 in_data;
              cin_t16 out_data;
              in_data = fifo_cin_in.read();
              out_data = in_data;
              fifo_cin_out.write(out_data);
            }
          }
        }
    }
  }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in_inter_trans_boundary(int idx, int idy, int c0, int c3, cin_t16 local_cin[10][10][8], hls::stream<cin_t16> &fifo_cin_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c5 = p1; c5 <= 6; c5 += 1)
    if (c5 == p1) {
      // io_L1
      for (ap_uint<5> c6 = 0; c6 <= 9; c6 += 1)
        for (ap_uint<5> c7 = 0; c7 <= 9; c7 += 1) {
          // access_coalesce
          for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
          #pragma HLS PIPELINE II=1
            {
              cin_t16 in_data;
              cin_t16 out_data;
              in_data = fifo_cin_in.read();
              out_data = in_data;
              local_cin[c6][c7][c8] = out_data;
            }
          }
        }
    }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in(int idx, int idy, hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_out, hls::stream<cin_t16> &fifo_cin_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  cin_t16 local_cin_ping[10][10][8];
  #pragma HLS RESOURCE variable=local_cin_ping core=RAM_1P_BRAM
  cin_t16 local_cin_pong[10][10][8];
  #pragma HLS RESOURCE variable=local_cin_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1)
      for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
        // array
        // io_L3
        // io_L2
        {
          if (arb == 0) {
            cin_IO_L1_in_inter_trans(
              /* module id */ idx, 
              /* module id */ idy, 
              /* host iter */ c0, 
              /* host iter */ c3, 
              /* array */ local_cin_pong, 
              /* fifo */ fifo_cin_in, 
              /* fifo */ fifo_cin_out, 
              /* enable */ inter_trans_en
            );
            cin_IO_L1_in_intra_trans(
              /* module id */ idx, 
              /* module id */ idy, 
              /* host iter */ c0_prev, 
              /* host iter */ c3_prev, 
              /* array */ local_cin_ping, 
              /* fifo */ fifo_cin_local_out, 
              /* enable */ intra_trans_en
            );
          } else {
            cin_IO_L1_in_inter_trans(
              /* module id */ idx, 
              /* module id */ idy, 
              /* host iter */ c0, 
              /* host iter */ c3, 
              /* array */ local_cin_ping, 
              /* fifo */ fifo_cin_in, 
              /* fifo */ fifo_cin_out, 
              /* enable */ inter_trans_en
            );
            cin_IO_L1_in_intra_trans(
              /* module id */ idx, 
              /* module id */ idy, 
              /* host iter */ c0_prev, 
              /* host iter */ c3_prev, 
              /* array */ local_cin_pong, 
              /* fifo */ fifo_cin_local_out, 
              /* enable */ intra_trans_en
            );
          }
          intra_trans_en = 1;
          arb = !arb;
          c0_prev = c0;
          c3_prev = c3;
        }
      }
    if (arb == 0) {
      cin_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* module id */ idy, 
        /* host iter */ c0_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_cin_ping, 
        /* fifo */ fifo_cin_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      cin_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* module id */ idy, 
        /* host iter */ c0_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_cin_pong, 
        /* fifo */ fifo_cin_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in_wrapper(int idx, int idy, hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_out, hls::stream<cin_t16> &fifo_cin_local_out)
 {
  cin_IO_L1_in(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_cin_in, 
    /* fifo */ fifo_cin_out, 
    /* fifo */ fifo_cin_local_out);
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in_boundary(int idx, int idy, hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  cin_t16 local_cin_ping[10][10][8];
  #pragma HLS RESOURCE variable=local_cin_ping core=RAM_1P_BRAM
  cin_t16 local_cin_pong[10][10][8];
  #pragma HLS RESOURCE variable=local_cin_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1)
      for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
        // array
        // io_L3
        // io_L2
        {
          if (arb == 0) {
            cin_IO_L1_in_inter_trans_boundary(
              /* module id */ idx, 
              /* module id */ idy, 
              /* host iter */ c0, 
              /* host iter */ c3, 
              /* array */ local_cin_pong, 
              /* fifo */ fifo_cin_in, 
              /* enable */ inter_trans_en
            );
            cin_IO_L1_in_intra_trans(
              /* module id */ idx, 
              /* module id */ idy, 
              /* host iter */ c0_prev, 
              /* host iter */ c3_prev, 
              /* array */ local_cin_ping, 
              /* fifo */ fifo_cin_local_out, 
              /* enable */ intra_trans_en
            );
          } else {
            cin_IO_L1_in_inter_trans_boundary(
              /* module id */ idx, 
              /* module id */ idy, 
              /* host iter */ c0, 
              /* host iter */ c3, 
              /* array */ local_cin_ping, 
              /* fifo */ fifo_cin_in, 
              /* enable */ inter_trans_en
            );
            cin_IO_L1_in_intra_trans(
              /* module id */ idx, 
              /* module id */ idy, 
              /* host iter */ c0_prev, 
              /* host iter */ c3_prev, 
              /* array */ local_cin_pong, 
              /* fifo */ fifo_cin_local_out, 
              /* enable */ intra_trans_en
            );
          }
          intra_trans_en = 1;
          arb = !arb;
          c0_prev = c0;
          c3_prev = c3;
        }
      }
    if (arb == 0) {
      cin_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* module id */ idy, 
        /* host iter */ c0_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_cin_ping, 
        /* fifo */ fifo_cin_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      cin_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* module id */ idy, 
        /* host iter */ c0_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_cin_pong, 
        /* fifo */ fifo_cin_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in_boundary_wrapper(int idx, int idy, hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t16> &fifo_cin_local_out)
 {
  cin_IO_L1_in_boundary(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_cin_in, 
    /* fifo */ fifo_cin_local_out);
}
/* Module Definition */

/* Module Definition */
void w_IO_L3_in(hls::stream<w_t16> &fifo_w_in, hls::stream<w_t16> &fifo_w_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1)
    for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
      // array
      // io_L3
      for (ap_uint<4> c4 = 0; c4 <= 6; c4 += 1) {
        // io_L2
        for (int c5 = 0; c5 <= min(27, -28 * c0 + 639); c5 += 1)
          for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
            for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
              // access_coalesce
              // access_serialize
              for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
              #pragma HLS PIPELINE II=1
                {
                  w_t16 in_data;
                  w_t16 out_data;
                  in_data = fifo_w_in.read();
                  out_data = in_data;
                  fifo_w_local_out.write(out_data);
                }
              }
            }
      }
    }
}
/* Module Definition */

/* Module Definition */
void w_IO_L3_in_serialize(w_t16 *w, hls::stream<w_t16> &fifo_w_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<22> i = 0; i < 1290240; i++) {
  #pragma HLS PIPELINE II=1
    w_t16 fifo_data;
    fifo_data = w[i];
    fifo_w_local_out.write(fifo_data);
  }
}
/* Module Definition */

/* Module Definition */
void w_IO_L2_in_intra_trans(int idx, int c0, int c3, w_t16 local_w[28][3][3][8], hls::stream<w_t16> &fifo_w_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
    for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
      for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
        for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
          // latency
          for (int c10 = 0; c10 <= min(13, -28 * c0 - 14 * c6 + 639); c10 += 1) {
            // latency
            for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
              // latency
              for (ap_uint<4> c12 = 0; c12 <= 7; c12 += 1) {
              #pragma HLS PIPELINE II=1
                // simd
                {
                  w_t16 in_data;
                  w_t16 out_data;
                  in_data = local_w[c10 + 14*c6][c8][c9][c7];
                  out_data = in_data;
                  fifo_w_local_out.write(out_data);
                }
              }
            }
          }
        }
}
/* Module Definition */

/* Module Definition */
void w_IO_L2_in_inter_trans(int idx, int c0, int c3, w_t16 local_w[28][3][3][8], hls::stream<w_t16> &fifo_w_in, hls::stream<w_t16> &fifo_w_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c4 = p0; c4 <= 6; c4 += 1) {
    // io_L2
    if (c4 == p0) {
      for (int c5 = 0; c5 <= min(27, -28 * c0 + 639); c5 += 1)
        for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
          for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
            // access_coalesce
            for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
            #pragma HLS PIPELINE II=1
              {
                w_t16 in_data;
                w_t16 out_data;
                in_data = fifo_w_in.read();
                out_data = in_data;
                local_w[c5][c6][c7][c8] = out_data;
              }
            }
          }
    } else {
      for (int c5 = 0; c5 <= min(27, -28 * c0 + 639); c5 += 1)
        for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
          for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
            // access_coalesce
            for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
            #pragma HLS PIPELINE II=1
              {
                w_t16 in_data;
                w_t16 out_data;
                in_data = fifo_w_in.read();
                out_data = in_data;
                fifo_w_out.write(out_data);
              }
            }
          }
    }
  }
}
/* Module Definition */

/* Module Definition */
void w_IO_L2_in_inter_trans_boundary(int idx, int c0, int c3, w_t16 local_w[28][3][3][8], hls::stream<w_t16> &fifo_w_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<4> c4 = p0; c4 <= 6; c4 += 1)
    if (c4 == p0) {
      // io_L2
      for (int c5 = 0; c5 <= min(27, -28 * c0 + 639); c5 += 1)
        for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
          for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
            // access_coalesce
            for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
            #pragma HLS PIPELINE II=1
              {
                w_t16 in_data;
                w_t16 out_data;
                in_data = fifo_w_in.read();
                out_data = in_data;
                local_w[c5][c6][c7][c8] = out_data;
              }
            }
          }
    }
}
/* Module Definition */

/* Module Definition */
void w_IO_L2_in(int idx, hls::stream<w_t16> &fifo_w_in, hls::stream<w_t16> &fifo_w_out, hls::stream<w_t16> &fifo_w_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  w_t16 local_w_ping[28][3][3][8];
  #pragma HLS RESOURCE variable=local_w_ping core=RAM_1P_BRAM
  w_t16 local_w_pong[28][3][3][8];
  #pragma HLS RESOURCE variable=local_w_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1)
      for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
        // array
        // io_L3
        {
          if (arb == 0) {
            w_IO_L2_in_inter_trans(
              /* module id */ idx, 
              /* host iter */ c0, 
              /* host iter */ c3, 
              /* array */ local_w_pong, 
              /* fifo */ fifo_w_in, 
              /* fifo */ fifo_w_out, 
              /* enable */ inter_trans_en
            );
            w_IO_L2_in_intra_trans(
              /* module id */ idx, 
              /* host iter */ c0_prev, 
              /* host iter */ c3_prev, 
              /* array */ local_w_ping, 
              /* fifo */ fifo_w_local_out, 
              /* enable */ intra_trans_en
            );
          } else {
            w_IO_L2_in_inter_trans(
              /* module id */ idx, 
              /* host iter */ c0, 
              /* host iter */ c3, 
              /* array */ local_w_ping, 
              /* fifo */ fifo_w_in, 
              /* fifo */ fifo_w_out, 
              /* enable */ inter_trans_en
            );
            w_IO_L2_in_intra_trans(
              /* module id */ idx, 
              /* host iter */ c0_prev, 
              /* host iter */ c3_prev, 
              /* array */ local_w_pong, 
              /* fifo */ fifo_w_local_out, 
              /* enable */ intra_trans_en
            );
          }
          intra_trans_en = 1;
          arb = !arb;
          c0_prev = c0;
          c3_prev = c3;
        }
      }
    if (arb == 0) {
      w_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_w_ping, 
        /* fifo */ fifo_w_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      w_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_w_pong, 
        /* fifo */ fifo_w_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void w_IO_L2_in_boundary(int idx, hls::stream<w_t16> &fifo_w_in, hls::stream<w_t16> &fifo_w_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  w_t16 local_w_ping[28][3][3][8];
  #pragma HLS RESOURCE variable=local_w_ping core=RAM_1P_BRAM
  w_t16 local_w_pong[28][3][3][8];
  #pragma HLS RESOURCE variable=local_w_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1)
      for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
        // array
        // io_L3
        {
          if (arb == 0) {
            w_IO_L2_in_inter_trans_boundary(
              /* module id */ idx, 
              /* host iter */ c0, 
              /* host iter */ c3, 
              /* array */ local_w_pong, 
              /* fifo */ fifo_w_in, 
              /* enable */ inter_trans_en
            );
            w_IO_L2_in_intra_trans(
              /* module id */ idx, 
              /* host iter */ c0_prev, 
              /* host iter */ c3_prev, 
              /* array */ local_w_ping, 
              /* fifo */ fifo_w_local_out, 
              /* enable */ intra_trans_en
            );
          } else {
            w_IO_L2_in_inter_trans_boundary(
              /* module id */ idx, 
              /* host iter */ c0, 
              /* host iter */ c3, 
              /* array */ local_w_ping, 
              /* fifo */ fifo_w_in, 
              /* enable */ inter_trans_en
            );
            w_IO_L2_in_intra_trans(
              /* module id */ idx, 
              /* host iter */ c0_prev, 
              /* host iter */ c3_prev, 
              /* array */ local_w_pong, 
              /* fifo */ fifo_w_local_out, 
              /* enable */ intra_trans_en
            );
          }
          intra_trans_en = 1;
          arb = !arb;
          c0_prev = c0;
          c3_prev = c3;
        }
      }
    if (arb == 0) {
      w_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_w_ping, 
        /* fifo */ fifo_w_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      w_IO_L2_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_w_pong, 
        /* fifo */ fifo_w_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void PE(int idx, int idy, hls::stream<cin_t16> &fifo_cin_in, hls::stream<float> &fifo_cout_drain_out, hls::stream<w_t16> &fifo_w_in, hls::stream<w_t16> &fifo_w_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  cin_t1 local_cin[1][1][16];
  #pragma HLS ARRAY_PARTITION variable=local_cin dim=3 factor=16 cyclic
  #pragma HLS RESOURCE variable=local_cin core=RAM_2P_BRAM
  cout_t1 local_cout[8][8][28];
  #pragma HLS RESOURCE variable=local_cout core=RAM_2P_BRAM
  w_t1 local_w[1][1][1][16];
  #pragma HLS ARRAY_PARTITION variable=local_w dim=4 factor=16 cyclic
  #pragma HLS RESOURCE variable=local_w core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1)
    for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
      // array
      // pe
      for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1) {
        if (c3 == 0) {
          // latency
          for (int c8 = 0; c8 <= min(13, -28 * c0 - 14 * c6 + 639); c8 += 1) {
            // latency
            for (ap_uint<4> c9 = 0; c9 <= 7; c9 += 1) {
              // latency
              for (ap_uint<4> c10 = 0; c10 <= 7; c10 += 1) {
              #pragma HLS PIPELINE II=1
                // simd
                // hls_unroll
                local_cout[c10][c9][14*c6 + c8] = 0;
              }
            }
          }
        }
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
          for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
            for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
              // latency
              for (int c10 = 0; c10 <= min(13, -28 * c0 - 14 * c6 + 639); c10 += 1) {
                // latency
                for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
                  // latency
                  for (ap_uint<4> c12 = 0; c12 <= 7; c12 += 1) {
                  #pragma HLS PIPELINE II=1
                    {
                      {
                        cin_t16 fifo_data;
                        fifo_data = fifo_cin_in.read();
                        for (ap_uint<5> n = 0; n < 16; n++) {
                        #pragma HLS UNROLL
                          union {unsigned int ui; float ut;} u;
                          u.ui = (unsigned int)fifo_data(31, 0);
                          local_cin[0][0][n] = u.ut;
                          fifo_data = fifo_data >> 32;
                        }
                      }
                      {
                        w_t16 fifo_data;
                        fifo_data = fifo_w_in.read();
                        for (ap_uint<5> n = 0; n < 16; n++) {
                        #pragma HLS UNROLL
                          union {unsigned int ui; float ut;} u;
                          u.ui = (unsigned int)fifo_data(31, 0);
                          local_w[0][0][0][n] = u.ut;
                          fifo_data = fifo_data >> 32;
                        }
                      }
                      // simd
                      for (ap_uint<5> c13 = 0; c13 <= 15; c13 += 1) {
                      #pragma HLS UNROLL
                        local_cout[c12][c11][c10 + 14*c6] = (local_cout[c12][c11][c10 + 14*c6] + (local_cin[0][0][c13] * local_w[0][0][0][c13]));
                      }
                      {
                        w_t16 fifo_data;
                        union {unsigned int ui; float ut;} u15, u14, u13, u12, u11, u10, u9, u8, u7, u6, u5, u4, u3, u2, u1, u0;
                        u15.ut = local_w[0][0][0][15];
                        u14.ut = local_w[0][0][0][14];
                        u13.ut = local_w[0][0][0][13];
                        u12.ut = local_w[0][0][0][12];
                        u11.ut = local_w[0][0][0][11];
                        u10.ut = local_w[0][0][0][10];
                        u9.ut = local_w[0][0][0][9];
                        u8.ut = local_w[0][0][0][8];
                        u7.ut = local_w[0][0][0][7];
                        u6.ut = local_w[0][0][0][6];
                        u5.ut = local_w[0][0][0][5];
                        u4.ut = local_w[0][0][0][4];
                        u3.ut = local_w[0][0][0][3];
                        u2.ut = local_w[0][0][0][2];
                        u1.ut = local_w[0][0][0][1];
                        u0.ut = local_w[0][0][0][0];
                        fifo_data = (ap_uint<32>(u15.ui), ap_uint<32>(u14.ui), ap_uint<32>(u13.ui), ap_uint<32>(u12.ui), ap_uint<32>(u11.ui), ap_uint<32>(u10.ui), ap_uint<32>(u9.ui), ap_uint<32>(u8.ui), ap_uint<32>(u7.ui), ap_uint<32>(u6.ui), ap_uint<32>(u5.ui), ap_uint<32>(u4.ui), ap_uint<32>(u3.ui), ap_uint<32>(u2.ui), ap_uint<32>(u1.ui), ap_uint<32>(u0.ui));
                        fifo_w_out.write(fifo_data);
                      }
                      if (c3 == 3 && c7 == 7 && c8 == 2 && c9 == 2)
                        fifo_cout_drain_out.write(local_cout[c12][c11][c10 + 14*c6]);
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
void PE_wrapper(int idx, int idy, hls::stream<cin_t16> &fifo_cin_in, hls::stream<float> &fifo_cout_drain_out, hls::stream<w_t16> &fifo_w_in, hls::stream<w_t16> &fifo_w_out)
 {
  PE(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_cin_in, 
    /* fifo */ fifo_cout_drain_out, 
    /* fifo */ fifo_w_in, 
    /* fifo */ fifo_w_out);
}
/* Module Definition */

/* Module Definition */
void w_PE_dummy_in(int idx, int idy, hls::stream<w_t16> &fifo_w_in) {
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1) {
    // array
    {
    }
    for (ap_uint<3> c3 = 0; c3 <= 3; c3 += 1) {
      // array
      // pe
      for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
          for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
            for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
              // latency
              for (int c10 = 0; c10 <= min(13, -28 * c0 - 14 * c6 + 639); c10 += 1) {
                // latency
                for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
                  // latency
                  for (ap_uint<4> c12 = 0; c12 <= 7; c12 += 1) {
                  #pragma HLS PIPELINE II=1
                    w_t16 fifo_data;
                    fifo_data = fifo_w_in.read();
                  }
                }
              }
            }
    }
  }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_intra_trans(int idx, int idy, int c0, cout_t4 local_cout[8][8][7], hls::stream<float> &fifo_cout_drain_local_in) {
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
    for (int c10 = 0; c10 <= min(13, -28 * c0 - 14 * c6 + 639); c10 += 1) {
      // latency
      for (ap_uint<4> c11 = 0; c11 <= 7; c11 += 1) {
        // latency
        for (ap_uint<4> c12 = 0; c12 <= 7; c12 += 1) {
        #pragma HLS PIPELINE II=1
          // simd
          {
            cout_t1 in_data;
            cout_t4 out_data;
            in_data = fifo_cout_drain_local_in.read();
            int split_idx = (c10 + 14*c6) % 4;
            out_data = local_cout[c12][c11][(14 * c6 + c10) / 4];
            for (ap_uint<3> n = 0; n < 4; n++) {
            #pragma HLS UNROLL
              data_split[n] = out_data(31, 0);
              out_data = out_data >> 32;
            }
            union {unsigned int ui; float ut;} u;
            u.ut = in_data;
            data_split[split_idx] = ap_uint<32>(u.ui);
            out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);            local_cout[c12][c11][(14 * c6 + c10) / 4] = out_data;
          }
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_inter_trans(int idx, int idy, int c0, cout_t4 local_cout[8][8][7], hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<4> c5 = p1; c5 <= 6; c5 += 1) {
    // io_L1
    if (c5 == p1) {
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
          // access_coalesce
          for (int c8 = 0; c8 <= min(6, -7 * c0 + 159); c8 += 1) {
          #pragma HLS PIPELINE II=1
            {
              cout_t4 in_data;
              cout_t4 out_data;
              in_data = local_cout[c6][c7][c8];
              out_data = in_data;
              fifo_cout_drain_out.write(out_data);
            }
          }
        }
    } else {
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
          // access_coalesce
          for (int c8 = 0; c8 <= min(6, -7 * c0 + 159); c8 += 1) {
          #pragma HLS PIPELINE II=1
            {
              cout_t4 in_data;
              cout_t4 out_data;
              in_data = fifo_cout_drain_in.read();
              out_data = in_data;
              fifo_cout_drain_out.write(out_data);
            }
          }
        }
    }
  }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_inter_trans_boundary(int idx, int idy, int c0, cout_t4 local_cout[8][8][7], hls::stream<cout_t4> &fifo_cout_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  /* Variable Declaration */

  for (ap_uint<4> c5 = p1; c5 <= 6; c5 += 1)
    if (c5 == p1) {
      // io_L1
      for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
        for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
          // access_coalesce
          for (int c8 = 0; c8 <= min(6, -7 * c0 + 159); c8 += 1) {
          #pragma HLS PIPELINE II=1
            {
              cout_t4 in_data;
              cout_t4 out_data;
              in_data = local_cout[c6][c7][c8];
              out_data = in_data;
              fifo_cout_drain_out.write(out_data);
            }
          }
        }
    }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out(int idx, int idy, hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  cout_t4 local_cout[8][8][7];
  #pragma HLS RESOURCE variable=local_cout core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1) {
    // array
    // io_L3
    // io_L2
    cout_drain_IO_L1_out_intra_trans(
      /* module id */ idx, 
      /* module id */ idy, 
      /* host iter */ c0, 
      /* array */ local_cout, 
      /* fifo */ fifo_cout_drain_local_in
    );
    cout_drain_IO_L1_out_inter_trans(
      /* module id */ idx, 
      /* module id */ idy, 
      /* host iter */ c0, 
      /* array */ local_cout, 
      /* fifo */ fifo_cout_drain_in, 
      /* fifo */ fifo_cout_drain_out
    );
  }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_wrapper(int idx, int idy, hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in)
 {
  cout_drain_IO_L1_out(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_cout_drain_in, 
    /* fifo */ fifo_cout_drain_out, 
    /* fifo */ fifo_cout_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_boundary(int idx, int idy, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx, p1 = idy; // module id
  cout_t4 local_cout[8][8][7];
  #pragma HLS RESOURCE variable=local_cout core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1) {
    // array
    // io_L3
    // io_L2
    cout_drain_IO_L1_out_intra_trans(
      /* module id */ idx, 
      /* module id */ idy, 
      /* host iter */ c0, 
      /* array */ local_cout, 
      /* fifo */ fifo_cout_drain_local_in
    );
    cout_drain_IO_L1_out_inter_trans_boundary(
      /* module id */ idx, 
      /* module id */ idy, 
      /* host iter */ c0, 
      /* array */ local_cout, 
      /* fifo */ fifo_cout_drain_out
    );
  }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_boundary_wrapper(int idx, int idy, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in)
 {
  cout_drain_IO_L1_out_boundary(
    /* module id */ idx, 
    /* module id */ idy, 
    /* fifo */ fifo_cout_drain_out, 
    /* fifo */ fifo_cout_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L2_out(int idx, hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<cout_t4> &fifo_cout_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1) {
    // array
    // io_L3
    for (ap_uint<4> c4 = p0; c4 <= 6; c4 += 1) {
      // io_L2
      if (c4 == p0) {
        for (ap_uint<4> c5 = 0; c5 <= 6; c5 += 1) {
          // io_L1
          for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
            for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
              // access_coalesce
              for (int c8 = 0; c8 <= min(6, -7 * c0 + 159); c8 += 1) {
              #pragma HLS PIPELINE II=1
                {
                  cout_t4 in_data;
                  cout_t4 out_data;
                  in_data = fifo_cout_drain_local_in.read();
                  out_data = in_data;
                  fifo_cout_drain_out.write(out_data);
                }
              }
            }
        }
      } else {
        for (ap_uint<4> c5 = 0; c5 <= 6; c5 += 1) {
          // io_L1
          for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
            for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
              // access_coalesce
              for (int c8 = 0; c8 <= min(6, -7 * c0 + 159); c8 += 1) {
              #pragma HLS PIPELINE II=1
                {
                  cout_t4 in_data;
                  cout_t4 out_data;
                  in_data = fifo_cout_drain_in.read();
                  out_data = in_data;
                  fifo_cout_drain_out.write(out_data);
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
void cout_drain_IO_L2_out_boundary(int idx, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<cout_t4> &fifo_cout_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1) {
    // array
    // io_L3
    for (ap_uint<4> c4 = p0; c4 <= 6; c4 += 1)
      if (c4 == p0) {
        // io_L2
        for (ap_uint<4> c5 = 0; c5 <= 6; c5 += 1) {
          // io_L1
          for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
            for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
              // access_coalesce
              for (int c8 = 0; c8 <= min(6, -7 * c0 + 159); c8 += 1) {
              #pragma HLS PIPELINE II=1
                {
                  cout_t4 in_data;
                  cout_t4 out_data;
                  in_data = fifo_cout_drain_local_in.read();
                  out_data = in_data;
                  fifo_cout_drain_out.write(out_data);
                }
              }
            }
        }
      }
  }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L3_out(hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<cout_t4> &fifo_cout_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<6> c0 = 0; c0 <= 22; c0 += 1) {
    // array
    // io_L3
    for (ap_uint<4> c4 = 0; c4 <= 6; c4 += 1) {
      // io_L2
      for (ap_uint<4> c5 = 0; c5 <= 6; c5 += 1) {
        // io_L1
        for (ap_uint<4> c6 = 0; c6 <= 7; c6 += 1)
          for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
            // access_coalesce
            // access_serialize
            for (int c8 = 0; c8 <= min(6, -7 * c0 + 159); c8 += 1) {
            #pragma HLS PIPELINE II=1
              {
                cout_t4 in_data;
                cout_t4 out_data;
                in_data = fifo_cout_drain_local_in.read();
                out_data = in_data;
                fifo_cout_drain_out.write(out_data);
              }
            }
          }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L3_out_serialize(cout_t16 *cout, hls::stream<cout_t4> &fifo_cout_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<18> i = 0; i < 125440; i++) {
  #pragma HLS PIPELINE II=1
    cout_t4 fifo_data;
    cout_t16 mem_data;
    cout_t4 mem_data_split[4];
    #pragma HLS ARRAY_PARTITION variable=mem_data_split complete
    for (ap_uint<3> p = 0; p < 4; p++) {
      fifo_data = fifo_cout_drain_local_in.read();
      mem_data_split[p] = fifo_data;
    }
    mem_data = (mem_data_split[3], mem_data_split[2], mem_data_split[1], mem_data_split[0]);
    cout[i] = mem_data;
  }
}
/* Module Definition */

extern "C" {
void kernel0(cin_t16 *cin, cout_t16 *cout, w_t16 *w)
{
#pragma HLS INTERFACE m_axi port=cin offset=slave bundle=gmem_cin
#pragma HLS INTERFACE m_axi port=cout offset=slave bundle=gmem_cout
#pragma HLS INTERFACE m_axi port=w offset=slave bundle=gmem_w
#pragma HLS INTERFACE s_axilite port=cin bundle=control
#pragma HLS INTERFACE s_axilite port=cout bundle=control
#pragma HLS INTERFACE s_axilite port=w bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW

  /* FIFO Declaration */
  /* cin_IO_L3_in_serialize fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L3_in_serialize;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L3_in_serialize depth=2
  /* w_IO_L3_in_serialize fifo */ hls::stream<w_t16> fifo_w_w_IO_L3_in_serialize;
  #pragma HLS STREAM variable=fifo_w_w_IO_L3_in_serialize depth=2
  /* cout_drain_IO_L3_out_serialize fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L3_out_serialize;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L3_out_serialize depth=2
  /* cin_IO_L2_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L2_in_0;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L2_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L2_in_0 core=FIFO_SRL
  /* cin_IO_L2_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L2_in_1;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L2_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L2_in_1 core=FIFO_SRL
  /* cin_IO_L2_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L2_in_2;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L2_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L2_in_2 core=FIFO_SRL
  /* cin_IO_L2_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L2_in_3;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L2_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L2_in_3 core=FIFO_SRL
  /* cin_IO_L2_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L2_in_4;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L2_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L2_in_4 core=FIFO_SRL
  /* cin_IO_L2_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L2_in_5;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L2_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L2_in_5 core=FIFO_SRL
  /* cin_IO_L2_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L2_in_6;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L2_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L2_in_6 core=FIFO_SRL
  /* cin_IO_L2_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L2_in_7;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L2_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L2_in_7 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_0_0;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_0_0 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_0_1;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_0_1 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_0_2;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_0_2 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_0_3;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_0_3 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_0_4;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_0_4 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_0_5;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_0_5 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_0_6;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_0_6 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_0_7;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_0_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_0_7 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_1_0;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_1_0 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_1_1;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_1_1 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_1_2;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_1_2 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_1_3;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_1_3 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_1_4;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_1_4 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_1_5;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_1_5 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_1_6;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_1_6 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_1_7;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_1_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_1_7 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_2_0;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_2_0 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_2_1;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_2_1 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_2_2;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_2_2 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_2_3;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_2_3 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_2_4;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_2_4 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_2_5;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_2_5 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_2_6;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_2_6 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_2_7;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_2_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_2_7 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_3_0;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_3_0 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_3_1;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_3_1 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_3_2;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_3_2 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_3_3;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_3_3 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_3_4;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_3_4 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_3_5;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_3_5 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_3_6;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_3_6 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_3_7;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_3_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_3_7 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_4_0;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_4_0 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_4_1;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_4_1 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_4_2;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_4_2 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_4_3;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_4_3 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_4_4;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_4_4 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_4_5;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_4_5 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_4_6;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_4_6 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_4_7;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_4_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_4_7 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_5_0;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_5_0 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_5_1;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_5_1 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_5_2;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_5_2 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_5_3;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_5_3 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_5_4;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_5_4 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_5_5;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_5_5 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_5_6;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_5_6 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_5_7;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_5_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_5_7 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_6_0;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_6_0 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_6_1;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_6_1 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_6_2;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_6_2 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_6_3;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_6_3 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_6_4;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_6_4 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_6_5;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_6_5 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_6_6;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_6_6 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L1_in_6_7;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_6_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_6_7 core=FIFO_SRL
  /* w_IO_L2_in fifo */ hls::stream<w_t16> fifo_w_w_IO_L2_in_0;
  #pragma HLS STREAM variable=fifo_w_w_IO_L2_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L2_in_0 core=FIFO_SRL
  /* w_IO_L2_in fifo */ hls::stream<w_t16> fifo_w_w_IO_L2_in_1;
  #pragma HLS STREAM variable=fifo_w_w_IO_L2_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L2_in_1 core=FIFO_SRL
  /* w_IO_L2_in fifo */ hls::stream<w_t16> fifo_w_w_IO_L2_in_2;
  #pragma HLS STREAM variable=fifo_w_w_IO_L2_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L2_in_2 core=FIFO_SRL
  /* w_IO_L2_in fifo */ hls::stream<w_t16> fifo_w_w_IO_L2_in_3;
  #pragma HLS STREAM variable=fifo_w_w_IO_L2_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L2_in_3 core=FIFO_SRL
  /* w_IO_L2_in fifo */ hls::stream<w_t16> fifo_w_w_IO_L2_in_4;
  #pragma HLS STREAM variable=fifo_w_w_IO_L2_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L2_in_4 core=FIFO_SRL
  /* w_IO_L2_in fifo */ hls::stream<w_t16> fifo_w_w_IO_L2_in_5;
  #pragma HLS STREAM variable=fifo_w_w_IO_L2_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L2_in_5 core=FIFO_SRL
  /* w_IO_L2_in fifo */ hls::stream<w_t16> fifo_w_w_IO_L2_in_6;
  #pragma HLS STREAM variable=fifo_w_w_IO_L2_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L2_in_6 core=FIFO_SRL
  /* w_IO_L2_in fifo */ hls::stream<w_t16> fifo_w_w_IO_L2_in_7;
  #pragma HLS STREAM variable=fifo_w_w_IO_L2_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L2_in_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_0_0;
  #pragma HLS STREAM variable=fifo_cin_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_1_0;
  #pragma HLS STREAM variable=fifo_cin_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_2_0;
  #pragma HLS STREAM variable=fifo_cin_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_3_0;
  #pragma HLS STREAM variable=fifo_cin_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_4_0;
  #pragma HLS STREAM variable=fifo_cin_PE_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_4_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_5_0;
  #pragma HLS STREAM variable=fifo_cin_PE_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_5_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_6_0;
  #pragma HLS STREAM variable=fifo_cin_PE_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_6_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_0_1;
  #pragma HLS STREAM variable=fifo_cin_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_1_1;
  #pragma HLS STREAM variable=fifo_cin_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_2_1;
  #pragma HLS STREAM variable=fifo_cin_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_3_1;
  #pragma HLS STREAM variable=fifo_cin_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_4_1;
  #pragma HLS STREAM variable=fifo_cin_PE_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_4_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_5_1;
  #pragma HLS STREAM variable=fifo_cin_PE_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_5_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_6_1;
  #pragma HLS STREAM variable=fifo_cin_PE_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_6_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_0_2;
  #pragma HLS STREAM variable=fifo_cin_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_1_2;
  #pragma HLS STREAM variable=fifo_cin_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_2_2;
  #pragma HLS STREAM variable=fifo_cin_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_3_2;
  #pragma HLS STREAM variable=fifo_cin_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_4_2;
  #pragma HLS STREAM variable=fifo_cin_PE_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_4_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_5_2;
  #pragma HLS STREAM variable=fifo_cin_PE_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_5_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_6_2;
  #pragma HLS STREAM variable=fifo_cin_PE_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_6_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_0_3;
  #pragma HLS STREAM variable=fifo_cin_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_1_3;
  #pragma HLS STREAM variable=fifo_cin_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_2_3;
  #pragma HLS STREAM variable=fifo_cin_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_3_3;
  #pragma HLS STREAM variable=fifo_cin_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_3_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_4_3;
  #pragma HLS STREAM variable=fifo_cin_PE_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_4_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_5_3;
  #pragma HLS STREAM variable=fifo_cin_PE_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_5_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_6_3;
  #pragma HLS STREAM variable=fifo_cin_PE_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_6_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_0_4;
  #pragma HLS STREAM variable=fifo_cin_PE_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_0_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_1_4;
  #pragma HLS STREAM variable=fifo_cin_PE_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_1_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_2_4;
  #pragma HLS STREAM variable=fifo_cin_PE_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_2_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_3_4;
  #pragma HLS STREAM variable=fifo_cin_PE_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_3_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_4_4;
  #pragma HLS STREAM variable=fifo_cin_PE_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_4_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_5_4;
  #pragma HLS STREAM variable=fifo_cin_PE_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_5_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_6_4;
  #pragma HLS STREAM variable=fifo_cin_PE_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_6_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_0_5;
  #pragma HLS STREAM variable=fifo_cin_PE_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_0_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_1_5;
  #pragma HLS STREAM variable=fifo_cin_PE_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_1_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_2_5;
  #pragma HLS STREAM variable=fifo_cin_PE_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_2_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_3_5;
  #pragma HLS STREAM variable=fifo_cin_PE_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_3_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_4_5;
  #pragma HLS STREAM variable=fifo_cin_PE_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_4_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_5_5;
  #pragma HLS STREAM variable=fifo_cin_PE_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_5_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_6_5;
  #pragma HLS STREAM variable=fifo_cin_PE_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_6_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_0_6;
  #pragma HLS STREAM variable=fifo_cin_PE_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_0_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_1_6;
  #pragma HLS STREAM variable=fifo_cin_PE_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_1_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_2_6;
  #pragma HLS STREAM variable=fifo_cin_PE_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_2_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_3_6;
  #pragma HLS STREAM variable=fifo_cin_PE_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_3_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_4_6;
  #pragma HLS STREAM variable=fifo_cin_PE_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_4_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_5_6;
  #pragma HLS STREAM variable=fifo_cin_PE_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_5_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t16> fifo_cin_PE_6_6;
  #pragma HLS STREAM variable=fifo_cin_PE_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_6_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_0_0;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_1_0;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_2_0;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_3_0;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_4_0;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_4_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_5_0;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_5_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_6_0;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_6_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_0_1;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_1_1;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_2_1;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_3_1;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_4_1;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_4_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_5_1;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_5_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_6_1;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_6_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_0_2;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_1_2;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_2_2;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_3_2;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_4_2;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_4_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_5_2;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_5_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_6_2;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_6_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_0_3;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_1_3;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_2_3;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_3_3;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_3_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_4_3;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_4_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_5_3;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_5_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_6_3;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_6_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_0_4;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_0_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_1_4;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_1_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_2_4;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_2_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_3_4;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_3_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_4_4;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_4_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_5_4;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_5_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_6_4;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_6_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_0_5;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_0_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_1_5;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_1_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_2_5;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_2_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_3_5;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_3_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_4_5;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_4_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_5_5;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_5_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_6_5;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_6_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_0_6;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_0_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_1_6;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_1_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_2_6;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_2_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_3_6;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_3_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_4_6;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_4_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_5_6;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_5_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_6_6;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_6_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_0_0;
  #pragma HLS STREAM variable=fifo_w_PE_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_0_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_1_0;
  #pragma HLS STREAM variable=fifo_w_PE_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_1_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_2_0;
  #pragma HLS STREAM variable=fifo_w_PE_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_2_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_3_0;
  #pragma HLS STREAM variable=fifo_w_PE_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_3_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_4_0;
  #pragma HLS STREAM variable=fifo_w_PE_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_4_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_5_0;
  #pragma HLS STREAM variable=fifo_w_PE_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_5_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_6_0;
  #pragma HLS STREAM variable=fifo_w_PE_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_6_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_7_0;
  #pragma HLS STREAM variable=fifo_w_PE_7_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_7_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_0_1;
  #pragma HLS STREAM variable=fifo_w_PE_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_0_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_1_1;
  #pragma HLS STREAM variable=fifo_w_PE_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_1_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_2_1;
  #pragma HLS STREAM variable=fifo_w_PE_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_2_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_3_1;
  #pragma HLS STREAM variable=fifo_w_PE_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_3_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_4_1;
  #pragma HLS STREAM variable=fifo_w_PE_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_4_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_5_1;
  #pragma HLS STREAM variable=fifo_w_PE_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_5_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_6_1;
  #pragma HLS STREAM variable=fifo_w_PE_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_6_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_7_1;
  #pragma HLS STREAM variable=fifo_w_PE_7_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_7_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_0_2;
  #pragma HLS STREAM variable=fifo_w_PE_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_0_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_1_2;
  #pragma HLS STREAM variable=fifo_w_PE_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_1_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_2_2;
  #pragma HLS STREAM variable=fifo_w_PE_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_2_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_3_2;
  #pragma HLS STREAM variable=fifo_w_PE_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_3_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_4_2;
  #pragma HLS STREAM variable=fifo_w_PE_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_4_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_5_2;
  #pragma HLS STREAM variable=fifo_w_PE_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_5_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_6_2;
  #pragma HLS STREAM variable=fifo_w_PE_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_6_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_7_2;
  #pragma HLS STREAM variable=fifo_w_PE_7_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_7_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_0_3;
  #pragma HLS STREAM variable=fifo_w_PE_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_0_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_1_3;
  #pragma HLS STREAM variable=fifo_w_PE_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_1_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_2_3;
  #pragma HLS STREAM variable=fifo_w_PE_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_2_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_3_3;
  #pragma HLS STREAM variable=fifo_w_PE_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_3_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_4_3;
  #pragma HLS STREAM variable=fifo_w_PE_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_4_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_5_3;
  #pragma HLS STREAM variable=fifo_w_PE_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_5_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_6_3;
  #pragma HLS STREAM variable=fifo_w_PE_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_6_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_7_3;
  #pragma HLS STREAM variable=fifo_w_PE_7_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_7_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_0_4;
  #pragma HLS STREAM variable=fifo_w_PE_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_0_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_1_4;
  #pragma HLS STREAM variable=fifo_w_PE_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_1_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_2_4;
  #pragma HLS STREAM variable=fifo_w_PE_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_2_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_3_4;
  #pragma HLS STREAM variable=fifo_w_PE_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_3_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_4_4;
  #pragma HLS STREAM variable=fifo_w_PE_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_4_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_5_4;
  #pragma HLS STREAM variable=fifo_w_PE_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_5_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_6_4;
  #pragma HLS STREAM variable=fifo_w_PE_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_6_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_7_4;
  #pragma HLS STREAM variable=fifo_w_PE_7_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_7_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_0_5;
  #pragma HLS STREAM variable=fifo_w_PE_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_0_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_1_5;
  #pragma HLS STREAM variable=fifo_w_PE_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_1_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_2_5;
  #pragma HLS STREAM variable=fifo_w_PE_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_2_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_3_5;
  #pragma HLS STREAM variable=fifo_w_PE_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_3_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_4_5;
  #pragma HLS STREAM variable=fifo_w_PE_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_4_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_5_5;
  #pragma HLS STREAM variable=fifo_w_PE_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_5_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_6_5;
  #pragma HLS STREAM variable=fifo_w_PE_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_6_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_7_5;
  #pragma HLS STREAM variable=fifo_w_PE_7_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_7_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_0_6;
  #pragma HLS STREAM variable=fifo_w_PE_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_0_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_1_6;
  #pragma HLS STREAM variable=fifo_w_PE_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_1_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_2_6;
  #pragma HLS STREAM variable=fifo_w_PE_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_2_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_3_6;
  #pragma HLS STREAM variable=fifo_w_PE_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_3_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_4_6;
  #pragma HLS STREAM variable=fifo_w_PE_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_4_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_5_6;
  #pragma HLS STREAM variable=fifo_w_PE_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_5_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_6_6;
  #pragma HLS STREAM variable=fifo_w_PE_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_6_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t16> fifo_w_PE_7_6;
  #pragma HLS STREAM variable=fifo_w_PE_7_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_7_6 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_0_0;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_0_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_0_0 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_0_1;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_0_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_0_1 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_0_2;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_0_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_0_2 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_0_3;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_0_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_0_3 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_0_4;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_0_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_0_4 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_0_5;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_0_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_0_5 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_0_6;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_0_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_0_6 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_0_7;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_0_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_0_7 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_1_0;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_1_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_1_0 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_1_1;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_1_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_1_1 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_1_2;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_1_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_1_2 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_1_3;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_1_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_1_3 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_1_4;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_1_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_1_4 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_1_5;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_1_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_1_5 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_1_6;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_1_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_1_6 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_1_7;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_1_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_1_7 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_2_0;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_2_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_2_0 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_2_1;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_2_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_2_1 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_2_2;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_2_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_2_2 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_2_3;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_2_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_2_3 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_2_4;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_2_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_2_4 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_2_5;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_2_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_2_5 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_2_6;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_2_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_2_6 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_2_7;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_2_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_2_7 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_3_0;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_3_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_3_0 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_3_1;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_3_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_3_1 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_3_2;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_3_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_3_2 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_3_3;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_3_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_3_3 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_3_4;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_3_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_3_4 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_3_5;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_3_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_3_5 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_3_6;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_3_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_3_6 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_3_7;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_3_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_3_7 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_4_0;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_4_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_4_0 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_4_1;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_4_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_4_1 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_4_2;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_4_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_4_2 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_4_3;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_4_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_4_3 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_4_4;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_4_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_4_4 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_4_5;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_4_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_4_5 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_4_6;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_4_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_4_6 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_4_7;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_4_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_4_7 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_5_0;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_5_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_5_0 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_5_1;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_5_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_5_1 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_5_2;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_5_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_5_2 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_5_3;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_5_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_5_3 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_5_4;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_5_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_5_4 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_5_5;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_5_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_5_5 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_5_6;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_5_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_5_6 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_5_7;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_5_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_5_7 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_6_0;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_6_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_6_0 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_6_1;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_6_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_6_1 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_6_2;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_6_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_6_2 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_6_3;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_6_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_6_3 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_6_4;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_6_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_6_4 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_6_5;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_6_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_6_5 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_6_6;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_6_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_6_6 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_6_7;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_6_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_6_7 core=FIFO_SRL
  /* cout_drain_IO_L2_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L2_out_0;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L2_out_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L2_out_0 core=FIFO_SRL
  /* cout_drain_IO_L2_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L2_out_1;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L2_out_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L2_out_1 core=FIFO_SRL
  /* cout_drain_IO_L2_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L2_out_2;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L2_out_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L2_out_2 core=FIFO_SRL
  /* cout_drain_IO_L2_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L2_out_3;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L2_out_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L2_out_3 core=FIFO_SRL
  /* cout_drain_IO_L2_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L2_out_4;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L2_out_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L2_out_4 core=FIFO_SRL
  /* cout_drain_IO_L2_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L2_out_5;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L2_out_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L2_out_5 core=FIFO_SRL
  /* cout_drain_IO_L2_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L2_out_6;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L2_out_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L2_out_6 core=FIFO_SRL
  /* cout_drain_IO_L2_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L2_out_7;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L2_out_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L2_out_7 core=FIFO_SRL
  /* FIFO Declaration */

  /* Module Call */
  cin_IO_L3_in_serialize(
    /* array */ cin,
    /* fifo */ fifo_cin_cin_IO_L3_in_serialize
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L3_in(
    /* fifo */ fifo_cin_cin_IO_L3_in_serialize,
    /* fifo */ fifo_cin_cin_IO_L2_in_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L2_in(
    /* module id */ 0,
    /* fifo */ fifo_cin_cin_IO_L2_in_0,
    /* fifo */ fifo_cin_cin_IO_L2_in_1,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L2_in(
    /* module id */ 1,
    /* fifo */ fifo_cin_cin_IO_L2_in_1,
    /* fifo */ fifo_cin_cin_IO_L2_in_2,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L2_in(
    /* module id */ 2,
    /* fifo */ fifo_cin_cin_IO_L2_in_2,
    /* fifo */ fifo_cin_cin_IO_L2_in_3,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L2_in(
    /* module id */ 3,
    /* fifo */ fifo_cin_cin_IO_L2_in_3,
    /* fifo */ fifo_cin_cin_IO_L2_in_4,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L2_in(
    /* module id */ 4,
    /* fifo */ fifo_cin_cin_IO_L2_in_4,
    /* fifo */ fifo_cin_cin_IO_L2_in_5,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L2_in(
    /* module id */ 5,
    /* fifo */ fifo_cin_cin_IO_L2_in_5,
    /* fifo */ fifo_cin_cin_IO_L2_in_6,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L2_in_boundary(
    /* module id */ 6,
    /* fifo */ fifo_cin_cin_IO_L2_in_6,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_0,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_1,
    /* fifo */ fifo_cin_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 0,
    /* module id */ 1,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_1,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_2,
    /* fifo */ fifo_cin_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 0,
    /* module id */ 2,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_2,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_3,
    /* fifo */ fifo_cin_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 0,
    /* module id */ 3,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_3,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_4,
    /* fifo */ fifo_cin_PE_3_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 0,
    /* module id */ 4,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_4,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_5,
    /* fifo */ fifo_cin_PE_4_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 0,
    /* module id */ 5,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_5,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_6,
    /* fifo */ fifo_cin_PE_5_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_boundary_wrapper(
    /* module id */ 0,
    /* module id */ 6,
    /* fifo */ fifo_cin_cin_IO_L1_in_0_6,
    /* fifo */ fifo_cin_PE_6_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_0,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_1,
    /* fifo */ fifo_cin_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_1,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_2,
    /* fifo */ fifo_cin_PE_1_1
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 1,
    /* module id */ 2,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_2,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_3,
    /* fifo */ fifo_cin_PE_2_1
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 1,
    /* module id */ 3,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_3,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_4,
    /* fifo */ fifo_cin_PE_3_1
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 1,
    /* module id */ 4,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_4,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_5,
    /* fifo */ fifo_cin_PE_4_1
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 1,
    /* module id */ 5,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_5,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_6,
    /* fifo */ fifo_cin_PE_5_1
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_boundary_wrapper(
    /* module id */ 1,
    /* module id */ 6,
    /* fifo */ fifo_cin_cin_IO_L1_in_1_6,
    /* fifo */ fifo_cin_PE_6_1
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 2,
    /* module id */ 0,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_0,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_1,
    /* fifo */ fifo_cin_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 2,
    /* module id */ 1,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_1,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_2,
    /* fifo */ fifo_cin_PE_1_2
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 2,
    /* module id */ 2,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_2,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_3,
    /* fifo */ fifo_cin_PE_2_2
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 2,
    /* module id */ 3,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_3,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_4,
    /* fifo */ fifo_cin_PE_3_2
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 2,
    /* module id */ 4,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_4,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_5,
    /* fifo */ fifo_cin_PE_4_2
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 2,
    /* module id */ 5,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_5,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_6,
    /* fifo */ fifo_cin_PE_5_2
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_boundary_wrapper(
    /* module id */ 2,
    /* module id */ 6,
    /* fifo */ fifo_cin_cin_IO_L1_in_2_6,
    /* fifo */ fifo_cin_PE_6_2
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 3,
    /* module id */ 0,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_0,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_1,
    /* fifo */ fifo_cin_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 3,
    /* module id */ 1,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_1,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_2,
    /* fifo */ fifo_cin_PE_1_3
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 3,
    /* module id */ 2,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_2,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_3,
    /* fifo */ fifo_cin_PE_2_3
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 3,
    /* module id */ 3,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_3,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_4,
    /* fifo */ fifo_cin_PE_3_3
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 3,
    /* module id */ 4,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_4,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_5,
    /* fifo */ fifo_cin_PE_4_3
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 3,
    /* module id */ 5,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_5,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_6,
    /* fifo */ fifo_cin_PE_5_3
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_boundary_wrapper(
    /* module id */ 3,
    /* module id */ 6,
    /* fifo */ fifo_cin_cin_IO_L1_in_3_6,
    /* fifo */ fifo_cin_PE_6_3
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 4,
    /* module id */ 0,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_0,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_1,
    /* fifo */ fifo_cin_PE_0_4
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 4,
    /* module id */ 1,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_1,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_2,
    /* fifo */ fifo_cin_PE_1_4
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 4,
    /* module id */ 2,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_2,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_3,
    /* fifo */ fifo_cin_PE_2_4
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 4,
    /* module id */ 3,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_3,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_4,
    /* fifo */ fifo_cin_PE_3_4
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 4,
    /* module id */ 4,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_4,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_5,
    /* fifo */ fifo_cin_PE_4_4
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 4,
    /* module id */ 5,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_5,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_6,
    /* fifo */ fifo_cin_PE_5_4
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_boundary_wrapper(
    /* module id */ 4,
    /* module id */ 6,
    /* fifo */ fifo_cin_cin_IO_L1_in_4_6,
    /* fifo */ fifo_cin_PE_6_4
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 5,
    /* module id */ 0,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_0,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_1,
    /* fifo */ fifo_cin_PE_0_5
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 5,
    /* module id */ 1,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_1,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_2,
    /* fifo */ fifo_cin_PE_1_5
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 5,
    /* module id */ 2,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_2,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_3,
    /* fifo */ fifo_cin_PE_2_5
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 5,
    /* module id */ 3,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_3,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_4,
    /* fifo */ fifo_cin_PE_3_5
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 5,
    /* module id */ 4,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_4,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_5,
    /* fifo */ fifo_cin_PE_4_5
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 5,
    /* module id */ 5,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_5,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_6,
    /* fifo */ fifo_cin_PE_5_5
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_boundary_wrapper(
    /* module id */ 5,
    /* module id */ 6,
    /* fifo */ fifo_cin_cin_IO_L1_in_5_6,
    /* fifo */ fifo_cin_PE_6_5
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 6,
    /* module id */ 0,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_0,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_1,
    /* fifo */ fifo_cin_PE_0_6
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 6,
    /* module id */ 1,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_1,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_2,
    /* fifo */ fifo_cin_PE_1_6
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 6,
    /* module id */ 2,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_2,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_3,
    /* fifo */ fifo_cin_PE_2_6
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 6,
    /* module id */ 3,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_3,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_4,
    /* fifo */ fifo_cin_PE_3_6
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 6,
    /* module id */ 4,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_4,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_5,
    /* fifo */ fifo_cin_PE_4_6
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 6,
    /* module id */ 5,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_5,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_6,
    /* fifo */ fifo_cin_PE_5_6
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_boundary_wrapper(
    /* module id */ 6,
    /* module id */ 6,
    /* fifo */ fifo_cin_cin_IO_L1_in_6_6,
    /* fifo */ fifo_cin_PE_6_6
  );
  /* Module Call */

  /* Module Call */
  w_IO_L3_in_serialize(
    /* array */ w,
    /* fifo */ fifo_w_w_IO_L3_in_serialize
  );
  /* Module Call */

  /* Module Call */
  w_IO_L3_in(
    /* fifo */ fifo_w_w_IO_L3_in_serialize,
    /* fifo */ fifo_w_w_IO_L2_in_0
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in(
    /* module id */ 0,
    /* fifo */ fifo_w_w_IO_L2_in_0,
    /* fifo */ fifo_w_w_IO_L2_in_1,
    /* fifo */ fifo_w_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in(
    /* module id */ 1,
    /* fifo */ fifo_w_w_IO_L2_in_1,
    /* fifo */ fifo_w_w_IO_L2_in_2,
    /* fifo */ fifo_w_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in(
    /* module id */ 2,
    /* fifo */ fifo_w_w_IO_L2_in_2,
    /* fifo */ fifo_w_w_IO_L2_in_3,
    /* fifo */ fifo_w_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in(
    /* module id */ 3,
    /* fifo */ fifo_w_w_IO_L2_in_3,
    /* fifo */ fifo_w_w_IO_L2_in_4,
    /* fifo */ fifo_w_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in(
    /* module id */ 4,
    /* fifo */ fifo_w_w_IO_L2_in_4,
    /* fifo */ fifo_w_w_IO_L2_in_5,
    /* fifo */ fifo_w_PE_0_4
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in(
    /* module id */ 5,
    /* fifo */ fifo_w_w_IO_L2_in_5,
    /* fifo */ fifo_w_w_IO_L2_in_6,
    /* fifo */ fifo_w_PE_0_5
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in_boundary(
    /* module id */ 6,
    /* fifo */ fifo_w_w_IO_L2_in_6,
    /* fifo */ fifo_w_PE_0_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_cin_PE_0_0,
    /* fifo */ fifo_cout_drain_PE_0_0,
    /* fifo */ fifo_w_PE_0_0,
    /* fifo */ fifo_w_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 1,
    /* fifo */ fifo_cin_PE_0_1,
    /* fifo */ fifo_cout_drain_PE_0_1,
    /* fifo */ fifo_w_PE_0_1,
    /* fifo */ fifo_w_PE_1_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 2,
    /* fifo */ fifo_cin_PE_0_2,
    /* fifo */ fifo_cout_drain_PE_0_2,
    /* fifo */ fifo_w_PE_0_2,
    /* fifo */ fifo_w_PE_1_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 3,
    /* fifo */ fifo_cin_PE_0_3,
    /* fifo */ fifo_cout_drain_PE_0_3,
    /* fifo */ fifo_w_PE_0_3,
    /* fifo */ fifo_w_PE_1_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 4,
    /* fifo */ fifo_cin_PE_0_4,
    /* fifo */ fifo_cout_drain_PE_0_4,
    /* fifo */ fifo_w_PE_0_4,
    /* fifo */ fifo_w_PE_1_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 5,
    /* fifo */ fifo_cin_PE_0_5,
    /* fifo */ fifo_cout_drain_PE_0_5,
    /* fifo */ fifo_w_PE_0_5,
    /* fifo */ fifo_w_PE_1_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* module id */ 6,
    /* fifo */ fifo_cin_PE_0_6,
    /* fifo */ fifo_cout_drain_PE_0_6,
    /* fifo */ fifo_w_PE_0_6,
    /* fifo */ fifo_w_PE_1_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_cin_PE_1_0,
    /* fifo */ fifo_cout_drain_PE_1_0,
    /* fifo */ fifo_w_PE_1_0,
    /* fifo */ fifo_w_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_cin_PE_1_1,
    /* fifo */ fifo_cout_drain_PE_1_1,
    /* fifo */ fifo_w_PE_1_1,
    /* fifo */ fifo_w_PE_2_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 2,
    /* fifo */ fifo_cin_PE_1_2,
    /* fifo */ fifo_cout_drain_PE_1_2,
    /* fifo */ fifo_w_PE_1_2,
    /* fifo */ fifo_w_PE_2_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 3,
    /* fifo */ fifo_cin_PE_1_3,
    /* fifo */ fifo_cout_drain_PE_1_3,
    /* fifo */ fifo_w_PE_1_3,
    /* fifo */ fifo_w_PE_2_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 4,
    /* fifo */ fifo_cin_PE_1_4,
    /* fifo */ fifo_cout_drain_PE_1_4,
    /* fifo */ fifo_w_PE_1_4,
    /* fifo */ fifo_w_PE_2_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 5,
    /* fifo */ fifo_cin_PE_1_5,
    /* fifo */ fifo_cout_drain_PE_1_5,
    /* fifo */ fifo_w_PE_1_5,
    /* fifo */ fifo_w_PE_2_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* module id */ 6,
    /* fifo */ fifo_cin_PE_1_6,
    /* fifo */ fifo_cout_drain_PE_1_6,
    /* fifo */ fifo_w_PE_1_6,
    /* fifo */ fifo_w_PE_2_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 0,
    /* fifo */ fifo_cin_PE_2_0,
    /* fifo */ fifo_cout_drain_PE_2_0,
    /* fifo */ fifo_w_PE_2_0,
    /* fifo */ fifo_w_PE_3_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 1,
    /* fifo */ fifo_cin_PE_2_1,
    /* fifo */ fifo_cout_drain_PE_2_1,
    /* fifo */ fifo_w_PE_2_1,
    /* fifo */ fifo_w_PE_3_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 2,
    /* fifo */ fifo_cin_PE_2_2,
    /* fifo */ fifo_cout_drain_PE_2_2,
    /* fifo */ fifo_w_PE_2_2,
    /* fifo */ fifo_w_PE_3_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 3,
    /* fifo */ fifo_cin_PE_2_3,
    /* fifo */ fifo_cout_drain_PE_2_3,
    /* fifo */ fifo_w_PE_2_3,
    /* fifo */ fifo_w_PE_3_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 4,
    /* fifo */ fifo_cin_PE_2_4,
    /* fifo */ fifo_cout_drain_PE_2_4,
    /* fifo */ fifo_w_PE_2_4,
    /* fifo */ fifo_w_PE_3_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 5,
    /* fifo */ fifo_cin_PE_2_5,
    /* fifo */ fifo_cout_drain_PE_2_5,
    /* fifo */ fifo_w_PE_2_5,
    /* fifo */ fifo_w_PE_3_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* module id */ 6,
    /* fifo */ fifo_cin_PE_2_6,
    /* fifo */ fifo_cout_drain_PE_2_6,
    /* fifo */ fifo_w_PE_2_6,
    /* fifo */ fifo_w_PE_3_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 0,
    /* fifo */ fifo_cin_PE_3_0,
    /* fifo */ fifo_cout_drain_PE_3_0,
    /* fifo */ fifo_w_PE_3_0,
    /* fifo */ fifo_w_PE_4_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 1,
    /* fifo */ fifo_cin_PE_3_1,
    /* fifo */ fifo_cout_drain_PE_3_1,
    /* fifo */ fifo_w_PE_3_1,
    /* fifo */ fifo_w_PE_4_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 2,
    /* fifo */ fifo_cin_PE_3_2,
    /* fifo */ fifo_cout_drain_PE_3_2,
    /* fifo */ fifo_w_PE_3_2,
    /* fifo */ fifo_w_PE_4_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 3,
    /* fifo */ fifo_cin_PE_3_3,
    /* fifo */ fifo_cout_drain_PE_3_3,
    /* fifo */ fifo_w_PE_3_3,
    /* fifo */ fifo_w_PE_4_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 4,
    /* fifo */ fifo_cin_PE_3_4,
    /* fifo */ fifo_cout_drain_PE_3_4,
    /* fifo */ fifo_w_PE_3_4,
    /* fifo */ fifo_w_PE_4_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 5,
    /* fifo */ fifo_cin_PE_3_5,
    /* fifo */ fifo_cout_drain_PE_3_5,
    /* fifo */ fifo_w_PE_3_5,
    /* fifo */ fifo_w_PE_4_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* module id */ 6,
    /* fifo */ fifo_cin_PE_3_6,
    /* fifo */ fifo_cout_drain_PE_3_6,
    /* fifo */ fifo_w_PE_3_6,
    /* fifo */ fifo_w_PE_4_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 0,
    /* fifo */ fifo_cin_PE_4_0,
    /* fifo */ fifo_cout_drain_PE_4_0,
    /* fifo */ fifo_w_PE_4_0,
    /* fifo */ fifo_w_PE_5_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 1,
    /* fifo */ fifo_cin_PE_4_1,
    /* fifo */ fifo_cout_drain_PE_4_1,
    /* fifo */ fifo_w_PE_4_1,
    /* fifo */ fifo_w_PE_5_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 2,
    /* fifo */ fifo_cin_PE_4_2,
    /* fifo */ fifo_cout_drain_PE_4_2,
    /* fifo */ fifo_w_PE_4_2,
    /* fifo */ fifo_w_PE_5_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 3,
    /* fifo */ fifo_cin_PE_4_3,
    /* fifo */ fifo_cout_drain_PE_4_3,
    /* fifo */ fifo_w_PE_4_3,
    /* fifo */ fifo_w_PE_5_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 4,
    /* fifo */ fifo_cin_PE_4_4,
    /* fifo */ fifo_cout_drain_PE_4_4,
    /* fifo */ fifo_w_PE_4_4,
    /* fifo */ fifo_w_PE_5_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 5,
    /* fifo */ fifo_cin_PE_4_5,
    /* fifo */ fifo_cout_drain_PE_4_5,
    /* fifo */ fifo_w_PE_4_5,
    /* fifo */ fifo_w_PE_5_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* module id */ 6,
    /* fifo */ fifo_cin_PE_4_6,
    /* fifo */ fifo_cout_drain_PE_4_6,
    /* fifo */ fifo_w_PE_4_6,
    /* fifo */ fifo_w_PE_5_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 0,
    /* fifo */ fifo_cin_PE_5_0,
    /* fifo */ fifo_cout_drain_PE_5_0,
    /* fifo */ fifo_w_PE_5_0,
    /* fifo */ fifo_w_PE_6_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 1,
    /* fifo */ fifo_cin_PE_5_1,
    /* fifo */ fifo_cout_drain_PE_5_1,
    /* fifo */ fifo_w_PE_5_1,
    /* fifo */ fifo_w_PE_6_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 2,
    /* fifo */ fifo_cin_PE_5_2,
    /* fifo */ fifo_cout_drain_PE_5_2,
    /* fifo */ fifo_w_PE_5_2,
    /* fifo */ fifo_w_PE_6_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 3,
    /* fifo */ fifo_cin_PE_5_3,
    /* fifo */ fifo_cout_drain_PE_5_3,
    /* fifo */ fifo_w_PE_5_3,
    /* fifo */ fifo_w_PE_6_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 4,
    /* fifo */ fifo_cin_PE_5_4,
    /* fifo */ fifo_cout_drain_PE_5_4,
    /* fifo */ fifo_w_PE_5_4,
    /* fifo */ fifo_w_PE_6_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 5,
    /* fifo */ fifo_cin_PE_5_5,
    /* fifo */ fifo_cout_drain_PE_5_5,
    /* fifo */ fifo_w_PE_5_5,
    /* fifo */ fifo_w_PE_6_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* module id */ 6,
    /* fifo */ fifo_cin_PE_5_6,
    /* fifo */ fifo_cout_drain_PE_5_6,
    /* fifo */ fifo_w_PE_5_6,
    /* fifo */ fifo_w_PE_6_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 0,
    /* fifo */ fifo_cin_PE_6_0,
    /* fifo */ fifo_cout_drain_PE_6_0,
    /* fifo */ fifo_w_PE_6_0,
    /* fifo */ fifo_w_PE_7_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 1,
    /* fifo */ fifo_cin_PE_6_1,
    /* fifo */ fifo_cout_drain_PE_6_1,
    /* fifo */ fifo_w_PE_6_1,
    /* fifo */ fifo_w_PE_7_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 2,
    /* fifo */ fifo_cin_PE_6_2,
    /* fifo */ fifo_cout_drain_PE_6_2,
    /* fifo */ fifo_w_PE_6_2,
    /* fifo */ fifo_w_PE_7_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 3,
    /* fifo */ fifo_cin_PE_6_3,
    /* fifo */ fifo_cout_drain_PE_6_3,
    /* fifo */ fifo_w_PE_6_3,
    /* fifo */ fifo_w_PE_7_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 4,
    /* fifo */ fifo_cin_PE_6_4,
    /* fifo */ fifo_cout_drain_PE_6_4,
    /* fifo */ fifo_w_PE_6_4,
    /* fifo */ fifo_w_PE_7_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 5,
    /* fifo */ fifo_cin_PE_6_5,
    /* fifo */ fifo_cout_drain_PE_6_5,
    /* fifo */ fifo_w_PE_6_5,
    /* fifo */ fifo_w_PE_7_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* module id */ 6,
    /* fifo */ fifo_cin_PE_6_6,
    /* fifo */ fifo_cout_drain_PE_6_6,
    /* fifo */ fifo_w_PE_6_6,
    /* fifo */ fifo_w_PE_7_6
  );
  /* Module Call */

  /* Module Call */
  w_PE_dummy_in(
    /* module id */ 6,
    /* module id */ 0,
    /* fifo */ fifo_w_PE_7_0
  );
  /* Module Call */

  /* Module Call */
  w_PE_dummy_in(
    /* module id */ 6,
    /* module id */ 1,
    /* fifo */ fifo_w_PE_7_1
  );
  /* Module Call */

  /* Module Call */
  w_PE_dummy_in(
    /* module id */ 6,
    /* module id */ 2,
    /* fifo */ fifo_w_PE_7_2
  );
  /* Module Call */

  /* Module Call */
  w_PE_dummy_in(
    /* module id */ 6,
    /* module id */ 3,
    /* fifo */ fifo_w_PE_7_3
  );
  /* Module Call */

  /* Module Call */
  w_PE_dummy_in(
    /* module id */ 6,
    /* module id */ 4,
    /* fifo */ fifo_w_PE_7_4
  );
  /* Module Call */

  /* Module Call */
  w_PE_dummy_in(
    /* module id */ 6,
    /* module id */ 5,
    /* fifo */ fifo_w_PE_7_5
  );
  /* Module Call */

  /* Module Call */
  w_PE_dummy_in(
    /* module id */ 6,
    /* module id */ 6,
    /* fifo */ fifo_w_PE_7_6
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 0,
    /* module id */ 6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_6,
    /* fifo */ fifo_cout_drain_PE_6_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_5,
    /* fifo */ fifo_cout_drain_PE_5_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_4,
    /* fifo */ fifo_cout_drain_PE_4_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_3,
    /* fifo */ fifo_cout_drain_PE_3_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_2,
    /* fifo */ fifo_cout_drain_PE_2_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_1,
    /* fifo */ fifo_cout_drain_PE_1_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* module id */ 0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_0,
    /* fifo */ fifo_cout_drain_PE_0_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 1,
    /* module id */ 6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_6,
    /* fifo */ fifo_cout_drain_PE_6_1
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_5,
    /* fifo */ fifo_cout_drain_PE_5_1
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_4,
    /* fifo */ fifo_cout_drain_PE_4_1
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_3,
    /* fifo */ fifo_cout_drain_PE_3_1
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_2,
    /* fifo */ fifo_cout_drain_PE_2_1
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_1,
    /* fifo */ fifo_cout_drain_PE_1_1
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* module id */ 0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_0,
    /* fifo */ fifo_cout_drain_PE_0_1
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 2,
    /* module id */ 6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_6,
    /* fifo */ fifo_cout_drain_PE_6_2
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_5,
    /* fifo */ fifo_cout_drain_PE_5_2
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_4,
    /* fifo */ fifo_cout_drain_PE_4_2
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_3,
    /* fifo */ fifo_cout_drain_PE_3_2
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_2,
    /* fifo */ fifo_cout_drain_PE_2_2
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_1,
    /* fifo */ fifo_cout_drain_PE_1_2
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* module id */ 0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_0,
    /* fifo */ fifo_cout_drain_PE_0_2
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 3,
    /* module id */ 6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_6,
    /* fifo */ fifo_cout_drain_PE_6_3
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_5,
    /* fifo */ fifo_cout_drain_PE_5_3
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_4,
    /* fifo */ fifo_cout_drain_PE_4_3
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_3,
    /* fifo */ fifo_cout_drain_PE_3_3
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_2,
    /* fifo */ fifo_cout_drain_PE_2_3
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_1,
    /* fifo */ fifo_cout_drain_PE_1_3
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* module id */ 0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_0,
    /* fifo */ fifo_cout_drain_PE_0_3
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 4,
    /* module id */ 6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_6,
    /* fifo */ fifo_cout_drain_PE_6_4
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_5,
    /* fifo */ fifo_cout_drain_PE_5_4
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_4,
    /* fifo */ fifo_cout_drain_PE_4_4
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_3,
    /* fifo */ fifo_cout_drain_PE_3_4
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_2,
    /* fifo */ fifo_cout_drain_PE_2_4
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_1,
    /* fifo */ fifo_cout_drain_PE_1_4
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* module id */ 0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_0,
    /* fifo */ fifo_cout_drain_PE_0_4
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 5,
    /* module id */ 6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_6,
    /* fifo */ fifo_cout_drain_PE_6_5
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_5,
    /* fifo */ fifo_cout_drain_PE_5_5
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_4,
    /* fifo */ fifo_cout_drain_PE_4_5
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_3,
    /* fifo */ fifo_cout_drain_PE_3_5
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_2,
    /* fifo */ fifo_cout_drain_PE_2_5
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_1,
    /* fifo */ fifo_cout_drain_PE_1_5
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* module id */ 0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_0,
    /* fifo */ fifo_cout_drain_PE_0_5
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 6,
    /* module id */ 6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_6,
    /* fifo */ fifo_cout_drain_PE_6_6
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_5,
    /* fifo */ fifo_cout_drain_PE_5_6
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_4,
    /* fifo */ fifo_cout_drain_PE_4_6
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_3,
    /* fifo */ fifo_cout_drain_PE_3_6
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_2,
    /* fifo */ fifo_cout_drain_PE_2_6
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_1,
    /* fifo */ fifo_cout_drain_PE_1_6
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* module id */ 0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_0,
    /* fifo */ fifo_cout_drain_PE_0_6
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L2_out_boundary(
    /* module id */ 6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L2_out(
    /* module id */ 5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L2_out(
    /* module id */ 4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L2_out(
    /* module id */ 3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L2_out(
    /* module id */ 2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L2_out(
    /* module id */ 1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L2_out(
    /* module id */ 0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L3_out(
    /* fifo */ fifo_cout_drain_cout_drain_IO_L3_out_serialize,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L3_out_serialize(
    /* array */ cout,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L3_out_serialize
  );
  /* Module Call */

}
}

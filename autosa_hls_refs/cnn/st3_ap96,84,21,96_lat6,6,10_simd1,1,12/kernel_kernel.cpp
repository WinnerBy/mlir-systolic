// AutoSA Command:
// /workspaces/mlir-systolic/third_party/AutoSA/autosa /workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/cnn/kernel.c --config=/workspaces/mlir-systolic/third_party/AutoSA/autosa_config/autosa_config.json --target=autosa_hls_c --output-dir=/workspaces/mlir-systolic/third_party/AutoSA/autosa.tmp/output --host-serialize --simd-info=/workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/cnn/simd_info.json --sa-sizes={kernel[]->space_time[3];kernel[]->array_part[96,84,21,96];kernel[]->latency[6,6,10];kernel[]->simd[1,1,12]}

#include <ap_int.h>
#include <hls_stream.h>

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Data Type */
typedef float cin_t1;
typedef float cout_t1;
typedef float w_t1;
typedef ap_uint<128> cout_t4;
/* Data Type */

extern "C" {
void kernel0(cin_t1 *cin, cout_t4 *cout_0, cout_t4 *cout_1, cout_t4 *cout_2, w_t1 *w);
}
void cin_IO_L1_in_intra_trans(int idx, int c0, int c1, int c3, float local_cin[23][58][1], hls::stream<float> &fifo_cin_local_out, bool intra_trans_en);
void cin_IO_L1_in_inter_trans(int idx, int c0, int c1, int c3, float local_cin[23][58][1], hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cin_out, bool inter_trans_en);
void cin_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c3, float local_cin[23][58][1], hls::stream<float> &fifo_cin_in, bool inter_trans_en);
void cin_IO_L1_in_wrapper(int idx, hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cin_out, hls::stream<float> &fifo_cin_local_out);
void cin_IO_L1_in_boundary_wrapper(int idx, hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cin_local_out);
void cout_1_IO_L2_in_intra_trans(int c0, int c1, int c3, cout_t4 *cout, cout_t4 local_cout_1[21][56][21], hls::stream<float> &fifo_cout_1_local_out, bool intra_trans_en);
void w_IO_L1_in_intra_trans(int idx, int c0, int c1, int c3, float local_w[84][3][3][1], hls::stream<float> &fifo_w_local_out, bool intra_trans_en);
void w_IO_L1_in_inter_trans(int idx, int c0, int c1, int c3, float local_w[84][3][3][1], hls::stream<float> &fifo_w_in, hls::stream<float> &fifo_w_out, bool inter_trans_en);
void w_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c3, float local_w[84][3][3][1], hls::stream<float> &fifo_w_in, bool inter_trans_en);
void w_IO_L1_in_wrapper(int idx, hls::stream<float> &fifo_w_in, hls::stream<float> &fifo_w_out, hls::stream<float> &fifo_w_local_out);
void w_IO_L1_in_boundary_wrapper(int idx, hls::stream<float> &fifo_w_in, hls::stream<float> &fifo_w_local_out);
void PE_wrapper(int idx, hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cout_1_in, hls::stream<float> &fifo_cout_1_out, hls::stream<float> &fifo_cout_drain_out, hls::stream<float> &fifo_w_in);
void cout_1_IO_L2_out_intra_trans(int c0, int c1, int c3, cout_t4 *cout, cout_t4 local_cout_1[21][56][21], hls::stream<float> &fifo_cout_1_local_in, bool intra_trans_en);
void cout_1_IO_L2_out_inter_trans_boundary(int c0, int c1, cout_t4 *cout, cout_t4 local_cout_1[21][56][21], hls::stream<cout_t4> &fifo_cout_1_out, bool inter_trans_en);
void cout_drain_IO_L1_out_intra_trans(int idx, int c0, int c1, cout_t4 local_cout[21][56][21], hls::stream<float> &fifo_cout_drain_local_in);
void cout_drain_IO_L1_out_inter_trans(int idx, int c0, int c1, cout_t4 local_cout[21][56][21], hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out);
void cout_drain_IO_L1_out_inter_trans_boundary(int idx, int c0, int c1, cout_t4 local_cout[21][56][21], hls::stream<cout_t4> &fifo_cout_drain_out);
void cout_drain_IO_L1_out_wrapper(int idx, hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in);
void cout_drain_IO_L1_out_boundary_wrapper(int idx, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in);

/* Module Definition */
void cin_IO_L2_in(cin_t1 *cin, hls::stream<float> &fifo_cin_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1)
      for (ap_uint<4> c3 = 0; c3 <= 5; c3 += 1) {
        // array
        // io_L2
        for (int c4 = 0; c4 <= min(95, -96 * c3 + 511); c4 += 1) {
          // io_L1
          for (int c5 = 0; c5 <= min(22, -21 * c1 + 57); c5 += 1)
            for (ap_uint<7> c6 = 0; c6 <= 57; c6 += 1) {
            #pragma HLS PIPELINE II=1
              // access_coalesce
              {
                cin_t1 in_data;
                cin_t1 out_data;
                in_data = cin[623616*c1 + 96*c3 + c4 + 29696*c5 + 512*c6];
                out_data = in_data;
                fifo_cin_local_out.write(out_data);
              }
            }
        }
      }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in_intra_trans(int idx, int c0, int c1, int c3, float local_cin[23][58][1], hls::stream<float> &fifo_cin_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L1
  // pe
  for (int c5 = 0; c5 <= min(13, -14 * c0 + 106); c5 += 1)
    for (int c6 = 0; c6 <= min(3, -4 * c1 + c1 / 2 + 9); c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 5; c7 += 1)
        for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
          for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
            // latency
            for (int c10 = 0; c10 <= min(9, -10 * c7 + 55); c10 += 1) {
              // latency
              for (int c11 = 0; c11 <= min(min(5, -6 * c6 + 20), -21 * c1 - 6 * c6 + 55); c11 += 1) {
                // latency
                for (int c12 = 0; c12 <= min(5, -84 * c0 - 6 * c5 + 639); c12 += 1) {
                #pragma HLS PIPELINE II=1
                  {
                    cin_t1 in_data;
                    cin_t1 out_data;
                    in_data = local_cin[c11 + 6*c6 + c8][c10 + 10*c7 + c9][0];
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
void cin_IO_L1_in_inter_trans(int idx, int c0, int c1, int c3, float local_cin[23][58][1], hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cin_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (int c4 = p0; c4 <= min(95, -96 * c3 + 511); c4 += 1) {
    // io_L1
    if (c4 == p0) {
      for (int c5 = 0; c5 <= min(22, -21 * c1 + 57); c5 += 1)
        for (ap_uint<7> c6 = 0; c6 <= 57; c6 += 1) {
        #pragma HLS PIPELINE II=1
          // access_coalesce
          {
            cin_t1 in_data;
            cin_t1 out_data;
            in_data = fifo_cin_in.read();
            out_data = in_data;
            local_cin[c5][c6][0] = out_data;
          }
        }
    } else {
      for (int c5 = 0; c5 <= min(22, -21 * c1 + 57); c5 += 1)
        for (ap_uint<7> c6 = 0; c6 <= 57; c6 += 1) {
        #pragma HLS PIPELINE II=1
          // access_coalesce
          {
            cin_t1 in_data;
            cin_t1 out_data;
            in_data = fifo_cin_in.read();
            out_data = in_data;
            fifo_cin_out.write(out_data);
          }
        }
    }
  }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c3, float local_cin[23][58][1], hls::stream<float> &fifo_cin_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (int c4 = p0; c4 <= min(95, -96 * c3 + 511); c4 += 1)
    if (c4 == p0) {
      // io_L1
      for (int c5 = 0; c5 <= min(22, -21 * c1 + 57); c5 += 1)
        for (ap_uint<7> c6 = 0; c6 <= 57; c6 += 1) {
        #pragma HLS PIPELINE II=1
          // access_coalesce
          {
            cin_t1 in_data;
            cin_t1 out_data;
            in_data = fifo_cin_in.read();
            out_data = in_data;
            local_cin[c5][c6][0] = out_data;
          }
        }
    }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in(int idx, hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cin_out, hls::stream<float> &fifo_cin_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  cin_t1 local_cin_ping[23][58][1];
  #pragma HLS RESOURCE variable=local_cin_ping core=RAM_1P_BRAM
  float local_cin_pong[23][58][1];
  #pragma HLS RESOURCE variable=local_cin_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 5; c3 += 1) {
          // array
          // io_L2
          {
            if (arb == 0) {
              cin_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ local_cin_pong, 
                /* fifo */ fifo_cin_in, 
                /* fifo */ fifo_cin_out, 
                /* enable */ inter_trans_en
              );
              cin_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_cin_ping, 
                /* fifo */ fifo_cin_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              cin_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ local_cin_ping, 
                /* fifo */ fifo_cin_in, 
                /* fifo */ fifo_cin_out, 
                /* enable */ inter_trans_en
              );
              cin_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_cin_pong, 
                /* fifo */ fifo_cin_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c1_prev = c1;
            c3_prev = c3;
          }
        }
    if (arb == 0) {
      cin_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_cin_ping, 
        /* fifo */ fifo_cin_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      cin_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
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
void cin_IO_L1_in_wrapper(int idx, hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cin_out, hls::stream<float> &fifo_cin_local_out)
 {
  cin_IO_L1_in(
    /* module id */ idx, 
    /* fifo */ fifo_cin_in, 
    /* fifo */ fifo_cin_out, 
    /* fifo */ fifo_cin_local_out);
}
/* Module Definition */

/* Module Definition */
void cin_IO_L1_in_boundary(int idx, hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cin_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  cin_t1 local_cin_ping[23][58][1];
  #pragma HLS RESOURCE variable=local_cin_ping core=RAM_1P_BRAM
  float local_cin_pong[23][58][1];
  #pragma HLS RESOURCE variable=local_cin_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 5; c3 += 1) {
          // array
          // io_L2
          {
            if (arb == 0) {
              cin_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ local_cin_pong, 
                /* fifo */ fifo_cin_in, 
                /* enable */ inter_trans_en
              );
              cin_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_cin_ping, 
                /* fifo */ fifo_cin_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              cin_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ local_cin_ping, 
                /* fifo */ fifo_cin_in, 
                /* enable */ inter_trans_en
              );
              cin_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_cin_pong, 
                /* fifo */ fifo_cin_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c1_prev = c1;
            c3_prev = c3;
          }
        }
    if (arb == 0) {
      cin_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_cin_ping, 
        /* fifo */ fifo_cin_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      cin_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
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
void cin_IO_L1_in_boundary_wrapper(int idx, hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cin_local_out)
 {
  cin_IO_L1_in_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_cin_in, 
    /* fifo */ fifo_cin_local_out);
}
/* Module Definition */

/* Module Definition */
void cout_1_IO_L2_in_intra_trans(int c0, int c1, int c3, cout_t4 *cout, cout_t4 local_cout_1[21][56][21], hls::stream<float> &fifo_cout_1_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (int c5 = 0; c5 <= min(13, -14 * c0 + 106); c5 += 1)
    for (int c6 = 0; c6 <= min(3, -4 * c1 + c1 / 2 + 9); c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 5; c7 += 1) {
        // latency
        for (int c10 = 0; c10 <= min(9, -10 * c7 + 55); c10 += 1) {
          // latency
          for (int c11 = 0; c11 <= min(min(5, -6 * c6 + 20), -21 * c1 - 6 * c6 + 55); c11 += 1) {
            // latency
            for (int c12 = 0; c12 <= min(5, -84 * c0 - 6 * c5 + 639); c12 += 1) {
            #pragma HLS PIPELINE II=1
              {
                cout_t4 in_data;
                cout_t1 out_data;
                in_data = local_cout_1[c11 + 6*c6][c10 + 10*c7][(6 * c5 + c12) / 4];
                for (ap_uint<3> n = 0; n < 4; n++) {
                #pragma HLS UNROLL
                  data_split[n] = in_data(31, 0);
                  in_data = in_data >> 32;
                }
                int split_idx = (c12 + 6*c5) % 4;
                union {unsigned int ui; float ut;} u;
                u.ui = (unsigned int)data_split[split_idx];
                out_data = u.ut;
                fifo_cout_1_local_out.write(out_data);
              }
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void cout_1_IO_L2_in_boundary(cout_t4 *cout, hls::stream<float> &fifo_cout_1_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  cout_t4 local_cout_1_ping[21][56][21];
  #pragma HLS RESOURCE variable=local_cout_1_ping core=RAM_2P_BRAM
  cout_t4 local_cout_1_pong[21][56][21];
  #pragma HLS RESOURCE variable=local_cout_1_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1)
        for (ap_uint<4> c3 = 1; c3 <= 5; c3 += 1) {
          // array
          {
            if (arb == 0) {
              cout_1_IO_L2_in_inter_trans_boundary(
                /* array */ cout, 
                /* array */ local_cout_1_pong, 
                /* fifo */ fifo_cout_1_in, 
                /* enable */ inter_trans_en
              );
              cout_1_IO_L2_in_intra_trans(
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ cout, 
                /* array */ local_cout_1_ping, 
                /* fifo */ fifo_cout_1_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              cout_1_IO_L2_in_inter_trans_boundary(
                /* array */ cout, 
                /* array */ local_cout_1_ping, 
                /* fifo */ fifo_cout_1_in, 
                /* enable */ inter_trans_en
              );
              cout_1_IO_L2_in_intra_trans(
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ cout, 
                /* array */ local_cout_1_pong, 
                /* fifo */ fifo_cout_1_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c1_prev = c1;
            c3_prev = c3;
          }
        }
    if (arb == 0) {
      cout_1_IO_L2_in_intra_trans(
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c3_prev, 
        /* array */ cout, 
        /* array */ local_cout_1_ping, 
        /* fifo */ fifo_cout_1_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      cout_1_IO_L2_in_intra_trans(
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c3_prev, 
        /* array */ cout, 
        /* array */ local_cout_1_pong, 
        /* fifo */ fifo_cout_1_local_out, 
        /* enable */ intra_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void w_IO_L2_in(w_t1 *w, hls::stream<float> &fifo_w_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1)
      for (ap_uint<4> c3 = 0; c3 <= 5; c3 += 1) {
        // array
        // io_L2
        for (int c4 = 0; c4 <= min(95, -96 * c3 + 511); c4 += 1) {
          // io_L1
          for (int c5 = 0; c5 <= min(83, -84 * c0 + 639); c5 += 1)
            for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
              for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
              #pragma HLS PIPELINE II=1
                // access_coalesce
                {
                  w_t1 in_data;
                  w_t1 out_data;
                  in_data = w[387072*c0 + 96*c3 + c4 + 4608*c5 + 1536*c6 + 512*c7];
                  out_data = in_data;
                  fifo_w_local_out.write(out_data);
                }
              }
        }
      }
}
/* Module Definition */

/* Module Definition */
void w_IO_L1_in_intra_trans(int idx, int c0, int c1, int c3, float local_w[84][3][3][1], hls::stream<float> &fifo_w_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L1
  // pe
  for (int c5 = 0; c5 <= min(13, -14 * c0 + 106); c5 += 1)
    for (int c6 = 0; c6 <= min(3, -4 * c1 + c1 / 2 + 9); c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 5; c7 += 1)
        for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
          for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
            // latency
            for (int c10 = 0; c10 <= min(9, -10 * c7 + 55); c10 += 1) {
              // latency
              for (int c11 = 0; c11 <= min(min(5, -6 * c6 + 20), -21 * c1 - 6 * c6 + 55); c11 += 1) {
                // latency
                for (int c12 = 0; c12 <= min(5, -84 * c0 - 6 * c5 + 639); c12 += 1) {
                #pragma HLS PIPELINE II=1
                  {
                    w_t1 in_data;
                    w_t1 out_data;
                    in_data = local_w[c12 + 6*c5][c8][c9][0];
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
void w_IO_L1_in_inter_trans(int idx, int c0, int c1, int c3, float local_w[84][3][3][1], hls::stream<float> &fifo_w_in, hls::stream<float> &fifo_w_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (int c4 = p0; c4 <= min(95, -96 * c3 + 511); c4 += 1) {
    // io_L1
    if (c4 == p0) {
      for (int c5 = 0; c5 <= min(83, -84 * c0 + 639); c5 += 1)
        for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
          for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
          #pragma HLS PIPELINE II=1
            // access_coalesce
            {
              w_t1 in_data;
              w_t1 out_data;
              in_data = fifo_w_in.read();
              out_data = in_data;
              local_w[c5][c6][c7][0] = out_data;
            }
          }
    } else {
      for (int c5 = 0; c5 <= min(83, -84 * c0 + 639); c5 += 1)
        for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
          for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
          #pragma HLS PIPELINE II=1
            // access_coalesce
            {
              w_t1 in_data;
              w_t1 out_data;
              in_data = fifo_w_in.read();
              out_data = in_data;
              fifo_w_out.write(out_data);
            }
          }
    }
  }
}
/* Module Definition */

/* Module Definition */
void w_IO_L1_in_inter_trans_boundary(int idx, int c0, int c1, int c3, float local_w[84][3][3][1], hls::stream<float> &fifo_w_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (int c4 = p0; c4 <= min(95, -96 * c3 + 511); c4 += 1)
    if (c4 == p0) {
      // io_L1
      for (int c5 = 0; c5 <= min(83, -84 * c0 + 639); c5 += 1)
        for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
          for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
          #pragma HLS PIPELINE II=1
            // access_coalesce
            {
              w_t1 in_data;
              w_t1 out_data;
              in_data = fifo_w_in.read();
              out_data = in_data;
              local_w[c5][c6][c7][0] = out_data;
            }
          }
    }
}
/* Module Definition */

/* Module Definition */
void w_IO_L1_in(int idx, hls::stream<float> &fifo_w_in, hls::stream<float> &fifo_w_out, hls::stream<float> &fifo_w_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  w_t1 local_w_ping[84][3][3][1];
  #pragma HLS RESOURCE variable=local_w_ping core=RAM_1P_BRAM
  float local_w_pong[84][3][3][1];
  #pragma HLS RESOURCE variable=local_w_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 5; c3 += 1) {
          // array
          // io_L2
          {
            if (arb == 0) {
              w_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ local_w_pong, 
                /* fifo */ fifo_w_in, 
                /* fifo */ fifo_w_out, 
                /* enable */ inter_trans_en
              );
              w_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_w_ping, 
                /* fifo */ fifo_w_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              w_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ local_w_ping, 
                /* fifo */ fifo_w_in, 
                /* fifo */ fifo_w_out, 
                /* enable */ inter_trans_en
              );
              w_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_w_pong, 
                /* fifo */ fifo_w_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c1_prev = c1;
            c3_prev = c3;
          }
        }
    if (arb == 0) {
      w_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_w_ping, 
        /* fifo */ fifo_w_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      w_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
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
void w_IO_L1_in_wrapper(int idx, hls::stream<float> &fifo_w_in, hls::stream<float> &fifo_w_out, hls::stream<float> &fifo_w_local_out)
 {
  w_IO_L1_in(
    /* module id */ idx, 
    /* fifo */ fifo_w_in, 
    /* fifo */ fifo_w_out, 
    /* fifo */ fifo_w_local_out);
}
/* Module Definition */

/* Module Definition */
void w_IO_L1_in_boundary(int idx, hls::stream<float> &fifo_w_in, hls::stream<float> &fifo_w_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  w_t1 local_w_ping[84][3][3][1];
  #pragma HLS RESOURCE variable=local_w_ping core=RAM_1P_BRAM
  float local_w_pong[84][3][3][1];
  #pragma HLS RESOURCE variable=local_w_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c1, c1_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 5; c3 += 1) {
          // array
          // io_L2
          {
            if (arb == 0) {
              w_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ local_w_pong, 
                /* fifo */ fifo_w_in, 
                /* enable */ inter_trans_en
              );
              w_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_w_ping, 
                /* fifo */ fifo_w_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              w_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ local_w_ping, 
                /* fifo */ fifo_w_in, 
                /* enable */ inter_trans_en
              );
              w_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_w_pong, 
                /* fifo */ fifo_w_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c1_prev = c1;
            c3_prev = c3;
          }
        }
    if (arb == 0) {
      w_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_w_ping, 
        /* fifo */ fifo_w_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      w_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
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
void w_IO_L1_in_boundary_wrapper(int idx, hls::stream<float> &fifo_w_in, hls::stream<float> &fifo_w_local_out)
 {
  w_IO_L1_in_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_w_in, 
    /* fifo */ fifo_w_local_out);
}
/* Module Definition */

/* Module Definition */
void PE(int idx, hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cout_1_in, hls::stream<float> &fifo_cout_1_out, hls::stream<float> &fifo_cout_drain_out, hls::stream<float> &fifo_w_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  cin_t1 local_cin[1][1][1];
  #pragma HLS ARRAY_PARTITION variable=local_cin dim=0 complete
  cout_t1 local_cout[21][56][84];
  #pragma HLS RESOURCE variable=local_cout core=RAM_2P_BRAM
  w_t1 local_w[1][1][1][1];
  #pragma HLS ARRAY_PARTITION variable=local_w dim=0 complete
  /* Variable Declaration */

  for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1)
      for (ap_uint<4> c3 = 0; c3 <= 5; c3 += 1)
        if (p0 + 96 * c3 <= 511) {
          // array
          // pe
          for (int c5 = 0; c5 <= min(13, -14 * c0 + 106); c5 += 1)
            for (int c6 = 0; c6 <= min(3, -4 * c1 + c1 / 2 + 9); c6 += 1)
              for (ap_uint<4> c7 = 0; c7 <= 5; c7 += 1) {
                if (p0 == 0 && c3 == 0) {
                  // latency
                  for (int c8 = 0; c8 <= min(9, -10 * c7 + 55); c8 += 1) {
                    // latency
                    for (int c9 = 0; c9 <= min(min(5, -6 * c6 + 20), -21 * c1 - 6 * c6 + 55); c9 += 1) {
                      // latency
                      for (int c10 = 0; c10 <= min(5, -84 * c0 - 6 * c5 + 639); c10 += 1) {
                      #pragma HLS PIPELINE II=1
                        local_cout[6*c6 + c9][10*c7 + c8][c10 + 6*c5] = 0;
                      }
                    }
                  }
                }
                for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
                  for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
                    // latency
                    for (int c10 = 0; c10 <= min(9, -10 * c7 + 55); c10 += 1) {
                      // latency
                      for (int c11 = 0; c11 <= min(min(5, -6 * c6 + 20), -21 * c1 - 6 * c6 + 55); c11 += 1) {
                        // latency
                        for (int c12 = 0; c12 <= min(5, -84 * c0 - 6 * c5 + 639); c12 += 1) {
                        #pragma HLS PIPELINE II=1
                          {
                            local_cin[0][0][0] = fifo_cin_in.read();
                            if (p0 + 96 * c3 >= 1 && c8 == 0 && c9 == 0)
                              local_cout[c11 + 6*c6][c10 + 10*c7][c12 + 6*c5] = fifo_cout_1_in.read();
                            local_w[0][0][0][0] = fifo_w_in.read();
                            local_cout[c11 + 6*c6][c10 + 10*c7][c12 + 6*c5] = (local_cout[c11 + 6*c6][c10 + 10*c7][c12 + 6*c5] + (local_cin[0][0][0] * local_w[0][0][0][0]));
                            if (p0 == 31 && c3 == 5 && c8 == 2 && c9 == 2) {
                              fifo_cout_drain_out.write(local_cout[c11 + 6*c6][c10 + 10*c7][c12 + 6*c5]);
                            } else if (p0 + 96 * c3 <= 510 && c8 == 2 && c9 == 2) {
                              fifo_cout_1_out.write(local_cout[c11 + 6*c6][c10 + 10*c7][c12 + 6*c5]);
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
void PE_wrapper(int idx, hls::stream<float> &fifo_cin_in, hls::stream<float> &fifo_cout_1_in, hls::stream<float> &fifo_cout_1_out, hls::stream<float> &fifo_cout_drain_out, hls::stream<float> &fifo_w_in)
 {
  PE(
    /* module id */ idx, 
    /* fifo */ fifo_cin_in, 
    /* fifo */ fifo_cout_1_in, 
    /* fifo */ fifo_cout_1_out, 
    /* fifo */ fifo_cout_drain_out, 
    /* fifo */ fifo_w_in);
}
/* Module Definition */

/* Module Definition */
void cout_1_PE_dummy_out(int idx, hls::stream<float> &fifo_cout_1_out) {
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1) {
      // array
      {
      }
      for (ap_uint<4> c3 = 0; c3 <= 5; c3 += 1)
        if (p0 + 96 * c3 >= 1 && p0 + 96 * c3 <= 511) {
          // array
          // pe
          for (int c5 = 0; c5 <= min(13, -14 * c0 + 106); c5 += 1)
            for (int c6 = 0; c6 <= min(3, -4 * c1 + c1 / 2 + 9); c6 += 1)
              for (ap_uint<4> c7 = 0; c7 <= 5; c7 += 1)
                for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
                  if (c8 == 0)
                    for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1)
                      if (c9 == 0) {
                        // latency
                        for (int c10 = 0; c10 <= min(9, -10 * c7 + 55); c10 += 1) {
                          // latency
                          for (int c11 = 0; c11 <= min(min(5, -6 * c6 + 20), -21 * c1 - 6 * c6 + 55); c11 += 1) {
                            // latency
                            for (int c12 = 0; c12 <= min(5, -84 * c0 - 6 * c5 + 639); c12 += 1) {
                            #pragma HLS PIPELINE II=1
                              cout_t1 fifo_data = 0;
                              fifo_cout_1_out.write(fifo_data);
                            }
                          }
                        }
                      }
        }
    }
}
/* Module Definition */

/* Module Definition */
void cout_1_IO_L2_out_intra_trans(int c0, int c1, int c3, cout_t4 *cout, cout_t4 local_cout_1[21][56][21], hls::stream<float> &fifo_cout_1_local_in, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (int c5 = 0; c5 <= min(13, -14 * c0 + 106); c5 += 1)
    for (int c6 = 0; c6 <= min(3, -4 * c1 + c1 / 2 + 9); c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 5; c7 += 1) {
        // latency
        for (int c10 = 0; c10 <= min(9, -10 * c7 + 55); c10 += 1) {
          // latency
          for (int c11 = 0; c11 <= min(min(5, -6 * c6 + 20), -21 * c1 - 6 * c6 + 55); c11 += 1) {
            // latency
            for (int c12 = 0; c12 <= min(5, -84 * c0 - 6 * c5 + 639); c12 += 1) {
            #pragma HLS PIPELINE II=1
              {
                cout_t1 in_data;
                cout_t4 out_data;
                in_data = fifo_cout_1_local_in.read();
                int split_idx = (c12 + 6*c5) % 4;
                out_data = local_cout_1[c11 + 6*c6][c10 + 10*c7][(6 * c5 + c12) / 4];
                for (ap_uint<3> n = 0; n < 4; n++) {
                #pragma HLS UNROLL
                  data_split[n] = out_data(31, 0);
                  out_data = out_data >> 32;
                }
                union {unsigned int ui; float ut;} u;
                u.ut = in_data;
                data_split[split_idx] = ap_uint<32>(u.ui);
                out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);                local_cout_1[c11 + 6*c6][c10 + 10*c7][(6 * c5 + c12) / 4] = out_data;
              }
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void cout_1_IO_L2_out_inter_trans_boundary(int c0, int c1, cout_t4 *cout, cout_t4 local_cout_1[21][56][21], hls::stream<cout_t4> &fifo_cout_1_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  if (!inter_trans_en) return;

  // io_L2
  for (int c4 = 0; c4 <= min(20, -21 * c1 + 55); c4 += 1)
    for (ap_uint<7> c5 = 0; c5 <= 55; c5 += 1) {
      // access_coalesce
      for (int c6 = 0; c6 <= min(20, -21 * c0 + 159); c6 += 1) {
      #pragma HLS PIPELINE II=1
        {
          cout_t4 in_data;
          cout_t4 out_data;
          in_data = local_cout_1[c4][c5][c6];
          out_data = in_data;
          cout[21*c0 + 188160*c1 + 8960*c4 + 160*c5 + c6] = out_data;
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void cout_1_IO_L2_out_boundary(cout_t4 *cout, hls::stream<float> &fifo_cout_1_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  cout_t4 local_cout_1_ping[21][56][21];
  #pragma HLS RESOURCE variable=local_cout_1_ping core=RAM_2P_BRAM
  cout_t4 local_cout_1_pong[21][56][21];
  #pragma HLS RESOURCE variable=local_cout_1_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 0;
  bool intra_trans_en = 1;
  int c0, c0_prev;
  int c1, c1_prev;
  /* Variable Declaration */

  {
    for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
      for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 4; c3 += 1) {
          // array
          {
            if (arb == 0) {
              cout_1_IO_L2_out_intra_trans(
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ cout, 
                /* array */ local_cout_1_ping, 
                /* fifo */ fifo_cout_1_local_in, 
                /* enable */ intra_trans_en
              );
              cout_1_IO_L2_out_inter_trans_boundary(
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* array */ cout, 
                /* array */ local_cout_1_pong, 
                /* fifo */ fifo_cout_1_out, 
                /* enable */ inter_trans_en
              );
            } else {
              cout_1_IO_L2_out_intra_trans(
                /* host iter */ c0, 
                /* host iter */ c1, 
                /* host iter */ c3, 
                /* array */ cout, 
                /* array */ local_cout_1_pong, 
                /* fifo */ fifo_cout_1_local_in, 
                /* enable */ intra_trans_en
              );
              cout_1_IO_L2_out_inter_trans_boundary(
                /* host iter */ c0_prev, 
                /* host iter */ c1_prev, 
                /* array */ cout, 
                /* array */ local_cout_1_ping, 
                /* fifo */ fifo_cout_1_out, 
                /* enable */ inter_trans_en
              );
            }
            inter_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c1_prev = c1;
          }
        }
    if (arb == 0) {
      cout_1_IO_L2_out_inter_trans_boundary(
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* array */ cout, 
        /* array */ local_cout_1_pong, 
        /* fifo */ fifo_cout_1_out, 
        /* enable */ inter_trans_en
      );
    } else {
      cout_1_IO_L2_out_inter_trans_boundary(
        /* host iter */ c0_prev, 
        /* host iter */ c1_prev, 
        /* array */ cout, 
        /* array */ local_cout_1_ping, 
        /* fifo */ fifo_cout_1_out, 
        /* enable */ inter_trans_en
      );
    }
  }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_intra_trans(int idx, int c0, int c1, cout_t4 local_cout[21][56][21], hls::stream<float> &fifo_cout_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */


  // io_L1
  // pe
  for (int c5 = 0; c5 <= min(13, -14 * c0 + 106); c5 += 1)
    for (int c6 = 0; c6 <= min(3, -4 * c1 + c1 / 2 + 9); c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 5; c7 += 1) {
        // latency
        for (int c10 = 0; c10 <= min(9, -10 * c7 + 55); c10 += 1) {
          // latency
          for (int c11 = 0; c11 <= min(min(5, -6 * c6 + 20), -21 * c1 - 6 * c6 + 55); c11 += 1) {
            // latency
            for (int c12 = 0; c12 <= min(5, -84 * c0 - 6 * c5 + 639); c12 += 1) {
            #pragma HLS PIPELINE II=1
              {
                cout_t1 in_data;
                cout_t4 out_data;
                in_data = fifo_cout_drain_local_in.read();
                int split_idx = (c12 + 6*c5) % 4;
                out_data = local_cout[c11 + 6*c6][c10 + 10*c7][(6 * c5 + c12) / 4];
                for (ap_uint<3> n = 0; n < 4; n++) {
                #pragma HLS UNROLL
                  data_split[n] = out_data(31, 0);
                  out_data = out_data >> 32;
                }
                union {unsigned int ui; float ut;} u;
                u.ut = in_data;
                data_split[split_idx] = ap_uint<32>(u.ui);
                out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);                local_cout[c11 + 6*c6][c10 + 10*c7][(6 * c5 + c12) / 4] = out_data;
              }
            }
          }
        }
      }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_inter_trans(int idx, int c0, int c1, cout_t4 local_cout[21][56][21], hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  // io_L1
  if (p0 == 31) {
    for (int c5 = 0; c5 <= min(20, -21 * c1 + 55); c5 += 1)
      for (ap_uint<7> c6 = 0; c6 <= 55; c6 += 1) {
        // access_coalesce
        for (int c7 = 0; c7 <= min(20, -21 * c0 + 159); c7 += 1) {
        #pragma HLS PIPELINE II=1
          {
            cout_t4 in_data;
            cout_t4 out_data;
            in_data = local_cout[c5][c6][c7];
            out_data = in_data;
            fifo_cout_drain_out.write(out_data);
          }
        }
      }
  } else {
    for (int c5 = 0; c5 <= min(20, -21 * c1 + 55); c5 += 1)
      for (ap_uint<7> c6 = 0; c6 <= 55; c6 += 1) {
        // access_coalesce
        for (int c7 = 0; c7 <= min(20, -21 * c0 + 159); c7 += 1) {
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
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_inter_trans_boundary(int idx, int c0, int c1, cout_t4 local_cout[21][56][21], hls::stream<cout_t4> &fifo_cout_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  // io_L1
  for (int c5 = 0; c5 <= min(20, -21 * c1 + 55); c5 += 1)
    for (ap_uint<7> c6 = 0; c6 <= 55; c6 += 1) {
      // access_coalesce
      for (int c7 = 0; c7 <= min(20, -21 * c0 + 159); c7 += 1) {
      #pragma HLS PIPELINE II=1
        {
          cout_t4 in_data;
          cout_t4 out_data;
          in_data = local_cout[c5][c6][c7];
          out_data = in_data;
          fifo_cout_drain_out.write(out_data);
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out(int idx, hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  cout_t4 local_cout[21][56][21];
  #pragma HLS RESOURCE variable=local_cout core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1) {
      // array
      // io_L2
      cout_drain_IO_L1_out_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0, 
        /* host iter */ c1, 
        /* array */ local_cout, 
        /* fifo */ fifo_cout_drain_local_in
      );
      cout_drain_IO_L1_out_inter_trans(
        /* module id */ idx, 
        /* host iter */ c0, 
        /* host iter */ c1, 
        /* array */ local_cout, 
        /* fifo */ fifo_cout_drain_in, 
        /* fifo */ fifo_cout_drain_out
      );
    }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_wrapper(int idx, hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in)
 {
  cout_drain_IO_L1_out(
    /* module id */ idx, 
    /* fifo */ fifo_cout_drain_in, 
    /* fifo */ fifo_cout_drain_out, 
    /* fifo */ fifo_cout_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_boundary(int idx, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  cout_t4 local_cout[21][56][21];
  #pragma HLS RESOURCE variable=local_cout core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1) {
      // array
      // io_L2
      cout_drain_IO_L1_out_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0, 
        /* host iter */ c1, 
        /* array */ local_cout, 
        /* fifo */ fifo_cout_drain_local_in
      );
      cout_drain_IO_L1_out_inter_trans_boundary(
        /* module id */ idx, 
        /* host iter */ c0, 
        /* host iter */ c1, 
        /* array */ local_cout, 
        /* fifo */ fifo_cout_drain_out
      );
    }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_boundary_wrapper(int idx, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in)
 {
  cout_drain_IO_L1_out_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_cout_drain_out, 
    /* fifo */ fifo_cout_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L2_out(cout_t4 *cout, hls::stream<cout_t4> &fifo_cout_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<4> c0 = 0; c0 <= 7; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 2; c1 += 1) {
      // array
      // io_L2
      // io_L1
      for (int c5 = 0; c5 <= min(20, -21 * c1 + 55); c5 += 1)
        for (ap_uint<7> c6 = 0; c6 <= 55; c6 += 1) {
          // access_coalesce
          for (int c7 = 0; c7 <= min(20, -21 * c0 + 159); c7 += 1) {
          #pragma HLS PIPELINE II=1
            {
              cout_t4 in_data;
              cout_t4 out_data;
              in_data = fifo_cout_drain_local_in.read();
              out_data = in_data;
              cout[21*c0 + 188160*c1 + 8960*c5 + 160*c6 + c7] = out_data;
            }
          }
        }
    }
}
/* Module Definition */

extern "C" {
void kernel0(cin_t1 *cin, cout_t4 *cout_0, cout_t4 *cout_1, cout_t4 *cout_2, w_t1 *w)
{
#pragma HLS INTERFACE m_axi port=cin offset=slave bundle=gmem_cin
#pragma HLS INTERFACE m_axi port=cout_0 offset=slave bundle=gmem_cout_0
#pragma HLS INTERFACE m_axi port=cout_1 offset=slave bundle=gmem_cout_1
#pragma HLS INTERFACE m_axi port=cout_2 offset=slave bundle=gmem_cout_2
#pragma HLS INTERFACE m_axi port=w offset=slave bundle=gmem_w
#pragma HLS INTERFACE s_axilite port=cin bundle=control
#pragma HLS INTERFACE s_axilite port=cout_0 bundle=control
#pragma HLS INTERFACE s_axilite port=cout_1 bundle=control
#pragma HLS INTERFACE s_axilite port=cout_2 bundle=control
#pragma HLS INTERFACE s_axilite port=w bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW

  /* FIFO Declaration */
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_0;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_0 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_1;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_1 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_2;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_2 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_3;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_3 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_4;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_4 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_5;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_5 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_6;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_6 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_7;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_7 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_8;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_8 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_9;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_9 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_9 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_10;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_10 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_10 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_11;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_11 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_11 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_12;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_12 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_12 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_13;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_13 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_13 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_14;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_14 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_14 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_15;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_15 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_15 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_16;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_16 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_16 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_17;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_17 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_17 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_18;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_18 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_18 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_19;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_19 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_19 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_20;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_20 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_20 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_21;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_21 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_21 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_22;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_22 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_22 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_23;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_23 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_23 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_24;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_24 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_24 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_25;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_25 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_25 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_26;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_26 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_26 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_27;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_27 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_27 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_28;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_28 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_28 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_29;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_29 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_29 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_30;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_30 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_30 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_31;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_31 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_31 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_32;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_32 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_32 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_33;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_33 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_33 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_34;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_34 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_34 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_35;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_35 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_35 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_36;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_36 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_36 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_37;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_37 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_37 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_38;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_38 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_38 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_39;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_39 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_39 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_40;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_40 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_40 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_41;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_41 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_41 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_42;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_42 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_42 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_43;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_43 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_43 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_44;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_44 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_44 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_45;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_45 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_45 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_46;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_46 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_46 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_47;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_47 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_47 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_48;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_48 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_48 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_49;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_49 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_49 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_50;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_50 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_50 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_51;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_51 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_51 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_52;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_52 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_52 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_53;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_53 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_53 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_54;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_54 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_54 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_55;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_55 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_55 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_56;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_56 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_56 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_57;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_57 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_57 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_58;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_58 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_58 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_59;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_59 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_59 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_60;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_60 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_60 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_61;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_61 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_61 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_62;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_62 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_62 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_63;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_63 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_63 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_64;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_64 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_64 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_65;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_65 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_65 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_66;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_66 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_66 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_67;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_67 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_67 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_68;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_68 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_68 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_69;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_69 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_69 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_70;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_70 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_70 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_71;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_71 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_71 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_72;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_72 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_72 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_73;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_73 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_73 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_74;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_74 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_74 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_75;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_75 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_75 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_76;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_76 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_76 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_77;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_77 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_77 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_78;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_78 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_78 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_79;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_79 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_79 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_80;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_80 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_80 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_81;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_81 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_81 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_82;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_82 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_82 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_83;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_83 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_83 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_84;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_84 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_84 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_85;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_85 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_85 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_86;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_86 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_86 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_87;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_87 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_87 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_88;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_88 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_88 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_89;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_89 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_89 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_90;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_90 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_90 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_91;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_91 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_91 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_92;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_92 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_92 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_93;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_93 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_93 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_94;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_94 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_94 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_95;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_95 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_95 core=FIFO_SRL
  /* cin_IO_L1_in fifo */ hls::stream<float> fifo_cin_cin_IO_L1_in_96;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L1_in_96 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_cin_IO_L1_in_96 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_0;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_0 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_1;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_1 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_2;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_2 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_3;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_3 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_4;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_4 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_5;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_5 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_6;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_6 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_7;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_7 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_8;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_8 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_9;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_9 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_9 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_10;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_10 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_10 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_11;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_11 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_11 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_12;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_12 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_12 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_13;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_13 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_13 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_14;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_14 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_14 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_15;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_15 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_15 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_16;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_16 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_16 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_17;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_17 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_17 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_18;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_18 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_18 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_19;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_19 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_19 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_20;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_20 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_20 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_21;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_21 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_21 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_22;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_22 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_22 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_23;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_23 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_23 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_24;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_24 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_24 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_25;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_25 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_25 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_26;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_26 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_26 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_27;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_27 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_27 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_28;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_28 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_28 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_29;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_29 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_29 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_30;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_30 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_30 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_31;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_31 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_31 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_32;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_32 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_32 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_33;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_33 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_33 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_34;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_34 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_34 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_35;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_35 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_35 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_36;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_36 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_36 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_37;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_37 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_37 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_38;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_38 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_38 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_39;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_39 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_39 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_40;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_40 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_40 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_41;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_41 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_41 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_42;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_42 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_42 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_43;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_43 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_43 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_44;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_44 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_44 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_45;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_45 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_45 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_46;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_46 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_46 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_47;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_47 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_47 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_48;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_48 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_48 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_49;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_49 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_49 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_50;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_50 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_50 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_51;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_51 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_51 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_52;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_52 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_52 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_53;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_53 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_53 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_54;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_54 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_54 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_55;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_55 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_55 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_56;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_56 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_56 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_57;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_57 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_57 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_58;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_58 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_58 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_59;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_59 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_59 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_60;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_60 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_60 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_61;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_61 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_61 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_62;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_62 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_62 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_63;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_63 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_63 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_64;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_64 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_64 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_65;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_65 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_65 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_66;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_66 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_66 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_67;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_67 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_67 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_68;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_68 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_68 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_69;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_69 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_69 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_70;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_70 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_70 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_71;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_71 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_71 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_72;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_72 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_72 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_73;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_73 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_73 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_74;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_74 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_74 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_75;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_75 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_75 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_76;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_76 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_76 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_77;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_77 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_77 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_78;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_78 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_78 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_79;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_79 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_79 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_80;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_80 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_80 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_81;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_81 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_81 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_82;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_82 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_82 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_83;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_83 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_83 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_84;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_84 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_84 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_85;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_85 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_85 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_86;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_86 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_86 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_87;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_87 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_87 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_88;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_88 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_88 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_89;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_89 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_89 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_90;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_90 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_90 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_91;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_91 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_91 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_92;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_92 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_92 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_93;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_93 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_93 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_94;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_94 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_94 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_95;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_95 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_95 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<float> fifo_w_w_IO_L1_in_96;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_96 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_96 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_0;
  #pragma HLS STREAM variable=fifo_cin_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_1;
  #pragma HLS STREAM variable=fifo_cin_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_2;
  #pragma HLS STREAM variable=fifo_cin_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_3;
  #pragma HLS STREAM variable=fifo_cin_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_4;
  #pragma HLS STREAM variable=fifo_cin_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_5;
  #pragma HLS STREAM variable=fifo_cin_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_6;
  #pragma HLS STREAM variable=fifo_cin_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_7;
  #pragma HLS STREAM variable=fifo_cin_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_8;
  #pragma HLS STREAM variable=fifo_cin_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_9;
  #pragma HLS STREAM variable=fifo_cin_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_10;
  #pragma HLS STREAM variable=fifo_cin_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_11;
  #pragma HLS STREAM variable=fifo_cin_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_12;
  #pragma HLS STREAM variable=fifo_cin_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_13;
  #pragma HLS STREAM variable=fifo_cin_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_14;
  #pragma HLS STREAM variable=fifo_cin_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_15;
  #pragma HLS STREAM variable=fifo_cin_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_16;
  #pragma HLS STREAM variable=fifo_cin_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_17;
  #pragma HLS STREAM variable=fifo_cin_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_18;
  #pragma HLS STREAM variable=fifo_cin_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_19;
  #pragma HLS STREAM variable=fifo_cin_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_20;
  #pragma HLS STREAM variable=fifo_cin_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_21;
  #pragma HLS STREAM variable=fifo_cin_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_22;
  #pragma HLS STREAM variable=fifo_cin_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_23;
  #pragma HLS STREAM variable=fifo_cin_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_24;
  #pragma HLS STREAM variable=fifo_cin_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_25;
  #pragma HLS STREAM variable=fifo_cin_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_26;
  #pragma HLS STREAM variable=fifo_cin_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_27;
  #pragma HLS STREAM variable=fifo_cin_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_28;
  #pragma HLS STREAM variable=fifo_cin_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_29;
  #pragma HLS STREAM variable=fifo_cin_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_30;
  #pragma HLS STREAM variable=fifo_cin_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_31;
  #pragma HLS STREAM variable=fifo_cin_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_32;
  #pragma HLS STREAM variable=fifo_cin_PE_32 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_32 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_33;
  #pragma HLS STREAM variable=fifo_cin_PE_33 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_33 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_34;
  #pragma HLS STREAM variable=fifo_cin_PE_34 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_34 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_35;
  #pragma HLS STREAM variable=fifo_cin_PE_35 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_35 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_36;
  #pragma HLS STREAM variable=fifo_cin_PE_36 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_36 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_37;
  #pragma HLS STREAM variable=fifo_cin_PE_37 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_37 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_38;
  #pragma HLS STREAM variable=fifo_cin_PE_38 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_38 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_39;
  #pragma HLS STREAM variable=fifo_cin_PE_39 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_39 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_40;
  #pragma HLS STREAM variable=fifo_cin_PE_40 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_40 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_41;
  #pragma HLS STREAM variable=fifo_cin_PE_41 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_41 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_42;
  #pragma HLS STREAM variable=fifo_cin_PE_42 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_42 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_43;
  #pragma HLS STREAM variable=fifo_cin_PE_43 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_43 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_44;
  #pragma HLS STREAM variable=fifo_cin_PE_44 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_44 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_45;
  #pragma HLS STREAM variable=fifo_cin_PE_45 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_45 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_46;
  #pragma HLS STREAM variable=fifo_cin_PE_46 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_46 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_47;
  #pragma HLS STREAM variable=fifo_cin_PE_47 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_47 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_48;
  #pragma HLS STREAM variable=fifo_cin_PE_48 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_48 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_49;
  #pragma HLS STREAM variable=fifo_cin_PE_49 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_49 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_50;
  #pragma HLS STREAM variable=fifo_cin_PE_50 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_50 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_51;
  #pragma HLS STREAM variable=fifo_cin_PE_51 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_51 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_52;
  #pragma HLS STREAM variable=fifo_cin_PE_52 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_52 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_53;
  #pragma HLS STREAM variable=fifo_cin_PE_53 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_53 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_54;
  #pragma HLS STREAM variable=fifo_cin_PE_54 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_54 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_55;
  #pragma HLS STREAM variable=fifo_cin_PE_55 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_55 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_56;
  #pragma HLS STREAM variable=fifo_cin_PE_56 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_56 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_57;
  #pragma HLS STREAM variable=fifo_cin_PE_57 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_57 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_58;
  #pragma HLS STREAM variable=fifo_cin_PE_58 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_58 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_59;
  #pragma HLS STREAM variable=fifo_cin_PE_59 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_59 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_60;
  #pragma HLS STREAM variable=fifo_cin_PE_60 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_60 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_61;
  #pragma HLS STREAM variable=fifo_cin_PE_61 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_61 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_62;
  #pragma HLS STREAM variable=fifo_cin_PE_62 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_62 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_63;
  #pragma HLS STREAM variable=fifo_cin_PE_63 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_63 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_64;
  #pragma HLS STREAM variable=fifo_cin_PE_64 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_64 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_65;
  #pragma HLS STREAM variable=fifo_cin_PE_65 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_65 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_66;
  #pragma HLS STREAM variable=fifo_cin_PE_66 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_66 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_67;
  #pragma HLS STREAM variable=fifo_cin_PE_67 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_67 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_68;
  #pragma HLS STREAM variable=fifo_cin_PE_68 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_68 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_69;
  #pragma HLS STREAM variable=fifo_cin_PE_69 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_69 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_70;
  #pragma HLS STREAM variable=fifo_cin_PE_70 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_70 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_71;
  #pragma HLS STREAM variable=fifo_cin_PE_71 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_71 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_72;
  #pragma HLS STREAM variable=fifo_cin_PE_72 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_72 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_73;
  #pragma HLS STREAM variable=fifo_cin_PE_73 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_73 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_74;
  #pragma HLS STREAM variable=fifo_cin_PE_74 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_74 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_75;
  #pragma HLS STREAM variable=fifo_cin_PE_75 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_75 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_76;
  #pragma HLS STREAM variable=fifo_cin_PE_76 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_76 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_77;
  #pragma HLS STREAM variable=fifo_cin_PE_77 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_77 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_78;
  #pragma HLS STREAM variable=fifo_cin_PE_78 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_78 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_79;
  #pragma HLS STREAM variable=fifo_cin_PE_79 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_79 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_80;
  #pragma HLS STREAM variable=fifo_cin_PE_80 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_80 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_81;
  #pragma HLS STREAM variable=fifo_cin_PE_81 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_81 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_82;
  #pragma HLS STREAM variable=fifo_cin_PE_82 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_82 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_83;
  #pragma HLS STREAM variable=fifo_cin_PE_83 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_83 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_84;
  #pragma HLS STREAM variable=fifo_cin_PE_84 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_84 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_85;
  #pragma HLS STREAM variable=fifo_cin_PE_85 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_85 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_86;
  #pragma HLS STREAM variable=fifo_cin_PE_86 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_86 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_87;
  #pragma HLS STREAM variable=fifo_cin_PE_87 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_87 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_88;
  #pragma HLS STREAM variable=fifo_cin_PE_88 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_88 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_89;
  #pragma HLS STREAM variable=fifo_cin_PE_89 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_89 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_90;
  #pragma HLS STREAM variable=fifo_cin_PE_90 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_90 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_91;
  #pragma HLS STREAM variable=fifo_cin_PE_91 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_91 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_92;
  #pragma HLS STREAM variable=fifo_cin_PE_92 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_92 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_93;
  #pragma HLS STREAM variable=fifo_cin_PE_93 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_93 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_94;
  #pragma HLS STREAM variable=fifo_cin_PE_94 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_94 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cin_PE_95;
  #pragma HLS STREAM variable=fifo_cin_PE_95 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_95 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_0;
  #pragma HLS STREAM variable=fifo_cout_1_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_1;
  #pragma HLS STREAM variable=fifo_cout_1_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_2;
  #pragma HLS STREAM variable=fifo_cout_1_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_3;
  #pragma HLS STREAM variable=fifo_cout_1_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_4;
  #pragma HLS STREAM variable=fifo_cout_1_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_5;
  #pragma HLS STREAM variable=fifo_cout_1_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_6;
  #pragma HLS STREAM variable=fifo_cout_1_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_7;
  #pragma HLS STREAM variable=fifo_cout_1_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_8;
  #pragma HLS STREAM variable=fifo_cout_1_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_9;
  #pragma HLS STREAM variable=fifo_cout_1_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_10;
  #pragma HLS STREAM variable=fifo_cout_1_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_11;
  #pragma HLS STREAM variable=fifo_cout_1_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_12;
  #pragma HLS STREAM variable=fifo_cout_1_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_13;
  #pragma HLS STREAM variable=fifo_cout_1_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_14;
  #pragma HLS STREAM variable=fifo_cout_1_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_15;
  #pragma HLS STREAM variable=fifo_cout_1_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_16;
  #pragma HLS STREAM variable=fifo_cout_1_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_17;
  #pragma HLS STREAM variable=fifo_cout_1_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_18;
  #pragma HLS STREAM variable=fifo_cout_1_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_19;
  #pragma HLS STREAM variable=fifo_cout_1_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_20;
  #pragma HLS STREAM variable=fifo_cout_1_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_21;
  #pragma HLS STREAM variable=fifo_cout_1_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_22;
  #pragma HLS STREAM variable=fifo_cout_1_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_23;
  #pragma HLS STREAM variable=fifo_cout_1_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_24;
  #pragma HLS STREAM variable=fifo_cout_1_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_25;
  #pragma HLS STREAM variable=fifo_cout_1_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_26;
  #pragma HLS STREAM variable=fifo_cout_1_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_27;
  #pragma HLS STREAM variable=fifo_cout_1_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_28;
  #pragma HLS STREAM variable=fifo_cout_1_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_29;
  #pragma HLS STREAM variable=fifo_cout_1_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_30;
  #pragma HLS STREAM variable=fifo_cout_1_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_31;
  #pragma HLS STREAM variable=fifo_cout_1_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_32;
  #pragma HLS STREAM variable=fifo_cout_1_PE_32 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_32 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_33;
  #pragma HLS STREAM variable=fifo_cout_1_PE_33 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_33 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_34;
  #pragma HLS STREAM variable=fifo_cout_1_PE_34 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_34 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_35;
  #pragma HLS STREAM variable=fifo_cout_1_PE_35 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_35 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_36;
  #pragma HLS STREAM variable=fifo_cout_1_PE_36 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_36 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_37;
  #pragma HLS STREAM variable=fifo_cout_1_PE_37 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_37 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_38;
  #pragma HLS STREAM variable=fifo_cout_1_PE_38 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_38 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_39;
  #pragma HLS STREAM variable=fifo_cout_1_PE_39 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_39 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_40;
  #pragma HLS STREAM variable=fifo_cout_1_PE_40 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_40 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_41;
  #pragma HLS STREAM variable=fifo_cout_1_PE_41 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_41 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_42;
  #pragma HLS STREAM variable=fifo_cout_1_PE_42 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_42 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_43;
  #pragma HLS STREAM variable=fifo_cout_1_PE_43 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_43 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_44;
  #pragma HLS STREAM variable=fifo_cout_1_PE_44 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_44 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_45;
  #pragma HLS STREAM variable=fifo_cout_1_PE_45 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_45 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_46;
  #pragma HLS STREAM variable=fifo_cout_1_PE_46 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_46 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_47;
  #pragma HLS STREAM variable=fifo_cout_1_PE_47 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_47 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_48;
  #pragma HLS STREAM variable=fifo_cout_1_PE_48 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_48 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_49;
  #pragma HLS STREAM variable=fifo_cout_1_PE_49 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_49 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_50;
  #pragma HLS STREAM variable=fifo_cout_1_PE_50 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_50 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_51;
  #pragma HLS STREAM variable=fifo_cout_1_PE_51 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_51 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_52;
  #pragma HLS STREAM variable=fifo_cout_1_PE_52 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_52 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_53;
  #pragma HLS STREAM variable=fifo_cout_1_PE_53 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_53 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_54;
  #pragma HLS STREAM variable=fifo_cout_1_PE_54 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_54 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_55;
  #pragma HLS STREAM variable=fifo_cout_1_PE_55 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_55 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_56;
  #pragma HLS STREAM variable=fifo_cout_1_PE_56 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_56 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_57;
  #pragma HLS STREAM variable=fifo_cout_1_PE_57 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_57 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_58;
  #pragma HLS STREAM variable=fifo_cout_1_PE_58 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_58 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_59;
  #pragma HLS STREAM variable=fifo_cout_1_PE_59 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_59 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_60;
  #pragma HLS STREAM variable=fifo_cout_1_PE_60 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_60 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_61;
  #pragma HLS STREAM variable=fifo_cout_1_PE_61 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_61 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_62;
  #pragma HLS STREAM variable=fifo_cout_1_PE_62 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_62 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_63;
  #pragma HLS STREAM variable=fifo_cout_1_PE_63 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_63 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_64;
  #pragma HLS STREAM variable=fifo_cout_1_PE_64 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_64 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_65;
  #pragma HLS STREAM variable=fifo_cout_1_PE_65 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_65 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_66;
  #pragma HLS STREAM variable=fifo_cout_1_PE_66 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_66 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_67;
  #pragma HLS STREAM variable=fifo_cout_1_PE_67 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_67 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_68;
  #pragma HLS STREAM variable=fifo_cout_1_PE_68 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_68 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_69;
  #pragma HLS STREAM variable=fifo_cout_1_PE_69 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_69 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_70;
  #pragma HLS STREAM variable=fifo_cout_1_PE_70 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_70 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_71;
  #pragma HLS STREAM variable=fifo_cout_1_PE_71 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_71 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_72;
  #pragma HLS STREAM variable=fifo_cout_1_PE_72 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_72 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_73;
  #pragma HLS STREAM variable=fifo_cout_1_PE_73 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_73 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_74;
  #pragma HLS STREAM variable=fifo_cout_1_PE_74 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_74 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_75;
  #pragma HLS STREAM variable=fifo_cout_1_PE_75 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_75 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_76;
  #pragma HLS STREAM variable=fifo_cout_1_PE_76 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_76 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_77;
  #pragma HLS STREAM variable=fifo_cout_1_PE_77 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_77 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_78;
  #pragma HLS STREAM variable=fifo_cout_1_PE_78 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_78 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_79;
  #pragma HLS STREAM variable=fifo_cout_1_PE_79 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_79 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_80;
  #pragma HLS STREAM variable=fifo_cout_1_PE_80 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_80 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_81;
  #pragma HLS STREAM variable=fifo_cout_1_PE_81 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_81 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_82;
  #pragma HLS STREAM variable=fifo_cout_1_PE_82 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_82 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_83;
  #pragma HLS STREAM variable=fifo_cout_1_PE_83 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_83 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_84;
  #pragma HLS STREAM variable=fifo_cout_1_PE_84 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_84 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_85;
  #pragma HLS STREAM variable=fifo_cout_1_PE_85 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_85 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_86;
  #pragma HLS STREAM variable=fifo_cout_1_PE_86 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_86 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_87;
  #pragma HLS STREAM variable=fifo_cout_1_PE_87 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_87 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_88;
  #pragma HLS STREAM variable=fifo_cout_1_PE_88 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_88 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_89;
  #pragma HLS STREAM variable=fifo_cout_1_PE_89 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_89 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_90;
  #pragma HLS STREAM variable=fifo_cout_1_PE_90 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_90 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_91;
  #pragma HLS STREAM variable=fifo_cout_1_PE_91 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_91 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_92;
  #pragma HLS STREAM variable=fifo_cout_1_PE_92 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_92 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_93;
  #pragma HLS STREAM variable=fifo_cout_1_PE_93 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_93 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_94;
  #pragma HLS STREAM variable=fifo_cout_1_PE_94 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_94 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_95;
  #pragma HLS STREAM variable=fifo_cout_1_PE_95 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_95 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_1_PE_96;
  #pragma HLS STREAM variable=fifo_cout_1_PE_96 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_1_PE_96 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_0;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_1;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_2;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_3;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_4;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_5;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_6;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_7;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_8;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_9;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_10;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_11;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_12;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_13;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_14;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_15;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_16;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_17;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_18;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_19;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_20;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_21;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_22;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_23;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_24;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_25;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_26;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_27;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_28;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_29;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_30;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_31;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_32;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_32 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_32 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_33;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_33 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_33 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_34;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_34 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_34 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_35;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_35 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_35 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_36;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_36 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_36 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_37;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_37 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_37 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_38;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_38 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_38 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_39;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_39 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_39 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_40;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_40 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_40 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_41;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_41 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_41 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_42;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_42 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_42 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_43;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_43 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_43 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_44;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_44 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_44 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_45;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_45 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_45 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_46;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_46 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_46 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_47;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_47 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_47 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_48;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_48 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_48 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_49;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_49 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_49 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_50;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_50 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_50 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_51;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_51 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_51 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_52;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_52 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_52 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_53;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_53 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_53 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_54;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_54 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_54 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_55;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_55 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_55 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_56;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_56 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_56 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_57;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_57 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_57 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_58;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_58 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_58 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_59;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_59 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_59 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_60;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_60 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_60 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_61;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_61 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_61 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_62;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_62 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_62 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_63;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_63 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_63 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_64;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_64 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_64 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_65;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_65 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_65 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_66;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_66 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_66 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_67;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_67 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_67 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_68;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_68 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_68 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_69;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_69 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_69 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_70;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_70 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_70 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_71;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_71 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_71 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_72;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_72 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_72 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_73;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_73 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_73 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_74;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_74 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_74 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_75;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_75 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_75 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_76;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_76 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_76 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_77;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_77 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_77 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_78;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_78 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_78 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_79;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_79 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_79 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_80;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_80 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_80 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_81;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_81 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_81 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_82;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_82 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_82 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_83;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_83 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_83 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_84;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_84 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_84 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_85;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_85 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_85 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_86;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_86 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_86 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_87;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_87 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_87 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_88;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_88 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_88 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_89;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_89 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_89 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_90;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_90 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_90 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_91;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_91 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_91 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_92;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_92 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_92 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_93;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_93 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_93 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_94;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_94 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_94 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_cout_drain_PE_95;
  #pragma HLS STREAM variable=fifo_cout_drain_PE_95 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_PE_95 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_0;
  #pragma HLS STREAM variable=fifo_w_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_1;
  #pragma HLS STREAM variable=fifo_w_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_2;
  #pragma HLS STREAM variable=fifo_w_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_3;
  #pragma HLS STREAM variable=fifo_w_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_4;
  #pragma HLS STREAM variable=fifo_w_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_5;
  #pragma HLS STREAM variable=fifo_w_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_6;
  #pragma HLS STREAM variable=fifo_w_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_7;
  #pragma HLS STREAM variable=fifo_w_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_8;
  #pragma HLS STREAM variable=fifo_w_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_9;
  #pragma HLS STREAM variable=fifo_w_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_10;
  #pragma HLS STREAM variable=fifo_w_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_11;
  #pragma HLS STREAM variable=fifo_w_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_12;
  #pragma HLS STREAM variable=fifo_w_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_13;
  #pragma HLS STREAM variable=fifo_w_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_14;
  #pragma HLS STREAM variable=fifo_w_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_15;
  #pragma HLS STREAM variable=fifo_w_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_16;
  #pragma HLS STREAM variable=fifo_w_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_17;
  #pragma HLS STREAM variable=fifo_w_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_18;
  #pragma HLS STREAM variable=fifo_w_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_19;
  #pragma HLS STREAM variable=fifo_w_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_20;
  #pragma HLS STREAM variable=fifo_w_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_21;
  #pragma HLS STREAM variable=fifo_w_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_22;
  #pragma HLS STREAM variable=fifo_w_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_23;
  #pragma HLS STREAM variable=fifo_w_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_24;
  #pragma HLS STREAM variable=fifo_w_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_25;
  #pragma HLS STREAM variable=fifo_w_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_26;
  #pragma HLS STREAM variable=fifo_w_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_27;
  #pragma HLS STREAM variable=fifo_w_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_28;
  #pragma HLS STREAM variable=fifo_w_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_29;
  #pragma HLS STREAM variable=fifo_w_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_30;
  #pragma HLS STREAM variable=fifo_w_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_31;
  #pragma HLS STREAM variable=fifo_w_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_32;
  #pragma HLS STREAM variable=fifo_w_PE_32 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_32 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_33;
  #pragma HLS STREAM variable=fifo_w_PE_33 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_33 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_34;
  #pragma HLS STREAM variable=fifo_w_PE_34 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_34 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_35;
  #pragma HLS STREAM variable=fifo_w_PE_35 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_35 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_36;
  #pragma HLS STREAM variable=fifo_w_PE_36 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_36 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_37;
  #pragma HLS STREAM variable=fifo_w_PE_37 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_37 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_38;
  #pragma HLS STREAM variable=fifo_w_PE_38 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_38 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_39;
  #pragma HLS STREAM variable=fifo_w_PE_39 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_39 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_40;
  #pragma HLS STREAM variable=fifo_w_PE_40 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_40 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_41;
  #pragma HLS STREAM variable=fifo_w_PE_41 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_41 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_42;
  #pragma HLS STREAM variable=fifo_w_PE_42 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_42 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_43;
  #pragma HLS STREAM variable=fifo_w_PE_43 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_43 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_44;
  #pragma HLS STREAM variable=fifo_w_PE_44 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_44 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_45;
  #pragma HLS STREAM variable=fifo_w_PE_45 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_45 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_46;
  #pragma HLS STREAM variable=fifo_w_PE_46 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_46 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_47;
  #pragma HLS STREAM variable=fifo_w_PE_47 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_47 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_48;
  #pragma HLS STREAM variable=fifo_w_PE_48 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_48 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_49;
  #pragma HLS STREAM variable=fifo_w_PE_49 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_49 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_50;
  #pragma HLS STREAM variable=fifo_w_PE_50 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_50 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_51;
  #pragma HLS STREAM variable=fifo_w_PE_51 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_51 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_52;
  #pragma HLS STREAM variable=fifo_w_PE_52 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_52 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_53;
  #pragma HLS STREAM variable=fifo_w_PE_53 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_53 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_54;
  #pragma HLS STREAM variable=fifo_w_PE_54 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_54 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_55;
  #pragma HLS STREAM variable=fifo_w_PE_55 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_55 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_56;
  #pragma HLS STREAM variable=fifo_w_PE_56 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_56 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_57;
  #pragma HLS STREAM variable=fifo_w_PE_57 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_57 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_58;
  #pragma HLS STREAM variable=fifo_w_PE_58 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_58 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_59;
  #pragma HLS STREAM variable=fifo_w_PE_59 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_59 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_60;
  #pragma HLS STREAM variable=fifo_w_PE_60 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_60 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_61;
  #pragma HLS STREAM variable=fifo_w_PE_61 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_61 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_62;
  #pragma HLS STREAM variable=fifo_w_PE_62 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_62 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_63;
  #pragma HLS STREAM variable=fifo_w_PE_63 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_63 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_64;
  #pragma HLS STREAM variable=fifo_w_PE_64 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_64 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_65;
  #pragma HLS STREAM variable=fifo_w_PE_65 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_65 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_66;
  #pragma HLS STREAM variable=fifo_w_PE_66 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_66 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_67;
  #pragma HLS STREAM variable=fifo_w_PE_67 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_67 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_68;
  #pragma HLS STREAM variable=fifo_w_PE_68 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_68 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_69;
  #pragma HLS STREAM variable=fifo_w_PE_69 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_69 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_70;
  #pragma HLS STREAM variable=fifo_w_PE_70 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_70 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_71;
  #pragma HLS STREAM variable=fifo_w_PE_71 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_71 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_72;
  #pragma HLS STREAM variable=fifo_w_PE_72 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_72 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_73;
  #pragma HLS STREAM variable=fifo_w_PE_73 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_73 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_74;
  #pragma HLS STREAM variable=fifo_w_PE_74 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_74 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_75;
  #pragma HLS STREAM variable=fifo_w_PE_75 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_75 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_76;
  #pragma HLS STREAM variable=fifo_w_PE_76 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_76 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_77;
  #pragma HLS STREAM variable=fifo_w_PE_77 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_77 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_78;
  #pragma HLS STREAM variable=fifo_w_PE_78 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_78 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_79;
  #pragma HLS STREAM variable=fifo_w_PE_79 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_79 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_80;
  #pragma HLS STREAM variable=fifo_w_PE_80 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_80 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_81;
  #pragma HLS STREAM variable=fifo_w_PE_81 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_81 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_82;
  #pragma HLS STREAM variable=fifo_w_PE_82 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_82 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_83;
  #pragma HLS STREAM variable=fifo_w_PE_83 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_83 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_84;
  #pragma HLS STREAM variable=fifo_w_PE_84 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_84 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_85;
  #pragma HLS STREAM variable=fifo_w_PE_85 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_85 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_86;
  #pragma HLS STREAM variable=fifo_w_PE_86 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_86 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_87;
  #pragma HLS STREAM variable=fifo_w_PE_87 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_87 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_88;
  #pragma HLS STREAM variable=fifo_w_PE_88 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_88 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_89;
  #pragma HLS STREAM variable=fifo_w_PE_89 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_89 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_90;
  #pragma HLS STREAM variable=fifo_w_PE_90 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_90 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_91;
  #pragma HLS STREAM variable=fifo_w_PE_91 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_91 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_92;
  #pragma HLS STREAM variable=fifo_w_PE_92 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_92 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_93;
  #pragma HLS STREAM variable=fifo_w_PE_93 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_93 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_94;
  #pragma HLS STREAM variable=fifo_w_PE_94 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_94 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_w_PE_95;
  #pragma HLS STREAM variable=fifo_w_PE_95 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_95 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_0;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_0 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_1;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_1 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_2;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_2 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_3;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_3 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_4;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_4 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_5;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_5 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_6;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_6 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_7;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_7 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_8;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_8 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_8 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_9;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_9 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_9 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_10;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_10 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_10 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_11;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_11 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_11 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_12;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_12 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_12 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_13;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_13 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_13 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_14;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_14 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_14 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_15;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_15 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_15 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_16;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_16 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_16 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_17;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_17 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_17 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_18;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_18 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_18 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_19;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_19 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_19 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_20;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_20 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_20 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_21;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_21 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_21 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_22;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_22 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_22 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_23;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_23 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_23 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_24;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_24 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_24 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_25;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_25 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_25 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_26;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_26 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_26 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_27;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_27 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_27 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_28;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_28 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_28 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_29;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_29 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_29 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_30;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_30 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_30 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_31;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_31 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_31 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_32;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_32 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_32 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_33;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_33 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_33 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_34;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_34 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_34 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_35;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_35 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_35 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_36;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_36 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_36 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_37;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_37 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_37 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_38;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_38 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_38 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_39;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_39 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_39 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_40;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_40 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_40 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_41;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_41 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_41 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_42;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_42 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_42 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_43;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_43 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_43 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_44;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_44 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_44 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_45;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_45 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_45 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_46;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_46 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_46 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_47;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_47 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_47 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_48;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_48 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_48 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_49;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_49 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_49 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_50;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_50 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_50 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_51;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_51 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_51 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_52;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_52 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_52 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_53;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_53 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_53 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_54;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_54 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_54 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_55;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_55 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_55 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_56;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_56 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_56 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_57;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_57 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_57 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_58;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_58 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_58 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_59;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_59 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_59 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_60;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_60 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_60 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_61;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_61 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_61 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_62;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_62 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_62 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_63;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_63 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_63 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_64;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_64 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_64 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_65;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_65 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_65 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_66;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_66 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_66 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_67;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_67 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_67 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_68;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_68 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_68 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_69;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_69 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_69 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_70;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_70 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_70 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_71;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_71 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_71 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_72;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_72 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_72 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_73;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_73 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_73 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_74;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_74 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_74 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_75;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_75 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_75 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_76;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_76 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_76 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_77;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_77 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_77 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_78;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_78 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_78 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_79;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_79 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_79 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_80;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_80 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_80 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_81;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_81 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_81 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_82;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_82 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_82 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_83;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_83 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_83 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_84;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_84 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_84 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_85;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_85 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_85 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_86;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_86 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_86 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_87;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_87 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_87 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_88;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_88 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_88 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_89;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_89 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_89 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_90;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_90 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_90 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_91;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_91 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_91 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_92;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_92 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_92 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_93;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_93 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_93 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_94;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_94 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_94 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_95;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_95 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_95 core=FIFO_SRL
  /* cout_drain_IO_L1_out fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L1_out_96;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L1_out_96 depth=2
  #pragma HLS RESOURCE variable=fifo_cout_drain_cout_drain_IO_L1_out_96 core=FIFO_SRL
  /* FIFO Declaration */

  /* Module Call */
  cin_IO_L2_in(
    /* array */ cin,
    /* fifo */ fifo_cin_cin_IO_L1_in_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_cin_cin_IO_L1_in_0,
    /* fifo */ fifo_cin_cin_IO_L1_in_1,
    /* fifo */ fifo_cin_PE_0
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_cin_cin_IO_L1_in_1,
    /* fifo */ fifo_cin_cin_IO_L1_in_2,
    /* fifo */ fifo_cin_PE_1
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_cin_cin_IO_L1_in_2,
    /* fifo */ fifo_cin_cin_IO_L1_in_3,
    /* fifo */ fifo_cin_PE_2
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_cin_cin_IO_L1_in_3,
    /* fifo */ fifo_cin_cin_IO_L1_in_4,
    /* fifo */ fifo_cin_PE_3
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_cin_cin_IO_L1_in_4,
    /* fifo */ fifo_cin_cin_IO_L1_in_5,
    /* fifo */ fifo_cin_PE_4
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_cin_cin_IO_L1_in_5,
    /* fifo */ fifo_cin_cin_IO_L1_in_6,
    /* fifo */ fifo_cin_PE_5
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_cin_cin_IO_L1_in_6,
    /* fifo */ fifo_cin_cin_IO_L1_in_7,
    /* fifo */ fifo_cin_PE_6
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_cin_cin_IO_L1_in_7,
    /* fifo */ fifo_cin_cin_IO_L1_in_8,
    /* fifo */ fifo_cin_PE_7
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_cin_cin_IO_L1_in_8,
    /* fifo */ fifo_cin_cin_IO_L1_in_9,
    /* fifo */ fifo_cin_PE_8
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_cin_cin_IO_L1_in_9,
    /* fifo */ fifo_cin_cin_IO_L1_in_10,
    /* fifo */ fifo_cin_PE_9
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_cin_cin_IO_L1_in_10,
    /* fifo */ fifo_cin_cin_IO_L1_in_11,
    /* fifo */ fifo_cin_PE_10
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_cin_cin_IO_L1_in_11,
    /* fifo */ fifo_cin_cin_IO_L1_in_12,
    /* fifo */ fifo_cin_PE_11
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_cin_cin_IO_L1_in_12,
    /* fifo */ fifo_cin_cin_IO_L1_in_13,
    /* fifo */ fifo_cin_PE_12
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_cin_cin_IO_L1_in_13,
    /* fifo */ fifo_cin_cin_IO_L1_in_14,
    /* fifo */ fifo_cin_PE_13
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_cin_cin_IO_L1_in_14,
    /* fifo */ fifo_cin_cin_IO_L1_in_15,
    /* fifo */ fifo_cin_PE_14
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_cin_cin_IO_L1_in_15,
    /* fifo */ fifo_cin_cin_IO_L1_in_16,
    /* fifo */ fifo_cin_PE_15
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 16,
    /* fifo */ fifo_cin_cin_IO_L1_in_16,
    /* fifo */ fifo_cin_cin_IO_L1_in_17,
    /* fifo */ fifo_cin_PE_16
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 17,
    /* fifo */ fifo_cin_cin_IO_L1_in_17,
    /* fifo */ fifo_cin_cin_IO_L1_in_18,
    /* fifo */ fifo_cin_PE_17
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 18,
    /* fifo */ fifo_cin_cin_IO_L1_in_18,
    /* fifo */ fifo_cin_cin_IO_L1_in_19,
    /* fifo */ fifo_cin_PE_18
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 19,
    /* fifo */ fifo_cin_cin_IO_L1_in_19,
    /* fifo */ fifo_cin_cin_IO_L1_in_20,
    /* fifo */ fifo_cin_PE_19
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 20,
    /* fifo */ fifo_cin_cin_IO_L1_in_20,
    /* fifo */ fifo_cin_cin_IO_L1_in_21,
    /* fifo */ fifo_cin_PE_20
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 21,
    /* fifo */ fifo_cin_cin_IO_L1_in_21,
    /* fifo */ fifo_cin_cin_IO_L1_in_22,
    /* fifo */ fifo_cin_PE_21
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 22,
    /* fifo */ fifo_cin_cin_IO_L1_in_22,
    /* fifo */ fifo_cin_cin_IO_L1_in_23,
    /* fifo */ fifo_cin_PE_22
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 23,
    /* fifo */ fifo_cin_cin_IO_L1_in_23,
    /* fifo */ fifo_cin_cin_IO_L1_in_24,
    /* fifo */ fifo_cin_PE_23
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 24,
    /* fifo */ fifo_cin_cin_IO_L1_in_24,
    /* fifo */ fifo_cin_cin_IO_L1_in_25,
    /* fifo */ fifo_cin_PE_24
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 25,
    /* fifo */ fifo_cin_cin_IO_L1_in_25,
    /* fifo */ fifo_cin_cin_IO_L1_in_26,
    /* fifo */ fifo_cin_PE_25
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 26,
    /* fifo */ fifo_cin_cin_IO_L1_in_26,
    /* fifo */ fifo_cin_cin_IO_L1_in_27,
    /* fifo */ fifo_cin_PE_26
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 27,
    /* fifo */ fifo_cin_cin_IO_L1_in_27,
    /* fifo */ fifo_cin_cin_IO_L1_in_28,
    /* fifo */ fifo_cin_PE_27
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 28,
    /* fifo */ fifo_cin_cin_IO_L1_in_28,
    /* fifo */ fifo_cin_cin_IO_L1_in_29,
    /* fifo */ fifo_cin_PE_28
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 29,
    /* fifo */ fifo_cin_cin_IO_L1_in_29,
    /* fifo */ fifo_cin_cin_IO_L1_in_30,
    /* fifo */ fifo_cin_PE_29
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 30,
    /* fifo */ fifo_cin_cin_IO_L1_in_30,
    /* fifo */ fifo_cin_cin_IO_L1_in_31,
    /* fifo */ fifo_cin_PE_30
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 31,
    /* fifo */ fifo_cin_cin_IO_L1_in_31,
    /* fifo */ fifo_cin_cin_IO_L1_in_32,
    /* fifo */ fifo_cin_PE_31
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 32,
    /* fifo */ fifo_cin_cin_IO_L1_in_32,
    /* fifo */ fifo_cin_cin_IO_L1_in_33,
    /* fifo */ fifo_cin_PE_32
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 33,
    /* fifo */ fifo_cin_cin_IO_L1_in_33,
    /* fifo */ fifo_cin_cin_IO_L1_in_34,
    /* fifo */ fifo_cin_PE_33
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 34,
    /* fifo */ fifo_cin_cin_IO_L1_in_34,
    /* fifo */ fifo_cin_cin_IO_L1_in_35,
    /* fifo */ fifo_cin_PE_34
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 35,
    /* fifo */ fifo_cin_cin_IO_L1_in_35,
    /* fifo */ fifo_cin_cin_IO_L1_in_36,
    /* fifo */ fifo_cin_PE_35
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 36,
    /* fifo */ fifo_cin_cin_IO_L1_in_36,
    /* fifo */ fifo_cin_cin_IO_L1_in_37,
    /* fifo */ fifo_cin_PE_36
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 37,
    /* fifo */ fifo_cin_cin_IO_L1_in_37,
    /* fifo */ fifo_cin_cin_IO_L1_in_38,
    /* fifo */ fifo_cin_PE_37
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 38,
    /* fifo */ fifo_cin_cin_IO_L1_in_38,
    /* fifo */ fifo_cin_cin_IO_L1_in_39,
    /* fifo */ fifo_cin_PE_38
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 39,
    /* fifo */ fifo_cin_cin_IO_L1_in_39,
    /* fifo */ fifo_cin_cin_IO_L1_in_40,
    /* fifo */ fifo_cin_PE_39
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 40,
    /* fifo */ fifo_cin_cin_IO_L1_in_40,
    /* fifo */ fifo_cin_cin_IO_L1_in_41,
    /* fifo */ fifo_cin_PE_40
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 41,
    /* fifo */ fifo_cin_cin_IO_L1_in_41,
    /* fifo */ fifo_cin_cin_IO_L1_in_42,
    /* fifo */ fifo_cin_PE_41
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 42,
    /* fifo */ fifo_cin_cin_IO_L1_in_42,
    /* fifo */ fifo_cin_cin_IO_L1_in_43,
    /* fifo */ fifo_cin_PE_42
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 43,
    /* fifo */ fifo_cin_cin_IO_L1_in_43,
    /* fifo */ fifo_cin_cin_IO_L1_in_44,
    /* fifo */ fifo_cin_PE_43
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 44,
    /* fifo */ fifo_cin_cin_IO_L1_in_44,
    /* fifo */ fifo_cin_cin_IO_L1_in_45,
    /* fifo */ fifo_cin_PE_44
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 45,
    /* fifo */ fifo_cin_cin_IO_L1_in_45,
    /* fifo */ fifo_cin_cin_IO_L1_in_46,
    /* fifo */ fifo_cin_PE_45
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 46,
    /* fifo */ fifo_cin_cin_IO_L1_in_46,
    /* fifo */ fifo_cin_cin_IO_L1_in_47,
    /* fifo */ fifo_cin_PE_46
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 47,
    /* fifo */ fifo_cin_cin_IO_L1_in_47,
    /* fifo */ fifo_cin_cin_IO_L1_in_48,
    /* fifo */ fifo_cin_PE_47
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 48,
    /* fifo */ fifo_cin_cin_IO_L1_in_48,
    /* fifo */ fifo_cin_cin_IO_L1_in_49,
    /* fifo */ fifo_cin_PE_48
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 49,
    /* fifo */ fifo_cin_cin_IO_L1_in_49,
    /* fifo */ fifo_cin_cin_IO_L1_in_50,
    /* fifo */ fifo_cin_PE_49
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 50,
    /* fifo */ fifo_cin_cin_IO_L1_in_50,
    /* fifo */ fifo_cin_cin_IO_L1_in_51,
    /* fifo */ fifo_cin_PE_50
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 51,
    /* fifo */ fifo_cin_cin_IO_L1_in_51,
    /* fifo */ fifo_cin_cin_IO_L1_in_52,
    /* fifo */ fifo_cin_PE_51
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 52,
    /* fifo */ fifo_cin_cin_IO_L1_in_52,
    /* fifo */ fifo_cin_cin_IO_L1_in_53,
    /* fifo */ fifo_cin_PE_52
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 53,
    /* fifo */ fifo_cin_cin_IO_L1_in_53,
    /* fifo */ fifo_cin_cin_IO_L1_in_54,
    /* fifo */ fifo_cin_PE_53
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 54,
    /* fifo */ fifo_cin_cin_IO_L1_in_54,
    /* fifo */ fifo_cin_cin_IO_L1_in_55,
    /* fifo */ fifo_cin_PE_54
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 55,
    /* fifo */ fifo_cin_cin_IO_L1_in_55,
    /* fifo */ fifo_cin_cin_IO_L1_in_56,
    /* fifo */ fifo_cin_PE_55
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 56,
    /* fifo */ fifo_cin_cin_IO_L1_in_56,
    /* fifo */ fifo_cin_cin_IO_L1_in_57,
    /* fifo */ fifo_cin_PE_56
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 57,
    /* fifo */ fifo_cin_cin_IO_L1_in_57,
    /* fifo */ fifo_cin_cin_IO_L1_in_58,
    /* fifo */ fifo_cin_PE_57
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 58,
    /* fifo */ fifo_cin_cin_IO_L1_in_58,
    /* fifo */ fifo_cin_cin_IO_L1_in_59,
    /* fifo */ fifo_cin_PE_58
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 59,
    /* fifo */ fifo_cin_cin_IO_L1_in_59,
    /* fifo */ fifo_cin_cin_IO_L1_in_60,
    /* fifo */ fifo_cin_PE_59
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 60,
    /* fifo */ fifo_cin_cin_IO_L1_in_60,
    /* fifo */ fifo_cin_cin_IO_L1_in_61,
    /* fifo */ fifo_cin_PE_60
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 61,
    /* fifo */ fifo_cin_cin_IO_L1_in_61,
    /* fifo */ fifo_cin_cin_IO_L1_in_62,
    /* fifo */ fifo_cin_PE_61
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 62,
    /* fifo */ fifo_cin_cin_IO_L1_in_62,
    /* fifo */ fifo_cin_cin_IO_L1_in_63,
    /* fifo */ fifo_cin_PE_62
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 63,
    /* fifo */ fifo_cin_cin_IO_L1_in_63,
    /* fifo */ fifo_cin_cin_IO_L1_in_64,
    /* fifo */ fifo_cin_PE_63
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 64,
    /* fifo */ fifo_cin_cin_IO_L1_in_64,
    /* fifo */ fifo_cin_cin_IO_L1_in_65,
    /* fifo */ fifo_cin_PE_64
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 65,
    /* fifo */ fifo_cin_cin_IO_L1_in_65,
    /* fifo */ fifo_cin_cin_IO_L1_in_66,
    /* fifo */ fifo_cin_PE_65
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 66,
    /* fifo */ fifo_cin_cin_IO_L1_in_66,
    /* fifo */ fifo_cin_cin_IO_L1_in_67,
    /* fifo */ fifo_cin_PE_66
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 67,
    /* fifo */ fifo_cin_cin_IO_L1_in_67,
    /* fifo */ fifo_cin_cin_IO_L1_in_68,
    /* fifo */ fifo_cin_PE_67
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 68,
    /* fifo */ fifo_cin_cin_IO_L1_in_68,
    /* fifo */ fifo_cin_cin_IO_L1_in_69,
    /* fifo */ fifo_cin_PE_68
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 69,
    /* fifo */ fifo_cin_cin_IO_L1_in_69,
    /* fifo */ fifo_cin_cin_IO_L1_in_70,
    /* fifo */ fifo_cin_PE_69
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 70,
    /* fifo */ fifo_cin_cin_IO_L1_in_70,
    /* fifo */ fifo_cin_cin_IO_L1_in_71,
    /* fifo */ fifo_cin_PE_70
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 71,
    /* fifo */ fifo_cin_cin_IO_L1_in_71,
    /* fifo */ fifo_cin_cin_IO_L1_in_72,
    /* fifo */ fifo_cin_PE_71
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 72,
    /* fifo */ fifo_cin_cin_IO_L1_in_72,
    /* fifo */ fifo_cin_cin_IO_L1_in_73,
    /* fifo */ fifo_cin_PE_72
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 73,
    /* fifo */ fifo_cin_cin_IO_L1_in_73,
    /* fifo */ fifo_cin_cin_IO_L1_in_74,
    /* fifo */ fifo_cin_PE_73
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 74,
    /* fifo */ fifo_cin_cin_IO_L1_in_74,
    /* fifo */ fifo_cin_cin_IO_L1_in_75,
    /* fifo */ fifo_cin_PE_74
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 75,
    /* fifo */ fifo_cin_cin_IO_L1_in_75,
    /* fifo */ fifo_cin_cin_IO_L1_in_76,
    /* fifo */ fifo_cin_PE_75
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 76,
    /* fifo */ fifo_cin_cin_IO_L1_in_76,
    /* fifo */ fifo_cin_cin_IO_L1_in_77,
    /* fifo */ fifo_cin_PE_76
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 77,
    /* fifo */ fifo_cin_cin_IO_L1_in_77,
    /* fifo */ fifo_cin_cin_IO_L1_in_78,
    /* fifo */ fifo_cin_PE_77
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 78,
    /* fifo */ fifo_cin_cin_IO_L1_in_78,
    /* fifo */ fifo_cin_cin_IO_L1_in_79,
    /* fifo */ fifo_cin_PE_78
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 79,
    /* fifo */ fifo_cin_cin_IO_L1_in_79,
    /* fifo */ fifo_cin_cin_IO_L1_in_80,
    /* fifo */ fifo_cin_PE_79
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 80,
    /* fifo */ fifo_cin_cin_IO_L1_in_80,
    /* fifo */ fifo_cin_cin_IO_L1_in_81,
    /* fifo */ fifo_cin_PE_80
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 81,
    /* fifo */ fifo_cin_cin_IO_L1_in_81,
    /* fifo */ fifo_cin_cin_IO_L1_in_82,
    /* fifo */ fifo_cin_PE_81
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 82,
    /* fifo */ fifo_cin_cin_IO_L1_in_82,
    /* fifo */ fifo_cin_cin_IO_L1_in_83,
    /* fifo */ fifo_cin_PE_82
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 83,
    /* fifo */ fifo_cin_cin_IO_L1_in_83,
    /* fifo */ fifo_cin_cin_IO_L1_in_84,
    /* fifo */ fifo_cin_PE_83
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 84,
    /* fifo */ fifo_cin_cin_IO_L1_in_84,
    /* fifo */ fifo_cin_cin_IO_L1_in_85,
    /* fifo */ fifo_cin_PE_84
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 85,
    /* fifo */ fifo_cin_cin_IO_L1_in_85,
    /* fifo */ fifo_cin_cin_IO_L1_in_86,
    /* fifo */ fifo_cin_PE_85
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 86,
    /* fifo */ fifo_cin_cin_IO_L1_in_86,
    /* fifo */ fifo_cin_cin_IO_L1_in_87,
    /* fifo */ fifo_cin_PE_86
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 87,
    /* fifo */ fifo_cin_cin_IO_L1_in_87,
    /* fifo */ fifo_cin_cin_IO_L1_in_88,
    /* fifo */ fifo_cin_PE_87
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 88,
    /* fifo */ fifo_cin_cin_IO_L1_in_88,
    /* fifo */ fifo_cin_cin_IO_L1_in_89,
    /* fifo */ fifo_cin_PE_88
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 89,
    /* fifo */ fifo_cin_cin_IO_L1_in_89,
    /* fifo */ fifo_cin_cin_IO_L1_in_90,
    /* fifo */ fifo_cin_PE_89
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 90,
    /* fifo */ fifo_cin_cin_IO_L1_in_90,
    /* fifo */ fifo_cin_cin_IO_L1_in_91,
    /* fifo */ fifo_cin_PE_90
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 91,
    /* fifo */ fifo_cin_cin_IO_L1_in_91,
    /* fifo */ fifo_cin_cin_IO_L1_in_92,
    /* fifo */ fifo_cin_PE_91
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 92,
    /* fifo */ fifo_cin_cin_IO_L1_in_92,
    /* fifo */ fifo_cin_cin_IO_L1_in_93,
    /* fifo */ fifo_cin_PE_92
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 93,
    /* fifo */ fifo_cin_cin_IO_L1_in_93,
    /* fifo */ fifo_cin_cin_IO_L1_in_94,
    /* fifo */ fifo_cin_PE_93
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_wrapper(
    /* module id */ 94,
    /* fifo */ fifo_cin_cin_IO_L1_in_94,
    /* fifo */ fifo_cin_cin_IO_L1_in_95,
    /* fifo */ fifo_cin_PE_94
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L1_in_boundary_wrapper(
    /* module id */ 95,
    /* fifo */ fifo_cin_cin_IO_L1_in_95,
    /* fifo */ fifo_cin_PE_95
  );
  /* Module Call */

  /* Module Call */
  cout_1_IO_L2_in_boundary(
    /* array */ cout_0,
    /* fifo */ fifo_cout_1_PE_0
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in(
    /* array */ w,
    /* fifo */ fifo_w_w_IO_L1_in_0
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_w_w_IO_L1_in_0,
    /* fifo */ fifo_w_w_IO_L1_in_1,
    /* fifo */ fifo_w_PE_0
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_w_w_IO_L1_in_1,
    /* fifo */ fifo_w_w_IO_L1_in_2,
    /* fifo */ fifo_w_PE_1
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_w_w_IO_L1_in_2,
    /* fifo */ fifo_w_w_IO_L1_in_3,
    /* fifo */ fifo_w_PE_2
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_w_w_IO_L1_in_3,
    /* fifo */ fifo_w_w_IO_L1_in_4,
    /* fifo */ fifo_w_PE_3
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_w_w_IO_L1_in_4,
    /* fifo */ fifo_w_w_IO_L1_in_5,
    /* fifo */ fifo_w_PE_4
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_w_w_IO_L1_in_5,
    /* fifo */ fifo_w_w_IO_L1_in_6,
    /* fifo */ fifo_w_PE_5
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_w_w_IO_L1_in_6,
    /* fifo */ fifo_w_w_IO_L1_in_7,
    /* fifo */ fifo_w_PE_6
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_w_w_IO_L1_in_7,
    /* fifo */ fifo_w_w_IO_L1_in_8,
    /* fifo */ fifo_w_PE_7
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_w_w_IO_L1_in_8,
    /* fifo */ fifo_w_w_IO_L1_in_9,
    /* fifo */ fifo_w_PE_8
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_w_w_IO_L1_in_9,
    /* fifo */ fifo_w_w_IO_L1_in_10,
    /* fifo */ fifo_w_PE_9
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_w_w_IO_L1_in_10,
    /* fifo */ fifo_w_w_IO_L1_in_11,
    /* fifo */ fifo_w_PE_10
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_w_w_IO_L1_in_11,
    /* fifo */ fifo_w_w_IO_L1_in_12,
    /* fifo */ fifo_w_PE_11
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_w_w_IO_L1_in_12,
    /* fifo */ fifo_w_w_IO_L1_in_13,
    /* fifo */ fifo_w_PE_12
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_w_w_IO_L1_in_13,
    /* fifo */ fifo_w_w_IO_L1_in_14,
    /* fifo */ fifo_w_PE_13
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_w_w_IO_L1_in_14,
    /* fifo */ fifo_w_w_IO_L1_in_15,
    /* fifo */ fifo_w_PE_14
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_w_w_IO_L1_in_15,
    /* fifo */ fifo_w_w_IO_L1_in_16,
    /* fifo */ fifo_w_PE_15
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 16,
    /* fifo */ fifo_w_w_IO_L1_in_16,
    /* fifo */ fifo_w_w_IO_L1_in_17,
    /* fifo */ fifo_w_PE_16
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 17,
    /* fifo */ fifo_w_w_IO_L1_in_17,
    /* fifo */ fifo_w_w_IO_L1_in_18,
    /* fifo */ fifo_w_PE_17
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 18,
    /* fifo */ fifo_w_w_IO_L1_in_18,
    /* fifo */ fifo_w_w_IO_L1_in_19,
    /* fifo */ fifo_w_PE_18
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 19,
    /* fifo */ fifo_w_w_IO_L1_in_19,
    /* fifo */ fifo_w_w_IO_L1_in_20,
    /* fifo */ fifo_w_PE_19
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 20,
    /* fifo */ fifo_w_w_IO_L1_in_20,
    /* fifo */ fifo_w_w_IO_L1_in_21,
    /* fifo */ fifo_w_PE_20
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 21,
    /* fifo */ fifo_w_w_IO_L1_in_21,
    /* fifo */ fifo_w_w_IO_L1_in_22,
    /* fifo */ fifo_w_PE_21
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 22,
    /* fifo */ fifo_w_w_IO_L1_in_22,
    /* fifo */ fifo_w_w_IO_L1_in_23,
    /* fifo */ fifo_w_PE_22
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 23,
    /* fifo */ fifo_w_w_IO_L1_in_23,
    /* fifo */ fifo_w_w_IO_L1_in_24,
    /* fifo */ fifo_w_PE_23
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 24,
    /* fifo */ fifo_w_w_IO_L1_in_24,
    /* fifo */ fifo_w_w_IO_L1_in_25,
    /* fifo */ fifo_w_PE_24
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 25,
    /* fifo */ fifo_w_w_IO_L1_in_25,
    /* fifo */ fifo_w_w_IO_L1_in_26,
    /* fifo */ fifo_w_PE_25
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 26,
    /* fifo */ fifo_w_w_IO_L1_in_26,
    /* fifo */ fifo_w_w_IO_L1_in_27,
    /* fifo */ fifo_w_PE_26
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 27,
    /* fifo */ fifo_w_w_IO_L1_in_27,
    /* fifo */ fifo_w_w_IO_L1_in_28,
    /* fifo */ fifo_w_PE_27
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 28,
    /* fifo */ fifo_w_w_IO_L1_in_28,
    /* fifo */ fifo_w_w_IO_L1_in_29,
    /* fifo */ fifo_w_PE_28
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 29,
    /* fifo */ fifo_w_w_IO_L1_in_29,
    /* fifo */ fifo_w_w_IO_L1_in_30,
    /* fifo */ fifo_w_PE_29
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 30,
    /* fifo */ fifo_w_w_IO_L1_in_30,
    /* fifo */ fifo_w_w_IO_L1_in_31,
    /* fifo */ fifo_w_PE_30
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 31,
    /* fifo */ fifo_w_w_IO_L1_in_31,
    /* fifo */ fifo_w_w_IO_L1_in_32,
    /* fifo */ fifo_w_PE_31
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 32,
    /* fifo */ fifo_w_w_IO_L1_in_32,
    /* fifo */ fifo_w_w_IO_L1_in_33,
    /* fifo */ fifo_w_PE_32
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 33,
    /* fifo */ fifo_w_w_IO_L1_in_33,
    /* fifo */ fifo_w_w_IO_L1_in_34,
    /* fifo */ fifo_w_PE_33
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 34,
    /* fifo */ fifo_w_w_IO_L1_in_34,
    /* fifo */ fifo_w_w_IO_L1_in_35,
    /* fifo */ fifo_w_PE_34
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 35,
    /* fifo */ fifo_w_w_IO_L1_in_35,
    /* fifo */ fifo_w_w_IO_L1_in_36,
    /* fifo */ fifo_w_PE_35
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 36,
    /* fifo */ fifo_w_w_IO_L1_in_36,
    /* fifo */ fifo_w_w_IO_L1_in_37,
    /* fifo */ fifo_w_PE_36
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 37,
    /* fifo */ fifo_w_w_IO_L1_in_37,
    /* fifo */ fifo_w_w_IO_L1_in_38,
    /* fifo */ fifo_w_PE_37
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 38,
    /* fifo */ fifo_w_w_IO_L1_in_38,
    /* fifo */ fifo_w_w_IO_L1_in_39,
    /* fifo */ fifo_w_PE_38
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 39,
    /* fifo */ fifo_w_w_IO_L1_in_39,
    /* fifo */ fifo_w_w_IO_L1_in_40,
    /* fifo */ fifo_w_PE_39
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 40,
    /* fifo */ fifo_w_w_IO_L1_in_40,
    /* fifo */ fifo_w_w_IO_L1_in_41,
    /* fifo */ fifo_w_PE_40
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 41,
    /* fifo */ fifo_w_w_IO_L1_in_41,
    /* fifo */ fifo_w_w_IO_L1_in_42,
    /* fifo */ fifo_w_PE_41
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 42,
    /* fifo */ fifo_w_w_IO_L1_in_42,
    /* fifo */ fifo_w_w_IO_L1_in_43,
    /* fifo */ fifo_w_PE_42
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 43,
    /* fifo */ fifo_w_w_IO_L1_in_43,
    /* fifo */ fifo_w_w_IO_L1_in_44,
    /* fifo */ fifo_w_PE_43
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 44,
    /* fifo */ fifo_w_w_IO_L1_in_44,
    /* fifo */ fifo_w_w_IO_L1_in_45,
    /* fifo */ fifo_w_PE_44
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 45,
    /* fifo */ fifo_w_w_IO_L1_in_45,
    /* fifo */ fifo_w_w_IO_L1_in_46,
    /* fifo */ fifo_w_PE_45
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 46,
    /* fifo */ fifo_w_w_IO_L1_in_46,
    /* fifo */ fifo_w_w_IO_L1_in_47,
    /* fifo */ fifo_w_PE_46
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 47,
    /* fifo */ fifo_w_w_IO_L1_in_47,
    /* fifo */ fifo_w_w_IO_L1_in_48,
    /* fifo */ fifo_w_PE_47
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 48,
    /* fifo */ fifo_w_w_IO_L1_in_48,
    /* fifo */ fifo_w_w_IO_L1_in_49,
    /* fifo */ fifo_w_PE_48
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 49,
    /* fifo */ fifo_w_w_IO_L1_in_49,
    /* fifo */ fifo_w_w_IO_L1_in_50,
    /* fifo */ fifo_w_PE_49
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 50,
    /* fifo */ fifo_w_w_IO_L1_in_50,
    /* fifo */ fifo_w_w_IO_L1_in_51,
    /* fifo */ fifo_w_PE_50
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 51,
    /* fifo */ fifo_w_w_IO_L1_in_51,
    /* fifo */ fifo_w_w_IO_L1_in_52,
    /* fifo */ fifo_w_PE_51
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 52,
    /* fifo */ fifo_w_w_IO_L1_in_52,
    /* fifo */ fifo_w_w_IO_L1_in_53,
    /* fifo */ fifo_w_PE_52
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 53,
    /* fifo */ fifo_w_w_IO_L1_in_53,
    /* fifo */ fifo_w_w_IO_L1_in_54,
    /* fifo */ fifo_w_PE_53
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 54,
    /* fifo */ fifo_w_w_IO_L1_in_54,
    /* fifo */ fifo_w_w_IO_L1_in_55,
    /* fifo */ fifo_w_PE_54
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 55,
    /* fifo */ fifo_w_w_IO_L1_in_55,
    /* fifo */ fifo_w_w_IO_L1_in_56,
    /* fifo */ fifo_w_PE_55
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 56,
    /* fifo */ fifo_w_w_IO_L1_in_56,
    /* fifo */ fifo_w_w_IO_L1_in_57,
    /* fifo */ fifo_w_PE_56
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 57,
    /* fifo */ fifo_w_w_IO_L1_in_57,
    /* fifo */ fifo_w_w_IO_L1_in_58,
    /* fifo */ fifo_w_PE_57
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 58,
    /* fifo */ fifo_w_w_IO_L1_in_58,
    /* fifo */ fifo_w_w_IO_L1_in_59,
    /* fifo */ fifo_w_PE_58
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 59,
    /* fifo */ fifo_w_w_IO_L1_in_59,
    /* fifo */ fifo_w_w_IO_L1_in_60,
    /* fifo */ fifo_w_PE_59
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 60,
    /* fifo */ fifo_w_w_IO_L1_in_60,
    /* fifo */ fifo_w_w_IO_L1_in_61,
    /* fifo */ fifo_w_PE_60
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 61,
    /* fifo */ fifo_w_w_IO_L1_in_61,
    /* fifo */ fifo_w_w_IO_L1_in_62,
    /* fifo */ fifo_w_PE_61
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 62,
    /* fifo */ fifo_w_w_IO_L1_in_62,
    /* fifo */ fifo_w_w_IO_L1_in_63,
    /* fifo */ fifo_w_PE_62
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 63,
    /* fifo */ fifo_w_w_IO_L1_in_63,
    /* fifo */ fifo_w_w_IO_L1_in_64,
    /* fifo */ fifo_w_PE_63
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 64,
    /* fifo */ fifo_w_w_IO_L1_in_64,
    /* fifo */ fifo_w_w_IO_L1_in_65,
    /* fifo */ fifo_w_PE_64
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 65,
    /* fifo */ fifo_w_w_IO_L1_in_65,
    /* fifo */ fifo_w_w_IO_L1_in_66,
    /* fifo */ fifo_w_PE_65
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 66,
    /* fifo */ fifo_w_w_IO_L1_in_66,
    /* fifo */ fifo_w_w_IO_L1_in_67,
    /* fifo */ fifo_w_PE_66
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 67,
    /* fifo */ fifo_w_w_IO_L1_in_67,
    /* fifo */ fifo_w_w_IO_L1_in_68,
    /* fifo */ fifo_w_PE_67
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 68,
    /* fifo */ fifo_w_w_IO_L1_in_68,
    /* fifo */ fifo_w_w_IO_L1_in_69,
    /* fifo */ fifo_w_PE_68
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 69,
    /* fifo */ fifo_w_w_IO_L1_in_69,
    /* fifo */ fifo_w_w_IO_L1_in_70,
    /* fifo */ fifo_w_PE_69
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 70,
    /* fifo */ fifo_w_w_IO_L1_in_70,
    /* fifo */ fifo_w_w_IO_L1_in_71,
    /* fifo */ fifo_w_PE_70
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 71,
    /* fifo */ fifo_w_w_IO_L1_in_71,
    /* fifo */ fifo_w_w_IO_L1_in_72,
    /* fifo */ fifo_w_PE_71
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 72,
    /* fifo */ fifo_w_w_IO_L1_in_72,
    /* fifo */ fifo_w_w_IO_L1_in_73,
    /* fifo */ fifo_w_PE_72
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 73,
    /* fifo */ fifo_w_w_IO_L1_in_73,
    /* fifo */ fifo_w_w_IO_L1_in_74,
    /* fifo */ fifo_w_PE_73
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 74,
    /* fifo */ fifo_w_w_IO_L1_in_74,
    /* fifo */ fifo_w_w_IO_L1_in_75,
    /* fifo */ fifo_w_PE_74
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 75,
    /* fifo */ fifo_w_w_IO_L1_in_75,
    /* fifo */ fifo_w_w_IO_L1_in_76,
    /* fifo */ fifo_w_PE_75
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 76,
    /* fifo */ fifo_w_w_IO_L1_in_76,
    /* fifo */ fifo_w_w_IO_L1_in_77,
    /* fifo */ fifo_w_PE_76
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 77,
    /* fifo */ fifo_w_w_IO_L1_in_77,
    /* fifo */ fifo_w_w_IO_L1_in_78,
    /* fifo */ fifo_w_PE_77
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 78,
    /* fifo */ fifo_w_w_IO_L1_in_78,
    /* fifo */ fifo_w_w_IO_L1_in_79,
    /* fifo */ fifo_w_PE_78
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 79,
    /* fifo */ fifo_w_w_IO_L1_in_79,
    /* fifo */ fifo_w_w_IO_L1_in_80,
    /* fifo */ fifo_w_PE_79
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 80,
    /* fifo */ fifo_w_w_IO_L1_in_80,
    /* fifo */ fifo_w_w_IO_L1_in_81,
    /* fifo */ fifo_w_PE_80
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 81,
    /* fifo */ fifo_w_w_IO_L1_in_81,
    /* fifo */ fifo_w_w_IO_L1_in_82,
    /* fifo */ fifo_w_PE_81
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 82,
    /* fifo */ fifo_w_w_IO_L1_in_82,
    /* fifo */ fifo_w_w_IO_L1_in_83,
    /* fifo */ fifo_w_PE_82
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 83,
    /* fifo */ fifo_w_w_IO_L1_in_83,
    /* fifo */ fifo_w_w_IO_L1_in_84,
    /* fifo */ fifo_w_PE_83
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 84,
    /* fifo */ fifo_w_w_IO_L1_in_84,
    /* fifo */ fifo_w_w_IO_L1_in_85,
    /* fifo */ fifo_w_PE_84
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 85,
    /* fifo */ fifo_w_w_IO_L1_in_85,
    /* fifo */ fifo_w_w_IO_L1_in_86,
    /* fifo */ fifo_w_PE_85
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 86,
    /* fifo */ fifo_w_w_IO_L1_in_86,
    /* fifo */ fifo_w_w_IO_L1_in_87,
    /* fifo */ fifo_w_PE_86
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 87,
    /* fifo */ fifo_w_w_IO_L1_in_87,
    /* fifo */ fifo_w_w_IO_L1_in_88,
    /* fifo */ fifo_w_PE_87
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 88,
    /* fifo */ fifo_w_w_IO_L1_in_88,
    /* fifo */ fifo_w_w_IO_L1_in_89,
    /* fifo */ fifo_w_PE_88
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 89,
    /* fifo */ fifo_w_w_IO_L1_in_89,
    /* fifo */ fifo_w_w_IO_L1_in_90,
    /* fifo */ fifo_w_PE_89
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 90,
    /* fifo */ fifo_w_w_IO_L1_in_90,
    /* fifo */ fifo_w_w_IO_L1_in_91,
    /* fifo */ fifo_w_PE_90
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 91,
    /* fifo */ fifo_w_w_IO_L1_in_91,
    /* fifo */ fifo_w_w_IO_L1_in_92,
    /* fifo */ fifo_w_PE_91
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 92,
    /* fifo */ fifo_w_w_IO_L1_in_92,
    /* fifo */ fifo_w_w_IO_L1_in_93,
    /* fifo */ fifo_w_PE_92
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 93,
    /* fifo */ fifo_w_w_IO_L1_in_93,
    /* fifo */ fifo_w_w_IO_L1_in_94,
    /* fifo */ fifo_w_PE_93
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_wrapper(
    /* module id */ 94,
    /* fifo */ fifo_w_w_IO_L1_in_94,
    /* fifo */ fifo_w_w_IO_L1_in_95,
    /* fifo */ fifo_w_PE_94
  );
  /* Module Call */

  /* Module Call */
  w_IO_L1_in_boundary_wrapper(
    /* module id */ 95,
    /* fifo */ fifo_w_w_IO_L1_in_95,
    /* fifo */ fifo_w_PE_95
  );
  /* Module Call */

  /* Module Call */
  cout_1_PE_dummy_out(
    /* module id */ 0,
    /* fifo */ fifo_cout_1_PE_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_cin_PE_0,
    /* fifo */ fifo_cout_1_PE_0,
    /* fifo */ fifo_cout_1_PE_1,
    /* fifo */ fifo_cout_drain_PE_0,
    /* fifo */ fifo_w_PE_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_cin_PE_1,
    /* fifo */ fifo_cout_1_PE_1,
    /* fifo */ fifo_cout_1_PE_2,
    /* fifo */ fifo_cout_drain_PE_1,
    /* fifo */ fifo_w_PE_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_cin_PE_2,
    /* fifo */ fifo_cout_1_PE_2,
    /* fifo */ fifo_cout_1_PE_3,
    /* fifo */ fifo_cout_drain_PE_2,
    /* fifo */ fifo_w_PE_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_cin_PE_3,
    /* fifo */ fifo_cout_1_PE_3,
    /* fifo */ fifo_cout_1_PE_4,
    /* fifo */ fifo_cout_drain_PE_3,
    /* fifo */ fifo_w_PE_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_cin_PE_4,
    /* fifo */ fifo_cout_1_PE_4,
    /* fifo */ fifo_cout_1_PE_5,
    /* fifo */ fifo_cout_drain_PE_4,
    /* fifo */ fifo_w_PE_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_cin_PE_5,
    /* fifo */ fifo_cout_1_PE_5,
    /* fifo */ fifo_cout_1_PE_6,
    /* fifo */ fifo_cout_drain_PE_5,
    /* fifo */ fifo_w_PE_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_cin_PE_6,
    /* fifo */ fifo_cout_1_PE_6,
    /* fifo */ fifo_cout_1_PE_7,
    /* fifo */ fifo_cout_drain_PE_6,
    /* fifo */ fifo_w_PE_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_cin_PE_7,
    /* fifo */ fifo_cout_1_PE_7,
    /* fifo */ fifo_cout_1_PE_8,
    /* fifo */ fifo_cout_drain_PE_7,
    /* fifo */ fifo_w_PE_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_cin_PE_8,
    /* fifo */ fifo_cout_1_PE_8,
    /* fifo */ fifo_cout_1_PE_9,
    /* fifo */ fifo_cout_drain_PE_8,
    /* fifo */ fifo_w_PE_8
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_cin_PE_9,
    /* fifo */ fifo_cout_1_PE_9,
    /* fifo */ fifo_cout_1_PE_10,
    /* fifo */ fifo_cout_drain_PE_9,
    /* fifo */ fifo_w_PE_9
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_cin_PE_10,
    /* fifo */ fifo_cout_1_PE_10,
    /* fifo */ fifo_cout_1_PE_11,
    /* fifo */ fifo_cout_drain_PE_10,
    /* fifo */ fifo_w_PE_10
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_cin_PE_11,
    /* fifo */ fifo_cout_1_PE_11,
    /* fifo */ fifo_cout_1_PE_12,
    /* fifo */ fifo_cout_drain_PE_11,
    /* fifo */ fifo_w_PE_11
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_cin_PE_12,
    /* fifo */ fifo_cout_1_PE_12,
    /* fifo */ fifo_cout_1_PE_13,
    /* fifo */ fifo_cout_drain_PE_12,
    /* fifo */ fifo_w_PE_12
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_cin_PE_13,
    /* fifo */ fifo_cout_1_PE_13,
    /* fifo */ fifo_cout_1_PE_14,
    /* fifo */ fifo_cout_drain_PE_13,
    /* fifo */ fifo_w_PE_13
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_cin_PE_14,
    /* fifo */ fifo_cout_1_PE_14,
    /* fifo */ fifo_cout_1_PE_15,
    /* fifo */ fifo_cout_drain_PE_14,
    /* fifo */ fifo_w_PE_14
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_cin_PE_15,
    /* fifo */ fifo_cout_1_PE_15,
    /* fifo */ fifo_cout_1_PE_16,
    /* fifo */ fifo_cout_drain_PE_15,
    /* fifo */ fifo_w_PE_15
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 16,
    /* fifo */ fifo_cin_PE_16,
    /* fifo */ fifo_cout_1_PE_16,
    /* fifo */ fifo_cout_1_PE_17,
    /* fifo */ fifo_cout_drain_PE_16,
    /* fifo */ fifo_w_PE_16
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 17,
    /* fifo */ fifo_cin_PE_17,
    /* fifo */ fifo_cout_1_PE_17,
    /* fifo */ fifo_cout_1_PE_18,
    /* fifo */ fifo_cout_drain_PE_17,
    /* fifo */ fifo_w_PE_17
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 18,
    /* fifo */ fifo_cin_PE_18,
    /* fifo */ fifo_cout_1_PE_18,
    /* fifo */ fifo_cout_1_PE_19,
    /* fifo */ fifo_cout_drain_PE_18,
    /* fifo */ fifo_w_PE_18
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 19,
    /* fifo */ fifo_cin_PE_19,
    /* fifo */ fifo_cout_1_PE_19,
    /* fifo */ fifo_cout_1_PE_20,
    /* fifo */ fifo_cout_drain_PE_19,
    /* fifo */ fifo_w_PE_19
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 20,
    /* fifo */ fifo_cin_PE_20,
    /* fifo */ fifo_cout_1_PE_20,
    /* fifo */ fifo_cout_1_PE_21,
    /* fifo */ fifo_cout_drain_PE_20,
    /* fifo */ fifo_w_PE_20
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 21,
    /* fifo */ fifo_cin_PE_21,
    /* fifo */ fifo_cout_1_PE_21,
    /* fifo */ fifo_cout_1_PE_22,
    /* fifo */ fifo_cout_drain_PE_21,
    /* fifo */ fifo_w_PE_21
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 22,
    /* fifo */ fifo_cin_PE_22,
    /* fifo */ fifo_cout_1_PE_22,
    /* fifo */ fifo_cout_1_PE_23,
    /* fifo */ fifo_cout_drain_PE_22,
    /* fifo */ fifo_w_PE_22
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 23,
    /* fifo */ fifo_cin_PE_23,
    /* fifo */ fifo_cout_1_PE_23,
    /* fifo */ fifo_cout_1_PE_24,
    /* fifo */ fifo_cout_drain_PE_23,
    /* fifo */ fifo_w_PE_23
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 24,
    /* fifo */ fifo_cin_PE_24,
    /* fifo */ fifo_cout_1_PE_24,
    /* fifo */ fifo_cout_1_PE_25,
    /* fifo */ fifo_cout_drain_PE_24,
    /* fifo */ fifo_w_PE_24
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 25,
    /* fifo */ fifo_cin_PE_25,
    /* fifo */ fifo_cout_1_PE_25,
    /* fifo */ fifo_cout_1_PE_26,
    /* fifo */ fifo_cout_drain_PE_25,
    /* fifo */ fifo_w_PE_25
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 26,
    /* fifo */ fifo_cin_PE_26,
    /* fifo */ fifo_cout_1_PE_26,
    /* fifo */ fifo_cout_1_PE_27,
    /* fifo */ fifo_cout_drain_PE_26,
    /* fifo */ fifo_w_PE_26
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 27,
    /* fifo */ fifo_cin_PE_27,
    /* fifo */ fifo_cout_1_PE_27,
    /* fifo */ fifo_cout_1_PE_28,
    /* fifo */ fifo_cout_drain_PE_27,
    /* fifo */ fifo_w_PE_27
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 28,
    /* fifo */ fifo_cin_PE_28,
    /* fifo */ fifo_cout_1_PE_28,
    /* fifo */ fifo_cout_1_PE_29,
    /* fifo */ fifo_cout_drain_PE_28,
    /* fifo */ fifo_w_PE_28
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 29,
    /* fifo */ fifo_cin_PE_29,
    /* fifo */ fifo_cout_1_PE_29,
    /* fifo */ fifo_cout_1_PE_30,
    /* fifo */ fifo_cout_drain_PE_29,
    /* fifo */ fifo_w_PE_29
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 30,
    /* fifo */ fifo_cin_PE_30,
    /* fifo */ fifo_cout_1_PE_30,
    /* fifo */ fifo_cout_1_PE_31,
    /* fifo */ fifo_cout_drain_PE_30,
    /* fifo */ fifo_w_PE_30
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 31,
    /* fifo */ fifo_cin_PE_31,
    /* fifo */ fifo_cout_1_PE_31,
    /* fifo */ fifo_cout_1_PE_32,
    /* fifo */ fifo_cout_drain_PE_31,
    /* fifo */ fifo_w_PE_31
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 32,
    /* fifo */ fifo_cin_PE_32,
    /* fifo */ fifo_cout_1_PE_32,
    /* fifo */ fifo_cout_1_PE_33,
    /* fifo */ fifo_cout_drain_PE_32,
    /* fifo */ fifo_w_PE_32
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 33,
    /* fifo */ fifo_cin_PE_33,
    /* fifo */ fifo_cout_1_PE_33,
    /* fifo */ fifo_cout_1_PE_34,
    /* fifo */ fifo_cout_drain_PE_33,
    /* fifo */ fifo_w_PE_33
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 34,
    /* fifo */ fifo_cin_PE_34,
    /* fifo */ fifo_cout_1_PE_34,
    /* fifo */ fifo_cout_1_PE_35,
    /* fifo */ fifo_cout_drain_PE_34,
    /* fifo */ fifo_w_PE_34
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 35,
    /* fifo */ fifo_cin_PE_35,
    /* fifo */ fifo_cout_1_PE_35,
    /* fifo */ fifo_cout_1_PE_36,
    /* fifo */ fifo_cout_drain_PE_35,
    /* fifo */ fifo_w_PE_35
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 36,
    /* fifo */ fifo_cin_PE_36,
    /* fifo */ fifo_cout_1_PE_36,
    /* fifo */ fifo_cout_1_PE_37,
    /* fifo */ fifo_cout_drain_PE_36,
    /* fifo */ fifo_w_PE_36
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 37,
    /* fifo */ fifo_cin_PE_37,
    /* fifo */ fifo_cout_1_PE_37,
    /* fifo */ fifo_cout_1_PE_38,
    /* fifo */ fifo_cout_drain_PE_37,
    /* fifo */ fifo_w_PE_37
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 38,
    /* fifo */ fifo_cin_PE_38,
    /* fifo */ fifo_cout_1_PE_38,
    /* fifo */ fifo_cout_1_PE_39,
    /* fifo */ fifo_cout_drain_PE_38,
    /* fifo */ fifo_w_PE_38
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 39,
    /* fifo */ fifo_cin_PE_39,
    /* fifo */ fifo_cout_1_PE_39,
    /* fifo */ fifo_cout_1_PE_40,
    /* fifo */ fifo_cout_drain_PE_39,
    /* fifo */ fifo_w_PE_39
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 40,
    /* fifo */ fifo_cin_PE_40,
    /* fifo */ fifo_cout_1_PE_40,
    /* fifo */ fifo_cout_1_PE_41,
    /* fifo */ fifo_cout_drain_PE_40,
    /* fifo */ fifo_w_PE_40
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 41,
    /* fifo */ fifo_cin_PE_41,
    /* fifo */ fifo_cout_1_PE_41,
    /* fifo */ fifo_cout_1_PE_42,
    /* fifo */ fifo_cout_drain_PE_41,
    /* fifo */ fifo_w_PE_41
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 42,
    /* fifo */ fifo_cin_PE_42,
    /* fifo */ fifo_cout_1_PE_42,
    /* fifo */ fifo_cout_1_PE_43,
    /* fifo */ fifo_cout_drain_PE_42,
    /* fifo */ fifo_w_PE_42
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 43,
    /* fifo */ fifo_cin_PE_43,
    /* fifo */ fifo_cout_1_PE_43,
    /* fifo */ fifo_cout_1_PE_44,
    /* fifo */ fifo_cout_drain_PE_43,
    /* fifo */ fifo_w_PE_43
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 44,
    /* fifo */ fifo_cin_PE_44,
    /* fifo */ fifo_cout_1_PE_44,
    /* fifo */ fifo_cout_1_PE_45,
    /* fifo */ fifo_cout_drain_PE_44,
    /* fifo */ fifo_w_PE_44
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 45,
    /* fifo */ fifo_cin_PE_45,
    /* fifo */ fifo_cout_1_PE_45,
    /* fifo */ fifo_cout_1_PE_46,
    /* fifo */ fifo_cout_drain_PE_45,
    /* fifo */ fifo_w_PE_45
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 46,
    /* fifo */ fifo_cin_PE_46,
    /* fifo */ fifo_cout_1_PE_46,
    /* fifo */ fifo_cout_1_PE_47,
    /* fifo */ fifo_cout_drain_PE_46,
    /* fifo */ fifo_w_PE_46
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 47,
    /* fifo */ fifo_cin_PE_47,
    /* fifo */ fifo_cout_1_PE_47,
    /* fifo */ fifo_cout_1_PE_48,
    /* fifo */ fifo_cout_drain_PE_47,
    /* fifo */ fifo_w_PE_47
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 48,
    /* fifo */ fifo_cin_PE_48,
    /* fifo */ fifo_cout_1_PE_48,
    /* fifo */ fifo_cout_1_PE_49,
    /* fifo */ fifo_cout_drain_PE_48,
    /* fifo */ fifo_w_PE_48
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 49,
    /* fifo */ fifo_cin_PE_49,
    /* fifo */ fifo_cout_1_PE_49,
    /* fifo */ fifo_cout_1_PE_50,
    /* fifo */ fifo_cout_drain_PE_49,
    /* fifo */ fifo_w_PE_49
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 50,
    /* fifo */ fifo_cin_PE_50,
    /* fifo */ fifo_cout_1_PE_50,
    /* fifo */ fifo_cout_1_PE_51,
    /* fifo */ fifo_cout_drain_PE_50,
    /* fifo */ fifo_w_PE_50
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 51,
    /* fifo */ fifo_cin_PE_51,
    /* fifo */ fifo_cout_1_PE_51,
    /* fifo */ fifo_cout_1_PE_52,
    /* fifo */ fifo_cout_drain_PE_51,
    /* fifo */ fifo_w_PE_51
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 52,
    /* fifo */ fifo_cin_PE_52,
    /* fifo */ fifo_cout_1_PE_52,
    /* fifo */ fifo_cout_1_PE_53,
    /* fifo */ fifo_cout_drain_PE_52,
    /* fifo */ fifo_w_PE_52
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 53,
    /* fifo */ fifo_cin_PE_53,
    /* fifo */ fifo_cout_1_PE_53,
    /* fifo */ fifo_cout_1_PE_54,
    /* fifo */ fifo_cout_drain_PE_53,
    /* fifo */ fifo_w_PE_53
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 54,
    /* fifo */ fifo_cin_PE_54,
    /* fifo */ fifo_cout_1_PE_54,
    /* fifo */ fifo_cout_1_PE_55,
    /* fifo */ fifo_cout_drain_PE_54,
    /* fifo */ fifo_w_PE_54
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 55,
    /* fifo */ fifo_cin_PE_55,
    /* fifo */ fifo_cout_1_PE_55,
    /* fifo */ fifo_cout_1_PE_56,
    /* fifo */ fifo_cout_drain_PE_55,
    /* fifo */ fifo_w_PE_55
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 56,
    /* fifo */ fifo_cin_PE_56,
    /* fifo */ fifo_cout_1_PE_56,
    /* fifo */ fifo_cout_1_PE_57,
    /* fifo */ fifo_cout_drain_PE_56,
    /* fifo */ fifo_w_PE_56
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 57,
    /* fifo */ fifo_cin_PE_57,
    /* fifo */ fifo_cout_1_PE_57,
    /* fifo */ fifo_cout_1_PE_58,
    /* fifo */ fifo_cout_drain_PE_57,
    /* fifo */ fifo_w_PE_57
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 58,
    /* fifo */ fifo_cin_PE_58,
    /* fifo */ fifo_cout_1_PE_58,
    /* fifo */ fifo_cout_1_PE_59,
    /* fifo */ fifo_cout_drain_PE_58,
    /* fifo */ fifo_w_PE_58
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 59,
    /* fifo */ fifo_cin_PE_59,
    /* fifo */ fifo_cout_1_PE_59,
    /* fifo */ fifo_cout_1_PE_60,
    /* fifo */ fifo_cout_drain_PE_59,
    /* fifo */ fifo_w_PE_59
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 60,
    /* fifo */ fifo_cin_PE_60,
    /* fifo */ fifo_cout_1_PE_60,
    /* fifo */ fifo_cout_1_PE_61,
    /* fifo */ fifo_cout_drain_PE_60,
    /* fifo */ fifo_w_PE_60
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 61,
    /* fifo */ fifo_cin_PE_61,
    /* fifo */ fifo_cout_1_PE_61,
    /* fifo */ fifo_cout_1_PE_62,
    /* fifo */ fifo_cout_drain_PE_61,
    /* fifo */ fifo_w_PE_61
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 62,
    /* fifo */ fifo_cin_PE_62,
    /* fifo */ fifo_cout_1_PE_62,
    /* fifo */ fifo_cout_1_PE_63,
    /* fifo */ fifo_cout_drain_PE_62,
    /* fifo */ fifo_w_PE_62
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 63,
    /* fifo */ fifo_cin_PE_63,
    /* fifo */ fifo_cout_1_PE_63,
    /* fifo */ fifo_cout_1_PE_64,
    /* fifo */ fifo_cout_drain_PE_63,
    /* fifo */ fifo_w_PE_63
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 64,
    /* fifo */ fifo_cin_PE_64,
    /* fifo */ fifo_cout_1_PE_64,
    /* fifo */ fifo_cout_1_PE_65,
    /* fifo */ fifo_cout_drain_PE_64,
    /* fifo */ fifo_w_PE_64
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 65,
    /* fifo */ fifo_cin_PE_65,
    /* fifo */ fifo_cout_1_PE_65,
    /* fifo */ fifo_cout_1_PE_66,
    /* fifo */ fifo_cout_drain_PE_65,
    /* fifo */ fifo_w_PE_65
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 66,
    /* fifo */ fifo_cin_PE_66,
    /* fifo */ fifo_cout_1_PE_66,
    /* fifo */ fifo_cout_1_PE_67,
    /* fifo */ fifo_cout_drain_PE_66,
    /* fifo */ fifo_w_PE_66
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 67,
    /* fifo */ fifo_cin_PE_67,
    /* fifo */ fifo_cout_1_PE_67,
    /* fifo */ fifo_cout_1_PE_68,
    /* fifo */ fifo_cout_drain_PE_67,
    /* fifo */ fifo_w_PE_67
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 68,
    /* fifo */ fifo_cin_PE_68,
    /* fifo */ fifo_cout_1_PE_68,
    /* fifo */ fifo_cout_1_PE_69,
    /* fifo */ fifo_cout_drain_PE_68,
    /* fifo */ fifo_w_PE_68
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 69,
    /* fifo */ fifo_cin_PE_69,
    /* fifo */ fifo_cout_1_PE_69,
    /* fifo */ fifo_cout_1_PE_70,
    /* fifo */ fifo_cout_drain_PE_69,
    /* fifo */ fifo_w_PE_69
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 70,
    /* fifo */ fifo_cin_PE_70,
    /* fifo */ fifo_cout_1_PE_70,
    /* fifo */ fifo_cout_1_PE_71,
    /* fifo */ fifo_cout_drain_PE_70,
    /* fifo */ fifo_w_PE_70
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 71,
    /* fifo */ fifo_cin_PE_71,
    /* fifo */ fifo_cout_1_PE_71,
    /* fifo */ fifo_cout_1_PE_72,
    /* fifo */ fifo_cout_drain_PE_71,
    /* fifo */ fifo_w_PE_71
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 72,
    /* fifo */ fifo_cin_PE_72,
    /* fifo */ fifo_cout_1_PE_72,
    /* fifo */ fifo_cout_1_PE_73,
    /* fifo */ fifo_cout_drain_PE_72,
    /* fifo */ fifo_w_PE_72
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 73,
    /* fifo */ fifo_cin_PE_73,
    /* fifo */ fifo_cout_1_PE_73,
    /* fifo */ fifo_cout_1_PE_74,
    /* fifo */ fifo_cout_drain_PE_73,
    /* fifo */ fifo_w_PE_73
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 74,
    /* fifo */ fifo_cin_PE_74,
    /* fifo */ fifo_cout_1_PE_74,
    /* fifo */ fifo_cout_1_PE_75,
    /* fifo */ fifo_cout_drain_PE_74,
    /* fifo */ fifo_w_PE_74
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 75,
    /* fifo */ fifo_cin_PE_75,
    /* fifo */ fifo_cout_1_PE_75,
    /* fifo */ fifo_cout_1_PE_76,
    /* fifo */ fifo_cout_drain_PE_75,
    /* fifo */ fifo_w_PE_75
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 76,
    /* fifo */ fifo_cin_PE_76,
    /* fifo */ fifo_cout_1_PE_76,
    /* fifo */ fifo_cout_1_PE_77,
    /* fifo */ fifo_cout_drain_PE_76,
    /* fifo */ fifo_w_PE_76
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 77,
    /* fifo */ fifo_cin_PE_77,
    /* fifo */ fifo_cout_1_PE_77,
    /* fifo */ fifo_cout_1_PE_78,
    /* fifo */ fifo_cout_drain_PE_77,
    /* fifo */ fifo_w_PE_77
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 78,
    /* fifo */ fifo_cin_PE_78,
    /* fifo */ fifo_cout_1_PE_78,
    /* fifo */ fifo_cout_1_PE_79,
    /* fifo */ fifo_cout_drain_PE_78,
    /* fifo */ fifo_w_PE_78
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 79,
    /* fifo */ fifo_cin_PE_79,
    /* fifo */ fifo_cout_1_PE_79,
    /* fifo */ fifo_cout_1_PE_80,
    /* fifo */ fifo_cout_drain_PE_79,
    /* fifo */ fifo_w_PE_79
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 80,
    /* fifo */ fifo_cin_PE_80,
    /* fifo */ fifo_cout_1_PE_80,
    /* fifo */ fifo_cout_1_PE_81,
    /* fifo */ fifo_cout_drain_PE_80,
    /* fifo */ fifo_w_PE_80
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 81,
    /* fifo */ fifo_cin_PE_81,
    /* fifo */ fifo_cout_1_PE_81,
    /* fifo */ fifo_cout_1_PE_82,
    /* fifo */ fifo_cout_drain_PE_81,
    /* fifo */ fifo_w_PE_81
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 82,
    /* fifo */ fifo_cin_PE_82,
    /* fifo */ fifo_cout_1_PE_82,
    /* fifo */ fifo_cout_1_PE_83,
    /* fifo */ fifo_cout_drain_PE_82,
    /* fifo */ fifo_w_PE_82
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 83,
    /* fifo */ fifo_cin_PE_83,
    /* fifo */ fifo_cout_1_PE_83,
    /* fifo */ fifo_cout_1_PE_84,
    /* fifo */ fifo_cout_drain_PE_83,
    /* fifo */ fifo_w_PE_83
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 84,
    /* fifo */ fifo_cin_PE_84,
    /* fifo */ fifo_cout_1_PE_84,
    /* fifo */ fifo_cout_1_PE_85,
    /* fifo */ fifo_cout_drain_PE_84,
    /* fifo */ fifo_w_PE_84
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 85,
    /* fifo */ fifo_cin_PE_85,
    /* fifo */ fifo_cout_1_PE_85,
    /* fifo */ fifo_cout_1_PE_86,
    /* fifo */ fifo_cout_drain_PE_85,
    /* fifo */ fifo_w_PE_85
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 86,
    /* fifo */ fifo_cin_PE_86,
    /* fifo */ fifo_cout_1_PE_86,
    /* fifo */ fifo_cout_1_PE_87,
    /* fifo */ fifo_cout_drain_PE_86,
    /* fifo */ fifo_w_PE_86
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 87,
    /* fifo */ fifo_cin_PE_87,
    /* fifo */ fifo_cout_1_PE_87,
    /* fifo */ fifo_cout_1_PE_88,
    /* fifo */ fifo_cout_drain_PE_87,
    /* fifo */ fifo_w_PE_87
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 88,
    /* fifo */ fifo_cin_PE_88,
    /* fifo */ fifo_cout_1_PE_88,
    /* fifo */ fifo_cout_1_PE_89,
    /* fifo */ fifo_cout_drain_PE_88,
    /* fifo */ fifo_w_PE_88
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 89,
    /* fifo */ fifo_cin_PE_89,
    /* fifo */ fifo_cout_1_PE_89,
    /* fifo */ fifo_cout_1_PE_90,
    /* fifo */ fifo_cout_drain_PE_89,
    /* fifo */ fifo_w_PE_89
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 90,
    /* fifo */ fifo_cin_PE_90,
    /* fifo */ fifo_cout_1_PE_90,
    /* fifo */ fifo_cout_1_PE_91,
    /* fifo */ fifo_cout_drain_PE_90,
    /* fifo */ fifo_w_PE_90
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 91,
    /* fifo */ fifo_cin_PE_91,
    /* fifo */ fifo_cout_1_PE_91,
    /* fifo */ fifo_cout_1_PE_92,
    /* fifo */ fifo_cout_drain_PE_91,
    /* fifo */ fifo_w_PE_91
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 92,
    /* fifo */ fifo_cin_PE_92,
    /* fifo */ fifo_cout_1_PE_92,
    /* fifo */ fifo_cout_1_PE_93,
    /* fifo */ fifo_cout_drain_PE_92,
    /* fifo */ fifo_w_PE_92
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 93,
    /* fifo */ fifo_cin_PE_93,
    /* fifo */ fifo_cout_1_PE_93,
    /* fifo */ fifo_cout_1_PE_94,
    /* fifo */ fifo_cout_drain_PE_93,
    /* fifo */ fifo_w_PE_93
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 94,
    /* fifo */ fifo_cin_PE_94,
    /* fifo */ fifo_cout_1_PE_94,
    /* fifo */ fifo_cout_1_PE_95,
    /* fifo */ fifo_cout_drain_PE_94,
    /* fifo */ fifo_w_PE_94
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 95,
    /* fifo */ fifo_cin_PE_95,
    /* fifo */ fifo_cout_1_PE_95,
    /* fifo */ fifo_cout_1_PE_96,
    /* fifo */ fifo_cout_drain_PE_95,
    /* fifo */ fifo_w_PE_95
  );
  /* Module Call */

  /* Module Call */
  cout_1_IO_L2_out_boundary(
    /* array */ cout_1,
    /* fifo */ fifo_cout_1_PE_1
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 31,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_31,
    /* fifo */ fifo_cout_drain_PE_31
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 30,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_31,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_30,
    /* fifo */ fifo_cout_drain_PE_30
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 29,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_30,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_29,
    /* fifo */ fifo_cout_drain_PE_29
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 28,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_29,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_28,
    /* fifo */ fifo_cout_drain_PE_28
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 27,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_28,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_27,
    /* fifo */ fifo_cout_drain_PE_27
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 26,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_27,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_26,
    /* fifo */ fifo_cout_drain_PE_26
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 25,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_26,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_25,
    /* fifo */ fifo_cout_drain_PE_25
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 24,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_25,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_24,
    /* fifo */ fifo_cout_drain_PE_24
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 23,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_24,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_23,
    /* fifo */ fifo_cout_drain_PE_23
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 22,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_23,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_22,
    /* fifo */ fifo_cout_drain_PE_22
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 21,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_22,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_21,
    /* fifo */ fifo_cout_drain_PE_21
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 20,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_21,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_20,
    /* fifo */ fifo_cout_drain_PE_20
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 19,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_20,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_19,
    /* fifo */ fifo_cout_drain_PE_19
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 18,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_19,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_18,
    /* fifo */ fifo_cout_drain_PE_18
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 17,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_18,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_17,
    /* fifo */ fifo_cout_drain_PE_17
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 16,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_17,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_16,
    /* fifo */ fifo_cout_drain_PE_16
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_16,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_15,
    /* fifo */ fifo_cout_drain_PE_15
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_15,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_14,
    /* fifo */ fifo_cout_drain_PE_14
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_14,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_13,
    /* fifo */ fifo_cout_drain_PE_13
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_13,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_12,
    /* fifo */ fifo_cout_drain_PE_12
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_12,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_11,
    /* fifo */ fifo_cout_drain_PE_11
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_11,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_10,
    /* fifo */ fifo_cout_drain_PE_10
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_10,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_9,
    /* fifo */ fifo_cout_drain_PE_9
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_9,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_8,
    /* fifo */ fifo_cout_drain_PE_8
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_8,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_7,
    /* fifo */ fifo_cout_drain_PE_7
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_7,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6,
    /* fifo */ fifo_cout_drain_PE_6
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_6,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5,
    /* fifo */ fifo_cout_drain_PE_5
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_5,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4,
    /* fifo */ fifo_cout_drain_PE_4
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_4,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3,
    /* fifo */ fifo_cout_drain_PE_3
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_3,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2,
    /* fifo */ fifo_cout_drain_PE_2
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1,
    /* fifo */ fifo_cout_drain_PE_1
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_1,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0,
    /* fifo */ fifo_cout_drain_PE_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L2_out(
    /* array */ cout_2,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0
  );
  /* Module Call */

}
}

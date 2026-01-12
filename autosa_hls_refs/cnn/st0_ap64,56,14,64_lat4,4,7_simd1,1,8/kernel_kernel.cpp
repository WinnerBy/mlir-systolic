// AutoSA Command:
// /workspaces/mlir-systolic/third_party/AutoSA/autosa /workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/cnn/kernel.c --config=/workspaces/mlir-systolic/third_party/AutoSA/autosa_config/autosa_config.json --target=autosa_hls_c --output-dir=/workspaces/mlir-systolic/third_party/AutoSA/autosa.tmp/output --host-serialize --simd-info=/workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/cnn/simd_info.json --sa-sizes={kernel[]->space_time[0];kernel[]->array_part[64,56,14,64];kernel[]->latency[4,4,7];kernel[]->simd[1,1,8]}

#include <ap_int.h>
#include <hls_stream.h>

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Data Type */
typedef float cin_t1;
typedef float cout_t1;
typedef float w_t1;
typedef ap_uint<512> cin_t16;
typedef ap_uint<256> cin_t8;
typedef ap_uint<512> cout_t16;
typedef ap_uint<128> cout_t4;
typedef ap_uint<512> w_t16;
typedef ap_uint<256> w_t8;
/* Data Type */

extern "C" {
void kernel0(cin_t16 *cin, cout_t16 *cout, w_t16 *w);
}
void cin_IO_L2_in_intra_trans(int c0, int c2, int c3, cin_t16 local_cin[58][16][4], hls::stream<cin_t8> &fifo_cin_local_out, bool intra_trans_en);
void cin_IO_L2_in_inter_trans_boundary(int c0, int c2, int c3, cin_t16 local_cin[58][16][4], hls::stream<cin_t16> &fifo_cin_in, bool inter_trans_en);
void w_IO_L1_in_intra_trans(int idx, int c0, int c2, int c3, w_t8 local_w[4][3][3][8], hls::stream<w_t8> &fifo_w_local_out, bool intra_trans_en);
void w_IO_L1_in_inter_trans(int idx, int c0, int c2, int c3, w_t8 local_w[4][3][3][8], hls::stream<w_t8> &fifo_w_in, hls::stream<w_t8> &fifo_w_out, bool inter_trans_en);
void w_IO_L1_in_inter_trans_boundary(int idx, int c0, int c2, int c3, w_t8 local_w[4][3][3][8], hls::stream<w_t8> &fifo_w_in, bool inter_trans_en);
void w_IO_L1_in_wrapper(int idx, hls::stream<w_t8> &fifo_w_in, hls::stream<w_t8> &fifo_w_out, hls::stream<w_t8> &fifo_w_local_out);
void w_IO_L1_in_boundary_wrapper(int idx, hls::stream<w_t8> &fifo_w_in, hls::stream<w_t8> &fifo_w_local_out);
void PE_wrapper(int idx, hls::stream<cin_t8> &fifo_cin_in, hls::stream<cin_t8> &fifo_cin_out, hls::stream<float> &fifo_cout_drain_out, hls::stream<w_t8> &fifo_w_in);
void cout_drain_IO_L1_out_intra_trans(int idx, int c0, int c2, cout_t4 local_cout[56][14][1], hls::stream<float> &fifo_cout_drain_local_in);
void cout_drain_IO_L1_out_inter_trans(int idx, int c0, int c2, cout_t4 local_cout[56][14][1], hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out);
void cout_drain_IO_L1_out_inter_trans_boundary(int idx, int c0, int c2, cout_t4 local_cout[56][14][1], hls::stream<cout_t4> &fifo_cout_drain_out);
void cout_drain_IO_L1_out_wrapper(int idx, hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in);
void cout_drain_IO_L1_out_boundary_wrapper(int idx, hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<float> &fifo_cout_drain_local_in);

/* Module Definition */
void cin_IO_L2_in_intra_trans(int c0, int c2, int c3, cin_t16 local_cin[58][16][4], hls::stream<cin_t8> &fifo_cin_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  cin_t8 data_split[2];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L2
  // io_L1
  // pe
  for (ap_uint<5> c5 = 0; c5 <= 13; c5 += 1)
    for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
        for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
          for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
            // latency
            for (ap_uint<4> c10 = 0; c10 <= 6; c10 += 1) {
              // latency
              for (ap_uint<3> c11 = 0; c11 <= 3; c11 += 1) {
                // latency
                for (ap_uint<3> c12 = 0; c12 <= 3; c12 += 1) {
                #pragma HLS PIPELINE II=1
                  // simd
                  {
                    cin_t16 in_data;
                    cin_t8 out_data;
                    in_data = local_cin[c11 + 4*c5 + c8][c10 + 7*c6 + c9][8 * c7 / 16];
                    for (ap_uint<2> n = 0; n < 2; n++) {
                    #pragma HLS UNROLL
                      data_split[n] = in_data(255, 0);
                      in_data = in_data >> 256;
                    }
                    int split_idx = (c7) % 2;
                    out_data = data_split[split_idx];
                    fifo_cin_local_out.write(out_data);
                  }
                }
              }
            }
          }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L2_in_inter_trans_boundary(int c0, int c2, int c3, cin_t16 local_cin[58][16][4], hls::stream<cin_t16> &fifo_cin_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  if (!inter_trans_en) return;

  // io_L2
  for (ap_uint<7> c4 = 0; c4 <= 57; c4 += 1)
    for (ap_uint<5> c5 = 0; c5 <= 15; c5 += 1) {
      // access_coalesce
      // access_serialize
      for (ap_uint<3> c6 = 0; c6 <= 3; c6 += 1) {
      #pragma HLS PIPELINE II=1
        {
          cin_t16 in_data;
          cin_t16 out_data;
          in_data = fifo_cin_in.read();
          out_data = in_data;
          local_cin[c4][c5][c6] = out_data;
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void cin_IO_L2_in_boundary(hls::stream<cin_t16> &fifo_cin_in, hls::stream<cin_t8> &fifo_cin_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  cin_t16 local_cin_ping[58][16][4];
  #pragma HLS RESOURCE variable=local_cin_ping core=RAM_2P_BRAM
  cin_t16 local_cin_pong[58][16][4];
  #pragma HLS RESOURCE variable=local_cin_pong core=RAM_2P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c2, c2_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<5> c0 = 0; c0 <= 9; c0 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
          // array
          {
            if (arb == 0) {
              cin_IO_L2_in_inter_trans_boundary(
                /* host iter */ c0, 
                /* host iter */ c2, 
                /* host iter */ c3, 
                /* array */ local_cin_pong, 
                /* fifo */ fifo_cin_in, 
                /* enable */ inter_trans_en
              );
              cin_IO_L2_in_intra_trans(
                /* host iter */ c0_prev, 
                /* host iter */ c2_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_cin_ping, 
                /* fifo */ fifo_cin_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              cin_IO_L2_in_inter_trans_boundary(
                /* host iter */ c0, 
                /* host iter */ c2, 
                /* host iter */ c3, 
                /* array */ local_cin_ping, 
                /* fifo */ fifo_cin_in, 
                /* enable */ inter_trans_en
              );
              cin_IO_L2_in_intra_trans(
                /* host iter */ c0_prev, 
                /* host iter */ c2_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_cin_pong, 
                /* fifo */ fifo_cin_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c2_prev = c2;
            c3_prev = c3;
          }
        }
    if (arb == 0) {
      cin_IO_L2_in_intra_trans(
        /* host iter */ c0_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_cin_ping, 
        /* fifo */ fifo_cin_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      cin_IO_L2_in_intra_trans(
        /* host iter */ c0_prev, 
        /* host iter */ c2_prev, 
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
void cin_IO_L2_in_boundary_serialize(cin_t16 *cin, hls::stream<cin_t16> &fifo_cin_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<22> i = 0; i < 1187840; i++) {
  #pragma HLS PIPELINE II=1
    cin_t16 fifo_data;
    fifo_data = cin[i];
    fifo_cin_local_out.write(fifo_data);
  }
}
/* Module Definition */

/* Module Definition */
void w_IO_L2_in(hls::stream<w_t8> &fifo_w_in, hls::stream<w_t8> &fifo_w_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 9; c0 += 1)
    for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
      for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
        // array
        // io_L2
        for (ap_uint<5> c4 = 0; c4 <= 15; c4 += 1) {
          // io_L1
          for (ap_uint<3> c5 = 0; c5 <= 3; c5 += 1)
            for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
              for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
                // access_coalesce
                // access_serialize
                for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
                #pragma HLS PIPELINE II=1
                  {
                    w_t8 in_data;
                    w_t8 out_data;
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
void w_IO_L2_in_serialize(w_t16 *w, hls::stream<w_t8> &fifo_w_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  w_t8 fifo_data;
  w_t16 mem_data;
  for (ap_uint<21> i = 0; i < 737280; i++) {
  #pragma HLS PIPELINE II=1
    mem_data = w[i];
    for (ap_uint<2> p = 0; p < 2; p++) {
      fifo_data = mem_data(255, 0);
      mem_data = mem_data >> 256;
      fifo_w_local_out.write(fifo_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
void w_IO_L1_in_intra_trans(int idx, int c0, int c2, int c3, w_t8 local_w[4][3][3][8], hls::stream<w_t8> &fifo_w_local_out, bool intra_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!intra_trans_en) return;


  // io_L1
  // pe
  for (ap_uint<5> c5 = 0; c5 <= 13; c5 += 1)
    for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
      for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
        for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
          for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
            // latency
            for (ap_uint<4> c10 = 0; c10 <= 6; c10 += 1) {
              // latency
              for (ap_uint<3> c11 = 0; c11 <= 3; c11 += 1) {
                // latency
                for (ap_uint<3> c12 = 0; c12 <= 3; c12 += 1) {
                #pragma HLS PIPELINE II=1
                  // simd
                  {
                    w_t8 in_data;
                    w_t8 out_data;
                    in_data = local_w[c12][c8][c9][c7];
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
void w_IO_L1_in_inter_trans(int idx, int c0, int c2, int c3, w_t8 local_w[4][3][3][8], hls::stream<w_t8> &fifo_w_in, hls::stream<w_t8> &fifo_w_out, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<5> c4 = p0; c4 <= 15; c4 += 1) {
    // io_L1
    if (c4 == p0) {
      for (ap_uint<3> c5 = 0; c5 <= 3; c5 += 1)
        for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
          for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
            // access_coalesce
            for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
            #pragma HLS PIPELINE II=1
              {
                w_t8 in_data;
                w_t8 out_data;
                in_data = fifo_w_in.read();
                out_data = in_data;
                local_w[c5][c6][c7][c8] = out_data;
              }
            }
          }
    } else {
      for (ap_uint<3> c5 = 0; c5 <= 3; c5 += 1)
        for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
          for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
            // access_coalesce
            for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
            #pragma HLS PIPELINE II=1
              {
                w_t8 in_data;
                w_t8 out_data;
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
void w_IO_L1_in_inter_trans_boundary(int idx, int c0, int c2, int c3, w_t8 local_w[4][3][3][8], hls::stream<w_t8> &fifo_w_in, bool inter_trans_en) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  if (!inter_trans_en) return;

  for (ap_uint<5> c4 = p0; c4 <= 15; c4 += 1)
    if (c4 == p0) {
      // io_L1
      for (ap_uint<3> c5 = 0; c5 <= 3; c5 += 1)
        for (ap_uint<3> c6 = 0; c6 <= 2; c6 += 1)
          for (ap_uint<3> c7 = 0; c7 <= 2; c7 += 1) {
            // access_coalesce
            for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
            #pragma HLS PIPELINE II=1
              {
                w_t8 in_data;
                w_t8 out_data;
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
void w_IO_L1_in(int idx, hls::stream<w_t8> &fifo_w_in, hls::stream<w_t8> &fifo_w_out, hls::stream<w_t8> &fifo_w_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  w_t8 local_w_ping[4][3][3][8];
  #pragma HLS RESOURCE variable=local_w_ping core=RAM_1P_BRAM
  w_t8 local_w_pong[4][3][3][8];
  #pragma HLS RESOURCE variable=local_w_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c2, c2_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<5> c0 = 0; c0 <= 9; c0 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
          // array
          // io_L2
          {
            if (arb == 0) {
              w_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c2, 
                /* host iter */ c3, 
                /* array */ local_w_pong, 
                /* fifo */ fifo_w_in, 
                /* fifo */ fifo_w_out, 
                /* enable */ inter_trans_en
              );
              w_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c2_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_w_ping, 
                /* fifo */ fifo_w_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              w_IO_L1_in_inter_trans(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c2, 
                /* host iter */ c3, 
                /* array */ local_w_ping, 
                /* fifo */ fifo_w_in, 
                /* fifo */ fifo_w_out, 
                /* enable */ inter_trans_en
              );
              w_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c2_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_w_pong, 
                /* fifo */ fifo_w_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c2_prev = c2;
            c3_prev = c3;
          }
        }
    if (arb == 0) {
      w_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_w_ping, 
        /* fifo */ fifo_w_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      w_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c2_prev, 
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
void w_IO_L1_in_wrapper(int idx, hls::stream<w_t8> &fifo_w_in, hls::stream<w_t8> &fifo_w_out, hls::stream<w_t8> &fifo_w_local_out)
 {
  w_IO_L1_in(
    /* module id */ idx, 
    /* fifo */ fifo_w_in, 
    /* fifo */ fifo_w_out, 
    /* fifo */ fifo_w_local_out);
}
/* Module Definition */

/* Module Definition */
void w_IO_L1_in_boundary(int idx, hls::stream<w_t8> &fifo_w_in, hls::stream<w_t8> &fifo_w_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  w_t8 local_w_ping[4][3][3][8];
  #pragma HLS RESOURCE variable=local_w_ping core=RAM_1P_BRAM
  w_t8 local_w_pong[4][3][3][8];
  #pragma HLS RESOURCE variable=local_w_pong core=RAM_1P_BRAM
  bool arb = 0;
  bool inter_trans_en = 1;
  bool intra_trans_en = 0;
  int c0, c0_prev;
  int c2, c2_prev;
  int c3, c3_prev;
  /* Variable Declaration */

  {
    for (ap_uint<5> c0 = 0; c0 <= 9; c0 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
        for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
          // array
          // io_L2
          {
            if (arb == 0) {
              w_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c2, 
                /* host iter */ c3, 
                /* array */ local_w_pong, 
                /* fifo */ fifo_w_in, 
                /* enable */ inter_trans_en
              );
              w_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c2_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_w_ping, 
                /* fifo */ fifo_w_local_out, 
                /* enable */ intra_trans_en
              );
            } else {
              w_IO_L1_in_inter_trans_boundary(
                /* module id */ idx, 
                /* host iter */ c0, 
                /* host iter */ c2, 
                /* host iter */ c3, 
                /* array */ local_w_ping, 
                /* fifo */ fifo_w_in, 
                /* enable */ inter_trans_en
              );
              w_IO_L1_in_intra_trans(
                /* module id */ idx, 
                /* host iter */ c0_prev, 
                /* host iter */ c2_prev, 
                /* host iter */ c3_prev, 
                /* array */ local_w_pong, 
                /* fifo */ fifo_w_local_out, 
                /* enable */ intra_trans_en
              );
            }
            intra_trans_en = 1;
            arb = !arb;
            c0_prev = c0;
            c2_prev = c2;
            c3_prev = c3;
          }
        }
    if (arb == 0) {
      w_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c2_prev, 
        /* host iter */ c3_prev, 
        /* array */ local_w_ping, 
        /* fifo */ fifo_w_local_out, 
        /* enable */ intra_trans_en
      );
    } else {
      w_IO_L1_in_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0_prev, 
        /* host iter */ c2_prev, 
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
void w_IO_L1_in_boundary_wrapper(int idx, hls::stream<w_t8> &fifo_w_in, hls::stream<w_t8> &fifo_w_local_out)
 {
  w_IO_L1_in_boundary(
    /* module id */ idx, 
    /* fifo */ fifo_w_in, 
    /* fifo */ fifo_w_local_out);
}
/* Module Definition */

/* Module Definition */
void PE(int idx, hls::stream<cin_t8> &fifo_cin_in, hls::stream<cin_t8> &fifo_cin_out, hls::stream<float> &fifo_cout_drain_out, hls::stream<w_t8> &fifo_w_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  int p0 = idx; // module id
  cin_t1 local_cin[1][1][8];
  #pragma HLS ARRAY_PARTITION variable=local_cin dim=0 complete
  cout_t1 local_cout[56][14][4];
  #pragma HLS RESOURCE variable=local_cout core=RAM_2P_BRAM
  w_t1 local_w[1][1][1][8];
  #pragma HLS ARRAY_PARTITION variable=local_w dim=0 complete
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 9; c0 += 1)
    for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1)
      for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
        // array
        // pe
        for (ap_uint<5> c5 = 0; c5 <= 13; c5 += 1)
          for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1) {
            if (c3 == 0) {
              // latency
              for (ap_uint<4> c8 = 0; c8 <= 6; c8 += 1) {
                // latency
                for (ap_uint<3> c9 = 0; c9 <= 3; c9 += 1) {
                  // latency
                  for (ap_uint<3> c10 = 0; c10 <= 3; c10 += 1) {
                  #pragma HLS PIPELINE II=1
                    // simd
                    // hls_unroll
                    local_cout[4*c5 + c9][7*c6 + c8][c10] = 0;
                  }
                }
              }
            }
            for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
              for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
                for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
                  // latency
                  for (ap_uint<4> c10 = 0; c10 <= 6; c10 += 1) {
                    // latency
                    for (ap_uint<3> c11 = 0; c11 <= 3; c11 += 1) {
                      // latency
                      for (ap_uint<3> c12 = 0; c12 <= 3; c12 += 1) {
                      #pragma HLS PIPELINE II=1
                        {
                          {
                            cin_t8 fifo_data;
                            fifo_data = fifo_cin_in.read();
                            for (ap_uint<4> n = 0; n < 8; n++) {
                            #pragma HLS UNROLL
                              union {unsigned int ui; float ut;} u;
                              u.ui = (unsigned int)fifo_data(31, 0);
                              local_cin[0][0][n] = u.ut;
                              fifo_data = fifo_data >> 32;
                            }
                          }
                          {
                            w_t8 fifo_data;
                            fifo_data = fifo_w_in.read();
                            for (ap_uint<4> n = 0; n < 8; n++) {
                            #pragma HLS UNROLL
                              union {unsigned int ui; float ut;} u;
                              u.ui = (unsigned int)fifo_data(31, 0);
                              local_w[0][0][0][n] = u.ut;
                              fifo_data = fifo_data >> 32;
                            }
                          }
                          // simd
                          for (ap_uint<4> c13 = 0; c13 <= 7; c13 += 1) {
                          #pragma HLS UNROLL
                            local_cout[c11 + 4*c5][c10 + 7*c6][c12] = (local_cout[c11 + 4*c5][c10 + 7*c6][c12] + (local_cin[0][0][c13] * local_w[0][0][0][c13]));
                          }
                          if (c3 == 7 && c7 == 7 && c8 == 2 && c9 == 2)
                            fifo_cout_drain_out.write(local_cout[c11 + 4*c5][c10 + 7*c6][c12]);
                          {
                            cin_t8 fifo_data;
                            union {unsigned int ui; float ut;} u7, u6, u5, u4, u3, u2, u1, u0;
                            u7.ut = local_cin[0][0][7];
                            u6.ut = local_cin[0][0][6];
                            u5.ut = local_cin[0][0][5];
                            u4.ut = local_cin[0][0][4];
                            u3.ut = local_cin[0][0][3];
                            u2.ut = local_cin[0][0][2];
                            u1.ut = local_cin[0][0][1];
                            u0.ut = local_cin[0][0][0];
                            fifo_data = (ap_uint<32>(u7.ui), ap_uint<32>(u6.ui), ap_uint<32>(u5.ui), ap_uint<32>(u4.ui), ap_uint<32>(u3.ui), ap_uint<32>(u2.ui), ap_uint<32>(u1.ui), ap_uint<32>(u0.ui));
                            fifo_cin_out.write(fifo_data);
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
void PE_wrapper(int idx, hls::stream<cin_t8> &fifo_cin_in, hls::stream<cin_t8> &fifo_cin_out, hls::stream<float> &fifo_cout_drain_out, hls::stream<w_t8> &fifo_w_in)
 {
  PE(
    /* module id */ idx, 
    /* fifo */ fifo_cin_in, 
    /* fifo */ fifo_cin_out, 
    /* fifo */ fifo_cout_drain_out, 
    /* fifo */ fifo_w_in);
}
/* Module Definition */

/* Module Definition */
void cin_PE_dummy_in(int idx, hls::stream<cin_t8> &fifo_cin_in) {
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 9; c0 += 1)
    for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
      // array
      {
      }
      for (ap_uint<4> c3 = 0; c3 <= 7; c3 += 1) {
        // array
        // pe
        for (ap_uint<5> c5 = 0; c5 <= 13; c5 += 1)
          for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1)
            for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1)
              for (ap_uint<3> c8 = 0; c8 <= 2; c8 += 1)
                for (ap_uint<3> c9 = 0; c9 <= 2; c9 += 1) {
                  // latency
                  for (ap_uint<4> c10 = 0; c10 <= 6; c10 += 1) {
                    // latency
                    for (ap_uint<3> c11 = 0; c11 <= 3; c11 += 1) {
                      // latency
                      for (ap_uint<3> c12 = 0; c12 <= 3; c12 += 1) {
                      #pragma HLS PIPELINE II=1
                        cin_t8 fifo_data;
                        fifo_data = fifo_cin_in.read();
                      }
                    }
                  }
                }
      }
    }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_intra_trans(int idx, int c0, int c2, cout_t4 local_cout[56][14][1], hls::stream<float> &fifo_cout_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */


  // io_L1
  // pe
  for (ap_uint<5> c5 = 0; c5 <= 13; c5 += 1)
    for (ap_uint<2> c6 = 0; c6 <= 1; c6 += 1) {
      // latency
      for (ap_uint<4> c10 = 0; c10 <= 6; c10 += 1) {
        // latency
        for (ap_uint<3> c11 = 0; c11 <= 3; c11 += 1) {
          // latency
          for (ap_uint<3> c12 = 0; c12 <= 3; c12 += 1) {
          #pragma HLS PIPELINE II=1
            // simd
            {
              cout_t1 in_data;
              cout_t4 out_data;
              in_data = fifo_cout_drain_local_in.read();
              int split_idx = (c12) % 4;
              out_data = local_cout[c11 + 4*c5][c10 + 7*c6][c12 / 4];
              for (ap_uint<3> n = 0; n < 4; n++) {
              #pragma HLS UNROLL
                data_split[n] = out_data(31, 0);
                out_data = out_data >> 32;
              }
              union {unsigned int ui; float ut;} u;
              u.ut = in_data;
              data_split[split_idx] = ap_uint<32>(u.ui);
              out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);              local_cout[c11 + 4*c5][c10 + 7*c6][c12 / 4] = out_data;
            }
          }
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L1_out_inter_trans(int idx, int c0, int c2, cout_t4 local_cout[56][14][1], hls::stream<cout_t4> &fifo_cout_drain_in, hls::stream<cout_t4> &fifo_cout_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<5> c4 = p0; c4 <= 15; c4 += 1) {
    // io_L1
    if (c4 == p0) {
      for (ap_uint<7> c5 = 0; c5 <= 55; c5 += 1)
        for (ap_uint<5> c6 = 0; c6 <= 13; c6 += 1) {
        #pragma HLS PIPELINE II=1
          // access_coalesce
          {
            cout_t4 in_data;
            cout_t4 out_data;
            in_data = local_cout[c5][c6][0];
            out_data = in_data;
            fifo_cout_drain_out.write(out_data);
          }
        }
    } else {
      for (ap_uint<7> c5 = 0; c5 <= 55; c5 += 1)
        for (ap_uint<5> c6 = 0; c6 <= 13; c6 += 1) {
        #pragma HLS PIPELINE II=1
          // access_coalesce
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
void cout_drain_IO_L1_out_inter_trans_boundary(int idx, int c0, int c2, cout_t4 local_cout[56][14][1], hls::stream<cout_t4> &fifo_cout_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  int p0 = idx; // module id
  /* Variable Declaration */

  for (ap_uint<5> c4 = p0; c4 <= 15; c4 += 1)
    if (c4 == p0) {
      // io_L1
      for (ap_uint<7> c5 = 0; c5 <= 55; c5 += 1)
        for (ap_uint<5> c6 = 0; c6 <= 13; c6 += 1) {
        #pragma HLS PIPELINE II=1
          // access_coalesce
          {
            cout_t4 in_data;
            cout_t4 out_data;
            in_data = local_cout[c5][c6][0];
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
  cout_t4 local_cout[56][14][1];
  #pragma HLS RESOURCE variable=local_cout core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 9; c0 += 1)
    for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
      // array
      // io_L2
      cout_drain_IO_L1_out_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0, 
        /* host iter */ c2, 
        /* array */ local_cout, 
        /* fifo */ fifo_cout_drain_local_in
      );
      cout_drain_IO_L1_out_inter_trans(
        /* module id */ idx, 
        /* host iter */ c0, 
        /* host iter */ c2, 
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
  cout_t4 local_cout[56][14][1];
  #pragma HLS RESOURCE variable=local_cout core=RAM_2P_BRAM
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 9; c0 += 1)
    for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
      // array
      // io_L2
      cout_drain_IO_L1_out_intra_trans(
        /* module id */ idx, 
        /* host iter */ c0, 
        /* host iter */ c2, 
        /* array */ local_cout, 
        /* fifo */ fifo_cout_drain_local_in
      );
      cout_drain_IO_L1_out_inter_trans_boundary(
        /* module id */ idx, 
        /* host iter */ c0, 
        /* host iter */ c2, 
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
void cout_drain_IO_L2_out(hls::stream<cout_t4> &fifo_cout_drain_out, hls::stream<cout_t4> &fifo_cout_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<5> c0 = 0; c0 <= 9; c0 += 1)
    for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
      // array
      // io_L2
      for (ap_uint<5> c4 = 0; c4 <= 15; c4 += 1) {
        // io_L1
        for (ap_uint<7> c5 = 0; c5 <= 55; c5 += 1)
          for (ap_uint<5> c6 = 0; c6 <= 13; c6 += 1) {
          #pragma HLS PIPELINE II=1
            // access_coalesce
            // access_serialize
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
/* Module Definition */

/* Module Definition */
void cout_drain_IO_L2_out_serialize(cout_t16 *cout, hls::stream<cout_t4> &fifo_cout_drain_local_in) {
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
  /* cin_IO_L2_in_serialize fifo */ hls::stream<cin_t16> fifo_cin_cin_IO_L2_in_serialize;
  #pragma HLS STREAM variable=fifo_cin_cin_IO_L2_in_serialize depth=2
  /* w_IO_L2_in_serialize fifo */ hls::stream<w_t8> fifo_w_w_IO_L2_in_serialize;
  #pragma HLS STREAM variable=fifo_w_w_IO_L2_in_serialize depth=2
  /* cout_drain_IO_L2_out_serialize fifo */ hls::stream<cout_t4> fifo_cout_drain_cout_drain_IO_L2_out_serialize;
  #pragma HLS STREAM variable=fifo_cout_drain_cout_drain_IO_L2_out_serialize depth=2
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_0;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_0 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_1;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_1 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_2;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_2 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_3;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_3 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_4;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_4 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_5;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_5 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_6;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_6 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_7;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_7 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_8;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_8 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_9;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_9 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_9 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_10;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_10 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_10 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_11;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_11 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_11 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_12;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_12 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_12 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_13;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_13 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_13 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_14;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_14 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_14 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_15;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_15 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_15 core=FIFO_SRL
  /* w_IO_L1_in fifo */ hls::stream<w_t8> fifo_w_w_IO_L1_in_16;
  #pragma HLS STREAM variable=fifo_w_w_IO_L1_in_16 depth=2
  #pragma HLS RESOURCE variable=fifo_w_w_IO_L1_in_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_0;
  #pragma HLS STREAM variable=fifo_cin_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_1;
  #pragma HLS STREAM variable=fifo_cin_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_2;
  #pragma HLS STREAM variable=fifo_cin_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_3;
  #pragma HLS STREAM variable=fifo_cin_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_4;
  #pragma HLS STREAM variable=fifo_cin_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_5;
  #pragma HLS STREAM variable=fifo_cin_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_6;
  #pragma HLS STREAM variable=fifo_cin_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_7;
  #pragma HLS STREAM variable=fifo_cin_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_8;
  #pragma HLS STREAM variable=fifo_cin_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_9;
  #pragma HLS STREAM variable=fifo_cin_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_10;
  #pragma HLS STREAM variable=fifo_cin_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_11;
  #pragma HLS STREAM variable=fifo_cin_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_12;
  #pragma HLS STREAM variable=fifo_cin_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_13;
  #pragma HLS STREAM variable=fifo_cin_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_14;
  #pragma HLS STREAM variable=fifo_cin_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_15;
  #pragma HLS STREAM variable=fifo_cin_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<cin_t8> fifo_cin_PE_16;
  #pragma HLS STREAM variable=fifo_cin_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_cin_PE_16 core=FIFO_SRL
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
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_0;
  #pragma HLS STREAM variable=fifo_w_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_1;
  #pragma HLS STREAM variable=fifo_w_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_2;
  #pragma HLS STREAM variable=fifo_w_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_3;
  #pragma HLS STREAM variable=fifo_w_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_4;
  #pragma HLS STREAM variable=fifo_w_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_5;
  #pragma HLS STREAM variable=fifo_w_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_6;
  #pragma HLS STREAM variable=fifo_w_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_7;
  #pragma HLS STREAM variable=fifo_w_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_8;
  #pragma HLS STREAM variable=fifo_w_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_9;
  #pragma HLS STREAM variable=fifo_w_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_10;
  #pragma HLS STREAM variable=fifo_w_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_11;
  #pragma HLS STREAM variable=fifo_w_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_12;
  #pragma HLS STREAM variable=fifo_w_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_13;
  #pragma HLS STREAM variable=fifo_w_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_14;
  #pragma HLS STREAM variable=fifo_w_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<w_t8> fifo_w_PE_15;
  #pragma HLS STREAM variable=fifo_w_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_w_PE_15 core=FIFO_SRL
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
  /* FIFO Declaration */

  /* Module Call */
  cin_IO_L2_in_boundary_serialize(
    /* array */ cin,
    /* fifo */ fifo_cin_cin_IO_L2_in_serialize
  );
  /* Module Call */

  /* Module Call */
  cin_IO_L2_in_boundary(
    /* fifo */ fifo_cin_cin_IO_L2_in_serialize,
    /* fifo */ fifo_cin_PE_0
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in_serialize(
    /* array */ w,
    /* fifo */ fifo_w_w_IO_L2_in_serialize
  );
  /* Module Call */

  /* Module Call */
  w_IO_L2_in(
    /* fifo */ fifo_w_w_IO_L2_in_serialize,
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
  w_IO_L1_in_boundary_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_w_w_IO_L1_in_15,
    /* fifo */ fifo_w_PE_15
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 0,
    /* fifo */ fifo_cin_PE_0,
    /* fifo */ fifo_cin_PE_1,
    /* fifo */ fifo_cout_drain_PE_0,
    /* fifo */ fifo_w_PE_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 1,
    /* fifo */ fifo_cin_PE_1,
    /* fifo */ fifo_cin_PE_2,
    /* fifo */ fifo_cout_drain_PE_1,
    /* fifo */ fifo_w_PE_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 2,
    /* fifo */ fifo_cin_PE_2,
    /* fifo */ fifo_cin_PE_3,
    /* fifo */ fifo_cout_drain_PE_2,
    /* fifo */ fifo_w_PE_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 3,
    /* fifo */ fifo_cin_PE_3,
    /* fifo */ fifo_cin_PE_4,
    /* fifo */ fifo_cout_drain_PE_3,
    /* fifo */ fifo_w_PE_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 4,
    /* fifo */ fifo_cin_PE_4,
    /* fifo */ fifo_cin_PE_5,
    /* fifo */ fifo_cout_drain_PE_4,
    /* fifo */ fifo_w_PE_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 5,
    /* fifo */ fifo_cin_PE_5,
    /* fifo */ fifo_cin_PE_6,
    /* fifo */ fifo_cout_drain_PE_5,
    /* fifo */ fifo_w_PE_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 6,
    /* fifo */ fifo_cin_PE_6,
    /* fifo */ fifo_cin_PE_7,
    /* fifo */ fifo_cout_drain_PE_6,
    /* fifo */ fifo_w_PE_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 7,
    /* fifo */ fifo_cin_PE_7,
    /* fifo */ fifo_cin_PE_8,
    /* fifo */ fifo_cout_drain_PE_7,
    /* fifo */ fifo_w_PE_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 8,
    /* fifo */ fifo_cin_PE_8,
    /* fifo */ fifo_cin_PE_9,
    /* fifo */ fifo_cout_drain_PE_8,
    /* fifo */ fifo_w_PE_8
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 9,
    /* fifo */ fifo_cin_PE_9,
    /* fifo */ fifo_cin_PE_10,
    /* fifo */ fifo_cout_drain_PE_9,
    /* fifo */ fifo_w_PE_9
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 10,
    /* fifo */ fifo_cin_PE_10,
    /* fifo */ fifo_cin_PE_11,
    /* fifo */ fifo_cout_drain_PE_10,
    /* fifo */ fifo_w_PE_10
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 11,
    /* fifo */ fifo_cin_PE_11,
    /* fifo */ fifo_cin_PE_12,
    /* fifo */ fifo_cout_drain_PE_11,
    /* fifo */ fifo_w_PE_11
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 12,
    /* fifo */ fifo_cin_PE_12,
    /* fifo */ fifo_cin_PE_13,
    /* fifo */ fifo_cout_drain_PE_12,
    /* fifo */ fifo_w_PE_12
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 13,
    /* fifo */ fifo_cin_PE_13,
    /* fifo */ fifo_cin_PE_14,
    /* fifo */ fifo_cout_drain_PE_13,
    /* fifo */ fifo_w_PE_13
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 14,
    /* fifo */ fifo_cin_PE_14,
    /* fifo */ fifo_cin_PE_15,
    /* fifo */ fifo_cout_drain_PE_14,
    /* fifo */ fifo_w_PE_14
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper(
    /* module id */ 15,
    /* fifo */ fifo_cin_PE_15,
    /* fifo */ fifo_cin_PE_16,
    /* fifo */ fifo_cout_drain_PE_15,
    /* fifo */ fifo_w_PE_15
  );
  /* Module Call */

  /* Module Call */
  cin_PE_dummy_in(
    /* module id */ 15,
    /* fifo */ fifo_cin_PE_16
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L1_out_boundary_wrapper(
    /* module id */ 15,
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
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_serialize,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L1_out_0
  );
  /* Module Call */

  /* Module Call */
  cout_drain_IO_L2_out_serialize(
    /* array */ cout,
    /* fifo */ fifo_cout_drain_cout_drain_IO_L2_out_serialize
  );
  /* Module Call */

}
}

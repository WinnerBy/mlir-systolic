// AutoSA Command:
// /workspaces/mlir-systolic/third_party/AutoSA/autosa /workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/lu/kernel.c --config=/workspaces/mlir-systolic/third_party/AutoSA/autosa_config/autosa_config.json --target=autosa_hls_c --output-dir=/workspaces/mlir-systolic/third_party/AutoSA/autosa.tmp/output --host-serialize --simd-info=/workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/lu/simd_info.json --sa-sizes={kernel[]->space_time[1];kernel[]->array_part[-1,-1,-1];kernel[]->latency[4,4];kernel[]->simd[1,1]} --use-cplusplus-template --no-reschedule --live-range-reordering

#include <ap_int.h>
#include <hls_stream.h>

#define min(x,y) ((x < y) ? x : y)
#define max(x,y) ((x > y) ? x : y)

/* Data Type */
typedef float prev_V_t1;
typedef float V_t1;
typedef float U_tmp_t1;
typedef float L_tmp_t1;
typedef float A_t1;
typedef float L_t1;
typedef float U_t1;
typedef ap_uint<512> U_tmp_t16;
typedef ap_uint<512> A_t16;
typedef ap_uint<512> U_t16;
typedef ap_uint<128> U_t4;
/* Data Type */

extern "C" {
void kernel0(A_t16 *A, L_t1 *L, U_t16 *U);
}
template<int p0>
void A_IO_L1_in_intra_trans(float local_A[32][1], hls::stream<float> &fifo_A_local_out);
template<int p0>
void A_IO_L1_in_inter_trans(float local_A[32][1], hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out);
template<int p0>
void A_IO_L1_in_inter_trans_boundary(float local_A[32][1], hls::stream<float> &fifo_A_in);
template<int p0>
void A_IO_L1_in_wrapper(hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out, hls::stream<float> &fifo_A_local_out);
template<int p0>
void A_IO_L1_in_boundary_wrapper(hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_local_out);
template<int p0>
void PE_wrapper(hls::stream<float> &fifo_U_tmp_1_in, hls::stream<float> &fifo_U_tmp_1_out, hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_U_drain_out);
template<int p0>
void L_drain_IO_L1_out_intra_trans(float local_L[31][1], hls::stream<float> &fifo_L_drain_local_in);
template<int p0>
void L_drain_IO_L1_out_inter_trans(float local_L[31][1], hls::stream<float> &fifo_L_drain_in, hls::stream<float> &fifo_L_drain_out);
template<int p0>
void L_drain_IO_L1_out_inter_trans_boundary(float local_L[31][1], hls::stream<float> &fifo_L_drain_out);
template<int p0>
void L_drain_IO_L1_out_wrapper(hls::stream<float> &fifo_L_drain_in, hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_L_drain_local_in);
template<int p0>
void L_drain_IO_L1_out_boundary_wrapper(hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_L_drain_local_in);
template<int p0>
void U_drain_IO_L1_out_intra_trans(U_t4 local_U[1][8], hls::stream<float> &fifo_U_drain_local_in);
template<int p0>
void U_drain_IO_L1_out_inter_trans(U_t4 local_U[1][8], hls::stream<U_t4> &fifo_U_drain_in, hls::stream<U_t4> &fifo_U_drain_out);
template<int p0>
void U_drain_IO_L1_out_inter_trans_boundary(U_t4 local_U[1][8], hls::stream<U_t4> &fifo_U_drain_out);
template<int p0>
void U_drain_IO_L1_out_wrapper(hls::stream<U_t4> &fifo_U_drain_in, hls::stream<U_t4> &fifo_U_drain_out, hls::stream<float> &fifo_U_drain_local_in);
template<int p0>
void U_drain_IO_L1_out_boundary_wrapper(hls::stream<U_t4> &fifo_U_drain_out, hls::stream<float> &fifo_U_drain_local_in);

/* Module Definition */
void A_IO_L2_in(hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L2
  for (ap_uint<6> c0 = 0; c0 <= 31; c0 += 1) {
    // io_L1
    for (ap_uint<6> c1 = 0; c1 <= 31; c1 += 1) {
    #pragma HLS PIPELINE II=1
      // access_coalesce
      // access_serialize
      {
        A_t1 in_data;
        A_t1 out_data;
        in_data = fifo_A_in.read();
        out_data = in_data;
        fifo_A_local_out.write(out_data);
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void A_IO_L2_in_serialize(A_t16 *A, hls::stream<float> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  A_t1 fifo_data;
  A_t16 mem_data;
  union {unsigned int ui; float ut;} u;
  for (ap_uint<7> i = 0; i < 64; i++) {
  #pragma HLS PIPELINE II=1
    mem_data = A[i];
    for (ap_uint<5> p = 0; p < 16; p++) {
      u.ui = (unsigned int)mem_data(31, 0);
      fifo_data = u.ut;

      mem_data = mem_data >> 32;
      fifo_A_local_out.write(fifo_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void A_IO_L1_in_intra_trans(float local_A[32][1], hls::stream<float> &fifo_A_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  /* Variable Declaration */


  // array
  // io_L2
  // io_L1
  // pe
  for (ap_uint<6> c2 = 0; c2 <= 31; c2 += 1) {
  #pragma HLS PIPELINE II=1
    // hls_pipeline
    {
      A_t1 in_data;
      A_t1 out_data;
      in_data = local_A[c2][0];
      out_data = in_data;
      fifo_A_local_out.write(out_data);
    }
  }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void A_IO_L1_in_inter_trans(float local_A[32][1], hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<6> c0 = p0; c0 <= 31; c0 += 1) {
    // io_L1
    if (c0 == p0) {
      for (ap_uint<6> c1 = 0; c1 <= 31; c1 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          A_t1 in_data;
          A_t1 out_data;
          in_data = fifo_A_in.read();
          out_data = in_data;
          local_A[c1][0] = out_data;
        }
      }
    } else {
      for (ap_uint<6> c1 = 0; c1 <= 31; c1 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          A_t1 in_data;
          A_t1 out_data;
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
template<int p0>
void A_IO_L1_in_inter_trans_boundary(float local_A[32][1], hls::stream<float> &fifo_A_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<6> c0 = p0; c0 <= 31; c0 += 1)
    if (c0 == p0) {
      // io_L1
      for (ap_uint<6> c1 = 0; c1 <= 31; c1 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          A_t1 in_data;
          A_t1 out_data;
          in_data = fifo_A_in.read();
          out_data = in_data;
          local_A[c1][0] = out_data;
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void A_IO_L1_in(hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out, hls::stream<float> &fifo_A_local_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  A_t1 local_A[32][1];
  #pragma HLS RESOURCE variable=local_A core=RAM_1P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  A_IO_L1_in_inter_trans<p0>(
    /* array */ local_A, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_out
  );
  A_IO_L1_in_intra_trans<p0>(
    /* array */ local_A, 
    /* fifo */ fifo_A_local_out
  );
}
/* Module Definition */

/* Module Definition */
template<int p0>
void A_IO_L1_in_wrapper(hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out, hls::stream<float> &fifo_A_local_out)
 {
  A_IO_L1_in<p0>(
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_out, 
    /* fifo */ fifo_A_local_out);
}
/* Module Definition */

/* Module Definition */
template<int p0>
void A_IO_L1_in_boundary(hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_local_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  A_t1 local_A[32][1];
  #pragma HLS RESOURCE variable=local_A core=RAM_1P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  A_IO_L1_in_inter_trans_boundary<p0>(
    /* array */ local_A, 
    /* fifo */ fifo_A_in
  );
  A_IO_L1_in_intra_trans<p0>(
    /* array */ local_A, 
    /* fifo */ fifo_A_local_out
  );
}
/* Module Definition */

/* Module Definition */
template<int p0>
void A_IO_L1_in_boundary_wrapper(hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_local_out)
 {
  A_IO_L1_in_boundary<p0>(
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_A_local_out);
}
/* Module Definition */

/* Module Definition */
template<int p0>
void PE(hls::stream<float> &fifo_U_tmp_1_in, hls::stream<float> &fifo_U_tmp_1_out, hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_U_drain_out) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  prev_V_t1 local_prev_V[32][1];
  #pragma HLS RESOURCE variable=local_prev_V core=RAM_2P_BRAM
  V_t1 local_V[32][1];
  #pragma HLS RESOURCE variable=local_V core=RAM_2P_BRAM
  U_tmp_t1 local_U_tmp[32][2];
  #pragma HLS RESOURCE variable=local_U_tmp core=RAM_2P_BRAM
  L_tmp_t1 local_L_tmp[32][1];
  #pragma HLS RESOURCE variable=local_L_tmp core=RAM_2P_BRAM
  A_t1 local_A[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_A dim=0 complete
  L_t1 local_L[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_L dim=0 complete
  U_t1 local_U[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_U dim=0 complete
  /* Variable Declaration */

  // array
  // pe
  for (int c1 = 0; c1 <= p0; c1 += 1)
    for (ap_uint<6> c2 = c1; c2 <= 31; c2 += 1) {
    #pragma HLS PIPELINE II=1
      {
        if (c1 == 0) {
          local_A[0][0] = fifo_A_in.read();
          local_prev_V[c2][0] = local_A[0][0];
        } else {
          local_prev_V[c2][0] = local_V[c2][0];
        }
        if (c1 == p0) {
          local_U_tmp[c2][1] = local_prev_V[c2][0];
          if (p0 <= 30)
            fifo_U_tmp_1_out.write(local_U_tmp[c2][1]);
          local_U[0][0] = local_U_tmp[c2][1];
          fifo_U_drain_out.write(local_U[0][0]);
        } else {
          local_U_tmp[c2][0] = fifo_U_tmp_1_in.read();
          local_U_tmp[c2][1] = local_U_tmp[c2][0];
          if (p0 <= 30)
            fifo_U_tmp_1_out.write(local_U_tmp[c2][1]);
          if (c2 == c1) {
            local_L_tmp[c1][0] = (local_prev_V[c1][0] / local_U_tmp[c1][1]);
            local_L[0][0] = local_L_tmp[c1][0];
            fifo_L_drain_out.write(local_L[0][0]);
          } else {
            local_L_tmp[c2][0] = local_L_tmp[c2 - 1][0];
          }
          local_V[c2][0] = (local_prev_V[c2][0] - (local_L_tmp[c2][0] * local_U_tmp[c2][1]));
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void PE_wrapper(hls::stream<float> &fifo_U_tmp_1_in, hls::stream<float> &fifo_U_tmp_1_out, hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_U_drain_out)
 {
  PE<p0>(
    /* fifo */ fifo_U_tmp_1_in, 
    /* fifo */ fifo_U_tmp_1_out, 
    /* fifo */ fifo_A_in, 
    /* fifo */ fifo_L_drain_out, 
    /* fifo */ fifo_U_drain_out);
}
/* Module Definition */

/* Module Definition */
template<int p0>
void L_drain_IO_L1_out_intra_trans(float local_L[31][1], hls::stream<float> &fifo_L_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  /* Variable Declaration */


  // array
  // io_L2
  // io_L1
  // pe
  for (int c1 = 0; c1 < p0; c1 += 1) {
  #pragma HLS PIPELINE II=1
    // hls_pipeline
    {
      L_t1 in_data;
      L_t1 out_data;
      in_data = fifo_L_drain_local_in.read();
      out_data = in_data;
      local_L[c1][0] = out_data;
    }
  }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void L_drain_IO_L1_out_inter_trans(float local_L[31][1], hls::stream<float> &fifo_L_drain_in, hls::stream<float> &fifo_L_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<6> c0 = max(1, p0); c0 <= 31; c0 += 1) {
    // io_L1
    if (c0 == p0) {
      for (ap_uint<6> c1 = 0; c1 <= 30; c1 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          L_t1 in_data;
          L_t1 out_data;
          in_data = local_L[c1][0];
          out_data = in_data;
          fifo_L_drain_out.write(out_data);
        }
      }
    } else {
      for (ap_uint<6> c1 = 0; c1 <= 30; c1 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          L_t1 in_data;
          L_t1 out_data;
          in_data = fifo_L_drain_in.read();
          out_data = in_data;
          fifo_L_drain_out.write(out_data);
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void L_drain_IO_L1_out_inter_trans_boundary(float local_L[31][1], hls::stream<float> &fifo_L_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<6> c0 = max(1, p0); c0 <= 31; c0 += 1)
    if (c0 == p0) {
      // io_L1
      for (ap_uint<6> c1 = 0; c1 <= 30; c1 += 1) {
      #pragma HLS PIPELINE II=1
        // access_coalesce
        {
          L_t1 in_data;
          L_t1 out_data;
          in_data = local_L[c1][0];
          out_data = in_data;
          fifo_L_drain_out.write(out_data);
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void L_drain_IO_L1_out(hls::stream<float> &fifo_L_drain_in, hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_L_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  L_t1 local_L[31][1];
  #pragma HLS RESOURCE variable=local_L core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  L_drain_IO_L1_out_intra_trans<p0>(
    /* array */ local_L, 
    /* fifo */ fifo_L_drain_local_in
  );
  L_drain_IO_L1_out_inter_trans<p0>(
    /* array */ local_L, 
    /* fifo */ fifo_L_drain_in, 
    /* fifo */ fifo_L_drain_out
  );
}
/* Module Definition */

/* Module Definition */
template<int p0>
void L_drain_IO_L1_out_wrapper(hls::stream<float> &fifo_L_drain_in, hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_L_drain_local_in)
 {
  L_drain_IO_L1_out<p0>(
    /* fifo */ fifo_L_drain_in, 
    /* fifo */ fifo_L_drain_out, 
    /* fifo */ fifo_L_drain_local_in);
}
/* Module Definition */

/* Module Definition */
template<int p0>
void L_drain_IO_L1_out_boundary(hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_L_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  L_t1 local_L[31][1];
  #pragma HLS RESOURCE variable=local_L core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  L_drain_IO_L1_out_intra_trans<p0>(
    /* array */ local_L, 
    /* fifo */ fifo_L_drain_local_in
  );
  L_drain_IO_L1_out_inter_trans_boundary<p0>(
    /* array */ local_L, 
    /* fifo */ fifo_L_drain_out
  );
}
/* Module Definition */

/* Module Definition */
template<int p0>
void L_drain_IO_L1_out_boundary_wrapper(hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_L_drain_local_in)
 {
  L_drain_IO_L1_out_boundary<p0>(
    /* fifo */ fifo_L_drain_out, 
    /* fifo */ fifo_L_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void L_drain_IO_L2_out(hls::stream<float> &fifo_L_drain_out, hls::stream<float> &fifo_L_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L2
  for (ap_uint<6> c0 = 1; c0 <= 31; c0 += 1) {
    // io_L1
    for (ap_uint<6> c1 = 0; c1 <= 30; c1 += 1) {
    #pragma HLS PIPELINE II=1
      // access_coalesce
      // access_serialize
      {
        L_t1 in_data;
        L_t1 out_data;
        in_data = fifo_L_drain_local_in.read();
        out_data = in_data;
        fifo_L_drain_out.write(out_data);
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void L_drain_IO_L2_out_serialize(L_t1 *L, hls::stream<float> &fifo_L_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<11> i = 0; i < 961; i++) {
  #pragma HLS PIPELINE II=1
    L_t1 fifo_data;
    fifo_data = fifo_L_drain_local_in.read();
    L[i] = fifo_data;
  }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void U_drain_IO_L1_out_intra_trans(U_t4 local_U[1][8], hls::stream<float> &fifo_U_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  ap_uint<32> data_split[4];
  #pragma HLS ARRAY_PARTITION variable=data_split complete
  /* Variable Declaration */


  // array
  // io_L2
  // io_L1
  // pe
  for (ap_uint<6> c2 = p0; c2 <= 31; c2 += 1) {
  #pragma HLS PIPELINE II=1
    // hls_pipeline
    {
      U_t1 in_data;
      U_t4 out_data;
      in_data = fifo_U_drain_local_in.read();
      int split_idx = (c2 - p0) % 4;
      out_data = local_U[0][(-p0 + c2) / 4];
      for (ap_uint<3> n = 0; n < 4; n++) {
      #pragma HLS UNROLL
        data_split[n] = out_data(31, 0);
        out_data = out_data >> 32;
      }
      union {unsigned int ui; float ut;} u;
      u.ut = in_data;
      data_split[split_idx] = ap_uint<32>(u.ui);
      out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);      local_U[0][(-p0 + c2) / 4] = out_data;
    }
  }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void U_drain_IO_L1_out_inter_trans(U_t4 local_U[1][8], hls::stream<U_t4> &fifo_U_drain_in, hls::stream<U_t4> &fifo_U_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<6> c0 = p0; c0 <= 31; c0 += 1) {
    // io_L1
    if (c0 == p0) {
      // access_coalesce
      for (int c2 = 0; c2 <= -((p0 + 4) / 4) + 8; c2 += 1) {
      #pragma HLS PIPELINE II=1
        {
          U_t4 in_data;
          U_t4 out_data;
          in_data = local_U[0][c2];
          out_data = in_data;
          fifo_U_drain_out.write(out_data);
        }
      }
    } else {
      // access_coalesce
      for (int c2 = 0; c2 <= -((c0 + 4) / 4) + 8; c2 += 1) {
      #pragma HLS PIPELINE II=1
        {
          U_t4 in_data;
          U_t4 out_data;
          in_data = fifo_U_drain_in.read();
          out_data = in_data;
          fifo_U_drain_out.write(out_data);
        }
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void U_drain_IO_L1_out_inter_trans_boundary(U_t4 local_U[1][8], hls::stream<U_t4> &fifo_U_drain_out) {
#pragma HLS INLINE
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<6> c0 = p0; c0 <= 31; c0 += 1)
    if (c0 == p0) {
      // io_L1
      // access_coalesce
      for (int c2 = 0; c2 <= -((p0 + 4) / 4) + 8; c2 += 1) {
      #pragma HLS PIPELINE II=1
        {
          U_t4 in_data;
          U_t4 out_data;
          in_data = local_U[0][c2];
          out_data = in_data;
          fifo_U_drain_out.write(out_data);
        }
      }
    }
}
/* Module Definition */

/* Module Definition */
template<int p0>
void U_drain_IO_L1_out(hls::stream<U_t4> &fifo_U_drain_in, hls::stream<U_t4> &fifo_U_drain_out, hls::stream<float> &fifo_U_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  U_t4 local_U[1][8];
  #pragma HLS RESOURCE variable=local_U core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  U_drain_IO_L1_out_intra_trans<p0>(
    /* array */ local_U, 
    /* fifo */ fifo_U_drain_local_in
  );
  U_drain_IO_L1_out_inter_trans<p0>(
    /* array */ local_U, 
    /* fifo */ fifo_U_drain_in, 
    /* fifo */ fifo_U_drain_out
  );
}
/* Module Definition */

/* Module Definition */
template<int p0>
void U_drain_IO_L1_out_wrapper(hls::stream<U_t4> &fifo_U_drain_in, hls::stream<U_t4> &fifo_U_drain_out, hls::stream<float> &fifo_U_drain_local_in)
 {
  U_drain_IO_L1_out<p0>(
    /* fifo */ fifo_U_drain_in, 
    /* fifo */ fifo_U_drain_out, 
    /* fifo */ fifo_U_drain_local_in);
}
/* Module Definition */

/* Module Definition */
template<int p0>
void U_drain_IO_L1_out_boundary(hls::stream<U_t4> &fifo_U_drain_out, hls::stream<float> &fifo_U_drain_local_in) {
#pragma HLS INLINE
  /* Variable Declaration */
  U_t4 local_U[1][8];
  #pragma HLS RESOURCE variable=local_U core=RAM_2P_BRAM
  /* Variable Declaration */

  // array
  // io_L2
  U_drain_IO_L1_out_intra_trans<p0>(
    /* array */ local_U, 
    /* fifo */ fifo_U_drain_local_in
  );
  U_drain_IO_L1_out_inter_trans_boundary<p0>(
    /* array */ local_U, 
    /* fifo */ fifo_U_drain_out
  );
}
/* Module Definition */

/* Module Definition */
template<int p0>
void U_drain_IO_L1_out_boundary_wrapper(hls::stream<U_t4> &fifo_U_drain_out, hls::stream<float> &fifo_U_drain_local_in)
 {
  U_drain_IO_L1_out_boundary<p0>(
    /* fifo */ fifo_U_drain_out, 
    /* fifo */ fifo_U_drain_local_in);
}
/* Module Definition */

/* Module Definition */
void U_drain_IO_L2_out(hls::stream<U_t4> &fifo_U_drain_out, hls::stream<U_t4> &fifo_U_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  // array
  // io_L2
  for (ap_uint<6> c0 = 0; c0 <= 31; c0 += 1) {
    // io_L1
    // access_coalesce
    // access_serialize
    for (int c2 = 0; c2 <= -((c0 + 4) / 4) + 8; c2 += 1) {
    #pragma HLS PIPELINE II=1
      {
        U_t4 in_data;
        U_t4 out_data;
        in_data = fifo_U_drain_local_in.read();
        out_data = in_data;
        fifo_U_drain_out.write(out_data);
      }
    }
  }
}
/* Module Definition */

/* Module Definition */
void U_drain_IO_L2_out_serialize(U_t16 *U, hls::stream<U_t4> &fifo_U_drain_local_in) {
#pragma HLS INLINE OFF
  /* Variable Declaration */
  /* Variable Declaration */

  for (ap_uint<7> i = 0; i < 33; i++) {
  #pragma HLS PIPELINE II=1
    U_t4 fifo_data;
    U_t16 mem_data;
    U_t4 mem_data_split[4];
    #pragma HLS ARRAY_PARTITION variable=mem_data_split complete
    for (ap_uint<3> p = 0; p < 4; p++) {
      fifo_data = fifo_U_drain_local_in.read();
      mem_data_split[p] = fifo_data;
    }
    mem_data = (mem_data_split[3], mem_data_split[2], mem_data_split[1], mem_data_split[0]);
    U[i] = mem_data;
  }
}
/* Module Definition */

extern "C" {
void kernel0(A_t16 *A, L_t1 *L, U_t16 *U)
{
#pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem_A
#pragma HLS INTERFACE m_axi port=L offset=slave bundle=gmem_L
#pragma HLS INTERFACE m_axi port=U offset=slave bundle=gmem_U
#pragma HLS INTERFACE s_axilite port=A bundle=control
#pragma HLS INTERFACE s_axilite port=L bundle=control
#pragma HLS INTERFACE s_axilite port=U bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATAFLOW

  /* FIFO Declaration */
  /* A_IO_L2_in_serialize fifo */ hls::stream<float> fifo_A_A_IO_L2_in_serialize;
  #pragma HLS STREAM variable=fifo_A_A_IO_L2_in_serialize depth=2
  /* L_drain_IO_L2_out_serialize fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L2_out_serialize;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L2_out_serialize depth=2
  /* U_drain_IO_L2_out_serialize fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L2_out_serialize;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L2_out_serialize depth=2
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_0;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_0 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_1;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_1 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_2;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_2 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_3;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_3 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_4;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_4 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_5;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_5 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_6;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_6 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_7;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_7 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_8;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_8 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_9;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_9 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_9 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_10;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_10 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_10 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_11;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_11 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_11 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_12;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_12 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_12 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_13;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_13 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_13 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_14;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_14 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_14 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_15;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_15 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_15 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_16;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_16 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_16 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_17;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_17 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_17 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_18;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_18 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_18 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_19;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_19 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_19 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_20;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_20 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_20 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_21;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_21 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_21 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_22;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_22 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_22 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_23;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_23 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_23 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_24;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_24 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_24 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_25;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_25 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_25 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_26;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_26 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_26 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_27;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_27 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_27 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_28;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_28 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_28 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_29;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_29 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_29 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_30;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_30 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_30 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_31;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_31 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_31 core=FIFO_SRL
  /* A_IO_L1_in fifo */ hls::stream<float> fifo_A_A_IO_L1_in_32;
  #pragma HLS STREAM variable=fifo_A_A_IO_L1_in_32 depth=2
  #pragma HLS RESOURCE variable=fifo_A_A_IO_L1_in_32 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_0;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_1;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_2;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_3;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_4;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_5;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_6;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_7;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_8;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_9;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_10;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_11;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_12;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_13;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_14;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_15;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_16;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_17;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_18;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_19;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_20;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_21;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_22;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_23;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_24;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_25;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_26;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_27;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_28;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_29;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_30;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_31;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_tmp_1_PE_32;
  #pragma HLS STREAM variable=fifo_U_tmp_1_PE_32 depth=2
  #pragma HLS RESOURCE variable=fifo_U_tmp_1_PE_32 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_0;
  #pragma HLS STREAM variable=fifo_A_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_1;
  #pragma HLS STREAM variable=fifo_A_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_2;
  #pragma HLS STREAM variable=fifo_A_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_3;
  #pragma HLS STREAM variable=fifo_A_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_4;
  #pragma HLS STREAM variable=fifo_A_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_5;
  #pragma HLS STREAM variable=fifo_A_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_6;
  #pragma HLS STREAM variable=fifo_A_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_7;
  #pragma HLS STREAM variable=fifo_A_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_8;
  #pragma HLS STREAM variable=fifo_A_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_9;
  #pragma HLS STREAM variable=fifo_A_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_10;
  #pragma HLS STREAM variable=fifo_A_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_11;
  #pragma HLS STREAM variable=fifo_A_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_12;
  #pragma HLS STREAM variable=fifo_A_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_13;
  #pragma HLS STREAM variable=fifo_A_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_14;
  #pragma HLS STREAM variable=fifo_A_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_15;
  #pragma HLS STREAM variable=fifo_A_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_16;
  #pragma HLS STREAM variable=fifo_A_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_17;
  #pragma HLS STREAM variable=fifo_A_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_18;
  #pragma HLS STREAM variable=fifo_A_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_19;
  #pragma HLS STREAM variable=fifo_A_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_20;
  #pragma HLS STREAM variable=fifo_A_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_21;
  #pragma HLS STREAM variable=fifo_A_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_22;
  #pragma HLS STREAM variable=fifo_A_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_23;
  #pragma HLS STREAM variable=fifo_A_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_24;
  #pragma HLS STREAM variable=fifo_A_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_25;
  #pragma HLS STREAM variable=fifo_A_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_26;
  #pragma HLS STREAM variable=fifo_A_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_27;
  #pragma HLS STREAM variable=fifo_A_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_28;
  #pragma HLS STREAM variable=fifo_A_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_29;
  #pragma HLS STREAM variable=fifo_A_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_30;
  #pragma HLS STREAM variable=fifo_A_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_A_PE_31;
  #pragma HLS STREAM variable=fifo_A_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_A_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_0;
  #pragma HLS STREAM variable=fifo_L_drain_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_1;
  #pragma HLS STREAM variable=fifo_L_drain_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_2;
  #pragma HLS STREAM variable=fifo_L_drain_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_3;
  #pragma HLS STREAM variable=fifo_L_drain_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_4;
  #pragma HLS STREAM variable=fifo_L_drain_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_5;
  #pragma HLS STREAM variable=fifo_L_drain_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_6;
  #pragma HLS STREAM variable=fifo_L_drain_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_7;
  #pragma HLS STREAM variable=fifo_L_drain_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_8;
  #pragma HLS STREAM variable=fifo_L_drain_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_9;
  #pragma HLS STREAM variable=fifo_L_drain_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_10;
  #pragma HLS STREAM variable=fifo_L_drain_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_11;
  #pragma HLS STREAM variable=fifo_L_drain_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_12;
  #pragma HLS STREAM variable=fifo_L_drain_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_13;
  #pragma HLS STREAM variable=fifo_L_drain_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_14;
  #pragma HLS STREAM variable=fifo_L_drain_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_15;
  #pragma HLS STREAM variable=fifo_L_drain_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_16;
  #pragma HLS STREAM variable=fifo_L_drain_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_17;
  #pragma HLS STREAM variable=fifo_L_drain_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_18;
  #pragma HLS STREAM variable=fifo_L_drain_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_19;
  #pragma HLS STREAM variable=fifo_L_drain_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_20;
  #pragma HLS STREAM variable=fifo_L_drain_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_21;
  #pragma HLS STREAM variable=fifo_L_drain_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_22;
  #pragma HLS STREAM variable=fifo_L_drain_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_23;
  #pragma HLS STREAM variable=fifo_L_drain_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_24;
  #pragma HLS STREAM variable=fifo_L_drain_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_25;
  #pragma HLS STREAM variable=fifo_L_drain_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_26;
  #pragma HLS STREAM variable=fifo_L_drain_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_27;
  #pragma HLS STREAM variable=fifo_L_drain_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_28;
  #pragma HLS STREAM variable=fifo_L_drain_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_29;
  #pragma HLS STREAM variable=fifo_L_drain_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_30;
  #pragma HLS STREAM variable=fifo_L_drain_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_L_drain_PE_31;
  #pragma HLS STREAM variable=fifo_L_drain_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_PE_31 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_0;
  #pragma HLS STREAM variable=fifo_U_drain_PE_0 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_0 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_1;
  #pragma HLS STREAM variable=fifo_U_drain_PE_1 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_1 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_2;
  #pragma HLS STREAM variable=fifo_U_drain_PE_2 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_2 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_3;
  #pragma HLS STREAM variable=fifo_U_drain_PE_3 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_3 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_4;
  #pragma HLS STREAM variable=fifo_U_drain_PE_4 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_4 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_5;
  #pragma HLS STREAM variable=fifo_U_drain_PE_5 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_5 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_6;
  #pragma HLS STREAM variable=fifo_U_drain_PE_6 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_6 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_7;
  #pragma HLS STREAM variable=fifo_U_drain_PE_7 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_7 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_8;
  #pragma HLS STREAM variable=fifo_U_drain_PE_8 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_8 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_9;
  #pragma HLS STREAM variable=fifo_U_drain_PE_9 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_9 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_10;
  #pragma HLS STREAM variable=fifo_U_drain_PE_10 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_10 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_11;
  #pragma HLS STREAM variable=fifo_U_drain_PE_11 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_11 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_12;
  #pragma HLS STREAM variable=fifo_U_drain_PE_12 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_12 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_13;
  #pragma HLS STREAM variable=fifo_U_drain_PE_13 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_13 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_14;
  #pragma HLS STREAM variable=fifo_U_drain_PE_14 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_14 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_15;
  #pragma HLS STREAM variable=fifo_U_drain_PE_15 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_15 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_16;
  #pragma HLS STREAM variable=fifo_U_drain_PE_16 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_16 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_17;
  #pragma HLS STREAM variable=fifo_U_drain_PE_17 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_17 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_18;
  #pragma HLS STREAM variable=fifo_U_drain_PE_18 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_18 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_19;
  #pragma HLS STREAM variable=fifo_U_drain_PE_19 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_19 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_20;
  #pragma HLS STREAM variable=fifo_U_drain_PE_20 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_20 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_21;
  #pragma HLS STREAM variable=fifo_U_drain_PE_21 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_21 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_22;
  #pragma HLS STREAM variable=fifo_U_drain_PE_22 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_22 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_23;
  #pragma HLS STREAM variable=fifo_U_drain_PE_23 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_23 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_24;
  #pragma HLS STREAM variable=fifo_U_drain_PE_24 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_24 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_25;
  #pragma HLS STREAM variable=fifo_U_drain_PE_25 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_25 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_26;
  #pragma HLS STREAM variable=fifo_U_drain_PE_26 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_26 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_27;
  #pragma HLS STREAM variable=fifo_U_drain_PE_27 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_27 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_28;
  #pragma HLS STREAM variable=fifo_U_drain_PE_28 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_28 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_29;
  #pragma HLS STREAM variable=fifo_U_drain_PE_29 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_29 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_30;
  #pragma HLS STREAM variable=fifo_U_drain_PE_30 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_30 core=FIFO_SRL
  /* PE fifo */ hls::stream<float> fifo_U_drain_PE_31;
  #pragma HLS STREAM variable=fifo_U_drain_PE_31 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_PE_31 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_0;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_0 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_0 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_1;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_1 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_1 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_2;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_2 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_2 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_3;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_3 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_3 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_4;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_4 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_4 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_5;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_5 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_5 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_6;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_6 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_6 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_7;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_7 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_7 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_8;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_8 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_8 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_9;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_9 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_9 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_10;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_10 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_10 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_11;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_11 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_11 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_12;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_12 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_12 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_13;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_13 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_13 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_14;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_14 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_14 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_15;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_15 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_15 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_16;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_16 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_16 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_17;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_17 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_17 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_18;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_18 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_18 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_19;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_19 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_19 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_20;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_20 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_20 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_21;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_21 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_21 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_22;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_22 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_22 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_23;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_23 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_23 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_24;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_24 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_24 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_25;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_25 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_25 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_26;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_26 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_26 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_27;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_27 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_27 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_28;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_28 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_28 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_29;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_29 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_29 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_30;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_30 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_30 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_31;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_31 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_31 core=FIFO_SRL
  /* L_drain_IO_L1_out fifo */ hls::stream<float> fifo_L_drain_L_drain_IO_L1_out_32;
  #pragma HLS STREAM variable=fifo_L_drain_L_drain_IO_L1_out_32 depth=2
  #pragma HLS RESOURCE variable=fifo_L_drain_L_drain_IO_L1_out_32 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_0;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_0 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_0 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_1;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_1 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_1 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_2;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_2 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_2 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_3;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_3 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_3 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_4;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_4 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_4 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_5;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_5 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_5 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_6;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_6 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_6 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_7;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_7 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_7 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_8;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_8 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_8 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_9;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_9 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_9 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_10;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_10 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_10 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_11;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_11 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_11 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_12;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_12 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_12 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_13;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_13 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_13 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_14;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_14 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_14 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_15;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_15 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_15 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_16;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_16 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_16 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_17;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_17 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_17 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_18;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_18 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_18 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_19;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_19 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_19 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_20;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_20 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_20 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_21;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_21 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_21 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_22;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_22 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_22 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_23;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_23 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_23 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_24;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_24 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_24 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_25;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_25 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_25 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_26;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_26 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_26 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_27;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_27 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_27 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_28;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_28 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_28 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_29;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_29 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_29 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_30;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_30 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_30 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_31;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_31 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_31 core=FIFO_SRL
  /* U_drain_IO_L1_out fifo */ hls::stream<U_t4> fifo_U_drain_U_drain_IO_L1_out_32;
  #pragma HLS STREAM variable=fifo_U_drain_U_drain_IO_L1_out_32 depth=2
  #pragma HLS RESOURCE variable=fifo_U_drain_U_drain_IO_L1_out_32 core=FIFO_SRL
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
  A_IO_L1_in_wrapper<0>(
    /* fifo */ fifo_A_A_IO_L1_in_0,
    /* fifo */ fifo_A_A_IO_L1_in_1,
    /* fifo */ fifo_A_PE_0
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<1>(
    /* fifo */ fifo_A_A_IO_L1_in_1,
    /* fifo */ fifo_A_A_IO_L1_in_2,
    /* fifo */ fifo_A_PE_1
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<2>(
    /* fifo */ fifo_A_A_IO_L1_in_2,
    /* fifo */ fifo_A_A_IO_L1_in_3,
    /* fifo */ fifo_A_PE_2
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<3>(
    /* fifo */ fifo_A_A_IO_L1_in_3,
    /* fifo */ fifo_A_A_IO_L1_in_4,
    /* fifo */ fifo_A_PE_3
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<4>(
    /* fifo */ fifo_A_A_IO_L1_in_4,
    /* fifo */ fifo_A_A_IO_L1_in_5,
    /* fifo */ fifo_A_PE_4
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<5>(
    /* fifo */ fifo_A_A_IO_L1_in_5,
    /* fifo */ fifo_A_A_IO_L1_in_6,
    /* fifo */ fifo_A_PE_5
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<6>(
    /* fifo */ fifo_A_A_IO_L1_in_6,
    /* fifo */ fifo_A_A_IO_L1_in_7,
    /* fifo */ fifo_A_PE_6
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<7>(
    /* fifo */ fifo_A_A_IO_L1_in_7,
    /* fifo */ fifo_A_A_IO_L1_in_8,
    /* fifo */ fifo_A_PE_7
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<8>(
    /* fifo */ fifo_A_A_IO_L1_in_8,
    /* fifo */ fifo_A_A_IO_L1_in_9,
    /* fifo */ fifo_A_PE_8
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<9>(
    /* fifo */ fifo_A_A_IO_L1_in_9,
    /* fifo */ fifo_A_A_IO_L1_in_10,
    /* fifo */ fifo_A_PE_9
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<10>(
    /* fifo */ fifo_A_A_IO_L1_in_10,
    /* fifo */ fifo_A_A_IO_L1_in_11,
    /* fifo */ fifo_A_PE_10
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<11>(
    /* fifo */ fifo_A_A_IO_L1_in_11,
    /* fifo */ fifo_A_A_IO_L1_in_12,
    /* fifo */ fifo_A_PE_11
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<12>(
    /* fifo */ fifo_A_A_IO_L1_in_12,
    /* fifo */ fifo_A_A_IO_L1_in_13,
    /* fifo */ fifo_A_PE_12
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<13>(
    /* fifo */ fifo_A_A_IO_L1_in_13,
    /* fifo */ fifo_A_A_IO_L1_in_14,
    /* fifo */ fifo_A_PE_13
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<14>(
    /* fifo */ fifo_A_A_IO_L1_in_14,
    /* fifo */ fifo_A_A_IO_L1_in_15,
    /* fifo */ fifo_A_PE_14
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<15>(
    /* fifo */ fifo_A_A_IO_L1_in_15,
    /* fifo */ fifo_A_A_IO_L1_in_16,
    /* fifo */ fifo_A_PE_15
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<16>(
    /* fifo */ fifo_A_A_IO_L1_in_16,
    /* fifo */ fifo_A_A_IO_L1_in_17,
    /* fifo */ fifo_A_PE_16
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<17>(
    /* fifo */ fifo_A_A_IO_L1_in_17,
    /* fifo */ fifo_A_A_IO_L1_in_18,
    /* fifo */ fifo_A_PE_17
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<18>(
    /* fifo */ fifo_A_A_IO_L1_in_18,
    /* fifo */ fifo_A_A_IO_L1_in_19,
    /* fifo */ fifo_A_PE_18
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<19>(
    /* fifo */ fifo_A_A_IO_L1_in_19,
    /* fifo */ fifo_A_A_IO_L1_in_20,
    /* fifo */ fifo_A_PE_19
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<20>(
    /* fifo */ fifo_A_A_IO_L1_in_20,
    /* fifo */ fifo_A_A_IO_L1_in_21,
    /* fifo */ fifo_A_PE_20
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<21>(
    /* fifo */ fifo_A_A_IO_L1_in_21,
    /* fifo */ fifo_A_A_IO_L1_in_22,
    /* fifo */ fifo_A_PE_21
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<22>(
    /* fifo */ fifo_A_A_IO_L1_in_22,
    /* fifo */ fifo_A_A_IO_L1_in_23,
    /* fifo */ fifo_A_PE_22
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<23>(
    /* fifo */ fifo_A_A_IO_L1_in_23,
    /* fifo */ fifo_A_A_IO_L1_in_24,
    /* fifo */ fifo_A_PE_23
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<24>(
    /* fifo */ fifo_A_A_IO_L1_in_24,
    /* fifo */ fifo_A_A_IO_L1_in_25,
    /* fifo */ fifo_A_PE_24
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<25>(
    /* fifo */ fifo_A_A_IO_L1_in_25,
    /* fifo */ fifo_A_A_IO_L1_in_26,
    /* fifo */ fifo_A_PE_25
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<26>(
    /* fifo */ fifo_A_A_IO_L1_in_26,
    /* fifo */ fifo_A_A_IO_L1_in_27,
    /* fifo */ fifo_A_PE_26
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<27>(
    /* fifo */ fifo_A_A_IO_L1_in_27,
    /* fifo */ fifo_A_A_IO_L1_in_28,
    /* fifo */ fifo_A_PE_27
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<28>(
    /* fifo */ fifo_A_A_IO_L1_in_28,
    /* fifo */ fifo_A_A_IO_L1_in_29,
    /* fifo */ fifo_A_PE_28
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<29>(
    /* fifo */ fifo_A_A_IO_L1_in_29,
    /* fifo */ fifo_A_A_IO_L1_in_30,
    /* fifo */ fifo_A_PE_29
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_wrapper<30>(
    /* fifo */ fifo_A_A_IO_L1_in_30,
    /* fifo */ fifo_A_A_IO_L1_in_31,
    /* fifo */ fifo_A_PE_30
  );
  /* Module Call */

  /* Module Call */
  A_IO_L1_in_boundary_wrapper<31>(
    /* fifo */ fifo_A_A_IO_L1_in_31,
    /* fifo */ fifo_A_PE_31
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<0>(
    /* fifo */ fifo_U_tmp_1_PE_0,
    /* fifo */ fifo_U_tmp_1_PE_1,
    /* fifo */ fifo_A_PE_0,
    /* fifo */ fifo_L_drain_PE_0,
    /* fifo */ fifo_U_drain_PE_0
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<1>(
    /* fifo */ fifo_U_tmp_1_PE_1,
    /* fifo */ fifo_U_tmp_1_PE_2,
    /* fifo */ fifo_A_PE_1,
    /* fifo */ fifo_L_drain_PE_1,
    /* fifo */ fifo_U_drain_PE_1
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<2>(
    /* fifo */ fifo_U_tmp_1_PE_2,
    /* fifo */ fifo_U_tmp_1_PE_3,
    /* fifo */ fifo_A_PE_2,
    /* fifo */ fifo_L_drain_PE_2,
    /* fifo */ fifo_U_drain_PE_2
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<3>(
    /* fifo */ fifo_U_tmp_1_PE_3,
    /* fifo */ fifo_U_tmp_1_PE_4,
    /* fifo */ fifo_A_PE_3,
    /* fifo */ fifo_L_drain_PE_3,
    /* fifo */ fifo_U_drain_PE_3
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<4>(
    /* fifo */ fifo_U_tmp_1_PE_4,
    /* fifo */ fifo_U_tmp_1_PE_5,
    /* fifo */ fifo_A_PE_4,
    /* fifo */ fifo_L_drain_PE_4,
    /* fifo */ fifo_U_drain_PE_4
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<5>(
    /* fifo */ fifo_U_tmp_1_PE_5,
    /* fifo */ fifo_U_tmp_1_PE_6,
    /* fifo */ fifo_A_PE_5,
    /* fifo */ fifo_L_drain_PE_5,
    /* fifo */ fifo_U_drain_PE_5
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<6>(
    /* fifo */ fifo_U_tmp_1_PE_6,
    /* fifo */ fifo_U_tmp_1_PE_7,
    /* fifo */ fifo_A_PE_6,
    /* fifo */ fifo_L_drain_PE_6,
    /* fifo */ fifo_U_drain_PE_6
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<7>(
    /* fifo */ fifo_U_tmp_1_PE_7,
    /* fifo */ fifo_U_tmp_1_PE_8,
    /* fifo */ fifo_A_PE_7,
    /* fifo */ fifo_L_drain_PE_7,
    /* fifo */ fifo_U_drain_PE_7
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<8>(
    /* fifo */ fifo_U_tmp_1_PE_8,
    /* fifo */ fifo_U_tmp_1_PE_9,
    /* fifo */ fifo_A_PE_8,
    /* fifo */ fifo_L_drain_PE_8,
    /* fifo */ fifo_U_drain_PE_8
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<9>(
    /* fifo */ fifo_U_tmp_1_PE_9,
    /* fifo */ fifo_U_tmp_1_PE_10,
    /* fifo */ fifo_A_PE_9,
    /* fifo */ fifo_L_drain_PE_9,
    /* fifo */ fifo_U_drain_PE_9
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<10>(
    /* fifo */ fifo_U_tmp_1_PE_10,
    /* fifo */ fifo_U_tmp_1_PE_11,
    /* fifo */ fifo_A_PE_10,
    /* fifo */ fifo_L_drain_PE_10,
    /* fifo */ fifo_U_drain_PE_10
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<11>(
    /* fifo */ fifo_U_tmp_1_PE_11,
    /* fifo */ fifo_U_tmp_1_PE_12,
    /* fifo */ fifo_A_PE_11,
    /* fifo */ fifo_L_drain_PE_11,
    /* fifo */ fifo_U_drain_PE_11
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<12>(
    /* fifo */ fifo_U_tmp_1_PE_12,
    /* fifo */ fifo_U_tmp_1_PE_13,
    /* fifo */ fifo_A_PE_12,
    /* fifo */ fifo_L_drain_PE_12,
    /* fifo */ fifo_U_drain_PE_12
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<13>(
    /* fifo */ fifo_U_tmp_1_PE_13,
    /* fifo */ fifo_U_tmp_1_PE_14,
    /* fifo */ fifo_A_PE_13,
    /* fifo */ fifo_L_drain_PE_13,
    /* fifo */ fifo_U_drain_PE_13
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<14>(
    /* fifo */ fifo_U_tmp_1_PE_14,
    /* fifo */ fifo_U_tmp_1_PE_15,
    /* fifo */ fifo_A_PE_14,
    /* fifo */ fifo_L_drain_PE_14,
    /* fifo */ fifo_U_drain_PE_14
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<15>(
    /* fifo */ fifo_U_tmp_1_PE_15,
    /* fifo */ fifo_U_tmp_1_PE_16,
    /* fifo */ fifo_A_PE_15,
    /* fifo */ fifo_L_drain_PE_15,
    /* fifo */ fifo_U_drain_PE_15
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<16>(
    /* fifo */ fifo_U_tmp_1_PE_16,
    /* fifo */ fifo_U_tmp_1_PE_17,
    /* fifo */ fifo_A_PE_16,
    /* fifo */ fifo_L_drain_PE_16,
    /* fifo */ fifo_U_drain_PE_16
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<17>(
    /* fifo */ fifo_U_tmp_1_PE_17,
    /* fifo */ fifo_U_tmp_1_PE_18,
    /* fifo */ fifo_A_PE_17,
    /* fifo */ fifo_L_drain_PE_17,
    /* fifo */ fifo_U_drain_PE_17
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<18>(
    /* fifo */ fifo_U_tmp_1_PE_18,
    /* fifo */ fifo_U_tmp_1_PE_19,
    /* fifo */ fifo_A_PE_18,
    /* fifo */ fifo_L_drain_PE_18,
    /* fifo */ fifo_U_drain_PE_18
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<19>(
    /* fifo */ fifo_U_tmp_1_PE_19,
    /* fifo */ fifo_U_tmp_1_PE_20,
    /* fifo */ fifo_A_PE_19,
    /* fifo */ fifo_L_drain_PE_19,
    /* fifo */ fifo_U_drain_PE_19
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<20>(
    /* fifo */ fifo_U_tmp_1_PE_20,
    /* fifo */ fifo_U_tmp_1_PE_21,
    /* fifo */ fifo_A_PE_20,
    /* fifo */ fifo_L_drain_PE_20,
    /* fifo */ fifo_U_drain_PE_20
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<21>(
    /* fifo */ fifo_U_tmp_1_PE_21,
    /* fifo */ fifo_U_tmp_1_PE_22,
    /* fifo */ fifo_A_PE_21,
    /* fifo */ fifo_L_drain_PE_21,
    /* fifo */ fifo_U_drain_PE_21
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<22>(
    /* fifo */ fifo_U_tmp_1_PE_22,
    /* fifo */ fifo_U_tmp_1_PE_23,
    /* fifo */ fifo_A_PE_22,
    /* fifo */ fifo_L_drain_PE_22,
    /* fifo */ fifo_U_drain_PE_22
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<23>(
    /* fifo */ fifo_U_tmp_1_PE_23,
    /* fifo */ fifo_U_tmp_1_PE_24,
    /* fifo */ fifo_A_PE_23,
    /* fifo */ fifo_L_drain_PE_23,
    /* fifo */ fifo_U_drain_PE_23
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<24>(
    /* fifo */ fifo_U_tmp_1_PE_24,
    /* fifo */ fifo_U_tmp_1_PE_25,
    /* fifo */ fifo_A_PE_24,
    /* fifo */ fifo_L_drain_PE_24,
    /* fifo */ fifo_U_drain_PE_24
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<25>(
    /* fifo */ fifo_U_tmp_1_PE_25,
    /* fifo */ fifo_U_tmp_1_PE_26,
    /* fifo */ fifo_A_PE_25,
    /* fifo */ fifo_L_drain_PE_25,
    /* fifo */ fifo_U_drain_PE_25
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<26>(
    /* fifo */ fifo_U_tmp_1_PE_26,
    /* fifo */ fifo_U_tmp_1_PE_27,
    /* fifo */ fifo_A_PE_26,
    /* fifo */ fifo_L_drain_PE_26,
    /* fifo */ fifo_U_drain_PE_26
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<27>(
    /* fifo */ fifo_U_tmp_1_PE_27,
    /* fifo */ fifo_U_tmp_1_PE_28,
    /* fifo */ fifo_A_PE_27,
    /* fifo */ fifo_L_drain_PE_27,
    /* fifo */ fifo_U_drain_PE_27
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<28>(
    /* fifo */ fifo_U_tmp_1_PE_28,
    /* fifo */ fifo_U_tmp_1_PE_29,
    /* fifo */ fifo_A_PE_28,
    /* fifo */ fifo_L_drain_PE_28,
    /* fifo */ fifo_U_drain_PE_28
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<29>(
    /* fifo */ fifo_U_tmp_1_PE_29,
    /* fifo */ fifo_U_tmp_1_PE_30,
    /* fifo */ fifo_A_PE_29,
    /* fifo */ fifo_L_drain_PE_29,
    /* fifo */ fifo_U_drain_PE_29
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<30>(
    /* fifo */ fifo_U_tmp_1_PE_30,
    /* fifo */ fifo_U_tmp_1_PE_31,
    /* fifo */ fifo_A_PE_30,
    /* fifo */ fifo_L_drain_PE_30,
    /* fifo */ fifo_U_drain_PE_30
  );
  /* Module Call */

  /* Module Call */
  PE_wrapper<31>(
    /* fifo */ fifo_U_tmp_1_PE_31,
    /* fifo */ fifo_U_tmp_1_PE_32,
    /* fifo */ fifo_A_PE_31,
    /* fifo */ fifo_L_drain_PE_31,
    /* fifo */ fifo_U_drain_PE_31
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_boundary_wrapper<31>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_31,
    /* fifo */ fifo_L_drain_PE_31
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<30>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_31,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_30,
    /* fifo */ fifo_L_drain_PE_30
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<29>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_30,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_29,
    /* fifo */ fifo_L_drain_PE_29
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<28>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_29,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_28,
    /* fifo */ fifo_L_drain_PE_28
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<27>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_28,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_27,
    /* fifo */ fifo_L_drain_PE_27
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<26>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_27,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_26,
    /* fifo */ fifo_L_drain_PE_26
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<25>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_26,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_25,
    /* fifo */ fifo_L_drain_PE_25
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<24>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_25,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_24,
    /* fifo */ fifo_L_drain_PE_24
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<23>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_24,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_23,
    /* fifo */ fifo_L_drain_PE_23
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<22>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_23,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_22,
    /* fifo */ fifo_L_drain_PE_22
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<21>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_22,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_21,
    /* fifo */ fifo_L_drain_PE_21
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<20>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_21,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_20,
    /* fifo */ fifo_L_drain_PE_20
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<19>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_20,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_19,
    /* fifo */ fifo_L_drain_PE_19
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<18>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_19,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_18,
    /* fifo */ fifo_L_drain_PE_18
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<17>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_18,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_17,
    /* fifo */ fifo_L_drain_PE_17
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<16>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_17,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_16,
    /* fifo */ fifo_L_drain_PE_16
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<15>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_16,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_15,
    /* fifo */ fifo_L_drain_PE_15
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<14>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_15,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_14,
    /* fifo */ fifo_L_drain_PE_14
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<13>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_14,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_13,
    /* fifo */ fifo_L_drain_PE_13
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<12>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_13,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_12,
    /* fifo */ fifo_L_drain_PE_12
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<11>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_12,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_11,
    /* fifo */ fifo_L_drain_PE_11
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<10>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_11,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_10,
    /* fifo */ fifo_L_drain_PE_10
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<9>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_10,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_9,
    /* fifo */ fifo_L_drain_PE_9
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<8>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_9,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_8,
    /* fifo */ fifo_L_drain_PE_8
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<7>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_8,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_7,
    /* fifo */ fifo_L_drain_PE_7
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<6>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_7,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_6,
    /* fifo */ fifo_L_drain_PE_6
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<5>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_6,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_5,
    /* fifo */ fifo_L_drain_PE_5
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<4>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_5,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_4,
    /* fifo */ fifo_L_drain_PE_4
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<3>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_4,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_3,
    /* fifo */ fifo_L_drain_PE_3
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<2>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_3,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_2,
    /* fifo */ fifo_L_drain_PE_2
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<1>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_2,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_1,
    /* fifo */ fifo_L_drain_PE_1
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L1_out_wrapper<0>(
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_1,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_0,
    /* fifo */ fifo_L_drain_PE_0
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L2_out(
    /* fifo */ fifo_L_drain_L_drain_IO_L2_out_serialize,
    /* fifo */ fifo_L_drain_L_drain_IO_L1_out_0
  );
  /* Module Call */

  /* Module Call */
  L_drain_IO_L2_out_serialize(
    /* array */ L,
    /* fifo */ fifo_L_drain_L_drain_IO_L2_out_serialize
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_boundary_wrapper<31>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_31,
    /* fifo */ fifo_U_drain_PE_31
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<30>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_31,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_30,
    /* fifo */ fifo_U_drain_PE_30
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<29>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_30,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_29,
    /* fifo */ fifo_U_drain_PE_29
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<28>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_29,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_28,
    /* fifo */ fifo_U_drain_PE_28
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<27>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_28,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_27,
    /* fifo */ fifo_U_drain_PE_27
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<26>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_27,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_26,
    /* fifo */ fifo_U_drain_PE_26
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<25>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_26,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_25,
    /* fifo */ fifo_U_drain_PE_25
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<24>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_25,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_24,
    /* fifo */ fifo_U_drain_PE_24
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<23>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_24,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_23,
    /* fifo */ fifo_U_drain_PE_23
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<22>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_23,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_22,
    /* fifo */ fifo_U_drain_PE_22
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<21>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_22,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_21,
    /* fifo */ fifo_U_drain_PE_21
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<20>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_21,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_20,
    /* fifo */ fifo_U_drain_PE_20
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<19>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_20,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_19,
    /* fifo */ fifo_U_drain_PE_19
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<18>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_19,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_18,
    /* fifo */ fifo_U_drain_PE_18
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<17>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_18,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_17,
    /* fifo */ fifo_U_drain_PE_17
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<16>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_17,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_16,
    /* fifo */ fifo_U_drain_PE_16
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<15>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_16,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_15,
    /* fifo */ fifo_U_drain_PE_15
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<14>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_15,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_14,
    /* fifo */ fifo_U_drain_PE_14
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<13>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_14,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_13,
    /* fifo */ fifo_U_drain_PE_13
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<12>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_13,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_12,
    /* fifo */ fifo_U_drain_PE_12
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<11>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_12,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_11,
    /* fifo */ fifo_U_drain_PE_11
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<10>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_11,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_10,
    /* fifo */ fifo_U_drain_PE_10
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<9>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_10,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_9,
    /* fifo */ fifo_U_drain_PE_9
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<8>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_9,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_8,
    /* fifo */ fifo_U_drain_PE_8
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<7>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_8,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_7,
    /* fifo */ fifo_U_drain_PE_7
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<6>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_7,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_6,
    /* fifo */ fifo_U_drain_PE_6
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<5>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_6,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_5,
    /* fifo */ fifo_U_drain_PE_5
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<4>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_5,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_4,
    /* fifo */ fifo_U_drain_PE_4
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<3>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_4,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_3,
    /* fifo */ fifo_U_drain_PE_3
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<2>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_3,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_2,
    /* fifo */ fifo_U_drain_PE_2
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<1>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_2,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_1,
    /* fifo */ fifo_U_drain_PE_1
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L1_out_wrapper<0>(
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_1,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_0,
    /* fifo */ fifo_U_drain_PE_0
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L2_out(
    /* fifo */ fifo_U_drain_U_drain_IO_L2_out_serialize,
    /* fifo */ fifo_U_drain_U_drain_IO_L1_out_0
  );
  /* Module Call */

  /* Module Call */
  U_drain_IO_L2_out_serialize(
    /* array */ U,
    /* fifo */ fifo_U_drain_U_drain_IO_L2_out_serialize
  );
  /* Module Call */

}
}

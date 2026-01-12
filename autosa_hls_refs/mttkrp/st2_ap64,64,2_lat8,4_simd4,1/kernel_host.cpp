// AutoSA Command:
// /workspaces/mlir-systolic/third_party/AutoSA/autosa /workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/mttkrp/kernel.c --config=/workspaces/mlir-systolic/third_party/AutoSA/autosa_config/autosa_config.json --target=autosa_hls_c --output-dir=/workspaces/mlir-systolic/third_party/AutoSA/autosa.tmp/output --host-serialize --simd-info=/workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/mttkrp/simd_info.json --sa-sizes={kernel[]->space_time[2];kernel[]->array_part[64,64,2];kernel[]->latency[8,4];kernel[]->simd[4,1]}

#include "kernel_host.hpp"
#include <assert.h>
#include <stdio.h>
/*
 * This code implements the Matricized Tensor Times Khatri-Rao Product (MTTKRP), which performs:
 * D(i,j) += A(i,k,l) * B(k,j) * C(l,j)
 * Input: A[I][K][L], B[K][J], C[L][J]
 * Output: D[I][J]
 */

#include "kernel.h"

int main(int argc, char **argv){
  // declarations
  static data_t A[I][K][L];
  static data_t B[K][J];
//  static data_t C[L][J];
  static data_t C[J][L];
  static data_t D[I][J];
  static data_t D_golden[I][J];

  // data initialization
  for (int i = 0; i < I; i++)
    for (int k = 0; k < K; k++) 
      for (int l = 0; l < L; l++) {
        A[i][k][l] = 2.5;
      }
  for (int k = 0; k < K; k++)
    for (int j = 0; j < J; j++) {
      B[k][j] = 2.5;
    }
  for (int l = 0; l < L; l++)
    for (int j = 0; j < J; j++) {
//      C[l][j] = 2.5;
      C[j][l] = 2.5;
    }
  data_t tmp;

  // computation
  {
    if (argc != 2) {
      std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
      return EXIT_FAILURE;
    }

    cl_int err;
    std::vector<cl::Device> devices = get_devices();
    cl::Device device = devices[0];
    std::string device_name = device.getInfo<CL_DEVICE_NAME>();
    std::cout << "Found Device=" << device_name.c_str() << std::endl;
    // Creating Context and Command Queue for selected device
    cl::Context context(device);
    cl::CommandQueue q(context, device);
    // Import XCLBIN
    xclbin_file_name = argv[1];
    cl::Program::Binaries kernel_bins = import_binary_file();
    // Create Program and Kernel
    //devices.erase(devices.begin());
    devices.resize(1);
    cl::Program program(context, devices, kernel_bins);
    cl::Kernel krnl(program, "kernel0");

    // Allocate memory in host memory
    std::vector<float, aligned_allocator<float>> dev_A((256) * (256) * (256));
    std::vector<float, aligned_allocator<float>> dev_B((256) * (336));
    std::vector<float, aligned_allocator<float>> dev_C((336) * (256));
    std::vector<std::vector<float, aligned_allocator<float>>> dev_D;
    for (int i = 0; i < 2; i++) {
      std::vector<float, aligned_allocator<float>> dev_D_tmp((256) * (336));
      dev_D.push_back(dev_D_tmp);
    }

    // Initialize host buffers
    std::copy(reinterpret_cast<float *>(A), reinterpret_cast<float *>(A) + (256) * (256) * (256), dev_A.begin());
    std::copy(reinterpret_cast<float *>(B), reinterpret_cast<float *>(B) + (256) * (336), dev_B.begin());
    std::copy(reinterpret_cast<float *>(C), reinterpret_cast<float *>(C) + (336) * (256), dev_C.begin());

    // Allocate buffers in device memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // device-to-host communication
    std::vector<cl::Buffer> buffer_A;
    std::vector<cl::Buffer> buffer_B;
    std::vector<cl::Buffer> buffer_C;
    std::vector<cl::Buffer> buffer_D;
    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                cl::Buffer buffer_A_tmp(context,
                                        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                        (256) * (256) * (256) * sizeof(float),
                                        dev_A.data(),
                                        &err));
      buffer_A.push_back(std::move(buffer_A_tmp));
    }
    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                cl::Buffer buffer_B_tmp(context,
                                        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                        (256) * (336) * sizeof(float),
                                        dev_B.data(),
                                        &err));
      buffer_B.push_back(std::move(buffer_B_tmp));
    }
    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                cl::Buffer buffer_C_tmp(context,
                                        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                        (336) * (256) * sizeof(float),
                                        dev_C.data(),
                                        &err));
      buffer_C.push_back(std::move(buffer_C_tmp));
    }
    for (int i = 0; i < 2; i++) {
      OCL_CHECK(err,
                cl::Buffer buffer_D_tmp(context,
                                        CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                        (256) * (336) * sizeof(float),
                                        dev_D[i].data(),
                                        &err));
      buffer_D.push_back(std::move(buffer_D_tmp));
    }

    auto host_begin = std::chrono::high_resolution_clock::now();
    auto fpga_begin = std::chrono::high_resolution_clock::now();
    auto fpga_end = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                err = q.enqueueMigrateMemObjects({buffer_A[i]}, 0));
    }

    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                err = q.enqueueMigrateMemObjects({buffer_B[i]}, 0));
    }

    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                err = q.enqueueMigrateMemObjects({buffer_C[i]}, 0));
    }

    {
      OCL_CHECK(err, err = krnl.setArg(0, buffer_A[0]));
      OCL_CHECK(err, err = krnl.setArg(1, buffer_B[0]));
      OCL_CHECK(err, err = krnl.setArg(2, buffer_C[0]));
      OCL_CHECK(err, err = krnl.setArg(3, buffer_D[0]));
      OCL_CHECK(err, err = krnl.setArg(4, buffer_D[0]));
      OCL_CHECK(err, err = krnl.setArg(5, buffer_D[1]));
      q.finish();

      // Warm up
      OCL_CHECK(err, err = q.enqueueTask(krnl));
      q.finish();

      fpga_begin = std::chrono::high_resolution_clock::now();

      // Launch the kernel
      for (int i = 0; i < 10; i++)
        OCL_CHECK(err, err = q.enqueueTask(krnl));

      q.finish();
      fpga_end = std::chrono::high_resolution_clock::now();
    }

    for (int i = 0; i < 3; i++) {
      OCL_CHECK(err,
                err = q.enqueueMigrateMemObjects({buffer_D[i]}, CL_MIGRATE_MEM_OBJECT_HOST));
    }
    // Merge results
    for (int idx = 1; idx < 2; idx++) {
      D_drain_drain_merge(dev_D[0], dev_D[idx]);
    }

    q.finish();
    auto host_end = std::chrono::high_resolution_clock::now();

    // Calculate time
    std::chrono::duration<double> fpga_duration = fpga_end - fpga_begin;
    std::cout << "FPGA Time: " << fpga_duration.count() / 10 << " s" << std::endl;
    std::chrono::duration<double> host_duration = host_end - host_begin;
    std::cout << "Host Time: " << host_duration.count() << " s" << std::endl;

    // Restore data from host buffers
    std::copy(dev_D[0].begin(), dev_D[0].end(), reinterpret_cast<float *>(D));
  }

  for (int i = 0; i < I; i++)
    for (int j = 0; j < J; j++) {
      D_golden[i][j] = 0;
      for (int k = 0; k < K; k++) {
//        for (int l = 0; l < L; l++) {
//          D_golden[i][j] += A[i][k][l] * B[k][j] * C[l][j];
//        }
        data_t tmp = 0;
        for (int l = 0; l < L; l++) {
//          tmp += A[i][k][l] * C[l][j];
          tmp += A[i][k][l] * C[j][l];
        }
        D_golden[i][j] += B[k][j] * tmp;
      }
    }

  // comparison
  int err = 0;
  float thres = 0.01;
  for (int i = 0; i < I; i++) 
    for (int j = 0; j < J; j++) {
      if (fabs((float)D_golden[i][j] - (float)D[i][j]) > thres) {
        err++;
      }
    }

  if (err) {
    printf("Test failed with %d errors!\n", err);
    return -1;
  } else {
    printf("Test passed!\n");
    return 0;
  }
}

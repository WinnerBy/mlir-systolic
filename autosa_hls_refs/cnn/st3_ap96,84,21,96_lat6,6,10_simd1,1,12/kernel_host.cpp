// AutoSA Command:
// /workspaces/mlir-systolic/third_party/AutoSA/autosa /workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/cnn/kernel.c --config=/workspaces/mlir-systolic/third_party/AutoSA/autosa_config/autosa_config.json --target=autosa_hls_c --output-dir=/workspaces/mlir-systolic/third_party/AutoSA/autosa.tmp/output --host-serialize --simd-info=/workspaces/mlir-systolic/third_party/AutoSA/autosa_tests/large/cnn/simd_info.json --sa-sizes={kernel[]->space_time[3];kernel[]->array_part[96,84,21,96];kernel[]->latency[6,6,10];kernel[]->simd[1,1,12]}

#include "kernel_host.hpp"
#include <assert.h>
#include <stdio.h>
#include "kernel.h"

int main(int argc, char **argv){
  // declarations
//  data_t cin[I][R + K - 1][C + K - 1];
//  data_t w[O][I][K][K];
//  data_t cout[O][R][C];
//  data_t cout_golden[O][R][C];
  static data_t cin[R + K - 1][C + K - 1][I];
  static data_t w[O][K][K][I];
  static data_t cout[R][C][O];
  static data_t cout_golden[R][C][O];

  // data initialization
  for (int i = 0 ; i < I; i++)
    for (int r = 0; r < R + K - 1; r++)
      for (int c = 0; c < C + K - 1; c++) {
        cin[r][c][i] = 1;
      }

  for (int o = 0; o < O; o++)
    for (int i = 0; i < I; i++) 
      for (int p = 0; p < K; p++)
        for (int q = 0; q < K; q++) {
          w[o][p][q][i] = 1;
        }
 
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
    std::vector<float, aligned_allocator<float>> dev_cin((58) * (58) * (512));
    std::vector<std::vector<float, aligned_allocator<float>>> dev_cout;
    for (int i = 0; i < 2; i++) {
      std::vector<float, aligned_allocator<float>> dev_cout_tmp((56) * (56) * (640));
      dev_cout.push_back(dev_cout_tmp);
    }
    std::vector<float, aligned_allocator<float>> dev_w((640) * (3) * (3) * (512));

    // Initialize host buffers
    std::copy(reinterpret_cast<float *>(cin), reinterpret_cast<float *>(cin) + (58) * (58) * (512), dev_cin.begin());
    std::copy(reinterpret_cast<float *>(w), reinterpret_cast<float *>(w) + (640) * (3) * (3) * (512), dev_w.begin());

    // Allocate buffers in device memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // device-to-host communication
    std::vector<cl::Buffer> buffer_cin;
    std::vector<cl::Buffer> buffer_cout;
    std::vector<cl::Buffer> buffer_w;
    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                cl::Buffer buffer_cin_tmp(context,
                                          CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                          (58) * (58) * (512) * sizeof(float),
                                          dev_cin.data(),
                                          &err));
      buffer_cin.push_back(std::move(buffer_cin_tmp));
    }
    for (int i = 0; i < 2; i++) {
      OCL_CHECK(err,
                cl::Buffer buffer_cout_tmp(context,
                                           CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                           (56) * (56) * (640) * sizeof(float),
                                           dev_cout[i].data(),
                                           &err));
      buffer_cout.push_back(std::move(buffer_cout_tmp));
    }
    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                cl::Buffer buffer_w_tmp(context,
                                        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                        (640) * (3) * (3) * (512) * sizeof(float),
                                        dev_w.data(),
                                        &err));
      buffer_w.push_back(std::move(buffer_w_tmp));
    }

    auto host_begin = std::chrono::high_resolution_clock::now();
    auto fpga_begin = std::chrono::high_resolution_clock::now();
    auto fpga_end = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                err = q.enqueueMigrateMemObjects({buffer_cin[i]}, 0));
    }

    for (int i = 0; i < 1; i++) {
      OCL_CHECK(err,
                err = q.enqueueMigrateMemObjects({buffer_w[i]}, 0));
    }

    {
      OCL_CHECK(err, err = krnl.setArg(0, buffer_cin[0]));
      OCL_CHECK(err, err = krnl.setArg(1, buffer_cout[0]));
      OCL_CHECK(err, err = krnl.setArg(2, buffer_cout[0]));
      OCL_CHECK(err, err = krnl.setArg(3, buffer_cout[1]));
      OCL_CHECK(err, err = krnl.setArg(4, buffer_w[0]));
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
                err = q.enqueueMigrateMemObjects({buffer_cout[i]}, CL_MIGRATE_MEM_OBJECT_HOST));
    }
    // Merge results
    for (int idx = 1; idx < 2; idx++) {
      cout_drain_drain_merge(dev_cout[0], dev_cout[idx]);
    }

    q.finish();
    auto host_end = std::chrono::high_resolution_clock::now();

    // Calculate time
    std::chrono::duration<double> fpga_duration = fpga_end - fpga_begin;
    std::cout << "FPGA Time: " << fpga_duration.count() / 10 << " s" << std::endl;
    std::chrono::duration<double> host_duration = host_end - host_begin;
    std::cout << "Host Time: " << host_duration.count() << " s" << std::endl;

    // Restore data from host buffers
    std::copy(dev_cout[0].begin(), dev_cout[0].end(), reinterpret_cast<float *>(cout));
  }
 
  for (int o = 0; o < O; o++)
    for (int r = 0; r < R; r++)
      for (int c = 0; c < C; c++) {
        cout_golden[r][c][o] = 0;
        for (int i = 0; i < I; i++)
          for (int p = 0; p < 3; p++)
            for (int q = 0; q < 3; q++) {
              cout_golden[r][c][o] = cout_golden[r][c][o] + cin[r + p][c + q][i] * w[o][p][q][i];
            }
      }

  int err = 0;
  float thres = 0.001;
  for (int o = 0; o < O; o++)
    for (int r = 0; r < R; r++)
      for (int c = 0; c < C; c++) {
        if (fabs((float)cout_golden[r][c][o] - (float)cout[r][c][o]) > thres) {
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

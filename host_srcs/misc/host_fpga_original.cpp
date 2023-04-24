/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

/*
  Overlap Host Code

  There are many applications where all of the data cannot reside in an FPGA.
  For example, the data is too big to fit in an FPGA or the data is being
  streamed from a sensor or the network. In these situations data must be
  transferred to the host memory to the FPGA before the computation can be
  performed.

  Because PCIe is an full-duplex interconnect, you can transfer data to and from
  the FPGA simultaneously. Xilinx FPGAs can also perform computations during
  these data transfers. Performing all three of these operations at the same
  time allows you to keep the FPGA busy and take full advantage of all of the
  hardware on your system.

  In this example, we will demonstrate how to perform this using an out of order
  command queue.

  +---------+---------+---------+----------+---------+---------+---------
  | WriteA1 | WriteB1 | WriteA2 | Write B2 | WriteA1 | WriteB1 |   Wri...
  +---------+---------+---------+----------+---------+---------+---------
                      |       Compute1     |     Compute2      |  Compu...
                      +--------------------+-------------------+--------+
                                           | ReadC1 |          | ReadC2 |
                                           +--------+          +--------+

  Many OpenCL commands are asynchronous. This means that whenever you call an
  OpenCL function, the function will return before the operation has completed.
  Asynchronous nature of OpenCL allows you to simultaneously perform tasks on
  the host CPU as well as the FPGA.

  Memory transfer operations are asynchronous when the blocking_read,
  blocking_write parameters are set to CL_FALSE. These operations are behaving
  on host memory so it is important to make sure that the command has completed
  before that memory is used.

  You can make sure an operation has completed by querying events returned by
  these commands. Events are OpenCL objects that track the status of operations.
  Event objects are created by kernel execution commands, read, write, copy
  commands on memory objects or user events created using clCreateUserEvent.

  Events can be used to synchronize operations between the host thread and the
  device or between two operations in the same context. You can also use events
  to time a particular operation if the command queue was created using the
  CL_QUEUE_PROFILING_ENABLE flag.

  Most enqueuing commands return events by accepting a cl_event pointer as their
  last argument of the call. These events can be queried using the
  clGetEventInfo function to get the status of a particular operation.

  Many functions also accept event lists that can be used to enforce ordering in
  an OpenCL context. These events lists are especially important in the context
  of out of order command queues as they are the only way specify dependency.
  Normal in-order command queues do not need this because dependency is enforced
  in the order the operation was enqueued. See the concurrent execution example
  for additional details on how create an use these types of command queues.
 */
#include "host_fpga.h"
#ifdef FPGA_IMPL
#include "xcl2.hpp"
#endif 

#include <algorithm>
#include <cstdio>
#include <random>
#include <vector>

using namespace std;
using std::default_random_engine;
using std::generate;
using std::uniform_int_distribution;
using std::vector;

#define NUM_KERNEL 1

host_fpga::host_fpga(universalparams_t _universalparams){
	utilityobj = new utility(_universalparams);
	myuniversalparams = _universalparams;
}
host_fpga::~host_fpga(){} 

// Number of HBM PCs required
#define MAX_HBM_PC_COUNT 32
#define PC_NAME(n) n | XCL_MEM_TOPOLOGY
#ifdef FPGA_IMPL
const int pc[MAX_HBM_PC_COUNT] = {
    PC_NAME(0),  PC_NAME(1),  PC_NAME(2),  PC_NAME(3),  PC_NAME(4),  PC_NAME(5),  PC_NAME(6),  PC_NAME(7),
    PC_NAME(8),  PC_NAME(9),  PC_NAME(10), PC_NAME(11), PC_NAME(12), PC_NAME(13), PC_NAME(14), PC_NAME(15),
    PC_NAME(16), PC_NAME(17), PC_NAME(18), PC_NAME(19), PC_NAME(20), PC_NAME(21), PC_NAME(22), PC_NAME(23),
    PC_NAME(24), PC_NAME(25), PC_NAME(26), PC_NAME(27), PC_NAME(28), PC_NAME(29), PC_NAME(30), PC_NAME(31)};

// const int ARRAY_SIZE = 1 << 14;

int gen_random() {
    static default_random_engine e;
    static uniform_int_distribution<int> dist(0, 100);

    return dist(e);
}

// An event callback function that prints the operations performed by the OpenCL
// runtime.
void event_cb(cl_event event1, cl_int cmd_status, void* data) {
    cl_int err;
    cl_command_type command;
    cl::Event event(event1, true);
    OCL_CHECK(err, err = event.getInfo(CL_EVENT_COMMAND_TYPE, &command));
    cl_int status;
    OCL_CHECK(err, err = event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS, &status));
    const char* command_str;
    const char* status_str;
    switch (command) {
        case CL_COMMAND_READ_BUFFER:
            command_str = "buffer read";
            break;
        case CL_COMMAND_WRITE_BUFFER:
            command_str = "buffer write";
            break;
        case CL_COMMAND_NDRANGE_KERNEL:
            command_str = "kernel";
            break;
        case CL_COMMAND_MAP_BUFFER:
            command_str = "kernel";
            break;
        case CL_COMMAND_COPY_BUFFER:
            command_str = "kernel";
            break;
        case CL_COMMAND_MIGRATE_MEM_OBJECTS:
            command_str = "buffer migrate";
            break;
        default:
            command_str = "unknown";
    }
    switch (status) {
        case CL_QUEUED:
            status_str = "Queued";
            break;
        case CL_SUBMITTED:
            status_str = "Submitted";
            break;
        case CL_RUNNING:
            status_str = "Executing";
            break;
        case CL_COMPLETE:
            status_str = "Completed";
            break;
    }
    printf("[%s]: %s %s\n", reinterpret_cast<char*>(data), status_str, command_str);
    fflush(stdout);
}

// Sets the callback for a particular event
void set_callback(cl::Event event, const char* queue_name) {
    cl_int err;
    OCL_CHECK(err, err = event.setCallback(CL_COMPLETE, event_cb, (void*)queue_name));
}

long double host_fpga::runapp(std::string binaryFile__[2], HBM_channelAXISW_t * HBM_axichannel[2][NUM_PEs], HBM_channelAXISW_t * HBM_axicenter[2], universalparams_t universalparams){
	unsigned int ARRAY_SIZE = HBM_CHANNEL_SIZE * EDGE_PACK_SIZE;

    // auto binaryFile = argv[1];
	std::string binaryFile = binaryFile__[0]; 
    cl_int err;
    cl::CommandQueue q;
    cl::Context context;
    cl::Kernel krnl_vadd;

    // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    std::cout << "Creating Context..." << std::endl;
    auto devices = xcl::get_xil_devices();

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        // This example will use an out of order command queue. The default command
        // queue created by cl::CommandQueue is an inorder command queue.
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl_vadd = cl::Kernel(program, "top_function", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // We will break down our problem into multiple iterations. Each iteration
    // will perform computation on a subset of the entire data-set.
    size_t elements_per_iteration = 2048;
    size_t bytes_per_iteration = elements_per_iteration * sizeof(int);
    size_t num_iterations = 4; // ARRAY_SIZE / elements_per_iteration;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* // Allocate memory on the host and fill with random data.
	vector<int, aligned_allocator<int> > HH0(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH1(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH2(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH3(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH4(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH5(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH6(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH7(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH8(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH9(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH10(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH11(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH12(ARRAY_SIZE);
	vector<int, aligned_allocator<int> > HH13(ARRAY_SIZE);
	generate(begin(HH0), end(HH0), gen_random);
	generate(begin(HH1), end(HH1), gen_random);
	generate(begin(HH2), end(HH2), gen_random);
	generate(begin(HH3), end(HH3), gen_random);
	generate(begin(HH4), end(HH4), gen_random);
	generate(begin(HH5), end(HH5), gen_random);
	generate(begin(HH6), end(HH6), gen_random);
	generate(begin(HH7), end(HH7), gen_random);
	generate(begin(HH8), end(HH8), gen_random);
	generate(begin(HH9), end(HH9), gen_random);
	generate(begin(HH10), end(HH10), gen_random);
	generate(begin(HH11), end(HH11), gen_random);
	generate(begin(HH12), end(HH12), gen_random);
	generate(begin(HH13), end(HH13), gen_random);
    vector<int, aligned_allocator<int> > device_result(ARRAY_SIZE); */
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	unsigned int * PTR[2][NUM_PEs]; 
	for(unsigned int i=0; i<NUM_PEs; i++){ for(unsigned int n=0; n<2; n++){ PTR[n][i] = (unsigned int *)&HBM_axichannel[n][i][0]; }}
	std::vector<int, aligned_allocator<int> > HHX[NUM_PEs*2]; for(unsigned int i=0; i<NUM_PEs*2; i++){ HHX[i] = std::vector<int, aligned_allocator<int> >(ARRAY_SIZE); }
	for(unsigned int i=0; i<NUM_PEs; i++){ 
		for(unsigned int t=0; t<ARRAY_SIZE; t++){ 
			HHX[2*i].push_back(PTR[0][i][t]);
		}
		for(unsigned int t=0; t<ARRAY_SIZE; t++){ 
			HHX[2*i+1].push_back(PTR[1][i][t]);
		}
	}
	vector<int, aligned_allocator<int> > device_result(ARRAY_SIZE);

    // THIS PAIR OF EVENTS WILL BE USED TO TRACK WHEN A KERNEL IS FINISHED WITH
    // THE INPUT BUFFERS. ONCE THE KERNEL IS FINISHED PROCESSING THE DATA, A NEW
    // SET OF ELEMENTS WILL BE WRITTEN INTO THE BUFFER.
    vector<cl::Event> kernel_events(2);
    vector<cl::Event> read_events(2);
	cl::Buffer buffer_hbm0[2], buffer_hbm1[2], buffer_hbm2[2], buffer_hbm3[2], buffer_hbm4[2], buffer_hbm5[2], buffer_hbm6[2], buffer_hbm7[2], buffer_hbm8[2], buffer_hbm9[2], buffer_hbm10[2], buffer_hbm11[2], buffer_hbm12[2], buffer_hbm13[2];
	
	cl_mem_ext_ptr_t inBufExt0, inBufExt1, inBufExt2, inBufExt3, inBufExt4, inBufExt5, inBufExt6, inBufExt7, inBufExt8, inBufExt9, inBufExt10, inBufExt11, inBufExt12, inBufExt13;

    inBufExt0.obj = HHX[0].data();
    inBufExt0.param = 0;
    inBufExt0.flags = pc[0];
	
	inBufExt1.obj = HHX[1].data();
    inBufExt1.param = 0;
    inBufExt1.flags = pc[1];

    inBufExt2.obj = HHX[2].data();
    inBufExt2.param = 0;
    inBufExt2.flags = pc[2];
	
	inBufExt3.obj = HHX[3].data();
    inBufExt3.param = 0;
    inBufExt3.flags = pc[3];
	
	inBufExt4.obj = HHX[4].data();
    inBufExt4.param = 0;
    inBufExt4.flags = pc[4];
	
	inBufExt5.obj = HHX[5].data();
    inBufExt5.param = 0;
    inBufExt5.flags = pc[5];
	
	inBufExt6.obj = HHX[6].data();
    inBufExt6.param = 0;
    inBufExt6.flags = pc[6];
	
	inBufExt7.obj = HHX[7].data();
    inBufExt7.param = 0;
    inBufExt7.flags = pc[7];
	
	inBufExt8.obj = HHX[8].data();
    inBufExt8.param = 0;
    inBufExt8.flags = pc[8];
	
	inBufExt9.obj = HHX[9].data();
    inBufExt9.param = 0;
    inBufExt9.flags = pc[9];
	
	inBufExt10.obj = HHX[10].data();
    inBufExt10.param = 0;
    inBufExt10.flags = pc[10];
	
	inBufExt11.obj = HHX[11].data();
    inBufExt11.param = 0;
    inBufExt11.flags = pc[11];
	
	inBufExt12.obj = HHX[12].data();
    inBufExt12.param = 0;
    inBufExt12.flags = pc[12];
	
	inBufExt13.obj = HHX[13].data();
    inBufExt13.param = 0;
    inBufExt13.flags = pc[13];

	std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
    for (size_t iteration_idx = 0; iteration_idx < num_iterations; iteration_idx++) {
        std::cout <<"------------------------- iteration "<<iteration_idx<<" started... -------------------------"<<std::endl;
		int flag = iteration_idx % 2;

        if (iteration_idx >= 2) {
            OCL_CHECK(err, err = read_events[flag].wait());
        }

        // Allocate Buffer in Global Memory
        // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
        // Device-to-host communication
        std::cout << "Creating Buffers..." << std::endl;	
		OCL_CHECK(err, buffer_hbm0[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt0, &err));
		OCL_CHECK(err, buffer_hbm1[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt1, &err));			
		OCL_CHECK(err, buffer_hbm2[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt2, &err));	
		OCL_CHECK(err, buffer_hbm3[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt3, &err));	
		OCL_CHECK(err, buffer_hbm4[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt4, &err));	
		OCL_CHECK(err, buffer_hbm5[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt5, &err));	
		OCL_CHECK(err, buffer_hbm6[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt6, &err));	
		OCL_CHECK(err, buffer_hbm7[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt7, &err));	
		OCL_CHECK(err, buffer_hbm8[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt8, &err));	
		OCL_CHECK(err, buffer_hbm9[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt9, &err));	
		OCL_CHECK(err, buffer_hbm10[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt10, &err));	
		OCL_CHECK(err, buffer_hbm11[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt11, &err));	
		OCL_CHECK(err, buffer_hbm12[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt12, &err));	
		OCL_CHECK(err, buffer_hbm13[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt13, &err));	
											
        vector<cl::Event> write_event(1);

		OCL_CHECK(err, err = krnl_vadd.setArg(0, buffer_hbm0[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(1, buffer_hbm1[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(2, buffer_hbm2[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(3, buffer_hbm3[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(4, buffer_hbm4[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(5, buffer_hbm5[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(6, buffer_hbm6[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(7, buffer_hbm7[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(8, buffer_hbm8[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(9, buffer_hbm9[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(10, buffer_hbm10[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(11, buffer_hbm11[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(12, buffer_hbm12[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(13, buffer_hbm13[flag]));
        OCL_CHECK(err, err = krnl_vadd.setArg(14, int(elements_per_iteration)));

        // Copy input data to device global memory
        std::cout << "Copying data (Host to Device)..." << std::endl;
        // Because we are passing the write_event, it returns an event object
        // that identifies this particular command and can be used to query
        // or queue a wait for this particular command to complete.
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hbm0[flag], buffer_hbm1[flag], buffer_hbm2[flag], buffer_hbm3[flag], buffer_hbm4[flag], buffer_hbm5[flag], buffer_hbm6[flag], buffer_hbm7[flag], buffer_hbm8[flag], buffer_hbm9[flag], buffer_hbm10[flag], buffer_hbm11[flag], buffer_hbm12[flag], buffer_hbm13[flag]}, 0 /*0 means from host*/,
                                                        nullptr, &write_event[0]));
        set_callback(write_event[0], "ooo_queue");

        printf("Enqueueing NDRange kernel.\n");
        // This event needs to wait for the write buffer operations to complete
        // before executing. We are sending the write_events into its wait list to
        // ensure that the order of operations is correct.
        // Launch the Kernel
        std::vector<cl::Event> waitList;
        waitList.push_back(write_event[0]);
        OCL_CHECK(err, err = q.enqueueNDRangeKernel(krnl_vadd, 0, 1, 1, &waitList, &kernel_events[flag]));
        set_callback(kernel_events[flag], "ooo_queue");

        // Copy Result from Device Global Memory to Host Local Memory
        std::cout << "Getting Results (Device to Host)..." << std::endl;
        std::vector<cl::Event> eventList;
        eventList.push_back(kernel_events[flag]);
        // This operation only needs to wait for the kernel call. This call will
        // potentially overlap the next kernel call as well as the next read
        // operations
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hbm12[flag], buffer_hbm13[flag]}, CL_MIGRATE_MEM_OBJECT_HOST, &eventList,
                                                        &read_events[flag]));
        set_callback(read_events[flag], "ooo_queue");
    }
	double end_time = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time).count()) / 1000;	
	std::cout <<">>> kernel time elapsed for all iterations : "<<end_time<<" ms, "<<(end_time * 1000)<<" microsecs, "<<std::endl;
	
    // Wait for all of the OpenCL operations to complete
    printf("Waiting...\n");
    OCL_CHECK(err, err = q.flush());
    OCL_CHECK(err, err = q.finish());
    // OPENCL HOST CODE AREA ENDS
    bool match = true;
    // Verify the results
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int host_result = HHX[12][i] + HHX[13][i];
        if (device_result[i] != host_result) {
            printf("Error: Result mismatch:\n");
            printf("i = %d CPU result = %d Device result = %d\n", i, host_result, device_result[i]);
            match = false;
            break;
        }
    }

    printf("TEST %s\n", (match ? "PASSED" : "FAILED"));
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
#endif 


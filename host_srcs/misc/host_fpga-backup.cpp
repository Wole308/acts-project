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

long double host_fpga::runapp(action_t action, std::string binaryFile__[2], HBM_channelAXISW_t * HBM_axichannel[2][NUM_PEs], HBM_channelAXISW_t * HBM_axicenter[2], unsigned int globalparams[1024], universalparams_t universalparams){
	unsigned int ARRAY_SIZE = HBM_CHANNEL_SIZE * HBM_AXI_PACK_SIZE;

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
    // size_t elements_per_iteration = ARRAY_SIZE; // 2048;
    // size_t bytes_per_iteration = ARRAY_SIZE * sizeof(int); // elements_per_iteration * sizeof(int);
	size_t bytes_per_iteration = 64 * sizeof(int); // 1000000 * sizeof(int); ////////////////////////////////////// REMOVEME.
    size_t num_iterations = 1; 
	unsigned int batch = 0;
	
	// std::vector<int, aligned_allocator<int> > HHX[NUM_PEs*2]; for(unsigned int i=0; i<NUM_PEs*2; i++){ HHX[i] = std::vector<int, aligned_allocator<int> >(ARRAY_SIZE); }
	std::vector<int, aligned_allocator<int> > HHX[32]; for(unsigned int i=0; i<NUM_PEs*2; i++){ HHX[i] = std::vector<int, aligned_allocator<int> >(ARRAY_SIZE); }
	std::vector<int, aligned_allocator<int> > HHC[2]; for(unsigned int i=0; i<2; i++){ HHC[i] = std::vector<int, aligned_allocator<int> >(ARRAY_SIZE); }
	for(unsigned int i=0; i<NUM_PEs; i++){ 
		for(unsigned int t=0; t<HBM_CHANNEL_SIZE; t++){ 
			for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ 
				HHX[2*i][t*HBM_AXI_PACK_SIZE + v] = HBM_axichannel[0][i][t].data[v];
			}
			for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ 
				HHX[2*i+1][t*HBM_AXI_PACK_SIZE + v] = HBM_axichannel[1][i][t].data[v];
			}
		}
	}
	for(unsigned int t=0; t<HBM_CHANNEL_SIZE; t++){ 
		for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ 
			HHC[0][t*HBM_AXI_PACK_SIZE + v] = HBM_axicenter[0][t].data[v];
		}
		for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ 
			HHC[1][t*HBM_AXI_PACK_SIZE + v] = HBM_axicenter[1][t].data[v];
		}
	}
	vector<int, aligned_allocator<int> > device_result(ARRAY_SIZE);

    // THIS PAIR OF EVENTS WILL BE USED TO TRACK WHEN A KERNEL IS FINISHED WITH
    // THE INPUT BUFFERS. ONCE THE KERNEL IS FINISHED PROCESSING THE DATA, A NEW
    // SET OF ELEMENTS WILL BE WRITTEN INTO THE BUFFER.
    vector<cl::Event> kernel_events(2);
    vector<cl::Event> read_events(2);
	cl::Buffer buffer_hbm0[2], buffer_hbm1[2], buffer_hbm2[2], buffer_hbm3[2], buffer_hbm4[2], buffer_hbm5[2], buffer_hbm6[2], buffer_hbm7[2], buffer_hbm8[2], buffer_hbm9[2], buffer_hbm10[2], buffer_hbm11[2], buffer_hbm12[2], buffer_hbm13[2],
		buffer_hbm14[2], buffer_hbm15[2], buffer_hbm16[2], buffer_hbm17[2], buffer_hbm18[2], buffer_hbm19[2], buffer_hbm20[2], buffer_hbm21[2], buffer_hbm22[2], buffer_hbm23[2], buffer_hbm24[2], buffer_hbm25[2];
	
	cl_mem_ext_ptr_t inBufExt0, inBufExt1, inBufExt2, inBufExt3, inBufExt4, inBufExt5, inBufExt6, inBufExt7, inBufExt8, inBufExt9, inBufExt10, inBufExt11, inBufExt12, inBufExt13,
		inBufExt14, inBufExt15, inBufExt16, inBufExt17, inBufExt18, inBufExt19, inBufExt20, inBufExt21, inBufExt22, inBufExt23, inBufExt24, inBufExt25;

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
	
	inBufExt14.obj = HHX[14].data();
    inBufExt14.param = 0;
    inBufExt14.flags = pc[14];
	
	inBufExt15.obj = HHX[15].data();
    inBufExt15.param = 0;
    inBufExt15.flags = pc[15];
	
	inBufExt16.obj = HHX[16].data();
    inBufExt16.param = 0;
    inBufExt16.flags = pc[16];
	
	inBufExt17.obj = HHX[17].data();
    inBufExt17.param = 0;
    inBufExt17.flags = pc[17];
	
	inBufExt18.obj = HHX[18].data();
    inBufExt18.param = 0;
    inBufExt18.flags = pc[18];
	
	inBufExt19.obj = HHX[19].data();
    inBufExt19.param = 0;
    inBufExt19.flags = pc[19];
	
	inBufExt20.obj = HHX[20].data();
    inBufExt20.param = 0;
    inBufExt20.flags = pc[20];
	
	inBufExt21.obj = HHX[21].data();
    inBufExt21.param = 0;
    inBufExt21.flags = pc[21];
	
	inBufExt22.obj = HHX[22].data();
    inBufExt22.param = 0;
    inBufExt22.flags = pc[22];
	
	inBufExt23.obj = HHX[23].data();
    inBufExt23.param = 0;
    inBufExt23.flags = pc[23];

	inBufExt24.obj = HHC[0].data();
    inBufExt24.param = 0;
    inBufExt24.flags = pc[24];
	
	inBufExt25.obj = HHC[1].data();
    inBufExt25.param = 0;
    inBufExt25.flags = pc[25];
	
	#ifdef HOST_PRINT_RESULTS_XXXX
	cout<<"---------------------------------------------- host_fpga:: before ---------------------------------------------- "<<endl;
	unsigned int base_offsetttt__ = globalparams[GLOBALPARAMSCODE__BASEOFFSET__UPDATES] * HBM_AXI_PACK_SIZE;
	for(unsigned int i=0; i<1; i++){
		for(unsigned int t=0; t<4; t++){ 
			for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ 
				cout<<HHX[2*i+1][base_offsetttt__ + (t*HBM_AXI_PACK_SIZE + v)]<<", ";	
			}
			cout<<endl;
		}
	}
	#endif 

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
		OCL_CHECK(err, buffer_hbm0[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt0, &err));
		OCL_CHECK(err, buffer_hbm1[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt1, &err));	
		OCL_CHECK(err, buffer_hbm2[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt2, &err));	
		OCL_CHECK(err, buffer_hbm3[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt3, &err));	
		OCL_CHECK(err, buffer_hbm4[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt4, &err));	
		OCL_CHECK(err, buffer_hbm5[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt5, &err));	
		OCL_CHECK(err, buffer_hbm6[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt6, &err));	
		OCL_CHECK(err, buffer_hbm7[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt7, &err));	
		OCL_CHECK(err, buffer_hbm8[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt8, &err));	
		OCL_CHECK(err, buffer_hbm9[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt9, &err));	
		OCL_CHECK(err, buffer_hbm10[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt10, &err));	
		OCL_CHECK(err, buffer_hbm11[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt11, &err));	
		OCL_CHECK(err, buffer_hbm12[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt12, &err));	
		OCL_CHECK(err, buffer_hbm13[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt13, &err));
		buffer_hbm14[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt14, &err);
		// CL_INVALID_CONTEXT if context is not a valid context.
		// CL_INVALID_VALUE if values specified in flags are not valid as defined in the table above.
		// CL_INVALID_BUFFER_SIZE if size is 0.
		// Implementations may return CL_INVALID_BUFFER_SIZE if size is greater than the CL_DEVICE_MAX_MEM_ALLOC_SIZE value specified in the table of allowed values for param_name for clGetDeviceInfo for all devices in context.
		// CL_INVALID_HOST_PTR if host_ptr is NULL and CL_MEM_USE_HOST_PTR or CL_MEM_COPY_HOST_PTR are set in flags or if host_ptr is not NULL but CL_MEM_COPY_HOST_PTR or CL_MEM_USE_HOST_PTR are not set in flags.
		// CL_MEM_OBJECT_ALLOCATION_FAILURE if there is a failure to allocate memory for buffer object.
		// CL_OUT_OF_RESOURCES if there is a failure to allocate resources required by the OpenCL implementation on the device.
		// CL_OUT_OF_HOST_MEMORY if there is a failure to allocate resources required by the OpenCL implementation on the host.
		cout<<"err=="<<err<<" ------------"<<endl;
		if(err==CL_INVALID_CONTEXT){
			cout<<"err==CL_INVALID_CONTEXT ------------"<<endl;
		}
		if(err==CL_INVALID_VALUE){
			cout<<"err==CL_INVALID_VALUE ------------"<<endl;
		}
		if(err==CL_INVALID_BUFFER_SIZE){
			cout<<"err==CL_INVALID_BUFFER_SIZE ------------"<<endl;
		}
		if(err==CL_INVALID_HOST_PTR){
			cout<<"err==CL_INVALID_HOST_PTR ------------"<<endl;
		}
		if(err==CL_MEM_OBJECT_ALLOCATION_FAILURE){
			cout<<"err==CL_MEM_OBJECT_ALLOCATION_FAILURE ------------"<<endl;
		}
		if(err==CL_OUT_OF_RESOURCES){
			cout<<"err==CL_OUT_OF_RESOURCES ------------"<<endl;
		}
		if(err==CL_OUT_OF_HOST_MEMORY){
			cout<<"err==CL_OUT_OF_HOST_MEMORY ------------"<<endl;
		}
		if(err==CL_INVALID_CONTEXT){
			cout<<"err==CL_INVALID_CONTEXT ------------"<<endl;
		}
		OCL_CHECK(err, buffer_hbm14[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt14, &err));
		OCL_CHECK(err, buffer_hbm15[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt15, &err));
		OCL_CHECK(err, buffer_hbm16[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt16, &err));
		OCL_CHECK(err, buffer_hbm17[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt17, &err));
		OCL_CHECK(err, buffer_hbm18[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt18, &err));
		OCL_CHECK(err, buffer_hbm19[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt19, &err));
		OCL_CHECK(err, buffer_hbm20[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt20, &err));
		OCL_CHECK(err, buffer_hbm21[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt21, &err));
		OCL_CHECK(err, buffer_hbm22[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt22, &err));
		OCL_CHECK(err, buffer_hbm23[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt23, &err));
		OCL_CHECK(err, buffer_hbm24[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt24, &err));
		OCL_CHECK(err, buffer_hbm25[flag] = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            bytes_per_iteration, &inBufExt25, &err));
		exit(EXIT_SUCCESS); /////////////////////////////
											
        vector<cl::Event> write_event(1);

		OCL_CHECK(err, err = krnl_vadd.setArg(0, buffer_hbm0[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(1, buffer_hbm1[flag]));
		#if NUM_VALID_HBM_CHANNELS>1
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
		#if NUM_VALID_HBM_CHANNELS>6
		OCL_CHECK(err, err = krnl_vadd.setArg(12, buffer_hbm12[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(13, buffer_hbm13[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(14, buffer_hbm14[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(15, buffer_hbm15[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(16, buffer_hbm16[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(17, buffer_hbm17[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(18, buffer_hbm18[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(19, buffer_hbm19[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(20, buffer_hbm20[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(21, buffer_hbm21[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(22, buffer_hbm22[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(23, buffer_hbm23[flag]));
		#endif 
		#endif 
		
		#if NUM_VALID_HBM_CHANNELS==1
		OCL_CHECK(err, err = krnl_vadd.setArg(2, buffer_hbm24[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(3, buffer_hbm25[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(4, int(batch)));
		#endif
		#if NUM_VALID_HBM_CHANNELS==6
		OCL_CHECK(err, err = krnl_vadd.setArg(12, buffer_hbm24[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(13, buffer_hbm25[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(14, int(batch)));
		#endif		
		#if NUM_VALID_HBM_CHANNELS==12	
		OCL_CHECK(err, err = krnl_vadd.setArg(24, buffer_hbm24[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(25, buffer_hbm25[flag]));
		OCL_CHECK(err, err = krnl_vadd.setArg(26, int(action.module)));
		OCL_CHECK(err, err = krnl_vadd.setArg(27, int(action.start_pu)));
		OCL_CHECK(err, err = krnl_vadd.setArg(28, int(action.size_pu)));
		OCL_CHECK(err, err = krnl_vadd.setArg(29, int(action.start_pv)));
		OCL_CHECK(err, err = krnl_vadd.setArg(30, int(action.size_pv)));
		OCL_CHECK(err, err = krnl_vadd.setArg(31, int(action.start_llpset)));
		OCL_CHECK(err, err = krnl_vadd.setArg(32, int(action.size_llpset)));
		OCL_CHECK(err, err = krnl_vadd.setArg(33, int(action.start_llpid)));
		OCL_CHECK(err, err = krnl_vadd.setArg(34, int(action.size_llpid)));
		OCL_CHECK(err, err = krnl_vadd.setArg(35, int(action.start_gv)));
		OCL_CHECK(err, err = krnl_vadd.setArg(36, int(action.size_gv)));
		OCL_CHECK(err, err = krnl_vadd.setArg(37, int(action.finish)));
		#endif 

        // Copy input data to device global memory
        std::cout << "Copying data (Host to Device)..." << std::endl;
        // Because we are passing the write_event, it returns an event object
        // that identifies this particular command and can be used to query
        // or queue a wait for this particular command to complete.
		#if NUM_VALID_HBM_CHANNELS==1
		OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hbm0[flag], buffer_hbm1[flag], buffer_hbm24[flag], buffer_hbm25[flag]}, 0 /*0 means from host*/,
                                                        nullptr, &write_event[0]));
		#endif 
		#if NUM_VALID_HBM_CHANNELS==6
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hbm0[flag], buffer_hbm1[flag], buffer_hbm2[flag], buffer_hbm3[flag], buffer_hbm4[flag], buffer_hbm5[flag], buffer_hbm6[flag], buffer_hbm7[flag], buffer_hbm8[flag], buffer_hbm9[flag], buffer_hbm10[flag], buffer_hbm11[flag], buffer_hbm24[flag], buffer_hbm25[flag]}, 0 /*0 means from host*/,
                                                        nullptr, &write_event[0]));
		#endif 
		#if NUM_VALID_HBM_CHANNELS==12
		OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hbm0[flag], buffer_hbm1[flag], buffer_hbm2[flag], buffer_hbm3[flag], buffer_hbm4[flag], buffer_hbm5[flag], buffer_hbm6[flag], buffer_hbm7[flag], buffer_hbm8[flag], buffer_hbm9[flag], buffer_hbm10[flag], buffer_hbm11[flag], buffer_hbm12[flag], buffer_hbm13[flag], buffer_hbm14[flag], buffer_hbm15[flag], buffer_hbm16[flag], buffer_hbm17[flag], buffer_hbm18[flag], buffer_hbm19[flag], buffer_hbm20[flag], buffer_hbm21[flag], buffer_hbm22[flag], buffer_hbm23[flag], buffer_hbm24[flag], buffer_hbm25[flag]}, 0 /*0 means from host*/,				
                                                        nullptr, &write_event[0]));
		#endif 
        set_callback(write_event[0], "ooo_queue");
		OCL_CHECK(err, err = write_event[0].wait()); // REMOVEME

        printf("Enqueueing NDRange kernel.\n");
        // This event needs to wait for the write buffer operations to complete
        // before executing. We are sending the write_events into its wait list to
        // ensure that the order of operations is correct.
        // Launch the Kernel
		std::chrono::steady_clock::time_point begin_time1 = std::chrono::steady_clock::now();
		
        std::vector<cl::Event> waitList;
        waitList.push_back(write_event[0]);
        OCL_CHECK(err, err = q.enqueueNDRangeKernel(krnl_vadd, 0, 1, 1, &waitList, &kernel_events[flag]));
        set_callback(kernel_events[flag], "ooo_queue");
		OCL_CHECK(err, err = kernel_events[flag].wait());
		
		double end_time1 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time1).count()) / 1000;	
		std::cout <<">>> kernel time elapsed for current iteration : "<<end_time1<<" ms, "<<(end_time1 * 1000)<<" microsecs, "<<std::endl;

        // Copy Result from Device Global Memory to Host Local Memory
        std::cout << "Getting Results (Device to Host)..." << std::endl;
        std::vector<cl::Event> eventList;
        eventList.push_back(kernel_events[flag]);
        // This operation only needs to wait for the kernel call. This call will
        // potentially overlap the next kernel call as well as the next read
        // operations	
		#if NUM_VALID_HBM_CHANNELS==1
		OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hbm0[flag], buffer_hbm1[flag], buffer_hbm24[flag], buffer_hbm25[flag]}, CL_MIGRATE_MEM_OBJECT_HOST, &eventList,				
                                                        &read_events[flag]));
		#endif 
		#if NUM_VALID_HBM_CHANNELS==6
		OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hbm0[flag], buffer_hbm1[flag], buffer_hbm2[flag], buffer_hbm3[flag], buffer_hbm4[flag], buffer_hbm5[flag], buffer_hbm6[flag], buffer_hbm7[flag], buffer_hbm8[flag], buffer_hbm9[flag], buffer_hbm10[flag], buffer_hbm11[flag], buffer_hbm24[flag], buffer_hbm25[flag]}, CL_MIGRATE_MEM_OBJECT_HOST, &eventList,				
                                                        &read_events[flag]));
		#endif 
		#if NUM_VALID_HBM_CHANNELS==12
		OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hbm0[flag], buffer_hbm1[flag], buffer_hbm2[flag], buffer_hbm3[flag], buffer_hbm4[flag], buffer_hbm5[flag], buffer_hbm6[flag], buffer_hbm7[flag], buffer_hbm8[flag], buffer_hbm9[flag], buffer_hbm10[flag], buffer_hbm11[flag], buffer_hbm12[flag], buffer_hbm13[flag], buffer_hbm14[flag], buffer_hbm15[flag], buffer_hbm16[flag], buffer_hbm17[flag], buffer_hbm18[flag], buffer_hbm19[flag], buffer_hbm20[flag], buffer_hbm21[flag], buffer_hbm22[flag], buffer_hbm23[flag], buffer_hbm24[flag], buffer_hbm25[flag]}, CL_MIGRATE_MEM_OBJECT_HOST, &eventList,				
                                                        &read_events[flag]));
		#endif 
        set_callback(read_events[flag], "ooo_queue");
    }
	
    // Wait for all of the OpenCL operations to complete
    printf("Waiting...\n");
    OCL_CHECK(err, err = q.flush());
    OCL_CHECK(err, err = q.finish());
    // OPENCL HOST CODE AREA ENDS
	
	double end_time = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time).count()) / 1000;	
	std::cout <<">>> kernel time elapsed for all iterations : "<<end_time<<" ms, "<<(end_time * 1000)<<" microsecs, "<<std::endl;
	
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
	
	#ifdef HOST_PRINT_RESULTS_XXXX
	cout<<"---------------------------------------------- host_fpga:: after ---------------------------------------------- "<<endl;
	for(unsigned int i=0; i<1; i++){
		for(unsigned int t=0; t<4; t++){ 
			for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ 
				cout<<HHX[2*i+1][base_offsetttt__ + (t*HBM_AXI_PACK_SIZE + v)]<<", ";	
			}
			cout<<endl;
		}
	}
	#endif 

	printf("TEST %s\n", "PASSED");
    return EXIT_SUCCESS;
}
#endif 


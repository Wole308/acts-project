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
  device or between two operations in the same contexts. You can also use events
  to time a particular operation if the command queue was created using the
  CL_QUEUE_PROFILING_ENABLE flag.

  Most enqueuing commands return events by accepting a cl_event pointer as their
  last argument of the call. These events can be queried using the
  clGetEventInfo function to get the status of a particular operation.

  Many functions also accept event lists that can be used to enforce ordering in
  an OpenCL contexts. These events lists are especially important in the contexts
  of out of order command queues as they are the only way specify dependency.
  Normal in-order command queues do not need this because dependency is enforced
  in the order the operation was enqueued. See the concurrent execution example
  for additional details on how create an use these types of command queues.
 */
#include "host.h"
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
#define NUM_HBM_ARGS (NUM_VALID_HBM_CHANNELS * 2)
#define NUM_HBMC_ARGS 2
#define NUM_HBMIO_ARGS 2

#define RUN_SW_KERNEL
#define RUN_FPGA_KERNEL

#define ___PRE_RUN___
#define ___POST_RUN___ // FIXME. 

// #define ___SYNC___ //////////////////////////////////////////////////////////////////////////////////

#ifdef FPGA_IMPL
bool profiling0 = false; // false*, true;
#else 
bool profiling0 = false; // false, true*;	
#endif 
bool profiling1_timing = false; //false*, true
unsigned int num_prints2 = 1;//universalparams.NUM_FPGAS_; 

host::host(universalparams_t _universalparams){
	utilityobj = new utility(_universalparams);
	myuniversalparams = _universalparams;
}
host::~host(){} 

// Number of HBM PCs required
#define MAX_HBM_PC_COUNT 32
#define PC_NAME(n) n | XCL_MEM_TOPOLOGY
#ifdef FPGA_IMPL
const int pc[MAX_HBM_PC_COUNT] = {
    PC_NAME(0),  PC_NAME(1),  PC_NAME(2),  PC_NAME(3),  PC_NAME(4),  PC_NAME(5),  PC_NAME(6),  PC_NAME(7),
    PC_NAME(8),  PC_NAME(9),  PC_NAME(10), PC_NAME(11), PC_NAME(12), PC_NAME(13), PC_NAME(14), PC_NAME(15),
    PC_NAME(16), PC_NAME(17), PC_NAME(18), PC_NAME(19), PC_NAME(20), PC_NAME(21), PC_NAME(22), PC_NAME(23),
    PC_NAME(24), PC_NAME(25), PC_NAME(26), PC_NAME(27), PC_NAME(28), PC_NAME(29), PC_NAME(30), PC_NAME(31)};

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
    if(profiling0 == true){ printf("[%s]: %s %s\n", reinterpret_cast<char*>(data), status_str, command_str); }
    fflush(stdout);
}

// Sets the callback for a particular event
void set_callback(cl::Event event, const char* queue_name) {
    cl_int err;
    OCL_CHECK(err, err = event.setCallback(CL_COMPLETE, event_cb, (void*)queue_name));
}

void _set_args___actions(cl::Kernel * kernels, action_t action, unsigned int mask_i[MAX_IMPORT_BATCH_SIZE], universalparams_t universalparams, cl_int err){
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS, int(action.fpga)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 1, int(action.module)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 2, int(action.graph_iteration)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 3, int(action.start_pu)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 4, int(action.size_pu)));
	// OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 4, int(0))); /////////////
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 5, int(action.skip_pu))); // REMOVEME.	
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 6, int(action.start_pv_fpga)));	
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 7, int(action.start_pv)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 8, int(action.size_pv)));
	// OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 8, int(0))); // REMOVEME
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 9, int(action.start_llpset)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 10, int(action.size_llpset)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 11, int(action.start_llpid)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 12, int(action.size_llpid)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 13, int(action.start_gv_fpga)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 14, int(action.start_gv)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 15, int(action.size_gv)));
	// OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 15, int(0))); // REMOVEME.
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 16, int(action.id_process)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 17, int(action.id_import)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 18, int(action.id_export)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 19, int(action.size_import_export)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 20, int(action.status)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 21, int(universalparams.NUM_FPGAS_)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 22, int(mask_i[0])));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 23, int(mask_i[1])));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 24, int(mask_i[2])));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 25, int(mask_i[3])));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 26, int(mask_i[4])));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 27, int(mask_i[5])));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 28, int(mask_i[6])));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 29, int(mask_i[7])));
}
#endif 

unsigned int load_actions_fine(unsigned int fpga, action_t * actions[MAX_NUM_FPGAS], unsigned int _PE_BATCH_SIZE, unsigned int _AU_BATCH_SIZE, unsigned int _IMPORT_BATCH_SIZE, universalparams_t universalparams){
	unsigned int index = 0;
	unsigned int num_subpartition_per_partition = universalparams.GLOBAL_NUM_PEs_;
	
	unsigned int process_skip = _PE_BATCH_SIZE;
	unsigned int apply_skip = _AU_BATCH_SIZE;
	if(universalparams.NUM_FPGAS_==1){ process_skip = universalparams.NUM_UPARTITIONS; apply_skip = universalparams.NUM_APPLYPARTITIONS; }
	
	// process
	for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t+=process_skip){ 
		action_t action;
		
		action.module = PROCESS_EDGES_MODULE;
		action.graph_iteration = NAp;
		
		action.start_pu = NAp;
		action.size_pu = NAp; 
		action.skip_pu = NAp;
		
		action.start_pv_fpga = NAp;
		action.start_pv = NAp;
		action.size_pv = NAp; 
		
		action.start_gv_fpga = NAp;
		action.start_gv = NAp; 
		action.size_gv = NAp;
		
		action.start_llpset = NAp; 
		action.size_llpset = NAp; 
		action.start_llpid = NAp; 
		action.size_llpid = NAp; 
		
		action.id_process = INVALID_IOBUFFER_ID;
		action.id_import = INVALID_IOBUFFER_ID;
		action.id_export = INVALID_IOBUFFER_ID;
		action.size_import_export = _IMPORT_BATCH_SIZE; 
		action.status = 0;
		
		actions[fpga][index] = action;
		index += 1;
	}
	// return index; // REMOVEME.
	
	for(unsigned int apply_id=0; apply_id<universalparams.NUM_APPLYPARTITIONS; apply_id+=apply_skip){ 
		// apply
		action_t action;
		
		action.module = APPLY_UPDATES_MODULE;
		action.graph_iteration = NAp;
		
		action.start_pu = NAp; 
		action.size_pu = NAp; 
		action.skip_pu = NAp;
		
		action.start_pv_fpga = NAp;
		action.start_pv = apply_id; // local value 
		action.size_pv = _AU_BATCH_SIZE; 
		if(universalparams.NUM_FPGAS_==1){ action.size_pv = universalparams.NUM_APPLYPARTITIONS; }  // NEWCHANGE.
		if(action.start_pv + action.size_pv > universalparams.NUM_APPLYPARTITIONS){ action.size_pv = universalparams.NUM_APPLYPARTITIONS - action.start_pv; } // FIXME?
		
		action.start_gv_fpga = NAp;
		action.start_gv = NAp; 
		action.size_gv = NAp;
		
		action.start_llpset = NAp; 
		action.size_llpset = NAp; 
		action.start_llpid = NAp; 
		action.size_llpid = NAp;  
		
		action.id_process = INVALID_IOBUFFER_ID;
		action.id_import = INVALID_IOBUFFER_ID;
		action.id_export = INVALID_IOBUFFER_ID;
		action.size_import_export = _IMPORT_BATCH_SIZE; 
		action.status = 0;
		
		actions[fpga][index] = action;
		index += 1;
		
		// gather
		action.module = GATHER_FRONTIERS_MODULE;
		action.graph_iteration = NAp;
		
		action.start_pu = NAp; 
		action.size_pu = NAp; 
		action.skip_pu = NAp;
		
		action.start_pv_fpga = NAp;
		action.start_pv = NAp; 
		action.size_pv = NAp;
		
		action.start_gv_fpga = NAp;
		action.start_gv = apply_id * num_subpartition_per_partition;
		action.size_gv = _AU_BATCH_SIZE * universalparams.GLOBAL_NUM_PEs_; //  {{context['NUM_PEs']}}; // _GF_BATCH_SIZE;
		if(universalparams.NUM_FPGAS_==1){ action.size_gv = universalparams.NUM_UPARTITIONS; } // NEWCHANGE.
		if(action.start_gv + action.size_gv > universalparams.NUM_UPARTITIONS){ action.size_gv = universalparams.NUM_UPARTITIONS - action.start_gv; } // FIXME?	
		
		action.start_llpset = NAp; 
		action.size_llpset = NAp; 
		action.start_llpid = NAp; 
		action.size_llpid = NAp;  
		
		action.id_process = INVALID_IOBUFFER_ID;
		action.id_import = INVALID_IOBUFFER_ID;
		action.id_export = INVALID_IOBUFFER_ID;
		action.size_import_export = _IMPORT_BATCH_SIZE; 
	
		action.status = 0;
		
		actions[fpga][index] = action;
		index += 1;
	}
	return index;
}
unsigned int load_actions_coarse(unsigned int fpga, action_t * actions[MAX_NUM_FPGAS], unsigned int _PE_BATCH_SIZE, unsigned int _AU_BATCH_SIZE, unsigned int _IMPORT_BATCH_SIZE, universalparams_t universalparams){
	action_t action;
	
	action.module = ALL_MODULES;
	action.graph_iteration = NAp;
	
	action.start_pu = 0;
	action.size_pu = universalparams.NUM_UPARTITIONS;
	action.skip_pu = 1;
	
	action.start_pv_fpga = NAp;
	action.start_pv = 0;
	action.size_pv = universalparams.NUM_APPLYPARTITIONS;; // FIXME.
	
	action.start_gv_fpga = NAp;
	action.start_gv = 0; 
	action.size_gv = universalparams.NUM_UPARTITIONS; // FIXME.
	
	action.start_llpset = NAp; 
	action.size_llpset = NAp; 
	action.start_llpid = NAp; 
	action.size_llpid = NAp; 
	
	action.id_process = INVALID_IOBUFFER_ID;
	action.id_import = INVALID_IOBUFFER_ID;
	action.id_export = INVALID_IOBUFFER_ID;
	action.size_import_export = _IMPORT_BATCH_SIZE; 
	action.status = 0;
	
	actions[fpga][0] = action;
	return 1;
}

void initialize_Queue(bool all_vertices_active_in_all_iterations, gas_import_t * import_Queue[MAX_NUM_FPGAS], gas_process_t * process_Queue[MAX_NUM_FPGAS], gas_export_t * export_Queue[MAX_NUM_FPGAS], universalparams_t universalparams){
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
		for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ 
			import_Queue[fpga][t].ready_for_import = 0;
			// if(all_vertices_active_in_all_iterations == true){ process_Queue[fpga][t].ready_for_process = 1; } else { process_Queue[fpga][t].ready_for_process = 0; }
			process_Queue[fpga][t].ready_for_process = 1; // FIXME.
			export_Queue[fpga][t].ready_for_export = 0;
			
			import_Queue[fpga][t].tmp_state = 0;
			process_Queue[fpga][t].tmp_state = 0;
			export_Queue[fpga][t].tmp_state = 0;
			
			import_Queue[fpga][t].iteration = 0;
			process_Queue[fpga][t].iteration = 0;
			export_Queue[fpga][t].iteration = 0;
		}
	}
}

void run_traversal_algorithm_in_software(unsigned int GraphIter, vector<value_t> &actvvs, vector<value_t> &actvvs_nextit, vector<edge_t> &vertexptrbuffer, vector<edge3_type> &edgedatabuffer, 
		#ifdef FPGA_IMPL
		std::vector<int, aligned_allocator<int> > &vdatas,
		#else 
		vector<unsigned int> &vdatas,
		#endif
		unsigned int * vertex_properties_map, tuple_t * vpartition_stats, long double vertices_processed[128], long double edges_processed[128], universalparams_t universalparams){						
	for(unsigned int i=0; i<actvvs.size(); i++){
		unsigned int vid = actvvs[i];
		if(false){ cout<<"host: vid: "<<vid<<", edges_size "<<vertexptrbuffer[vid+1] - vertexptrbuffer[vid]<<""<<endl; }
		
		edge_t vptr_begin = vertexptrbuffer[vid];
		edge_t vptr_end = vertexptrbuffer[vid+1];
		edge_t edges_size = vptr_end - vptr_begin;
		if(vptr_end < vptr_begin){ continue; } // FIXME.
		#ifdef _DEBUGMODE_CHECKS3
		if(vid / universalparams._MAX_UPARTITION_SIZE >= MAX_NUM_UPARTITIONS){ cout<<"ERROR: vid("<<vid<<") / _MAX_UPARTITION_SIZE("<<universalparams._MAX_UPARTITION_SIZE<<") >= MAX_NUM_UPARTITIONS("<<MAX_NUM_UPARTITIONS<<"). exiting..."<<endl; exit(EXIT_FAILURE); }
		if(vptr_end < vptr_begin){ cout<<"ERROR: vptr_end("<<vptr_end<<") < vptr_begin("<<vptr_begin<<"). exiting..."<<endl; exit(EXIT_FAILURE); }
		#endif
		vertices_processed[GraphIter] += 1; 
		vpartition_stats[vid / universalparams._MAX_UPARTITION_SIZE].A += 1; 
		vpartition_stats[vid / universalparams._MAX_UPARTITION_SIZE].B += edges_size; // 
	
		for(unsigned int k=0; k<edges_size; k++){
			unsigned int dstvid = edgedatabuffer[vptr_begin + k].dstvid;
			unsigned int res = vdatas[vid] + 1;
			value_t vprop = vdatas[dstvid];
		
			value_t vtemp = min(vprop, res);
			vdatas[dstvid] = vtemp;
			if(vtemp != vprop){ 
				actvvs_nextit.push_back(dstvid);
				#ifdef _DEBUGMODE_CHECKS3
				if(dstvid / universalparams._MAX_UPARTITION_SIZE >= universalparams.NUM_UPARTITIONS){ cout<<"ERROR 232. dstvid ("<<dstvid<<") / _MAX_UPARTITION_SIZE ("<<universalparams._MAX_UPARTITION_SIZE<<") >= universalparams.NUM_UPARTITIONS ("<<universalparams.NUM_UPARTITIONS<<"). vid: "<<vid<<". EXITING..."<<endl; exit(EXIT_FAILURE); }	
				#endif 
				vertex_properties_map[dstvid / universalparams._MAX_UPARTITION_SIZE] += 1;
			}
			edges_processed[GraphIter] += 1; 
		}
	}
	// exit(EXIT_SUCCESS);
	return; 
}

long double host::runapp(string graph_path, std::string binaryFile__[2], 
		vector<edge3_type> &edgedatabuffer, vector<edge_t> &vertexptrbuffer, 
			HBM_channelAXISW_t * HBM_axichannel[2][MAX_GLOBAL_NUM_PEs], HBM_channelAXISW_t * HBM_axicenter[2][MAX_NUM_FPGAS], unsigned int hbm_channel_wwsize, unsigned int globalparams[1024], universalparams_t universalparams){
	unsigned int ARRAY_SIZE = hbm_channel_wwsize * HBM_AXI_PACK_SIZE; 
	unsigned int ARRAY_CENTER_SIZE = HBM_CENTER_SIZE * HBM_AXI_PACK_SIZE; 
	
	unsigned int _K0 = 1; // <lowerlimit:1, upperlimit:_GF_BATCH_SIZE>
	unsigned int _K1 = 2; // NUM_FPGAS // <lowerlimit:1, upperlimit:NUM_FPGAS*>
	unsigned int _AU_BATCH_SIZE = 2; 
	unsigned int _GF_BATCH_SIZE = _AU_BATCH_SIZE * universalparams.GLOBAL_NUM_PEs_; // 6 (i.e., 24 upartitions)
	unsigned int _IMPORT_BATCH_SIZE = (_GF_BATCH_SIZE / _K0); // 6
	unsigned int _PE_BATCH_SIZE = _IMPORT_BATCH_SIZE; // 6
	// unsigned int _EXPORT_BATCH_SIZE = (_GF_BATCH_SIZE * _K1); // 24
	unsigned int _EXPORT_BATCH_SIZE = _IMPORT_BATCH_SIZE; // 24 ///////////////////////////////////////////////////// NEWCHANGE
	unsigned int _IMPORT_EXPORT_GRANULARITY_VECSIZE = 8184;
	
	cout<<"host::run::  NUM_FPGAS: "<<universalparams.NUM_FPGAS_<<endl;
	cout<<"host::run::  NUM_PEs: "<<NUM_PEs<<endl;
	cout<<"host::run::  universalparams.GLOBAL_NUM_PEs_: "<<universalparams.GLOBAL_NUM_PEs_<<endl;
	cout<<"host::run::  RUN_IN_ASYNC_MODE: "<<RUN_IN_ASYNC_MODE<<endl;
	cout<<"host::run::  _PE_BATCH_SIZE: "<<_PE_BATCH_SIZE<<endl;
	cout<<"host::run::  GF_BATCH_SIZE: "<<_GF_BATCH_SIZE<<endl;
	cout<<"host::run::  AU_BATCH_SIZE: "<<_AU_BATCH_SIZE<<endl;
	cout<<"host::run::  IMPORT_BATCH_SIZE: "<<_IMPORT_BATCH_SIZE<<endl;
	cout<<"host::run::  EXPORT_BATCH_SIZE: "<<_EXPORT_BATCH_SIZE<<endl;
	
	unsigned int report_statistics[64]; for(unsigned int t=0; t<64; t++){ report_statistics[t] = 0; }
	unsigned int mask_i[MAX_NUM_FPGAS][MAX_IMPORT_BATCH_SIZE]; for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ for(unsigned int t=0; t<MAX_IMPORT_BATCH_SIZE; t++){ mask_i[fpga][t] = 0; }}
	
	bool all_vertices_active_in_all_iterations = false; // REMOVEME.
	if(universalparams.ALGORITHM == PAGERANK || universalparams.ALGORITHM == CF || universalparams.ALGORITHM == HITS || universalparams.ALGORITHM == SPMV){ all_vertices_active_in_all_iterations = true; }
	
	// sw sssp
	long double vertices_processed[128]; long double edges_processed[128]; for(unsigned int i=0; i<128; i++){ vertices_processed[i] = 0; edges_processed[i] = 0; }
	vector<value_t> actvvs; vector<value_t> actvvs_nextit; actvvs.push_back(1);
	tuple_t * vpartition_stats = new tuple_t[MAX_NUM_UPARTITIONS];
	for(unsigned int t=0; t<MAX_NUM_UPARTITIONS; t++){ vpartition_stats[t].A = 0; vpartition_stats[t].B = 0; }
	float ___hybrid___engine___vertex___threshold___ = (0.1 * universalparams.NUM_VERTICES) / 100; // 0.7 //////////////////////////// NEWCHANGE.
	unsigned int * processed_vertex_partitions_record[MAXNUMGRAPHITERATIONS]; for(unsigned int t=0; t<MAXNUMGRAPHITERATIONS; t++){ processed_vertex_partitions_record[t] = new unsigned int[MAX_NUM_UPARTITIONS]; }
	for(unsigned int iter=0; iter<MAXNUMGRAPHITERATIONS; iter++){ for(unsigned int t=0; t<MAX_NUM_UPARTITIONS; t++){ processed_vertex_partitions_record[iter][t] = 0; }}
	
	cout<<"host::runapp_sync: universalparams.NUM_FPGAS_: "<<universalparams.NUM_FPGAS_<<" ---"<<endl;
	cout<<"host::runapp_sync: NUM_HBM_ARGS: "<<NUM_HBM_ARGS<<" ---"<<endl;
	cout<<"host::runapp_sync: ARRAY_SIZE: "<<ARRAY_SIZE<<" ---"<<endl;
	cout<<"host::runapp_sync: HBM_CHANNEL_SIZE: "<<HBM_CHANNEL_SIZE<<" ---"<<endl;
	cout<<"host::runapp_sync: _IMPORT_EXPORT_GRANULARITY_VECSIZE: "<<_IMPORT_EXPORT_GRANULARITY_VECSIZE<<" ---"<<endl;
	cout<<"host::runapp_sync: hbm_channel_wwsize * HBM_AXI_PACK_SIZE: "<<hbm_channel_wwsize * HBM_AXI_PACK_SIZE<<" ---"<<endl;
	
	// load necessary commands
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
		for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){
			HBM_axichannel[0][i][GLOBALPARAMSCODE___ENABLE___PREPAREEDGEUPDATES].data[0] = 0; //
			HBM_axichannel[0][i][GLOBALPARAMSCODE___ENABLE___PROCESSEDGEUPDATES].data[0] = 0; //
			HBM_axichannel[0][i][GLOBALPARAMSCODE___ENABLE___PROCESSEDGES].data[0] = 1; //	
		}
	}
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ HBM_axichannel[0][i][GLOBALPARAMSCODE__COMMANDS__COMMAND0].data[0] = 2; }}
	
	// prepare OCL variables 
	#ifdef FPGA_IMPL
    // auto binaryFile = argv[1];
	std::string binaryFile = binaryFile__[0]; 
    cl_int err;
	
	vector<cl::Context> contexts(16); 
    vector<cl::Program> programs(16); 
    vector<cl::Kernel> kernels(16); 
    vector<cl::CommandQueue> q(16); 
    vector<std::string> device_name(16); 
	#endif 
	
    // load binary to FPGA 
	unsigned int device_count = universalparams.NUM_FPGAS_;
	#ifdef FPGA_IMPL
    std::cout << "Creating Context..." << std::endl;
    auto devices = xcl::get_xil_devices();
    bool valid_device = false;
	device_count = devices.size(); // FIXME.
	unsigned int num_fpga_devices = 0; 

	for(unsigned int fpga=0; fpga<devices.size(); fpga++){  
		cout<<"host:: FPGA "<<fpga<<" device name: "<<devices[fpga].getInfo<CL_DEVICE_NAME>()<<endl;
		if(devices[fpga].getInfo<CL_DEVICE_NAME>() == "xilinx_u280_gen3x16_xdma_base_1" || devices[fpga].getInfo<CL_DEVICE_NAME>() == "xilinx_u55c_gen3x16_xdma_base_3"){ num_fpga_devices += 1; } // xilinx_u280_xdma_201920_3
	} 

	device_count = num_fpga_devices;
	cl_context_properties props[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platform[0])(), 0}
	cout<<"------------------------------------------- host: "<<device_count<<" devices found. -------------------------------------------"<<endl;
	if(device_count==0){ cout<<"host: ERROR 234. no FPGA devices found. EXITING..."<<endl; exit(EXIT_FAILURE); }
	for(unsigned int fpga=0; fpga<device_count; fpga++){ // device_count
        auto device = devices[fpga];
        // Creating Context and Command Queue for selected Device
        // OCL_CHECK(err, contexts[fpga] = cl::Context(device, nullptr, nullptr, nullptr, &err));
		OCL_CHECK(err, contexts[fpga] = cl::Context(device, props, nullptr, nullptr, &err));
		
		#ifdef ___SYNC___
		OCL_CHECK(err, q[fpga] = cl::CommandQueue(contexts[fpga], device, CL_QUEUE_PROFILING_ENABLE, &err)); ////////// FIXME.
		#else 
		OCL_CHECK(err, q[fpga] = cl::CommandQueue(contexts[fpga], device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));	
		#endif 
		
		auto fileBuf = xcl::read_binary_file(binaryFile__[0]);
		cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};

        std::cout << "Trying to program device[" << fpga << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(contexts[fpga], {device}, bins, nullptr, &err);
		programs[fpga] = program;
		
		// programs[fpga] = load_cl2_binary(bins[fpga], devices[fpga], contexts[fpga]); // NEWCHANGE
        if (err != CL_SUCCESS){
            std::cout << "Failed to program device[" << fpga << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << fpga << "]: program successful!\n";
            OCL_CHECK(err, kernels[fpga] = cl::Kernel(programs[fpga], "top_function", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }
	#endif 
	// exit(EXIT_SUCCESS);/////////////////////////////////
	
	device_count = 4; //////////////////////////////////////////////////////////////////////////////////////////////// REMOVEME.

    // variables 
    size_t bytes_per_iteration = ARRAY_SIZE * sizeof(int);
	size_t bytesc_per_iteration = ARRAY_CENTER_SIZE * sizeof(int);
	cout<<"--- host::runapp_sync: bytes_per_iteration: "<<bytes_per_iteration<<", bytesc_per_iteration: "<<bytesc_per_iteration<<" ---"<<endl;

	// allocate sw buffers
	#ifdef FPGA_IMPL
	cout<<"host:: allocating channel buffers..."<<endl;
	std::vector<int, aligned_allocator<int> > HBM_axichannel_vector[MAX_NUM_FPGAS][32]; for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ for(unsigned int i=0; i<NUM_HBM_ARGS; i++){ HBM_axichannel_vector[fpga][i] = std::vector<int, aligned_allocator<int> >(ARRAY_SIZE); }}
	std::vector<int, aligned_allocator<int> > HBM_axicenter_vector[MAX_NUM_FPGAS][2]; for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ for(unsigned int i=0; i<2; i++){ HBM_axicenter_vector[fpga][i] = std::vector<int, aligned_allocator<int> >(ARRAY_CENTER_SIZE); }}
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
		for(unsigned int i=0; i<NUM_VALID_PEs; i++){ // NUM_VALID_PEs // FIXME.
			for(unsigned int t=0; t<hbm_channel_wwsize; t++){ 
				for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ 
					utilityobj->checkoutofbounds("host::ERROR 7121a::", t*HBM_AXI_PACK_SIZE + v, ARRAY_SIZE, ARRAY_SIZE, NAp, NAp);
					utilityobj->checkoutofbounds("host::ERROR 7121b::", t, HBM_CHANNEL_SIZE, ARRAY_SIZE, NAp, NAp);		
				}				
				for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ HBM_axichannel_vector[fpga][2*i][t*HBM_AXI_PACK_SIZE + v] = HBM_axichannel[0][(fpga * NUM_PEs) + i][t].data[v]; } // FIXME.
				for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ HBM_axichannel_vector[fpga][2*i+1][t*HBM_AXI_PACK_SIZE + v] = HBM_axichannel[1][(fpga * NUM_PEs) + i][t].data[v]; }
			}
		}
	}
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
		for(unsigned int t=0; t<hbm_channel_wwsize; t++){ 
			for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ 
				utilityobj->checkoutofbounds("host::ERROR 7121c::", t*HBM_AXI_PACK_SIZE + v, ARRAY_SIZE, ARRAY_SIZE, NAp, NAp);
				utilityobj->checkoutofbounds("host::ERROR 7121d::", t, HBM_CHANNEL_SIZE, ARRAY_SIZE, NAp, NAp);	
			}
			for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ HBM_axicenter_vector[fpga][0][t*HBM_AXI_PACK_SIZE + v] = HBM_axicenter[0][fpga][t].data[v]; }
			for(unsigned int v=0; v<HBM_AXI_PACK_SIZE; v++){ HBM_axicenter_vector[fpga][1][t*HBM_AXI_PACK_SIZE + v] = HBM_axicenter[1][fpga][t].data[v]; }
		}
	}
	#endif 
	
	cout<<"host:: allocating vertex property buffers..."<<endl;
	unsigned int * vertex_properties_map[MAXNUMGRAPHITERATIONS]; for(unsigned int t=0; t<MAXNUMGRAPHITERATIONS; t++){ vertex_properties_map[t] = new unsigned int[MAX_NUM_UPARTITIONS]; }
	#ifdef FPGA_IMPL
	std::vector<int, aligned_allocator<int> > vertex_properties((MAX_NUM_UPARTITIONS * MAX_UPARTITION_SIZE));  
	std::vector<int, aligned_allocator<int> > vdatas((MAX_NUM_UPARTITIONS * MAX_UPARTITION_SIZE));
	// std::vector<int, aligned_allocator<int> > frontier_properties[MAX_NUM_UPARTITIONS][MAX_NUM_FPGAS]; 
	// for(unsigned int p_u=0; p_u<MAX_NUM_UPARTITIONS; p_u++){ 
		// for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			// frontier_properties[p_u][fpga] = std::vector<int, aligned_allocator<int> >(MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE); 
		// }
	// }
	#else 
	vector<unsigned int> vertex_properties((MAX_NUM_UPARTITIONS * MAX_UPARTITION_SIZE));
	vector<unsigned int> vdatas((MAX_NUM_UPARTITIONS * MAX_UPARTITION_SIZE));
	// std::vector<unsigned int> frontier_properties[MAX_NUM_UPARTITIONS][MAX_NUM_FPGAS]; 
	// for(unsigned int p_u=0; p_u<MAX_NUM_UPARTITIONS; p_u++){ 
		// for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			// frontier_properties[p_u][fpga] = std::vector<unsigned int>(MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE); 
		// }
	// }
	#endif

	for(unsigned int i=0; i<universalparams.NUM_VERTICES; i++){ vdatas[i] = 0xFFFFFFFF; } for(unsigned int i=0; i<actvvs.size(); i++){ vdatas[actvvs[i]] = 0; }	
	for(unsigned int iter=0; iter<MAXNUMGRAPHITERATIONS; iter++){ for(unsigned int t=0; t<MAX_NUM_UPARTITIONS; t++){ vertex_properties_map[iter][t] = 0; }}
	for(unsigned int t=0; t<MAX_NUM_UPARTITIONS * MAX_UPARTITION_SIZE; t++){ vertex_properties[t] = 0xFFFFFFFF; }
	
    // declare buffers and ext_ptrs
	#ifdef FPGA_IMPL
	cl::Event kernel_events[MAX_NUM_FPGAS][2];
	cl::Event read_events[MAX_NUM_FPGAS][2];
	std::vector<cl::Buffer> buffer_hbm(1024);
	std::vector<cl::Buffer> buffer_hbmc(26);
	cl::Buffer buffer_import[MAX_NUM_FPGAS][2];
	cl::Buffer buffer_export[MAX_NUM_FPGAS][2];
	std::vector<cl_mem_ext_ptr_t> inBufExt(32);
	std::vector<cl_mem_ext_ptr_t> inBufExt_c(32);
	std::vector<cl_mem_ext_ptr_t> inBufExt_input(32);
	std::vector<cl_mem_ext_ptr_t> inBufExt_output(32);
	#endif 
	
	// attach ext pointers 
	#ifdef FPGA_IMPL
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		for (int i = 0; i < NUM_HBM_ARGS; i++) {
			inBufExt[fpga*NUM_HBM_ARGS + i].obj = HBM_axichannel_vector[fpga][i].data();
			inBufExt[fpga*NUM_HBM_ARGS + i].param = 0;
			inBufExt[fpga*NUM_HBM_ARGS + i].flags = pc[i];
		}
	}
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		for (int i = 0; i < NUM_HBMC_ARGS; i++) {
			inBufExt_c[fpga*NUM_HBM_ARGS + i].obj = HBM_axicenter_vector[fpga][i].data();
			inBufExt_c[fpga*NUM_HBM_ARGS + i].param = 0;
			inBufExt_c[fpga*NUM_HBM_ARGS + i].flags = pc[NUM_HBM_ARGS + i];
		}
	}
	for(unsigned int fpga=0; fpga<device_count; fpga++){ // FIXME. // NEWCHANGE.
		// inBufExt_input[fpga].obj = frontier_properties[0][0].data(); 
		inBufExt_input[fpga].param = 0;
		inBufExt_input[fpga].flags = pc[NUM_HBM_ARGS + NUM_HBMC_ARGS + 0];
	}
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		// inBufExt_output[fpga].obj = frontier_properties[0][0].data(); 
		inBufExt_output[fpga].param = 0;
		inBufExt_output[fpga].flags = pc[NUM_HBM_ARGS + NUM_HBMC_ARGS + 1];
	}
	#endif 
	
	std::cout << "Creating Buffers............................universalparams.NUM_FPGAS_: "<<universalparams.NUM_FPGAS_ <<", device_count: "<<device_count<<", NUM_HBM_ARGS: "<<NUM_HBM_ARGS<< std::endl;
	
	// Allocate Buffer in Global Memory
	#ifdef FPGA_IMPL
	std::cout << "Creating Buffers..." << std::endl;
	for(unsigned int fpga=0; fpga<device_count; fpga++){ // device_count
		for (int i = 0; i < NUM_HBM_ARGS; i++) {
			std::cout << "Creating Buffer "<<i<<" (fpga "<<fpga<<")..." << std::endl;
			// OCL_CHECK(err, buffer_hbm[fpga*NUM_HBM_ARGS + i] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
											// bytes_per_iteration, &inBufExt[fpga*NUM_HBM_ARGS + i], &err));
			OCL_CHECK(err, buffer_hbm[fpga*NUM_HBM_ARGS + i] = cl::Buffer(contexts[0], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
											bytes_per_iteration, &inBufExt[fpga*NUM_HBM_ARGS + i], &err));
		}
	}
	exit(EXIT_SUCCESS);/////////////////////////////////
	
	std::cout << "Creating Center Buffers..." << std::endl;
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		for (int i = 0; i < NUM_HBMC_ARGS; i++) {
			std::cout << "Creating Center Buffer "<<i<<"..." << std::endl;
			OCL_CHECK(err, buffer_hbmc[fpga*NUM_HBMC_ARGS + i] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
											bytesc_per_iteration, &inBufExt_c[fpga*NUM_HBMC_ARGS + i], &err)); 
		}
	}
	#endif
	
	#ifdef FPGA_IMPL
	std::cout << "Creating I/O Buffers..." << std::endl;
	for(unsigned int fpga=0; fpga<device_count; fpga++){
		for(unsigned int flag=0; flag<2; flag++){
			inBufExt_input[fpga].obj = &vertex_properties[0];
			inBufExt_output[fpga].obj = &vertex_properties[0];
			
			unsigned int import_sz = _IMPORT_BATCH_SIZE * MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE; 
			unsigned int export_sz = (_EXPORT_BATCH_SIZE * MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE) / universalparams.NUM_FPGAS_; // NOTE: fewer things to export than to import
			if(universalparams.NUM_FPGAS_ == 1){ import_sz = 16; export_sz = 16; }
			
			// import_sz = 16; 
			// export_sz = 16;  // REMOVEME.
			
			if(profiling0 == true){ std::cout << "Creating Import Buffers @ fpga "<<fpga<<"..." << std::endl; }
			OCL_CHECK(err, buffer_import[fpga][flag] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
											(import_sz * sizeof(int)), &inBufExt_input[fpga], &err)); 
			
			if(profiling0 == true){ std::cout << "Creating Export Buffers..." << std::endl; }
			OCL_CHECK(err, buffer_export[fpga][flag] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
											(export_sz * sizeof(int)), &inBufExt_output[fpga], &err)); 
		}
	}
	#endif 
	
	// Set Kernel Arguments
	#ifdef FPGA_IMPL
	std::cout << "Setting Kernel Arguments..." << std::endl;
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		for (int i = 0; i < NUM_HBM_ARGS; i++) {
			std::cout << "Setting the k_vadd Argument for argument "<<i<<"..." << std::endl;
			OCL_CHECK(err, err = kernels[fpga].setArg(i, buffer_hbm[fpga*NUM_HBM_ARGS + i]));
		}
	}
	std::cout << "Setting Kernel Arguments (center HBM)..." << std::endl;
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		for (int i = 0; i < NUM_HBMC_ARGS; i++) {
			std::cout << "Setting Kernel Argument for argument "<<NUM_HBM_ARGS + i<<"..." << std::endl;
			OCL_CHECK(err, err = kernels[fpga].setArg(NUM_HBM_ARGS + i, buffer_hbmc[fpga*NUM_HBMC_ARGS + i]));
		}
	}
	#endif 
	
	// Copy input data to device global memory
	#ifdef FPGA_IMPL
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		std::cout << "Copying data (Host to Device)..." << std::endl;
		std::chrono::steady_clock::time_point begin_time0 = std::chrono::steady_clock::now();
		for (int i = 0; i < NUM_HBM_ARGS; i++) {
			std::cout << "Copying data @ channel "<<i<<" (Host to Device)..." << std::endl;
			#ifdef FPGA_IMPL
			OCL_CHECK(err, err = q[fpga].enqueueMigrateMemObjects({buffer_hbm[i]}, 0));
			#endif 
		}
		for (int i = 0; i < NUM_HBMC_ARGS; i++) {
			std::cout << "Copying data @ center channel "<<i<<" (Host to Device)..." << std::endl;
			#ifdef FPGA_IMPL
			OCL_CHECK(err, err = q[fpga].enqueueMigrateMemObjects({buffer_hbmc[i]}, 0));
			#endif 
		}
		#ifdef FPGA_IMPL
		OCL_CHECK(err, err = q[fpga].finish());
		#endif 
		double end_time0 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time0).count()) / 1000;	
		std::cout <<">>> %%%% write-to-FPGA time elapsed : "<<end_time0<<" ms, "<<(end_time0 * 1000)<<" microsecs, "<<std::endl;
	}
	#endif 
	// exit(EXIT_SUCCESS);

	#ifndef FPGA_IMPL
	acts_kernel * acts = new acts_kernel(universalparams);
	#endif 
	
	gas_import_t * import_Queue[MAX_NUM_FPGAS]; for(unsigned int t=0; t<MAX_NUM_FPGAS; t++){ import_Queue[t] = new gas_import_t[4096]; } // MAX_NUM_UPARTITIONS
	gas_process_t * process_Queue[MAX_NUM_FPGAS]; for(unsigned int t=0; t<MAX_NUM_FPGAS; t++){ process_Queue[t] = new gas_process_t[4096]; } // MAX_NUM_UPARTITIONS
	gas_export_t * export_Queue[MAX_NUM_FPGAS]; for(unsigned int t=0; t<MAX_NUM_FPGAS; t++){ export_Queue[t] = new gas_export_t[4096]; } // MAX_NUM_UPARTITIONS
	initialize_Queue(all_vertices_active_in_all_iterations, import_Queue, process_Queue, export_Queue, universalparams);

	unsigned int num_launches = 0;
	action_t * actions[MAX_NUM_FPGAS]; for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ actions[fpga] = new action_t[1024]; }
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
		if(universalparams.NUM_FPGAS_ == 1){ num_launches = load_actions_coarse(fpga, actions, _PE_BATCH_SIZE, _AU_BATCH_SIZE, _IMPORT_BATCH_SIZE, universalparams); } 
		else { num_launches = load_actions_fine(fpga, actions, _PE_BATCH_SIZE, _AU_BATCH_SIZE, _IMPORT_BATCH_SIZE, universalparams); }
	}

	// Run kernel in CPU-only environment
	#ifdef RUN_SW_KERNEL
	actvvs.clear(); actvvs_nextit.clear(); 
	actvvs.push_back(universalparams.ROOTVID);
	for(unsigned int i=0; i<128; i++){ vertices_processed[i] = 0; edges_processed[i] = 0; } 
	for(unsigned int i=0; i<universalparams.NUM_VERTICES; i++){ vdatas[i] = 0xFFFFFFFF; }
	for(unsigned int i=0; i<actvvs.size(); i++){ vdatas[actvvs[i]] = 0; }
	for(unsigned int t=0; t<MAX_NUM_UPARTITIONS; t++){ vpartition_stats[t].A = 0; vpartition_stats[t].B = 0; }
	tuple_t active_vertices_in_iteration[2][128]; for(unsigned int i=0; i<128; i++){ active_vertices_in_iteration[0][i].A = 0; active_vertices_in_iteration[0][i].B = 0; active_vertices_in_iteration[1][i].A = 0; active_vertices_in_iteration[1][i].B = 0; } 
	tuple_t active_edges_in_iteration[2][128]; for(unsigned int i=0; i<128; i++){ active_edges_in_iteration[0][i].A = 0; active_edges_in_iteration[0][i].B = 0; active_edges_in_iteration[1][i].A = 0; active_edges_in_iteration[1][i].B = 0; } 
	active_vertices_in_iteration[0][0].A = 1;
	unsigned int GraphIter;
	for (GraphIter = 0; GraphIter < 16; GraphIter++){
		std::chrono::steady_clock::time_point begin_time0 = std::chrono::steady_clock::now();
		
		for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ vpartition_stats[t].A = 0; vpartition_stats[t].B = 0; }
		run_traversal_algorithm_in_software(GraphIter, actvvs, actvvs_nextit, vertexptrbuffer, edgedatabuffer, vdatas, vertex_properties_map[GraphIter+1], vpartition_stats, vertices_processed, edges_processed, universalparams);
		
		active_vertices_in_iteration[0][GraphIter + 1].A = actvvs_nextit.size();
		active_edges_in_iteration[0][GraphIter].A = edges_processed[GraphIter];
		unsigned int num_actv_edges = 0; for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ num_actv_edges += vpartition_stats[t].B; }
		if(actvvs_nextit.size() == 0){ cout<<"no more activer vertices to process. breaking out @ iteration "<<GraphIter<<"... "<<endl; break; }
		actvvs.clear(); for(unsigned int i=0; i<actvvs_nextit.size(); i++){ actvvs.push_back(actvvs_nextit[i]); } 
		
		double end_time0 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time0).count()) / 1000;	
		cout<<"host: end of iteration "<<GraphIter<<": "<<actvvs_nextit.size()<<" active vertices generated, "<<num_actv_edges<<" edges processed in ("<<end_time0<<" ms, "<<(end_time0 * 1000)<<" microsecs)"<<endl;
		active_vertices_in_iteration[0][GraphIter].B = end_time0; 
		
		actvvs_nextit.clear();
	}
	for (unsigned int iter = 0; iter < 16; iter++) {
		unsigned int count = 0;
		cout<<"host: active partitions for iteration: "<<iter<<": ";
		for (unsigned int t = 0; t < MAX_NUM_UPARTITIONS; t++){ if(vertex_properties_map[iter][t] > 1){ count += 1; cout<<""<<t<<", "; }} 
		cout<<" ("<<count<<" partitions)"<<endl;
	}
	for (unsigned int iter = 0; iter < 16; iter++) {
		if(false){
			unsigned int count = 0;
			cout<<"host: active partitions for iteration: "<<iter<<": ";
			for (unsigned int t = 0; t < MAX_NUM_UPARTITIONS; t++){ if(vertex_properties_map[iter][t] > 1){ count += 1; cout<<""<<t<<"("<<vertex_properties_map[iter][t]<<"), "<<endl; }} 
			cout<<" ("<<count<<" partitions)"<<endl;
		}
	}
	#endif 
	// exit(EXIT_SUCCESS); 
	
	unsigned int num_iterations = universalparams.NUM_ITERATIONS; if(all_vertices_active_in_all_iterations == false){ num_iterations = GraphIter; }
	unsigned int run_idx = 0; bool __run__iteration__in__SW__modes__[MAXNUMGRAPHITERATIONS]; bool __run__iteration__in__FPGA__modes__[MAXNUMGRAPHITERATIONS];
	for (unsigned int t = 0; t < MAXNUMGRAPHITERATIONS; t++) { __run__iteration__in__SW__modes__[t] = false; __run__iteration__in__FPGA__modes__[t] = false; }
	
	bool enable_import = true; bool enable_export = true; //for(unsigned int t=0; t<2; t++){ enable_export[t] = true; }
	bool read_events_bool[MAX_NUM_FPGAS][2]; for(unsigned int t=0; t<MAX_NUM_FPGAS; t++){ for(unsigned int k=0; k<2; k++){ read_events_bool[t][k] = false; }}
	
	actvvs.clear(); actvvs_nextit.clear(); 
	actvvs.push_back(universalparams.ROOTVID);
	
	unsigned int * iters_idx[MAX_NUM_FPGAS]; for (unsigned int t = 0; t < MAX_NUM_FPGAS; t++){ iters_idx[t] = new unsigned int[MAX_NUM_UPARTITIONS]; }
	for (unsigned int t = 0; t < MAX_NUM_FPGAS; t++){ for (unsigned int k = 0; k < MAX_NUM_UPARTITIONS; k++){ iters_idx[t][k] = 0; }}
	
	// ===== Run kernel in FPGA/CPU environment =====
	unsigned int launch_idx=0;
	unsigned int epoch=0;
	unsigned int num_iterations_dense = 0;
	if(all_vertices_active_in_all_iterations == false){ for(unsigned int t = 0; t < num_iterations; t++){ if(active_vertices_in_iteration[0][t].A >= ___hybrid___engine___vertex___threshold___){ num_iterations_dense += 1; }}}
	else { num_iterations_dense = num_iterations; }
	// num_iterations_dense = num_iterations;
	
	std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
	
	// ===== sparse processing ===== 	
	#ifdef RUN_SW_KERNEL 
	bool en = true;
	unsigned int iteration_idx = 0;
	if(all_vertices_active_in_all_iterations == false){
	std::cout << endl << TIMINGRESULTSCOLOR <<"#################################################################### ACTS in SW mode ####################################################################"<< RESET << std::endl;
	for (iteration_idx = 0; iteration_idx < num_iterations; iteration_idx++){ 			
		std::cout << endl << TIMINGRESULTSCOLOR <<"-------------------------------- host: GAS iteration: ---, iteration_idx "<<iteration_idx<<" (of "<<num_iterations<<"), started... --------------------------------"<< RESET << std::endl;
		
		if(active_vertices_in_iteration[0][iteration_idx].A < ___hybrid___engine___vertex___threshold___){ __run__iteration__in__SW__modes__[iteration_idx] = true; __run__iteration__in__FPGA__modes__[iteration_idx] = false; } else{ __run__iteration__in__SW__modes__[iteration_idx] = false; __run__iteration__in__FPGA__modes__[iteration_idx] = true; }
		if(__run__iteration__in__SW__modes__[iteration_idx] == false){ cout<<"host: FINISH: maximum iteration reached. breaking out of SW mode..."<<endl; break; }
		
		std::chrono::steady_clock::time_point begin_time0 = std::chrono::steady_clock::now();
		
		for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ vpartition_stats[t].A = 0; vpartition_stats[t].B = 0; }
		run_traversal_algorithm_in_software(iteration_idx, actvvs, actvvs_nextit, vertexptrbuffer, edgedatabuffer, vertex_properties, vertex_properties_map[iteration_idx], vpartition_stats, vertices_processed, edges_processed, universalparams);
		unsigned int num_actv_edges = 0; for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ num_actv_edges += vpartition_stats[t].B; }
		cout<<"host: end of iteration "<<iteration_idx<<": ("<<actvvs_nextit.size()<<" active vertices generated, "<<num_actv_edges<<" edges processed)"<<endl;
		if(actvvs_nextit.size() == 0){ cout<<"no more activer vertices to process. breaking out... "<<endl; } 
		actvvs.clear(); for(unsigned int i=0; i<actvvs_nextit.size(); i++){ actvvs.push_back(actvvs_nextit[i]); } actvvs_nextit.clear();
		
		double end_time0 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time0).count()) / 1000;	
		if(true){ std::cout << TIMINGRESULTSCOLOR << ">>> host::sw kernel time elapsed for iteration "<<iteration_idx<<" : "<<end_time0<<" ms, "<<(end_time0 * 1000)<<" microsecs, "<< RESET << std::endl; }	
	}
	}
	#endif 
	
	// ===== dense processing =====  
	#ifdef RUN_FPGA_KERNEL
	std::cout << endl << TIMINGRESULTSCOLOR <<"#################################################################### ACTS in FPGA mode ["<<num_iterations_dense<<" iterations] ####################################################################"<< RESET << std::endl;
	std::chrono::steady_clock::time_point begin_time1 = std::chrono::steady_clock::now();
	for(epoch=0; epoch<4096; epoch+=1){ 
		std::cout << endl << TIMINGRESULTSCOLOR <<"-------------------------------- host: GAS iteration: ~"<<iters_idx[0][0]<<", launch_idx "<<launch_idx<<" (of "<<num_launches<<"), epoch "<<epoch<<", fpgas [0 - "<<universalparams.NUM_FPGAS_-1<<"] started... --------------------------------"<< RESET << std::endl;
		
		action_t action[MAX_NUM_FPGAS];
		unsigned int import_pointer[MAX_NUM_FPGAS];
		unsigned int process_pointer[MAX_NUM_FPGAS];
		unsigned int export_pointer[MAX_NUM_FPGAS];
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ action[fpga] = actions[fpga][launch_idx]; }
		
		if(iters_idx[0][0] >= num_iterations_dense && action[0].module == PROCESS_EDGES_MODULE){ cout<<"host: FINISH: maximum iteration reached. breaking out..."<<endl; break; }
		// if(iters_idx[0][0] >= num_iterations_dense){ cout<<"host: FINISH: maximum iteration reached. breaking out..."<<endl; break; } // REMOVEME.
		
		int flag = run_idx % 2; 
		#ifdef FPGA_IMPL
		if (run_idx >= 2) {
			for(unsigned int fpga=0; fpga<device_count; fpga++){ 
				if(read_events_bool[fpga][flag] == true){ OCL_CHECK(err, err = read_events[fpga][flag].wait()); }
			}
		}
		#endif 
		
		#ifdef _DEBUGMODE_HOSTPRINTS4
		if(profiling0 == true){ 
			for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ std::cout<<">>> "<<universalparams.NUM_UPARTITIONS<<" imports queue @ fpga "<<fpga<<": "; for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ std::cout<<import_Queue[fpga][t].ready_for_import<<", "; } cout<<endl; }
			for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ std::cout<<">>> "<<universalparams.NUM_UPARTITIONS<<" exports queue @ fpga "<<fpga<<": "; for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ std::cout<<export_Queue[fpga][t].ready_for_export<<", "; } cout<<endl; }
			for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ std::cout<<">>> "<<universalparams.NUM_UPARTITIONS<<" process queue @ fpga "<<fpga<<": "; for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ std::cout<<process_Queue[fpga][t].ready_for_process<<", "; } cout<<endl; }
		}
		#endif
		
		// set scalar arguments
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			action[fpga].graph_iteration = iters_idx[fpga][launch_idx];
			action[fpga].id_import = 0; 
			action[fpga].id_export = 0; 
			action[fpga].id_process = launch_idx;
			action[fpga].size_import_export = _IMPORT_BATCH_SIZE; 
		}
		
		// pre-run
		std::chrono::steady_clock::time_point begin_time2 = std::chrono::steady_clock::now();
		#ifdef ___PRE_RUN___
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
			// pre import 
			import_pointer[fpga] = INVALID_IOBUFFER_ID;
			action[fpga].id_import = INVALID_IOBUFFER_ID;
			for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t+=_IMPORT_BATCH_SIZE){
				utilityobj->checkoutofbounds("host::ERROR 2111::", import_Queue[fpga][t].ready_for_import, MAX_NUM_UPARTITIONS, fpga, t, NAp);
				bool en = true; for(unsigned int k=0; k<_IMPORT_BATCH_SIZE; k++){ if((import_Queue[fpga][t+k].ready_for_import == 0) && ((t+k) < (universalparams.NUM_UPARTITIONS))){ en = false; break; }} // FIXME?
				if(en == true){ 
					import_pointer[fpga] = t;
					action[fpga].id_import = t; 
					break; 
				}
			}
			
			// pre process 
			process_pointer[fpga] = INVALID_IOBUFFER_ID;
			action[fpga].id_process = INVALID_IOBUFFER_ID; 
			if(action[fpga].module != APPLY_UPDATES_MODULE && action[fpga].module != GATHER_FRONTIERS_MODULE){
				for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t+=_PE_BATCH_SIZE){
					utilityobj->checkoutofbounds("host::ERROR 2112::", process_Queue[fpga][t].ready_for_process, MAX_NUM_UPARTITIONS, fpga, t, NAp);
					bool en = true; for(unsigned int k=0; k<_PE_BATCH_SIZE; k++){ if((process_Queue[fpga][t+k].ready_for_process == 0) && ((t+k) < (universalparams.NUM_UPARTITIONS))){ en = false; break; }} // FIXME?
					if(en == true){
						process_pointer[fpga] = t;
						action[fpga].id_process = t; 
						action[fpga].start_pu = t; 
						
						action[fpga].size_pu = _PE_BATCH_SIZE; 
						if(universalparams.NUM_FPGAS_==1){ action[fpga].size_pu = universalparams.NUM_UPARTITIONS; }  // NEWCHANGE.
						if(action[fpga].start_pu + action[fpga].size_pu > universalparams.NUM_UPARTITIONS){ action[fpga].size_pu = universalparams.NUM_UPARTITIONS - action[fpga].start_pu; } // FIXME?
						action[fpga].skip_pu = 1;
						
						break; 
					}
				}
			}
			
			// pre export 
			export_pointer[fpga] = INVALID_IOBUFFER_ID;
			action[fpga].id_export = INVALID_IOBUFFER_ID;
			for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t+=_EXPORT_BATCH_SIZE){ 
				utilityobj->checkoutofbounds("host::ERROR 2113::", export_Queue[fpga][t].ready_for_export, MAX_NUM_UPARTITIONS, fpga, t, NAp);
				bool en = true; for(unsigned int k=0; k<_EXPORT_BATCH_SIZE; k++){ if((export_Queue[fpga][t+k].ready_for_export == 0) && ((t+k) < universalparams.NUM_UPARTITIONS)){ en = false; break; }} // FIXME?
				if(en == true){
					export_pointer[fpga] = t;
					action[fpga].id_export = t; 
					break; 
				}
			}
		}
		
		// set active source partitions 		
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
			if(all_vertices_active_in_all_iterations == false && action[fpga].id_process != INVALID_IOBUFFER_ID){
				for(unsigned int t=0; t<_IMPORT_BATCH_SIZE; t++){
					SetBit(mask_i[fpga][action[fpga].start_pu / 32], action[fpga].start_pu % 32);
				}
			}
		}				
		#endif 
		double end_time2 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time2).count()) / 1000;	
		if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> host::pre-run time elapsed : "<<end_time2<<" ms, "<<(end_time2 * 1000)<<" microsecs, "<< RESET << std::endl; }
	
		// setup import and export variables
		std::chrono::steady_clock::time_point begin_time3 = std::chrono::steady_clock::now();
		size_t import_sz = 0; size_t export_sz = 0; size_t num_import_partitions = 0; size_t num_export_partitions = 0;
		for(unsigned int fpga=0; fpga<device_count; fpga++){ 
			enable_import = true;
			enable_export = true;
			read_events_bool[fpga][flag] = true;
			
			size_t import_id = action[fpga].id_import;
			size_t export_id = action[fpga].id_export;
			
			if(import_id == INVALID_IOBUFFER_ID || universalparams.NUM_FPGAS_ == 1){
				import_id = 0; import_sz = 16; num_import_partitions = 0;
			} else {
				num_import_partitions = _IMPORT_BATCH_SIZE; if((num_import_partitions + import_id) >= universalparams.NUM_UPARTITIONS){ num_import_partitions = universalparams.NUM_UPARTITIONS - import_id; }
				import_sz = num_import_partitions * MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE; 
			}
			
			if(export_id == INVALID_IOBUFFER_ID || universalparams.NUM_FPGAS_ == 1){
				export_id = 0; export_sz = 16; num_export_partitions = 0;
			} else {
				num_export_partitions = _EXPORT_BATCH_SIZE; if((num_export_partitions + export_id) >= universalparams.NUM_UPARTITIONS){ num_export_partitions = universalparams.NUM_UPARTITIONS - export_id; }
				export_sz = (num_export_partitions * MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE) / universalparams.NUM_FPGAS_;
			}
			
			// import_id = 0; import_sz = 16; num_import_partitions = 0;
			// export_id = 0; export_sz = 16; num_export_partitions = 0; // REMOVEME.
			
			if(fpga==0){ report_statistics[___CODE___IMPORT_BATCH_SIZE___] += num_import_partitions; report_statistics[___CODE___EXPORT_BATCH_SIZE___] += num_export_partitions; }
			#ifdef _DEBUGMODE_CHECKS3 
			utilityobj->checkoutofbounds("host::ERROR 2113c::", import_id, MAX_NUM_UPARTITIONS, import_sz, export_sz, NAp);
			utilityobj->checkoutofbounds("host::ERROR 2113d::", export_id, MAX_NUM_UPARTITIONS, import_sz, export_sz, NAp);
			utilityobj->checkoutofbounds("host::ERROR 2113e::", import_sz, (MAX_NUM_UPARTITIONS * MAX_UPARTITION_SIZE), import_sz, export_sz, NAp);
			utilityobj->checkoutofbounds("host::ERROR 2113f::", export_sz, (MAX_NUM_UPARTITIONS * MAX_UPARTITION_SIZE), import_sz, export_sz, NAp);
			#endif 

			#ifdef FPGA_IMPL
			inBufExt_input[fpga].obj = &vertex_properties[import_id * MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE];
			inBufExt_output[fpga].obj = &vertex_properties[export_id * ((MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE) / universalparams.NUM_FPGAS_)];
			// inBufExt_input[fpga].obj = &vertex_properties[0];
			// inBufExt_output[fpga].obj = &vertex_properties[0];

			if(import_sz < 64){
				if(true || profiling0 == true){ std::cout << "Creating Import Buffers @ fpga "<<fpga<<"..." << std::endl; }
				OCL_CHECK(err, buffer_import[fpga][flag] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
												(import_sz * sizeof(int)), &inBufExt_input[fpga], &err)); 
			}
			
			if(export_sz < 64){
				if(true || profiling0 == true){ std::cout << "Creating Export Buffers..." << std::endl; }
				OCL_CHECK(err, buffer_export[fpga][flag] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
												(export_sz * sizeof(int)), &inBufExt_output[fpga], &err)); 
			}
			#endif 
		}
		
		double end_time3 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time3).count()) / 1000;	
		if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> host::allocate-buffer-in-global-memory time elapsed "<<end_time3<<" ms, "<<(end_time3 * 1000)<<" microsecs, "<< RESET << std::endl; }
		
		#ifdef FPGA_IMPL
		vector<cl::Event> write_event(1 * universalparams.NUM_FPGAS_);
		#endif 
		
		std::chrono::steady_clock::time_point begin_time4 = std::chrono::steady_clock::now();
		#ifdef FPGA_IMPL
		for(unsigned int fpga=0; fpga<device_count; fpga++){ 
			if(profiling0 == true){ std::cout << "Setting Import/Export Arguments..." << std::endl; }
			OCL_CHECK(err, err = kernels[fpga].setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS, buffer_import[fpga][flag]));
			OCL_CHECK(err, err = kernels[fpga].setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + 1, buffer_export[fpga][flag]));
		}
		#endif 
		double end_time4 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time4).count()) / 1000;	
		if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> host::set-kernel-arguments time elapsed : "<<end_time4<<" ms, "<<(end_time4 * 1000)<<" microsecs, "<< RESET << std::endl; }	
		
		// set scalar arguments
		#ifdef FPGA_IMPL
		for(unsigned int fpga=0; fpga<device_count; fpga++){ 
			if(profiling0 == true){ std::cout << "Setting Scalar Arguments..." << std::endl; }
			_set_args___actions(&kernels[fpga], action[fpga], mask_i[fpga], universalparams, err);
		}
		#endif 
		
		// import frontiers
		std::chrono::steady_clock::time_point begin_time5 = std::chrono::steady_clock::now();
		#ifdef FPGA_IMPL
		if(profiling0 == true){ std::cout << "Host to FPGA Transfer..." << std::endl; }
		for(unsigned int fpga=0; fpga<device_count; fpga++){ 
			if(enable_import == true){
				OCL_CHECK(err, err = q[fpga].enqueueMigrateMemObjects({buffer_import[fpga][flag]}, 0, nullptr, &write_event[fpga]));
				set_callback(write_event[fpga], "ooo_queue");
				#ifdef ___SYNC___
				OCL_CHECK(err, err = write_event[fpga].wait()); 
				#endif 
			} else {
				if(profiling0 == true){ std::cout << "Skipping Host to FPGA Transfer..." << std::endl; }
			} 
		}
		#endif
		double end_time5 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time5).count()) / 1000;	
		if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR <<">>> host::import-frontiers time elapsed : "<<end_time5<<" ms, "<<(end_time5 * 1000)<<" microsecs, "<< RESET << std::endl;	}
		
		// ===== run kernel ===== 
		for(unsigned int fpga=0; fpga<1; fpga++){
			if(action[0].start_pu != NAp) { cout<<"host: launching acts [processing stage][GAS iteration: "<<iters_idx[0][action[fpga].id_process]<<"]: fpga: "<<fpga<<", start_pu: "<<action[0].start_pu<<" [id_process: "<<action[0].id_process<<"], size_pu: "<<action[0].size_pu<<", start_pv: "<<action[0].start_pv<<", size_pv: "<<action[0].size_pv<<", start_gv: "<<action[0].start_gv<<", size_gv: "<<action[0].size_gv<<endl; }
			if(action[0].start_pv != NAp) { cout<<"host: launching acts [applying stage]: fpga: "<<fpga<<", start_pu: "<<action[0].start_pu<<", size_pu: "<<action[0].size_pu<<", start_pv: "<<action[0].start_pv<<", size_pv: "<<action[0].size_pv<<", start_gv: "<<action[0].start_gv<<", size_gv: "<<action[0].size_gv<<endl; }
			if(action[0].start_gv != NAp) { cout<<"host: launching acts [gathering stage]: fpga: "<<fpga<<", start_pu: "<<action[0].start_pu<<", size_pu: "<<action[0].size_pu<<", start_pv: "<<action[0].start_pv<<", size_pv: "<<action[0].size_pv<<", start_gv: "<<action[0].start_gv<<", size_gv: "<<action[0].size_gv<<endl; }			
			if(action[0].id_import != INVALID_IOBUFFER_ID){ cout << "host: launching acts [importing stage]: --> importing upartition: "<<action[0].id_import<<" to "<<action[0].id_import + _IMPORT_BATCH_SIZE<<" (num_import_partitions: "<<num_import_partitions<<", import_sz: "<<import_sz<<")..." <<endl; }
			if(action[0].id_export != INVALID_IOBUFFER_ID){ cout << "host: launching acts [exporting stage]: <-- exporting vpartition: "<<action[0].id_export<<" to "<<action[0].id_export + _EXPORT_BATCH_SIZE<<"  [FPGAs "; for(unsigned int n=0; n<universalparams.NUM_FPGAS_; n++){ cout<<n<<", "; } cout<<"]... (num_export_partitions: "<<num_export_partitions<<", export_sz: "<<export_sz<<")" <<endl; }				
		}
		std::chrono::steady_clock::time_point begin_time6 = std::chrono::steady_clock::now();
		#ifdef FPGA_IMPL
			for(unsigned int fpga=0; fpga<device_count; fpga++){ // device_count universalparams.NUM_FPGAS_
				for(unsigned int sub_kernel=0; sub_kernel<NUM_KERNEL_SUBLAUNCHES_PER_LAUNCH; sub_kernel++){ 
					#ifdef FPGA_IMPL
					if(profiling0 == true){ printf("Enqueueing NDRange kernel.\n"); }
					#endif 				
					#ifdef ___SYNC___
					OCL_CHECK(err, err = q[fpga].enqueueNDRangeKernel(kernels[fpga], 0, 1, 1, NULL, &kernel_events[fpga][flag]));
					set_callback(kernel_events[fpga][flag], "ooo_queue");
					OCL_CHECK(err, err = kernel_events[fpga][flag].wait()); 
					#else 
					std::vector<cl::Event> waitList; if(enable_import == true){ waitList.push_back(write_event[fpga]); }
					OCL_CHECK(err, err = q[fpga].enqueueNDRangeKernel(kernels[fpga], 0, 1, 1, &waitList, &kernel_events[fpga][flag]));
					set_callback(kernel_events[fpga][flag], "ooo_queue");
					#endif 	
				}
			}					
		#else 
			for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
				for(unsigned int sub_kernel=0; sub_kernel<NUM_KERNEL_SUBLAUNCHES_PER_LAUNCH; sub_kernel++){ 
					unsigned int offset_i = fpga * NUM_PEs;
					acts->top_function(
						(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 0], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 0]
						#if NUM_VALID_HBM_CHANNELS>1
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 1], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 1] 
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 2], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 2] 
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 3], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 3] 
						#if NUM_VALID_HBM_CHANNELS>4 
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 4], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 4] 
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 5], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 5] 
						#if NUM_VALID_HBM_CHANNELS>6
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 6], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 6] 
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 7], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 7] 
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 8], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 8] 
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 9], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 9] 
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 10], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 10] 
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 11], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 11] 
						#if NUM_VALID_HBM_CHANNELS>12
						,(HBM_channelAXI_t *)HBM_axichannel[0][offset_i + 12], (HBM_channelAXI_t *)HBM_axichannel[1][offset_i + 12]
						#endif 
						#endif 
						#endif 
						#endif 
						,(HBM_channelAXI_t *)HBM_axicenter[0][fpga], (HBM_channelAXI_t *)HBM_axicenter[1][fpga]
						// ,(HBM_channelAXI_t *)&frontier_properties[fpga][action[fpga].id_import][0], (HBM_channelAXI_t *)&frontier_properties[fpga][action[fpga].id_export][0]
						,(HBM_channelAXI_t *)&vertex_properties[0], (HBM_channelAXI_t *)&vertex_properties[0]
						,fpga ,action[fpga].module ,action[fpga].graph_iteration ,action[fpga].start_pu ,action[fpga].size_pu ,action[fpga].skip_pu ,action[fpga].start_pv_fpga ,action[fpga].start_pv ,action[fpga].size_pv ,action[fpga].start_llpset ,action[fpga].size_llpset ,action[fpga].start_llpid ,action[fpga].size_llpid ,action[fpga].start_gv_fpga ,action[fpga].start_gv ,action[fpga].size_gv ,action[fpga].id_process ,action[fpga].id_import ,action[fpga].id_export ,action[fpga].size_import_export ,action[fpga].status ,universalparams.NUM_FPGAS_			
						,mask_i[fpga][0] ,mask_i[fpga][1] ,mask_i[fpga][2] ,mask_i[fpga][3] ,mask_i[fpga][4] ,mask_i[fpga][5] ,mask_i[fpga][6] ,mask_i[fpga][7]
						,report_statistics	
						);	
				}
			}
		#endif 
		double end_time6 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time6).count()) / 1000;	
		if(true || profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> kernel time elapsed for iteration "<<iters_idx[0][launch_idx]<<", launch_idx "<<launch_idx<<" : "<<end_time6<<" ms, "<<(end_time6 * 1000)<<" microsecs, "<< RESET <<std::endl; }
		
		// export frontiers
		std::chrono::steady_clock::time_point begin_time7 = std::chrono::steady_clock::now();
		#ifdef FPGA_IMPL
		if(profiling0 == true){ std::cout << "FPGA to Host Transfer..." << std::endl; }
		for(unsigned int fpga=0; fpga<device_count; fpga++){ 
			std::vector<cl::Event> eventList; eventList.push_back(kernel_events[fpga][flag]);
			if(enable_export == true){
				OCL_CHECK(err, err = q[fpga].enqueueMigrateMemObjects({buffer_export[fpga][flag]}, CL_MIGRATE_MEM_OBJECT_HOST, &eventList,
														&read_events[fpga][flag]));		
				set_callback(read_events[fpga][flag], "ooo_queue");
				#ifdef ___SYNC___ // FIXME
				OCL_CHECK(err, err = read_events[fpga][flag].wait()); 
				#endif 
			} else {
				if(profiling0 == true){ std::cout << "Skipping FPGA to Host Transfer..." << std::endl; }
			}
		}
		#endif
		double end_time7 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time7).count()) / 1000;	
		if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> host::export-frontiers time elapsed : "<<end_time7<<" ms, "<<(end_time7 * 1000)<<" microsecs, "<< RESET << std::endl; }
		
		// post-run 
		std::chrono::steady_clock::time_point begin_time8 = std::chrono::steady_clock::now();
		#ifdef ___POST_RUN___
		// post import: remove partitions just imported from queue; add partitions just imported to process queue 
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
			if(import_pointer[fpga] == INVALID_IOBUFFER_ID){ continue; }
			for(unsigned int k=0; k<_IMPORT_BATCH_SIZE; k++){
				if(import_pointer[fpga] + k < universalparams.NUM_UPARTITIONS){
					if(universalparams.NUM_FPGAS_ > 1){ import_Queue[fpga][import_pointer[fpga] + k].ready_for_import -= universalparams.NUM_FPGAS_; }
					if(universalparams.NUM_FPGAS_ > 1){ process_Queue[fpga][import_pointer[fpga] + k].ready_for_process = 1; }
					utilityobj->checkoutofbounds("host::ERROR 2121::", import_Queue[fpga][import_pointer[fpga] + k].ready_for_import, MAX_NUM_UPARTITIONS, fpga, k, import_pointer[fpga]);		
				}
			}
		}
		
		// post process: remove partitions just processed from queue; indicate that partition has been processed 
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			if(process_pointer[fpga] == INVALID_IOBUFFER_ID){ continue; } 
			for(unsigned int k=0; k<_PE_BATCH_SIZE; k++){
				if(process_pointer[fpga] + k >= MAX_NUM_UPARTITIONS){ continue; }
				utilityobj->checkoutofbounds("host::ERROR 7120::", process_pointer[fpga] + k, MAX_NUM_UPARTITIONS, NAp, NAp, NAp);
				process_Queue[fpga][process_pointer[fpga] + k].ready_for_process = 0; 
				processed_vertex_partitions_record[iters_idx[fpga][launch_idx]][process_pointer[fpga] + k] = 1; 
			}
		}
		
		// post-apply 
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			if(action[fpga].start_pv != NAp){
				// if(fpga==0){ launch_idx += 1; if(launch_idx >= num_launches){ launch_idx = 0; }} // NEWCHANGE.
			}
		}
		
		// post gather: add partitions just gathered to export queue  
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			if(action[fpga].module != GATHER_FRONTIERS_MODULE){	continue; }
			for(unsigned int t=action[fpga].start_gv; t<action[fpga].start_gv + action[fpga].size_gv; t+=1){	
				utilityobj->checkoutofbounds("host::ERROR 7121::", t, MAX_NUM_UPARTITIONS, NAp, NAp, NAp);
				if(universalparams.NUM_FPGAS_ > 1){ export_Queue[fpga][t].ready_for_export = 1;	}
			}
		}
		
		// post export: remove partitions just exported from queue
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			if(export_pointer[fpga] == INVALID_IOBUFFER_ID){ continue; } // NEWCHANGE.
			for(unsigned int k=0; k<_EXPORT_BATCH_SIZE; k++){ 
				if(export_pointer[fpga] + k >= MAX_NUM_UPARTITIONS){ continue; }
				utilityobj->checkoutofbounds("host::ERROR 7122::", export_pointer[fpga] + k, MAX_NUM_UPARTITIONS, NAp, NAp, NAp);
				if(universalparams.NUM_FPGAS_ > 1){ export_Queue[fpga][export_pointer[fpga] + k].ready_for_export = 0; }
			}
		}
		
		// post export: transfer export to imports
		if(action[0].id_export != INVALID_IOBUFFER_ID){							
			for(unsigned int k=0; k<_EXPORT_BATCH_SIZE; k++){ 
				unsigned int upartition_id = action[0].id_export + k; 
				if(upartition_id >= universalparams.NUM_UPARTITIONS){ continue; }
				utilityobj->checkoutofbounds("host::ERROR 7123::", upartition_id, MAX_NUM_UPARTITIONS, NAp, NAp, NAp);
				for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
					if(universalparams.NUM_FPGAS_ > 1){ import_Queue[fpga][upartition_id].ready_for_import += universalparams.NUM_FPGAS_; }
					if(universalparams.NUM_FPGAS_ > 1){ import_Queue[fpga][upartition_id].iteration = iters_idx[fpga][launch_idx] + 1; }
				}
			}
		}	

		// post import: update iters_idx
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			if(import_pointer[fpga] != INVALID_IOBUFFER_ID){ 
				for(unsigned int k=0; k<_IMPORT_BATCH_SIZE; k++){ 
					if(import_pointer[fpga] + k < universalparams.NUM_UPARTITIONS){ iters_idx[fpga][import_pointer[fpga] + k] += 1; }
				}
			}
		}		
		
		// update launch_idx	
		if(process_pointer[0] != INVALID_IOBUFFER_ID){ launch_idx += 1; if(launch_idx >= num_launches){ launch_idx = 0; }}
		if(action[0].start_pv != NAp){ launch_idx += 1; if(launch_idx >= num_launches){ launch_idx = 0; }}
		if(action[0].module == GATHER_FRONTIERS_MODULE){ launch_idx += 1; if(launch_idx >= num_launches){ launch_idx = 0; }}
		#endif 
		double end_time8 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time8).count()) / 1000;	
		if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> host::post-process time elapsed : "<<end_time8<<" ms, "<<(end_time8 * 1000)<<" microsecs, "<< RESET << std::endl; }
		
		// record edges processed 			
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
			if(all_vertices_active_in_all_iterations == false && action[fpga].id_process != INVALID_IOBUFFER_ID){
				for(unsigned int t=0; t<_IMPORT_BATCH_SIZE; t++){						
					utilityobj->checkoutofbounds("host::ERROR 2223::", action[fpga].id_process, universalparams.NUM_UPARTITIONS, MAX_NUM_UPARTITIONS, NAp, NAp);
					active_edges_in_iteration[1][iters_idx[fpga][launch_idx]].A += vertex_properties_map[iters_idx[fpga][launch_idx]][action[fpga].id_process];
				}
			}
		}	
		
		run_idx += 1;
		if(universalparams.NUM_FPGAS_ == 1){ break; }
		// break; // REMOVEME.....................................................................
		// exit(EXIT_SUCCESS);
	} // epoch
	double end_time1 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time1).count()) / 1000;	
	std::cout << TIMINGRESULTSCOLOR <<">>> total kernel time elapsed for current iteration : "<<end_time1<<" ms, "<<(end_time1 * 1000)<<" microsecs, "<< RESET << std::endl;

	cout<<"-------------------------+++++++++++++++++++++++++++--------------------- num_iterations_dense: "<<num_iterations_dense<<", num_iterations: "<<num_iterations<<endl;
	for(unsigned int iter=0; iter<num_iterations; iter+=1){ // FIXME (num_launches * num_iterations * 2)
		active_vertices_in_iteration[1][iter].A = universalparams.NUM_VERTICES;
		active_vertices_in_iteration[1][iter].B = end_time1 / num_iterations_dense; // num_iterations;
		if(true){ active_edges_in_iteration[1][iter].A = universalparams.NUM_EDGES; }
	}
	#endif 
	
	#ifdef _DEBUGMODE_HOSTPRINTS//4
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ std::cout<<">>> imports @ fpga "<<fpga<<": "; for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ std::cout<<import_Queue[fpga][t].ready_for_import<<", "; } cout<<endl; }
	#endif 

	// Wait for all of the OpenCL operations to complete
    printf("Waiting...\n");
	#ifdef FPGA_IMPL
	for(unsigned int fpga=0; fpga<device_count; fpga++){
		OCL_CHECK(err, err = q[fpga].flush());
		OCL_CHECK(err, err = q[fpga].finish());
	}
	#endif 
	
	double end_time = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time).count()) / 1000;	
	std::cout << TIMINGRESULTSCOLOR <<">>> total kernel time elapsed for all iterations : "<<end_time<<" ms, "<<(end_time * 1000)<<" microsecs, "<< RESET << std::endl;
	
	unsigned int total_vertices_processed = 0; for(unsigned int iter=0; iter<num_iterations; iter++){ total_vertices_processed += vertices_processed[iter]; cout<<"host:: number of active vertices in iteration "<<iter<<": "<<(unsigned int)vertices_processed[iter]<<endl; } cout<<"host:: total: "<<total_vertices_processed<<endl;
	unsigned int total_edges_processed = 0; for(unsigned int iter=0; iter<num_iterations; iter++){ total_edges_processed += edges_processed[iter]; cout<<"host:: number of edges processed in iteration "<<iter<<": "<<(unsigned int)edges_processed[iter]<<endl; } cout<<"host:: total # edges processed across entire run: "<<total_edges_processed<<endl;
	
	cout<<"[READ_FRONTIERS, PROCESSEDGES, READ_DESTS, APPLYUPDATES, COLLECT_FRONTIERS, SAVE_DEST, GATHER_FRONTIERS]"<<endl;																									
	cout<<">>> [";
	cout<<"*"<<(report_statistics[___CODE___READ_FRONTIER_PROPERTIES___] * EDGE_PACK_SIZE) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___ECPROCESSEDGES___] * EDGE_PACK_SIZE) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___READ_DEST_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___APPLYUPDATES___] * EDGE_PACK_SIZE) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___SAVE_DEST_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___GATHER_FRONTIERINFOS___] * EDGE_PACK_SIZE) / num_iterations_dense<<"";
	cout<<"][Per FPGA / iteration]"<<endl;
	
	cout<<"[NUMBER_OF_EDGE_INSERTIONS, NUMBER_OF_EDGE_UPDATINGS, NUMBER_OF_EDGE_DELETIONS]"<<endl;																									
	cout<<">>> [";
	cout<<"*"<<(report_statistics[___CODE___NUMBER_OF_EDGE_INSERTIONS___] * EDGE_PACK_SIZE) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___NUMBER_OF_EDGE_UPDATINGS___] * EDGE_PACK_SIZE) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___NUMBER_OF_EDGE_DELETIONS___] * EDGE_PACK_SIZE) / num_iterations_dense<<", ";
	cout<<"][Per FPGA / iteration]"<<endl;
		
	cout<<"[IMPORT_BATCH_SIZE, EXPORT_BATCH_SIZE, NUM_UPARTITIONS, NUM_APPLYPARTITIONS]"<<endl;	
	cout<<">>> [";
	cout<<"*"<<(report_statistics[___CODE___IMPORT_BATCH_SIZE___]) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___EXPORT_BATCH_SIZE___]) / num_iterations_dense<<", ";
	cout<<"*"<<(report_statistics[___CODE___IMPORT_BATCH_SIZE___] / num_iterations_dense) * MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE<<", ";
	cout<<"*"<<(report_statistics[___CODE___EXPORT_BATCH_SIZE___] / num_iterations_dense) * MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE<<", ";
	cout<<"*"<<universalparams.NUM_UPARTITIONS<<", ";
	cout<<"*"<<universalparams.NUM_APPLYPARTITIONS<<", ";
	cout<<"][Per FPGA / iteration]"<<endl;

	cout<< TIMINGRESULTSCOLOR << ">>> total processing achieved in "<<epoch / num_launches<<" 'iterations' | versus ideal "<<num_iterations_dense<<" GAS iterations"<< RESET <<endl;

	unsigned long total_edges_traversed = 0;
	unsigned long total_time_elapsed = 0;
		
	cout<<"host:: FPGA-Only mode"<<endl;
	unsigned int tmp = 0;
	for(unsigned int t=0; t<num_iterations; t++){ 
		tmp += active_vertices_in_iteration[1][t].B;
		cout<<"--- "<<active_vertices_in_iteration[1][t].A<<" active vertices processed in iteration "<<t<<" in "<<active_vertices_in_iteration[1][t].B<<" ms  [FPGA]"<<endl; 
	}	
	if(all_vertices_active_in_all_iterations == false){ std::cout << TIMINGRESULTSCOLOR <<">>> total kernel time elapsed for all iterations : "<<tmp<<" ms, "<<(tmp * 1000)<<" microsecs, "<< RESET << std::endl; }
	
	if(all_vertices_active_in_all_iterations == false){ 
		cout<<"host:: Software-Only mode"<<endl;
		unsigned int tmp = 0;
		for(unsigned int t=0; t<num_iterations; t++){ 
			tmp += active_vertices_in_iteration[0][t].B;
			cout<<"--- "<<active_vertices_in_iteration[0][t].A<<" active vertices processed in iteration "<<t<<" in "<<active_vertices_in_iteration[0][t].B<<" ms  [SW]"<<endl; 
		}	
		std::cout << TIMINGRESULTSCOLOR <<">>> total kernel time elapsed for all iterations : "<<tmp<<" ms, "<<(tmp * 1000)<<" microsecs, "<< RESET << std::endl;
	}
	
	if(all_vertices_active_in_all_iterations == false){ 
		cout<<"host:: Hybrid (Software-FPGA) mode"<<endl;
		total_edges_traversed = 0;
		total_time_elapsed = 0;
		for(unsigned int t=0; t<num_iterations; t++){ 
			if(active_vertices_in_iteration[0][t].A < ___hybrid___engine___vertex___threshold___){
				total_edges_traversed += active_edges_in_iteration[0][t].A;
				total_time_elapsed += active_vertices_in_iteration[0][t].B;
				cout<<"--- "<<active_vertices_in_iteration[0][t].A<<" active vertices processed in iteration "<<t<<" in "<<active_vertices_in_iteration[0][t].B<<" ms  [SW]"<<endl; 
			} else { 	
				total_edges_traversed += active_edges_in_iteration[1][t].A;
				total_time_elapsed += active_vertices_in_iteration[1][t].B;
				cout<<"--- "<<active_vertices_in_iteration[1][t].A<<" active vertices processed in iteration "<<t<<" in "<<active_vertices_in_iteration[1][t].B<<" ms  [FPGA]"<<endl; 		
			}
		} 
		cout<<"---+++++++++++++++++++++++++++++++++++++++++++++++++ total_edges_traversed: "<<total_edges_traversed<<endl; 
		std::cout << TIMINGRESULTSCOLOR <<">>> total kernel time elapsed for all iterations : "<<total_time_elapsed<<" ms, "<<(total_time_elapsed * 1000)<<" microsecs, "<< RESET << std::endl;
	}
	
	if(all_vertices_active_in_all_iterations == true){ 
		std::cout << endl << TIMINGRESULTSCOLOR << ">>> "<<": Total # edges processed: " << universalparams.NUM_EDGES << ", time elapsed per iteration = " << end_time / num_iterations << " ms "<< RESET << std::endl;			
		unsigned int num_trav_edges = universalparams.NUM_EDGES; if(universalparams.ALGORITHM == HITS){ num_trav_edges = universalparams.NUM_EDGES / 2; } else { num_trav_edges = universalparams.NUM_EDGES; }
		std::cout << TIMINGRESULTSCOLOR << ">>> "<<": Average Throughput (MTEPS) = " << (num_trav_edges / (end_time / num_iterations)) / 1000 << " MTEPS, Throughput (BTEPS) = " << (num_trav_edges / (end_time / num_iterations)) / 1000000 << " BTEPS "<< RESET << std::endl; 
	} else {
		std::cout << endl << TIMINGRESULTSCOLOR << ">>> "<<": Total # edges processed: " << total_edges_traversed << ", time elapsed = " << total_time_elapsed << " ms "<< RESET << std::endl;			
		std::cout << TIMINGRESULTSCOLOR << ">>> "<<": Average Throughput (MTEPS) = " << (total_edges_traversed / total_time_elapsed) / 1000 << " MTEPS, Throughput (BTEPS) = " << (double)(total_edges_traversed / total_time_elapsed) / 1000000 << " BTEPS "<< RESET << std::endl;			
	}
	
	#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
	std::cout << TIMINGRESULTSCOLOR << ">>> "<<": Average Edge-update Throughput (MUEPS) = " << ((universalparams.NUM_UPARTITIONS * universalparams.NUM_APPLYPARTITIONS * NUM_PEs * universalparams.NUM_FPGAS_ * 512 * 16) / (end_time / num_iterations)) / 1000 << " MUEPS, Throughput (BTEPS) = " << ((universalparams.NUM_UPARTITIONS * universalparams.NUM_APPLYPARTITIONS * NUM_PEs * universalparams.NUM_FPGAS_ * 512 * 16) / (end_time / num_iterations)) / 1000000 << " BTEPS "<< RESET << std::endl;			
	#endif 
	
	// Copy Result from Device Global Memory to Host Local Memory
	#ifdef FPGA_IMPL
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		std::cout << "Getting Results (Device to Host)..." << std::endl;
		std::chrono::steady_clock::time_point begin_time2 = std::chrono::steady_clock::now();
		for (int i = 0; i < NUM_HBMC_ARGS; i++) {
			std::cout << "Copying data @ center channel "<<i<<" (Device to Host)..." << std::endl;
			#ifdef FPGA_IMPL
			OCL_CHECK(err, err = q[fpga].enqueueMigrateMemObjects({buffer_hbmc[i]}, CL_MIGRATE_MEM_OBJECT_HOST));
			#endif 
		}
		#ifdef FPGA_IMPL
		OCL_CHECK(err, err = q[fpga].finish());
		#endif 
		double end_time2 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time2).count()) / 1000;	
		std::cout <<">>> read-from-FPGA time elapsed : "<<end_time2<<" ms, "<<(end_time2 * 1000)<<" microsecs, "<<std::endl;
	}	
	#endif 
	
	// Wait for all of the OpenCL operations to complete
	#ifdef FPGA_IMPL
    printf("Waiting...\n");
	for(unsigned int fpga=0; fpga<device_count; fpga++){
		OCL_CHECK(err, err = q[fpga].flush());
		OCL_CHECK(err, err = q[fpga].finish());
	}
	#endif
	
	cout<<"host::run:: Graph: "<<graph_path<<" ran successfully."<<endl;
	printf("TEST %s\n", "PASSED");
    return EXIT_SUCCESS;
}


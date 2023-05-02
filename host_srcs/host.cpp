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
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 5, int(action.skip_pu)));	
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 6, int(action.start_pv_fpga)));	
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 7, int(action.start_pv)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 8, int(action.size_pv)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 9, int(action.start_llpset)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 10, int(action.size_llpset)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 11, int(action.start_llpid)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 12, int(action.size_llpid)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 13, int(action.start_gv_fpga)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 14, int(action.start_gv)));
	OCL_CHECK(err, err = kernels->setArg(NUM_HBM_ARGS + NUM_HBMC_ARGS + NUM_HBMIO_ARGS + 15, int(action.size_gv)));
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
	
	// process
	for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t+=_PE_BATCH_SIZE){ 
		action_t action;
		
		action.module = PROCESS_EDGES_MODULE;
		action.graph_iteration = NAp;
		
		action.start_pu = t;
		action.size_pu = _PE_BATCH_SIZE; 
		if(universalparams.NUM_FPGAS_==1){ action.size_pu = 1024; }  // NEWCHANGE.
		if(action.start_pu + action.size_pu > universalparams.NUM_UPARTITIONS){ action.size_pu = universalparams.NUM_UPARTITIONS - action.start_pu; } // FIXME?
		action.skip_pu = 1;
		
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
	// return index; // FIXME.
	
	for(unsigned int apply_id=0; apply_id<universalparams.NUM_APPLYPARTITIONS; apply_id+=_AU_BATCH_SIZE){ 
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
		if(universalparams.NUM_FPGAS_==1){ action.size_pv = 2000; }  // NEWCHANGE.
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
		if(universalparams.NUM_FPGAS_==1){ action.size_gv = 2000; } // NEWCHANGE.
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

// void initialize_Queue(bool all_vertices_active_in_all_iterations, gas_import_t import_Queue[MAX_NUM_FPGAS][MAX_NUM_UPARTITIONS], gas_process_t process_Queue[MAX_NUM_FPGAS][MAX_NUM_UPARTITIONS], gas_export_t export_Queue[MAX_NUM_FPGAS][MAX_NUM_UPARTITIONS], universalparams_t universalparams){
void initialize_Queue(bool all_vertices_active_in_all_iterations, gas_import_t * import_Queue[MAX_NUM_FPGAS], gas_process_t * process_Queue[MAX_NUM_FPGAS], gas_export_t * export_Queue[MAX_NUM_FPGAS], universalparams_t universalparams){
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
		for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ 
			import_Queue[fpga][t].ready_for_import = 0;
			if(all_vertices_active_in_all_iterations == true){ process_Queue[fpga][t].ready_for_process = 1; } else { process_Queue[fpga][t].ready_for_process = 0; }
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
		if(vid / MAX_UPARTITION_SIZE >= MAX_NUM_UPARTITIONS){ cout<<"ERROR: vid("<<vid<<") / MAX_UPARTITION_SIZE("<<MAX_UPARTITION_SIZE<<") >= MAX_NUM_UPARTITIONS("<<MAX_NUM_UPARTITIONS<<"). exiting..."<<endl; exit(EXIT_FAILURE); }
		if(vptr_end < vptr_begin){ cout<<"ERROR: vptr_end("<<vptr_end<<") < vptr_begin("<<vptr_begin<<"). exiting..."<<endl; exit(EXIT_FAILURE); }
		#endif
		vertices_processed[GraphIter] += 1; 
		vpartition_stats[vid / MAX_UPARTITION_SIZE].A += 1; 
		vpartition_stats[vid / MAX_UPARTITION_SIZE].B += edges_size; // 
	
		for(unsigned int k=0; k<edges_size; k++){
			unsigned int dstvid = edgedatabuffer[vptr_begin + k].dstvid;
			unsigned int res = vdatas[vid] + 1;
			value_t vprop = vdatas[dstvid];
		
			value_t vtemp = min(vprop, res);
			vdatas[dstvid] = vtemp;
			if(vtemp != vprop){ 
				actvvs_nextit.push_back(dstvid);
				#ifdef _DEBUGMODE_CHECKS3
				if(dstvid / MAX_UPARTITION_SIZE >= universalparams.NUM_UPARTITIONS){ cout<<"ERROR 232. dstvid ("<<dstvid<<") / MAX_UPARTITION_SIZE ("<<MAX_UPARTITION_SIZE<<") >= universalparams.NUM_UPARTITIONS ("<<universalparams.NUM_UPARTITIONS<<"). vid: "<<vid<<". EXITING..."<<endl; exit(EXIT_FAILURE); }	
				#endif 
				vertex_properties_map[dstvid / MAX_UPARTITION_SIZE] = 1;
			}
			edges_processed[GraphIter] += 1; 
		}
	}
	return; 
}

long double host::runapp(std::string binaryFile__[2], 
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
	unsigned int _EXPORT_BATCH_SIZE = (_GF_BATCH_SIZE * _K1); // 24
	unsigned int _IMPORT_EXPORT_GRANULARITY_VECSIZE = 8184;
	
	cout<<"app::run::  NUM_FPGAS: "<<universalparams.NUM_FPGAS_<<endl;
	cout<<"app::run::  NUM_PEs: "<<NUM_PEs<<endl;
	cout<<"app::run::  universalparams.GLOBAL_NUM_PEs_: "<<universalparams.GLOBAL_NUM_PEs_<<endl;
	cout<<"app::run::  RUN_IN_ASYNC_MODE: "<<RUN_IN_ASYNC_MODE<<endl;
	cout<<"app::run::  _PE_BATCH_SIZE: "<<_PE_BATCH_SIZE<<endl;
	cout<<"app::run::  GF_BATCH_SIZE: "<<_GF_BATCH_SIZE<<endl;
	cout<<"app::run::  AU_BATCH_SIZE: "<<_AU_BATCH_SIZE<<endl;
	cout<<"app::run::  IMPORT_BATCH_SIZE: "<<_IMPORT_BATCH_SIZE<<endl;
	cout<<"app::run::  EXPORT_BATCH_SIZE: "<<_EXPORT_BATCH_SIZE<<endl;
	
	unsigned int report_statistics[64]; for(unsigned int t=0; t<64; t++){ report_statistics[t] = 0; }
	unsigned int mask_i[MAX_NUM_FPGAS][MAX_IMPORT_BATCH_SIZE]; for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ for(unsigned int t=0; t<MAX_IMPORT_BATCH_SIZE; t++){ mask_i[fpga][t] = 0; }}
	
	bool all_vertices_active_in_all_iterations = false; 
	if(universalparams.ALGORITHM == PAGERANK || universalparams.ALGORITHM == CF || universalparams.ALGORITHM == HITS || universalparams.ALGORITHM == SPMV){ all_vertices_active_in_all_iterations = true; }
	
	// sw sssp
	long double vertices_processed[128]; long double edges_processed[128]; for(unsigned int i=0; i<128; i++){ vertices_processed[i] = 0; edges_processed[i] = 0; }
	vector<value_t> actvvs; vector<value_t> actvvs_nextit; actvvs.push_back(1);
	tuple_t * vpartition_stats = new tuple_t[MAX_NUM_UPARTITIONS];
	for(unsigned int t=0; t<MAX_NUM_UPARTITIONS; t++){ vpartition_stats[t].A = 0; vpartition_stats[t].B = 0; }
	float ___hybrid___engine___vertex___threshold___ = (0.3 * universalparams.NUM_VERTICES) / 100; // 0.7
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
	#ifdef FPGA_IMPL
    std::cout << "Creating Context..." << std::endl;
    auto devices = xcl::get_xil_devices();
    bool valid_device = false;
	unsigned int device_count = devices.size(); // FIXME.
	unsigned int num_u280_devices = 0; 
	for(unsigned int fpga=0; fpga<devices.size(); fpga++){ if(devices[fpga].getInfo<CL_DEVICE_NAME>() == "xilinx_u280_xdma_201920_3"){ num_u280_devices += 1; }}
	device_count = num_u280_devices;
	cout<<"------------------------------------------- host: "<<device_count<<" devices found. -------------------------------------------"<<endl;
	if(device_count==0){ cout<<"host: ERROR 234. no FPGA devices found. EXITING..."<<endl; exit(EXIT_FAILURE); }
	for(unsigned int fpga=0; fpga<device_count; fpga++){ // device_count
        auto device = devices[fpga];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, contexts[fpga] = cl::Context(device, nullptr, nullptr, nullptr, &err));
		
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
        if (err != CL_SUCCESS) {
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
	std::vector<int, aligned_allocator<int> > frontier_properties[MAX_NUM_UPARTITIONS][MAX_NUM_FPGAS]; 
	for(unsigned int p_u=0; p_u<MAX_NUM_UPARTITIONS; p_u++){ 
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			frontier_properties[p_u][fpga] = std::vector<int, aligned_allocator<int> >(MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE); 
		}
	}
	#else 
	vector<unsigned int> vertex_properties((MAX_NUM_UPARTITIONS * MAX_UPARTITION_SIZE));
	vector<unsigned int> vdatas((MAX_NUM_UPARTITIONS * MAX_UPARTITION_SIZE));
	std::vector<unsigned int> frontier_properties[MAX_NUM_UPARTITIONS][MAX_NUM_FPGAS]; 
	for(unsigned int p_u=0; p_u<MAX_NUM_UPARTITIONS; p_u++){ 
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			frontier_properties[p_u][fpga] = std::vector<unsigned int>(MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE); 
		}
	}
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
		inBufExt_input[fpga].obj = frontier_properties[0][0].data(); 
		inBufExt_input[fpga].param = 0;
		inBufExt_input[fpga].flags = pc[NUM_HBM_ARGS + NUM_HBMC_ARGS + 0];
	}
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		inBufExt_output[fpga].obj = frontier_properties[0][0].data(); 
		inBufExt_output[fpga].param = 0;
		inBufExt_output[fpga].flags = pc[NUM_HBM_ARGS + NUM_HBMC_ARGS + 1];
	}
	#endif 
	
	// Allocate Buffer in Global Memory
	#ifdef FPGA_IMPL
	std::cout << "Creating Buffers..." << std::endl;
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		for (int i = 0; i < NUM_HBM_ARGS; i++) {
			std::cout << "Creating Buffer "<<i<<"..." << std::endl;
			// #if NUM_PEs==1 // NEWCHANGE.
			// OCL_CHECK(err, buffer_hbm[fpga*NUM_HBM_ARGS + i] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
											// bytes_per_iteration, HBM_axichannel_vector[fpga][i].data(), &err));
			// #else 
			OCL_CHECK(err, buffer_hbm[fpga*NUM_HBM_ARGS + i] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
											bytes_per_iteration, &inBufExt[fpga*NUM_HBM_ARGS + i], &err));
			// #endif 
		}
	}
	
	std::cout << "Creating Center Buffers..." << std::endl;
	for(unsigned int fpga=0; fpga<device_count; fpga++){ 
		for (int i = 0; i < NUM_HBMC_ARGS; i++) {
			std::cout << "Creating Center Buffer "<<i<<"..." << std::endl;
			OCL_CHECK(err, buffer_hbmc[fpga*NUM_HBMC_ARGS + i] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
											bytesc_per_iteration, &inBufExt_c[fpga*NUM_HBMC_ARGS + i], &err)); 
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
	
	// gas_import_t import_Queue[MAX_NUM_FPGAS][MAX_NUM_UPARTITIONS]; 
	// gas_process_t process_Queue[MAX_NUM_FPGAS][MAX_NUM_UPARTITIONS]; 
	// gas_export_t export_Queue[MAX_NUM_FPGAS][MAX_NUM_UPARTITIONS]; 
	gas_import_t * import_Queue[MAX_NUM_FPGAS]; for(unsigned int t=0; t<MAX_NUM_FPGAS; t++){ import_Queue[t] = new gas_import_t[MAX_NUM_UPARTITIONS]; }
	gas_process_t * process_Queue[MAX_NUM_FPGAS]; for(unsigned int t=0; t<MAX_NUM_FPGAS; t++){ process_Queue[t] = new gas_process_t[MAX_NUM_UPARTITIONS]; }
	gas_export_t * export_Queue[MAX_NUM_FPGAS]; for(unsigned int t=0; t<MAX_NUM_FPGAS; t++){ export_Queue[t] = new gas_export_t[MAX_NUM_UPARTITIONS]; }
	initialize_Queue(all_vertices_active_in_all_iterations, import_Queue, process_Queue, export_Queue, universalparams);

	unsigned int num_launches = 0;
	action_t * actions[MAX_NUM_FPGAS]; for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ actions[fpga] = new action_t[1024]; }
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
		num_launches = load_actions_fine(fpga, actions, _PE_BATCH_SIZE, _AU_BATCH_SIZE, _IMPORT_BATCH_SIZE, universalparams);
	}

	// Run kernel in CPU-only environment
	#ifdef RUN_SW_KERNEL
	actvvs.clear(); actvvs_nextit.clear(); 
	actvvs.push_back(1);
	for(unsigned int i=0; i<128; i++){ vertices_processed[i] = 0; edges_processed[i] = 0; } 
	for(unsigned int i=0; i<universalparams.NUM_VERTICES; i++){ vdatas[i] = 0xFFFFFFFF; }
	for(unsigned int i=0; i<actvvs.size(); i++){ vdatas[actvvs[i]] = 0; }
	for(unsigned int t=0; t<MAX_NUM_UPARTITIONS; t++){ vpartition_stats[t].A = 0; vpartition_stats[t].B = 0; }
	tuple_t active_vertices_in_iteration[2][128]; for(unsigned int i=0; i<128; i++){ active_vertices_in_iteration[0][i].A = 0; active_vertices_in_iteration[0][i].B = 0; active_vertices_in_iteration[1][i].A = 0; active_vertices_in_iteration[1][i].B = 0; } 
	active_vertices_in_iteration[0][0].A = 1;
	unsigned int GraphIter;
	for (GraphIter = 0; GraphIter < 16; GraphIter++) {
		std::chrono::steady_clock::time_point begin_time0 = std::chrono::steady_clock::now();
		
		for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ vpartition_stats[t].A = 0; vpartition_stats[t].B = 0; }
		run_traversal_algorithm_in_software(GraphIter, actvvs, actvvs_nextit, vertexptrbuffer, edgedatabuffer, vdatas, vertex_properties_map[GraphIter], vpartition_stats, vertices_processed, edges_processed, universalparams);
		
		active_vertices_in_iteration[0][GraphIter + 1].A = actvvs_nextit.size();
		unsigned int num_actv_edges = 0; for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ num_actv_edges += vpartition_stats[t].B; }
		if(actvvs_nextit.size() == 0){ cout<<"no more activer vertices to process. breaking out... "<<endl; break; }
		actvvs.clear(); for(unsigned int i=0; i<actvvs_nextit.size(); i++){ actvvs.push_back(actvvs_nextit[i]); } 
		
		double end_time0 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time0).count()) / 1000;	
		cout<<"host: end of iteration "<<GraphIter<<": "<<actvvs_nextit.size()<<" active vertices generated, "<<num_actv_edges<<" edges processed in ("<<end_time0<<" ms, "<<(end_time0 * 1000)<<" microsecs)"<<endl;
		active_vertices_in_iteration[0][GraphIter].B = end_time0;
		
		actvvs_nextit.clear();
	}
	for (unsigned int iter = 0; iter < 16; iter++) {
		unsigned int count = 0;
		cout<<"host: active partitions for iteration: "<<iter<<": ";
		for (unsigned int t = 0; t < MAX_NUM_UPARTITIONS; t++){ if(vertex_properties_map[iter][t] == 1){ count += 1; cout<<""<<t<<", "; }} 
		cout<<" ("<<count<<" partitions)"<<endl;
	}
	#endif 
	
	unsigned int num_iterations = universalparams.NUM_ITERATIONS; if(all_vertices_active_in_all_iterations == false){ num_iterations = GraphIter; }
	unsigned int run_idx = 0; bool __run__iteration__in__SW__modes__[MAXNUMGRAPHITERATIONS]; bool __run__iteration__in__FPGA__modes__[MAXNUMGRAPHITERATIONS];
	for (unsigned int t = 0; t < MAXNUMGRAPHITERATIONS; t++) { __run__iteration__in__SW__modes__[t] = false; __run__iteration__in__FPGA__modes__[t] = false; }
	
	// Run kernel in FPGA/CPU environment
	std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
	for (unsigned int iteration_idx = 0; iteration_idx < num_iterations; iteration_idx++) {
		
		if(active_vertices_in_iteration[0][iteration_idx].A < ___hybrid___engine___vertex___threshold___){ __run__iteration__in__SW__modes__[iteration_idx] = true; __run__iteration__in__FPGA__modes__[iteration_idx] = false; } else{ __run__iteration__in__SW__modes__[iteration_idx] = false; __run__iteration__in__FPGA__modes__[iteration_idx] = true; }
		if(all_vertices_active_in_all_iterations == true){ __run__iteration__in__SW__modes__[iteration_idx] = false; __run__iteration__in__FPGA__modes__[iteration_idx] = true; } 

		// FIXME
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ process_Queue[fpga][t].ready_for_process = 1; }} // FIXME
		
		// sw kernel 
		#ifdef RUN_SW_KERNEL 
		if(__run__iteration__in__SW__modes__[iteration_idx] == true){
			std::cout << endl << TIMINGRESULTSCOLOR <<"#################################################################### GAS iteration: "<<iteration_idx<< " [SW mode] ####################################################################"<< RESET << std::endl;
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
		#endif 
		
		// switch (SW to FPGA)
		#ifdef RUN_SW_KERNEL
		if(iteration_idx > 0){
			if(__run__iteration__in__FPGA__modes__[iteration_idx] == true && __run__iteration__in__FPGA__modes__[iteration_idx-1] == false){
				cout<<"host: switching from SW to FPGA @ iteration "<<iteration_idx<<"... "<<endl;
				for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
					for(unsigned int p=0; p<universalparams.NUM_UPARTITIONS; p+=1){ 
						import_Queue[fpga][p].ready_for_import = universalparams.NUM_FPGAS_;
					}
				}
			}	
		}
		#endif 
		
		// fpga kernel 
		#ifdef RUN_FPGA_KERNEL
		if(__run__iteration__in__FPGA__modes__[iteration_idx] == true){
			std::cout << endl << TIMINGRESULTSCOLOR <<"#################################################################### GAS iteration: "<<iteration_idx<< " [FPGA mode] ####################################################################"<< RESET << std::endl;
			std::chrono::steady_clock::time_point begin_time1 = std::chrono::steady_clock::now();
			for(unsigned int launch_idx=0; launch_idx<num_launches; launch_idx+=1){
				std::cout << endl << TIMINGRESULTSCOLOR <<"-------------------------------- host: GAS iteration: "<<iteration_idx<<", launch_idx "<<launch_idx<<" (of "<<num_launches<<"), fpgas [0 - "<<universalparams.NUM_FPGAS_-1<<"] started... --------------------------------"<< RESET << std::endl; 
				
				action_t action[MAX_NUM_FPGAS];
				unsigned int import_pointer[MAX_NUM_FPGAS];
				unsigned int process_pointer[MAX_NUM_FPGAS];
				unsigned int export_pointer[MAX_NUM_FPGAS];
				for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ action[fpga] = actions[fpga][launch_idx]; }
				
				int flag = run_idx % 2;
				#ifdef FPGA_IMPL
				if (run_idx >= 2) {
					for(unsigned int fpga=0; fpga<device_count; fpga++){ 
						OCL_CHECK(err, err = read_events[fpga][flag].wait());
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
					action[fpga].graph_iteration = iteration_idx;
					action[fpga].id_import = 0; 
					action[fpga].id_export = 0; 
					action[fpga].id_process = launch_idx;
					action[fpga].size_import_export = _IMPORT_BATCH_SIZE; 
				}
				
				// pre-run
				std::chrono::steady_clock::time_point begin_time2 = std::chrono::steady_clock::now();
				#ifdef ___PRE_RUN___
				if(universalparams.NUM_FPGAS_ > 1){ // FIXME.
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
									break; 
								}
							}
						}
						
						// pre gather  
						if(all_vertices_active_in_all_iterations == false && action[fpga].module == GATHER_FRONTIERS_MODULE){
							unsigned int was_changed = 0; for(unsigned int k=0; k<_PE_BATCH_SIZE; k++){ was_changed += processed_vertex_partitions_record[iteration_idx][action[fpga].start_gv + k]; }
							if(was_changed == 0){
								// action[fpga].start_gv = 0; action[fpga].size_gv = 0; 
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
					
					#ifdef RUN_SW_KERNEL 				
					for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
						if(all_vertices_active_in_all_iterations == false && action[fpga].id_process != INVALID_IOBUFFER_ID){
							for(unsigned int t=0; t<_IMPORT_BATCH_SIZE; t++){
								unsigned int p_u = ((action[fpga].start_pu + t) * universalparams.NUM_FPGAS_) + fpga; 
								mask_i[fpga][t] = 1; // vertex_properties_map[iteration_idx][p_u];
							}
						}
					}
					#endif 				
				}
				#endif 
				double end_time2 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time2).count()) / 1000;	
				if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> host::pre-run time elapsed : "<<end_time2<<" ms, "<<(end_time2 * 1000)<<" microsecs, "<< RESET << std::endl; }
			
				// Allocate Buffer in Global Memory
				std::chrono::steady_clock::time_point begin_time3 = std::chrono::steady_clock::now();
				#ifdef FPGA_IMPL
				for(unsigned int fpga=0; fpga<device_count; fpga++){ 
					size_t import_id = action[fpga].id_import;
					size_t export_id = action[fpga].id_export;
					size_t import_sz = MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE; if(action[fpga].id_import == INVALID_IOBUFFER_ID){ import_id = 0; import_sz = 16; }
					size_t export_sz = MAX_UPARTITION_VECSIZE * HBM_AXI_PACK_SIZE; if(action[fpga].id_export == INVALID_IOBUFFER_ID){ export_id = 0; export_sz = 16; } // NOTE: export from an FPGA only carries portion of a upartition
					// import_id = 0; export_id = 0; import_sz = 16; export_sz = 16; // FIXME.
					
					#ifdef _DEBUGMODE_CHECKS3 
					utilityobj->checkoutofbounds("host::ERROR 2113c::", import_id, MAX_NUM_UPARTITIONS, import_sz, export_sz, NAp);
					utilityobj->checkoutofbounds("host::ERROR 2113d::", export_id, MAX_NUM_UPARTITIONS, import_sz, export_sz, NAp);
					#endif 
					
					inBufExt_input[fpga].obj = &frontier_properties[import_id][0][0];
					inBufExt_output[fpga].obj = &frontier_properties[export_id][0][0];	
					// inBufExt_input[fpga].obj = &frontier_properties[0][0][0];
					// inBufExt_output[fpga].obj = &frontier_properties[0][0][0];	

					if(profiling0 == true){ std::cout << "Creating Import Buffers @ fpga "<<fpga<<"..." << std::endl; }
					OCL_CHECK(err, buffer_import[fpga][flag] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
													(import_sz * sizeof(int)), &inBufExt_input[fpga], &err)); 
					
					if(profiling0 == true){ std::cout << "Creating Export Buffers..." << std::endl; }
					OCL_CHECK(err, buffer_export[fpga][flag] = cl::Buffer(contexts[fpga], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
													(export_sz * sizeof(int)), &inBufExt_output[fpga], &err)); 
				}
				#endif
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
					OCL_CHECK(err, err = q[fpga].enqueueMigrateMemObjects({buffer_import[fpga][flag]}, 0, nullptr, &write_event[fpga]));
					set_callback(write_event[fpga], "ooo_queue");
					#ifdef ___SYNC___
					OCL_CHECK(err, err = write_event[fpga].wait()); 
					#endif 
				}
				#endif
				double end_time5 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time5).count()) / 1000;	
				if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR <<">>> host::import-frontiers time elapsed : "<<end_time5<<" ms, "<<(end_time5 * 1000)<<" microsecs, "<< RESET << std::endl;	}
				
				// run kernel 
				#ifdef FPGA_IMPL	
				for(unsigned int fpga=0; fpga<1; fpga++){
					if(action[0].start_pu != NAp) { cout<<"acts started [processing stage]: fpga: "<<fpga<<", start_pu: "<<action[0].start_pu<<" [id_process: "<<action[0].id_process<<"], size_pu: "<<action[0].size_pu<<", start_pv: "<<action[0].start_pv<<", size_pv: "<<action[0].size_pv<<", start_gv: "<<action[0].start_gv<<", size_gv: "<<action[0].size_gv<<endl; }
					if(action[0].start_pv != NAp) { cout<<"acts started [applying stage]: fpga: "<<fpga<<", start_pu: "<<action[0].start_pu<<", size_pu: "<<action[0].size_pu<<", start_pv: "<<action[0].start_pv<<", size_pv: "<<action[0].size_pv<<", start_gv: "<<action[0].start_gv<<", size_gv: "<<action[0].size_gv<<endl; }
					if(action[0].start_gv != NAp) { cout<<"acts started [gathering stage]: fpga: "<<fpga<<", start_pu: "<<action[0].start_pu<<", size_pu: "<<action[0].size_pu<<", start_pv: "<<action[0].start_pv<<", size_pv: "<<action[0].size_pv<<", start_gv: "<<action[0].start_gv<<", size_gv: "<<action[0].size_gv<<endl; }			
					if(action[0].id_import != INVALID_IOBUFFER_ID){ cout << "acts started [importing stage]: --> importing upartition: "<<action[0].id_import<<" to "<<action[0].id_import + _IMPORT_BATCH_SIZE<<"..." <<endl; }
					if(action[0].id_export != INVALID_IOBUFFER_ID){ cout << "acts started [exporting stage]: <-- exporting vpartition: "<<action[0].id_export<<" to "<<action[0].id_export + _EXPORT_BATCH_SIZE<<"  [FPGAs "; for(unsigned int n=0; n<universalparams.NUM_FPGAS_; n++){ cout<<n<<", "; } cout<<"]..." <<endl; }				
				}
				#endif 
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
							std::vector<cl::Event> waitList; waitList.push_back(write_event[fpga]);
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
								,(HBM_channelAXI_t *)&frontier_properties[fpga][action[fpga].id_import][0], (HBM_channelAXI_t *)&frontier_properties[fpga][action[fpga].id_export][0]
								,fpga ,action[fpga].module ,action[fpga].graph_iteration ,action[fpga].start_pu ,action[fpga].size_pu ,action[fpga].skip_pu ,action[fpga].start_pv_fpga ,action[fpga].start_pv ,action[fpga].size_pv ,action[fpga].start_llpset ,action[fpga].size_llpset ,action[fpga].start_llpid ,action[fpga].size_llpid ,action[fpga].start_gv_fpga ,action[fpga].start_gv ,action[fpga].size_gv ,action[fpga].id_process ,action[fpga].id_import ,action[fpga].id_export ,action[fpga].size_import_export ,action[fpga].status ,universalparams.NUM_FPGAS_			
								,mask_i[fpga][0] ,mask_i[fpga][1] ,mask_i[fpga][2] ,mask_i[fpga][3] ,mask_i[fpga][4] ,mask_i[fpga][5] ,mask_i[fpga][6] ,mask_i[fpga][7]
								,report_statistics	
								);	
						}
					}
				#endif 
				double end_time6 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time6).count()) / 1000;	
				if(true || profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> kernel time elapsed for iteration "<<iteration_idx<<", launch_idx "<<launch_idx<<" : "<<end_time6<<" ms, "<<(end_time6 * 1000)<<" microsecs, "<< RESET <<std::endl; }
				
				// export frontiers
				std::chrono::steady_clock::time_point begin_time7 = std::chrono::steady_clock::now();
				#ifdef FPGA_IMPL
				if(profiling0 == true){ std::cout << "FPGA to Host Transfer..." << std::endl; }
				for(unsigned int fpga=0; fpga<device_count; fpga++){ 
					std::vector<cl::Event> eventList; eventList.push_back(kernel_events[fpga][flag]);
					OCL_CHECK(err, err = q[fpga].enqueueMigrateMemObjects({buffer_export[fpga][flag]}, CL_MIGRATE_MEM_OBJECT_HOST, &eventList,
															&read_events[fpga][flag]));			
					set_callback(read_events[fpga][flag], "ooo_queue");
					#ifdef ___SYNC___ // FIXME
					OCL_CHECK(err, err = read_events[fpga][flag].wait()); 
					#endif 
				}
				#endif
				double end_time7 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time7).count()) / 1000;	
				if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> host::export-frontiers time elapsed : "<<end_time7<<" ms, "<<(end_time7 * 1000)<<" microsecs, "<< RESET << std::endl; }
				
				// post-run 
				std::chrono::steady_clock::time_point begin_time8 = std::chrono::steady_clock::now();
				#ifdef ___POST_RUN___
				if(universalparams.NUM_FPGAS_ > 1){ // FIXME.
					// post import: remove partitions just imported from queue; add partitions just imported to process queue 
					for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
						if(import_pointer[fpga] == INVALID_IOBUFFER_ID){ continue; }
						for(unsigned int k=0; k<_IMPORT_BATCH_SIZE; k++){
							if(import_pointer[fpga] + k < universalparams.NUM_UPARTITIONS){
								import_Queue[fpga][import_pointer[fpga] + k].ready_for_import -= universalparams.NUM_FPGAS_; 
								process_Queue[fpga][import_pointer[fpga] + k].ready_for_process = 1; 
								utilityobj->checkoutofbounds("host::ERROR 2121::", import_Queue[fpga][import_pointer[fpga] + k].ready_for_import, MAX_NUM_UPARTITIONS, fpga, k, import_pointer[fpga]);
								
								/* process_Queue[fpga][import_pointer[fpga] + k].tmp_state += universalparams.NUM_FPGAS_;	
								utilityobj->checkoutofbounds("host::ERROR 2121::", import_Queue[fpga][import_pointer[fpga] + k].ready_for_import, MAX_NUM_UPARTITIONS, fpga, k, import_pointer[fpga]);
								if(process_Queue[fpga][import_pointer[fpga] + k].tmp_state == universalparams.NUM_FPGAS_){ 
									utilityobj->checkoutofbounds("host::ERROR 7112::", import_pointer[fpga] + k, MAX_NUM_UPARTITIONS, NAp, NAp, NAp);
									process_Queue[fpga][import_pointer[fpga] + k].ready_for_process = 1; 
									process_Queue[fpga][import_pointer[fpga] + k].tmp_state = 0;
								}	 */			
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
							processed_vertex_partitions_record[iteration_idx][process_pointer[fpga] + k] = 1; 
						}
					}
					
					// post gather: add partitions just gathered to export queue  
					for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
						if(action[fpga].module != GATHER_FRONTIERS_MODULE){	continue; }
						for(unsigned int t=action[fpga].start_gv; t<action[fpga].start_gv + action[fpga].size_gv; t+=1){	
							utilityobj->checkoutofbounds("host::ERROR 7121::", t, MAX_NUM_UPARTITIONS, NAp, NAp, NAp);
							export_Queue[fpga][t].ready_for_export = 1;	
						}
					}
					
					// post export: remove partitions just exported from queue
					for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
						if(export_pointer[fpga] == INVALID_IOBUFFER_ID){ continue; } // NEWCHANGE.
						for(unsigned int k=0; k<_EXPORT_BATCH_SIZE; k++){ 
							if(export_pointer[fpga] + k >= MAX_NUM_UPARTITIONS){ continue; }
							utilityobj->checkoutofbounds("host::ERROR 7122::", export_pointer[fpga] + k, MAX_NUM_UPARTITIONS, NAp, NAp, NAp);
							export_Queue[fpga][export_pointer[fpga] + k].ready_for_export = 0; 
						}
					}
					
					// post export: transfer export to imports
					if(action[0].id_export != INVALID_IOBUFFER_ID){							
						for(unsigned int k=0; k<_EXPORT_BATCH_SIZE; k++){ 
							unsigned int upartition_id = action[0].id_export + k; 
							if(upartition_id >= universalparams.NUM_UPARTITIONS){ continue; }
							utilityobj->checkoutofbounds("host::ERROR 7123::", upartition_id, MAX_NUM_UPARTITIONS, NAp, NAp, NAp);
							for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
								import_Queue[fpga][upartition_id].ready_for_import += universalparams.NUM_FPGAS_; 
								import_Queue[fpga][upartition_id].iteration = iteration_idx + 1; 
							}
						}
					}				
				}				
				#endif 
				double end_time8 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time8).count()) / 1000;	
				if(profiling1_timing == true){ std::cout << TIMINGRESULTSCOLOR << ">>> host::post-process time elapsed : "<<end_time8<<" ms, "<<(end_time8 * 1000)<<" microsecs, "<< RESET << std::endl; }
				
				run_idx += 1;
				// exit(EXIT_SUCCESS);
			}
			double end_time1 = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin_time1).count()) / 1000;	
			std::cout << TIMINGRESULTSCOLOR <<">>> total kernel time elapsed for current iteration : "<<end_time1<<" ms, "<<(end_time1 * 1000)<<" microsecs, "<< RESET << std::endl;
		
			active_vertices_in_iteration[1][iteration_idx].A = universalparams.NUM_VERTICES;
			active_vertices_in_iteration[1][iteration_idx].B = end_time1;
		}
		#endif 
		
		// enable sw kernel only on rising curve. FIXME.
		if(iteration_idx > 0){
			if(__run__iteration__in__SW__modes__[iteration_idx] == true && __run__iteration__in__SW__modes__[iteration_idx-1] == false){
				cout<<"host: breaking out @ iteration "<<iteration_idx<<"... (falling edge not implemented yet)... "<<endl;
				break;
			}	
		}
		
		#ifdef _DEBUGMODE_HOSTPRINTS//4
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ std::cout<<">>> imports @ fpga "<<fpga<<": "; for(unsigned int t=0; t<universalparams.NUM_UPARTITIONS; t++){ std::cout<<import_Queue[fpga][t].ready_for_import<<", "; } cout<<endl; }
		#endif 
	}
	
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
	unsigned int total_edges_processed = 0; for(unsigned int iter=0; iter<num_iterations; iter++){ total_edges_processed += edges_processed[iter]; cout<<"host:: number of edges processed in iteration "<<iter<<": "<<(unsigned int)edges_processed[iter]<<endl; } cout<<"host:: total: "<<total_edges_processed<<endl;
	
	// report_statistics[___CODE___IMPORT_FRONTIERINFOS___]
	cout<<"[READ_FRONTIERS, PROCESSEDGES, READ_DESTS, APPLYUPDATES, COLLECT_FRONTIERS, SAVE_DEST, GATHER_FRONTIERS]"<<endl;																									
	cout<<">>> [";
	cout<<"*"<<(report_statistics[___CODE___READ_FRONTIER_PROPERTIES___] * EDGE_PACK_SIZE) / num_iterations<<", ";
	cout<<"*"<<(report_statistics[___CODE___ECPROCESSEDGES___] * EDGE_PACK_SIZE) / num_iterations<<", ";
	cout<<"*"<<(report_statistics[___CODE___READ_DEST_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_) / num_iterations<<", ";
	cout<<"*"<<(report_statistics[___CODE___APPLYUPDATES___] * EDGE_PACK_SIZE) / num_iterations<<", ";
	cout<<"*"<<(report_statistics[___CODE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_) / num_iterations<<", ";
	cout<<"*"<<(report_statistics[___CODE___SAVE_DEST_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_) / num_iterations<<", ";
	cout<<"*"<<(report_statistics[___CODE___GATHER_FRONTIERINFOS___] * EDGE_PACK_SIZE) / num_iterations<<"";
	cout<<"][Per FPGA / iteration]"<<endl;
	if(false){
	cout<<">>> [";
	cout<<""<<(report_statistics[___CODE___READ_FRONTIER_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.NUM_FPGAS_) / num_iterations<<", ";
	cout<<""<<(report_statistics[___CODE___ECPROCESSEDGES___] * EDGE_PACK_SIZE * universalparams.NUM_FPGAS_) / num_iterations<<", ";
	cout<<""<<(report_statistics[___CODE___READ_DEST_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_ * universalparams.NUM_FPGAS_) / num_iterations<<", ";
	cout<<""<<(report_statistics[___CODE___APPLYUPDATES___] * EDGE_PACK_SIZE * universalparams.NUM_FPGAS_) / num_iterations<<", ";
	cout<<""<<(report_statistics[___CODE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_ * universalparams.NUM_FPGAS_) / num_iterations<<", ";
	cout<<""<<(report_statistics[___CODE___SAVE_DEST_PROPERTIES___] * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_ * universalparams.NUM_FPGAS_) / num_iterations<<", ";
	cout<<""<<(report_statistics[___CODE___GATHER_FRONTIERINFOS___] * EDGE_PACK_SIZE * universalparams.NUM_FPGAS_) / num_iterations<<"";
	cout<<"][Per Cluster / iteration]"<<endl<<endl;
	}

	cout<<"host:: FPGA-Only mode"<<endl;
	unsigned int total_time = 0;
	for(unsigned int t=0; t<num_iterations; t++){ 
		total_time += active_vertices_in_iteration[1][t].B;
		cout<<"--- "<<active_vertices_in_iteration[1][t].A<<" active vertices processed in iteration "<<t<<" in "<<active_vertices_in_iteration[1][t].B<<" ms  [FPGA]"<<endl; 
	}
	std::cout << TIMINGRESULTSCOLOR <<">>> total kernel time elapsed for all iterations : "<<total_time<<" ms, "<<(total_time * 1000)<<" microsecs, "<< RESET << std::endl;
	if(all_vertices_active_in_all_iterations == false){ 
		cout<<"host:: Software-Only mode"<<endl;
		unsigned int total_time = 0;
		for(unsigned int t=0; t<num_iterations; t++){ 
			total_time += active_vertices_in_iteration[0][t].B;
			cout<<"--- "<<active_vertices_in_iteration[0][t].A<<" active vertices processed in iteration "<<t<<" in "<<active_vertices_in_iteration[0][t].B<<" ms  [SW]"<<endl; 
		}
		std::cout << TIMINGRESULTSCOLOR <<">>> total kernel time elapsed for all iterations : "<<total_time<<" ms, "<<(total_time * 1000)<<" microsecs, "<< RESET << std::endl;
		
		cout<<"host:: Hybrid (Software-FPGA) mode"<<endl;
		total_time = 0;
		for(unsigned int t=0; t<num_iterations; t++){ 
			if(active_vertices_in_iteration[0][t].A < ___hybrid___engine___vertex___threshold___){ 
				total_time += active_vertices_in_iteration[0][t].B;
				cout<<"--- "<<active_vertices_in_iteration[0][t].A<<" active vertices processed in iteration "<<t<<" in "<<active_vertices_in_iteration[0][t].B<<" ms  [SW]"<<endl; 
			} else { 
				total_time += active_vertices_in_iteration[1][t].B;
				cout<<"--- "<<active_vertices_in_iteration[1][t].A<<" active vertices processed in iteration "<<t<<" in "<<active_vertices_in_iteration[1][t].B<<" ms  [FPGA]"<<endl; 		
			}
		} 
		std::cout << TIMINGRESULTSCOLOR <<">>> total kernel time elapsed for all iterations : "<<total_time<<" ms, "<<(total_time * 1000)<<" microsecs, "<< RESET << std::endl;
	}

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
		// _migrate_device_to_host(&q[fpga], err, fpga, buffer_hbm, buffer_hbmc); 
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

	printf("TEST %s\n", "PASSED");
    return EXIT_SUCCESS;
}


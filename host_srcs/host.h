#ifndef HOST_H_
#define HOST_H_
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include <map>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <mutex>
#include "algorithm.h"
#include "utility.h"
#include "../include/common.h"
#ifdef FPGA_IMPL
#include "xcl2.hpp"
#include <CL/cl2.hpp>
#include <iostream>
#include <fstream>
#include <CL/cl_ext_xilinx.h>
#endif 
#include "utility.h"
#include "algorithm.h"
#include "../acts_templates/acts_kernel.h"	
#include "../include/common.h"

class host {
public:
	host(universalparams_t _universalparams);
	~host();
	
	long double runapp(std::string binaryFile[2], 
		vector<edge3_type> &edgedatabuffer, vector<edge_t> &vertexptrbuffer, 
			HBM_channelAXISW_t * HBM_axichannel[MAX_NUM_FPGAS][NUM_PEs][2], HBM_channelAXISW_t * HBM_axicenter[MAX_NUM_FPGAS][2], unsigned int hbm_channel_wwsize, unsigned int globalparams[1024], universalparams_t universalparams,
				vector<edge3_type> (&final_edge_updates)[NUM_PEs][MAX_NUM_UPARTITIONS][MAX_NUM_LLPSETS]);					
	
private:
	utility * utilityobj;
	universalparams_t myuniversalparams;
};
#endif








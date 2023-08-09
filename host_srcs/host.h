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
	
	long double runapp(string graph_path, std::string binaryFile__[2], 
		vector<edge3_type> &edgedatabuffer, vector<edge_t> &vertexptrbuffer, HBM_channelAXISW_t * HBM_EDGES[2][MAX_GLOBAL_NUM_PEs], 
			unsigned int hbm_channel_wwsize, unsigned int globalparams[1024], universalparams_t universalparams);						
	
private:
	utility * utilityobj;
	universalparams_t myuniversalparams;
};
#endif








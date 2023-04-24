#ifndef CREATE_ACT_PACK_H
#define CREATE_ACT_PACK_H
#include <mutex>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include <map>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <thread>
#include "utility.h"
#include "algorithm.h"
#include "../include/common.h"
using namespace std;

class create_act_pack {
public:
	create_act_pack(universalparams_t _universalparams);
	~create_act_pack();

	unsigned int create_actpack(
		vector<edge3_type> (&partitioned_edges)[NUM_PEs][MAX_NUM_UPARTITIONS][MAX_NUM_LLPSETS], HBM_channelAXISW_t * HBM_channel[NUM_PEs][2], map_t * edge_maps[NUM_PEs], map_t * vu_map[NUM_PEs], unsigned int offset_dest, 
		unsigned int num_upartitions, unsigned int num_vpartitions, unsigned int start_pu, unsigned int size_pu, unsigned int skip_pu, unsigned int cmd
		);
		
private:
	utility * utilityobj;
	universalparams_t universalparams;
};
#endif








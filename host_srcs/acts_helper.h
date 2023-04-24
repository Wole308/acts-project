#ifndef ACTS_HELPER_H
#define ACTS_HELPER_H
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <ctime>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <unistd.h>
#include <chrono>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <algorithm>
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
#include <string.h>
#include <mutex>
#include "utility.h"
#include "algorithm.h"
#include "../include/common.h"
using namespace std;

class acts_helper {
public:
	acts_helper(universalparams_t universalparams);
	acts_helper();
	~acts_helper();
	
	unsigned int gethash(unsigned int vid);
	unsigned int getlocalvid(unsigned int vid);
	
	void set_edgeblock_headers(int GraphIter, unsigned int v_p, 
		uint512_ivec_dt * tempvdram, uint512_ivec_dt * tempkvdram[NUM_PEs], unsigned int vdram_BASEOFFSETKVS_ACTIVEEDGEBLOCKS, unsigned int kvdram_BASEOFFSETKVS_ACTIVEEDGEBLOCKS,
			unsigned int * indexes[MAXNUM_PEs]);
	
	unsigned int extract_stats(uint512_vec_dt * vdram, uint512_vec_dt * kvbuffer[MAXNUM_PEs], 
		vector<vertex_t> &srcvids, vector<edge_t> &vertexptrbuffer, vector<edge2_type> &edgedatabuffer, 
			long double edgesprocessed_totals[128], tuple_t * vpartition_stats[MAXNUMGRAPHITERATIONS], unsigned int num_edges_processed[MAXNUMGRAPHITERATIONS], globalparams_TWOt globalparams);
			
	float get_results(string message, string graphpath, uint512_vec_dt * vdram, uint512_vec_dt * vdramtemp0, uint512_vec_dt * vdramtemp1, uint512_vec_dt * vdramtemp2, uint512_vec_dt * kvbuffer[NUM_PEs], universalparams_t universalparams);
	
	void verifyresults(uint512_vec_dt * vbuffer, globalparams_t globalparams, universalparams_t universalparams);
	void verifyresults2(uint512_vec_dt * vbuffer, globalparams_t globalparams, universalparams_t universalparams);
	void verifyresults3(uint512_vec_dt * vbuffer, globalparams_t globalparams, universalparams_t universalparams);
	
private:
	utility * utilityobj;
	universalparams_t myuniversalparams;
};
#endif








#ifndef UTILITY_H
#define UTILITY_H

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
#include "../include/common.h"
#ifdef FPGA_IMPL
#include <ap_int.h>
// #include "CL/cl.h"
#include <CL/opencl.h>
#include "../xcl.h"
// #include "xcl2.hpp"
#endif
using namespace std;

// #define UNVISITED 0
// #define VISITED_IN_CURRENT_ITERATION 1
// #define VISITED_IN_PAST_ITERATION 3

class utility {
public:
	utility(universalparams_t universalparams);
	utility();
	~utility();
	
	void printallparameters();
	void print1(string messagea, unsigned int dataa);
	void print2(string messagea, string messageb, unsigned int dataa, unsigned int datab);
	void print4(string messagea, string messageb, string messagec, string messaged, unsigned int dataa, unsigned int datab, unsigned int datac, unsigned int datad);
	void print5(string messagea, string messageb, string messagec, string messaged, string messagee, unsigned int dataa, unsigned int datab, unsigned int datac, unsigned int datad, unsigned int datae);
	void print6(string messagea, string messageb, string messagec, string messaged, string messagee, string messagef, unsigned int dataa, unsigned int datab, unsigned int datac, unsigned int datad, unsigned int datae, unsigned int datef);
	
	void printkeyvalues(string message, keyvalue_t * keyvalues, unsigned int size);
	void printkeyvalues(string message, keyvalue_t * keyvalues, unsigned int size, unsigned int skipsize);
	void printvalues(string message, unsigned int * values, unsigned int size);
	
	void checkoutofbounds(string message, unsigned int data, unsigned int upper_bound, unsigned int msgdata1, unsigned int msgdata2, unsigned int msgdata3);
	void checkforlessthanthan(string message, unsigned int data1, unsigned int data2);
	
	void stopTIME(string caption, std::chrono::steady_clock::time_point begintime, unsigned int iteration_idx);
	bool channel_is_active(unsigned int id);
	
	unsigned int get_H(unsigned int vid);
	unsigned int get_local(unsigned int vid);
	unsigned int get_global(unsigned int lvid, unsigned int H);

private:
	universalparams_t universalparams;
};
#endif








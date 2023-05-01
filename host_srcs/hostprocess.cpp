#include <chrono>
#include <stdlib.h>
#include <ctime>
#include <map>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include "utility.h"
#include "app.h"
#include "../include/common.h"
using namespace std;

int main(int argc, char** argv){
	cout<<"Hostprocess:: Graph Analytics Started..."<<endl;
	if (argc < 4) { 
		// ./host pr 1 0 12 /home/oj2zf/Documents/acts-clusterscale/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/kron_g500-logn20.mtx
		std::cout << "USAGE: ./host [--algo] [--rootvid] [--direction] [--numiterations] [--XCLBIN] [--graph_path]" << std::endl;
        return EXIT_FAILURE;
    }
	
	#ifdef _DEBUGMODE_TIMERS3
	std::chrono::steady_clock::time_point begintime_overallexecution = std::chrono::steady_clock::now();
	#endif
	
	app * appobj = new app();	
	appobj->run(argv[1], stoi(argv[2]), stoi(argv[3]), stoi(argv[4]), stoi(argv[5]), argv[6], argv[7]);
	
	#ifdef _DEBUGMODE_TIMERS3
	std::cout << endl << "TEST FINISHED" << std::endl; 
	cout<<"HOSTPROCESS:: FINISHED RUNNING "<<argv[4]<<endl;
	utility * utilityobj = new utility();
	utilityobj->stopTIME("HOSTPROCESS:: TIMING SUMMARY: TOTAL TIME ELAPSED: ", begintime_overallexecution, NAp);
	#endif
	return EXIT_SUCCESS;
}


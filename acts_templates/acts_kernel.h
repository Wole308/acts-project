#ifndef ACTS_SW_H
#define ACTS_SW_H
#include "../include/common.h"
#ifndef ___RUNNING_FPGA_SYNTHESIS___ // FPGA_IMPL
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

#include <cstdlib>
#include <iostream>
#include <time.h>


#include "../host_srcs/algorithm.h"
#include "../host_srcs/utility.h"
#endif 
#ifndef FPGA_IMPL
using namespace std;
#endif 

class acts_kernel {
public:
	acts_kernel(universalparams_t universalparams);
	~acts_kernel();
	
	// unsigned int top_function( HBM_channelAXI_t * HBM_channelA0, HBM_channelAXI_t * HBM_channelB0, HBM_channelAXI_t * HBM_channelA1, HBM_channelAXI_t * HBM_channelB1, HBM_channelAXI_t * HBM_channelA2, HBM_channelAXI_t * HBM_channelB2, HBM_channelAXI_t * HBM_channelA3, HBM_channelAXI_t * HBM_channelB3, HBM_channelAXI_t * HBM_channelA4, HBM_channelAXI_t * HBM_channelB4, HBM_channelAXI_t * HBM_channelA5, HBM_channelAXI_t * HBM_channelB5, HBM_channelAXI_t * HBM_channelA6, HBM_channelAXI_t * HBM_channelB6, HBM_channelAXI_t * HBM_channelA7, HBM_channelAXI_t * HBM_channelB7, HBM_channelAXI_t * HBM_channelA8, HBM_channelAXI_t * HBM_channelB8, HBM_channelAXI_t * HBM_channelA9, HBM_channelAXI_t * HBM_channelB9, HBM_channelAXI_t * HBM_channelA10, HBM_channelAXI_t * HBM_channelB10, HBM_channelAXI_t * HBM_channelA11, HBM_channelAXI_t * HBM_channelB11, HBM_channelAXI_t * HBM_channelA12, HBM_channelAXI_t * HBM_channelB12, HBM_channelAXI_t * HBM_channelA13, HBM_channelAXI_t * HBM_channelB13, HBM_channelAXI_t * HBM_channelA14, HBM_channelAXI_t * HBM_channelB14, HBM_channelAXI_t * HBM_centerA, HBM_channelAXI_t * HBM_centerB, 	
		// HBM_channelAXI_t * HBM_import, HBM_channelAXI_t * HBM_export, 
		// unsigned int fpga, unsigned int module, unsigned int graph_iteration, unsigned int start_pu, unsigned int size_pu, unsigned int skip_pu, unsigned int start_pv_fpga, unsigned int start_pv, unsigned int size_pv, unsigned int start_llpset, unsigned int size_llpset, unsigned int start_llpid, unsigned int size_llpid, unsigned int start_gv_fpga, unsigned int start_gv, unsigned int size_gv, unsigned int id_process, unsigned int id_import, unsigned int id_export, unsigned int size_import_export, unsigned int status, unsigned int numfpgas, unsigned int command,				
		// unsigned int mask0, unsigned int mask1, unsigned int mask2, unsigned int mask3, unsigned int mask4, unsigned int mask5, unsigned int mask6, unsigned int mask7
		// #ifndef ___RUNNING_FPGA_SYNTHESIS___	
		// ,unsigned int report_statistics[64]	
		// #endif 
		// );		

	unsigned int top_function(
 HBM_channelAXI_t * HBM_channelA0, HBM_channelAXI_t * HBM_channelB0, HBM_channelAXI_t * HBM_channelA1, HBM_channelAXI_t * HBM_channelB1, HBM_channelAXI_t * HBM_channelA2, HBM_channelAXI_t * HBM_channelB2, HBM_channelAXI_t * HBM_channelA3, HBM_channelAXI_t * HBM_channelB3, HBM_channelAXI_t * HBM_channelA4, HBM_channelAXI_t * HBM_channelB4, HBM_channelAXI_t * HBM_channelA5, HBM_channelAXI_t * HBM_channelB5, HBM_channelAXI_t * HBM_channelA6, HBM_channelAXI_t * HBM_channelB6, HBM_channelAXI_t * HBM_channelA7, HBM_channelAXI_t * HBM_channelB7, HBM_channelAXI_t * HBM_channelA8, HBM_channelAXI_t * HBM_channelB8, HBM_channelAXI_t * HBM_channelA9, HBM_channelAXI_t * HBM_channelB9, HBM_channelAXI_t * HBM_channelA10, HBM_channelAXI_t * HBM_channelB10, HBM_channelAXI_t * HBM_channelA11, HBM_channelAXI_t * HBM_channelB11, HBM_channelAXI_t * HBM_channelA12, HBM_channelAXI_t * HBM_channelB12, HBM_channelAXI_t * HBM_channelA13, HBM_channelAXI_t * HBM_channelB13, HBM_channelAXI_t * HBM_channelA14, HBM_channelAXI_t * HBM_channelB14,
 HBM_channelAXI_t * HBM_SRCA0, HBM_channelAXI_t * HBM_SRCB0, HBM_channelAXI_t * HBM_SRCA1, HBM_channelAXI_t * HBM_SRCB1, HBM_channelAXI_t * HBM_SRCA2, HBM_channelAXI_t * HBM_SRCB2, HBM_channelAXI_t * HBM_SRCA3, HBM_channelAXI_t * HBM_SRCB3, HBM_channelAXI_t * HBM_SRCA4, HBM_channelAXI_t * HBM_SRCB4, HBM_channelAXI_t * HBM_SRCA5, HBM_channelAXI_t * HBM_SRCB5, HBM_channelAXI_t * HBM_SRCA6, HBM_channelAXI_t * HBM_SRCB6, HBM_channelAXI_t * HBM_SRCA7, HBM_channelAXI_t * HBM_SRCB7, HBM_channelAXI_t * HBM_SRCA8, HBM_channelAXI_t * HBM_SRCB8, HBM_channelAXI_t * HBM_SRCA9, HBM_channelAXI_t * HBM_SRCB9, HBM_channelAXI_t * HBM_SRCA10, HBM_channelAXI_t * HBM_SRCB10, HBM_channelAXI_t * HBM_SRCA11, HBM_channelAXI_t * HBM_SRCB11, HBM_channelAXI_t * HBM_SRCA12, HBM_channelAXI_t * HBM_SRCB12, HBM_channelAXI_t * HBM_SRCA13, HBM_channelAXI_t * HBM_SRCB13, HBM_channelAXI_t * HBM_SRCA14, HBM_channelAXI_t * HBM_SRCB14,
			unsigned int fpga, unsigned int module, unsigned int graph_iteration, unsigned int start_pu, unsigned int size_pu, unsigned int skip_pu, unsigned int start_pv_fpga, unsigned int start_pv, unsigned int size_pv, unsigned int start_llpset, unsigned int size_llpset, unsigned int start_llpid, unsigned int size_llpid, unsigned int start_gv_fpga, unsigned int start_gv, unsigned int size_gv, unsigned int id_process, unsigned int id_import, unsigned int id_export, unsigned int size_import_export, unsigned int status, unsigned int numfpgas, unsigned int command,				
			unsigned int mask0, unsigned int mask1, unsigned int mask2, unsigned int mask3, unsigned int mask4, unsigned int mask5, unsigned int mask6, unsigned int mask7
			#ifndef ___RUNNING_FPGA_SYNTHESIS___	
			,unsigned int report_statistics[64]	
			#endif 
		);	

private:
	utility * utilityobj;
	universalparams_t universalparams;
	algorithm * algorithmobj;
};
#endif







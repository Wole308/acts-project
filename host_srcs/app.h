#ifndef APP_H
#define APP_H
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include <map>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h>
#include <algorithm>
#include <thread>
#include <iostream>
#include <mutex>
#include <vector>
#include <iomanip>
#include "utility.h"
#include "act_pack.h"
#include "create_act_pack.h"
#include "prepare_graph.h"
#include "algorithm.h"
#include "host.h"
#include "../acts_templates/acts_kernel.h"	
#include "../include/common.h"

class app {
public:
	app();
	~app();
	void finish();
	
	void run(std::string algo, unsigned int num_fpgas, unsigned int rootvid, int graphisundirected, unsigned int numiterations, string graph_path, std::string _binaryFile1);
	void run2(std::string algo, unsigned int num_fpgas, unsigned int rootvid, int graphisundirected, unsigned int numiterations, string graph_path, std::string _binaryFile1);
	
	void summary();
	
private:
	utility * utilityobj;
	algorithm * algorithmobj;
};
#endif









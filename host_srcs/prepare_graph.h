#ifndef PREPAREGRAPH
#define PREPAREGRAPH
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
#include "../include/common.h"
using namespace std;

class prepare_graph {
public:
	prepare_graph();
	~prepare_graph();
	
	void create_graph(string graphpath, vector<edge3_type> &edgesbuffer_dup, vector<edge_t> &vptr_dup);
	tuple_t start(string graphpath, vector<edge3_type> &edgesbuffer_dup, vector<edge_t> &vptr_dup, bool graphisundirected);	
	
private:
};
#endif









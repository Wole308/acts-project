#ifndef MAKE_GRAPH
#define MAKE_GRAPH
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

// #define TTTTT

typedef unsigned int procid_t;
typedef unsigned int vertex_id_type;

class make_graph {
public:
	make_graph();
	~make_graph();
	
	void start(string graphpath, vector<edge3_type> (&edgesbuffer_outs)[NUM_PROCS], vector<edge_t> (&vptr_out)[NUM_PROCS], bool graphisundirected);
	void load_graph(string graphpath, vector<edge3_type> &edgesbuffer, unsigned int * _max_vertex, unsigned int * _num_edges);
	void prepare_graph(vector<edge3_type> &edgesbuffer_in, vector<edge3_type> &edgesbuffer_inter, vector<edge_t> &vptr_inter, unsigned int * vertex_outdegrees, unsigned int max_vertex, unsigned int num_edges, bool graphisundirected);				

	void distr_graph(vector<edge3_type> (&edgesbuffer_outs)[NUM_PROCS], vector<edge_t> (&vptr_outs)[NUM_PROCS], vector<edge3_type> &edgesbuffer_inter, unsigned int * vertex_outdegrees, unsigned int num_vertices);
	procid_t edge_to_proc_greedy(vertex_id_type source, vertex_id_type target, int * degrees[NUM_PROCS], unsigned int proc_num_edges[NUM_PROCS]);
	procid_t edge_to_proc_hdrf(vertex_id_type source, vertex_id_type target, int * degrees[NUM_PROCS], unsigned int * vertex_outdegrees, unsigned int proc_num_edges[NUM_PROCS]);
	
private:
};
#endif









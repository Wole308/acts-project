#include "algorithm.h"
using namespace std;

algorithm::algorithm(){}
algorithm::~algorithm(){} 

unsigned int algorithm::vertex_initdata(unsigned int Algo, unsigned int index){
	if(Algo == PAGERANK){ return 1; } 
	else if(Algo == SPMV){ return 1; } 
	else if(Algo == HITS){ return 1; } 
	else if(Algo == SSSP){ return 0xFFFFFFEE; } // 0xFFFFFFEE, 1
	else { return 0; }
	return 0;
}

unsigned int algorithm::get_algorithm_id(std::string Algo){
	if(Algo == "pr"){ return PAGERANK; } 
	else if(Algo == "spmv"){ return SPMV; } 
	else if(Algo == "hits"){ return HITS; } 
	else if(Algo == "sssp"){ return SSSP; } 
	else { cout<<"ERROR: ALGORITHM "<<Algo<<" IS NOT DEFINED."<<endl; exit(EXIT_FAILURE); return 0; }
	return 0;
}







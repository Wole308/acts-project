#include "make_graph.h"
using namespace std;

make_graph::make_graph(){}
make_graph::~make_graph(){}

void make_graph::start(string graphpath, vector<edge3_type> (&edgesbuffer_outs)[NUM_PROCS], vector<edge_t> (&vptr_outs)[NUM_PROCS], bool graphisundirected){
	vector<edge3_type> edgesbuffer_in; 
	unsigned int _max_vertex = 0; 
	unsigned int _num_edges = 0;
	vector<edge3_type> edgesbuffer_inter;
	vector<edge_t> vptr_inter;
	
	load_graph(graphpath, edgesbuffer_in, &_max_vertex, &_num_edges);
	
	unsigned int num_vertices = _max_vertex + 1;
	unsigned int padded_num_vertices = num_vertices + 1000;
	unsigned int * vertex_outdegrees; vertex_outdegrees = new unsigned int[padded_num_vertices];
	prepare_graph(edgesbuffer_in, edgesbuffer_inter, vptr_inter, vertex_outdegrees, _max_vertex, _num_edges, graphisundirected);
	
	distr_graph(edgesbuffer_outs, vptr_outs, edgesbuffer_inter, vertex_outdegrees, _max_vertex + 1);
}

void make_graph::load_graph(string graphpath, vector<edge3_type> &edgesbuffer_in, unsigned int * _max_vertex, unsigned int * _num_edges){
	cout<<"load_graph:: preparing graph @ "<<graphpath<<"..."<<endl;
	unsigned int srcv = 0;
	unsigned int dstv = 0;
	unsigned int weight = 0;
	unsigned int ew = NAp;
	unsigned int num_vertices = 0;
	unsigned int num_vertices2 = 0;
	unsigned int num_edges = 0;
	unsigned int max_vertex = 0;

	std::ifstream file1_graph(graphpath);
	if (file1_graph.is_open()) {
		std::string line;
		unsigned int linecount = 0;
		while (getline(file1_graph, line)) {
			if (line.find("%") == 0){ continue; }
			if (linecount == 0){ 
				sscanf(line.c_str(), "%i %i %i", &num_vertices, &num_vertices2, &num_edges);
				num_vertices += 1000; num_vertices2 += 1000; num_edges += 1000;
				cout<<"load_graph:: dataset header: num_vertices: "<<num_vertices<<", num_vertices2: "<<num_vertices2<<", num_edges: "<<num_edges<<endl;
				linecount++; continue; }	
			
			sscanf(line.c_str(), "%i %i", &dstv, &srcv);
			if(srcv > max_vertex){ max_vertex = srcv; }
			if(dstv > max_vertex){ max_vertex = dstv; }
			
			edge3_type edge; edge.srcvid = srcv; edge.dstvid = dstv; edge.weight = rand() % 10 - 1;
			edgesbuffer_in.push_back(edge);
		
			linecount += 1;
		}
		cout<<"SUMMARY: linecount: "<<linecount<<", edgesbuffer_in.size(): "<<edgesbuffer_in.size()<<endl;
	}
	file1_graph.close();
	*_max_vertex = max_vertex;
	*_num_edges = num_edges;
	cout<<"load_graph:: Finished loading graph from file @ "<<graphpath<<" (num_vertices: "<<num_vertices<<", num_edges: "<<num_edges<<")"<<endl;
	// exit(EXIT_SUCCESS);
}

void make_graph::prepare_graph(vector<edge3_type> &edgesbuffer_in, vector<edge3_type> &edgesbuffer_inter, vector<edge_t> &vptr_inter, unsigned int * vertex_outdegrees, unsigned int max_vertex, unsigned int num_edges, bool graphisundirected){
	cout<<"prepare_graph:: starting..."<<endl;
	unsigned int mult_factor = 0;
	if(num_edges > 200000000){ cout<<"************* prepare_graph::[OVERRIDING...] Undirected graph too large to fit in memory. using directed graph instead"<<endl; graphisundirected = false; }
	if(graphisundirected==true){ mult_factor = 2; } else { mult_factor = 1; }
	
	cout<<"prepare_graph:: assigning variables..."<<endl;
	unsigned int num_vertices = max_vertex + 1;
	unsigned int padded_num_vertices = num_vertices + 1000;
	unsigned int padded_num_edges = num_edges + 1000;
	
	cout<<"prepare_graph:: creating buffers..."<<endl;
	edgesbuffer_inter.resize((mult_factor * padded_num_edges));
	vptr_inter.resize((padded_num_vertices));
	// unsigned int * vertex_outdegrees; 
	// vertex_outdegrees = new unsigned int[padded_num_vertices];
	for(unsigned int i=0; i<padded_num_vertices; i++){ vertex_outdegrees[i] = 0; vptr_inter[i] = 0; }
	for(unsigned int i=0; i<(mult_factor * padded_num_edges); i++){ edgesbuffer_inter[i].srcvid = 0; edgesbuffer_inter[i].dstvid = 0; }
	
	// populate inout degrees of all vertices 
	cout<<"prepare_graph:: populating inout degrees of all vertices..."<<endl;
	for(unsigned int i=0; i<edgesbuffer_in.size(); i++){ 
		if(edgesbuffer_in[i].srcvid >= padded_num_vertices || edgesbuffer_in[i].dstvid >= padded_num_vertices){ cout<<"prepare_graph::start(21):: edgesbuffer_in[i].srcvid("<<edgesbuffer_in[i].srcvid<<") >= padded_num_vertices("<<padded_num_vertices<<") || edgesbuffer_in[i].dstvid("<<edgesbuffer_in[i].dstvid<<") >= padded_num_vertices("<<padded_num_vertices<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }	
		vertex_outdegrees[edgesbuffer_in[i].srcvid] += 1;
		if(graphisundirected==true){ vertex_outdegrees[edgesbuffer_in[i].dstvid] += 1; }
	}
	cout<<"prepare_graph:: finished."<<endl;
	
	// creating vertex pointers...
	cout<<"prepare_graph:: creating vertex pointers..."<<endl;
	vptr_inter[0] = 0;
	for(unsigned int k=1; k<num_vertices; k++){
		vptr_inter[k] = vptr_inter[k-1] + vertex_outdegrees[k-1];
		if(k<4 || k>num_vertices-4){ cout<<"prepare_graph:: vptr_inter["<<k<<"]: "<<vptr_inter[k]<<endl; }
		if(vptr_inter[k] < vptr_inter[k-1]){ cout<<"creategraphs::writevertexptrstofile:ERROR: non-increasing vertex ptrs: vptr_inter["<<k<<"]: "<<vptr_inter[k]<<", vptr_inter["<<k-1<<"]: "<<vptr_inter[k-1]<<endl; exit(EXIT_FAILURE); }
	}
	cout<<"prepare_graph:: finished. (last: vptr_inter["<<num_vertices-1<<"]: "<<vptr_inter[num_vertices-1]<<")"<<endl;

	// checking new vertex pointers created...
	cout<<"prepare_graph:: checking new vertex pointers created..."<<endl;
	for(unsigned int k=0; k<num_vertices-2; k++){
		if(vptr_inter[k] + vertex_outdegrees[k] > vptr_inter[k+1]){ 
			cout<<"prepare_graph:: ERROR 2A. vptr_inter["<<k<<"]("<<vptr_inter[k]<<") + vertex_outdegrees["<<k<<"]("<<vertex_outdegrees[k]<<") >= vptr_inter["<<k+1<<"]("<<vptr_inter[k+1]<<")"<<endl; 
			exit(EXIT_FAILURE); 
		}
	}
	cout<<"prepare_graph:: finished."<<endl;
	
	cout<<"prepare_graph:: buffering duplicate edges..."<<endl;
	unsigned int edgesbuffer_out_size = 0;
	for(unsigned int i=0; i<num_vertices; i++){ vertex_outdegrees[i] = 0; }
	for(unsigned int i=0; i<edgesbuffer_in.size(); i++){
		unsigned int srcv = edgesbuffer_in[i].srcvid;
		unsigned int dstv = edgesbuffer_in[i].dstvid;
		unsigned int weight = edgesbuffer_in[i].weight;
		
		edge3_type edge1; edge1.srcvid = srcv; edge1.dstvid = dstv; edge1.weight = weight;
		if(vptr_inter[edge1.srcvid] + vertex_outdegrees[edge1.srcvid] >= edgesbuffer_inter.size()){ cout<<"prepare_graph::start(1):: vptr_inter[edge1.srcvid]("<<vptr_inter[edge1.srcvid]<<") + vertex_outdegrees[edge1.srcvid]("<<vertex_outdegrees[edge1.srcvid]<<") >= edgesbuffer_inter.size()("<<edgesbuffer_inter.size()<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }							
		edgesbuffer_inter[vptr_inter[edge1.srcvid] + vertex_outdegrees[edge1.srcvid]] = edge1;
		vertex_outdegrees[edge1.srcvid] += 1;
		
		if(graphisundirected==true){ 
			edge3_type edge2; edge2.srcvid = dstv; edge2.dstvid = srcv; edge2.weight = weight;
			if(vptr_inter[edge2.srcvid] + vertex_outdegrees[edge2.srcvid] >= edgesbuffer_inter.size()){ cout<<"prepare_graph::start(2):: vptr_inter[edge2.srcvid]("<<vptr_inter[edge2.srcvid]<<") + vertex_outdegrees[edge2.srcvid]("<<vertex_outdegrees[edge2.srcvid]<<") >= edgesbuffer_inter.size()("<<edgesbuffer_inter.size()<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }							
			edgesbuffer_inter[vptr_inter[edge2.srcvid] + vertex_outdegrees[edge2.srcvid]] = edge2;
			vertex_outdegrees[edge2.srcvid] += 1;
		}
		
		edgesbuffer_out_size += mult_factor;
	}
	cout<<"prepare_graph:: finished. (num_vertices: "<<num_vertices<<", num_edges: "<<num_edges<<", edgesbuffer_out_size: "<<edgesbuffer_out_size<<", edgesbuffer_inter.size(): "<<edgesbuffer_inter.size()<<")"<<endl;
	
	cout<<"prepare_graph:: checking edges for errors... "<<endl;
	unsigned int numerrors = 0;
	for(unsigned int t=1; t<edgesbuffer_out_size; t++){
		if(edgesbuffer_inter[t].srcvid < edgesbuffer_inter[t-1].srcvid){
			cout<<"prepare_graph::ERROR: edgesbuffer_inter["<<t<<"].srcvid("<<edgesbuffer_inter[t].srcvid<<") < edgesbuffer_inter["<<t-1<<"].srcvid("<<edgesbuffer_inter[t-1].srcvid<<")"<<endl;
			for(unsigned int k=0; k<16; k++){ cout<<"... prepare_graph::loadedges:: edgesbuffer_inter["<<k+t-8<<"].srcvid: "<<edgesbuffer_inter[k+t-8].srcvid<<", edgesbuffer_inter["<<k+t-8<<"].dstvid: "<<edgesbuffer_inter[k+t-8].dstvid<<endl; }
			numerrors += 1;
			if(numerrors > 1000){ exit(EXIT_FAILURE); }
			exit(EXIT_FAILURE);
		}
	}
	cout<<"finished. (numerrors: "<<numerrors<<")"<<endl;

	unsigned int zerocount = 0;
	unsigned int maxsz = edgesbuffer_out_size;
	for(unsigned int i=0; i<maxsz; i++){
		edge3_type edge = edgesbuffer_inter[i];
		if(edge.srcvid==0 && edge.dstvid==0){
			if(zerocount>10000){ cout<<"prepare_graph::writeedgestofile:: ERROR: too many zeros ("<<zerocount<<"). check... EXITING... "<<endl; exit(EXIT_FAILURE); }
			else { zerocount += 1; } 
		}
		if(i > maxsz-1){ cout<<"### prepare_graph::writeedgestofile:: maxsz: "<<maxsz<<", edge.dstvid: "<<edge.dstvid<<", edge.srcvid: "<<edge.srcvid<<endl; }
	}
	cout<<"prepare_graph:: Finished preparing graph."<<endl;
	
	edgesbuffer_in.clear();
	// delete [] vertex_outdegrees;
	return;
}

void make_graph::distr_graph(vector<edge3_type> (&edgesbuffer_outs)[NUM_PROCS], vector<edge_t> (&vptr_outs)[NUM_PROCS], vector<edge3_type> &edgesbuffer_inter, unsigned int * vertex_outdegrees, unsigned int num_vertices){
	cout<<"distr_graph:: starting..."<<endl;
	
	unsigned int procs_contents[NUM_PROCS]; for(unsigned int i=0; i<NUM_PROCS; i++){ procs_contents[i] = 0; }
	
	#ifdef XXXXXXXXXXXXXX
	int * hash_tables[NUM_PROCS]; for(unsigned int i=0; i<NUM_PROCS; i++){ hash_tables[i] = new int[num_vertices]; }
	for(unsigned int i=0; i<NUM_PROCS; i++){ for(unsigned int t=0; t<num_vertices; t++){ hash_tables[i][t] = -1; }}
	#endif 
	
	int * degrees[NUM_PROCS]; for(unsigned int i=0; i<NUM_PROCS; i++){ degrees[i] = new int[num_vertices]; }
	for(unsigned int i=0; i<NUM_PROCS; i++){ for(unsigned int t=0; t<num_vertices; t++){ degrees[i][t] = 0; }} // -1
	unsigned int proc_num_edges[NUM_PROCS]; for(unsigned int i=0; i<NUM_PROCS; i++){ proc_num_edges[i] = 0; }
	unsigned int sz = edgesbuffer_inter.size(); // 16
	for(unsigned int i=0; i<sz; i++){ 
		if(i % 10000000 == 0 || i < 64){ cout<<"SUMMARY: i: "<<i<<", edge: ["<<edgesbuffer_inter[i].srcvid<<", "<<edgesbuffer_inter[i].dstvid<<"]"<<endl; }
		
		edge3_type edge = edgesbuffer_inter[i];
		vertex_id_type source = edge.srcvid; 
		vertex_id_type target = edge.dstvid; 
		if(source >= num_vertices){ cout<<"distr_graph:: ERROR. out-of-range. source("<<source<<") >= num_vertices("<<num_vertices<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }
		if(target >= num_vertices){ cout<<"distr_graph:: ERROR. out-of-range. target("<<target<<") >= num_vertices("<<num_vertices<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }

		procid_t best_proc = edge_to_proc_greedy
		// procid_t best_proc = edge_to_proc_hdrf
				(
				source, 
				target,
				degrees,
				// vertex_outdegrees,
				proc_num_edges);
				
		procs_contents[best_proc] += 1;
		edgesbuffer_outs[best_proc].push_back(edge);
		#ifdef XXXXXXXXXXXXXX
		hash_tables[best_proc][source] = ht_index; ht_index += 1;
		hash_tables[best_proc][target] = ht_index; ht_index += 1;
		#endif 
	
		if(i < 64 || (sz-i) < 64 || i % 1000000==0){ cout<<"$$$ distr_graph:: best_proc "<<best_proc<<""<<endl; }
	}
	for(unsigned int i=0; i<NUM_PROCS; i++){ 
		cout<<"SUMMARY:: procs_contents["<<i<<"] "<<procs_contents[i]<<""<<endl;
	}
	for(unsigned int i=0; i<NUM_PROCS; i++){ 
		for(unsigned int t=0; t<16; t++){
			if(t < edgesbuffer_outs[i].size()){ cout<<"SUMMARY:: edge #"<<i<<": ["<<edgesbuffer_outs[i][t].srcvid<<", "<<edgesbuffer_outs[i][t].dstvid<<"]"<<endl; }
		}
	}
	
	#ifdef XXXXXXXXXXXXXX
	for(unsigned int i=0; i<NUM_PROCS; i++){ 
		for(unsigned int t=0; t<16; t++){
			cout<<"SUMMARY:: hash_tables["<<i<<"]["<<t<<"]: "<<hash_tables[i][t]<<"]"<<endl;
		}
	}
	#endif 
	
	unsigned int num_mirrors = 0;
	for(unsigned int vid=0; vid<num_vertices; vid++){
		unsigned int sum = 0;
		for(unsigned int i=0; i<NUM_PROCS; i++){ 
			sum += degrees[i][vid];
		}
		if(sum > 1){ num_mirrors += 1; }
	}
	cout<<"SUMMARY:: num_mirrors: "<<num_mirrors<<""<<endl;

	cout<<"prepare_graph:: Finished distributing graph."<<endl;
}

procid_t make_graph::edge_to_proc_greedy(vertex_id_type source, vertex_id_type target, int * degrees[NUM_PROCS], unsigned int proc_num_edges[NUM_PROCS]){
	bool usehash = false;
	bool userecent = false;
	unsigned int numprocs = NUM_PROCS;
	unsigned int new_sd = 0;
	unsigned int new_td = 0;

	// Compute the score of each proc.
	procid_t best_proc = -1; 
	double maxscore = 0.0;
	double epsilon = 1.0; // 1.0; 
	std::vector<double> proc_score(numprocs);
	unsigned int minedges=0xFFFFFFF; unsigned int maxedges = 0;
	for(unsigned int i = 0; i < NUM_PROCS; ++i){ if(proc_num_edges[i] < minedges){ minedges = proc_num_edges[i]; }}
	for(unsigned int i = 0; i < NUM_PROCS; ++i){ if(proc_num_edges[i] > maxedges){ maxedges = proc_num_edges[i]; }}
	#ifdef TTTTT
	cout<<"edge_to_proc_greedy:: minedges: "<<minedges<<endl;
	cout<<"edge_to_proc_greedy:: maxedges: "<<maxedges<<endl;
	for(unsigned int i = 0; i < NUM_PROCS; ++i){ cout<<"edge_to_proc_greedy:: proc_num_edges["<<i<<"]: "<<proc_num_edges[i]<<endl; }
	#endif 
	
	for (unsigned int i = 0; i < numprocs; ++i) {
		int sd = degrees[i][source] + (usehash && (source % numprocs == i));
		int td = degrees[i][target] + (usehash && (target % numprocs == i));
		
		if(sd > 0){ new_sd = sd; } else { new_sd = 0; }
		if(td > 0){ new_td = td; } else { new_td = 0; }
		
		double bal = (maxedges - proc_num_edges[i])/(epsilon + maxedges - minedges);
		// proc_score[i] = bal + ((new_sd > 0) + (new_td > 0));
		proc_score[i] = bal + (new_sd + new_td);
		
		#ifdef TTTTT
		cout<<"edge_to_proc_greedy:: degrees["<<i<<"]["<<source<<"]: "<<degrees[i][source]<<", degrees["<<i<<"]["<<target<<"]: "<<degrees[i][target]<<", sd: "<<sd<<", td: "<<td<<", bal: "<<bal<<", new_sd: "<<new_sd<<", new_td: "<<new_td<<", proc_score["<<i<<"]: "<<proc_score[i]<<endl;				
		#endif
	}
	maxscore = *std::max_element(proc_score.begin(), proc_score.end());
	#ifdef TTTTT
	cout<<"edge_to_proc_greedy:: maxscore: "<<maxscore<<endl;
	#endif 

	std::vector<procid_t> top_procs; 
	for (unsigned int i = 0; i < numprocs; ++i){
	  if (std::fabs(proc_score[i] - maxscore) < 1e-5){
		top_procs.push_back(i);
		#ifdef TTTTT
		cout<<"~~~~~~~~~~~~~~~ edge_to_proc_greedy:: a top_proc: "<<i<<endl;
		#endif 
	  }
	}
	
	best_proc = top_procs[rand() % top_procs.size()];
	if(best_proc >= NUM_PROCS){ cout<<"edge_to_proc_greedy:: ERROR. out-of-range. best_proc("<<best_proc<<") >= NUM_PROCS("<<NUM_PROCS<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }
	degrees[best_proc][source] = 1;
	degrees[best_proc][target] = 1;
	proc_num_edges[best_proc] += 1;
	return best_proc;
};

procid_t make_graph::edge_to_proc_hdrf(vertex_id_type source, vertex_id_type target, int * degrees[NUM_PROCS], unsigned int * vertex_outdegrees, unsigned int proc_num_edges[NUM_PROCS]){
	bool usehash = true;//false;
	bool userecent = false;
	unsigned int numprocs = NUM_PROCS;

	size_t degree_u = vertex_outdegrees[source]; // src_true_degree;
	degree_u = degree_u +1;
	size_t degree_v = vertex_outdegrees[target]; // dst_true_degree;
	degree_v = degree_v +1;
	size_t SUM = degree_u + degree_v;
	#ifdef TTTTT
	cout<<"edge_to_proc_hdrf:: SUM: "<<SUM<<", vertex_outdegrees["<<source<<"]: "<<vertex_outdegrees[source]<<", vertex_outdegrees["<<target<<"]: "<<vertex_outdegrees[target]<<endl;
	#endif 
	if(SUM==0){ cout<<"edge_to_proc_hdrf:: ERROR. SUM("<<SUM<<")==0. EXITING... "<<endl; exit(EXIT_FAILURE); SUM = 1; }
	double fu = degree_u;
	fu /= SUM;
	double fv = degree_v;
	fv /= SUM;
	#ifdef TTTTT
	cout<<"edge_to_proc_hdrf:: fu: "<<fu<<", fv: "<<fv<<endl;
	#endif 
	
	// Compute the score of each proc.
	procid_t best_proc = -1; 
	double maxscore = 0.0;
	double epsilon = 1.0; 
	std::vector<double> proc_score(numprocs); 
	unsigned int minedges=0xFFFFFFF; unsigned int maxedges = 0;
	for(unsigned int i = 0; i < NUM_PROCS; ++i){ if(proc_num_edges[i] < minedges){ minedges = proc_num_edges[i]; }}
	for(unsigned int i = 0; i < NUM_PROCS; ++i){ if(proc_num_edges[i] > maxedges){ maxedges = proc_num_edges[i]; }}
	
	#ifdef TTTTT
	cout<<"edge_to_proc_hdrf:: minedges: "<<minedges<<endl;
	cout<<"edge_to_proc_hdrf:: maxedges: "<<maxedges<<endl;
	for(unsigned int i = 0; i < NUM_PROCS; ++i){ cout<<"edge_to_proc_hdrf:: proc_num_edges["<<i<<"]: "<<proc_num_edges[i]<<endl; }
	#endif 
	
	for (size_t i = 0; i < numprocs; ++i) {
		double new_sd = 0;
		double new_td = 0;
		int sd = degrees[i][source] + (usehash && (source % numprocs == i));
		int td = degrees[i][target] + (usehash && (target % numprocs == i));
		if (sd > 0){ new_sd = 1+(1-fu); }
		if (td > 0){ new_td = 1+(1-fv); }
		double bal = (maxedges - proc_num_edges[i])/(epsilon + maxedges - minedges);

		proc_score[i] = bal + new_sd + new_td;
		
		#ifdef TTTTT
		cout<<"edge_to_proc_hdrf:: degrees["<<i<<"]["<<source<<"]: "<<degrees[i][source]<<", degrees["<<i<<"]["<<target<<"]: "<<degrees[i][target]<<", sd: "<<sd<<", td: "<<td<<", bal: "<<bal<<", new_sd: "<<new_sd<<", new_td: "<<new_td<<", proc_score["<<i<<"]: "<<proc_score[i]<<endl;				
		#endif
	}
	
	maxscore = *std::max_element(proc_score.begin(), proc_score.end());
	#ifdef TTTTT
	cout<<"edge_to_proc_hdrf:: maxscore: "<<maxscore<<endl;
	#endif 
	
	std::vector<procid_t> top_procs; 
	for (size_t i = 0; i < numprocs; ++i){
	  if (std::fabs(proc_score[i] - maxscore) < 1e-5){
		top_procs.push_back(i);
		#ifdef TTTTT
		cout<<"~~~~~~~~~~~~~~~ edge_to_proc_hdrf:: a top_proc: "<<i<<endl;
		#endif 
	  }
	}

	best_proc = top_procs[rand() % top_procs.size()];
	if(best_proc >= NUM_PROCS){ cout<<"edge_to_proc_hdrf:: ERROR. out-of-range. best_proc("<<best_proc<<") >= NUM_PROCS("<<NUM_PROCS<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }
	degrees[best_proc][source] = 1;
	degrees[best_proc][target] = 1;
	proc_num_edges[best_proc] += 1;
	
	return best_proc;
};







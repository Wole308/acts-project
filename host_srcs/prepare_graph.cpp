#include "prepare_graph.h"
using namespace std;

prepare_graph::prepare_graph(){}
prepare_graph::~prepare_graph(){}

void prepare_graph::create_graph(string graphpath, vector<edge3_type> &edgesbuffer_dup, vector<edge_t> &vptr_dup){
	cout<<">>> prepare_graph::create_graph: STARTED."<<endl;
	
	std::ofstream ofile;
	// ofile.open("/home/oj2zf/dataset/delicious.mtx", std::ios::app);
	// ofile.open("/home/oj2zf/dataset/delicious.mtx", std::ios::app);

	// std::ifstream file1_graph("/home/oj2zf/dataset/out.delicious");
	// ofile.open(graphpath, std::ofstream::out | std::ofstream::trunc);
	
	std::ifstream file1_graph("/home/oj2zf/dataset/out.dbpedia-link");
	ofile.open(graphpath, std::ofstream::out | std::ofstream::trunc);
	
	cout<<"prepare_graph::create_graph: creating graph @ "<<graphpath<<"..."<<endl;
	unsigned int srcv = 0;
	unsigned int dstv = 0;
	unsigned int A, B = 0;
	
	// ofile << 33777768 << " " << 33777768 << " " << 301183605 << std::endl;
	ofile << 18268992 << " " << 18268992 << " " << 172308906 << std::endl;
	if (file1_graph.is_open()) {
		std::string line;
		unsigned int linecount = 0;
		while (getline(file1_graph, line)) {
			if (line.find("%") == 0){ continue; }
			if (linecount % 10000000 == 0){ cout<<"create_graph::["<<linecount<<"] dstv: "<<dstv<<", srcv: "<<srcv<<endl; }
			
			sscanf(line.c_str(), "%i %i", &dstv, &srcv);
			
			// cout<<"prepare_graph:: dstv: "<<dstv<<", srcv: "<<srcv<<endl;
			ofile << dstv << " " << srcv << std::endl;
			
			linecount += 1;
			// if(linecount > 128){ break; }
		}
	}
	file1_graph.close();
	ofile.close();
	cout<<">>> prepare_graph::create_graph: FINISHED SUCCESSFULLY."<<endl;
	exit(EXIT_SUCCESS);
}

void prepare_graph::start(string graphpath, vector<edge3_type> &edgesbuffer_dup, vector<edge_t> &vptr_dup, bool graphisundirected){
	cout<<"prepare_graph:: preparing graph @ "<<graphpath<<"..."<<endl;
	unsigned int srcv = 0;
	unsigned int dstv = 0;
	unsigned int weight = 0;
	unsigned int ew = NAp;
	unsigned int num_vertices = 0;
	unsigned int num_vertices2 = 0;
	unsigned int num_edges = 0;
	unsigned int max_vertex = 0;
	vector<edge3_type> edgesbuffer;

	std::ifstream file1_graph(graphpath);
	if (file1_graph.is_open()) {
		std::string line;
		unsigned int linecount = 0;
		while (getline(file1_graph, line)) {
			if (line.find("%") == 0){ continue; }
			if (linecount == 0){ 
				sscanf(line.c_str(), "%i %i %i", &num_vertices, &num_vertices2, &num_edges);
				num_vertices += 1000; num_vertices2 += 1000; num_edges += 1000;
				cout<<"prepare_graph:: dataset header: num_vertices: "<<num_vertices<<", num_vertices2: "<<num_vertices2<<", num_edges: "<<num_edges<<endl;
				// exit(EXIT_SUCCESS);
				linecount++; continue; }	
			if (linecount % 100000000 == 0){ cout<<"prepare_graph: loading edges "<<linecount<<endl; } 
			
			sscanf(line.c_str(), "%i %i", &dstv, &srcv);
			if(srcv > max_vertex){ max_vertex = srcv; }
			if(dstv > max_vertex){ max_vertex = dstv; }
			
			edge3_type edge; edge.srcvid = srcv; edge.dstvid = dstv; edge.weight = rand() % 10 - 1;
			edgesbuffer.push_back(edge);
		
			linecount += 1;
			// if(linecount){}
		}
		cout<<"SUMMARY: linecount: "<<linecount<<", edgesbuffer.size(): "<<edgesbuffer.size()<<endl;
	}
	file1_graph.close();
	cout<<"prepare_graph:: Finished Stage 1: Buffering edges of "<<graphpath<<" (num_vertices: "<<num_vertices<<", num_vertices2: "<<num_vertices2<<", num_edges: "<<num_edges<<")"<<endl;
	// exit(EXIT_SUCCESS);
	
	// decide whether to use directed or graphisundirected (FIXME.)
	unsigned int mult_factor = 0;
	if(num_edges > 200000000){ cout<<"************* prepare_graph::[OVERRIDING...] Undirected graph too large to fit in memory. using directed graph instead"<<endl; graphisundirected = false; }
	if(graphisundirected==true){ mult_factor = 2; } else { mult_factor = 1; }
	
	cout<<"prepare_graph: assigning variables..."<<endl;
	num_vertices = max_vertex + 1;
	unsigned int padded_num_vertices = num_vertices + 1000;
	unsigned int padded_num_edges = num_edges + 1000;
	
	cout<<"prepare_graph: creating buffers..."<<endl;
	edgesbuffer_dup.resize((mult_factor * padded_num_edges));
	vptr_dup.resize((padded_num_vertices));
	
	cout<<"prepare_graph: creating buffers(2)..."<<endl;
	unsigned int * outdegree; outdegree = new unsigned int[padded_num_vertices];
	for(unsigned int i=0; i<padded_num_vertices; i++){ outdegree[i] = 0; vptr_dup[i] = 0; }
	for(unsigned int i=0; i<(mult_factor * padded_num_edges); i++){ edgesbuffer_dup[i].srcvid = 0; edgesbuffer_dup[i].dstvid = 0; }
	
	// populate inout degrees of all vertices 
	cout<<"prepare_graph: populating inout degrees of all vertices..."<<endl;
	for(unsigned int i=0; i<edgesbuffer.size(); i++){ // num_edges
		if(edgesbuffer[i].srcvid >= padded_num_vertices || edgesbuffer[i].dstvid >= padded_num_vertices){ cout<<"prepare_graph::start(21):: edgesbuffer[i].srcvid("<<edgesbuffer[i].srcvid<<") >= padded_num_vertices("<<padded_num_vertices<<") || edgesbuffer[i].dstvid("<<edgesbuffer[i].dstvid<<") >= padded_num_vertices("<<padded_num_vertices<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }	
		outdegree[edgesbuffer[i].srcvid] += 1;
		if(graphisundirected==true){ outdegree[edgesbuffer[i].dstvid] += 1; }
	}
	
	// creating vertex pointers...
	cout<<"prepare_graph: creating vertex pointers..."<<endl;
	vptr_dup[0] = 0;
	for(unsigned int k=1; k<num_vertices; k++){
		vptr_dup[k] = vptr_dup[k-1] + outdegree[k-1];
		if(k<4 || k>num_vertices-4){ cout<<"prepare_graph::start:: vptr_dup["<<k<<"]: "<<vptr_dup[k]<<endl; }
		if(vptr_dup[k] < vptr_dup[k-1]){ cout<<"creategraphs::writevertexptrstofile:ERROR: non-increasing vertex ptrs: vptr_dup["<<k<<"]: "<<vptr_dup[k]<<", vptr_dup["<<k-1<<"]: "<<vptr_dup[k-1]<<endl; exit(EXIT_FAILURE); }
	}
	cout<<"prepare_graph::start:: last: vptr_dup["<<num_vertices-1<<"]: "<<vptr_dup[num_vertices-1]<<endl;
	// exit(EXIT_SUCCESS);
	
	// checking new vertex pointers created...
	cout<<"prepare_graph: checking new vertex pointers created..."<<endl;
	cout<<"prepare_graph::start:: checking..."<<endl;
	for(unsigned int k=0; k<num_vertices-2; k++){
		if(vptr_dup[k] + outdegree[k] > vptr_dup[k+1]){ 
			cout<<"prepare_graph::start:: ERROR 2A. vptr_dup["<<k<<"]("<<vptr_dup[k]<<") + outdegree["<<k<<"]("<<outdegree[k]<<") >= vptr_dup["<<k+1<<"]("<<vptr_dup[k+1]<<")"<<endl; 
			exit(EXIT_FAILURE); 
		}
	}
	cout<<"prepare_graph::start:: finished checking."<<endl;
	
	unsigned int edgesbuffer_dup_size = 0;
	for(unsigned int i=0; i<num_vertices; i++){ outdegree[i] = 0; }
	for(unsigned int i=0; i<edgesbuffer.size(); i++){ // num_edges
		srcv = edgesbuffer[i].srcvid;
		dstv = edgesbuffer[i].dstvid;
		weight = edgesbuffer[i].weight;
		
		edge3_type edge1; edge1.srcvid = srcv; edge1.dstvid = dstv; edge1.weight = weight;
		if(vptr_dup[edge1.srcvid] + outdegree[edge1.srcvid] >= edgesbuffer_dup.size()){ cout<<"prepare_graph::start(1):: vptr_dup[edge1.srcvid]("<<vptr_dup[edge1.srcvid]<<") + outdegree[edge1.srcvid]("<<outdegree[edge1.srcvid]<<") >= edgesbuffer_dup.size()("<<edgesbuffer_dup.size()<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }							
		edgesbuffer_dup[vptr_dup[edge1.srcvid] + outdegree[edge1.srcvid]] = edge1;
		outdegree[edge1.srcvid] += 1;
		
		if(graphisundirected==true){ 
			edge3_type edge2; edge2.srcvid = dstv; edge2.dstvid = srcv; edge2.weight = weight;
			if(vptr_dup[edge2.srcvid] + outdegree[edge2.srcvid] >= edgesbuffer_dup.size()){ cout<<"prepare_graph::start(2):: vptr_dup[edge2.srcvid]("<<vptr_dup[edge2.srcvid]<<") + outdegree[edge2.srcvid]("<<outdegree[edge2.srcvid]<<") >= edgesbuffer_dup.size()("<<edgesbuffer_dup.size()<<"). EXITING... "<<endl; exit(EXIT_FAILURE); }							
			edgesbuffer_dup[vptr_dup[edge2.srcvid] + outdegree[edge2.srcvid]] = edge2;
			outdegree[edge2.srcvid] += 1;
		}
		
		edgesbuffer_dup_size += mult_factor;
	}
	cout<<"prepare_graph:: Finished Stage 3: Buffering duplicate edges of "<<graphpath<<" (num_vertices: "<<num_vertices<<", num_vertices2: "<<num_vertices2<<", num_edges: "<<num_edges<<", edgesbuffer_dup_size: "<<edgesbuffer_dup_size<<", edgesbuffer_dup.size(): "<<edgesbuffer_dup.size()<<")"<<endl;
	
	cout<<"prepare_graph:: checking edge data for errors... "<<endl;
	unsigned int numerrors = 0;
	for(unsigned int t=1; t<edgesbuffer_dup_size; t++){
		if(edgesbuffer_dup[t].srcvid < edgesbuffer_dup[t-1].srcvid){
			cout<<"prepare_graph::ERROR: edgesbuffer_dup["<<t<<"].srcvid("<<edgesbuffer_dup[t].srcvid<<") < edgesbuffer_dup["<<t-1<<"].srcvid("<<edgesbuffer_dup[t-1].srcvid<<")"<<endl;
			for(unsigned int k=0; k<16; k++){ cout<<"... prepare_graph::loadedges:: edgesbuffer_dup["<<k+t-8<<"].srcvid: "<<edgesbuffer_dup[k+t-8].srcvid<<", edgesbuffer_dup["<<k+t-8<<"].dstvid: "<<edgesbuffer_dup[k+t-8].dstvid<<endl; }
			numerrors += 1;
			if(numerrors > 1000){ exit(EXIT_FAILURE); }
			exit(EXIT_FAILURE);
		}
	}
	cout<<"Finished checking edge data for errors: numerrors: "<<numerrors<<endl;
	// exit(EXIT_SUCCESS);

	unsigned int zerocount = 0;
	unsigned int maxsz = edgesbuffer_dup_size;
	for(unsigned int i=0; i<maxsz; i++){
		edge3_type edge = edgesbuffer_dup[i];
		if(edge.srcvid==0 && edge.dstvid==0){
			if(zerocount>10000){ cout<<"prepare_graph::writeedgestofile:: ERROR: too many zeros ("<<zerocount<<"). check... EXITING... "<<endl; exit(EXIT_FAILURE); }
			else { zerocount += 1; } 
		}
		if(i > maxsz-1){ cout<<"### prepare_graph::writeedgestofile:: maxsz: "<<maxsz<<", edge.dstvid: "<<edge.dstvid<<", edge.srcvid: "<<edge.srcvid<<endl; }
	}
	cout<<"prepare_graph:: Finished preparing "<<graphpath<<endl;
	
	edgesbuffer.clear();
	delete [] outdegree;
	// exit(EXIT_SUCCESS);
	return;
}





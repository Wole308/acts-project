#include "act_pack.h"
using namespace std;

act_pack::act_pack(universalparams_t _universalparams){
	utilityobj = new utility(_universalparams);
	universalparams = _universalparams;
}
act_pack::~act_pack(){} 

unsigned int get_local2(unsigned int vid, universalparams_t universalparams){
	unsigned int W = (FOLD_SIZE * EDGE_PACK_SIZE) * universalparams.GLOBAL_NUM_PEs_;
	unsigned int y = vid / W; 
	unsigned int x = vid % (FOLD_SIZE * EDGE_PACK_SIZE);
	unsigned int lvid = (y * (FOLD_SIZE * EDGE_PACK_SIZE)) + x;
	return lvid;
}
unsigned int get_local4(unsigned int vid, universalparams_t universalparams){
	return (vid / universalparams.GLOBAL_NUM_PEs_) + (vid % 234);
}
unsigned int get_local7(unsigned int vid, universalparams_t universalparams){
	return (vid / universalparams.GLOBAL_NUM_PEs_) + (vid % 234);
}
unsigned int get_local(unsigned int s, unsigned int vid, universalparams_t universalparams){
	if(s==0){ return get_local2(vid, universalparams); }
	else if (s==1){ return get_local4(vid, universalparams); }
	else if (s==2){ return get_local7(vid, universalparams); } // FIXME.
	else { cout<<"act_pack: ERROR 2323: s("<<s<<") option is invalid. EXITING..."<<endl; exit(EXIT_FAILURE); }
}

unsigned int get_H1(unsigned int vid, universalparams_t universalparams){	
	return vid % universalparams.GLOBAL_NUM_PEs_;	
}
unsigned int get_H2(unsigned int vid, universalparams_t universalparams){	
	return (vid % (FOLD_SIZE * EDGE_PACK_SIZE * universalparams.GLOBAL_NUM_PEs_)) / (FOLD_SIZE * EDGE_PACK_SIZE);	
}
unsigned int get_H4(unsigned int vid, universalparams_t universalparams){
	return (vid / 234) % universalparams.GLOBAL_NUM_PEs_; // FIXME
}
unsigned int get_H7(unsigned int vid, unsigned int ping_pong, universalparams_t universalparams){
	return ping_pong; // FIXME
}

void getXYLayoutV(unsigned int s, unsigned int depths[EDGE_PACK_SIZE]){
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
		depths[v] = (EDGE_PACK_SIZE + v - s) % EDGE_PACK_SIZE; 
	}
}
edge3_vec_dt rearrangeLayoutV(unsigned int s, edge3_vec_dt edge_vec){
	edge3_vec_dt edge_vec3;
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
		edge_vec3.data[(EDGE_PACK_SIZE + v - s) % EDGE_PACK_SIZE] = edge_vec.data[v]; 
	}
	return edge_vec3;
}
edge3_vec_dt rearrangeLayoutVB(unsigned int s, edge3_vec_dt edge_vec){
	edge3_vec_dt edge_vec3;
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
		edge_vec3.data[(EDGE_PACK_SIZE + v + s) % EDGE_PACK_SIZE] = edge_vec.data[v]; 
	}
	return edge_vec3;
}

void act_pack::load_edges(vector<edge_t> &vertexptrbuffer, vector<edge3_type> &edgedatabuffer, vector<edge3_type> (&final_edge_updates)[MAX_GLOBAL_NUM_PEs][MAX_NUM_UPARTITIONS][MAX_NUM_LLPSETS]){			
	cout<<"=== act_pack: EDGE_PACK_SIZE: "<<EDGE_PACK_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_CHANNEL_PACK_SIZE: "<<HBM_CHANNEL_PACK_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_AXI_PACK_SIZE: "<<HBM_AXI_PACK_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_AXI_PACK_BITSIZE: "<<HBM_AXI_PACK_BITSIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_CHANNEL_BYTESIZE: "<<HBM_CHANNEL_BYTESIZE<<" ==="<<endl;
	cout<<"=== act_pack: universalparams._MAX_APPLYPARTITION_VECSIZE: "<<universalparams._MAX_APPLYPARTITION_VECSIZE<<" ==="<<endl;
	cout<<"=== act_pack: _MAX_APPLYPARTITION_SIZE: "<<universalparams._MAX_APPLYPARTITION_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: universalparams._MAX_UPARTITION_VECSIZE: "<<universalparams._MAX_UPARTITION_VECSIZE<<" ==="<<endl;
	cout<<"=== act_pack: universalparams._MAX_UPARTITION_SIZE: "<<universalparams._MAX_UPARTITION_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_CHANNEL_BYTESIZE: "<<HBM_CHANNEL_BYTESIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_CHANNEL_SIZE: "<<HBM_CHANNEL_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: MAX_NUM_LLPSETS: "<<MAX_NUM_LLPSETS<<" ==="<<endl;
	cout<<"=== act_pack: UPDATES_BUFFER_PACK_SIZE: "<<UPDATES_BUFFER_PACK_SIZE<<" ==="<<endl;
	
	bool debug = false; // true;// false;

	vector<edge3_type> edges_in_channel[universalparams.GLOBAL_NUM_PEs_];
	vector<edge3_type> edgesin_srcvp[MAX_NUM_UPARTITIONS];
	
	bool satisfied=false;
	unsigned int tryy=0;
	for(tryy=2; tryy<3; tryy++){ // FIXME.
	// for(tryy=0; tryy<1; tryy++){ // FIXME. /////////////////////////
		cout<<"+++ act_pack:: tryy: "<<tryy<<". "<<endl; 
		unsigned int ping_pong = 0;
		for(unsigned int vid=0; vid<universalparams.NUM_VERTICES-1; vid++){
			edge_t vptr_begin = vertexptrbuffer[vid];
			edge_t vptr_end = vertexptrbuffer[vid+1];
			edge_t edges_size = vptr_end - vptr_begin;
			// if(vptr_end < vptr_begin){ continue; }
			if(vptr_end < vptr_begin){ edges_size = 0; }
			
			for(unsigned int i=0; i<edges_size; i++){
				edge3_type this_edge = edgedatabuffer[vptr_begin + i];
				edge3_type edge; edge.srcvid = this_edge.srcvid; edge.dstvid = this_edge.dstvid; 
				if(edge.srcvid >= universalparams.NUM_VERTICES || edge.dstvid >= universalparams.NUM_VERTICES){ continue; } 
				
				unsigned int H=0;
				if(tryy==0){ H = get_H2(edge.dstvid, universalparams); } 
				else if(tryy==1){ H = get_H4(edge.dstvid, universalparams); }
				else { H = get_H7(edge.dstvid, ping_pong, universalparams); ping_pong+=1; if(ping_pong >= universalparams.GLOBAL_NUM_PEs_){ ping_pong = 0; } }	// FIXME.				
				utilityobj->checkoutofbounds("loadedges::ERROR 223::", H, universalparams.GLOBAL_NUM_PEs_, edge.srcvid, edge.dstvid, universalparams._MAX_UPARTITION_SIZE);
				
				edges_in_channel[H].push_back(edge);
			}
		}
		#ifdef _DEBUGMODE_HOSTPRINTS4
		for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ cout<<"act-pack edges:: PE: "<<i<<": edges_in_channel["<<i<<"].size(): "<<edges_in_channel[i].size()<<""<<endl; }
		cout<<"----- ideal act-pack edges:: "<<(universalparams.NUM_EDGES / universalparams.GLOBAL_NUM_PEs_)<<" ----- "<<endl;
		#endif 
		unsigned int max=0; for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ if(max < edges_in_channel[i].size()){ max = edges_in_channel[i].size(); } }
		unsigned int min=0xFFFFFFFE; for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ if(min > edges_in_channel[i].size()){ min = edges_in_channel[i].size(); } }
		if(((float)((float)(max - min) / (float)max) * 100) < 30){ cout<<"act_pack: SUCCESS 447. (min("<<min<<") / max("<<max<<")) * 100 < 30..."<<endl; satisfied = true; break; }
		else { cout<<"act_pack: UNSATISFACTORY. 446. (min("<<min<<") / max("<<max<<")) * 100 > 40. TRYING AGAIN..."<<endl; }
		for(unsigned int h=0; h<universalparams.GLOBAL_NUM_PEs_; h++){ edges_in_channel[h].clear(); }
	}
	if(satisfied == false){ cout<<"act_pack: ERROR 4452. satisfied == false. EXITING..."<<endl; exit(EXIT_FAILURE); } 
	// exit(EXIT_SUCCESS);////////////////////////
	
	#ifdef _DEBUGMODE_HOSTPRINTS4
	cout<<"load_edges: loading edges [STAGE 2]: preparing edges..."<<endl;
	#endif 
	for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ // universalparams.GLOBAL_NUM_PEs_
		if(utilityobj->channel_is_active(i) == false){ continue; }
		#ifdef _DEBUGMODE_HOSTPRINTS3
		cout<<"act_pack:: [PE: "<<i<<"]"<<endl;
		#endif 
		for(unsigned int p_u=0; p_u<universalparams.NUM_UPARTITIONS; p_u++){ edgesin_srcvp[p_u].clear(); } // clear 
		
		// within a HBM channel, partition into v-partitions 
		if(debug){ cout<<"STAGE 1: within a HBM channel, partition into v-partitions "<<endl; }
		for(unsigned int t=0; t<edges_in_channel[i].size(); t++){
			edge3_type edge = edges_in_channel[i][t];
	
			unsigned int p_u = (edge.srcvid / universalparams._MAX_UPARTITION_SIZE);
			if(p_u >= universalparams.NUM_UPARTITIONS){ p_u = universalparams.NUM_UPARTITIONS-1; } 
	
			#ifdef _DEBUGMODE_HOSTCHECKS3
			utilityobj->checkoutofbounds("act_pack::ERROR 22c::", p_u, universalparams.NUM_UPARTITIONS, edge.srcvid, edge.srcvid, universalparams._MAX_UPARTITION_SIZE);
			#endif 
			edgesin_srcvp[p_u].push_back(edge);
		}
		
		for(unsigned int p_u=0; p_u<universalparams.NUM_UPARTITIONS; p_u++){
			if(debug){ cout<<"load_edges: STAGE 2: [i: "<<i<<", v-partition "<<p_u<<"] => partition into last-level-partitions (LLPsets)"<<endl; } 
			for(unsigned int t=0; t<edgesin_srcvp[p_u].size(); t++){
				edge3_type edge = edgesin_srcvp[p_u][t];
				unsigned int local_dstvid = get_local(tryy, edge.dstvid, universalparams); // FIXME.
				#ifdef _DEBUGMODE_HOSTCHECKS3
				utilityobj->checkoutofbounds("act_pack::ERROR 2234c::", local_dstvid, universalparams.NUM_APPLYPARTITIONS * universalparams._MAX_APPLYPARTITION_SIZE, edge.srcvid, edge.srcvid, local_dstvid);
				#endif 
				unsigned int llp_set = local_dstvid / universalparams._MAX_APPLYPARTITION_SIZE;
				llp_set = 0; // FIXME.
				edge.dstvid = local_dstvid; 
				final_edge_updates[i][p_u][llp_set].push_back(edge);
			}
		} // iteration end: p_u
		
		#ifdef _DEBUGMODE_KERNELPRINTS//4
		for(unsigned int p_u=0; p_u<universalparams.NUM_UPARTITIONS; p_u++){
			for(unsigned int llp_set=0; llp_set<1; llp_set++){ 
				if(i==0 && final_edge_updates[i][p_u][llp_set].size() > 0){ cout<<">>> final_edge_updates["<<i<<"]["<<p_u<<"]["<<llp_set<<"].size(): "<<final_edge_updates[i][p_u][llp_set].size()<<""<<endl; }
			}
		}
		#endif 
	} // iteration end: i(universalparams.GLOBAL_NUM_PEs_) end here
	// exit(EXIT_SUCCESS);
	return;
}

void act_pack::load_edges_new(vector<edge_t> &vertexptrbuffer, vector<edge3_type> &edgedatabuffer, vector<edge3_type> (&final_edge_updates)[MAX_GLOBAL_NUM_PEs][MAX_NUM_UPARTITIONS][MAX_NUM_LLPSETS], vprop_dest_t * local_vertex_properties[64]){			
	cout<<"=== act_pack: EDGE_PACK_SIZE: "<<EDGE_PACK_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_CHANNEL_PACK_SIZE: "<<HBM_CHANNEL_PACK_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_AXI_PACK_SIZE: "<<HBM_AXI_PACK_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_AXI_PACK_BITSIZE: "<<HBM_AXI_PACK_BITSIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_CHANNEL_BYTESIZE: "<<HBM_CHANNEL_BYTESIZE<<" ==="<<endl;
	cout<<"=== act_pack: universalparams._MAX_APPLYPARTITION_VECSIZE: "<<universalparams._MAX_APPLYPARTITION_VECSIZE<<" ==="<<endl;
	cout<<"=== act_pack: _MAX_APPLYPARTITION_SIZE: "<<universalparams._MAX_APPLYPARTITION_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: universalparams._MAX_UPARTITION_VECSIZE: "<<universalparams._MAX_UPARTITION_VECSIZE<<" ==="<<endl;
	cout<<"=== act_pack: _MAX_UPARTITION_SIZE: "<<universalparams._MAX_UPARTITION_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_CHANNEL_BYTESIZE: "<<HBM_CHANNEL_BYTESIZE<<" ==="<<endl;
	cout<<"=== act_pack: HBM_CHANNEL_SIZE: "<<HBM_CHANNEL_SIZE<<" ==="<<endl;
	cout<<"=== act_pack: MAX_NUM_LLPSETS: "<<MAX_NUM_LLPSETS<<" ==="<<endl;
	cout<<"=== act_pack: UPDATES_BUFFER_PACK_SIZE: "<<UPDATES_BUFFER_PACK_SIZE<<" ==="<<endl;
	
	translator_t * vertex_translator[64]; 
	for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ vertex_translator[i] = new translator_t[universalparams.NUM_VERTICES]; } 
	for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ 
		for(unsigned int t=0; t<universalparams.NUM_VERTICES; t++){
			vertex_translator[i][t].local_id = INVALIDDATA; 
			vertex_translator[i][t].global_id = INVALIDDATA; 
		}
	}
	
	// vprop_dest_t * local_vertex_properties[64]; 		
	// for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ local_vertex_properties[i] = new vprop_dest_t[universalparams.NUM_VERTICES / universalparams.GLOBAL_NUM_PEs_]; } 
	// for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ 
		// for(unsigned int t=0; t<universalparams.NUM_VERTICES / universalparams.GLOBAL_NUM_PEs_; t++){
			// local_vertex_properties[i][t].prop = 0; 
			// local_vertex_properties[i][t].gvid = 0; 
		// }
	// }	
	
	unsigned int next_index[64]; for(unsigned int t=0; t<64; t++){ next_index[t] = 0; }
	vector<edge3_type> distr_edges[100];
	vector<unsigned int> distr_vertices[100];
	
	bool debug = false; // true;// false;

	vector<edge3_type> edges_in_channel[universalparams.GLOBAL_NUM_PEs_];
	vector<edge3_type> edgesin_srcvp[MAX_NUM_UPARTITIONS];
	
	bool satisfied=false;
	unsigned int tryy=2;
	unsigned int max_edges_size = 0, max_degree_vertex = 0;
	
	cout<<"+++ act_pack:: tryy: "<<tryy<<". "<<endl; 
	unsigned int ping_pong = 0;
	
	for(unsigned int vid=0; vid<universalparams.NUM_VERTICES-1; vid++){
		edge_t vptr_begin = vertexptrbuffer[vid];
		edge_t vptr_end = vertexptrbuffer[vid+1];
		edge_t edges_size = vptr_end - vptr_begin;
		if(vptr_end < vptr_begin){ edges_size = 0; }
		if(edges_size > max_edges_size){ max_degree_vertex = vid; max_edges_size = edges_size; }
	}
	
	max_edges_size = universalparams.NUM_EDGES / universalparams.NUM_VERTICES; //////////////
	// max_edges_size = max_edges_size / 2; //////////////
	cout<<"----- act-pack:: max_degree_vertex: "<<max_degree_vertex<<", max_edges_size:: "<<max_edges_size<<" ----- "<<endl;
	
	for(unsigned int n=0; n<universalparams.NUM_VERTICES / universalparams._MAX_UPARTITION_SIZE; n++){		
		for(unsigned int lvid=0; lvid<universalparams._MAX_UPARTITION_SIZE; lvid++){
			unsigned int gvid = (n * universalparams._MAX_UPARTITION_SIZE) + lvid;			
			edge_t vptr_begin = vertexptrbuffer[gvid];
			edge_t vptr_end = vertexptrbuffer[gvid+1];
			edge_t edges_size = vptr_end - vptr_begin;
			if(vptr_end < vptr_begin){ edges_size = 0; }
			unsigned int index = ((edges_size * 100) / max_edges_size); if(index >= 100){ index = index % 100; } 
			utilityobj->checkoutofbounds("loadedges::ERROR 1143::", index, 101, edges_size, max_edges_size, universalparams._MAX_UPARTITION_SIZE);
			distr_vertices[index].push_back(gvid);
			for(unsigned int i=0; i<edges_size; i++){		
				edge3_type this_edge = edgedatabuffer[vptr_begin + i];
				distr_edges[index].push_back(this_edge);
			}			
		}
		
		#ifdef _DEBUGMODE_HOSTPRINTS//4
		if(n==0){ for(unsigned int i=0; i<100; i++){ cout<<"act_pack:: distr_vertices["<<i<<"].size(): "<<distr_vertices[i].size()<<endl; } }
		if(n==0){ for(unsigned int i=0; i<100; i++){ cout<<"act_pack:: distr_edges["<<i<<"].size(): "<<distr_edges[i].size()<<endl; } }
		#endif 
		
		for(unsigned int u=0; u<100; u++){
			for(unsigned int t=0; t<distr_vertices[u].size(); t++){
				unsigned int gvid = distr_vertices[u][t];
			
				edge_t vptr_begin = vertexptrbuffer[gvid];
				edge_t vptr_end = vertexptrbuffer[gvid+1];
				edge_t edges_size = vptr_end - vptr_begin;
				if(vptr_end < vptr_begin){ edges_size = 0; }

				for(unsigned int i=0; i<edges_size; i++){
					edge3_type this_edge = edgedatabuffer[vptr_begin + i];
					edge3_type edge; edge.srcvid = this_edge.srcvid; edge.dstvid = this_edge.dstvid; 
					if(edge.srcvid >= universalparams.NUM_VERTICES || edge.dstvid >= universalparams.NUM_VERTICES){ continue; } 
					
					unsigned int H = get_H1(edge.dstvid, universalparams);
					// unsigned int H = get_H2(edge.dstvid, universalparams);
					// unsigned int H = get_H7(edge.dstvid, ping_pong, universalparams); ping_pong+=1; if(ping_pong >= universalparams.GLOBAL_NUM_PEs_){ ping_pong = 0; }
					utilityobj->checkoutofbounds("loadedges::ERROR 223::", H, universalparams.GLOBAL_NUM_PEs_, edge.srcvid, edge.dstvid, universalparams._MAX_UPARTITION_SIZE);
					
					if(vertex_translator[H][edge.dstvid].local_id == INVALIDDATA){ 
						vertex_translator[H][edge.dstvid].local_id = next_index[H]; 
						local_vertex_properties[H][next_index[H]].gvid = edge.dstvid;
						next_index[H] += 1; 
					}
					// next_index[H] += 1;
					
					edges_in_channel[H].push_back(edge);
				}
			}
		}
		
		for(unsigned int i=0; i<100; i++){ distr_vertices[i].clear(); }
		for(unsigned int i=0; i<100; i++){ distr_edges[i].clear(); }
		// break; // FIXME.
	}
	
	#ifdef _DEBUGMODE_HOSTPRINTS4
	for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ cout<<"act_pack:: next_index["<<i<<"]: "<<next_index[i]<<endl; }
	for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ cout<<"act_pack:: edges_in_channel["<<i<<"].size(): "<<edges_in_channel[i].size()<<endl; }
	#endif 
	// exit(EXIT_SUCCESS);
	
	#ifdef _DEBUGMODE_HOSTPRINTS4
	for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ cout<<"act-pack edges:: PE: "<<i<<": edges_in_channel["<<i<<"].size(): "<<edges_in_channel[i].size()<<""<<endl; }
	cout<<"----- ideal act-pack edges:: "<<(universalparams.NUM_EDGES / universalparams.GLOBAL_NUM_PEs_)<<" ----- "<<endl;
	#endif 
	unsigned int max=0; for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ if(max < edges_in_channel[i].size()){ max = edges_in_channel[i].size(); } }
	unsigned int min=0xFFFFFFFE; for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ if(min > edges_in_channel[i].size()){ min = edges_in_channel[i].size(); } }
	if(((float)((float)(max - min) / (float)max) * 100) < 30){ cout<<"act_pack: SUCCESS 447. (min("<<min<<") / max("<<max<<")) * 100 < 30..."<<endl; satisfied = true; }
	else { cout<<"act_pack: UNSATISFACTORY. 446. (min("<<min<<") / max("<<max<<")) * 100 > 40. TRYING AGAIN..."<<endl; }
	if(satisfied == false){ cout<<"act_pack: ERROR 4452. satisfied == false. EXITING..."<<endl; exit(EXIT_FAILURE); } 
	// exit(EXIT_SUCCESS);
	
	#ifdef _DEBUGMODE_HOSTPRINTS4
	cout<<"load_edges: loading edges [STAGE 2]: preparing edges..."<<endl;
	#endif 
	for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ 
		if(utilityobj->channel_is_active(i) == false){ continue; }
		#ifdef _DEBUGMODE_HOSTPRINTS3
		cout<<"act_pack:: [PE: "<<i<<"]"<<endl;
		#endif 
		for(unsigned int p_u=0; p_u<universalparams.NUM_UPARTITIONS; p_u++){ edgesin_srcvp[p_u].clear(); } // clear 
		
		// within a HBM channel, partition into v-partitions 
		if(debug){ cout<<"STAGE 1: within a HBM channel, partition into v-partitions "<<endl; }
		for(unsigned int t=0; t<edges_in_channel[i].size(); t++){
			edge3_type edge = edges_in_channel[i][t];
	
			unsigned int p_u = (edge.srcvid / universalparams._MAX_UPARTITION_SIZE);
			if(p_u >= universalparams.NUM_UPARTITIONS){ p_u = universalparams.NUM_UPARTITIONS-1; } 
	
			#ifdef _DEBUGMODE_HOSTCHECKS3
			utilityobj->checkoutofbounds("act_pack::ERROR 22c::", p_u, universalparams.NUM_UPARTITIONS, edge.srcvid, edge.srcvid, universalparams._MAX_UPARTITION_SIZE);
			#endif 
			edgesin_srcvp[p_u].push_back(edge);
		}
		
		for(unsigned int p_u=0; p_u<universalparams.NUM_UPARTITIONS; p_u++){
			if(debug){ cout<<"load_edges: STAGE 2: [i: "<<i<<", v-partition "<<p_u<<"] => partition into last-level-partitions (LLPsets)"<<endl; } 
			for(unsigned int t=0; t<edgesin_srcvp[p_u].size(); t++){
				edge3_type edge = edgesin_srcvp[p_u][t];
				
				// unsigned int local_dstvid = get_local(tryy, edge.dstvid, universalparams); // FIXME.
				unsigned int local_dstvid = vertex_translator[i][edge.dstvid].local_id;
				
				#ifdef _DEBUGMODE_HOSTCHECKS3
				utilityobj->checkoutofbounds("act_pack::ERROR 2234c::", local_dstvid, universalparams.NUM_APPLYPARTITIONS * universalparams._MAX_APPLYPARTITION_SIZE, edge.srcvid, edge.srcvid, local_dstvid);
				#endif 
				unsigned int llp_set = 0; // local_dstvid / universalparams._MAX_APPLYPARTITION_SIZE; // FIXME.
				edge.dstvid = local_dstvid; 
				final_edge_updates[i][p_u][llp_set].push_back(edge);
			}
		} // iteration end: p_u
		
		#ifdef _DEBUGMODE_KERNELPRINTS//4
		for(unsigned int p_u=0; p_u<universalparams.NUM_UPARTITIONS; p_u++){
			for(unsigned int llp_set=0; llp_set<1; llp_set++){ 
				if(i==0 && final_edge_updates[i][p_u][llp_set].size() > 0){ cout<<">>> final_edge_updates["<<i<<"]["<<p_u<<"]["<<llp_set<<"].size(): "<<final_edge_updates[i][p_u][llp_set].size()<<""<<endl; }
			}
		}
		#endif 
	} // iteration end: i(universalparams.GLOBAL_NUM_PEs_) end here

	// detete arrays
	for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ delete [] vertex_translator[i]; } 
	for(unsigned int i=0; i<100; i++){ distr_edges[i].clear(); distr_vertices[i].clear(); }
	
	#ifdef _DEBUGMODE_HOSTPRINTS//4
	for(unsigned int i=0; i<universalparams.GLOBAL_NUM_PEs_; i++){ 
		for(unsigned int t=0; t<4; t++){ // universalparams.NUM_VERTICES / universalparams.GLOBAL_NUM_PEs_
			cout<<"act-pack:: local_vertex_properties["<<i<<"]["<<t<<"].prop: "<<local_vertex_properties[i][t].prop<<endl; 
			cout<<"act-pack:: local_vertex_properties["<<i<<"]["<<t<<"].gvid: "<<local_vertex_properties[i][t].gvid<<endl; 
		}
	}
	#endif 
	
	exit(EXIT_SUCCESS);
	return;
}




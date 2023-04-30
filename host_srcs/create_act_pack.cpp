#include "create_act_pack.h"
using namespace std;

// #define _DEBUGMODE_KERNELPRINTS4_CREATEACTPACT

typedef struct {
	keyy_t srcvid;	
	keyy_t dstvid; 
} edge_dtype;
typedef struct {
	edge_dtype data[EDGE_PACK_SIZE];
} edge_vec_dtype;

create_act_pack::create_act_pack(universalparams_t _universalparams){
	utilityobj = new utility(_universalparams);
	universalparams = _universalparams;
}
create_act_pack::~create_act_pack(){} 

void checkoutofbounds_(string message, unsigned int data, unsigned int upper_bound, unsigned int msgdata1, unsigned int msgdata2, unsigned int msgdata3){
	#ifdef _DEBUGMODE_CHECKS3
	if(data >= upper_bound){ std::cout<<"utility::checkoutofbounds_: ERROR. out of bounds. message: "<<message<<", data: "<<data<<", upper_bound: "<<upper_bound<<", msgdata1: "<<msgdata1<<", msgdata2: "<<msgdata2<<", msgdata3: "<<msgdata3<<std::endl; exit(EXIT_FAILURE); }			
	#endif 
}

unsigned int owner_fpga2(unsigned int dstvid){
	// return dstvid % universalparams.NUM_FPGAS_;
	return 0; // FIXME.
}

unsigned int owner_partition(unsigned int c, unsigned int data){
	return data % EDGE_PACK_SIZE;
}

void rearrange_layout(unsigned int s, edge_dtype in[EDGE_PACK_SIZE], edge_dtype out[EDGE_PACK_SIZE]){
	#ifndef FPGA_IMPL
	checkoutofbounds_("create_act_pack::rearrange_layout::ERROR 2125::", s, EDGE_PACK_SIZE, NAp, NAp, NAp);
	#endif 
	if(s==0){ 
		out[0] = in[0]; 
		out[1] = in[1]; 
		out[2] = in[2]; 
		out[3] = in[3]; 
		out[4] = in[4]; 
		out[5] = in[5]; 
		out[6] = in[6]; 
		out[7] = in[7]; 
		out[8] = in[8]; 
		out[9] = in[9]; 
		out[10] = in[10]; 
		out[11] = in[11]; 
		out[12] = in[12]; 
		out[13] = in[13]; 
		out[14] = in[14]; 
		out[15] = in[15]; 
	}
	else if(s==1){ 
		out[1] = in[0]; 
		out[2] = in[1]; 
		out[3] = in[2]; 
		out[4] = in[3]; 
		out[5] = in[4]; 
		out[6] = in[5]; 
		out[7] = in[6]; 
		out[8] = in[7]; 
		out[9] = in[8]; 
		out[10] = in[9]; 
		out[11] = in[10]; 
		out[12] = in[11]; 
		out[13] = in[12]; 
		out[14] = in[13]; 
		out[15] = in[14]; 
		out[0] = in[15]; 
	}
	else if(s==2){ 
		out[2] = in[0]; 
		out[3] = in[1]; 
		out[4] = in[2]; 
		out[5] = in[3]; 
		out[6] = in[4]; 
		out[7] = in[5]; 
		out[8] = in[6]; 
		out[9] = in[7]; 
		out[10] = in[8]; 
		out[11] = in[9]; 
		out[12] = in[10]; 
		out[13] = in[11]; 
		out[14] = in[12]; 
		out[15] = in[13]; 
		out[0] = in[14]; 
		out[1] = in[15]; 
	}
	else if(s==3){ 
		out[3] = in[0]; 
		out[4] = in[1]; 
		out[5] = in[2]; 
		out[6] = in[3]; 
		out[7] = in[4]; 
		out[8] = in[5]; 
		out[9] = in[6]; 
		out[10] = in[7]; 
		out[11] = in[8]; 
		out[12] = in[9]; 
		out[13] = in[10]; 
		out[14] = in[11]; 
		out[15] = in[12]; 
		out[0] = in[13]; 
		out[1] = in[14]; 
		out[2] = in[15]; 
	}
	else if(s==4){ 
		out[4] = in[0]; 
		out[5] = in[1]; 
		out[6] = in[2]; 
		out[7] = in[3]; 
		out[8] = in[4]; 
		out[9] = in[5]; 
		out[10] = in[6]; 
		out[11] = in[7]; 
		out[12] = in[8]; 
		out[13] = in[9]; 
		out[14] = in[10]; 
		out[15] = in[11]; 
		out[0] = in[12]; 
		out[1] = in[13]; 
		out[2] = in[14]; 
		out[3] = in[15]; 
	}
	else if(s==5){ 
		out[5] = in[0]; 
		out[6] = in[1]; 
		out[7] = in[2]; 
		out[8] = in[3]; 
		out[9] = in[4]; 
		out[10] = in[5]; 
		out[11] = in[6]; 
		out[12] = in[7]; 
		out[13] = in[8]; 
		out[14] = in[9]; 
		out[15] = in[10]; 
		out[0] = in[11]; 
		out[1] = in[12]; 
		out[2] = in[13]; 
		out[3] = in[14]; 
		out[4] = in[15]; 
	}
	else if(s==6){ 
		out[6] = in[0]; 
		out[7] = in[1]; 
		out[8] = in[2]; 
		out[9] = in[3]; 
		out[10] = in[4]; 
		out[11] = in[5]; 
		out[12] = in[6]; 
		out[13] = in[7]; 
		out[14] = in[8]; 
		out[15] = in[9]; 
		out[0] = in[10]; 
		out[1] = in[11]; 
		out[2] = in[12]; 
		out[3] = in[13]; 
		out[4] = in[14]; 
		out[5] = in[15]; 
	}
	else if(s==7){ 
		out[7] = in[0]; 
		out[8] = in[1]; 
		out[9] = in[2]; 
		out[10] = in[3]; 
		out[11] = in[4]; 
		out[12] = in[5]; 
		out[13] = in[6]; 
		out[14] = in[7]; 
		out[15] = in[8]; 
		out[0] = in[9]; 
		out[1] = in[10]; 
		out[2] = in[11]; 
		out[3] = in[12]; 
		out[4] = in[13]; 
		out[5] = in[14]; 
		out[6] = in[15]; 
	}
	else if(s==8){ 
		out[8] = in[0]; 
		out[9] = in[1]; 
		out[10] = in[2]; 
		out[11] = in[3]; 
		out[12] = in[4]; 
		out[13] = in[5]; 
		out[14] = in[6]; 
		out[15] = in[7]; 
		out[0] = in[8]; 
		out[1] = in[9]; 
		out[2] = in[10]; 
		out[3] = in[11]; 
		out[4] = in[12]; 
		out[5] = in[13]; 
		out[6] = in[14]; 
		out[7] = in[15]; 
	}
	else if(s==9){ 
		out[9] = in[0]; 
		out[10] = in[1]; 
		out[11] = in[2]; 
		out[12] = in[3]; 
		out[13] = in[4]; 
		out[14] = in[5]; 
		out[15] = in[6]; 
		out[0] = in[7]; 
		out[1] = in[8]; 
		out[2] = in[9]; 
		out[3] = in[10]; 
		out[4] = in[11]; 
		out[5] = in[12]; 
		out[6] = in[13]; 
		out[7] = in[14]; 
		out[8] = in[15]; 
	}
	else if(s==10){ 
		out[10] = in[0]; 
		out[11] = in[1]; 
		out[12] = in[2]; 
		out[13] = in[3]; 
		out[14] = in[4]; 
		out[15] = in[5]; 
		out[0] = in[6]; 
		out[1] = in[7]; 
		out[2] = in[8]; 
		out[3] = in[9]; 
		out[4] = in[10]; 
		out[5] = in[11]; 
		out[6] = in[12]; 
		out[7] = in[13]; 
		out[8] = in[14]; 
		out[9] = in[15]; 
	}
	else if(s==11){ 
		out[11] = in[0]; 
		out[12] = in[1]; 
		out[13] = in[2]; 
		out[14] = in[3]; 
		out[15] = in[4]; 
		out[0] = in[5]; 
		out[1] = in[6]; 
		out[2] = in[7]; 
		out[3] = in[8]; 
		out[4] = in[9]; 
		out[5] = in[10]; 
		out[6] = in[11]; 
		out[7] = in[12]; 
		out[8] = in[13]; 
		out[9] = in[14]; 
		out[10] = in[15]; 
	}
	else if(s==12){ 
		out[12] = in[0]; 
		out[13] = in[1]; 
		out[14] = in[2]; 
		out[15] = in[3]; 
		out[0] = in[4]; 
		out[1] = in[5]; 
		out[2] = in[6]; 
		out[3] = in[7]; 
		out[4] = in[8]; 
		out[5] = in[9]; 
		out[6] = in[10]; 
		out[7] = in[11]; 
		out[8] = in[12]; 
		out[9] = in[13]; 
		out[10] = in[14]; 
		out[11] = in[15]; 
	}
	else if(s==13){ 
		out[13] = in[0]; 
		out[14] = in[1]; 
		out[15] = in[2]; 
		out[0] = in[3]; 
		out[1] = in[4]; 
		out[2] = in[5]; 
		out[3] = in[6]; 
		out[4] = in[7]; 
		out[5] = in[8]; 
		out[6] = in[9]; 
		out[7] = in[10]; 
		out[8] = in[11]; 
		out[9] = in[12]; 
		out[10] = in[13]; 
		out[11] = in[14]; 
		out[12] = in[15]; 
	}
	else if(s==14){ 
		out[14] = in[0]; 
		out[15] = in[1]; 
		out[0] = in[2]; 
		out[1] = in[3]; 
		out[2] = in[4]; 
		out[3] = in[5]; 
		out[4] = in[6]; 
		out[5] = in[7]; 
		out[6] = in[8]; 
		out[7] = in[9]; 
		out[8] = in[10]; 
		out[9] = in[11]; 
		out[10] = in[12]; 
		out[11] = in[13]; 
		out[12] = in[14]; 
		out[13] = in[15]; 
	}
	else { 
		out[15] = in[0]; 
		out[0] = in[1]; 
		out[1] = in[2]; 
		out[2] = in[3]; 
		out[3] = in[4]; 
		out[4] = in[5]; 
		out[5] = in[6]; 
		out[6] = in[7]; 
		out[7] = in[8]; 
		out[8] = in[9]; 
		out[9] = in[10]; 
		out[10] = in[11]; 
		out[11] = in[12]; 
		out[12] = in[13]; 
		out[13] = in[14]; 
		out[14] = in[15]; 
	}
	return;
}

int save_tmp_edges(edge_dtype * URAM_edges[EDGE_PACK_SIZE], map_t stats[EDGE_PACK_SIZE][EDGE_PACK_SIZE], edge_vec_dtype * HBM_channelTmp){
	// NOTE: this function arranges all source vids into EDGE_PACK_SIZE groups {v=0;srcvid%EDGE_PACK_SIZE==0, v=1;srcvid%EDGE_PACK_SIZE==1 etc.}
	edge_dtype edge_in[NUM_LLP_PER_LLPSET];	
	edge_dtype edge_out[NUM_LLP_PER_LLPSET];
	unsigned int offset_p[NUM_LLP_PER_LLPSET];
	unsigned int p_[NUM_LLP_PER_LLPSET];
	unsigned int index = 0;
	unsigned int max[NUM_LLP_PER_LLPSET]; 
	unsigned int index_t = 0;
	for(unsigned int llp_id=0; llp_id<NUM_LLP_PER_LLPSET; llp_id++){ max[llp_id] = 0; for(unsigned int v=0; v<NUM_LLP_PER_LLPSET; v++){ if(max[llp_id] < stats[v][llp_id].size){ max[llp_id] = stats[v][llp_id].size; }}}
	unsigned int max_ = 0; for(unsigned int v=0; v<NUM_LLP_PER_LLPSET; v++){ max_ += max[v]; }
	
	unsigned int llp_id = 0;
	for(unsigned int t=0; t<max_; t++){ 
		if(index_t >= max[llp_id]){ llp_id += 1; index_t = 0; }
		
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 	
			unsigned int p_ = ((EDGE_PACK_SIZE + v - llp_id) % EDGE_PACK_SIZE);		
			if(index_t < stats[v][p_].size){ edge_in[v] = URAM_edges[v][stats[v][p_].offset + index_t]; } 		
			else { edge_in[v].srcvid = INVALIDDATA; edge_in[v].dstvid = INVALIDDATA; }	
		}
		
		#ifdef _DEBUGMODE_KERNELPRINTS//4
		if(t==0){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ cout<<"save_partiallyprepared_edgeupdates (before-cyclic-rotate): llp_id: "<<llp_id<<", t:"<<t<<", edge_in["<<v<<"].srcvid: "<<edge_in[v].srcvid<<" ("<<edge_in[v].srcvid % EDGE_PACK_SIZE<<"), edge_in["<<v<<"].dstvid: "<<edge_in[v].dstvid<<" ("<<edge_in[v].dstvid % EDGE_PACK_SIZE<<")"<<endl; }}
		#endif 
		unsigned int rotateby = edge_in[0].srcvid % EDGE_PACK_SIZE;
		rearrange_layout(rotateby, edge_in, edge_out); 		
		#ifdef _DEBUGMODE_KERNELPRINTS//4
		if(t==0){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ cout<<"save_partiallyprepared_edgeupdates (after-cyclic-rotate): llp_id: "<<llp_id<<", t:"<<t<<", edge_out["<<v<<"].srcvid: "<<edge_out[v].srcvid<<" ("<<edge_out[v].srcvid % EDGE_PACK_SIZE<<"), edge_out["<<v<<"].dstvid: "<<edge_out[v].dstvid<<" ("<<edge_out[v].dstvid % EDGE_PACK_SIZE<<")"<<endl; }}
		#endif 
		
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){				
			HBM_channelTmp[index].data[v].srcvid = edge_out[v].srcvid;
			HBM_channelTmp[index].data[v].dstvid = edge_out[v].dstvid;
		}
		
		index += 1;	
		index_t += 1;
		#ifndef FPGA_IMPL
		checkoutofbounds_("create_act_pack::ERROR 221815::", index, EDGE_UPDATES_DRAMBUFFER_LONGSIZE, NAp, index_t, max_);
		#endif 
	}
	
	#ifdef _DEBUGMODE_KERNELPRINTS//4
	cout<<">>> save_partiallyprepared_edgeupdates (after-cyclic-rotate): index: "<<index<<endl;
	#endif 
	return index;
}	

void save_final_edges(unsigned int cmd, unsigned int base_offset, map_t stats[EDGE_PACK_SIZE][EDGE_PACK_SIZE], edge_dtype * URAM_edges[EDGE_PACK_SIZE], map_t * edges_map, map_t edges_dmap[MAX_NUM_FPGAS][NUM_LLP_PER_LLPSET], HBM_channelAXISW_t * HBM_channelA, HBM_channelAXISW_t * HBM_channelB, universalparams_t universalparams){
	unsigned int offset_p[EDGE_PACK_SIZE];
	unsigned int p_[EDGE_PACK_SIZE];
	edge_dtype edge[EDGE_PACK_SIZE];	
	
	for(unsigned int llp_id=0; llp_id<EDGE_PACK_SIZE; llp_id++){	
		unsigned int offset = base_offset + edges_map[llp_id].offset + edges_map[llp_id].size;
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ p_[v] =  ((llp_id + v) % EDGE_PACK_SIZE); offset_p[v] = stats[v][p_[v]].offset; }
		unsigned int max = 0; for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ if(max < stats[v][llp_id].size){ max = stats[v][llp_id].size; }}	
		#ifdef _DEBUGMODE_KERNELPRINTS//4
		cout<<"save_fullyprepared_edgeupdates: llp_id: "<<llp_id<<endl; for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ cout<<""<<p_[v]<<", "; } cout<<endl;
		cout<<"save_fullyprepared_edgeupdates: llp_id: "<<llp_id<<endl; for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ cout<<""<<offset_p[v]<<", "; } cout<<endl;
		#endif 
		
		unsigned int fpga_sel[MAX_NUM_FPGAS][EDGE_PACK_SIZE];
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ fpga_sel[fpga][v] = 0; }}
		
		SAVE_FULLYPREPARED_EDGEUPDATES: for(unsigned int t=0; t<max; t++){
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 	
				edge_dtype edge_update_ = URAM_edges[v][offset_p[v] + t];
				if(t < stats[v][p_[v]].size){ edge[v].srcvid = edge_update_.srcvid / EDGE_PACK_SIZE; edge[v].dstvid = edge_update_.dstvid / EDGE_PACK_SIZE; } // NOTE: converted local partition vid to local vector partition 
				else { edge[v].srcvid = INVALIDDATA; edge[v].dstvid = INVALIDDATA; }
				#ifdef _DEBUGMODE_KERNELPRINTS//4
				if(t==0){ cout<<"save_fullyprepared_edgeupdates: llp_id: "<<llp_id<<", t:"<<t<<", edge["<<v<<"].srcvid: "<<edge[v].srcvid<<" ("<<edge[v].srcvid % EDGE_PACK_SIZE<<"), edge["<<v<<"].dstvid: "<<edge[v].dstvid<<" ("<<edge[v].dstvid % EDGE_PACK_SIZE<<")"<<endl; }
				#endif 
			}
			
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ if(edge[v].dstvid != INVALIDDATA){ unsigned int fpga = owner_fpga2(edge[v].dstvid); fpga_sel[fpga][v] += 1; }}	
			
			for(unsigned int v=0; v<EDGE_PACK_SIZE/2; v++){				
				HBM_channelA[offset + t].data[2*v] = edge[v].srcvid;
				HBM_channelA[offset + t].data[2*v + 1] = edge[v].dstvid;
			}
			for(unsigned int v=0; v<EDGE_PACK_SIZE/2; v++){				
				HBM_channelB[offset + t].data[2*v] = edge[EDGE_PACK_SIZE/2 + v].srcvid;
				HBM_channelB[offset + t].data[2*v + 1] = edge[EDGE_PACK_SIZE/2 + v].dstvid;
			}
		}		
		
		edges_map[llp_id].size += max;
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
			unsigned int max_val = 0; 
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ if(max_val < fpga_sel[fpga][v]){ max_val = fpga_sel[fpga][v]; }}
			edges_dmap[fpga][llp_id].size += max_val; 
		}
	}
	// exit(EXIT_SUCCESS);
}

unsigned int create_act_pack::create_actpack(
		vector<edge3_type> (&partitioned_edges)[NUM_PEs][MAX_NUM_UPARTITIONS][MAX_NUM_LLPSETS], HBM_channelAXISW_t * HBM_channelA[NUM_PEs], HBM_channelAXISW_t * HBM_channelB[NUM_PEs], map_t * returned_edge_maps[NUM_PEs], map_t * returned_vu_map[NUM_PEs], unsigned int offset_dest, 
		unsigned int num_upartitions, unsigned int num_vpartitions, unsigned int start_pu, unsigned int size_pu, unsigned int skip_pu, unsigned int cmd
		){		

	edge_dtype * URAM_edges[NUM_VALID_PEs][EDGE_PACK_SIZE];for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ URAM_edges[i][v] = new edge_dtype[EDGE_UPDATES_DRAMBUFFER_LONGSIZE]; }}
	edge_vec_dtype * HBM_channelTmp[NUM_VALID_PEs]; for(unsigned int i=0; i<NUM_VALID_PEs; i++){ HBM_channelTmp[i] = new edge_vec_dtype[EDGE_UPDATES_DRAMBUFFER_LONGSIZE]; }

	unsigned int _MAX_BUFFER_SIZE = EDGE_UPDATES_DRAMBUFFER_LONGSIZE; 
	unsigned int _MAX_WORKBUFFER_SIZE = EDGE_UPDATES_DRAMBUFFER_LONGSIZE; 
	
	map_t stats[NUM_VALID_PEs][EDGE_PACK_SIZE][EDGE_PACK_SIZE]; 
	map_t edges_map[NUM_VALID_PEs][NUM_LLP_PER_LLPSET];
	unsigned int temp_size[NUM_VALID_PEs];
	unsigned int total_sz = 0;
	unsigned int running_offset[NUM_VALID_PEs]; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ running_offset[n] = 0; }
	
	map_t vupdates_map[MAX_NUM_UPARTITIONS][MAX_NUM_LLPSETS];
	for(unsigned int p_u=0; p_u<num_upartitions; p_u++){	
		for(unsigned int t=0; t<num_vpartitions; t++){	
			vupdates_map[p_u][t].offset = 0; vupdates_map[p_u][t].size = 0;
		}
	}
	
	map_t vupdates2_map[MAX_NUM_UPARTITIONS][MAX_NUM_FPGAS][MAX_NUM_LLPSETS];
	for(unsigned int p_u=0; p_u<num_upartitions; p_u++){	
		for(unsigned int t=0; t<num_vpartitions; t++){	
			for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
				vupdates2_map[p_u][fpga][t].offset = 0; vupdates2_map[p_u][fpga][t].size = 0;
			}
		}
	}
	
	unsigned int returned_volume_size = 0; unsigned int returned_volume2_size = 0;
	CREATE_ACTPACK_BASELOOP1: for(unsigned int p_u=start_pu; p_u<start_pu + size_pu; p_u+=skip_pu){
		#ifdef _DEBUGMODE_KERNELPRINTS4
		cout<<"### creating act-pack in upartition "<<p_u<<": [PEs "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<n<<", "; } cout<<"]"<<endl; 
		#endif 
		CREATE_ACTPACK_BASELOOP1B: for(unsigned int llp_set=0; llp_set<num_vpartitions; llp_set++){ // num_vpartitions
			if(partitioned_edges[0][p_u][llp_set].size() == 0){ continue; } // NEW FIXME.	
				
			#ifdef _DEBUGMODE_KERNELPRINTS//4	
			if(false && num_upartitions <= 32){ cout<<">>> creating act-pack in upartition "<<p_u<<", llp_set: "<<llp_set<<"...."<<endl; }
			if(llp_set <= 4){ cout<<">>> creating act-pack in upartition "<<p_u<<", llp_set: "<<llp_set<<"...."<<endl; }
			#endif 
			for(unsigned int llp_id=0; llp_id<NUM_LLP_PER_LLPSET; llp_id++){ 
				for(unsigned int n=0; n<NUM_VALID_PEs; n++){ 
					edges_map[n][llp_id].offset = 0; edges_map[n][llp_id].size = 0;
				}
			}
			
			map_t edges_dmap[NUM_VALID_PEs][MAX_NUM_FPGAS][NUM_LLP_PER_LLPSET]; // FIXME. ADD HLS PRAGMAS?
			for(unsigned int llp_id=0; llp_id<NUM_LLP_PER_LLPSET; llp_id++){ 
				for(unsigned int n=0; n<NUM_VALID_PEs; n++){ 
					for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
						edges_dmap[n][fpga][llp_id].offset = 0; edges_dmap[n][fpga][llp_id].size = 0;
					}
				}
			}
			
			map_t rawedge_maps[NUM_VALID_PEs];
			for(unsigned int n=0; n<NUM_VALID_PEs; n++){ rawedge_maps[n].size = partitioned_edges[n][p_u][llp_set].size() / EDGE_PACK_SIZE; }
			// cout<<"^^^^^ create_act_pack::prepare-edge-updates:: rawedges_maps: "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<rawedge_maps[n].size<<", "; } cout<<endl;
			
			for(unsigned int c=0; c<2; c++){
				unsigned int maxnum_rawedges = 0; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ if(maxnum_rawedges < rawedge_maps[n].size){ maxnum_rawedges = rawedge_maps[n].size; }}
				// if(maxnum_rawedges >= _MAX_WORKBUFFER_SIZE){ maxnum_rawedges = _MAX_WORKBUFFER_SIZE; } // trim
				unsigned int maxnum_preprocedges = 0; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ if(maxnum_preprocedges < temp_size[n]){ maxnum_preprocedges = temp_size[n]; }}
				unsigned int work_size;	if(c==0){ work_size = maxnum_rawedges; } else { work_size = maxnum_preprocedges; }
				#ifdef _DEBUGMODE_CHECKS3
				if(work_size > _MAX_BUFFER_SIZE){ cout<<"ALERT: c("<<c<<")=="<<c<<" && work_size("<<work_size<<") > _MAX_BUFFER_SIZE("<<_MAX_BUFFER_SIZE<<"). EDGE_UPDATES_DRAMBUFFER_LONGSIZE("<<EDGE_UPDATES_DRAMBUFFER_LONGSIZE<<" EXITING..."<<endl; exit(EXIT_FAILURE); }
				#endif
				
				#ifdef _DEBUGMODE_KERNELPRINTS4_CREATEACTPACT
				cout<<">>> create_act_pack::prepare-edge-updates:: c: "<<c<<", p_u: "<<p_u<<", llp_set: "<<llp_set<<", work_size: "<<work_size<<", _MAX_BUFFER_SIZE: "<<_MAX_BUFFER_SIZE<<", partitioned_edges[0]["<<p_u<<"]["<<llp_set<<"].size(): "<<partitioned_edges[0][p_u][llp_set].size() / EDGE_PACK_SIZE<<endl; 
				cout<<"create_act_pack::prepare-edge-updates:: rawedges_maps: "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<rawedge_maps[n].size<<", "; } cout<<endl;
				if(c==1){ cout<<"create_act_pack::prepare-edge-updates:: temp_size: "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<temp_size[n]<<", "; } cout<<endl; }
				#endif 
				
				// reset
				LOOP1734: for(unsigned int t=0; t<EDGE_PACK_SIZE; t++){
					for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 
						for(unsigned int n=0; n<NUM_VALID_PEs; n++){
							stats[n][v][t].offset = 0;
							stats[n][v][t].size = 0;
						}
					}
				}
			
				// partition stage 1
				edge_vec_dtype edges_vec[NUM_VALID_PEs];
				CREATE_ACTPACK_LOOP1: for(unsigned int t=0; t<work_size; t++){
					for(unsigned int n=0; n<NUM_VALID_PEs; n++){ 
						for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
							if(c==0){
								edges_vec[n].data[v].srcvid = partitioned_edges[n][p_u][llp_set][t*EDGE_PACK_SIZE + v].srcvid % MAX_UPARTITION_SIZE; // NOTE: converted vid to local partition vid 
								edges_vec[n].data[v].dstvid = partitioned_edges[n][p_u][llp_set][t*EDGE_PACK_SIZE + v].dstvid % MAX_UPARTITION_SIZE; // NOTE: converted vid to local partition vid
							} else {
								edges_vec[n].data[v] = HBM_channelTmp[n][t].data[v]; 
							}
						}
					}
					
					for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
						for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
							if(edges_vec[inst].data[v].srcvid != INVALIDDATA){
								#ifdef _DEBUGMODE_KERNELPRINTS//4
								if(inst == 0 && t<2){ cout<<">>> collect-stats: [t:"<<t<<"]: edges_vec["<<inst<<"].data["<<v<<"].srcvid: "<<edges_vec[inst].data[v].srcvid<<" ("<<edges_vec[inst].data[v].srcvid % EDGE_PACK_SIZE<<"), edges_vec["<<inst<<"].data["<<v<<"].dstvid: "<<edges_vec[inst].data[v].dstvid<<" ("<<edges_vec[inst].data[v].dstvid % EDGE_PACK_SIZE<<")"<<endl; }		
								if(inst == 0 && t<2){ cout<<">>> collect-stats: [t:"<<t<<"]: edge_update_vecs2["<<inst<<"].data["<<v<<"].srcvid: "<<edge_update_vecs2[inst].data[v].srcvid<<" ("<<edge_update_vecs2[inst].data[v].srcvid % EDGE_PACK_SIZE<<"), edge_update_vecs2["<<inst<<"].data["<<v<<"].dstvid: "<<edge_update_vecs2[inst].data[v].dstvid<<" ("<<edge_update_vecs2[inst].data[v].dstvid % EDGE_PACK_SIZE<<")"<<endl; }		
								#endif 
								unsigned int data = 0; if(c==0){ data = edges_vec[inst].data[v].srcvid; } else { data = edges_vec[inst].data[v].dstvid; }
								unsigned int p = owner_partition(c, data); // data % EDGE_PACK_SIZE;
								#ifndef FPGA_IMPL
								checkoutofbounds_("create_act_pack::ERROR 2215::", p, EDGE_PACK_SIZE, NAp, NAp, NAp);
								#endif 
								if(data != INVALIDDATA){ stats[inst][v][p].size += 1; }
							}
						}
					}
				}
				#ifdef _DEBUGMODE_KERNELPRINTS//4
				for(unsigned int p=0; p<EDGE_PACK_SIZE; p++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ for(unsigned int n=0; n<1; n++){ cout<<"c: "<<c<<" (before): stats["<<n<<"]["<<v<<"]["<<p<<"].offset: "<<stats[n][v][p].offset<<", stats["<<n<<"]["<<v<<"]["<<p<<"].size: "<<stats[n][v][p].size<<endl; }}}		
				#endif 
				
				// calculate counts offsets 
				for(unsigned int p=1; p<EDGE_PACK_SIZE; p++){ 
					for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 
						for(unsigned int n=0; n<NUM_VALID_PEs; n++){
							stats[n][v][p].offset = stats[n][v][p-1].offset + stats[n][v][p-1].size;					
							#ifdef _DEBUGMODE_CHECKS3
							if(stats[0][v][p-1].offset + stats[0][v][p-1].size > _MAX_BUFFER_SIZE){ cout<<"prepare-edge-updates:: ALERT: stats[0]["<<v<<"]["<<p-1<<"].offset ("<<stats[0][v][p-1].offset<<") + stats[0]["<<v<<"]["<<p-1<<"].size ("<<stats[0][v][p-1].size<<") (:"<<stats[0][v][p-1].offset + stats[0][v][p-1].size<<":) >= EDGE_UPDATES_DRAMBUFFER_SIZE. EXITING..."<<endl; exit(EXIT_FAILURE); }					
							#endif	
						}	
					}	
				}		
				#ifdef _DEBUGMODE_KERNELPRINTS//4
				for(unsigned int p=0; p<EDGE_PACK_SIZE; p++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ for(unsigned int n=0; n<1; n++){ cout<<"c: "<<c<<" (after): stats["<<n<<"]["<<v<<"]["<<p<<"].offset: "<<stats[n][v][p].offset<<", stats["<<n<<"]["<<v<<"]["<<p<<"].size: "<<stats[n][v][p].size<<endl; }}}	
				#endif 
				
				// reset counts sizes
				LOOP1234: for(unsigned int p=0; p<EDGE_PACK_SIZE; p++){ 
					for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 
						for(unsigned int n=0; n<NUM_VALID_PEs; n++){
							stats[n][v][p].size = 0;
						}
					}
				}

				// partition stage 2
				CREATE_ACTPACK_LOOP2: for(unsigned int t=0; t<work_size; t++){ 
					for(unsigned int n=0; n<NUM_VALID_PEs; n++){ 
						for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
							if(c==0){
								edges_vec[n].data[v].srcvid = partitioned_edges[n][p_u][llp_set][t*EDGE_PACK_SIZE + v].srcvid % MAX_UPARTITION_SIZE; 
								edges_vec[n].data[v].dstvid = partitioned_edges[n][p_u][llp_set][t*EDGE_PACK_SIZE + v].dstvid % MAX_UPARTITION_SIZE; 
							} else {
								edges_vec[n].data[v] = HBM_channelTmp[n][t].data[v]; 
							}
						}
					}
					for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
						for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
							if(edges_vec[inst].data[v].srcvid != INVALIDDATA){ 
								unsigned int data = 0; if(c==0){ data = edges_vec[inst].data[v].srcvid; } else { data = edges_vec[inst].data[v].dstvid; }
								unsigned int p = owner_partition(c, data); // data % EDGE_PACK_SIZE;
								#ifndef FPGA_IMPL
								checkoutofbounds_("create_act_pack::ERROR 2215::", p, EDGE_PACK_SIZE, NAp, NAp, NAp);
								#endif 
								URAM_edges[inst][v][stats[inst][v][p].offset + stats[inst][v][p].size] = edges_vec[inst].data[v];
								if(data != INVALIDDATA){ stats[inst][v][p].size += 1; }
								
								#ifdef _DEBUGMODE_KERNELPRINTS//4	
								if(inst==0 && t<8 && v==0){ cout<<"[finalized edge update: p: "<<p<<", srcvid: "<<edges_vec[inst].data[v].srcvid<<", dstvid: "<<edges_vec[inst].data[v].dstvid<<"]"<<endl; }						
								#endif 
							}
						}
					}	
				}
			
				// store edge updates
				if(c==0){ 
					for(unsigned int i=0; i<NUM_VALID_PEs; i++){
						temp_size[i] = save_tmp_edges(URAM_edges[i], stats[i], HBM_channelTmp[i]);	
					}
				} else {
					#ifdef _DEBUGMODE_KERNELPRINTS4_CREATEACTPACT // NUM_LLP_PER_LLPSET
					for(unsigned int n=0; n<NUM_VALID_PEs; n++){ for(unsigned int llp_id=0; llp_id<NUM_LLP_PER_LLPSET; llp_id++){ cout<<"prepare-edge-updates (before): edges_map[n]["<<llp_id<<"]: p_u: "<<p_u<<", llp_set: "<<llp_set<<", llp_id: "<<llp_id<<", offset: "<<edges_map[n][llp_id].offset<<", size: "<<edges_map[n][llp_id].size<<""<<endl; }}
					#endif 	
					for(unsigned int i=0; i<NUM_VALID_PEs; i++){
						save_final_edges(cmd, offset_dest, stats[i], URAM_edges[i], edges_map[i], edges_dmap[i], HBM_channelA[i], HBM_channelB[i], universalparams);	
					}
					#ifdef _DEBUGMODE_KERNELPRINTS4_CREATEACTPACT
					for(unsigned int n=0; n<NUM_VALID_PEs; n++){ for(unsigned int llp_id=0; llp_id<NUM_LLP_PER_LLPSET; llp_id++){ cout<<"prepare-edge-updates (after): edges_map[n]["<<llp_id<<"]: p_u: "<<p_u<<", llp_set: "<<llp_set<<", llp_id: "<<llp_id<<", offset: "<<edges_map[n][llp_id].offset<<", size: "<<edges_map[n][llp_id].size<<""<<endl; }}
					#endif 
				}	

				// update edge maps
				if(c == 1){ 
					SAVE_EDGEMAPS_LOOP1: for(unsigned int llp_id=0; llp_id<NUM_LLP_PER_LLPSET; llp_id++){	
						for(unsigned int n=0; n<NUM_VALID_PEs; n++){
							edges_map[n][llp_id].offset = running_offset[n]; // set offset
							#ifdef _DEBUGMODE_KERNELPRINTS//4
							cout<<"^^^^^^^^^^ prepare-edge-updates: edges_map["<<n<<"]["<<llp_id<<"].offset: "<<edges_map[n][llp_id].offset<<", edges_map["<<n<<"]["<<llp_id<<"].size: "<<edges_map[n][llp_id].size<<""<<endl; 
							cout<<"---------- prepare-edge-updates: running_offset["<<n<<"]: "<<running_offset[n]<<""<<endl; 
							#endif 
							running_offset[n] += edges_map[n][llp_id].size;
							returned_volume2_size += edges_map[n][llp_id].size;
						}
						
						map_t edge_map_vec[NUM_VALID_PEs];
						for(unsigned int n=0; n<NUM_VALID_PEs; n++){
							edge_map_vec[n] = edges_map[n][llp_id];
							returned_edge_maps[n][p_u*MAX_NUM_LLP_PER_UPARTITION + llp_set*NUM_LLP_PER_LLPSET + llp_id] = edges_map[n][llp_id];
						}
						vupdates_map[p_u][llp_set].size += edges_map[0][llp_id].size;
						for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ 
							vupdates2_map[p_u][fpga][llp_set].size += edges_dmap[0][fpga][llp_id].size; 
							returned_volume_size += edges_dmap[0][fpga][llp_id].size;
						}
						
						#ifdef _DEBUGMODE_KERNELPRINTS//4
						for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<"prepare-edge-updates: edges_map["<<n<<"]["<<llp_id<<"].offset: "<<edges_map[n][llp_id].offset<<", edges_map["<<n<<"]["<<llp_id<<"].size: "<<edges_map[n][llp_id].size<<""<<endl; }
						#endif
					}
				}
				
				#ifdef _DEBUGMODE_CHECKS3
				for(unsigned int llp_id=0; llp_id<NUM_LLP_PER_LLPSET; llp_id++){ total_sz += edges_map[0][llp_id].size; }
				#endif 
				// exit(EXIT_SUCCESS); 
			} // c
			// exit(EXIT_SUCCESS); 
		} // llp_set
		// exit(EXIT_SUCCESS);
	} // p_u
	
	map_t vertex_updates_map[MAX_NUM_LLPSETS]; 
	map_t vertex_updates2_map[MAX_NUM_FPGAS][MAX_NUM_LLPSETS]; 
	for(unsigned int t=0; t<num_vpartitions+1; t++){ 
		vertex_updates_map[t].offset=0; vertex_updates_map[t].size=0;
		for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){ vertex_updates2_map[fpga][t].offset=0; vertex_updates2_map[fpga][t].size=0; }
	}
	unsigned int returned_running_size1 = 0;
	for(unsigned int p_u=0; p_u<num_upartitions; p_u++){	
		for(unsigned int t=0; t<num_vpartitions; t++){ 
			#ifdef _DEBUGMODE_KERNELPRINTS//4
			cout<<"finish: vupdates_map["<<p_u<<"]["<<t<<"].size: "<<vupdates_map[p_u][t].size<<endl;
			#endif 
			vertex_updates_map[t].size += vupdates_map[p_u][t].size; 
			for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
				vertex_updates2_map[fpga][t].size += vupdates2_map[p_u][fpga][t].size; 
				returned_running_size1 += vupdates2_map[p_u][fpga][t].size;
				#ifdef _DEBUGMODE_KERNELPRINTS//4
				cout<<"*********** finish: p_u: "<<p_u<<", t: "<<t<<", fpga: "<<fpga<<", returned_running_size1: "<<returned_running_size1<<endl;
				#endif 
			}
		}
	}	
	for(unsigned int t=1; t<num_vpartitions+1; t++){	
		vertex_updates_map[t].offset = vertex_updates_map[t-1].offset + vertex_updates_map[t-1].size;	// 35:34
	}	
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){		
		for(unsigned int t=1; t<num_vpartitions+1; t++){	
			vertex_updates2_map[fpga][t].offset = vertex_updates2_map[fpga][t-1].offset + vertex_updates2_map[fpga][t-1].size;	
		}	
		unsigned int last_index = num_vpartitions - 1;
		vertex_updates2_map[fpga+1][0].offset = vertex_updates2_map[fpga][last_index].offset + vertex_updates2_map[fpga][last_index].size; 
	}	
	for(unsigned int t=0; t<num_vpartitions+1; t++){	
		#ifdef _DEBUGMODE_KERNELPRINTS//4
		cout<<"finish: vertex_updates_map["<<t<<"].offset: "<<vertex_updates_map[t].offset<<", vertex_updates_map["<<t<<"].size: "<<vertex_updates_map[t].size<<endl;
		#endif
	}	
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){	
		#ifdef _DEBUGMODE_KERNELPRINTS//4
		cout<<"finish: offsets for fpga "<<fpga<<endl;
		#endif 
		for(unsigned int t=0; t<num_vpartitions+1; t++){
			#ifdef _DEBUGMODE_KERNELPRINTS//4
			cout<<"finish: [vertex_updates2_map["<<fpga<<"]["<<t<<"].offset: "<<vertex_updates2_map[fpga][t].offset<<", vertex_updates2_map["<<fpga<<"]["<<t<<"].size: "<<vertex_updates2_map[fpga][t].size<<"]"<<endl;
			#endif 
		}
	}	
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){	
		for(unsigned int t=0; t<num_vpartitions+1; t++){	
			vertex_updates_map[t].size = 0;	
			vertex_updates2_map[fpga][t].size = 0;	
		}
	}
	for(unsigned int fpga=0; fpga<universalparams.NUM_FPGAS_; fpga++){
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			for(unsigned int t=0; t<num_vpartitions; t++){		
				unsigned int index = (fpga * MAX_NUM_LLPSETS) + t;
				returned_vu_map[n][index] = vertex_updates2_map[fpga][t];
				#ifdef _DEBUGMODE_KERNELPRINTS//4
				cout<<"---------- prepare-edge-updates: returned_vu_map["<<n<<"]["<<index<<"].offset: "<<returned_vu_map[n][index].offset<<", returned_vu_map["<<n<<"]["<<index<<"].size: "<<returned_vu_map[n][index].size<<endl; 
				#endif 
			}
		}
	}
	#ifdef _DEBUGMODE_KERNELPRINTS//4
	cout<<"------------------------ create_act_pack::FINISH: returned_volume_size: "<<returned_volume_size<<", returned_volume_size * EDGE_PACK_SIZE: "<<returned_volume_size * EDGE_PACK_SIZE<<endl;
	cout<<"------------------------ create_act_pack::FINISH: returned_volume2_size: "<<returned_volume2_size<<", returned_volume2_size * EDGE_PACK_SIZE: "<<returned_volume2_size * EDGE_PACK_SIZE<<endl;
	#endif 
	// return running_offset[0];
	// exit(EXIT_SUCCESS);
	// return returned_volume_size;
	return returned_volume2_size;
}





#include "../include/common.h"
#ifndef ___RUNNING_FPGA_SYNTHESIS___ // FPGA_IMPL
#include "acts_kernel.h"
using namespace std;
#endif 

#define ___ENABLE___RESETBUFFERSATSTART___ 
#define ___ENABLE___CLEAR_COUNTERS___
#define ___ENABLE___PROCESSEDGES___ 
#define ___ENABLE___READ_FRONTIER_PROPERTIES___
#define ___ENABLE___RESET_BUFFERS___
#define ___ENABLE___ECPROCESSEDGES___II1___ 
#define ___ENABLE___APPLYUPDATESMODULE___ 
#define ___ENABLE___READ_DEST_PROPERTIES___ 	
	#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___ 
		#define ___ENABLE___APPLYVERTEXUPDATES_AND_APPLYEDGES___II1___
	#else 
		#define ___ENABLE___APPLYUPDATES___II1___
	#endif 
	#define ___ENABLE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___ 
	#define ___ENABLE___SAVE_DEST_PROPERTIES___ 
// #define ___ENABLE___GATHER_FRONTIERINFOS___ // FIXME^
#define ___ENABLE___REPORT_STATISTICS___
// #define ___ENABLE___IMPORT_EXPORT___
#define _____DEBUG_____	

#define MAXVALID_APPLYPARTITION_VECSIZE (MAX_APPLYPARTITION_VECSIZE >> POW_VALID_VDATA)
#define MAXVALID_VERTEXUPDATES_BUFFER_SIZE (VERTEXUPDATES_BUFFER_SIZE >> POW_VALID_VDATA)

unsigned int num_prints = 16;	

#ifdef ___RUNNING_FPGA_SYNTHESIS___ // FPGA_IMPL
#define MY_IFDEF_NFRONTIER() keyvalue_t nfrontier_buffer[EDGE_PACK_SIZE][MAX_VDATA_SUBPARTITION_VECSIZE]
#define MY_IFDEF_CFRONTIER_TMP() keyvalue_t URAM_frontiers[EDGE_PACK_SIZE][MAXVALID_APPLYPARTITION_VECSIZE]
#define MY_IFDEF_CFRONTIER() keyvalue_t cfrontier_buffer[NUM_VALID_PEs][MAXVALID_APPLYPARTITION_VECSIZE]
#define MY_IFDEF_UPDATESTMPBUFFER() keyvalue_t updates_buffer2[UPDATES_BUFFER_PACK_SIZE][UPDATES_BUFFER_SIZE]	
#define MY_IFDEF_VPTRBUFFER() vtr_t vptr_buffer[VPTR_BUFFER_SIZE]
#define MY_IFDEF_EDGESBUFFER() edge3_type edges_buffer[EDGE_PACK_SIZE][EDGE_BUFFER_SIZE]
#define MY_IFDEF_EDGEUPDATESBUFFER() edge_update_type URAM_edges[EDGE_PACK_SIZE][EDGE_UPDATES_DRAMBUFFER_SIZE]
#define MY_IFDEF_VDATABUFFER() vprop_t URAM_vprop[EDGE_PACK_SIZE][MAXVALID_APPLYPARTITION_VECSIZE]
#else
#define MY_IFDEF_NFRONTIER() keyvalue_t * nfrontier_buffer[EDGE_PACK_SIZE]
#define MY_IFDEF_CFRONTIER_TMP() keyvalue_t * URAM_frontiers[EDGE_PACK_SIZE]
#define MY_IFDEF_CFRONTIER() keyvalue_t * cfrontier_buffer[NUM_VALID_PEs]
#define MY_IFDEF_UPDATESTMPBUFFER() keyvalue_t * updates_buffer2[UPDATES_BUFFER_PACK_SIZE]
#define MY_IFDEF_VPTRBUFFER() vtr_t * vptr_buffer
#define MY_IFDEF_EDGESBUFFER() edge3_type * edges_buffer[EDGE_PACK_SIZE]
#define MY_IFDEF_EDGEUPDATESBUFFER() edge_update_type * URAM_edges[EDGE_PACK_SIZE]
#define MY_IFDEF_VDATABUFFER() vprop_t * URAM_vprop[EDGE_PACK_SIZE]			
#endif

#ifdef ___RUNNING_FPGA_SYNTHESIS___ // FPGA_IMPL
#define MY_IFDEF_TOPLEVELFUNC() void top_function( \
 HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4, \
 HBM_channelAXI_t * HBM_SRCA0, HBM_channelAXI_t * HBM_SRCB0, HBM_channelAXI_t * HBM_SRCA1, HBM_channelAXI_t * HBM_SRCB1, HBM_channelAXI_t * HBM_SRCA2, HBM_channelAXI_t * HBM_SRCB2, HBM_channelAXI_t * HBM_SRCA3, HBM_channelAXI_t * HBM_SRCB3, HBM_channelAXI_t * HBM_SRCA4, HBM_channelAXI_t * HBM_SRCB4, \
	unsigned int fpga, unsigned int module, unsigned int graph_iteration, unsigned int start_pu, unsigned int size_pu, unsigned int skip_pu, unsigned int start_pv_fpga, unsigned int start_pv, unsigned int size_pv, unsigned int start_llpset, unsigned int size_llpset, unsigned int start_llpid, unsigned int size_llpid, unsigned int start_gv_fpga, unsigned int start_gv, unsigned int size_gv, unsigned int id_process, unsigned int id_import, unsigned int id_export, unsigned int size_import_export, unsigned int status, unsigned int numfpgas, unsigned int command, \
	unsigned int mask0, unsigned int mask1, unsigned int mask2, unsigned int mask3, unsigned int mask4, unsigned int mask5, unsigned int mask6, unsigned int mask7 \
	)
#else
#define MY_IFDEF_TOPLEVELFUNC() unsigned int acts_kernel::top_function( \
 HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4, \
 HBM_channelAXI_t * HBM_SRCA0, HBM_channelAXI_t * HBM_SRCB0, HBM_channelAXI_t * HBM_SRCA1, HBM_channelAXI_t * HBM_SRCB1, HBM_channelAXI_t * HBM_SRCA2, HBM_channelAXI_t * HBM_SRCB2, HBM_channelAXI_t * HBM_SRCA3, HBM_channelAXI_t * HBM_SRCB3, HBM_channelAXI_t * HBM_SRCA4, HBM_channelAXI_t * HBM_SRCB4, \
	unsigned int fpga, unsigned int module, unsigned int graph_iteration, unsigned int start_pu, unsigned int size_pu, unsigned int skip_pu, unsigned int start_pv_fpga, unsigned int start_pv, unsigned int size_pv, unsigned int start_llpset, unsigned int size_llpset, unsigned int start_llpid, unsigned int size_llpid, unsigned int start_gv_fpga, unsigned int start_gv, unsigned int size_gv, unsigned int id_process, unsigned int id_import, unsigned int id_export, unsigned int size_import_export, unsigned int status, unsigned int numfpgas, unsigned int command, \
	unsigned int mask0, unsigned int mask1, unsigned int mask2, unsigned int mask3, unsigned int mask4, unsigned int mask5, unsigned int mask6, unsigned int mask7, \
	unsigned int report_statistics[64])					
#endif

#ifndef ___RUNNING_FPGA_SYNTHESIS___
unsigned int * globalparams_debug;  
acts_kernel::acts_kernel(universalparams_t _universalparams){
	utilityobj = new utility(_universalparams);
	universalparams = _universalparams;
	algorithmobj = new algorithm();
}
acts_kernel::~acts_kernel(){} 
#endif 

/////////////////////////////////////////
#ifndef FPGA_IMPL
void checkoutofbounds(string message, unsigned int data, unsigned int upper_bound, unsigned int msgdata1, unsigned int msgdata2, unsigned int msgdata3){
	#ifdef _DEBUGMODE_CHECKS3
	if(data >= upper_bound){ std::cout<<"utility::checkoutofbounds: ERROR. out of bounds. message: "<<message<<", data: "<<data<<", upper_bound: "<<upper_bound<<", msgdata1: "<<msgdata1<<", msgdata2: "<<msgdata2<<", msgdata3: "<<msgdata3<<std::endl; exit(EXIT_FAILURE); }			
	#endif 
}	
#endif 

unsigned int hash_edge(unsigned int srcvid, unsigned int dstvid, unsigned int index){
	unsigned int group = dstvid % 512;
	// unsigned int edge_hashid = (group * 16) + (dstvid % 16);
	unsigned int edge_hashid = (group * 16) + (srcvid % 16);
	
	#ifdef _DEBUGMODE_CHECKS3
	checkoutofbounds("utility::ERROR 25133::", edge_hashid, EDGE_UPDATES_DRAMBUFFER_SIZE, srcvid, dstvid, NAp);
	#endif

	return edge_hashid; 
}	

unsigned int hash2_edge(map_t URAM_map, unsigned int srcvid, unsigned int dstvid, unsigned int padding_factor){
#pragma HLS INLINE
	unsigned int allotted_hashspace = URAM_map.size * padding_factor; 
	#ifdef _DEBUGMODE_CHECKS3
	if(URAM_map.size == 0){ cout<<"-------------------------------- hash2_edge: URAM_map.size == 0. EXITING..."<<endl; exit(EXIT_FAILURE); }
	if(padding_factor == 0){ cout<<"-------------------------------- hash2_edge: padding_factor == 0. EXITING..."<<endl; exit(EXIT_FAILURE); }
	// if(dstvid == 0){ cout<<"-------------------------------- hash2_edge: srcvid = 0. dstvid == 0. EXITING..."<<endl; exit(EXIT_FAILURE); }
	#endif 
	unsigned int edge_hashid = URAM_map.offset + (dstvid % allotted_hashspace); 
	#ifdef _DEBUGMODE_CHECKS3
	checkoutofbounds("utility::ERROR 779911::", edge_hashid, EDGE_UPDATES_DRAMBUFFER_SIZE, dstvid, URAM_map.offset, URAM_map.size); 
	#endif
	return edge_hashid;							
}

void rearrangeLayoutV16x16B(unsigned int s, keyvalue_t in[NUM_VALID_HBM_CHANNELS][EDGE_PACK_SIZE], keyvalue_t out[NUM_VALID_HBM_CHANNELS][EDGE_PACK_SIZE]){
	// if(s==1){
		// 		// 		// out[1][0] = in[0][0]; 
		// 		// out[1][1] = in[0][1]; 
		// 		// out[1][2] = in[0][2]; 
		// 		// out[1][3] = in[0][3]; 
		// 		// out[1][4] = in[0][4]; 
		// 		// out[1][5] = in[0][5]; 
		// 		// out[1][6] = in[0][6]; 
		// 		// out[1][7] = in[0][7]; 
		// 		// out[1][8] = in[0][8]; 
		// 		// out[1][9] = in[0][9]; 
		// 		// out[1][10] = in[0][10]; 
		// 		// out[1][11] = in[0][11]; 
		// 		// out[1][12] = in[0][12]; 
		// 		// out[1][13] = in[0][13]; 
		// 		// out[1][14] = in[0][14]; 
		// 		// out[1][15] = in[0][15]; 
		// 	
		// 		// 		// out[2][0] = in[1][0]; 
		// 		// out[2][1] = in[1][1]; 
		// 		// out[2][2] = in[1][2]; 
		// 		// out[2][3] = in[1][3]; 
		// 		// out[2][4] = in[1][4]; 
		// 		// out[2][5] = in[1][5]; 
		// 		// out[2][6] = in[1][6]; 
		// 		// out[2][7] = in[1][7]; 
		// 		// out[2][8] = in[1][8]; 
		// 		// out[2][9] = in[1][9]; 
		// 		// out[2][10] = in[1][10]; 
		// 		// out[2][11] = in[1][11]; 
		// 		// out[2][12] = in[1][12]; 
		// 		// out[2][13] = in[1][13]; 
		// 		// out[2][14] = in[1][14]; 
		// 		// out[2][15] = in[1][15]; 
		// 	
		// 		// 		// out[3][0] = in[2][0]; 
		// 		// out[3][1] = in[2][1]; 
		// 		// out[3][2] = in[2][2]; 
		// 		// out[3][3] = in[2][3]; 
		// 		// out[3][4] = in[2][4]; 
		// 		// out[3][5] = in[2][5]; 
		// 		// out[3][6] = in[2][6]; 
		// 		// out[3][7] = in[2][7]; 
		// 		// out[3][8] = in[2][8]; 
		// 		// out[3][9] = in[2][9]; 
		// 		// out[3][10] = in[2][10]; 
		// 		// out[3][11] = in[2][11]; 
		// 		// out[3][12] = in[2][12]; 
		// 		// out[3][13] = in[2][13]; 
		// 		// out[3][14] = in[2][14]; 
		// 		// out[3][15] = in[2][15]; 
		// 	
		// 		// 		// out[4][0] = in[3][0]; 
		// 		// out[4][1] = in[3][1]; 
		// 		// out[4][2] = in[3][2]; 
		// 		// out[4][3] = in[3][3]; 
		// 		// out[4][4] = in[3][4]; 
		// 		// out[4][5] = in[3][5]; 
		// 		// out[4][6] = in[3][6]; 
		// 		// out[4][7] = in[3][7]; 
		// 		// out[4][8] = in[3][8]; 
		// 		// out[4][9] = in[3][9]; 
		// 		// out[4][10] = in[3][10]; 
		// 		// out[4][11] = in[3][11]; 
		// 		// out[4][12] = in[3][12]; 
		// 		// out[4][13] = in[3][13]; 
		// 		// out[4][14] = in[3][14]; 
		// 		// out[4][15] = in[3][15]; 
		// 	
		// 		// 		// out[0][0] = in[4][0]; 
		// 		// out[0][1] = in[4][1]; 
		// 		// out[0][2] = in[4][2]; 
		// 		// out[0][3] = in[4][3]; 
		// 		// out[0][4] = in[4][4]; 
		// 		// out[0][5] = in[4][5]; 
		// 		// out[0][6] = in[4][6]; 
		// 		// out[0][7] = in[4][7]; 
		// 		// out[0][8] = in[4][8]; 
		// 		// out[0][9] = in[4][9]; 
		// 		// out[0][10] = in[4][10]; 
		// 		// out[0][11] = in[4][11]; 
		// 		// out[0][12] = in[4][12]; 
		// 		// out[0][13] = in[4][13]; 
		// 		// out[0][14] = in[4][14]; 
		// 		// out[0][15] = in[4][15]; 
		// 	
		// 	// } else {	
		out[0][0] = in[0][0]; 
		out[0][1] = in[0][1]; 
		out[0][2] = in[0][2]; 
		out[0][3] = in[0][3]; 
		out[0][4] = in[0][4]; 
		out[0][5] = in[0][5]; 
		out[0][6] = in[0][6]; 
		out[0][7] = in[0][7]; 
		out[0][8] = in[0][8]; 
		out[0][9] = in[0][9]; 
		out[0][10] = in[0][10]; 
		out[0][11] = in[0][11]; 
		out[0][12] = in[0][12]; 
		out[0][13] = in[0][13]; 
		out[0][14] = in[0][14]; 
		out[0][15] = in[0][15]; 
	
		out[1][0] = in[1][0]; 
		out[1][1] = in[1][1]; 
		out[1][2] = in[1][2]; 
		out[1][3] = in[1][3]; 
		out[1][4] = in[1][4]; 
		out[1][5] = in[1][5]; 
		out[1][6] = in[1][6]; 
		out[1][7] = in[1][7]; 
		out[1][8] = in[1][8]; 
		out[1][9] = in[1][9]; 
		out[1][10] = in[1][10]; 
		out[1][11] = in[1][11]; 
		out[1][12] = in[1][12]; 
		out[1][13] = in[1][13]; 
		out[1][14] = in[1][14]; 
		out[1][15] = in[1][15]; 
	
		out[2][0] = in[2][0]; 
		out[2][1] = in[2][1]; 
		out[2][2] = in[2][2]; 
		out[2][3] = in[2][3]; 
		out[2][4] = in[2][4]; 
		out[2][5] = in[2][5]; 
		out[2][6] = in[2][6]; 
		out[2][7] = in[2][7]; 
		out[2][8] = in[2][8]; 
		out[2][9] = in[2][9]; 
		out[2][10] = in[2][10]; 
		out[2][11] = in[2][11]; 
		out[2][12] = in[2][12]; 
		out[2][13] = in[2][13]; 
		out[2][14] = in[2][14]; 
		out[2][15] = in[2][15]; 
	
		out[3][0] = in[3][0]; 
		out[3][1] = in[3][1]; 
		out[3][2] = in[3][2]; 
		out[3][3] = in[3][3]; 
		out[3][4] = in[3][4]; 
		out[3][5] = in[3][5]; 
		out[3][6] = in[3][6]; 
		out[3][7] = in[3][7]; 
		out[3][8] = in[3][8]; 
		out[3][9] = in[3][9]; 
		out[3][10] = in[3][10]; 
		out[3][11] = in[3][11]; 
		out[3][12] = in[3][12]; 
		out[3][13] = in[3][13]; 
		out[3][14] = in[3][14]; 
		out[3][15] = in[3][15]; 
	
		out[4][0] = in[4][0]; 
		out[4][1] = in[4][1]; 
		out[4][2] = in[4][2]; 
		out[4][3] = in[4][3]; 
		out[4][4] = in[4][4]; 
		out[4][5] = in[4][5]; 
		out[4][6] = in[4][6]; 
		out[4][7] = in[4][7]; 
		out[4][8] = in[4][8]; 
		out[4][9] = in[4][9]; 
		out[4][10] = in[4][10]; 
		out[4][11] = in[4][11]; 
		out[4][12] = in[4][12]; 
		out[4][13] = in[4][13]; 
		out[4][14] = in[4][14]; 
		out[4][15] = in[4][15]; 
	
	// }	
	return;
}
void rearrangeLayoutVx16B(unsigned int s, keyvalue_t in[EDGE_PACK_SIZE], keyvalue_t out[EDGE_PACK_SIZE]){
	#ifndef FPGA_IMPL
	checkoutofbounds("acts_kernel::rearrangeLayoutVx16B::ERROR 2925::", s, EDGE_PACK_SIZE, NAp, NAp, NAp);
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

unsigned int min_(unsigned int val1, unsigned int val2){
	#pragma HLS INLINE
	if(val1 < val2){ return val1; }
	else { return val2; }
}
void update_dramnumclockcycles(unsigned int _NUMCLOCKCYCLES_[2][32], unsigned int x, unsigned int clock_cycles){
	#ifdef _DEBUGMODE_CHECKS3
	_NUMCLOCKCYCLES_[0][x] += clock_cycles;
	#endif
}
void update_bramnumclockcycles(unsigned int _NUMCLOCKCYCLES_[2][32], unsigned int x, unsigned int clock_cycles){
	#ifdef _DEBUGMODE_CHECKS3
	_NUMCLOCKCYCLES_[0][x] += clock_cycles;
	// _NUMCLOCKCYCLES_[1][x] += clock_cycles;
	#endif
}

/////////////////////////////////////////

value_t process_func_pr(vprop_t uprop, value_t edgew){
	#pragma HLS INLINE 
	return uprop.prop + uprop.degree; // FIXME.
}
value_t reduce_func_pr(vprop_dest_t vprop, value_t res){
	#pragma HLS INLINE 
	return vprop.prop + res; // 6.11; // 
}

value_t process_func_spmv(vprop_t uprop, value_t edgew){ // see graphlily slides
	#pragma HLS INLINE 
	return uprop.prop * edgew;
}
value_t reduce_func_spmv(vprop_dest_t vprop, value_t res){
	#pragma HLS INLINE 
	return vprop.prop + res;
}

value_t process_func_hits(vprop_t uprop, value_t edgew){
	#pragma HLS INLINE 
	return uprop.prop;
}
value_t reduce_func_hits(vprop_dest_t vprop, value_t res){
	#pragma HLS INLINE 
	return vprop.prop + res;
}

value_t process_func_sssp(vprop_t uprop, value_t edgew){
	#pragma HLS INLINE 
	value_t res = uprop.prop + edgew;
	return res;
}
value_t reduce_func_sssp(vprop_dest_t vprop, value_t res){
	#pragma HLS INLINE 
	if(res < vprop.prop){ return res; } else { return vprop.prop; }
}

value_t process_funcG(vprop_t uprop, value_t edgew, unsigned int algo){
	#pragma HLS INLINE 
	#ifdef BAKE_PAGERANK_ALGORITHM_FUNCS
	return process_func_pr(uprop, edgew);	
	#endif 
	#ifdef BAKE_SPMV_ALGORITHM_FUNCS
	return process_func_spmv(uprop, edgew);
	#endif 
	#ifdef BAKE_HITS_ALGORITHM_FUNCS
	return process_func_hits(uprop, edgew);
	#endif 
}
value_t reduce_funcG(vprop_dest_t vprop, value_t res, unsigned int algo){
	#pragma HLS INLINE 
	#ifdef BAKE_PAGERANK_ALGORITHM_FUNCS
	return reduce_func_pr(vprop, res);	
	#endif 
	#ifdef BAKE_SPMV_ALGORITHM_FUNCS
	return reduce_func_spmv(vprop, res);
	#endif 
	#ifdef BAKE_HITS_ALGORITHM_FUNCS	
	return reduce_func_hits(vprop, res);	
	#endif 
}

value_t process_func(unsigned int GraphAlgo, vprop_t uprop, vprop_dest_t vprop, value_t edgew, unsigned int algo){
	#pragma HLS INLINE 
	if(GraphAlgo == PAGERANK){
		return process_func_pr(uprop, edgew);
	} else if(GraphAlgo == SPMV){
		return process_func_spmv(uprop, edgew);
	} else if(GraphAlgo == HITS){
		return process_func_hits(uprop, edgew);
	} else if(GraphAlgo == SSSP){
		return process_func_sssp(uprop, edgew);
	} else {
		return process_func_pr(uprop, edgew);
	}
}
value_t reduce_func(unsigned int GraphAlgo, vprop_dest_t vprop, value_t res, unsigned int algo){
	#pragma HLS INLINE 
	if(GraphAlgo == PAGERANK){ 
		return reduce_func_pr(vprop, res);
	} else if(GraphAlgo == SPMV){
		return reduce_func_spmv(vprop, res);
	} else if(GraphAlgo == HITS){
		return reduce_func_hits(vprop, res);
	} else if(GraphAlgo == SSSP){
		return reduce_func_sssp(vprop, res);
	} else {
		return reduce_func_pr(vprop, res);
	}
}		

void master_insertmany_vec(unsigned int offsets[NUM_VALID_PEs], unsigned int data[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE // FIXME_HARDWARE
	
	HBM_EDGESA0[offsets[0]].data[0] = data[0][0];
	HBM_EDGESA0[offsets[0]].data[1] = data[0][1];
	HBM_EDGESA0[offsets[0]].data[2] = data[0][2];
	HBM_EDGESA0[offsets[0]].data[3] = data[0][3];
	HBM_EDGESA0[offsets[0]].data[4] = data[0][4];
	HBM_EDGESA0[offsets[0]].data[5] = data[0][5];
	HBM_EDGESA0[offsets[0]].data[6] = data[0][6];
	HBM_EDGESA0[offsets[0]].data[7] = data[0][7];
	HBM_EDGESA0[offsets[0]].data[8] = data[0][8];
	HBM_EDGESA0[offsets[0]].data[9] = data[0][9];
	HBM_EDGESA0[offsets[0]].data[10] = data[0][10];
	HBM_EDGESA0[offsets[0]].data[11] = data[0][11];
	HBM_EDGESA0[offsets[0]].data[12] = data[0][12];
	HBM_EDGESA0[offsets[0]].data[13] = data[0][13];
	HBM_EDGESA0[offsets[0]].data[14] = data[0][14];
	HBM_EDGESA0[offsets[0]].data[15] = data[0][15];
	HBM_EDGESB0[offsets[0]].data[0] = data[0][16];
	HBM_EDGESB0[offsets[0]].data[1] = data[0][17];
	HBM_EDGESB0[offsets[0]].data[2] = data[0][18];
	HBM_EDGESB0[offsets[0]].data[3] = data[0][19];
	HBM_EDGESB0[offsets[0]].data[4] = data[0][20];
	HBM_EDGESB0[offsets[0]].data[5] = data[0][21];
	HBM_EDGESB0[offsets[0]].data[6] = data[0][22];
	HBM_EDGESB0[offsets[0]].data[7] = data[0][23];
	HBM_EDGESB0[offsets[0]].data[8] = data[0][24];
	HBM_EDGESB0[offsets[0]].data[9] = data[0][25];
	HBM_EDGESB0[offsets[0]].data[10] = data[0][26];
	HBM_EDGESB0[offsets[0]].data[11] = data[0][27];
	HBM_EDGESB0[offsets[0]].data[12] = data[0][28];
	HBM_EDGESB0[offsets[0]].data[13] = data[0][29];
	HBM_EDGESB0[offsets[0]].data[14] = data[0][30];
	HBM_EDGESB0[offsets[0]].data[15] = data[0][31];
	
	
	HBM_EDGESA1[offsets[1]].data[0] = data[1][0];
	HBM_EDGESA1[offsets[1]].data[1] = data[1][1];
	HBM_EDGESA1[offsets[1]].data[2] = data[1][2];
	HBM_EDGESA1[offsets[1]].data[3] = data[1][3];
	HBM_EDGESA1[offsets[1]].data[4] = data[1][4];
	HBM_EDGESA1[offsets[1]].data[5] = data[1][5];
	HBM_EDGESA1[offsets[1]].data[6] = data[1][6];
	HBM_EDGESA1[offsets[1]].data[7] = data[1][7];
	HBM_EDGESA1[offsets[1]].data[8] = data[1][8];
	HBM_EDGESA1[offsets[1]].data[9] = data[1][9];
	HBM_EDGESA1[offsets[1]].data[10] = data[1][10];
	HBM_EDGESA1[offsets[1]].data[11] = data[1][11];
	HBM_EDGESA1[offsets[1]].data[12] = data[1][12];
	HBM_EDGESA1[offsets[1]].data[13] = data[1][13];
	HBM_EDGESA1[offsets[1]].data[14] = data[1][14];
	HBM_EDGESA1[offsets[1]].data[15] = data[1][15];
	HBM_EDGESB1[offsets[1]].data[0] = data[1][16];
	HBM_EDGESB1[offsets[1]].data[1] = data[1][17];
	HBM_EDGESB1[offsets[1]].data[2] = data[1][18];
	HBM_EDGESB1[offsets[1]].data[3] = data[1][19];
	HBM_EDGESB1[offsets[1]].data[4] = data[1][20];
	HBM_EDGESB1[offsets[1]].data[5] = data[1][21];
	HBM_EDGESB1[offsets[1]].data[6] = data[1][22];
	HBM_EDGESB1[offsets[1]].data[7] = data[1][23];
	HBM_EDGESB1[offsets[1]].data[8] = data[1][24];
	HBM_EDGESB1[offsets[1]].data[9] = data[1][25];
	HBM_EDGESB1[offsets[1]].data[10] = data[1][26];
	HBM_EDGESB1[offsets[1]].data[11] = data[1][27];
	HBM_EDGESB1[offsets[1]].data[12] = data[1][28];
	HBM_EDGESB1[offsets[1]].data[13] = data[1][29];
	HBM_EDGESB1[offsets[1]].data[14] = data[1][30];
	HBM_EDGESB1[offsets[1]].data[15] = data[1][31];
	
	
	HBM_EDGESA2[offsets[2]].data[0] = data[2][0];
	HBM_EDGESA2[offsets[2]].data[1] = data[2][1];
	HBM_EDGESA2[offsets[2]].data[2] = data[2][2];
	HBM_EDGESA2[offsets[2]].data[3] = data[2][3];
	HBM_EDGESA2[offsets[2]].data[4] = data[2][4];
	HBM_EDGESA2[offsets[2]].data[5] = data[2][5];
	HBM_EDGESA2[offsets[2]].data[6] = data[2][6];
	HBM_EDGESA2[offsets[2]].data[7] = data[2][7];
	HBM_EDGESA2[offsets[2]].data[8] = data[2][8];
	HBM_EDGESA2[offsets[2]].data[9] = data[2][9];
	HBM_EDGESA2[offsets[2]].data[10] = data[2][10];
	HBM_EDGESA2[offsets[2]].data[11] = data[2][11];
	HBM_EDGESA2[offsets[2]].data[12] = data[2][12];
	HBM_EDGESA2[offsets[2]].data[13] = data[2][13];
	HBM_EDGESA2[offsets[2]].data[14] = data[2][14];
	HBM_EDGESA2[offsets[2]].data[15] = data[2][15];
	HBM_EDGESB2[offsets[2]].data[0] = data[2][16];
	HBM_EDGESB2[offsets[2]].data[1] = data[2][17];
	HBM_EDGESB2[offsets[2]].data[2] = data[2][18];
	HBM_EDGESB2[offsets[2]].data[3] = data[2][19];
	HBM_EDGESB2[offsets[2]].data[4] = data[2][20];
	HBM_EDGESB2[offsets[2]].data[5] = data[2][21];
	HBM_EDGESB2[offsets[2]].data[6] = data[2][22];
	HBM_EDGESB2[offsets[2]].data[7] = data[2][23];
	HBM_EDGESB2[offsets[2]].data[8] = data[2][24];
	HBM_EDGESB2[offsets[2]].data[9] = data[2][25];
	HBM_EDGESB2[offsets[2]].data[10] = data[2][26];
	HBM_EDGESB2[offsets[2]].data[11] = data[2][27];
	HBM_EDGESB2[offsets[2]].data[12] = data[2][28];
	HBM_EDGESB2[offsets[2]].data[13] = data[2][29];
	HBM_EDGESB2[offsets[2]].data[14] = data[2][30];
	HBM_EDGESB2[offsets[2]].data[15] = data[2][31];
	
	
	HBM_EDGESA3[offsets[3]].data[0] = data[3][0];
	HBM_EDGESA3[offsets[3]].data[1] = data[3][1];
	HBM_EDGESA3[offsets[3]].data[2] = data[3][2];
	HBM_EDGESA3[offsets[3]].data[3] = data[3][3];
	HBM_EDGESA3[offsets[3]].data[4] = data[3][4];
	HBM_EDGESA3[offsets[3]].data[5] = data[3][5];
	HBM_EDGESA3[offsets[3]].data[6] = data[3][6];
	HBM_EDGESA3[offsets[3]].data[7] = data[3][7];
	HBM_EDGESA3[offsets[3]].data[8] = data[3][8];
	HBM_EDGESA3[offsets[3]].data[9] = data[3][9];
	HBM_EDGESA3[offsets[3]].data[10] = data[3][10];
	HBM_EDGESA3[offsets[3]].data[11] = data[3][11];
	HBM_EDGESA3[offsets[3]].data[12] = data[3][12];
	HBM_EDGESA3[offsets[3]].data[13] = data[3][13];
	HBM_EDGESA3[offsets[3]].data[14] = data[3][14];
	HBM_EDGESA3[offsets[3]].data[15] = data[3][15];
	HBM_EDGESB3[offsets[3]].data[0] = data[3][16];
	HBM_EDGESB3[offsets[3]].data[1] = data[3][17];
	HBM_EDGESB3[offsets[3]].data[2] = data[3][18];
	HBM_EDGESB3[offsets[3]].data[3] = data[3][19];
	HBM_EDGESB3[offsets[3]].data[4] = data[3][20];
	HBM_EDGESB3[offsets[3]].data[5] = data[3][21];
	HBM_EDGESB3[offsets[3]].data[6] = data[3][22];
	HBM_EDGESB3[offsets[3]].data[7] = data[3][23];
	HBM_EDGESB3[offsets[3]].data[8] = data[3][24];
	HBM_EDGESB3[offsets[3]].data[9] = data[3][25];
	HBM_EDGESB3[offsets[3]].data[10] = data[3][26];
	HBM_EDGESB3[offsets[3]].data[11] = data[3][27];
	HBM_EDGESB3[offsets[3]].data[12] = data[3][28];
	HBM_EDGESB3[offsets[3]].data[13] = data[3][29];
	HBM_EDGESB3[offsets[3]].data[14] = data[3][30];
	HBM_EDGESB3[offsets[3]].data[15] = data[3][31];
	
	
	HBM_EDGESA4[offsets[4]].data[0] = data[4][0];
	HBM_EDGESA4[offsets[4]].data[1] = data[4][1];
	HBM_EDGESA4[offsets[4]].data[2] = data[4][2];
	HBM_EDGESA4[offsets[4]].data[3] = data[4][3];
	HBM_EDGESA4[offsets[4]].data[4] = data[4][4];
	HBM_EDGESA4[offsets[4]].data[5] = data[4][5];
	HBM_EDGESA4[offsets[4]].data[6] = data[4][6];
	HBM_EDGESA4[offsets[4]].data[7] = data[4][7];
	HBM_EDGESA4[offsets[4]].data[8] = data[4][8];
	HBM_EDGESA4[offsets[4]].data[9] = data[4][9];
	HBM_EDGESA4[offsets[4]].data[10] = data[4][10];
	HBM_EDGESA4[offsets[4]].data[11] = data[4][11];
	HBM_EDGESA4[offsets[4]].data[12] = data[4][12];
	HBM_EDGESA4[offsets[4]].data[13] = data[4][13];
	HBM_EDGESA4[offsets[4]].data[14] = data[4][14];
	HBM_EDGESA4[offsets[4]].data[15] = data[4][15];
	HBM_EDGESB4[offsets[4]].data[0] = data[4][16];
	HBM_EDGESB4[offsets[4]].data[1] = data[4][17];
	HBM_EDGESB4[offsets[4]].data[2] = data[4][18];
	HBM_EDGESB4[offsets[4]].data[3] = data[4][19];
	HBM_EDGESB4[offsets[4]].data[4] = data[4][20];
	HBM_EDGESB4[offsets[4]].data[5] = data[4][21];
	HBM_EDGESB4[offsets[4]].data[6] = data[4][22];
	HBM_EDGESB4[offsets[4]].data[7] = data[4][23];
	HBM_EDGESB4[offsets[4]].data[8] = data[4][24];
	HBM_EDGESB4[offsets[4]].data[9] = data[4][25];
	HBM_EDGESB4[offsets[4]].data[10] = data[4][26];
	HBM_EDGESB4[offsets[4]].data[11] = data[4][27];
	HBM_EDGESB4[offsets[4]].data[12] = data[4][28];
	HBM_EDGESB4[offsets[4]].data[13] = data[4][29];
	HBM_EDGESB4[offsets[4]].data[14] = data[4][30];
	HBM_EDGESB4[offsets[4]].data[15] = data[4][31];
	
	return;
}
void master_retrievemany_vec(unsigned int offsets[NUM_VALID_PEs], unsigned int data[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE // FIXME_HARDWARE
	data[0][0] = HBM_EDGESA0[offsets[0]].data[0];
	data[0][1] = HBM_EDGESA0[offsets[0]].data[1];
	data[0][2] = HBM_EDGESA0[offsets[0]].data[2];
	data[0][3] = HBM_EDGESA0[offsets[0]].data[3];
	data[0][4] = HBM_EDGESA0[offsets[0]].data[4];
	data[0][5] = HBM_EDGESA0[offsets[0]].data[5];
	data[0][6] = HBM_EDGESA0[offsets[0]].data[6];
	data[0][7] = HBM_EDGESA0[offsets[0]].data[7];
	data[0][8] = HBM_EDGESA0[offsets[0]].data[8];
	data[0][9] = HBM_EDGESA0[offsets[0]].data[9];
	data[0][10] = HBM_EDGESA0[offsets[0]].data[10];
	data[0][11] = HBM_EDGESA0[offsets[0]].data[11];
	data[0][12] = HBM_EDGESA0[offsets[0]].data[12];
	data[0][13] = HBM_EDGESA0[offsets[0]].data[13];
	data[0][14] = HBM_EDGESA0[offsets[0]].data[14];
	data[0][15] = HBM_EDGESA0[offsets[0]].data[15];
	data[0][16] = HBM_EDGESB0[offsets[0]].data[0];
	data[0][17] = HBM_EDGESB0[offsets[0]].data[1];
	data[0][18] = HBM_EDGESB0[offsets[0]].data[2];
	data[0][19] = HBM_EDGESB0[offsets[0]].data[3];
	data[0][20] = HBM_EDGESB0[offsets[0]].data[4];
	data[0][21] = HBM_EDGESB0[offsets[0]].data[5];
	data[0][22] = HBM_EDGESB0[offsets[0]].data[6];
	data[0][23] = HBM_EDGESB0[offsets[0]].data[7];
	data[0][24] = HBM_EDGESB0[offsets[0]].data[8];
	data[0][25] = HBM_EDGESB0[offsets[0]].data[9];
	data[0][26] = HBM_EDGESB0[offsets[0]].data[10];
	data[0][27] = HBM_EDGESB0[offsets[0]].data[11];
	data[0][28] = HBM_EDGESB0[offsets[0]].data[12];
	data[0][29] = HBM_EDGESB0[offsets[0]].data[13];
	data[0][30] = HBM_EDGESB0[offsets[0]].data[14];
	data[0][31] = HBM_EDGESB0[offsets[0]].data[15];
	data[1][0] = HBM_EDGESA1[offsets[1]].data[0];
	data[1][1] = HBM_EDGESA1[offsets[1]].data[1];
	data[1][2] = HBM_EDGESA1[offsets[1]].data[2];
	data[1][3] = HBM_EDGESA1[offsets[1]].data[3];
	data[1][4] = HBM_EDGESA1[offsets[1]].data[4];
	data[1][5] = HBM_EDGESA1[offsets[1]].data[5];
	data[1][6] = HBM_EDGESA1[offsets[1]].data[6];
	data[1][7] = HBM_EDGESA1[offsets[1]].data[7];
	data[1][8] = HBM_EDGESA1[offsets[1]].data[8];
	data[1][9] = HBM_EDGESA1[offsets[1]].data[9];
	data[1][10] = HBM_EDGESA1[offsets[1]].data[10];
	data[1][11] = HBM_EDGESA1[offsets[1]].data[11];
	data[1][12] = HBM_EDGESA1[offsets[1]].data[12];
	data[1][13] = HBM_EDGESA1[offsets[1]].data[13];
	data[1][14] = HBM_EDGESA1[offsets[1]].data[14];
	data[1][15] = HBM_EDGESA1[offsets[1]].data[15];
	data[1][16] = HBM_EDGESB1[offsets[1]].data[0];
	data[1][17] = HBM_EDGESB1[offsets[1]].data[1];
	data[1][18] = HBM_EDGESB1[offsets[1]].data[2];
	data[1][19] = HBM_EDGESB1[offsets[1]].data[3];
	data[1][20] = HBM_EDGESB1[offsets[1]].data[4];
	data[1][21] = HBM_EDGESB1[offsets[1]].data[5];
	data[1][22] = HBM_EDGESB1[offsets[1]].data[6];
	data[1][23] = HBM_EDGESB1[offsets[1]].data[7];
	data[1][24] = HBM_EDGESB1[offsets[1]].data[8];
	data[1][25] = HBM_EDGESB1[offsets[1]].data[9];
	data[1][26] = HBM_EDGESB1[offsets[1]].data[10];
	data[1][27] = HBM_EDGESB1[offsets[1]].data[11];
	data[1][28] = HBM_EDGESB1[offsets[1]].data[12];
	data[1][29] = HBM_EDGESB1[offsets[1]].data[13];
	data[1][30] = HBM_EDGESB1[offsets[1]].data[14];
	data[1][31] = HBM_EDGESB1[offsets[1]].data[15];
	data[2][0] = HBM_EDGESA2[offsets[2]].data[0];
	data[2][1] = HBM_EDGESA2[offsets[2]].data[1];
	data[2][2] = HBM_EDGESA2[offsets[2]].data[2];
	data[2][3] = HBM_EDGESA2[offsets[2]].data[3];
	data[2][4] = HBM_EDGESA2[offsets[2]].data[4];
	data[2][5] = HBM_EDGESA2[offsets[2]].data[5];
	data[2][6] = HBM_EDGESA2[offsets[2]].data[6];
	data[2][7] = HBM_EDGESA2[offsets[2]].data[7];
	data[2][8] = HBM_EDGESA2[offsets[2]].data[8];
	data[2][9] = HBM_EDGESA2[offsets[2]].data[9];
	data[2][10] = HBM_EDGESA2[offsets[2]].data[10];
	data[2][11] = HBM_EDGESA2[offsets[2]].data[11];
	data[2][12] = HBM_EDGESA2[offsets[2]].data[12];
	data[2][13] = HBM_EDGESA2[offsets[2]].data[13];
	data[2][14] = HBM_EDGESA2[offsets[2]].data[14];
	data[2][15] = HBM_EDGESA2[offsets[2]].data[15];
	data[2][16] = HBM_EDGESB2[offsets[2]].data[0];
	data[2][17] = HBM_EDGESB2[offsets[2]].data[1];
	data[2][18] = HBM_EDGESB2[offsets[2]].data[2];
	data[2][19] = HBM_EDGESB2[offsets[2]].data[3];
	data[2][20] = HBM_EDGESB2[offsets[2]].data[4];
	data[2][21] = HBM_EDGESB2[offsets[2]].data[5];
	data[2][22] = HBM_EDGESB2[offsets[2]].data[6];
	data[2][23] = HBM_EDGESB2[offsets[2]].data[7];
	data[2][24] = HBM_EDGESB2[offsets[2]].data[8];
	data[2][25] = HBM_EDGESB2[offsets[2]].data[9];
	data[2][26] = HBM_EDGESB2[offsets[2]].data[10];
	data[2][27] = HBM_EDGESB2[offsets[2]].data[11];
	data[2][28] = HBM_EDGESB2[offsets[2]].data[12];
	data[2][29] = HBM_EDGESB2[offsets[2]].data[13];
	data[2][30] = HBM_EDGESB2[offsets[2]].data[14];
	data[2][31] = HBM_EDGESB2[offsets[2]].data[15];
	data[3][0] = HBM_EDGESA3[offsets[3]].data[0];
	data[3][1] = HBM_EDGESA3[offsets[3]].data[1];
	data[3][2] = HBM_EDGESA3[offsets[3]].data[2];
	data[3][3] = HBM_EDGESA3[offsets[3]].data[3];
	data[3][4] = HBM_EDGESA3[offsets[3]].data[4];
	data[3][5] = HBM_EDGESA3[offsets[3]].data[5];
	data[3][6] = HBM_EDGESA3[offsets[3]].data[6];
	data[3][7] = HBM_EDGESA3[offsets[3]].data[7];
	data[3][8] = HBM_EDGESA3[offsets[3]].data[8];
	data[3][9] = HBM_EDGESA3[offsets[3]].data[9];
	data[3][10] = HBM_EDGESA3[offsets[3]].data[10];
	data[3][11] = HBM_EDGESA3[offsets[3]].data[11];
	data[3][12] = HBM_EDGESA3[offsets[3]].data[12];
	data[3][13] = HBM_EDGESA3[offsets[3]].data[13];
	data[3][14] = HBM_EDGESA3[offsets[3]].data[14];
	data[3][15] = HBM_EDGESA3[offsets[3]].data[15];
	data[3][16] = HBM_EDGESB3[offsets[3]].data[0];
	data[3][17] = HBM_EDGESB3[offsets[3]].data[1];
	data[3][18] = HBM_EDGESB3[offsets[3]].data[2];
	data[3][19] = HBM_EDGESB3[offsets[3]].data[3];
	data[3][20] = HBM_EDGESB3[offsets[3]].data[4];
	data[3][21] = HBM_EDGESB3[offsets[3]].data[5];
	data[3][22] = HBM_EDGESB3[offsets[3]].data[6];
	data[3][23] = HBM_EDGESB3[offsets[3]].data[7];
	data[3][24] = HBM_EDGESB3[offsets[3]].data[8];
	data[3][25] = HBM_EDGESB3[offsets[3]].data[9];
	data[3][26] = HBM_EDGESB3[offsets[3]].data[10];
	data[3][27] = HBM_EDGESB3[offsets[3]].data[11];
	data[3][28] = HBM_EDGESB3[offsets[3]].data[12];
	data[3][29] = HBM_EDGESB3[offsets[3]].data[13];
	data[3][30] = HBM_EDGESB3[offsets[3]].data[14];
	data[3][31] = HBM_EDGESB3[offsets[3]].data[15];
	data[4][0] = HBM_EDGESA4[offsets[4]].data[0];
	data[4][1] = HBM_EDGESA4[offsets[4]].data[1];
	data[4][2] = HBM_EDGESA4[offsets[4]].data[2];
	data[4][3] = HBM_EDGESA4[offsets[4]].data[3];
	data[4][4] = HBM_EDGESA4[offsets[4]].data[4];
	data[4][5] = HBM_EDGESA4[offsets[4]].data[5];
	data[4][6] = HBM_EDGESA4[offsets[4]].data[6];
	data[4][7] = HBM_EDGESA4[offsets[4]].data[7];
	data[4][8] = HBM_EDGESA4[offsets[4]].data[8];
	data[4][9] = HBM_EDGESA4[offsets[4]].data[9];
	data[4][10] = HBM_EDGESA4[offsets[4]].data[10];
	data[4][11] = HBM_EDGESA4[offsets[4]].data[11];
	data[4][12] = HBM_EDGESA4[offsets[4]].data[12];
	data[4][13] = HBM_EDGESA4[offsets[4]].data[13];
	data[4][14] = HBM_EDGESA4[offsets[4]].data[14];
	data[4][15] = HBM_EDGESA4[offsets[4]].data[15];
	data[4][16] = HBM_EDGESB4[offsets[4]].data[0];
	data[4][17] = HBM_EDGESB4[offsets[4]].data[1];
	data[4][18] = HBM_EDGESB4[offsets[4]].data[2];
	data[4][19] = HBM_EDGESB4[offsets[4]].data[3];
	data[4][20] = HBM_EDGESB4[offsets[4]].data[4];
	data[4][21] = HBM_EDGESB4[offsets[4]].data[5];
	data[4][22] = HBM_EDGESB4[offsets[4]].data[6];
	data[4][23] = HBM_EDGESB4[offsets[4]].data[7];
	data[4][24] = HBM_EDGESB4[offsets[4]].data[8];
	data[4][25] = HBM_EDGESB4[offsets[4]].data[9];
	data[4][26] = HBM_EDGESB4[offsets[4]].data[10];
	data[4][27] = HBM_EDGESB4[offsets[4]].data[11];
	data[4][28] = HBM_EDGESB4[offsets[4]].data[12];
	data[4][29] = HBM_EDGESB4[offsets[4]].data[13];
	data[4][30] = HBM_EDGESB4[offsets[4]].data[14];
	data[4][31] = HBM_EDGESB4[offsets[4]].data[15];
	return;
}

void master_insertmany_Avec(unsigned int offsets[NUM_VALID_PEs], unsigned int data[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE 
	
		HBM_EDGESA0[offsets[0]].data[0] = data[0][0];	// FIXME
		HBM_EDGESA0[offsets[0]].data[1] = data[0][1];	// FIXME
		HBM_EDGESA0[offsets[0]].data[2] = data[0][2];	// FIXME
		HBM_EDGESA0[offsets[0]].data[3] = data[0][3];	// FIXME
		HBM_EDGESA0[offsets[0]].data[4] = data[0][4];	// FIXME
		HBM_EDGESA0[offsets[0]].data[5] = data[0][5];	// FIXME
		HBM_EDGESA0[offsets[0]].data[6] = data[0][6];	// FIXME
		HBM_EDGESA0[offsets[0]].data[7] = data[0][7];	// FIXME
		HBM_EDGESA0[offsets[0]].data[8] = data[0][8];	// FIXME
		HBM_EDGESA0[offsets[0]].data[9] = data[0][9];	// FIXME
		HBM_EDGESA0[offsets[0]].data[10] = data[0][10];	// FIXME
		HBM_EDGESA0[offsets[0]].data[11] = data[0][11];	// FIXME
		HBM_EDGESA0[offsets[0]].data[12] = data[0][12];	// FIXME
		HBM_EDGESA0[offsets[0]].data[13] = data[0][13];	// FIXME
		HBM_EDGESA0[offsets[0]].data[14] = data[0][14];	// FIXME
		HBM_EDGESA0[offsets[0]].data[15] = data[0][15];	// FIXME
	
	
		HBM_EDGESA1[offsets[1]].data[0] = data[1][0];	// FIXME
		HBM_EDGESA1[offsets[1]].data[1] = data[1][1];	// FIXME
		HBM_EDGESA1[offsets[1]].data[2] = data[1][2];	// FIXME
		HBM_EDGESA1[offsets[1]].data[3] = data[1][3];	// FIXME
		HBM_EDGESA1[offsets[1]].data[4] = data[1][4];	// FIXME
		HBM_EDGESA1[offsets[1]].data[5] = data[1][5];	// FIXME
		HBM_EDGESA1[offsets[1]].data[6] = data[1][6];	// FIXME
		HBM_EDGESA1[offsets[1]].data[7] = data[1][7];	// FIXME
		HBM_EDGESA1[offsets[1]].data[8] = data[1][8];	// FIXME
		HBM_EDGESA1[offsets[1]].data[9] = data[1][9];	// FIXME
		HBM_EDGESA1[offsets[1]].data[10] = data[1][10];	// FIXME
		HBM_EDGESA1[offsets[1]].data[11] = data[1][11];	// FIXME
		HBM_EDGESA1[offsets[1]].data[12] = data[1][12];	// FIXME
		HBM_EDGESA1[offsets[1]].data[13] = data[1][13];	// FIXME
		HBM_EDGESA1[offsets[1]].data[14] = data[1][14];	// FIXME
		HBM_EDGESA1[offsets[1]].data[15] = data[1][15];	// FIXME
	
	
		HBM_EDGESA2[offsets[2]].data[0] = data[2][0];	// FIXME
		HBM_EDGESA2[offsets[2]].data[1] = data[2][1];	// FIXME
		HBM_EDGESA2[offsets[2]].data[2] = data[2][2];	// FIXME
		HBM_EDGESA2[offsets[2]].data[3] = data[2][3];	// FIXME
		HBM_EDGESA2[offsets[2]].data[4] = data[2][4];	// FIXME
		HBM_EDGESA2[offsets[2]].data[5] = data[2][5];	// FIXME
		HBM_EDGESA2[offsets[2]].data[6] = data[2][6];	// FIXME
		HBM_EDGESA2[offsets[2]].data[7] = data[2][7];	// FIXME
		HBM_EDGESA2[offsets[2]].data[8] = data[2][8];	// FIXME
		HBM_EDGESA2[offsets[2]].data[9] = data[2][9];	// FIXME
		HBM_EDGESA2[offsets[2]].data[10] = data[2][10];	// FIXME
		HBM_EDGESA2[offsets[2]].data[11] = data[2][11];	// FIXME
		HBM_EDGESA2[offsets[2]].data[12] = data[2][12];	// FIXME
		HBM_EDGESA2[offsets[2]].data[13] = data[2][13];	// FIXME
		HBM_EDGESA2[offsets[2]].data[14] = data[2][14];	// FIXME
		HBM_EDGESA2[offsets[2]].data[15] = data[2][15];	// FIXME
	
	
		HBM_EDGESA3[offsets[3]].data[0] = data[3][0];	// FIXME
		HBM_EDGESA3[offsets[3]].data[1] = data[3][1];	// FIXME
		HBM_EDGESA3[offsets[3]].data[2] = data[3][2];	// FIXME
		HBM_EDGESA3[offsets[3]].data[3] = data[3][3];	// FIXME
		HBM_EDGESA3[offsets[3]].data[4] = data[3][4];	// FIXME
		HBM_EDGESA3[offsets[3]].data[5] = data[3][5];	// FIXME
		HBM_EDGESA3[offsets[3]].data[6] = data[3][6];	// FIXME
		HBM_EDGESA3[offsets[3]].data[7] = data[3][7];	// FIXME
		HBM_EDGESA3[offsets[3]].data[8] = data[3][8];	// FIXME
		HBM_EDGESA3[offsets[3]].data[9] = data[3][9];	// FIXME
		HBM_EDGESA3[offsets[3]].data[10] = data[3][10];	// FIXME
		HBM_EDGESA3[offsets[3]].data[11] = data[3][11];	// FIXME
		HBM_EDGESA3[offsets[3]].data[12] = data[3][12];	// FIXME
		HBM_EDGESA3[offsets[3]].data[13] = data[3][13];	// FIXME
		HBM_EDGESA3[offsets[3]].data[14] = data[3][14];	// FIXME
		HBM_EDGESA3[offsets[3]].data[15] = data[3][15];	// FIXME
	
	
		HBM_EDGESA4[offsets[4]].data[0] = data[4][0];	// FIXME
		HBM_EDGESA4[offsets[4]].data[1] = data[4][1];	// FIXME
		HBM_EDGESA4[offsets[4]].data[2] = data[4][2];	// FIXME
		HBM_EDGESA4[offsets[4]].data[3] = data[4][3];	// FIXME
		HBM_EDGESA4[offsets[4]].data[4] = data[4][4];	// FIXME
		HBM_EDGESA4[offsets[4]].data[5] = data[4][5];	// FIXME
		HBM_EDGESA4[offsets[4]].data[6] = data[4][6];	// FIXME
		HBM_EDGESA4[offsets[4]].data[7] = data[4][7];	// FIXME
		HBM_EDGESA4[offsets[4]].data[8] = data[4][8];	// FIXME
		HBM_EDGESA4[offsets[4]].data[9] = data[4][9];	// FIXME
		HBM_EDGESA4[offsets[4]].data[10] = data[4][10];	// FIXME
		HBM_EDGESA4[offsets[4]].data[11] = data[4][11];	// FIXME
		HBM_EDGESA4[offsets[4]].data[12] = data[4][12];	// FIXME
		HBM_EDGESA4[offsets[4]].data[13] = data[4][13];	// FIXME
		HBM_EDGESA4[offsets[4]].data[14] = data[4][14];	// FIXME
		HBM_EDGESA4[offsets[4]].data[15] = data[4][15];	// FIXME
	
	
	return;
}
void master_insertmany_Bvec(unsigned int offsets[NUM_VALID_PEs], unsigned int data[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE 
	
		HBM_EDGESB0[offsets[0]].data[0] = data[0][16];
		HBM_EDGESB0[offsets[0]].data[1] = data[0][17];
		HBM_EDGESB0[offsets[0]].data[2] = data[0][18];
		HBM_EDGESB0[offsets[0]].data[3] = data[0][19];
		HBM_EDGESB0[offsets[0]].data[4] = data[0][20];
		HBM_EDGESB0[offsets[0]].data[5] = data[0][21];
		HBM_EDGESB0[offsets[0]].data[6] = data[0][22];
		HBM_EDGESB0[offsets[0]].data[7] = data[0][23];
		HBM_EDGESB0[offsets[0]].data[8] = data[0][24];
		HBM_EDGESB0[offsets[0]].data[9] = data[0][25];
		HBM_EDGESB0[offsets[0]].data[10] = data[0][26];
		HBM_EDGESB0[offsets[0]].data[11] = data[0][27];
		HBM_EDGESB0[offsets[0]].data[12] = data[0][28];
		HBM_EDGESB0[offsets[0]].data[13] = data[0][29];
		HBM_EDGESB0[offsets[0]].data[14] = data[0][30];
		HBM_EDGESB0[offsets[0]].data[15] = data[0][31];
	
	
		HBM_EDGESB1[offsets[1]].data[0] = data[1][16];
		HBM_EDGESB1[offsets[1]].data[1] = data[1][17];
		HBM_EDGESB1[offsets[1]].data[2] = data[1][18];
		HBM_EDGESB1[offsets[1]].data[3] = data[1][19];
		HBM_EDGESB1[offsets[1]].data[4] = data[1][20];
		HBM_EDGESB1[offsets[1]].data[5] = data[1][21];
		HBM_EDGESB1[offsets[1]].data[6] = data[1][22];
		HBM_EDGESB1[offsets[1]].data[7] = data[1][23];
		HBM_EDGESB1[offsets[1]].data[8] = data[1][24];
		HBM_EDGESB1[offsets[1]].data[9] = data[1][25];
		HBM_EDGESB1[offsets[1]].data[10] = data[1][26];
		HBM_EDGESB1[offsets[1]].data[11] = data[1][27];
		HBM_EDGESB1[offsets[1]].data[12] = data[1][28];
		HBM_EDGESB1[offsets[1]].data[13] = data[1][29];
		HBM_EDGESB1[offsets[1]].data[14] = data[1][30];
		HBM_EDGESB1[offsets[1]].data[15] = data[1][31];
	
	
		HBM_EDGESB2[offsets[2]].data[0] = data[2][16];
		HBM_EDGESB2[offsets[2]].data[1] = data[2][17];
		HBM_EDGESB2[offsets[2]].data[2] = data[2][18];
		HBM_EDGESB2[offsets[2]].data[3] = data[2][19];
		HBM_EDGESB2[offsets[2]].data[4] = data[2][20];
		HBM_EDGESB2[offsets[2]].data[5] = data[2][21];
		HBM_EDGESB2[offsets[2]].data[6] = data[2][22];
		HBM_EDGESB2[offsets[2]].data[7] = data[2][23];
		HBM_EDGESB2[offsets[2]].data[8] = data[2][24];
		HBM_EDGESB2[offsets[2]].data[9] = data[2][25];
		HBM_EDGESB2[offsets[2]].data[10] = data[2][26];
		HBM_EDGESB2[offsets[2]].data[11] = data[2][27];
		HBM_EDGESB2[offsets[2]].data[12] = data[2][28];
		HBM_EDGESB2[offsets[2]].data[13] = data[2][29];
		HBM_EDGESB2[offsets[2]].data[14] = data[2][30];
		HBM_EDGESB2[offsets[2]].data[15] = data[2][31];
	
	
		HBM_EDGESB3[offsets[3]].data[0] = data[3][16];
		HBM_EDGESB3[offsets[3]].data[1] = data[3][17];
		HBM_EDGESB3[offsets[3]].data[2] = data[3][18];
		HBM_EDGESB3[offsets[3]].data[3] = data[3][19];
		HBM_EDGESB3[offsets[3]].data[4] = data[3][20];
		HBM_EDGESB3[offsets[3]].data[5] = data[3][21];
		HBM_EDGESB3[offsets[3]].data[6] = data[3][22];
		HBM_EDGESB3[offsets[3]].data[7] = data[3][23];
		HBM_EDGESB3[offsets[3]].data[8] = data[3][24];
		HBM_EDGESB3[offsets[3]].data[9] = data[3][25];
		HBM_EDGESB3[offsets[3]].data[10] = data[3][26];
		HBM_EDGESB3[offsets[3]].data[11] = data[3][27];
		HBM_EDGESB3[offsets[3]].data[12] = data[3][28];
		HBM_EDGESB3[offsets[3]].data[13] = data[3][29];
		HBM_EDGESB3[offsets[3]].data[14] = data[3][30];
		HBM_EDGESB3[offsets[3]].data[15] = data[3][31];
	
	
		HBM_EDGESB4[offsets[4]].data[0] = data[4][16];
		HBM_EDGESB4[offsets[4]].data[1] = data[4][17];
		HBM_EDGESB4[offsets[4]].data[2] = data[4][18];
		HBM_EDGESB4[offsets[4]].data[3] = data[4][19];
		HBM_EDGESB4[offsets[4]].data[4] = data[4][20];
		HBM_EDGESB4[offsets[4]].data[5] = data[4][21];
		HBM_EDGESB4[offsets[4]].data[6] = data[4][22];
		HBM_EDGESB4[offsets[4]].data[7] = data[4][23];
		HBM_EDGESB4[offsets[4]].data[8] = data[4][24];
		HBM_EDGESB4[offsets[4]].data[9] = data[4][25];
		HBM_EDGESB4[offsets[4]].data[10] = data[4][26];
		HBM_EDGESB4[offsets[4]].data[11] = data[4][27];
		HBM_EDGESB4[offsets[4]].data[12] = data[4][28];
		HBM_EDGESB4[offsets[4]].data[13] = data[4][29];
		HBM_EDGESB4[offsets[4]].data[14] = data[4][30];
		HBM_EDGESB4[offsets[4]].data[15] = data[4][31];
	
	return;
}
void master_retrievemany_Avec(unsigned int offsets[NUM_VALID_PEs], unsigned int data[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE
	data[0][0] = HBM_EDGESA0[offsets[0]].data[0];
	data[0][1] = HBM_EDGESA0[offsets[0]].data[1];
	data[0][2] = HBM_EDGESA0[offsets[0]].data[2];
	data[0][3] = HBM_EDGESA0[offsets[0]].data[3];
	data[0][4] = HBM_EDGESA0[offsets[0]].data[4];
	data[0][5] = HBM_EDGESA0[offsets[0]].data[5];
	data[0][6] = HBM_EDGESA0[offsets[0]].data[6];
	data[0][7] = HBM_EDGESA0[offsets[0]].data[7];
	data[0][8] = HBM_EDGESA0[offsets[0]].data[8];
	data[0][9] = HBM_EDGESA0[offsets[0]].data[9];
	data[0][10] = HBM_EDGESA0[offsets[0]].data[10];
	data[0][11] = HBM_EDGESA0[offsets[0]].data[11];
	data[0][12] = HBM_EDGESA0[offsets[0]].data[12];
	data[0][13] = HBM_EDGESA0[offsets[0]].data[13];
	data[0][14] = HBM_EDGESA0[offsets[0]].data[14];
	data[0][15] = HBM_EDGESA0[offsets[0]].data[15];
	data[1][0] = HBM_EDGESA1[offsets[1]].data[0];
	data[1][1] = HBM_EDGESA1[offsets[1]].data[1];
	data[1][2] = HBM_EDGESA1[offsets[1]].data[2];
	data[1][3] = HBM_EDGESA1[offsets[1]].data[3];
	data[1][4] = HBM_EDGESA1[offsets[1]].data[4];
	data[1][5] = HBM_EDGESA1[offsets[1]].data[5];
	data[1][6] = HBM_EDGESA1[offsets[1]].data[6];
	data[1][7] = HBM_EDGESA1[offsets[1]].data[7];
	data[1][8] = HBM_EDGESA1[offsets[1]].data[8];
	data[1][9] = HBM_EDGESA1[offsets[1]].data[9];
	data[1][10] = HBM_EDGESA1[offsets[1]].data[10];
	data[1][11] = HBM_EDGESA1[offsets[1]].data[11];
	data[1][12] = HBM_EDGESA1[offsets[1]].data[12];
	data[1][13] = HBM_EDGESA1[offsets[1]].data[13];
	data[1][14] = HBM_EDGESA1[offsets[1]].data[14];
	data[1][15] = HBM_EDGESA1[offsets[1]].data[15];
	data[2][0] = HBM_EDGESA2[offsets[2]].data[0];
	data[2][1] = HBM_EDGESA2[offsets[2]].data[1];
	data[2][2] = HBM_EDGESA2[offsets[2]].data[2];
	data[2][3] = HBM_EDGESA2[offsets[2]].data[3];
	data[2][4] = HBM_EDGESA2[offsets[2]].data[4];
	data[2][5] = HBM_EDGESA2[offsets[2]].data[5];
	data[2][6] = HBM_EDGESA2[offsets[2]].data[6];
	data[2][7] = HBM_EDGESA2[offsets[2]].data[7];
	data[2][8] = HBM_EDGESA2[offsets[2]].data[8];
	data[2][9] = HBM_EDGESA2[offsets[2]].data[9];
	data[2][10] = HBM_EDGESA2[offsets[2]].data[10];
	data[2][11] = HBM_EDGESA2[offsets[2]].data[11];
	data[2][12] = HBM_EDGESA2[offsets[2]].data[12];
	data[2][13] = HBM_EDGESA2[offsets[2]].data[13];
	data[2][14] = HBM_EDGESA2[offsets[2]].data[14];
	data[2][15] = HBM_EDGESA2[offsets[2]].data[15];
	data[3][0] = HBM_EDGESA3[offsets[3]].data[0];
	data[3][1] = HBM_EDGESA3[offsets[3]].data[1];
	data[3][2] = HBM_EDGESA3[offsets[3]].data[2];
	data[3][3] = HBM_EDGESA3[offsets[3]].data[3];
	data[3][4] = HBM_EDGESA3[offsets[3]].data[4];
	data[3][5] = HBM_EDGESA3[offsets[3]].data[5];
	data[3][6] = HBM_EDGESA3[offsets[3]].data[6];
	data[3][7] = HBM_EDGESA3[offsets[3]].data[7];
	data[3][8] = HBM_EDGESA3[offsets[3]].data[8];
	data[3][9] = HBM_EDGESA3[offsets[3]].data[9];
	data[3][10] = HBM_EDGESA3[offsets[3]].data[10];
	data[3][11] = HBM_EDGESA3[offsets[3]].data[11];
	data[3][12] = HBM_EDGESA3[offsets[3]].data[12];
	data[3][13] = HBM_EDGESA3[offsets[3]].data[13];
	data[3][14] = HBM_EDGESA3[offsets[3]].data[14];
	data[3][15] = HBM_EDGESA3[offsets[3]].data[15];
	data[4][0] = HBM_EDGESA4[offsets[4]].data[0];
	data[4][1] = HBM_EDGESA4[offsets[4]].data[1];
	data[4][2] = HBM_EDGESA4[offsets[4]].data[2];
	data[4][3] = HBM_EDGESA4[offsets[4]].data[3];
	data[4][4] = HBM_EDGESA4[offsets[4]].data[4];
	data[4][5] = HBM_EDGESA4[offsets[4]].data[5];
	data[4][6] = HBM_EDGESA4[offsets[4]].data[6];
	data[4][7] = HBM_EDGESA4[offsets[4]].data[7];
	data[4][8] = HBM_EDGESA4[offsets[4]].data[8];
	data[4][9] = HBM_EDGESA4[offsets[4]].data[9];
	data[4][10] = HBM_EDGESA4[offsets[4]].data[10];
	data[4][11] = HBM_EDGESA4[offsets[4]].data[11];
	data[4][12] = HBM_EDGESA4[offsets[4]].data[12];
	data[4][13] = HBM_EDGESA4[offsets[4]].data[13];
	data[4][14] = HBM_EDGESA4[offsets[4]].data[14];
	data[4][15] = HBM_EDGESA4[offsets[4]].data[15];
	
	return;
}
void master_retrievemany_Bvec(unsigned int offsets[NUM_VALID_PEs], unsigned int data[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE 
 /////////////////
	data[0][16] = HBM_EDGESB0[offsets[0]].data[0];
 /////////////////
	data[0][17] = HBM_EDGESB0[offsets[0]].data[1];
 /////////////////
	data[0][18] = HBM_EDGESB0[offsets[0]].data[2];
 /////////////////
	data[0][19] = HBM_EDGESB0[offsets[0]].data[3];
 /////////////////
	data[0][20] = HBM_EDGESB0[offsets[0]].data[4];
 /////////////////
	data[0][21] = HBM_EDGESB0[offsets[0]].data[5];
 /////////////////
	data[0][22] = HBM_EDGESB0[offsets[0]].data[6];
 /////////////////
	data[0][23] = HBM_EDGESB0[offsets[0]].data[7];
 /////////////////
	data[0][24] = HBM_EDGESB0[offsets[0]].data[8];
 /////////////////
	data[0][25] = HBM_EDGESB0[offsets[0]].data[9];
 /////////////////
	data[0][26] = HBM_EDGESB0[offsets[0]].data[10];
 /////////////////
	data[0][27] = HBM_EDGESB0[offsets[0]].data[11];
 /////////////////
	data[0][28] = HBM_EDGESB0[offsets[0]].data[12];
 /////////////////
	data[0][29] = HBM_EDGESB0[offsets[0]].data[13];
 /////////////////
	data[0][30] = HBM_EDGESB0[offsets[0]].data[14];
 /////////////////
	data[0][31] = HBM_EDGESB0[offsets[0]].data[15];
	
 /////////////////
	data[1][16] = HBM_EDGESB1[offsets[1]].data[0];
 /////////////////
	data[1][17] = HBM_EDGESB1[offsets[1]].data[1];
 /////////////////
	data[1][18] = HBM_EDGESB1[offsets[1]].data[2];
 /////////////////
	data[1][19] = HBM_EDGESB1[offsets[1]].data[3];
 /////////////////
	data[1][20] = HBM_EDGESB1[offsets[1]].data[4];
 /////////////////
	data[1][21] = HBM_EDGESB1[offsets[1]].data[5];
 /////////////////
	data[1][22] = HBM_EDGESB1[offsets[1]].data[6];
 /////////////////
	data[1][23] = HBM_EDGESB1[offsets[1]].data[7];
 /////////////////
	data[1][24] = HBM_EDGESB1[offsets[1]].data[8];
 /////////////////
	data[1][25] = HBM_EDGESB1[offsets[1]].data[9];
 /////////////////
	data[1][26] = HBM_EDGESB1[offsets[1]].data[10];
 /////////////////
	data[1][27] = HBM_EDGESB1[offsets[1]].data[11];
 /////////////////
	data[1][28] = HBM_EDGESB1[offsets[1]].data[12];
 /////////////////
	data[1][29] = HBM_EDGESB1[offsets[1]].data[13];
 /////////////////
	data[1][30] = HBM_EDGESB1[offsets[1]].data[14];
 /////////////////
	data[1][31] = HBM_EDGESB1[offsets[1]].data[15];
	
 /////////////////
	data[2][16] = HBM_EDGESB2[offsets[2]].data[0];
 /////////////////
	data[2][17] = HBM_EDGESB2[offsets[2]].data[1];
 /////////////////
	data[2][18] = HBM_EDGESB2[offsets[2]].data[2];
 /////////////////
	data[2][19] = HBM_EDGESB2[offsets[2]].data[3];
 /////////////////
	data[2][20] = HBM_EDGESB2[offsets[2]].data[4];
 /////////////////
	data[2][21] = HBM_EDGESB2[offsets[2]].data[5];
 /////////////////
	data[2][22] = HBM_EDGESB2[offsets[2]].data[6];
 /////////////////
	data[2][23] = HBM_EDGESB2[offsets[2]].data[7];
 /////////////////
	data[2][24] = HBM_EDGESB2[offsets[2]].data[8];
 /////////////////
	data[2][25] = HBM_EDGESB2[offsets[2]].data[9];
 /////////////////
	data[2][26] = HBM_EDGESB2[offsets[2]].data[10];
 /////////////////
	data[2][27] = HBM_EDGESB2[offsets[2]].data[11];
 /////////////////
	data[2][28] = HBM_EDGESB2[offsets[2]].data[12];
 /////////////////
	data[2][29] = HBM_EDGESB2[offsets[2]].data[13];
 /////////////////
	data[2][30] = HBM_EDGESB2[offsets[2]].data[14];
 /////////////////
	data[2][31] = HBM_EDGESB2[offsets[2]].data[15];
	
 /////////////////
	data[3][16] = HBM_EDGESB3[offsets[3]].data[0];
 /////////////////
	data[3][17] = HBM_EDGESB3[offsets[3]].data[1];
 /////////////////
	data[3][18] = HBM_EDGESB3[offsets[3]].data[2];
 /////////////////
	data[3][19] = HBM_EDGESB3[offsets[3]].data[3];
 /////////////////
	data[3][20] = HBM_EDGESB3[offsets[3]].data[4];
 /////////////////
	data[3][21] = HBM_EDGESB3[offsets[3]].data[5];
 /////////////////
	data[3][22] = HBM_EDGESB3[offsets[3]].data[6];
 /////////////////
	data[3][23] = HBM_EDGESB3[offsets[3]].data[7];
 /////////////////
	data[3][24] = HBM_EDGESB3[offsets[3]].data[8];
 /////////////////
	data[3][25] = HBM_EDGESB3[offsets[3]].data[9];
 /////////////////
	data[3][26] = HBM_EDGESB3[offsets[3]].data[10];
 /////////////////
	data[3][27] = HBM_EDGESB3[offsets[3]].data[11];
 /////////////////
	data[3][28] = HBM_EDGESB3[offsets[3]].data[12];
 /////////////////
	data[3][29] = HBM_EDGESB3[offsets[3]].data[13];
 /////////////////
	data[3][30] = HBM_EDGESB3[offsets[3]].data[14];
 /////////////////
	data[3][31] = HBM_EDGESB3[offsets[3]].data[15];
	
 /////////////////
	data[4][16] = HBM_EDGESB4[offsets[4]].data[0];
 /////////////////
	data[4][17] = HBM_EDGESB4[offsets[4]].data[1];
 /////////////////
	data[4][18] = HBM_EDGESB4[offsets[4]].data[2];
 /////////////////
	data[4][19] = HBM_EDGESB4[offsets[4]].data[3];
 /////////////////
	data[4][20] = HBM_EDGESB4[offsets[4]].data[4];
 /////////////////
	data[4][21] = HBM_EDGESB4[offsets[4]].data[5];
 /////////////////
	data[4][22] = HBM_EDGESB4[offsets[4]].data[6];
 /////////////////
	data[4][23] = HBM_EDGESB4[offsets[4]].data[7];
 /////////////////
	data[4][24] = HBM_EDGESB4[offsets[4]].data[8];
 /////////////////
	data[4][25] = HBM_EDGESB4[offsets[4]].data[9];
 /////////////////
	data[4][26] = HBM_EDGESB4[offsets[4]].data[10];
 /////////////////
	data[4][27] = HBM_EDGESB4[offsets[4]].data[11];
 /////////////////
	data[4][28] = HBM_EDGESB4[offsets[4]].data[12];
 /////////////////
	data[4][29] = HBM_EDGESB4[offsets[4]].data[13];
 /////////////////
	data[4][30] = HBM_EDGESB4[offsets[4]].data[14];
 /////////////////
	data[4][31] = HBM_EDGESB4[offsets[4]].data[15];
	
	
	return;
}

//////////////////////////////////////////////////////////////////
void load_edgemaps(unsigned int index, unsigned int base_offset, map_t edge_map[NUM_VALID_PEs],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
#pragma HLS INLINE
	index = index * 2; // '*2' because data is dual: i.e., offset and size
	#ifdef _DEBUGMODE_CHECKS3
	checkoutofbounds("acts_kernel::ERROR 9601::", base_offset + (index / HBM_AXI_PACK_SIZE), ((1 << 28)/4)/16, index, HBM_CHANNEL_PACK_SIZE, NAp);
	#endif 
	edge_map[0].offset = HBM_EDGESA0[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)];
	edge_map[1].offset = HBM_EDGESA1[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)];
	edge_map[2].offset = HBM_EDGESA2[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)];
	edge_map[3].offset = HBM_EDGESA3[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)];
	edge_map[4].offset = HBM_EDGESA4[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)];
	edge_map[0].size = HBM_EDGESA0[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)];
	edge_map[1].size = HBM_EDGESA1[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)];
	edge_map[2].size = HBM_EDGESA2[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)];
	edge_map[3].size = HBM_EDGESA3[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)];
	edge_map[4].size = HBM_EDGESA4[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)];
	
	return;
}
void save_edgemaps(unsigned int index, unsigned int base_offset, map_t edge_map[NUM_VALID_PEs],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
#pragma HLS INLINE
	index = index * 2; // '*2' because data is dual: i.e., offset and size	
	#ifdef _DEBUGMODE_CHECKS3
	checkoutofbounds("acts_kernel::ERROR 9602::", base_offset + (index / HBM_AXI_PACK_SIZE), ((1 << 28)/4)/16, index, HBM_CHANNEL_PACK_SIZE, NAp);
	#endif 	
	HBM_EDGESA0[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)] = edge_map[0].offset;
	HBM_EDGESA1[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)] = edge_map[1].offset;
	HBM_EDGESA2[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)] = edge_map[2].offset;
	HBM_EDGESA3[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)] = edge_map[3].offset;
	HBM_EDGESA4[base_offset + (index / HBM_AXI_PACK_SIZE)].data[(index % HBM_AXI_PACK_SIZE)] = edge_map[4].offset;
	HBM_EDGESA0[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)] = edge_map[0].size;
	HBM_EDGESA1[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)] = edge_map[1].size;
	HBM_EDGESA2[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)] = edge_map[2].size;
	HBM_EDGESA3[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)] = edge_map[3].size;
	HBM_EDGESA4[base_offset + ((index + 1) / HBM_AXI_PACK_SIZE)].data[((index + 1) % HBM_AXI_PACK_SIZE)] = edge_map[4].size;
	
	return;
}

map_t load_vupdate_map(unsigned int offset, unsigned int index,  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
#pragma HLS INLINE
	map_t data;
	data.offset = HBM_EDGESA0[offset + index].data[0];
	data.size = HBM_EDGESA0[offset + index].data[1];
	return data; 
}
void save_vupdate_map(unsigned int offset, unsigned int index, map_t data,  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
#pragma HLS INLINE
 
	HBM_EDGESA0[offset + index].data[0] = data.offset;
	HBM_EDGESA0[offset + index].data[1] = data.size;
 
	HBM_EDGESA0[offset + index].data[2] = data.offset;
	HBM_EDGESA0[offset + index].data[3] = data.size;
 
	HBM_EDGESA0[offset + index].data[4] = data.offset;
	HBM_EDGESA0[offset + index].data[5] = data.size;
 
	HBM_EDGESA0[offset + index].data[6] = data.offset;
	HBM_EDGESA0[offset + index].data[7] = data.size;
 
	HBM_EDGESA0[offset + index].data[8] = data.offset;
	HBM_EDGESA0[offset + index].data[9] = data.size;
 
	HBM_EDGESA0[offset + index].data[10] = data.offset;
	HBM_EDGESA0[offset + index].data[11] = data.size;
 
	HBM_EDGESA0[offset + index].data[12] = data.offset;
	HBM_EDGESA0[offset + index].data[13] = data.size;
 
	HBM_EDGESA0[offset + index].data[14] = data.offset;
	HBM_EDGESA0[offset + index].data[15] = data.size;
	
	return;
}

////////////////////////////////////////////////////////////////
unsigned int dretrieve_globalparams(unsigned int base_offset__, unsigned int index, HBM_channelAXI_t * HBM_EDGESA, HBM_channelAXI_t * HBM_EDGESB, unsigned int inst){
	#pragma HLS INLINE 
	return HBM_EDGESA[base_offset__ + index].data[0];
}

void dretrievemany_edges(unsigned int base_offset__, unsigned int offsets[NUM_VALID_PEs], unsigned int t, edge3_vec_dt edges[NUM_VALID_PEs],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE 
	#ifdef _DEBUGMODE_CHECKS3
	checkoutofbounds("acts_kernel::ERROR 71723::", offsets[0], HBM_CHANNEL_SIZE, NAp, NAp, NAp);							
	#endif 

	#pragma HLS INLINE 
	unsigned int data_[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE]; // NEW
	#pragma HLS ARRAY_PARTITION variable=data_ complete dim=0
	unsigned int offsets_[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=offsets_ complete
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		offsets_[n] = base_offset__ + offsets[n] + t;
	}
	master_retrievemany_vec(offsets_, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
	#pragma HLS UNROLL
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			edges[n].data[v].srcvid = data_[n][2*v];
			edges[n].data[v].dstvid = data_[n][2*v+1];
		}
	}
	return;
}
void dinsertmany_edgesdram(unsigned int offset__, edge3_vec_dt data[NUM_VALID_PEs],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE 
	unsigned int data_[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE]; 
	#pragma HLS ARRAY_PARTITION variable=data_ complete dim=0
	unsigned int offsets[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=offsets complete
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		offsets[n] = offset__;
	}
	
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
	#pragma HLS UNROLL
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			data_[n][2*v] = data[n].data[v].srcvid;
			data_[n][2*v+1] = data[n].data[v].dstvid;
		}
	}
	#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
	master_insertmany_vec(offsets, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
	#else 
	master_insertmany_Bvec(offsets, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); 
	#endif 
	return;
}

void dinsertmany_updatesdram(unsigned int offset__, keyvalue_t data[NUM_VALID_PEs][EDGE_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE 
	unsigned int data_[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE]; 
	#pragma HLS ARRAY_PARTITION variable=data_ complete dim=0
	unsigned int offsets[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=offsets complete
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		offsets[n] = offset__;
	}
	
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
	#pragma HLS UNROLL
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			data_[n][2*v] = data[n][v].key;
			data_[n][2*v+1] = data[n][v].value;
		}
	}
	#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
	master_insertmany_vec(offsets, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); // NEWCHANGE.
	#else 
	master_insertmany_Bvec(offsets, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
	#endif 
	return;
}
void dretrievemany_udatesdram(unsigned int offset__, uint512_vec_dt data[NUM_VALID_PEs],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE 
	#ifdef _DEBUGMODE_CHECKS3
	unsigned int wwsize = globalparams_debug[GLOBALPARAMSCODE__WWSIZE__VERTEXUPDATES];
	#endif 

	unsigned int data_[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE]; 
	#pragma HLS ARRAY_PARTITION variable=data_ complete dim=0
	unsigned int offsets[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=offsets complete
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		offsets[n] = offset__;
	}
	
	master_retrievemany_vec(offsets, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); // FIXME.
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
		#pragma HLS UNROLL
			data[n].data[v].key = data_[n][2*v];	
			data[n].data[v].value = data_[n][2*v+1];
		}
	}	
	return; 
}

void dinsertmany_vdatadram(unsigned int offset__, unsigned int t, vprop_dest_t datas[NUM_VALID_PEs][EDGE_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE // FIXME_HARDWARE
	#ifdef _DEBUGMODE_CHECKS3
	unsigned int wwsize = globalparams_debug[GLOBALPARAMSCODE__WWSIZE__VDATAS];
	// checkoutofbounds("acts_kernel::ERROR 711b::", offset__, globalparams_debug[GLOBALPARAMSCODE__BASEOFFSET__CFRONTIERSTMP], NAp, NAp, NAp);
	checkoutofbounds("acts_kernel::ERROR 711b::", offset__, HBM_CHANNEL_SIZE, NAp, NAp, NAp);
	#endif 
	
	unsigned int data_[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE]; // NEW
	#pragma HLS ARRAY_PARTITION variable=data_ complete dim=0
	unsigned int offsets_[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=offsets_ complete
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		offsets_[n] = offset__ + t;
	}
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
		#pragma HLS UNROLL
			data_[n][2*v] = datas[n][v].prop;
			data_[n][2*v+1] = datas[n][v].gvid;
		}
	}
	master_insertmany_vec(offsets_, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
	return;
}
void dretrievemany_vdatadram(unsigned int offset__, unsigned int t, vprop_dest_t datas[NUM_VALID_PEs][EDGE_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE 
	#ifdef _DEBUGMODE_CHECKS3
	unsigned int wwsize = globalparams_debug[GLOBALPARAMSCODE__WWSIZE__VDATAS];
	checkoutofbounds("acts_kernel::ERROR 712::", offset__, HBM_CHANNEL_SIZE, NAp, NAp, NAp);
	#endif 
	
	unsigned int data_[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE]; 
	#pragma HLS ARRAY_PARTITION variable=data_ complete dim=0
	unsigned int offsets_[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=offsets_ complete
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		offsets_[n] = offset__ + t;
	}
	master_retrievemany_vec(offsets_, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
	#pragma HLS UNROLL
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			datas[n][v].prop = data_[n][2*v];
			datas[n][v].gvid = data_[n][2*v+1];
		}
	}
	return;
}

void dinsertmany_nfrontierdram(unsigned int offsets[NUM_VALID_PEs], unsigned int t, keyvalue_t datas[NUM_VALID_PEs][EDGE_PACK_SIZE], bool ens[NUM_VALID_PEs][EDGE_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE
	#ifdef _DEBUGMODE_CHECKS3
	checkoutofbounds("acts_kernel::ERROR 71564a::", offsets[0], HBM_CHANNEL_SIZE, NAp, NAp, NAp);
	#endif 
	
	unsigned int data_[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE];
	#pragma HLS ARRAY_PARTITION variable=data_ complete dim=0
	unsigned int offsets_[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=offsets_ complete
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		offsets_[n] = offsets[n];
	}
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL	
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
		#pragma HLS UNROLL
			data_[n][2*v] = datas[n][v].key;
			data_[n][2*v+1] = datas[n][v].value;
		}
	}
	master_insertmany_vec(offsets_, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
}
void dretrievemany_cfrontierdram_tmp(unsigned int offset__, unsigned int t, keyvalue_t datas[NUM_VALID_PEs][EDGE_PACK_SIZE],  HBM_channelAXI_t * HBM_EDGESA0, HBM_channelAXI_t * HBM_EDGESB0, HBM_channelAXI_t * HBM_EDGESA1, HBM_channelAXI_t * HBM_EDGESB1, HBM_channelAXI_t * HBM_EDGESA2, HBM_channelAXI_t * HBM_EDGESB2, HBM_channelAXI_t * HBM_EDGESA3, HBM_channelAXI_t * HBM_EDGESB3, HBM_channelAXI_t * HBM_EDGESA4, HBM_channelAXI_t * HBM_EDGESB4){
	#pragma HLS INLINE 
	#ifdef _DEBUGMODE_CHECKS3
	unsigned int wwsize = globalparams_debug[GLOBALPARAMSCODE__WWSIZE__CFRONTIERSTMP];
	checkoutofbounds("acts_kernel::ERROR 714a::", offset__ + t, HBM_CHANNEL_SIZE, NAp, NAp, NAp);
	#endif 
	
	unsigned int data_[NUM_VALID_PEs][HBM_CHANNEL_PACK_SIZE];
	#pragma HLS ARRAY_PARTITION variable=data_ complete dim=0
	unsigned int offsets_[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=offsets_ complete
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		offsets_[n] = offset__ + t;
	}
	master_retrievemany_vec(offsets_, data_,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
	#pragma HLS UNROLL
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
		#pragma HLS UNROLL
			datas[n][v].key = data_[n][2*v];
			datas[n][v].value = data_[n][2*v+1];
		}
	}
	return; 
}


	

unsigned int owner_fpga(unsigned int dstvid){
	return 0; // FIXME.
}

unsigned int owner_vpartition(unsigned int dstvid, unsigned int msg){
	// return 0;
	return msg; // FIXME.
}

extern "C" {	
MY_IFDEF_TOPLEVELFUNC(){	
#pragma HLS INTERFACE m_axi port = HBM_EDGESA0 offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = HBM_EDGESB0 offset = slave bundle = gmem1
#pragma HLS INTERFACE m_axi port = HBM_EDGESA1 offset = slave bundle = gmem2
#pragma HLS INTERFACE m_axi port = HBM_EDGESB1 offset = slave bundle = gmem3
#pragma HLS INTERFACE m_axi port = HBM_EDGESA2 offset = slave bundle = gmem4
#pragma HLS INTERFACE m_axi port = HBM_EDGESB2 offset = slave bundle = gmem5
#pragma HLS INTERFACE m_axi port = HBM_EDGESA3 offset = slave bundle = gmem6
#pragma HLS INTERFACE m_axi port = HBM_EDGESB3 offset = slave bundle = gmem7
#pragma HLS INTERFACE m_axi port = HBM_EDGESA4 offset = slave bundle = gmem8
#pragma HLS INTERFACE m_axi port = HBM_EDGESB4 offset = slave bundle = gmem9
#pragma HLS INTERFACE m_axi port = HBM_SRCA0 offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = HBM_SRCB0 offset = slave bundle = gmem1
#pragma HLS INTERFACE m_axi port = HBM_SRCA1 offset = slave bundle = gmem2
#pragma HLS INTERFACE m_axi port = HBM_SRCB1 offset = slave bundle = gmem3
#pragma HLS INTERFACE m_axi port = HBM_SRCA2 offset = slave bundle = gmem4
#pragma HLS INTERFACE m_axi port = HBM_SRCB2 offset = slave bundle = gmem5
#pragma HLS INTERFACE m_axi port = HBM_SRCA3 offset = slave bundle = gmem6
#pragma HLS INTERFACE m_axi port = HBM_SRCB3 offset = slave bundle = gmem7
#pragma HLS INTERFACE m_axi port = HBM_SRCA4 offset = slave bundle = gmem8
#pragma HLS INTERFACE m_axi port = HBM_SRCB4 offset = slave bundle = gmem9

#pragma HLS INTERFACE s_axilite port = HBM_EDGESA0
#pragma HLS INTERFACE s_axilite port = HBM_EDGESB0
#pragma HLS INTERFACE s_axilite port = HBM_EDGESA1
#pragma HLS INTERFACE s_axilite port = HBM_EDGESB1
#pragma HLS INTERFACE s_axilite port = HBM_EDGESA2
#pragma HLS INTERFACE s_axilite port = HBM_EDGESB2
#pragma HLS INTERFACE s_axilite port = HBM_EDGESA3
#pragma HLS INTERFACE s_axilite port = HBM_EDGESB3
#pragma HLS INTERFACE s_axilite port = HBM_EDGESA4
#pragma HLS INTERFACE s_axilite port = HBM_EDGESB4
#pragma HLS INTERFACE s_axilite port = HBM_SRCA0
#pragma HLS INTERFACE s_axilite port = HBM_SRCB0
#pragma HLS INTERFACE s_axilite port = HBM_SRCA1
#pragma HLS INTERFACE s_axilite port = HBM_SRCB1
#pragma HLS INTERFACE s_axilite port = HBM_SRCA2
#pragma HLS INTERFACE s_axilite port = HBM_SRCB2
#pragma HLS INTERFACE s_axilite port = HBM_SRCA3
#pragma HLS INTERFACE s_axilite port = HBM_SRCB3
#pragma HLS INTERFACE s_axilite port = HBM_SRCA4
#pragma HLS INTERFACE s_axilite port = HBM_SRCB4

#pragma HLS INTERFACE s_axilite port = fpga
#pragma HLS INTERFACE s_axilite port = module
#pragma HLS INTERFACE s_axilite port = graph_iteration
#pragma HLS INTERFACE s_axilite port = start_pu 
#pragma HLS INTERFACE s_axilite port = size_pu
#pragma HLS INTERFACE s_axilite port = skip_pu
#pragma HLS INTERFACE s_axilite port = start_pv_fpga
#pragma HLS INTERFACE s_axilite port = start_pv
#pragma HLS INTERFACE s_axilite port = size_pv
#pragma HLS INTERFACE s_axilite port = start_llpset
#pragma HLS INTERFACE s_axilite port = size_llpset 
#pragma HLS INTERFACE s_axilite port = start_llpid
#pragma HLS INTERFACE s_axilite port = size_llpid
#pragma HLS INTERFACE s_axilite port = start_gv_fpga
#pragma HLS INTERFACE s_axilite port = start_gv
#pragma HLS INTERFACE s_axilite port = size_gv
#pragma HLS INTERFACE s_axilite port = id_process
#pragma HLS INTERFACE s_axilite port = id_import
#pragma HLS INTERFACE s_axilite port = id_export
#pragma HLS INTERFACE s_axilite port = size_import_export
#pragma HLS INTERFACE s_axilite port = status
#pragma HLS INTERFACE s_axilite port = numfpgas
#pragma HLS INTERFACE s_axilite port = command
#pragma HLS INTERFACE s_axilite port = mask0
#pragma HLS INTERFACE s_axilite port = mask1
#pragma HLS INTERFACE s_axilite port = mask2
#pragma HLS INTERFACE s_axilite port = mask3
#pragma HLS INTERFACE s_axilite port = mask4
#pragma HLS INTERFACE s_axilite port = mask5
#pragma HLS INTERFACE s_axilite port = mask6
#pragma HLS INTERFACE s_axilite port = mask7
#pragma HLS INTERFACE s_axilite port = return

	#ifdef _DEBUGMODE_KERNELPRINTS4	
	unsigned int _K0 = 1; // <lowerlimit:1, upperlimit:_GF_BATCH_SIZE>
	unsigned int _K1 = 2; // NUM_FPGAS // <lowerlimit:1, upperlimit:NUM_FPGAS*>
	unsigned int _AU_BATCH_SIZE = 2; 
	unsigned int _GF_BATCH_SIZE = _AU_BATCH_SIZE * universalparams.GLOBAL_NUM_PEs_; // 6 (i.e., 24 upartitions)
	unsigned int _IMPORT_BATCH_SIZE = (_GF_BATCH_SIZE / _K0); // 6
	unsigned int _PE_BATCH_SIZE = _IMPORT_BATCH_SIZE; // 6
	unsigned int _EXPORT_BATCH_SIZE = _IMPORT_BATCH_SIZE; // (_GF_BATCH_SIZE * _K1); // 24
	unsigned int _IMPORT_EXPORT_GRANULARITY_VECSIZE = 8184;
	#endif 
	
	#ifdef _DEBUGMODE_KERNELPRINTS4	
	if(start_pu != NAp && fpga < num_prints) { cout<<"acts started [processing stage]: fpga: "<<fpga<<", start_pu: "<<start_pu<<", size_pu: "<<size_pu<<", start_pv_fpga: "<<start_pv_fpga<<", start_pv: "<<start_pv<<", size_pv: "<<size_pv<<", start_gv_fpga: "<<start_gv_fpga<<", start_gv: "<<start_gv<<", size_gv: "<<size_gv<<endl; }
	if(start_pv != NAp && fpga < num_prints) { cout<<"acts started [applying stage]: fpga: "<<fpga<<", start_pu: "<<start_pu<<", size_pu: "<<size_pu<<", start_pv_fpga: "<<start_pv_fpga<<", start_pv: "<<start_pv<<", size_pv: "<<size_pv<<", start_gv_fpga: "<<start_gv_fpga<<", start_gv: "<<start_gv<<", size_gv: "<<size_gv<<endl; }
	if(start_gv != NAp && fpga < num_prints) { cout<<"acts started [gathering stage]: fpga: "<<fpga<<", start_pu: "<<start_pu<<", size_pu: "<<size_pu<<", start_pv_fpga: "<<start_pv_fpga<<", start_pv: "<<start_pv<<", size_pv: "<<size_pv<<", start_gv_fpga: "<<start_gv_fpga<<", start_gv: "<<start_gv<<", size_gv: "<<size_gv<<endl; }			
	if(id_import != INVALID_IOBUFFER_ID && fpga < num_prints){ cout << "### acts started [importing stage]: --> importing upartition: "<<id_import<<" to "<<id_import + _IMPORT_BATCH_SIZE<<"..." <<endl; }
	if(id_export != INVALID_IOBUFFER_ID && fpga < num_prints){ cout << "### acts started [exporting stage]: <-- exporting vpartition: "<<id_export<<" to "<<id_export + _EXPORT_BATCH_SIZE<<"  [FPGAs "; for(unsigned int n=0; n<numfpgas; n++){ cout<<n<<", "; } cout<<"]..." <<endl; }				
	#endif 

	// commands from host 
	action_t action;
	action.fpga = fpga; 
	action.module = module; 
	action.graph_iteration = graph_iteration; 
	action.start_pu = start_pu; 
	action.size_pu = size_pu; 
	action.skip_pu = skip_pu; 
	action.start_pv_fpga = start_pv_fpga;
	action.start_pv = start_pv;
	action.size_pv = size_pv; 
	action.start_llpset = start_llpset; 
	action.size_llpset = size_llpset; 
	action.start_llpid = start_llpid; 
	action.size_llpid = size_llpid; 
	action.start_gv_fpga = start_gv_fpga;
	action.start_gv = start_gv; 
	action.size_gv = size_gv;
	action.id_process = id_process;
	action.id_import = id_import;
	action.id_export = id_export;
	action.size_import_export = size_import_export;
	action.status = status;
	action.numfpgas = numfpgas; 
	action.command = command;
	
	unsigned int mask[8]; mask[0] = mask0; mask[1] = mask1; mask[2] = mask2; mask[3] = mask3; mask[4] = mask4; mask[5] = mask5; mask[6] = mask6; mask[7] = mask7;
	
	#ifdef _DEBUGMODE_KERNELPRINTS//4					
	if(action.id_import != INVALID_IOBUFFER_ID && action.fpga < num_prints){ cout<< TIMINGRESULTSCOLOR << "--> importing upartition: "<<action.id_import<<" to "<<action.id_import + IMPORT_BATCH_SIZE_<<"..."<< RESET <<endl; }
	if(action.id_export != INVALID_IOBUFFER_ID && action.fpga < num_prints){ cout<< TIMINGRESULTSCOLOR << "<-- exporting vpartition: "<<action.id_export<<" to "<<action.id_export + EXPORT_BATCH_SIZE_<<"  [FPGAs "; for(unsigned int n=0; n<action.numfpgas; n++){ cout<<n<<", "; } cout<<"]..."<< RESET <<endl; }				
	#endif 
	
	if(action.id_process == INVALID_IOBUFFER_ID && action.module != APPLY_UPDATES_MODULE && action.module != GATHER_FRONTIERS_MODULE){ 
		#ifdef ___RUNNING_FPGA_SYNTHESIS___
		return; 
		#else 
		return 0;	
		#endif 
	}
	
	#ifdef _DEBUGMODE_KERNELPRINTS//4				
	if(action.module == PROCESS_EDGES_MODULE && action.fpga < num_prints){ cout<< TIMINGRESULTSCOLOR << "--- processing upartition: "<<action.id_process<<" to "<<action.id_process + PE_BATCH_SIZE - 1<<" "<< RESET <<endl; }
	else if(action.module == APPLY_UPDATES_MODULE && action.fpga < num_prints){ cout<< TIMINGRESULTSCOLOR << "--- applying vpartition: "<<action.start_pv<<" to "<<action.start_pv + AU_BATCH_SIZE - 1<<" [FPGAs "; for(unsigned int n=0; n<action.numfpgas; n++){ cout<<n<<", "; } cout<<"]..."<< RESET <<endl; }
	else if(action.module == GATHER_FRONTIERS_MODULE && action.fpga < num_prints){ cout<< TIMINGRESULTSCOLOR << "--- gathering frontiers for upartition: "<<action.start_gv<<" to "<<action.start_gv + GF_BATCH_SIZE - 1<<" [target FPGAs "; for(unsigned int n=0; n<action.numfpgas; n++){ cout<<n<<", "; } cout<<"]"; cout<<"..."<< RESET <<endl; }
	else { if(action.module != ALL_MODULES && action.fpga < num_prints){ cout<<"acts: ERROR 232. EXITING..."<<endl; exit(EXIT_FAILURE); }}
	#endif 
	
	// convert
	if(action.id_import != INVALID_IOBUFFER_ID){ action.id_import = (action.id_import * action.numfpgas) + fpga; }	
	
	// declarations
// declaration of BRAM variables
#ifdef ___RUNNING_FPGA_SYNTHESIS___ // FPGA_IMPL
	keyvalue_t nfrontier_buffer[NUM_VALID_PEs][EDGE_PACK_SIZE][MAX_VDATA_SUBPARTITION_VECSIZE]; 
	#pragma HLS ARRAY_PARTITION variable=nfrontier_buffer complete dim=1
	#pragma HLS ARRAY_PARTITION variable=nfrontier_buffer complete dim=2
	vtr_t vptr_buffer[NUM_VALID_PEs][VPTR_BUFFER_SIZE];
	#pragma HLS ARRAY_PARTITION variable=vptr_buffer complete dim=1
	edge3_type edges_buffer[NUM_VALID_PEs][EDGE_PACK_SIZE][EDGE_BUFFER_SIZE];	
	#pragma HLS ARRAY_PARTITION variable=edges_buffer complete dim=1	
	#pragma HLS ARRAY_PARTITION variable=edges_buffer complete dim=2	
	keyvalue_t updates_buffer2[NUM_VALID_PEs][UPDATES_BUFFER_PACK_SIZE][UPDATES_BUFFER_SIZE];		
	#pragma HLS ARRAY_PARTITION variable=updates_buffer2 complete dim=1
	#pragma HLS ARRAY_PARTITION variable=updates_buffer2 complete dim=2

	/* keyvalue_t URAM_frontiers[EDGE_PACK_SIZE][MAXVALID_APPLYPARTITION_VECSIZE]; 
	// #pragma HLS resource variable=URAM_frontiers core=XPM_MEMORY uram
	#pragma HLS BIND_STORAGE variable=URAM_frontiers type=RAM_1P impl=URAM
	#pragma HLS ARRAY_PARTITION variable=URAM_frontiers complete dim=1 */

	vprop_dest_t URAM_vprop[NUM_VALID_PEs][EDGE_PACK_SIZE][MAXVALID_APPLYPARTITION_VECSIZE];
	// #pragma HLS resource variable=URAM_vprop core=XPM_MEMORY uram 
	#pragma HLS BIND_STORAGE variable=URAM_vprop type=RAM_1P impl=URAM
	#pragma HLS ARRAY_PARTITION variable=URAM_vprop complete dim=1
	#pragma HLS ARRAY_PARTITION variable=URAM_vprop complete dim=2 
	// #pragma HLS aggregate variable=URAM_vprop 

	// map_t URAM_map[NUM_VALID_PEs][EDGE_PACK_SIZE][EDGE_UPDATES_DRAMBUFFER_SIZE];
	// #pragma HLS resource variable=URAM_map core=XPM_MEMORY uram 
	// #pragma HLS ARRAY_PARTITION variable=URAM_map complete dim=1	
	// #pragma HLS ARRAY_PARTITION variable=URAM_map complete dim=2	
	
	keyvalue_t URAM_updates[NUM_VALID_PEs][EDGE_PACK_SIZE][MAXVALID_VERTEXUPDATES_BUFFER_SIZE]; 
	// #pragma HLS resource variable=URAM_updates core=XPM_MEMORY uram 
	#pragma HLS BIND_STORAGE variable=URAM_updates type=RAM_1P impl=URAM
	#pragma HLS ARRAY_PARTITION variable=URAM_updates complete dim=1	
	#pragma HLS ARRAY_PARTITION variable=URAM_updates complete dim=2		
	
#else 
	keyvalue_t * nfrontier_buffer[NUM_VALID_PEs][EDGE_PACK_SIZE]; 
	keyvalue_t * URAM_frontiers[EDGE_PACK_SIZE]; 
	vtr_t * vptr_buffer[NUM_VALID_PEs]; 
	edge3_type * edges_buffer[NUM_VALID_PEs][EDGE_PACK_SIZE];
	// map_t * URAM_map[NUM_VALID_PEs][EDGE_PACK_SIZE];
	keyvalue_t * updates_buffer2[NUM_VALID_PEs][UPDATES_BUFFER_PACK_SIZE]; 
	vprop_dest_t * URAM_vprop[NUM_VALID_PEs][EDGE_PACK_SIZE];
	keyvalue_t * URAM_updates[NUM_VALID_PEs][EDGE_PACK_SIZE];	
	for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ nfrontier_buffer[i][v] = new keyvalue_t[MAX_VDATA_SUBPARTITION_VECSIZE]; }}
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ URAM_frontiers[v] = new keyvalue_t[MAX_APPLYPARTITION_VECSIZE]; }	
	for(unsigned int i=0; i<NUM_VALID_PEs; i++){ vptr_buffer[i] = new vtr_t[VPTR_BUFFER_SIZE]; }
	for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ edges_buffer[i][v] = new edge3_type[EDGE_BUFFER_SIZE]; }}
	// for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ URAM_map[i][v] = new map_t[EDGE_UPDATES_DRAMBUFFER_SIZE]; }}
	for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int v=0; v<UPDATES_BUFFER_PACK_SIZE; v++){ updates_buffer2[i][v] = new keyvalue_t[UPDATES_BUFFER_SIZE]; }}
	for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ URAM_vprop[i][v] = new vprop_dest_t[MAXVALID_APPLYPARTITION_VECSIZE]; }}
	for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ URAM_updates[i][v] = new keyvalue_t[8192]; }}
#endif 

#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
#ifdef ___RUNNING_FPGA_SYNTHESIS___ 
	edge_update_type URAM_edgeupdates[NUM_VALID_PEs][EDGE_PACK_SIZE][EDGE_UPDATES_DRAMBUFFER_SIZE];	
	#pragma HLS resource variable=URAM_edgeupdates core=XPM_MEMORY uram 
	#pragma HLS ARRAY_PARTITION variable=URAM_edgeupdates complete dim=1	
	#pragma HLS ARRAY_PARTITION variable=URAM_edgeupdates complete dim=2	
#else 
	edge_update_type * URAM_edgeupdates[NUM_VALID_PEs][EDGE_PACK_SIZE];
	for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ URAM_edgeupdates[i][v] = new edge_update_type[EDGE_UPDATES_DRAMBUFFER_SIZE]; }}
#endif 
#endif 

#ifdef FPGA_IMPL	
unsigned int cfrontier_dram___size[MAX_NUM_UPARTITIONS]; 
unsigned int nfrontier_dram___size[NUM_VALID_PEs][MAX_NUM_UPARTITIONS];
#pragma HLS ARRAY_PARTITION variable = nfrontier_dram___size complete dim=1
unsigned int updates_tmpbuffer___size[NUM_VALID_PEs][NUM_VALID_PEs]; 
#pragma HLS ARRAY_PARTITION variable = updates_tmpbuffer___size complete dim=1
unsigned int updates_buffer___size[NUM_VALID_PEs][EDGE_PACK_SIZE][MAX_NUM_APPLYPARTITIONS]; 
#pragma HLS ARRAY_PARTITION variable = updates_buffer___size complete dim=1
#pragma HLS ARRAY_PARTITION variable = updates_buffer___size complete dim=2
unsigned int stats_buffer___size[NUM_VALID_PEs][MAX_NUM_APPLYPARTITIONS]; 
#pragma HLS ARRAY_PARTITION variable = stats_buffer___size complete dim=1
unsigned int statsbuffer_idbased___size[NUM_VALID_PEs][MAX_NUM_APPLYPARTITIONS]; 
#pragma HLS ARRAY_PARTITION variable = statsbuffer_idbased___size complete dim=1
offset_t upartition_vertices[MAX_NUM_UPARTITIONS]; 
offset_t vpartition_vertices[NUM_VALID_PEs][MAX_NUM_APPLYPARTITIONS]; 
#pragma HLS ARRAY_PARTITION variable = vpartition_vertices complete dim=1
unsigned int vptrbuffer___size[NUM_VALID_PEs]; 
#pragma HLS ARRAY_PARTITION variable = vptrbuffer___size complete // dim=1
unsigned int edges_buffer___size[NUM_VALID_PEs]; 
#pragma HLS ARRAY_PARTITION variable = edges_buffer___size complete // dim=1
unsigned int cfrontier_bufferREAL___size[NUM_VALID_PEs]; 
#pragma HLS ARRAY_PARTITION variable = cfrontier_bufferREAL___size complete
#pragma HLS ARRAY_MAP variable=nfrontier_dram___size instance=array1 horizontal
#pragma HLS ARRAY_MAP variable=stats_buffer___size instance=array1 horizontal
#pragma HLS ARRAY_MAP variable=statsbuffer_idbased___size instance=array1 horizontal
#pragma HLS ARRAY_MAP variable=vpartition_vertices instance=array1 horizontal
#pragma HLS ARRAY_MAP variable=vptrbuffer___size instance=array2 horizontal
#pragma HLS ARRAY_MAP variable=edges_buffer___size instance=array2 horizontal
#ifndef HW
unsigned int hybrid_map[MAXNUMGRAPHITERATIONS][MAX_NUM_UPARTITIONS]; 
#endif 
#else 
unsigned int * cfrontier_dram___size = new unsigned int[MAX_NUM_UPARTITIONS]; 
unsigned int * nfrontier_dram___size[NUM_VALID_PEs]; for(unsigned int i=0; i<NUM_VALID_PEs; i++){ nfrontier_dram___size[i] = new unsigned int[MAX_NUM_UPARTITIONS]; }
unsigned int updates_tmpbuffer___size[NUM_VALID_PEs][NUM_VALID_PEs]; 
unsigned int * updates_buffer___size[NUM_VALID_PEs][EDGE_PACK_SIZE]; for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ updates_buffer___size[i][v] = new unsigned int[MAX_NUM_APPLYPARTITIONS]; }}
unsigned int stats_buffer___size[NUM_VALID_PEs][MAX_NUM_APPLYPARTITIONS]; 
unsigned int statsbuffer_idbased___size[NUM_VALID_PEs][MAX_NUM_APPLYPARTITIONS]; 
offset_t * upartition_vertices = new offset_t[MAX_NUM_UPARTITIONS]; 
offset_t * vpartition_vertices[NUM_VALID_PEs]; for(unsigned int i=0; i<NUM_VALID_PEs; i++){ vpartition_vertices[i] = new offset_t[MAX_NUM_APPLYPARTITIONS];  }
unsigned int vptrbuffer___size[NUM_VALID_PEs]; 
unsigned int edges_buffer___size[NUM_VALID_PEs]; 
unsigned int cfrontier_bufferREAL___size[NUM_VALID_PEs]; 
unsigned int * hybrid_map[MAXNUMGRAPHITERATIONS]; for(unsigned int i=0; i<MAXNUMGRAPHITERATIONS; i++){ hybrid_map[i] = new unsigned int[MAX_NUM_UPARTITIONS]; }
#endif 

edge3_type edges[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = edges complete dim=0
unsigned int res[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = res complete dim=0
unsigned int vtemp_in[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = vtemp_in complete dim=0
unsigned int vtemp_out[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = vtemp_out complete dim=0
vprop_t uprop[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = uprop complete dim=0
keyvalue_t update_in[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = update_in complete dim=0
keyvalue_t update_out[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = update_out complete dim=0
bool ens[NUM_VALID_PEs][EDGE_PACK_SIZE]; 
#pragma HLS ARRAY_PARTITION variable = ens complete dim=0
vprop_t data[EDGE_PACK_SIZE]; 
#pragma HLS ARRAY_PARTITION variable = data complete
vprop_t datas[NUM_VALID_PEs][EDGE_PACK_SIZE]; 
#pragma HLS ARRAY_PARTITION variable = datas complete dim=0
keyvalue_t kvdata[EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = kvdata complete
keyvalue_t kvdatas[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = kvdatas complete dim=0
unsigned int offsets0[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = offsets0 complete
unsigned int offsets[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = offsets complete
unsigned int offsets2[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = offsets2 complete
unsigned int counts[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = counts complete
keyvalue_t actvvs[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = actvvs complete dim=0
unsigned int vid_first0[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = vid_first0 complete
unsigned int vid_first1[NUM_VALID_PEs]; 
#pragma HLS ARRAY_PARTITION variable = vid_first1 complete
unsigned int globalparams[GLOBALBUFFER_SIZE];
map_t updatesptrs[MAX_NUM_APPLYPARTITIONS]; // MAX_NUM_LLPSETS];
offset_t edgeupdatesptrs[NUM_VALID_PEs][EDGE_UPDATES_PTR_MAXSIZE];
#pragma HLS ARRAY_PARTITION variable=edgeupdatesptrs complete dim=1
unsigned int limits[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = limits complete
unsigned int max_limits[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = max_limits complete
uint512_vec_dt updates_vecs[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = updates_vecs complete
uint512_vec_dt edges_vecs[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = edges_vecs complete
unsigned int stats_count[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable=stats_count complete
unsigned int stats_counts[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable=stats_counts complete dim=0
unsigned int edge_counts[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable=edge_counts complete dim=0
unsigned int cummtv2[NUM_VALID_PEs]; 
#pragma HLS ARRAY_PARTITION variable = cummtv2 complete
unsigned int masks[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable=masks complete dim=0
edge3_vec_dt edge3_vecs[NUM_VALID_PEs]; 
#pragma HLS ARRAY_PARTITION variable=edge3_vecs complete
map_t maps[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable=maps complete	
unsigned int offsets3[NUM_VALID_PEs]; 
#pragma HLS ARRAY_PARTITION variable = offsets3 complete
map_t edgeupdate_map[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable=edgeupdate_map complete
map_t edge_map[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable=edge_map complete
unsigned int offsets_eu[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = offsets_eu complete
unsigned int limits_eu[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = limits_eu complete
unsigned int max_sz = 0; 
#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
map_t edge_maps_buffer[NUM_VALID_PEs][MAX_NUM_UPARTITIONS];	
#pragma HLS ARRAY_PARTITION variable=edge_maps_buffer complete dim=1
map_t edgeu_maps_buffer[NUM_VALID_PEs][MAX_NUM_UPARTITIONS];	
#pragma HLS ARRAY_PARTITION variable=edgeu_maps_buffer complete dim=1
#endif 

#ifdef FPGA_IMPL
unsigned int _NUMCLOCKCYCLES_[2][32]; 
#else 
unsigned int _NUMCLOCKCYCLES_[2][32]; 
#endif 

	
	
	// initialize variables 
unsigned int max_limit = 0, max_limit2 = 0; 
for(unsigned int k=0; k<2; k++){ for(unsigned int t=0; t<32; t++){ _NUMCLOCKCYCLES_[k][t] = 0; } }
unsigned int total_cycles = 0;

// load global parameters	
MY_LOOP209: for(unsigned int t=0; t<GLOBALBUFFER_SIZE; t++){
#pragma HLS PIPELINE II=1
	globalparams[t] = dretrieve_globalparams(0, t, HBM_EDGESA0, HBM_EDGESB0, 0);
}
unsigned int __NUM_UPARTITIONS = globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS];
unsigned int __NUM_APPLYPARTITIONS = globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]; 
#ifndef FPGA_IMPL
globalparams_debug = (unsigned int *)&globalparams[0];
#endif

int ___ENABLE___RESETBUFFERSATSTART___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___RESETBUFFERSATSTART];
int ___ENABLE___PREPAREEDGEUPDATES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___PREPAREEDGEUPDATES];
int ___ENABLE___PROCESSEDGEUPDATES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___PROCESSEDGEUPDATES];
int ___ENABLE___PROCESSEDGES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___PROCESSEDGES];
	int ___ENABLE___READ_FRONTIER_PROPERTIES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___READ_FRONTIER_PROPERTIES];
	int ___ENABLE___VCPROCESSEDGES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___VCPROCESSEDGES]; 
int ___ENABLE___SAVEVCUPDATES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___SAVEVCUPDATES]; 
int ___ENABLE___COLLECTACTIVEDSTVIDS___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___COLLECTACTIVEDSTVIDS];
int ___ENABLE___APPLYUPDATESMODULE___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___APPLYUPDATESMODULE]; 
	int ___ENABLE___READ_DEST_PROPERTIES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___READ_DEST_PROPERTIES];
	int ___ENABLE___APPLYUPDATES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___APPLYUPDATES]; 
	int ___ENABLE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES]; 
	int ___ENABLE___SAVE_DEST_PROPERTIES___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___SAVE_DEST_PROPERTIES]; 
int ___ENABLE___EXCHANGEFRONTIERINFOS___BOOL___ = globalparams[GLOBALPARAMSCODE___ENABLE___EXCHANGEFRONTIERINFOS]; 

unsigned int threshold___activedstvids = globalparams[GLOBALPARAMSCODE__PARAM__THRESHOLD__ACTIVEDSTVID];
unsigned int async_batch = globalparams[GLOBALPARAMSCODE__ASYNC__BATCH]; 
unsigned int _ASYNC_BATCH_SIZE_ = globalparams[GLOBALPARAMSCODE__ASYNC__BATCHSIZE];	

for(unsigned int p_u=0; p_u<globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS]; p_u+=1){ cfrontier_dram___size[p_u] = MAXVALID_APPLYPARTITION_VECSIZE; }
unsigned int threshold___activefrontiers = globalparams[GLOBALPARAMSCODE__PARAM__THRESHOLD__ACTIVEFRONTIERSFORCONTROLSWITCH]; 

#ifdef _DEBUGMODE_KERNELPRINTS//4
if(action.status == 1){
	cout<<"=== acts_kernel::run:: parameters ==="<<endl;
	cout<<"=== num vertices: "<<globalparams[GLOBALPARAMSCODE__PARAM__NUM_VERTICES]<<" === "<<endl;
	cout<<"=== num edges: "<<globalparams[GLOBALPARAMSCODE__PARAM__NUM_EDGES]<<" === "<<endl;
	cout<<"=== num_pes: "<<NUM_VALID_PEs<<" === "<<endl;
	cout<<"=== EDGE_PACK_SIZE: "<<EDGE_PACK_SIZE<<" === "<<endl;
	cout<<"=== MAX_UPARTITION_SIZE: "<<MAX_UPARTITION_SIZE<<" === "<<endl;
	cout<<"=== NUM_UPARTITIONS: "<<__NUM_UPARTITIONS<<" === "<<endl;
	cout<<"=== MAX_APPLYPARTITION_SIZE: "<<MAX_APPLYPARTITION_SIZE<<" === "<<endl;
	cout<<"=== NUM_APPLYPARTITIONS: "<<__NUM_APPLYPARTITIONS<<" === "<<endl;
	cout<<"=== MULT_FACTOR: "<<MULT_FACTOR<<" === "<<endl;
	cout<<"---------------------------------------------------------------------- ACTS running "<<NUM_VALID_PEs<<" instances ----------------------------------------------------------------------"<<endl;
	cout<<"### GAS iteration: 0 [1 active vertices]"<<endl; 
}
#endif 
		

	// resets
	if(___ENABLE___RESETBUFFERSATSTART___BOOL___ == 1){
		#ifdef ___ENABLE___RESETBUFFERSATSTART___
MY_LOOP201: for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int p=0; p<MAX_NUM_UPARTITIONS; p++){ nfrontier_dram___size[i][p] = 0; }}
MY_LOOP205: for(unsigned int i=0; i<NUM_VALID_PEs; i++){ for(unsigned int p=0; p<__NUM_APPLYPARTITIONS; p++){ vpartition_vertices[i][p].offset = 0; vpartition_vertices[i][p].size = 0; vpartition_vertices[i][p].count = 0; }}					
for(unsigned int p=0; p<__NUM_APPLYPARTITIONS; p++){ updatesptrs[p].size = 0; }
MY_LOOP207: for(unsigned int i=0; i<NUM_VALID_PEs; i++){ vptrbuffer___size[i] = 0; }
MY_LOOP208: for(unsigned int i=0; i<NUM_VALID_PEs; i++){ edges_buffer___size[i] = 0; }
#ifndef HW
for(unsigned int i=0; i<MAXNUMGRAPHITERATIONS; i++){ for(unsigned int t=0; t<MAX_NUM_UPARTITIONS; t++){ hybrid_map[i][t] = 1; }}
#endif
#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
for(unsigned int p=0; p<MAX_NUM_UPARTITIONS; p++){
	for(unsigned int i=0; i<NUM_VALID_PEs; i++){ 
	#pragma HLS UNROLL	
		edge_maps_buffer[i][p].offset = 0; edge_maps_buffer[i][p].size = 0; 
	}
}
#endif 
#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
MY_LOOP1580: for(unsigned int t=0; t<EDGE_UPDATES_DRAMBUFFER_SIZE; t++){ 
#pragma HLS PIPELINE II=1
	for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
	#pragma HLS UNROLL
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
		#pragma HLS UNROLL		
			URAM_map[inst][v][t].offset = 0; 
			URAM_map[inst][v][t].size = 0;	
			URAM_edgeupdates[inst][v][t].srcvid = INVALIDDATA; 
			URAM_edgeupdates[inst][v][t].dstvid = INVALIDDATA; 
		}
	}
}
#endif
#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
for(unsigned int t=0; t<1024; t++){
	for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 	
		#pragma HLS UNROLL
		edgeupdatesptrs[inst][t].offset = 0; 
		edgeupdatesptrs[inst][t].size = 0;
	}
}
#endif 		
		#endif 
	}
	
	unsigned int algo = globalparams[GLOBALPARAMSCODE__PARAM__ALGORITHM];
	unsigned int import_offset = action.id_import * action.size_import_export;
	unsigned int export_offset = action.id_export * action.size_import_export;
	unsigned int vdata_subpartition_vecsize = MAX_UPARTITION_VECSIZE / globalparams[GLOBALPARAMSCODE__PARAM__GLOBAL_NUM_PEs];
	unsigned int num_subpartition_per_partition = globalparams[GLOBALPARAMSCODE__PARAM__GLOBAL_NUM_PEs];
	#ifdef _DEBUGMODE_KERNELPRINTS//4
	cout<<"acts kernel- vdata_subpartition_vecsize: "<<vdata_subpartition_vecsize<<endl;
	cout<<"acts kernel- num_subpartition_per_partition: "<<num_subpartition_per_partition<<endl;
	#endif 
	
	// load vertex-updates map 
	#ifdef _DEBUGMODE_KERNELPRINTS4
	if((action.module == ALL_MODULES || action.module == PROCESS_EDGES_MODULE) && (action.start_pu == 0)){ if(action.fpga < num_prints){ cout << "acts: resetting updates space..." <<endl; }}
	#endif 
	LOAD_UPDATEPTRS_lOOP1: for(unsigned int t=0; t<globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]; t++){	
	#pragma HLS PIPELINE II=1
		updatesptrs[t] = load_vupdate_map(globalparams[GLOBALPARAMSCODE__BASEOFFSET__UPDATESPTRS], t,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);	
		updatesptrs[t].offset = 0; // FIXME.
		if((action.module == ALL_MODULES || action.module == PROCESS_EDGES_MODULE) && (action.start_pu == 0)){ updatesptrs[t].size = 0; }				
		#ifdef _DEBUGMODE_KERNELPRINTS//4
		if(action.fpga < num_prints){ cout<<"acts: start: updatesptrs["<<t<<"].offset: "<<updatesptrs[t].offset<<", updatesptrs["<<t<<"].size: "<<updatesptrs[t].size<<endl; }
		#endif 
	}
	
	// load edge map 
	#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
	map_t edge_maps_l[NUM_VALID_PEs];
	map_t edgeu_maps_l[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=edge_maps_l complete
	#pragma HLS ARRAY_PARTITION variable=edgeu_maps_l complete
	LOAD_EDGEPTRS_lOOP1: for(unsigned int p_u=action.start_pu; p_u<action.start_pu + action.size_pu + 4; p_u+=action.skip_pu){
		load_edgemaps((p_u * MAX_NUM_LLPSETS) + 0, globalparams[GLOBALPARAMSCODE__BASEOFFSET__ACTPACKVPTRS], edge_maps_l,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); 
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			#pragma HLS UNROLL
			edge_maps_buffer[n][p_u].offset = edge_maps_l[n].offset; edge_maps_buffer[n][p_u].size = edge_maps_l[n].size;
			#ifdef _DEBUGMODE_KERNELPRINTS//4
			cout<<">>> acts : [load] edge_maps_l["<<n<<"]["<<p_u<<"].offset: "<<edge_maps_l[n].offset<<", edge_maps_l["<<n<<"]["<<p_u<<"].size: "<<edge_maps_l[n].size<<", maxsz: "<<(edge_maps_buffer[n][p_u + 1].offset - edge_maps_buffer[n][p_u].offset)<<endl;
			#endif 
		}
	}
	LOAD_EDGEPTRS_lOOP2: for(unsigned int p_u=action.start_pu; p_u<action.start_pu + action.size_pu + 4; p_u+=action.skip_pu){
		load_edgemaps((p_u * MAX_NUM_LLPSETS) + 0, globalparams[GLOBALPARAMSCODE__BASEOFFSET__EDGEUPDATESPTRS], edgeu_maps_l,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); // REMOVEME.
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			#pragma HLS UNROLL
			edgeu_maps_buffer[n][p_u].offset = edgeu_maps_l[n].offset; edgeu_maps_buffer[n][p_u].size = edgeu_maps_l[n].size;
			#ifdef _DEBUGMODE_KERNELPRINTS//4
			cout<<">>> acts : [load] edgeu_maps_l["<<n<<"]["<<p_u<<"].offset: "<<edgeu_maps_l[n].offset<<", edgeu_maps_l["<<n<<"]["<<p_u<<"].size: "<<edgeu_maps_l[n].size<<endl;
			#endif 
		}
	}
	for(unsigned int p_u=action.start_pu; p_u<action.start_pu + action.size_pu + 4; p_u+=action.skip_pu){
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			#ifdef _DEBUGMODE_KERNELPRINTS4
			cout<<">>> acts : [load][edge_maps_s,edgeu_maps_s][p_u: "<<p_u<<"]: offset: "<<edge_maps_buffer[n][p_u].offset<<", proc: "<<edge_maps_buffer[n][p_u].size<<", rem: "<<edgeu_maps_buffer[n][p_u].size<<", maxsz: "<<(edge_maps_buffer[n][p_u + 1].offset - edge_maps_buffer[n][p_u].offset)<<endl;
			#endif 
		}
	}
	#endif

	unsigned int GraphIter=0; 
	unsigned int MASK_CODE = 1 + GraphIter;
	unsigned int MASK_CODE_PE = 1 + GraphIter;
	unsigned int MASK_CODE_AU = 4094 + 1 + GraphIter;
	
	// clear counters
	#ifdef ___ENABLE___CLEAR_COUNTERS___
// clear counters
CLEAR_COUNTERS_LOOP1: for(unsigned int p_v=0; p_v<__NUM_APPLYPARTITIONS; p_v++){ 
#pragma HLS PIPELINE II=1
	for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 
	#pragma HLS UNROLL
		stats_buffer___size[inst][p_v] = 0; statsbuffer_idbased___size[inst][p_v] = 0;
		vpartition_vertices[inst][p_v].offset = 0; vpartition_vertices[inst][p_v].size = 0; vpartition_vertices[inst][p_v].count = 0;
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 
		#pragma HLS UNROLL
			updates_buffer___size[inst][v][p_v] = 0; 
		}
	}
}		
	#endif
	
	// process-edges and partition-updates 
	#ifdef ___ENABLE___PROCESSEDGES___
	if(action.module == PROCESS_EDGES_MODULE || action.module == ALL_MODULES){
	PROCESS_EDGES_MODULE_LOOP1B: for(unsigned int local_pu=action.start_pu; local_pu<action.start_pu + action.size_pu; local_pu+=action.skip_pu){
		unsigned int p_u = local_pu; 
		action.id_process = p_u; 
		MASK_CODE_PE = ((1 + GraphIter) * MAX_NUM_UPARTITIONS) + p_u;
		if(p_u >= globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS]){ continue; } 
	
		#ifdef _DEBUGMODE_KERNELPRINTS//4 
		if(action.fpga < num_prints){ cout<<"### processing edges in upartition "<<local_pu<<" (global: "<<p_u<<"): [PEs "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<n<<", "; } cout<<"] [max "<<globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS]<<"]"<<endl; }
		#endif 
		
		// read & map frontier properties 
		#ifdef ___ENABLE___READ_FRONTIER_PROPERTIES___
		if(___ENABLE___READ_FRONTIER_PROPERTIES___BOOL___ == 1){ 
			if(p_u % NUM_VALID_HBM_CHANNELS_PER_SLR==0){ // sharing reduces number of reads. FIXME^
// broadcast active frontiers [done]
unsigned int uoffset = p_u * MAX_UPARTITION_VECSIZE; 		
unsigned int data[HBM_CHANNEL_PACK_SIZE];	
#pragma HLS ARRAY_PARTITION variable=data complete
keyvalue_t kvdatas[NUM_VALID_PEs][EDGE_PACK_SIZE];	
#pragma HLS ARRAY_PARTITION variable = kvdatas complete dim=0
unsigned int offset_c = globalparams[GLOBALPARAMSCODE__BASEOFFSET__CFRONTIERSTMP];

unsigned int sz = MAXVALID_APPLYPARTITION_VECSIZE; if(action.command == GRAPH_UPDATE_ONLY || action.command == GRAPH_ANALYTICS_EXCLUDEVERTICES){ sz = 0; }

#ifdef _DEBUGMODE_KERNELPRINTS4
if(action.fpga < num_prints){ cout<<"``` reading "<<(sz << POW_VALID_VDATA) * EDGE_PACK_SIZE<<" frontiers from HBM in upartition "<<p_u<<": [PEs "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<n<<", "; } cout<<"]"<<endl; }
#endif 

// parallel-read and map active frontiers [done]
READ_FRONTIERS_LOOP2: for(unsigned int t=0; t<sz; t++){ 
#pragma HLS PIPELINE II=1	
	dretrievemany_cfrontierdram_tmp(0, t, kvdatas,  HBM_SRCA0, HBM_SRCB0, HBM_SRCA1, HBM_SRCB1, HBM_SRCA2, HBM_SRCB2, HBM_SRCA3, HBM_SRCB3, HBM_SRCA4, HBM_SRCB4);
	for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
	#pragma HLS UNROLL 
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
		#pragma HLS UNROLL
			unsigned int srcvid_lpv = (kvdatas[inst][v].key - uoffset) >> EDGE_PACK_SIZE_POW;
			if(kvdatas[inst][v].key != INVALIDDATA && srcvid_lpv < MAX_UPARTITION_VECSIZE){
				vprop_dest_t tmp; tmp.prop = kvdatas[inst][v].value; tmp.gvid = kvdatas[inst][v].key; 
				URAM_vprop[inst][v][srcvid_lpv] = tmp; 
			}
		}
	}
	
	#ifdef ___ENABLE___IMPORT_EXPORT___
	hidden_import_and_export_function(t, import_offset, export_offset, action.size_import_export, HBM_centerA, HBM_centerB, HBM_import, HBM_export, action);
	#endif 
	 
	update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___READ_FRONTIER_PROPERTIES___, 1);
}








						
			}
		}
		#endif
		
		// process-edges and partition-updates
		#ifdef ___ENABLE___ECPROCESSEDGES___II1___ 
		if(___ENABLE___PROCESSEDGES___BOOL___ == 1){ 
unsigned int offsets[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = offsets complete
offset_t offset_partition[NUM_VALID_PEs][EDGE_PACK_SIZE][MAX_NUM_APPLYPARTITIONS];
#pragma HLS ARRAY_PARTITION variable=offset_partition complete dim=1	
#pragma HLS ARRAY_PARTITION variable=offset_partition complete dim=2
map_t edge_maps[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable=edge_maps complete	
keyvalue_t update_in[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = update_in complete dim=0
keyvalue_t update_out[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = update_out complete dim=0
keyvalue_t update_out2[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = update_out2 complete dim=0
bool ens2[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = ens2 complete
keyvalue_t out[4][NUM_VALID_HBM_CHANNELS][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = out complete dim=1
#pragma HLS ARRAY_PARTITION variable = out complete dim=2
#pragma HLS ARRAY_PARTITION variable = out complete dim=3
			
unsigned int import_offset = id_import * action.size_import_export;
unsigned int export_offset = id_export * action.size_import_export;

unsigned int llp_set=0;
unsigned int llp_id=0;

// load edges map
#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___ 
for(unsigned int n=0; n<NUM_VALID_PEs; n++){ edge_maps[n].offset = edge_maps_buffer[n][p_u].offset; edge_maps[n].size = edge_maps_buffer[n][p_u].size; }
#else 
load_edgemaps((p_u * MAX_NUM_LLPSETS) + llp_set, globalparams[GLOBALPARAMSCODE__BASEOFFSET__ACTPACKVPTRS], edge_maps,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); // FIXME.
#endif 

// prepare maps, offsets, variables 
max_sz = 0; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ limits[n] = edge_maps[n].size; } for(unsigned int n=0; n<NUM_VALID_PEs; n++){ if(max_sz < limits[n]){ max_sz = limits[n]; }}			
for(unsigned int n=0; n<NUM_VALID_PEs; n++){ offsets[n] = edge_maps[n].offset; }	
#ifdef _DEBUGMODE_KERNELPRINTS//4
if(action.fpga < num_prints){ for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<"process-edges: edges map offsets: "; cout<<offsets[n]<<", max_sz: "<<max_sz<<", updatesptrs["<<llp_set<<"].offset: "<<updatesptrs[llp_set].offset<<", updatesptrs["<<llp_set<<"].size: "<<updatesptrs[llp_set].size<<endl; }}				
#endif 	

unsigned int sz = edge_maps[0].size; unsigned int max_num_edges = 0;
#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
max_num_edges = edge_maps_buffer[0][p_u+1].offset - edge_maps_buffer[0][p_u].offset; 
if((action.command == GRAPH_UPDATE_ONLY) && (edge_maps[0].size == max_num_edges)){ for(unsigned int n=0; n<NUM_VALID_PEs; n++){ sz = 0; max_sz = 0; }}
#endif 

#ifdef _DEBUGMODE_KERNELPRINTS4 
if(action.fpga < num_prints){ cout<<"+++ processing "<<sz * EDGE_PACK_SIZE<<" edges (of "<<max_num_edges * EDGE_PACK_SIZE<<") in upartition "<<p_u<<": [PEs "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<n<<", "; } cout<<"]"<<endl; }
#endif 

// process edges and save vertex updates 
unsigned int batch_size = 4096; // 512;
unsigned int packsz = 1; unsigned int pack_id=0; // 2; // FIXME. 
PROCESS_EDGES_MAINLOOP1C: for(unsigned int t1=0; t1<(max_sz + batch_size - 1) / batch_size; t1++){ 
	// resets 		
	for(unsigned int partition=0; partition<globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]; partition++){ 
	#pragma HLS PIPELINE II=1
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
			#pragma HLS UNROLL
				offset_partition[n][v][partition].offset = 0; offset_partition[n][v][partition].size = 0; 
			}
		}
	}
	
	unsigned int process_size = batch_size; if((t1 * batch_size) + batch_size >= max_sz){ process_size = max_sz - (t1 * batch_size); }
	if(offsets[0] + ((t1 * 512) + process_size) >= globalparams[GLOBALPARAMSCODE__WWSIZE__ACTPACKEDGES]){ process_size = 0; } // continue; 
	unsigned int readoffset = t1 * 512; 
	unsigned int placeholder_partition = 0;
	
	// process edges 
	PROCESS_EDGES_MAINLOOP1DAAA: for(unsigned int t2=0; t2<process_size / packsz; t2++){ 
	#pragma HLS PIPELINE II=1
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
			#pragma HLS UNROLL
				update_out2[inst][v].key = 0;
				update_out2[inst][v].value = 0;
			}
		}
		
		unsigned int t = readoffset + t2*packsz + pack_id;
		dretrievemany_edges(globalparams[GLOBALPARAMSCODE__BASEOFFSET__ACTPACKEDGES], offsets, t, edge3_vecs,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
		
		#ifdef ENABLE_SHARING_SRCs
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
			#pragma HLS UNROLL
				unsigned int src = edge3_vecs[inst].data[v].srcvid;
				if(src < MAX_UPARTITION_VECSIZE){ out[0][inst][v].key = URAM_vprop[inst][v][src].prop; out[0][inst][v].value = URAM_vprop[inst][v][src].gvid; }
			}
		}
		unsigned int s = p_u % NUM_VALID_HBM_CHANNELS;
		unsigned int rotatebys[4]; rotatebys[0] = 0; rotatebys[1] = 0; rotatebys[2] = 0; rotatebys[3] = 0;
		// if(s == 1){ rearrangeLayoutV16x16B(rotatebys[1], out[0], out[1]); } 
		// if(s == 2){ rearrangeLayoutV16x16B(rotatebys[2], out[1], out[2]); } 
		// if(s == 3){ rearrangeLayoutV16x16B( rotatebys[3], out[2], out[3]); }
		if(s == 1){ rearrangeLayoutV16x16B(0, out[0], out[1]); } 
		if(s == 2){ rearrangeLayoutV16x16B(0, out[1], out[2]); } 
		if(s == 3){ rearrangeLayoutV16x16B(0, out[2], out[3]); }
		#endif 
		
		EC_PROCESS_EDGES_LOOP1F: for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
		#pragma HLS UNROLL
			bool en = true; if(t >= limits[inst]){ en = false; } else { en = true; } 
			edge3_vec_dt edge_vec = edge3_vecs[inst];
			unsigned int rotateby = 0;
			
			#ifdef _DEBUGMODE_CHECKS3
			checkoutofbounds("acts_kernel::ERROR 213::", rotateby, EDGE_PACK_SIZE, NAp, NAp, NAp);
			#endif
			
			// read source properties and process edge to form vertex-updates 
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
			#pragma HLS UNROLL
				edge3_type edge = edge_vec.data[v];
				unsigned int srcvid_lpv = edge.srcvid; 
				
				#ifdef ENABLE_SHARING_SRCs
				vprop_t uprop; uprop.prop = out[3][inst][v].key; uprop.degree = out[3][inst][v].value; 
				#else 
				vprop_t uprop; if(edge.srcvid != INVALIDDATA && edge.srcvid < MAX_UPARTITION_VECSIZE){ uprop.prop = URAM_vprop[inst][v][srcvid_lpv].prop; uprop.degree = URAM_vprop[inst][v][srcvid_lpv].gvid; }
				#endif 
				
				if(edge.srcvid != INVALIDDATA && edge.srcvid < MAX_UPARTITION_VECSIZE){ 
					unsigned int res = process_funcG(uprop, 1, globalparams[GLOBALPARAMSCODE__PARAM__ALGORITHM]);
					#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___ 
					update_in[inst][v].key = edge.dstvid; update_in[inst][v].value = edge.srcvid;
					#else 
					update_in[inst][v].key = edge.dstvid; update_in[inst][v].value = res;
					#endif 
					#ifdef _DEBUGMODE_CHECKS//3
					checkoutofbounds("acts_kernel::ERROR 1213c::", edge.srcvid, MAX_UPARTITION_VECSIZE, inst, NAp, NAp);
					checkoutofbounds("acts_kernel::ERROR 1213d::", edge.dstvid, MAX_UPARTITION_VECSIZE, inst, NAp, NAp);
					#endif
					#ifdef _DEBUGMODE_KERNELPRINTS_TRACE3
					std::cout<<"EC - PROCESS EDGE SEEN @: inst: ["<<inst<<"], [srcvid_lpv: "<<srcvid_lpv<<", dstvid: "<<edge.dstvid % MAX_APPLYPARTITION_SIZE<<"], [edge.srcvid: "<<edge.srcvid<<", dstvid: "<<edge.dstvid<<"]"<<std::endl; 
					#endif 
				} else {
					update_in[inst][v].key = INVALIDDATA; update_in[inst][v].value = INVALIDDATA;
				}
			}
			
			// circular shift >>>
			rearrangeLayoutVx16B(rotateby, update_in[inst], update_out[inst]); 
			
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
			#pragma HLS UNROLL
				update_out2[inst][v].key = update_out[inst][v].key;
				update_out2[inst][v].value = update_out[inst][v].value; 
			}	
		}
		
		update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___ECPROCESSEDGES___, 1);
	
		// buffer updates 
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
			#pragma HLS UNROLL
				unsigned int partition = owner_vpartition(update_out2[inst][v].key, placeholder_partition); 
				#ifdef _DEBUGMODE_CHECKS3	
				checkoutofbounds("acts_kernel::process-edges::ERROR 8873yyy::", (partition * 0) + offset_partition[inst][v][partition].size, VERTEXUPDATES_BUFFER_SIZE, partition, offset_partition[inst][v][partition].offset, offset_partition[inst][v][partition].size); 
				#endif
				URAM_updates[inst][v][(partition * 0) + offset_partition[inst][v][partition].size] = update_out2[inst][v]; // FIXME.
				offset_partition[inst][v][partition].size += 1;
			}	
		}
		placeholder_partition += 1; if(placeholder_partition >= globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]){ placeholder_partition = 0; } // FIXME.		
	}		
	
	// save vertex updates
	SAVE_VERTEXUPDATES_MAINLOOP1: for(unsigned int partition=0; partition<globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]; partition++){
		#ifdef _DEBUGMODE_KERNELPRINTS//4 
		cout<<"### saving "<<offset_partition[0][0][partition].size<<" vertex updates into fpga partition "<<partition<<"..."<<endl; 
		#endif 
		SAVE_VERTEXUPDATES_MAINLOOP1B: for(unsigned int t=0; t<offset_partition[0][0][partition].size; t++){ 
		#pragma HLS PIPELINE II=1
			for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 	
			#pragma HLS UNROLL
				for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
				#pragma HLS UNROLL
					update_out[inst][v] = URAM_updates[inst][v][offset_partition[inst][v][partition].offset + t];
					#ifdef _DEBUGMODE_KERNELPRINTS_TRACE3
					std::cout<<"EC --- PROCESS EDGE SEEN @: [update_out["<<inst<<"]["<<v<<"].key: "<<update_out[inst][v].key<<", update_out["<<inst<<"]["<<v<<"].value: "<<update_out[inst][v].value<<"]"<<std::endl; 
					#endif 
				}
			}
			
			unsigned int offset___ = globalparams[GLOBALPARAMSCODE__BASEOFFSET__VERTEXUPDATES] + updatesptrs[partition].offset + updatesptrs[partition].size + t;
			
			#ifdef _DEBUGMODE_CHECKS3	
			// checkoutofbounds("acts_kernel::process-edges::ERROR 8813rrr::", offset___, globalparams_debug[GLOBALPARAMSCODE__BASEOFFSET__VDATAS], NAp, updatesptrs[partition].size, NAp); 
			checkoutofbounds("acts_kernel::process-edges::ERROR 8813rrr::", offset___, HBM_CHANNEL_SIZE, NAp, updatesptrs[partition].size, NAp); 
			#endif
			dinsertmany_updatesdram(offset___, update_out,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
		}
		
		updatesptrs[partition].size += offset_partition[0][0][partition].size;
	}
	
	// record updates 
	#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
	unsigned int offset = (p_u * globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]);
	for(unsigned int partition=0; partition<globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]; partition++){
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 	
			#pragma HLS UNROLL
			#ifdef _DEBUGMODE_CHECKS3	
			checkoutofbounds("acts_kernel::process-edges::ERROR 88137::", offset + partition, EDGE_UPDATES_PTR_MAXSIZE, NAp, offset, partition); 
			#endif	
			edgeupdatesptrs[inst][offset + partition].size += offset_partition[0][0][partition].size;
		}
	}
	#endif
}

// collect stats
for(unsigned int n=0; n<NUM_VALID_PEs; n++){ vpartition_vertices[n][llp_set].count += max_sz; }	
// exit(EXIT_SUCCESS); 


								
		}
		#endif
	}
	}
	#endif

	unsigned int totalactvvs2 = 0; 
	frontier_t actvv[EDGE_PACK_SIZE]; 
	for(unsigned int p_u=0; p_u<globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS]; p_u++){ upartition_vertices[p_u].count = 0; }
	for(unsigned int p_u=0; p_u<globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS]; p_u++){ cfrontier_dram___size[p_u] = 0; } // reset
	
	// apply updates 
	#ifdef ___ENABLE___APPLYUPDATESMODULE___ 
	if(action.module == APPLY_UPDATES_MODULE || action.module == ALL_MODULES){
	APPLY_UPDATES_MODULE_LOOP: for(unsigned int p_v=action.start_pv; p_v<action.start_pv + action.size_pv; p_v++){
		#ifndef ___ENABLE___DYNAMICGRAPHANALYTICS___ 
		if(updatesptrs[p_v].size == 0 || p_v >= globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]){ continue; } 
		#endif 
		
		#ifndef FPGA_IMPL
		checkoutofbounds("acts_kernel::ERROR 862::", p_v, globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS] * action.numfpgas, p_v, globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS], NAp);
		#endif 
		unsigned int voffset = globalparams[GLOBALPARAMSCODE__BASEOFFSET__VDATAS] + (p_v * MAX_APPLYPARTITION_VECSIZE);
		
		#ifdef _DEBUGMODE_KERNELPRINTS4 
		if(action.fpga < num_prints){ cout<<"### applying vertex updates in vpartition "<<p_v<<": [PEs "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<n<<", "; } cout<<"] [target FPGAs "; for(unsigned int n=0; n<action.numfpgas; n++){ cout<<n<<", "; } cout<<"] [max "<<globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]<<", "<<updatesptrs[p_v].size<<"]"<<endl; } 
		#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___ 
		if(action.fpga < num_prints){ cout<<"### applying edge updates in vpartition "<<p_v<<": [PEs "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<n<<", "; } cout<<"] [target FPGAs "; for(unsigned int n=0; n<action.numfpgas; n++){ cout<<n<<", "; } cout<<"]"<<endl; } 
		#endif 
		if(false && action.fpga < num_prints){ cout<<"APPLY_UPDATES_MODULE_LOOP: action.start_pv_fpga: "<<action.start_pv_fpga<<", updatesptrs["<<p_v<<"].size: "<<updatesptrs[p_v].size<<" ("<<updatesptrs[p_v].size * EDGE_PACK_SIZE<<") ((((((((((((((((((((((( "<<endl; }
		if(false && action.fpga < num_prints){ cout<<"APPLY_UPDATES_MODULE_LOOP: action.start_pv_fpga: "<<action.start_pv_fpga<<", vpartition_vertices[0]["<<p_v<<"].count (ww): "<<vpartition_vertices[0][p_v].count<<" ((((((((((((((((((((((( "<<endl; }
		#endif 
		
		// read destination properties
		#ifdef ___ENABLE___READ_DEST_PROPERTIES___
		if(___ENABLE___READ_DEST_PROPERTIES___BOOL___ == 1){
vprop_dest_t vprop[NUM_VALID_PEs][EDGE_PACK_SIZE]; 
#pragma HLS ARRAY_PARTITION variable = vprop complete dim=0

unsigned int sz = MAXVALID_APPLYPARTITION_VECSIZE; if(action.command == GRAPH_UPDATE_ONLY || action.command == GRAPH_ANALYTICS_EXCLUDEVERTICES){ sz = 0; }

READ_DEST_PROPERTIES_LOOP2B: for(unsigned int t=0; t<sz; t++){
#pragma HLS PIPELINE II=1
	dretrievemany_vdatadram(voffset, t, vprop,  HBM_SRCA0, HBM_SRCB0, HBM_SRCA1, HBM_SRCB1, HBM_SRCA2, HBM_SRCB2, HBM_SRCA3, HBM_SRCB3, HBM_SRCA4, HBM_SRCB4);
	READ_DEST_PROPERTIES_LOOP2C: for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 
	#pragma HLS UNROLL
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 
		#pragma HLS UNROLL
			#ifdef _DEBUGMODE_CHECKS3
			checkoutofbounds("acts_kernel::ERROR 104::", t, MAXVALID_APPLYPARTITION_VECSIZE, NAp, NAp, NAp);
			#endif 
			URAM_vprop[inst][v][t] = vprop[inst][v];
		}
	}
	
	#ifdef ___ENABLE___IMPORT_EXPORT___
	hidden_import_and_export_function(t, import_offset, export_offset, action.size_import_export, HBM_centerA, HBM_centerB, HBM_import, HBM_export, action);
	#endif 
		
	update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___READ_DEST_PROPERTIES___, 1);
}





					
		}
		#endif
		
		// apply updates
		#ifdef ___ENABLE___APPLYUPDATES___II1___ 
		if(___ENABLE___APPLYUPDATES___BOOL___ == 1){
uint512_vec_dt updates_vecs[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable=updates_vecs complete
max_limit = 0; for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ limits[inst] = updatesptrs[p_v].size; } for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ if(max_limit < limits[inst]){ max_limit = limits[inst]; }}

#ifdef _DEBUGMODE_KERNELPRINTS//4 
if(en == true){ cout<<"### applying vertex updates in vpartition "<<p_v<<": [PEs "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<n<<", "; } cout<<"]"<<endl; } 
#endif 

unsigned int updates_offset = globalparams[GLOBALPARAMSCODE__BASEOFFSET__VERTEXUPDATES] + updatesptrs[p_v].offset;
unsigned int edge_dir = 1;
APPLY_UPDATES_LOOP1: for(unsigned int t=0; t<max_limit; t++){
#pragma HLS PIPELINE II=3	
	dretrievemany_udatesdram(updates_offset + t, updates_vecs,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); // NEW
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){		
			#pragma HLS UNROLL
				unsigned int dstvid_lp; if(algo == HITS && edge_dir == 1){ dstvid_lp = updates_vecs[inst].data[v].key * 2; } else { dstvid_lp = updates_vecs[inst].data[v].key; }
				// if(dstvid_lp >= MAXVALID_APPLYPARTITION_SIZE){ dstvid_lp = 0; }
				unsigned int dstvid_lpv = dstvid_lp / EDGE_PACK_SIZE;	
				if(dstvid_lpv >= MAXVALID_APPLYPARTITION_VECSIZE){ dstvid_lpv = 0; }
				
				#ifdef _DEBUGMODE_CHECKS3
				if(t < limits[inst]){ checkoutofbounds("acts_kernel::ERROR 727a::", dstvid_lpv, MAXVALID_APPLYPARTITION_VECSIZE, NAp, inst, updates_vecs[inst].data[v].key); checkoutofbounds("acts_kernel::ERROR 727b::", dstvid_lpv, MAXVALID_APPLYPARTITION_VECSIZE, NAp, inst, updates_vecs[inst].data[v].key); }					
				#endif
				vprop_dest_t vprop = URAM_vprop[inst][v][dstvid_lpv];
				vprop_dest_t newprop; newprop.prop = INVALIDDATA; 
				unsigned int new_vprop = reduce_funcG(vprop, updates_vecs[inst].data[v].value, globalparams[GLOBALPARAMSCODE__PARAM__ALGORITHM]);
				if(t < limits[inst]){ 
					#ifdef _DEBUGMODE_KERNELPRINTS_TRACE3
					if(t < 1024){ std::cout<<"APPLY (ACT-PACK) UPDATE SEEN @: t: "<<t<<", inst: ["<<inst<<"], v: ["<<v<<"]: dstvid_lp: "<<dstvid_lp<<", dstvid_lpv: "<<dstvid_lpv<<", new_vprop: "<<new_vprop<<", update["<<t<<"].key: "<<updates_vecs[inst].data[v].key<<", update["<<t<<"].value: "<<updates_vecs[inst].data[v].value<<std::endl; }						
					#endif
					newprop.prop = new_vprop; 
				}
				URAM_vprop[inst][v][dstvid_lpv].prop = newprop.prop; 
				#ifdef _DEBUGMODE_CHECKS3
				if(inst==0 && v==0){ update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___APPLYUPDATES___, 1); }
				#endif 
			}
		}
	
	#ifdef ___ENABLE___IMPORT_EXPORT___
	hidden_import_and_export_function(t, import_offset, export_offset, action.size_import_export, HBM_centerA, HBM_centerB, HBM_import, HBM_export, action);
	#endif 
}		
// exit(EXIT_SUCCESS);	




			
		}	
		#endif 
		
		#ifdef ___ENABLE___APPLYVERTEXUPDATES_AND_APPLYEDGES___II1___ 
		if(___ENABLE___APPLYUPDATES___BOOL___ == 1){
#ifdef _DEBUGMODE_CHECKS3
#define _DEBUGMODE_APPLYEDGEUPDATES_PRINTS4
#endif
#define MISS_BUFFER_SZ 512 // EDGE_UPDATES_CHUNKSZ, BLOCKRAM_SIZE*
// #define MISS_BUFFER_SZ EDGE_UPDATES_CHUNKSZ

edge_update_type MISSBUFFER_edgeupdates[NUM_VALID_PEs][EDGE_PACK_SIZE][MISS_BUFFER_SZ]; 
#pragma HLS ARRAY_PARTITION variable=MISSBUFFER_edgeupdates complete dim=1	
#pragma HLS ARRAY_PARTITION variable=MISSBUFFER_edgeupdates complete dim=2

unsigned int MISSBUFFER_edgeupdates_index[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable=MISSBUFFER_edgeupdates_index complete dim=1	
#pragma HLS ARRAY_PARTITION variable=MISSBUFFER_edgeupdates_index complete dim=2

// edge_update_vec_dt tmp_buffer[NUM_VALID_PEs][EDGE_UPDATES_CHUNKSZ];	
// #pragma HLS ARRAY_PARTITION variable=tmp_buffer complete dim=1
edge3_vec_dt tmp_buffer2[NUM_VALID_PEs][BLOCKRAM_SIZE];	
#pragma HLS ARRAY_PARTITION variable=tmp_buffer2 complete dim=1
edge_update_vec_dt edge_update_vecs[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable=edge_update_vecs complete
edge3_vec_dt edge3_vecs[NUM_VALID_PEs]; 
#pragma HLS ARRAY_PARTITION variable=edge3_vecs complete
unsigned int offsets[NUM_VALID_PEs];
#pragma HLS ARRAY_PARTITION variable = offsets complete

// clear indices
for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
#pragma HLS UNROLL
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
	#pragma HLS UNROLL
		MISSBUFFER_edgeupdates_index[inst][v] = 0;
	}
}
unsigned int nn = 0;
unsigned int miss_added = 0;

for(unsigned int n=0; n<NUM_VALID_PEs; n++){ offsets[n] = 0; }	
TOP_APPLY_VERTEX_AND_EDGE_UPDATES: for(unsigned int p_u=0; p_u<globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS]; p_u++){
	unsigned int offset = (p_u * globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]);
	unsigned int max_limit = edgeupdatesptrs[0][offset + p_v].size; 
	unsigned int max_num_edges = edge_maps_buffer[0][p_u+1].offset - edge_maps_buffer[0][p_u].offset;

	unsigned int padding_factor = 1;
	unsigned int total_num_misses_ = 0; 
	unsigned int num_misses_ = 0;
	
	#ifdef _DEBUGMODE_KERNELPRINTS//4
	unsigned int sum = 0; for(unsigned int t=0; t<globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS]; t++){ sum += edgeupdatesptrs[0][offset + t].size; }
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<"apply-edges: [p_v: "<<p_v<<", p_u: "<<p_u<<"] edgeupdatesptrs[0]["<<offset + p_v<<"].size: "<<edgeupdatesptrs[0][offset + p_v].size<<" (of "<<sum<<")"<<endl; }
	// continue; // REMOVEME.
	#endif 
	
	#ifdef _DEBUGMODE_KERNELPRINTS4 
	if(action.fpga < num_prints && max_limit > 0){ cout<<"--- applying "<<max_limit * EDGE_PACK_SIZE<<" vertex/edge updates in vpartition "<<p_v<<", upartition "<<p_u<<", PE 0: [PEs "; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ cout<<n<<", "; } cout<<"] [target FPGAs "; for(unsigned int n=0; n<action.numfpgas; n++){ cout<<n<<", "; } cout<<"]"<<endl; } 
	#endif	

	// print summary
	#ifdef _DEBUGMODE_CHECKS3
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ num_misses_ += MISSBUFFER_edgeupdates_index[0][v]; }
	#endif 
	#ifdef _DEBUGMODE_KERNELPRINTS//4
	unsigned int tots_ = 0; unsigned int tots2_ = 0;
	for(unsigned int t=0; t<EDGE_UPDATES_DRAMBUFFER_SIZE; t++){ for(unsigned int inst=0; inst<1; inst++){ for(unsigned int v=0; v<1; v++){ if(URAM_map[inst][v][t].size > 0){ cout<<"------ URAM_map["<<inst<<"]["<<v<<"]["<<t<<"].offset: "<<URAM_map[inst][v][t].offset<<", URAM_map["<<inst<<"]["<<v<<"]["<<t<<"].size: "<<URAM_map[inst][v][t].size<<endl; tots_ += 1; tots2_ += URAM_map[inst][v][t].size; }}}}
	cout<<"acts-apply: summary: used "<<tots_<<" slots (of 8192 slots). tots2: "<<tots2_<<endl;
	#endif 	
	// exit(EXIT_SUCCESS);

	// hash edge updates to URAM 
	unsigned int chunksz = EDGE_UPDATES_CHUNKSZ; unsigned int batch_sz = 512; 
	if(chunksz >= edgeu_maps_buffer[0][p_u].size){ chunksz = edgeu_maps_buffer[0][p_u].size; } 
	unsigned int base_offset = globalparams[GLOBALPARAMSCODE__BASEOFFSET__EDGEUPDATES]; for(unsigned int n=0; n<NUM_VALID_PEs; n++){ offsets[n] = edge_maps_buffer[n][p_u].offset; }
	#ifdef _DEBUGMODE_KERNELPRINTS//4
	cout<<"~~~~~~~~~~~~~~ chunksz: "<<chunksz<<", max_num_edges: "<<max_num_edges<<", edge_maps_buffer[0]["<<p_u<<"].size: "<<edge_maps_buffer[0][p_u].size<<", max_limit: "<<max_limit<<" ~~~~~~~~~~~~~~"<<endl;
	#endif 
	#ifdef _DEBUGMODE_CHECKS3
	checkoutofbounds("acts_kernel::ERROR 88223::", chunksz, HBM_CHANNEL_SIZE, NAp, NAp, INVALIDDATA); 
	#endif
	
	unsigned int last_edge_hashid = 0;
	HASH_EDGEUPDATES_LOOP1: for(unsigned int t1=0; t1<(chunksz + (batch_sz-1)) / batch_sz; t1++){
		HASH_EDGEUPDATES_LOOP1B: for(unsigned int t2=0; t2<batch_sz; t2++){ 
		#pragma HLS PIPELINE II=2 
		#pragma HLS dependence variable=URAM_edgeupdates type=inter false // FIXME?
			unsigned int t = (t1 * batch_sz) + t2;
			dretrievemany_edges(base_offset, offsets, t, edge3_vecs,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
			
			for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
			#pragma HLS UNROLL 
				for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
				#pragma HLS UNROLL
					edge_update_type raw_edge_update = edge3_vecs[inst].data[v]; 
					edge_update_type edge_update; edge_update.srcvid = raw_edge_update.srcvid; edge_update.dstvid = raw_edge_update.dstvid;
					
					// tmp_buffer[inst][t].data[v] = edge_update;
					#ifdef _DEBUGMODE_CHECKS3
					if(raw_edge_update.srcvid != INVALIDDATA && t < chunksz){ checkoutofbounds("acts_kernel::ERROR 8813::", edge_update.srcvid, MAX_UPARTITION_SIZE, t, raw_edge_update.srcvid, INVALIDDATA); }
					if(raw_edge_update.srcvid != INVALIDDATA && t < chunksz){ checkoutofbounds("acts_kernel::ERROR 7713::", edge_update.srcvid, EDGE_UPDATES_DRAMBUFFER_SIZE, t, v, inst); }
					#endif
					
					if(raw_edge_update.srcvid != INVALIDDATA && t < chunksz){
						unsigned int edge_hashid = hash_edge(edge_update.srcvid, edge_update.dstvid, NAp); 
						
						bool en = true; 
						if((t > 0) && (last_edge_hashid == edge_hashid)){ en = false; } 
						last_edge_hashid = edge_hashid;
						#ifdef _DEBUGMODE_KERNELPRINTS//4 
						if(inst==0 && p_u==0 && t<2){ cout<<"$$$ load-edge-updates::["<<inst<<"]["<<t<<"]["<<v<<"]: [edge_update-update: srcvid: "<<edge_update.srcvid<<", dstvid: "<<edge_update.dstvid<<"]---[edge_hashid: "<<edge_hashid<<"]"<<endl; }								
						#endif	
						
						if(URAM_edgeupdates[inst][v][edge_hashid].srcvid == INVALIDDATA && en == true){
							#ifdef _DEBUGMODE_KERNELPRINTS//4 
							if(inst==0){ cout<<"$$$ load-edge-updates::["<<inst<<"]["<<t<<"]["<<v<<"]: hit @ "<<edge_hashid<<". [edge_update-update: srcvid: "<<edge_update.srcvid<<", dstvid: "<<edge_update.dstvid<<"]"<<endl; }								
							#endif	
							URAM_edgeupdates[inst][v][edge_hashid] = edge_update; 
						} else {
							#ifdef _DEBUGMODE_KERNELPRINTS//4 
							if(inst==0 && MISSBUFFER_edgeupdates_index[inst][v] < 64){ cout<<"$$$ load-edge-updates::["<<inst<<"]["<<t<<"]["<<v<<"]: miss @ "<<edge_hashid<<". [edge_update-update: srcvid: "<<edge_update.srcvid<<", dstvid: "<<edge_update.dstvid<<"]"<<endl; }								
							#endif	
							#ifdef _DEBUGMODE_CHECKS3
							checkoutofbounds("acts_kernel::ERROR 68813::", MISSBUFFER_edgeupdates_index[inst][v], MISS_BUFFER_SZ, t, NAp, INVALIDDATA); 
							#endif
							MISSBUFFER_edgeupdates[inst][v][MISSBUFFER_edgeupdates_index[inst][v]] = edge_update;	
							// if(MISSBUFFER_edgeupdates_index[inst][v] < MISS_BUFFER_SZ-1){ MISSBUFFER_edgeupdates_index[inst][v] += 1; } // FIXME.
							MISSBUFFER_edgeupdates_index[inst][v] += 1;
						}
					}
				}
			}
		}
	}
	// exit(EXIT_SUCCESS);

	// print summary
	#ifdef _DEBUGMODE_CHECKS3
	for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ total_num_misses_ += MISSBUFFER_edgeupdates_index[0][v]; }
	#endif 
	#ifdef _DEBUGMODE_APPLYEDGEUPDATES_PRINTS//4
	if(true){ for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ cout<<"acts-apply: MISSBUFFER_edgeupdates_index[0]["<<v<<"]: "<<MISSBUFFER_edgeupdates_index[0][v]<<endl; }}
	cout<<"+++ apply-edge-updates: miss summary: total number of misses: "<<total_num_misses_<<" (of "<<EDGE_UPDATES_CHUNKSZ * EDGE_PACK_SIZE<<") ( "<<(total_num_misses_ * 100) / (EDGE_UPDATES_CHUNKSZ * EDGE_PACK_SIZE)<<"%)"<<endl;
	#endif 
	// exit(EXIT_SUCCESS);

	// (1) apply vertex updates to vertices; (2) edge updates to edges
	unsigned int num_edges_updated = 0; 
	unsigned int num_edges_inserted = 0; 
	uint512_vec_dt updates_vecs[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=updates_vecs complete
	APPLY_UPDATES_LOOP: for(unsigned int it=0; it<(max_limit + BLOCKRAM_SIZE - 1) / BLOCKRAM_SIZE; it++){
		unsigned int sz = BLOCKRAM_SIZE; if((it + 1) * BLOCKRAM_SIZE > max_limit){ sz = max_limit - (it * BLOCKRAM_SIZE); } if(action.command == GRAPH_UPDATE_ONLY){ sz = 0; }
		unsigned int sz2 = sz; if(chunksz == 0){ sz2 = 0; } 
		unsigned int updates_offset = globalparams[GLOBALPARAMSCODE__BASEOFFSET__VERTEXUPDATES] + updatesptrs[p_v].offset + (it * BLOCKRAM_SIZE);
		unsigned int edges_offset = globalparams[GLOBALPARAMSCODE__BASEOFFSET__VERTEXUPDATES] + updatesptrs[p_v].offset + (it * BLOCKRAM_SIZE);
		
		APPLY_VERTEX_UPDATES_LOOP: for(unsigned int t=0; t<sz; t++){				
		#pragma HLS PIPELINE II=3 // <<<FIXME. II too long>>>
			dretrievemany_udatesdram(updates_offset + t, updates_vecs,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); // NEW
			
			for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
			#pragma HLS UNROLL
				for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 
				#pragma HLS UNROLL
					tmp_buffer2[inst][t].data[v].srcvid = updates_vecs[inst].data[v].key;
					tmp_buffer2[inst][t].data[v].dstvid = updates_vecs[inst].data[v].value;
				}
			}
			
			for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 
			#pragma HLS UNROLL
				uint512_vec_dt updates_vec = updates_vecs[inst];
				for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){		
				#pragma HLS UNROLL
					keyvalue_t update = updates_vec.data[v];
					unsigned int dstvid_lp = update.key; 
					if(update.key >= MAX_APPLYPARTITION_SIZE){ dstvid_lp = 0; }
					unsigned int dstvid_lpv = dstvid_lp / EDGE_PACK_SIZE;	
					
					#ifdef _DEBUGMODE_KERNELPRINTS//4
					if(t< 160 || true){ std::cout<<"SAMPLE VERTEX UPDATE (APPLY) @: t: "<<t<<", inst: "<<inst<<", v: "<<v<<", update.key: "<<update.key<<", update.value: "<<update.value<<std::endl; }
					#endif
					
					#ifdef _DEBUGMODE_CHECKS3
					if(t < limits[inst]){ checkoutofbounds("acts_kernel::ERROR 727a::", dstvid_lp, MAX_APPLYPARTITION_SIZE, NAp, inst, update.key); checkoutofbounds("acts_kernel::ERROR 727b::", dstvid_lpv, MAX_APPLYPARTITION_VECSIZE, NAp, inst, update.key); }					
					#endif
					vprop_dest_t vprop = URAM_vprop[inst][v][dstvid_lpv];
					vprop_dest_t newprop; newprop.prop = INVALIDDATA; 
					unsigned int new_vprop = reduce_funcG(vprop, update.value, globalparams[GLOBALPARAMSCODE__PARAM__ALGORITHM]);
					if(t < limits[inst]){ 
						#ifdef _DEBUGMODE_KERNELPRINTS_TRACE3
						std::cout<<"APPLY (ACT-PACK) UPDATE SEEN @: t: "<<t<<", inst: ["<<inst<<"]: dstvid_lp: "<<dstvid_lp<<", dstvid_lpv: "<<dstvid_lpv<<", new_vprop: "<<new_vprop<<", vid: "<<update.key<<std::endl;
						#endif
						newprop.prop = new_vprop; 
					}
					URAM_vprop[inst][v][dstvid_lpv].prop = newprop.prop; 
					#ifdef _DEBUGMODE_CHECKS3
					if(inst==0 && v==0){ update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___APPLYUPDATES___, 2); }
					#endif 
				}
			}
			
			#ifdef ___ENABLE___IMPORT_EXPORT___
			hidden_import_and_export_function(t, import_offset, export_offset, action.size_import_export, HBM_centerA, HBM_centerB, HBM_import, HBM_export, action);
			#endif 
		}
		
		APPLY_EDGE_UPDATES_LOOP: for(unsigned int t=0; t<sz2; t++){
		#pragma HLS PIPELINE II=1
			for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
			#pragma HLS UNROLL
				edge3_vecs[inst] = tmp_buffer2[inst][t];
				for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 
				#pragma HLS UNROLL	
					edge3_type edge = edge3_vecs[inst].data[v];
					unsigned int weight = edge.weight;	

					map_t map; map.size = 0;
					if(edge.srcvid < EDGE_UPDATES_DRAMBUFFER_SIZE){ map = URAM_map[inst][v][edge.srcvid]; }
					
					unsigned int edge_hashid = hash_edge(edge.srcvid, edge.dstvid, NAp); 
					unsigned int eu_weight = 1;
					
					#ifdef _DEBUGMODE_KERNELPRINTS//4 
					if(inst==0 && p_u==0 && llp_set==0 && t<8){ cout<<"### process-edge-updates::["<<inst<<"]["<<t<<"]["<<v<<"]:edges: srcvid: "<<edge.srcvid<<", dstvid: "<<edge.dstvid<<"]---[edge: srcvid: "<<edge.srcvid<<", dstvid: "<<edge.dstvid<<"]---[edge_hashid: "<<edge_hashid<<"]"<<endl; }								
					#endif 
					
					edge_update_type edge_update = URAM_edgeupdates[inst][v][edge_hashid];
					if(edge.srcvid != INVALIDDATA && edge.srcvid == edge_update.srcvid && edge.dstvid == edge_update.dstvid){		
						// edge3_vecs[inst].data[v].weight = eu_weight; // += eu_weight; II too long>>>		
						edge3_vecs[inst].data[v].dstvid += 1; 
						#ifdef _DEBUGMODE_CHECKS3
						if(inst==0){ num_edges_updated += 1; }
						if(inst==0 && false){ cout<<"[process-edge-updates:: hit seen @ ["<<inst<<"]["<<t<<"]["<<v<<"]: [edge.srcvid: "<<edge.srcvid<<", edge.dstvid: "<<edge.dstvid<<"] [edge_update.srcvid: "<<edge_update.srcvid<<", edge_update.dstvid: "<<edge_update.dstvid<<"], edge_hashid: "<<edge_hashid<<"]"<<endl; }
						#endif 
						if(inst==0 && v==0){ update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___NUMBER_OF_EDGE_UPDATINGS___, 1); }
					}
				}
			}
		
			dinsertmany_edgesdram(edges_offset + t, edge3_vecs,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4);
		}
	}
	// exit(EXIT_SUCCESS);	
	
	// print summary
	#ifdef _DEBUGMODE_APPLYEDGEUPDATES_PRINTS//4
	cout<<"apply-edges: summary: total number of edges updated : "<<num_edges_updated<<" (of "<<max_limit * EDGE_PACK_SIZE<<")"<<endl;
	#endif 
	// exit(EXIT_SUCCESS);	
	
	// (1) insert new edges; (2) reset URAM buffers
	#ifdef ___NOT___IMPLEMENTED___
	EDGE_INSERTIONS_LOOP: for(unsigned int t=0; t<chunksz; t++){ 
	#pragma HLS PIPELINE II=1
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
			#pragma HLS UNROLL
				edge_update_type edge_update = tmp_buffer[inst][t].data[v];
				unsigned int edge_hashid = hash_edge(edge_update.srcvid, edge_update.dstvid, NAp); 
				
				if(edge_update.srcvid < EDGE_UPDATES_DRAMBUFFER_SIZE){ 
					URAM_edgeupdates[inst][v][edge_hashid].srcvid = INVALIDDATA; //
					URAM_edgeupdates[inst][v][edge_hashid].dstvid = INVALIDDATA; //

					#ifdef _DEBUGMODE_CHECKS3
					if(inst==0){ num_edges_inserted += 1; }
					#endif 
					if(inst==0 && v==0){ update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___NUMBER_OF_EDGE_INSERTIONS___, 1); }
				}				
			}
		}
		dinsertmany_edgesdram(globalparams[GLOBALPARAMSCODE__BASEOFFSET__ACTPACKEDGES] + edge_maps_buffer[0][p_u].offset + edge_maps_buffer[0][p_u].size + t, edge3_vecs,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); 
	}
	#endif 

	// (1) insert new edges; (2) reset URAM buffers
	// #ifdef ___NOT___IMPLEMENTED___
	unsigned int data[NUM_VALID_PEs][HBM_AXI_PACK_SIZE]; 
	#pragma HLS ARRAY_PARTITION variable=data complete dim=0
	unsigned int src_offset = globalparams[GLOBALPARAMSCODE__BASEOFFSET__EDGEUPDATES];
	unsigned int dest_offset = globalparams[GLOBALPARAMSCODE__BASEOFFSET__ACTPACKEDGES] + edge_maps_buffer[0][p_u].offset + edge_maps_buffer[0][p_u].size;
	EDGE_INSERTIONS_LOOP1: for(unsigned int t=0; t<chunksz; t++){ 
	#pragma HLS PIPELINE II=1		
		data[0][0] = HBM_channelA0[src_offset + offsets[0] + t].data[0];
		data[0][1] = HBM_channelA0[src_offset + offsets[0] + t].data[1];
		data[0][2] = HBM_channelA0[src_offset + offsets[0] + t].data[2];
		data[0][3] = HBM_channelA0[src_offset + offsets[0] + t].data[3];
		data[0][4] = HBM_channelA0[src_offset + offsets[0] + t].data[4];
		data[0][5] = HBM_channelA0[src_offset + offsets[0] + t].data[5];
		data[0][6] = HBM_channelA0[src_offset + offsets[0] + t].data[6];
		data[0][7] = HBM_channelA0[src_offset + offsets[0] + t].data[7];
		data[0][8] = HBM_channelA0[src_offset + offsets[0] + t].data[8];
		data[0][9] = HBM_channelA0[src_offset + offsets[0] + t].data[9];
		data[0][10] = HBM_channelA0[src_offset + offsets[0] + t].data[10];
		data[0][11] = HBM_channelA0[src_offset + offsets[0] + t].data[11];
		data[0][12] = HBM_channelA0[src_offset + offsets[0] + t].data[12];
		data[0][13] = HBM_channelA0[src_offset + offsets[0] + t].data[13];
		data[0][14] = HBM_channelA0[src_offset + offsets[0] + t].data[14];
		data[0][15] = HBM_channelA0[src_offset + offsets[0] + t].data[15];
		data[1][0] = HBM_channelA1[src_offset + offsets[1] + t].data[0];
		data[1][1] = HBM_channelA1[src_offset + offsets[1] + t].data[1];
		data[1][2] = HBM_channelA1[src_offset + offsets[1] + t].data[2];
		data[1][3] = HBM_channelA1[src_offset + offsets[1] + t].data[3];
		data[1][4] = HBM_channelA1[src_offset + offsets[1] + t].data[4];
		data[1][5] = HBM_channelA1[src_offset + offsets[1] + t].data[5];
		data[1][6] = HBM_channelA1[src_offset + offsets[1] + t].data[6];
		data[1][7] = HBM_channelA1[src_offset + offsets[1] + t].data[7];
		data[1][8] = HBM_channelA1[src_offset + offsets[1] + t].data[8];
		data[1][9] = HBM_channelA1[src_offset + offsets[1] + t].data[9];
		data[1][10] = HBM_channelA1[src_offset + offsets[1] + t].data[10];
		data[1][11] = HBM_channelA1[src_offset + offsets[1] + t].data[11];
		data[1][12] = HBM_channelA1[src_offset + offsets[1] + t].data[12];
		data[1][13] = HBM_channelA1[src_offset + offsets[1] + t].data[13];
		data[1][14] = HBM_channelA1[src_offset + offsets[1] + t].data[14];
		data[1][15] = HBM_channelA1[src_offset + offsets[1] + t].data[15];
		data[2][0] = HBM_channelA2[src_offset + offsets[2] + t].data[0];
		data[2][1] = HBM_channelA2[src_offset + offsets[2] + t].data[1];
		data[2][2] = HBM_channelA2[src_offset + offsets[2] + t].data[2];
		data[2][3] = HBM_channelA2[src_offset + offsets[2] + t].data[3];
		data[2][4] = HBM_channelA2[src_offset + offsets[2] + t].data[4];
		data[2][5] = HBM_channelA2[src_offset + offsets[2] + t].data[5];
		data[2][6] = HBM_channelA2[src_offset + offsets[2] + t].data[6];
		data[2][7] = HBM_channelA2[src_offset + offsets[2] + t].data[7];
		data[2][8] = HBM_channelA2[src_offset + offsets[2] + t].data[8];
		data[2][9] = HBM_channelA2[src_offset + offsets[2] + t].data[9];
		data[2][10] = HBM_channelA2[src_offset + offsets[2] + t].data[10];
		data[2][11] = HBM_channelA2[src_offset + offsets[2] + t].data[11];
		data[2][12] = HBM_channelA2[src_offset + offsets[2] + t].data[12];
		data[2][13] = HBM_channelA2[src_offset + offsets[2] + t].data[13];
		data[2][14] = HBM_channelA2[src_offset + offsets[2] + t].data[14];
		data[2][15] = HBM_channelA2[src_offset + offsets[2] + t].data[15];
		data[3][0] = HBM_channelA3[src_offset + offsets[3] + t].data[0];
		data[3][1] = HBM_channelA3[src_offset + offsets[3] + t].data[1];
		data[3][2] = HBM_channelA3[src_offset + offsets[3] + t].data[2];
		data[3][3] = HBM_channelA3[src_offset + offsets[3] + t].data[3];
		data[3][4] = HBM_channelA3[src_offset + offsets[3] + t].data[4];
		data[3][5] = HBM_channelA3[src_offset + offsets[3] + t].data[5];
		data[3][6] = HBM_channelA3[src_offset + offsets[3] + t].data[6];
		data[3][7] = HBM_channelA3[src_offset + offsets[3] + t].data[7];
		data[3][8] = HBM_channelA3[src_offset + offsets[3] + t].data[8];
		data[3][9] = HBM_channelA3[src_offset + offsets[3] + t].data[9];
		data[3][10] = HBM_channelA3[src_offset + offsets[3] + t].data[10];
		data[3][11] = HBM_channelA3[src_offset + offsets[3] + t].data[11];
		data[3][12] = HBM_channelA3[src_offset + offsets[3] + t].data[12];
		data[3][13] = HBM_channelA3[src_offset + offsets[3] + t].data[13];
		data[3][14] = HBM_channelA3[src_offset + offsets[3] + t].data[14];
		data[3][15] = HBM_channelA3[src_offset + offsets[3] + t].data[15];
		data[4][0] = HBM_channelA4[src_offset + offsets[4] + t].data[0];
		data[4][1] = HBM_channelA4[src_offset + offsets[4] + t].data[1];
		data[4][2] = HBM_channelA4[src_offset + offsets[4] + t].data[2];
		data[4][3] = HBM_channelA4[src_offset + offsets[4] + t].data[3];
		data[4][4] = HBM_channelA4[src_offset + offsets[4] + t].data[4];
		data[4][5] = HBM_channelA4[src_offset + offsets[4] + t].data[5];
		data[4][6] = HBM_channelA4[src_offset + offsets[4] + t].data[6];
		data[4][7] = HBM_channelA4[src_offset + offsets[4] + t].data[7];
		data[4][8] = HBM_channelA4[src_offset + offsets[4] + t].data[8];
		data[4][9] = HBM_channelA4[src_offset + offsets[4] + t].data[9];
		data[4][10] = HBM_channelA4[src_offset + offsets[4] + t].data[10];
		data[4][11] = HBM_channelA4[src_offset + offsets[4] + t].data[11];
		data[4][12] = HBM_channelA4[src_offset + offsets[4] + t].data[12];
		data[4][13] = HBM_channelA4[src_offset + offsets[4] + t].data[13];
		data[4][14] = HBM_channelA4[src_offset + offsets[4] + t].data[14];
		data[4][15] = HBM_channelA4[src_offset + offsets[4] + t].data[15];

		edge3_vec_dt edges;
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE/2; v++){
			#pragma HLS UNROLL
				edges.data[v].srcvid = data[inst][2*v];
				edges.data[v].dstvid = data[inst][2*v+1];
				
				unsigned int edge_hashid = hash_edge(edges.data[v].srcvid, edges.data[v].dstvid, NAp); 
			
				if(edges.data[v].srcvid < EDGE_UPDATES_DRAMBUFFER_SIZE){ 
					URAM_edgeupdates[inst][v][edge_hashid].srcvid = INVALIDDATA; //
					URAM_edgeupdates[inst][v][edge_hashid].dstvid = INVALIDDATA; //
				}
			}
		}
	
		// HBM_channelB0[dest_offset + t].data[0] = data[0][0]; 
		// HBM_channelB0[dest_offset + t].data[1] = data[0][1]; 
		// HBM_channelB0[dest_offset + t].data[2] = data[0][2]; 
		// HBM_channelB0[dest_offset + t].data[3] = data[0][3]; 
		// HBM_channelB0[dest_offset + t].data[4] = data[0][4]; 
		// HBM_channelB0[dest_offset + t].data[5] = data[0][5]; 
		// HBM_channelB0[dest_offset + t].data[6] = data[0][6]; 
		// HBM_channelB0[dest_offset + t].data[7] = data[0][7]; 
		// HBM_channelB0[dest_offset + t].data[8] = data[0][8]; 
		// HBM_channelB0[dest_offset + t].data[9] = data[0][9]; 
		// HBM_channelB0[dest_offset + t].data[10] = data[0][10]; 
		// HBM_channelB0[dest_offset + t].data[11] = data[0][11]; 
		// HBM_channelB0[dest_offset + t].data[12] = data[0][12]; 
		// HBM_channelB0[dest_offset + t].data[13] = data[0][13]; 
		// HBM_channelB0[dest_offset + t].data[14] = data[0][14]; 
		// HBM_channelB0[dest_offset + t].data[15] = data[0][15]; 
		// HBM_channelB1[dest_offset + t].data[0] = data[1][0]; 
		// HBM_channelB1[dest_offset + t].data[1] = data[1][1]; 
		// HBM_channelB1[dest_offset + t].data[2] = data[1][2]; 
		// HBM_channelB1[dest_offset + t].data[3] = data[1][3]; 
		// HBM_channelB1[dest_offset + t].data[4] = data[1][4]; 
		// HBM_channelB1[dest_offset + t].data[5] = data[1][5]; 
		// HBM_channelB1[dest_offset + t].data[6] = data[1][6]; 
		// HBM_channelB1[dest_offset + t].data[7] = data[1][7]; 
		// HBM_channelB1[dest_offset + t].data[8] = data[1][8]; 
		// HBM_channelB1[dest_offset + t].data[9] = data[1][9]; 
		// HBM_channelB1[dest_offset + t].data[10] = data[1][10]; 
		// HBM_channelB1[dest_offset + t].data[11] = data[1][11]; 
		// HBM_channelB1[dest_offset + t].data[12] = data[1][12]; 
		// HBM_channelB1[dest_offset + t].data[13] = data[1][13]; 
		// HBM_channelB1[dest_offset + t].data[14] = data[1][14]; 
		// HBM_channelB1[dest_offset + t].data[15] = data[1][15]; 
		// HBM_channelB2[dest_offset + t].data[0] = data[2][0]; 
		// HBM_channelB2[dest_offset + t].data[1] = data[2][1]; 
		// HBM_channelB2[dest_offset + t].data[2] = data[2][2]; 
		// HBM_channelB2[dest_offset + t].data[3] = data[2][3]; 
		// HBM_channelB2[dest_offset + t].data[4] = data[2][4]; 
		// HBM_channelB2[dest_offset + t].data[5] = data[2][5]; 
		// HBM_channelB2[dest_offset + t].data[6] = data[2][6]; 
		// HBM_channelB2[dest_offset + t].data[7] = data[2][7]; 
		// HBM_channelB2[dest_offset + t].data[8] = data[2][8]; 
		// HBM_channelB2[dest_offset + t].data[9] = data[2][9]; 
		// HBM_channelB2[dest_offset + t].data[10] = data[2][10]; 
		// HBM_channelB2[dest_offset + t].data[11] = data[2][11]; 
		// HBM_channelB2[dest_offset + t].data[12] = data[2][12]; 
		// HBM_channelB2[dest_offset + t].data[13] = data[2][13]; 
		// HBM_channelB2[dest_offset + t].data[14] = data[2][14]; 
		// HBM_channelB2[dest_offset + t].data[15] = data[2][15]; 
		// HBM_channelB3[dest_offset + t].data[0] = data[3][0]; 
		// HBM_channelB3[dest_offset + t].data[1] = data[3][1]; 
		// HBM_channelB3[dest_offset + t].data[2] = data[3][2]; 
		// HBM_channelB3[dest_offset + t].data[3] = data[3][3]; 
		// HBM_channelB3[dest_offset + t].data[4] = data[3][4]; 
		// HBM_channelB3[dest_offset + t].data[5] = data[3][5]; 
		// HBM_channelB3[dest_offset + t].data[6] = data[3][6]; 
		// HBM_channelB3[dest_offset + t].data[7] = data[3][7]; 
		// HBM_channelB3[dest_offset + t].data[8] = data[3][8]; 
		// HBM_channelB3[dest_offset + t].data[9] = data[3][9]; 
		// HBM_channelB3[dest_offset + t].data[10] = data[3][10]; 
		// HBM_channelB3[dest_offset + t].data[11] = data[3][11]; 
		// HBM_channelB3[dest_offset + t].data[12] = data[3][12]; 
		// HBM_channelB3[dest_offset + t].data[13] = data[3][13]; 
		// HBM_channelB3[dest_offset + t].data[14] = data[3][14]; 
		// HBM_channelB3[dest_offset + t].data[15] = data[3][15]; 
		// HBM_channelB4[dest_offset + t].data[0] = data[4][0]; 
		// HBM_channelB4[dest_offset + t].data[1] = data[4][1]; 
		// HBM_channelB4[dest_offset + t].data[2] = data[4][2]; 
		// HBM_channelB4[dest_offset + t].data[3] = data[4][3]; 
		// HBM_channelB4[dest_offset + t].data[4] = data[4][4]; 
		// HBM_channelB4[dest_offset + t].data[5] = data[4][5]; 
		// HBM_channelB4[dest_offset + t].data[6] = data[4][6]; 
		// HBM_channelB4[dest_offset + t].data[7] = data[4][7]; 
		// HBM_channelB4[dest_offset + t].data[8] = data[4][8]; 
		// HBM_channelB4[dest_offset + t].data[9] = data[4][9]; 
		// HBM_channelB4[dest_offset + t].data[10] = data[4][10]; 
		// HBM_channelB4[dest_offset + t].data[11] = data[4][11]; 
		// HBM_channelB4[dest_offset + t].data[12] = data[4][12]; 
		// HBM_channelB4[dest_offset + t].data[13] = data[4][13]; 
		// HBM_channelB4[dest_offset + t].data[14] = data[4][14]; 
		// HBM_channelB4[dest_offset + t].data[15] = data[4][15]; 
		
		#ifdef _DEBUGMODE_CHECKS3
		num_edges_inserted += 1; 
		#endif 
		update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___NUMBER_OF_EDGE_INSERTIONS___, 1); 
	}	
	EDGE_INSERTIONS_LOOP2: for(unsigned int t=0; t<chunksz; t++){ 
	#pragma HLS PIPELINE II=1		
		data[0][0] = HBM_channelB0[src_offset + offsets[0] + t].data[0];
		data[0][1] = HBM_channelB0[src_offset + offsets[0] + t].data[1];
		data[0][2] = HBM_channelB0[src_offset + offsets[0] + t].data[2];
		data[0][3] = HBM_channelB0[src_offset + offsets[0] + t].data[3];
		data[0][4] = HBM_channelB0[src_offset + offsets[0] + t].data[4];
		data[0][5] = HBM_channelB0[src_offset + offsets[0] + t].data[5];
		data[0][6] = HBM_channelB0[src_offset + offsets[0] + t].data[6];
		data[0][7] = HBM_channelB0[src_offset + offsets[0] + t].data[7];
		data[0][8] = HBM_channelB0[src_offset + offsets[0] + t].data[8];
		data[0][9] = HBM_channelB0[src_offset + offsets[0] + t].data[9];
		data[0][10] = HBM_channelB0[src_offset + offsets[0] + t].data[10];
		data[0][11] = HBM_channelB0[src_offset + offsets[0] + t].data[11];
		data[0][12] = HBM_channelB0[src_offset + offsets[0] + t].data[12];
		data[0][13] = HBM_channelB0[src_offset + offsets[0] + t].data[13];
		data[0][14] = HBM_channelB0[src_offset + offsets[0] + t].data[14];
		data[0][15] = HBM_channelB0[src_offset + offsets[0] + t].data[15];
		data[1][0] = HBM_channelB1[src_offset + offsets[1] + t].data[0];
		data[1][1] = HBM_channelB1[src_offset + offsets[1] + t].data[1];
		data[1][2] = HBM_channelB1[src_offset + offsets[1] + t].data[2];
		data[1][3] = HBM_channelB1[src_offset + offsets[1] + t].data[3];
		data[1][4] = HBM_channelB1[src_offset + offsets[1] + t].data[4];
		data[1][5] = HBM_channelB1[src_offset + offsets[1] + t].data[5];
		data[1][6] = HBM_channelB1[src_offset + offsets[1] + t].data[6];
		data[1][7] = HBM_channelB1[src_offset + offsets[1] + t].data[7];
		data[1][8] = HBM_channelB1[src_offset + offsets[1] + t].data[8];
		data[1][9] = HBM_channelB1[src_offset + offsets[1] + t].data[9];
		data[1][10] = HBM_channelB1[src_offset + offsets[1] + t].data[10];
		data[1][11] = HBM_channelB1[src_offset + offsets[1] + t].data[11];
		data[1][12] = HBM_channelB1[src_offset + offsets[1] + t].data[12];
		data[1][13] = HBM_channelB1[src_offset + offsets[1] + t].data[13];
		data[1][14] = HBM_channelB1[src_offset + offsets[1] + t].data[14];
		data[1][15] = HBM_channelB1[src_offset + offsets[1] + t].data[15];
		data[2][0] = HBM_channelB2[src_offset + offsets[2] + t].data[0];
		data[2][1] = HBM_channelB2[src_offset + offsets[2] + t].data[1];
		data[2][2] = HBM_channelB2[src_offset + offsets[2] + t].data[2];
		data[2][3] = HBM_channelB2[src_offset + offsets[2] + t].data[3];
		data[2][4] = HBM_channelB2[src_offset + offsets[2] + t].data[4];
		data[2][5] = HBM_channelB2[src_offset + offsets[2] + t].data[5];
		data[2][6] = HBM_channelB2[src_offset + offsets[2] + t].data[6];
		data[2][7] = HBM_channelB2[src_offset + offsets[2] + t].data[7];
		data[2][8] = HBM_channelB2[src_offset + offsets[2] + t].data[8];
		data[2][9] = HBM_channelB2[src_offset + offsets[2] + t].data[9];
		data[2][10] = HBM_channelB2[src_offset + offsets[2] + t].data[10];
		data[2][11] = HBM_channelB2[src_offset + offsets[2] + t].data[11];
		data[2][12] = HBM_channelB2[src_offset + offsets[2] + t].data[12];
		data[2][13] = HBM_channelB2[src_offset + offsets[2] + t].data[13];
		data[2][14] = HBM_channelB2[src_offset + offsets[2] + t].data[14];
		data[2][15] = HBM_channelB2[src_offset + offsets[2] + t].data[15];
		data[3][0] = HBM_channelB3[src_offset + offsets[3] + t].data[0];
		data[3][1] = HBM_channelB3[src_offset + offsets[3] + t].data[1];
		data[3][2] = HBM_channelB3[src_offset + offsets[3] + t].data[2];
		data[3][3] = HBM_channelB3[src_offset + offsets[3] + t].data[3];
		data[3][4] = HBM_channelB3[src_offset + offsets[3] + t].data[4];
		data[3][5] = HBM_channelB3[src_offset + offsets[3] + t].data[5];
		data[3][6] = HBM_channelB3[src_offset + offsets[3] + t].data[6];
		data[3][7] = HBM_channelB3[src_offset + offsets[3] + t].data[7];
		data[3][8] = HBM_channelB3[src_offset + offsets[3] + t].data[8];
		data[3][9] = HBM_channelB3[src_offset + offsets[3] + t].data[9];
		data[3][10] = HBM_channelB3[src_offset + offsets[3] + t].data[10];
		data[3][11] = HBM_channelB3[src_offset + offsets[3] + t].data[11];
		data[3][12] = HBM_channelB3[src_offset + offsets[3] + t].data[12];
		data[3][13] = HBM_channelB3[src_offset + offsets[3] + t].data[13];
		data[3][14] = HBM_channelB3[src_offset + offsets[3] + t].data[14];
		data[3][15] = HBM_channelB3[src_offset + offsets[3] + t].data[15];
		data[4][0] = HBM_channelB4[src_offset + offsets[4] + t].data[0];
		data[4][1] = HBM_channelB4[src_offset + offsets[4] + t].data[1];
		data[4][2] = HBM_channelB4[src_offset + offsets[4] + t].data[2];
		data[4][3] = HBM_channelB4[src_offset + offsets[4] + t].data[3];
		data[4][4] = HBM_channelB4[src_offset + offsets[4] + t].data[4];
		data[4][5] = HBM_channelB4[src_offset + offsets[4] + t].data[5];
		data[4][6] = HBM_channelB4[src_offset + offsets[4] + t].data[6];
		data[4][7] = HBM_channelB4[src_offset + offsets[4] + t].data[7];
		data[4][8] = HBM_channelB4[src_offset + offsets[4] + t].data[8];
		data[4][9] = HBM_channelB4[src_offset + offsets[4] + t].data[9];
		data[4][10] = HBM_channelB4[src_offset + offsets[4] + t].data[10];
		data[4][11] = HBM_channelB4[src_offset + offsets[4] + t].data[11];
		data[4][12] = HBM_channelB4[src_offset + offsets[4] + t].data[12];
		data[4][13] = HBM_channelB4[src_offset + offsets[4] + t].data[13];
		data[4][14] = HBM_channelB4[src_offset + offsets[4] + t].data[14];
		data[4][15] = HBM_channelB4[src_offset + offsets[4] + t].data[15];

		edge3_vec_dt edges;
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE/2; v++){
			#pragma HLS UNROLL
				edges.data[v].srcvid = data[inst][2*v];
				edges.data[v].dstvid = data[inst][2*v+1];
				
				unsigned int edge_hashid = hash_edge(edges.data[v].srcvid, edges.data[v].dstvid, NAp); 
			
				if(edges.data[v].srcvid < EDGE_UPDATES_DRAMBUFFER_SIZE){ 
					URAM_edgeupdates[inst][EDGE_PACK_SIZE/2 + v][edge_hashid].srcvid = INVALIDDATA; //
					URAM_edgeupdates[inst][EDGE_PACK_SIZE/2 + v][edge_hashid].dstvid = INVALIDDATA; //
				}
			}
		}
	
		HBM_channelA0[dest_offset + t].data[0] = data[0][0]; 
		HBM_channelA0[dest_offset + t].data[1] = data[0][1]; 
		HBM_channelA0[dest_offset + t].data[2] = data[0][2]; 
		HBM_channelA0[dest_offset + t].data[3] = data[0][3]; 
		HBM_channelA0[dest_offset + t].data[4] = data[0][4]; 
		HBM_channelA0[dest_offset + t].data[5] = data[0][5]; 
		HBM_channelA0[dest_offset + t].data[6] = data[0][6]; 
		HBM_channelA0[dest_offset + t].data[7] = data[0][7]; 
		HBM_channelA0[dest_offset + t].data[8] = data[0][8]; 
		HBM_channelA0[dest_offset + t].data[9] = data[0][9]; 
		HBM_channelA0[dest_offset + t].data[10] = data[0][10]; 
		HBM_channelA0[dest_offset + t].data[11] = data[0][11]; 
		HBM_channelA0[dest_offset + t].data[12] = data[0][12]; 
		HBM_channelA0[dest_offset + t].data[13] = data[0][13]; 
		HBM_channelA0[dest_offset + t].data[14] = data[0][14]; 
		HBM_channelA0[dest_offset + t].data[15] = data[0][15]; 
		HBM_channelA1[dest_offset + t].data[0] = data[1][0]; 
		HBM_channelA1[dest_offset + t].data[1] = data[1][1]; 
		HBM_channelA1[dest_offset + t].data[2] = data[1][2]; 
		HBM_channelA1[dest_offset + t].data[3] = data[1][3]; 
		HBM_channelA1[dest_offset + t].data[4] = data[1][4]; 
		HBM_channelA1[dest_offset + t].data[5] = data[1][5]; 
		HBM_channelA1[dest_offset + t].data[6] = data[1][6]; 
		HBM_channelA1[dest_offset + t].data[7] = data[1][7]; 
		HBM_channelA1[dest_offset + t].data[8] = data[1][8]; 
		HBM_channelA1[dest_offset + t].data[9] = data[1][9]; 
		HBM_channelA1[dest_offset + t].data[10] = data[1][10]; 
		HBM_channelA1[dest_offset + t].data[11] = data[1][11]; 
		HBM_channelA1[dest_offset + t].data[12] = data[1][12]; 
		HBM_channelA1[dest_offset + t].data[13] = data[1][13]; 
		HBM_channelA1[dest_offset + t].data[14] = data[1][14]; 
		HBM_channelA1[dest_offset + t].data[15] = data[1][15]; 
		HBM_channelA2[dest_offset + t].data[0] = data[2][0]; 
		HBM_channelA2[dest_offset + t].data[1] = data[2][1]; 
		HBM_channelA2[dest_offset + t].data[2] = data[2][2]; 
		HBM_channelA2[dest_offset + t].data[3] = data[2][3]; 
		HBM_channelA2[dest_offset + t].data[4] = data[2][4]; 
		HBM_channelA2[dest_offset + t].data[5] = data[2][5]; 
		HBM_channelA2[dest_offset + t].data[6] = data[2][6]; 
		HBM_channelA2[dest_offset + t].data[7] = data[2][7]; 
		HBM_channelA2[dest_offset + t].data[8] = data[2][8]; 
		HBM_channelA2[dest_offset + t].data[9] = data[2][9]; 
		HBM_channelA2[dest_offset + t].data[10] = data[2][10]; 
		HBM_channelA2[dest_offset + t].data[11] = data[2][11]; 
		HBM_channelA2[dest_offset + t].data[12] = data[2][12]; 
		HBM_channelA2[dest_offset + t].data[13] = data[2][13]; 
		HBM_channelA2[dest_offset + t].data[14] = data[2][14]; 
		HBM_channelA2[dest_offset + t].data[15] = data[2][15]; 
		HBM_channelA3[dest_offset + t].data[0] = data[3][0]; 
		HBM_channelA3[dest_offset + t].data[1] = data[3][1]; 
		HBM_channelA3[dest_offset + t].data[2] = data[3][2]; 
		HBM_channelA3[dest_offset + t].data[3] = data[3][3]; 
		HBM_channelA3[dest_offset + t].data[4] = data[3][4]; 
		HBM_channelA3[dest_offset + t].data[5] = data[3][5]; 
		HBM_channelA3[dest_offset + t].data[6] = data[3][6]; 
		HBM_channelA3[dest_offset + t].data[7] = data[3][7]; 
		HBM_channelA3[dest_offset + t].data[8] = data[3][8]; 
		HBM_channelA3[dest_offset + t].data[9] = data[3][9]; 
		HBM_channelA3[dest_offset + t].data[10] = data[3][10]; 
		HBM_channelA3[dest_offset + t].data[11] = data[3][11]; 
		HBM_channelA3[dest_offset + t].data[12] = data[3][12]; 
		HBM_channelA3[dest_offset + t].data[13] = data[3][13]; 
		HBM_channelA3[dest_offset + t].data[14] = data[3][14]; 
		HBM_channelA3[dest_offset + t].data[15] = data[3][15]; 
		HBM_channelA4[dest_offset + t].data[0] = data[4][0]; 
		HBM_channelA4[dest_offset + t].data[1] = data[4][1]; 
		HBM_channelA4[dest_offset + t].data[2] = data[4][2]; 
		HBM_channelA4[dest_offset + t].data[3] = data[4][3]; 
		HBM_channelA4[dest_offset + t].data[4] = data[4][4]; 
		HBM_channelA4[dest_offset + t].data[5] = data[4][5]; 
		HBM_channelA4[dest_offset + t].data[6] = data[4][6]; 
		HBM_channelA4[dest_offset + t].data[7] = data[4][7]; 
		HBM_channelA4[dest_offset + t].data[8] = data[4][8]; 
		HBM_channelA4[dest_offset + t].data[9] = data[4][9]; 
		HBM_channelA4[dest_offset + t].data[10] = data[4][10]; 
		HBM_channelA4[dest_offset + t].data[11] = data[4][11]; 
		HBM_channelA4[dest_offset + t].data[12] = data[4][12]; 
		HBM_channelA4[dest_offset + t].data[13] = data[4][13]; 
		HBM_channelA4[dest_offset + t].data[14] = data[4][14]; 
		HBM_channelA4[dest_offset + t].data[15] = data[4][15]; 
		
		#ifdef _DEBUGMODE_CHECKS3
		num_edges_inserted += 1; 
		#endif 
		update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___NUMBER_OF_EDGE_INSERTIONS___, 1); 
	}
	// #endif 
	
	// save-back any cache-miss for edge updates 
	unsigned int miss_sz = MISSBUFFER_edgeupdates_index[0][0];
	if(MISSBUFFER_edgeupdates_index[0][0] > 128){
		#ifdef _DEBUGMODE_KERNELPRINTS4
		cout<<"-------------------------------- apply-all-updates: saving "<<MISSBUFFER_edgeupdates_index[0][0]<<" ("<<MISSBUFFER_edgeupdates_index[0][0] * EDGE_PACK_SIZE<<") missed edge updates for later processing ("<<nn<<")... --------------------------------"<<endl; nn += 1;
		#endif 
	
		SAVE_MISSEDEDGEUPDATES_LOOP: for(unsigned int t=0; t<MISSBUFFER_edgeupdates_index[0][0]; t++){
			for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
			#pragma HLS UNROLL
				for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
				#pragma HLS UNROLL
					edge3_vecs[inst].data[v] = MISSBUFFER_edgeupdates[inst][v][t];	
					#ifdef _DEBUGMODE_KERNELPRINTS//4
					if(miss_added==1 && v==0){ cout<<"$$$ save misses::["<<inst<<"]["<<t<<"]["<<v<<"]: [edge_update-update: srcvid: "<<edge3_vecs[inst].data[v].srcvid<<", dstvid: "<<edge3_vecs[inst].data[v].dstvid<<"]---"<<endl; }								
					#endif 
				}
			}
			dinsertmany_edgesdram(globalparams[GLOBALPARAMSCODE__BASEOFFSET__EDGEUPDATES] + t, edge3_vecs,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); // GLOBALPARAMSCODE__BASEOFFSET__PARTIALLYPROCESSEDEDGEUPDATES
		}	
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
			#pragma HLS UNROLL
				MISSBUFFER_edgeupdates_index[inst][v] = 0;
			}
		}
	}
	
	// update edge & edgeupdate map
	for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){
		#pragma HLS UNROLL
		edge_maps_buffer[inst][p_u].size += chunksz;
		edgeu_maps_buffer[inst][p_u].size -= chunksz;
		#ifdef _DEBUGMODE_KERNELPRINTS//4
		cout<<"apply_all_updates : [save] edge_maps_buffer["<<inst<<"]["<<p_u<<"].offset: "<<edge_maps_buffer[inst][p_u].offset<<", edge_maps_buffer["<<inst<<"]["<<p_u<<"].size: "<<edge_maps_buffer[inst][p_u].size<<", maxsz: "<<(edge_maps_buffer[inst][p_u + 1].offset - edge_maps_buffer[inst][p_u].offset)<<endl;
		cout<<"apply_all_updates : [save] edgeu_maps_buffer["<<inst<<"]["<<p_u<<"].offset: "<<edgeu_maps_buffer[inst][p_u].offset<<", edgeu_maps_buffer["<<inst<<"]["<<p_u<<"].size: "<<edgeu_maps_buffer[inst][p_u].size<<", maxsz: "<<(edgeu_maps_buffer[inst][p_u + 1].offset - edgeu_maps_buffer[inst][p_u].offset)<<endl;
		#endif	
	}
	
	// print summary
	#ifdef _DEBUGMODE_APPLYEDGEUPDATES_PRINTS//4
	cout<<"apply-edges: summary: total number of edges inserted : "<<num_edges_inserted<<" (of "<<max_limit * EDGE_PACK_SIZE<<")"<<endl;
	#endif 
	// exit(EXIT_SUCCESS);	

	// print number of hits and misses
	#ifdef _DEBUGMODE_APPLYEDGEUPDATES_PRINTS4
	if(chunksz==0){ chunksz = 1; }
	if(action.fpga < num_prints && max_limit > 0){ cout<< TIMINGRESULTSCOLOR <<"--- apply-edge-updates: [p_v: "<<p_v<<", p_u: "<<p_u<<"][max_num_edges: "<<max_num_edges<<"]: # updated: "<<num_edges_updated<<", # inserted: "<<num_edges_inserted<<", # deleted: 0, # misses: "<<(total_num_misses_ - num_misses_) <<" (of "<<chunksz * EDGE_PACK_SIZE<<")("<<((total_num_misses_ - num_misses_) * 100) / (chunksz * EDGE_PACK_SIZE)<<"%)"<<", total # misses: "<<total_num_misses_ << RESET << endl; }				
	#endif 
	
	// if(miss_added == 1){ exit(EXIT_SUCCESS); } 
	miss_added = 0; // += 1;
	// if(p_u==1){ exit(EXIT_SUCCESS);	}
	// exit(EXIT_SUCCESS);
}	





	
		}
		#endif 
		
		// collect and save frontiers
		#ifdef ___ENABLE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___XXX // FIXME.
		if(___ENABLE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___BOOL___ == 1){
keyvalue_t frontier_data[NUM_VALID_PEs][EDGE_PACK_SIZE];
#pragma HLS ARRAY_PARTITION variable = frontier_data complete dim=0

COLLECT_AND_SAVE_FRONTIERS_LOOP1: for(unsigned int local_subpartitionID=0; local_subpartitionID<num_subpartition_per_partition; local_subpartitionID+=1){
	unsigned int upartitionID = (p_v * num_subpartition_per_partition) + local_subpartitionID;
	unsigned int foffset = globalparams[GLOBALPARAMSCODE__BASEOFFSET__NFRONTIERS] + (p_v * MAX_APPLYPARTITION_VECSIZE);
	unsigned int offset = local_subpartitionID * vdata_subpartition_vecsize;

	// cout<<">>> collect-and-save-frontiers ++++++++++++++++++ vdata_subpartition_vecsize: "<<vdata_subpartition_vecsize<<endl;
	COLLECT_FRONTIERS_LOOP: for(unsigned int t=0; t<vdata_subpartition_vecsize; t++){
	// cout<<">>> collect-and-save-frontiers ----------------- local_subpartitionID: "<<local_subpartitionID<<"(of "<<num_subpartition_per_partition<<"), t: "<<t<<"(of "<<vdata_subpartition_vecsize<<")"<<endl;
	#pragma HLS PIPELINE II=1
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 
		#pragma HLS UNROLL
			for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){
			#pragma HLS UNROLL
				// vprop_t vprop; //
				vprop_t vprop =  retrieve_vdatabuffer(v, t, URAM_vprop[inst]); // FIXME.
				unsigned int vid = 0; //FIXME.
				
				#ifdef _DEBUGMODE_KERNELPRINTS_TRACE3
				std::cout<<"COLLECT FRONTIER INFORMATION SEEN @: inst: ["<<inst<<"]: t: "<<t<<", v: "<<v<<", p__u__: "<<(vid / MAX_UPARTITION_SIZE)<<", vid: "<<vid<<std::endl;
				#endif
				#ifdef _DEBUGMODE_CHECKS3
				checkoutofbounds("acts_kernel::ERROR 21021::", t, MAX_VDATA_SUBPARTITION_VECSIZE, NAp, NAp, NAp);
				#endif 
				frontier_t actvv; actvv.key = vid; actvv.value = vprop.prop; 
				insert_nfrontierbuffer(v, t, actvv, nfrontier_buffer[inst]); 
				// nfrontier_buffer[inst][v][t] = actvv;
			}
		}
		update_bramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___, 1);
	}
	
	SAVE_FRONTIERS_LOOP: for(unsigned int t=0; t<vdata_subpartition_vecsize; t++){
	// cout<<">>> collect-and-save-frontiers b ----------------- local_subpartitionID: "<<local_subpartitionID<<"(of "<<num_subpartition_per_partition<<"), t: "<<t<<"(of "<<vdata_subpartition_vecsize<<")"<<endl;
	#pragma HLS PIPELINE II=1
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 
		#pragma HLS UNROLL
			#ifdef _DEBUGMODE_CHECKS3
			checkoutofbounds("acts_kernel::ERROR 61021::", t, MAX_VDATA_SUBPARTITION_VECSIZE, NAp, NAp, NAp);
			#endif 
			retrievevec_nfrontierbuffer(t, frontier_data[inst], nfrontier_buffer[inst]);  // FIXME.
		}
		for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 
		#pragma HLS UNROLL
			offsets2[inst] = foffset + offset + t;
		}
		#ifdef _DEBUGMODE_CHECKS3
		checkoutofbounds("acts_kernel::ERROR 71021::", offsets2[0], HBM_CHANNEL_SIZE, NAp, NAp, NAp);
		#endif 
		dinsertmany_nfrontierdram(offsets2, t, frontier_data, ens,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); // FIXME.
	}
	
	for(unsigned int n=0; n<NUM_VALID_PEs; n++){
	#pragma HLS UNROLL
		#ifdef _DEBUGMODE_CHECKS3
		checkoutofbounds("acts_kernel::ERROR 76021::", upartitionID, MAX_NUM_UPARTITIONS, NAp, NAp, NAp);
		#endif 
		nfrontier_dram___size[n][upartitionID] = vdata_subpartition_vecsize; 
	}
}
					
		}
		#endif 
		
		// save destination properties
		#ifdef ___ENABLE___SAVE_DEST_PROPERTIES___
		if(___ENABLE___SAVE_DEST_PROPERTIES___BOOL___ == 1){	
vprop_dest_t vprop[NUM_VALID_PEs][EDGE_PACK_SIZE]; 
#pragma HLS ARRAY_PARTITION variable = vprop complete dim=0

unsigned int sz = MAXVALID_APPLYPARTITION_VECSIZE; if(action.command == GRAPH_UPDATE_ONLY || action.command == GRAPH_ANALYTICS_EXCLUDEVERTICES){ sz = 0; }

SAVE_DEST_PROPERTIES_LOOP2: for(unsigned int t=0; t<sz; t++){
#pragma HLS PIPELINE II=1
	SAVE_DEST_PROPERTIES_LOOP2B: for(unsigned int inst=0; inst<NUM_VALID_PEs; inst++){ 
	#pragma HLS UNROLL
		for(unsigned int v=0; v<EDGE_PACK_SIZE; v++){ 
		#pragma HLS UNROLL
			#ifdef _DEBUGMODE_CHECKS3
			checkoutofbounds("acts_kernel::ERROR 105::", t, MAX_UPARTITION_VECSIZE, NAp, NAp, NAp);
			#endif 
			vprop[inst][v] = URAM_vprop[inst][v][t];
		}
	}
	dinsertmany_vdatadram(voffset, t, vprop,  HBM_SRCA0, HBM_SRCB0, HBM_SRCA1, HBM_SRCB1, HBM_SRCA2, HBM_SRCB2, HBM_SRCA3, HBM_SRCB3, HBM_SRCA4, HBM_SRCB4);
	
	update_dramnumclockcycles(_NUMCLOCKCYCLES_, ___CODE___SAVE_DEST_PROPERTIES___, 1);
}	
		}
		#endif 
	} 
	}
	#endif
	
	// gather frontiers 
	#ifdef ___ENABLE___GATHER_FRONTIERINFOS___
	#if NUM_PEs>1
	if(action.module == GATHER_FRONTIERS_MODULE || action.module == ALL_MODULES){
		GATHER_FRONTIERS_MODULE_LOOP: for(unsigned int upartitionID=action.start_gv; upartitionID<action.start_gv + action.size_gv; upartitionID++){	
			if(upartitionID >= globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS]){ continue; } 
			if(action.command == GRAPH_UPDATE_ONLY || action.command == GRAPH_ANALYTICS_EXCLUDEVERTICES){ continue; }
			
			#ifndef ___RUNNING_FPGA_SYNTHESIS___
			if(action.fpga < num_prints){ cout<<"### gathering frontiers for upartitionID "<<upartitionID<<": [PEs "; for(unsigned int n=0; n<NUM_PEs; n++){ cout<<n<<", "; } cout<<"] [target FPGAs "; for(unsigned int n=0; n<action.numfpgas; n++){ cout<<n<<", "; } cout<<"] [max "<<globalparams[GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS] / action.numfpgas<<"]"<<endl; }
			#endif	
			
			for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			#pragma HLS UNROLL
				nfrontier_dram___size[n][upartitionID] = vdata_subpartition_vecsize; 
			}
			
	
			gather_frontiers(0, upartitionID, cfrontier_dram___size, nfrontier_dram___size[0], upartition_vertices, HBM_channelA0, HBM_channelB0, HBM_centerA, HBM_centerB, vdata_subpartition_vecsize, globalparams, _NUMCLOCKCYCLES_);
	
			gather_frontiers(1, upartitionID, cfrontier_dram___size, nfrontier_dram___size[0], upartition_vertices, HBM_channelA0, HBM_channelB0, HBM_centerA, HBM_centerB, vdata_subpartition_vecsize, globalparams, _NUMCLOCKCYCLES_);
	
			gather_frontiers(2, upartitionID, cfrontier_dram___size, nfrontier_dram___size[0], upartition_vertices, HBM_channelA0, HBM_channelB0, HBM_centerA, HBM_centerB, vdata_subpartition_vecsize, globalparams, _NUMCLOCKCYCLES_);
	
			gather_frontiers(3, upartitionID, cfrontier_dram___size, nfrontier_dram___size[0], upartition_vertices, HBM_channelA0, HBM_channelB0, HBM_centerA, HBM_centerB, vdata_subpartition_vecsize, globalparams, _NUMCLOCKCYCLES_);
	
			gather_frontiers(4, upartitionID, cfrontier_dram___size, nfrontier_dram___size[0], upartition_vertices, HBM_channelA0, HBM_channelB0, HBM_centerA, HBM_centerB, vdata_subpartition_vecsize, globalparams, _NUMCLOCKCYCLES_);
		}
	}
	#endif 
	#endif 
	
	#ifdef ___ENABLE___REPORT_STATISTICS___
#ifdef _DEBUGMODE_CHECKS3 

#ifdef _DEBUGMODE_KERNELPRINTS4
if(action.fpga < num_prints){ 
	cout<<"[acts: READ_FRONTIERS, ";
	cout<<"PROCESSEDGES, "; 
	cout<<"READ_DESTS, "; 
	cout<<"APPLYUPDATES, "; 
	cout<<"COLLECT_FRONTIERS, "; 
	cout<<"SAVE_DEST, "; 
	cout<<"GATHER_FRONTIERS] "; 
	cout<<endl;
}																										
#endif 

#ifdef _DEBUGMODE_KERNELPRINTS4
if(action.fpga < num_prints){ 
cout<<">>> [Per FPGA][";
cout<<_NUMCLOCKCYCLES_[0][___CODE___READ_FRONTIER_PROPERTIES___] * EDGE_PACK_SIZE<<", ";
cout<<_NUMCLOCKCYCLES_[0][___CODE___ECPROCESSEDGES___] * EDGE_PACK_SIZE<<", ";
cout<<_NUMCLOCKCYCLES_[0][___CODE___READ_DEST_PROPERTIES___] * EDGE_PACK_SIZE * NUM_PEs<<", ";
cout<<_NUMCLOCKCYCLES_[0][___CODE___APPLYUPDATES___] * EDGE_PACK_SIZE<<", ";
cout<<_NUMCLOCKCYCLES_[0][___CODE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___] * EDGE_PACK_SIZE * NUM_PEs<<", ";
cout<<_NUMCLOCKCYCLES_[0][___CODE___SAVE_DEST_PROPERTIES___] * EDGE_PACK_SIZE * NUM_PEs<<", ";
cout<<_NUMCLOCKCYCLES_[0][___CODE___GATHER_FRONTIERINFOS___] * EDGE_PACK_SIZE<<"";
cout<<"]";
cout<<endl;
}
#endif 

#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
#ifdef _DEBUGMODE_KERNELPRINTS4
if(action.fpga < num_prints){ 
	cout<<"[acts: NUMBER_OF_EDGE_INSERTIONS, "; 
	cout<<"NUMBER_OF_EDGE_UPDATINGS, "; 
	cout<<"NUMBER_OF_EDGE_DELETIONS]";
	cout<<endl;
}	
#endif 

#ifdef _DEBUGMODE_KERNELPRINTS4
if(action.fpga < num_prints){ 
cout<<">>> [Per FPGA][";
cout<<_NUMCLOCKCYCLES_[0][___CODE___NUMBER_OF_EDGE_INSERTIONS___] * EDGE_PACK_SIZE<<", ";
cout<<_NUMCLOCKCYCLES_[0][___CODE___NUMBER_OF_EDGE_UPDATINGS___] * EDGE_PACK_SIZE<<", ";
cout<<_NUMCLOCKCYCLES_[0][___CODE___NUMBER_OF_EDGE_DELETIONS___] * EDGE_PACK_SIZE<<"";
cout<<"]";
cout<<endl;
}
#endif 
#endif 

#ifdef _DEBUGMODE_KERNELPRINTS4
if(action.fpga==0){
	report_statistics[___CODE___RESETBUFFERSATSTART___] += _NUMCLOCKCYCLES_[0][___CODE___RESETBUFFERSATSTART___];
	report_statistics[___CODE___READ_FRONTIER_PROPERTIES___] += _NUMCLOCKCYCLES_[0][___CODE___READ_FRONTIER_PROPERTIES___];
	report_statistics[___CODE___ECPROCESSEDGES___] += _NUMCLOCKCYCLES_[0][___CODE___ECPROCESSEDGES___];
	report_statistics[___CODE___READ_DEST_PROPERTIES___] += _NUMCLOCKCYCLES_[0][___CODE___READ_DEST_PROPERTIES___];
	report_statistics[___CODE___APPLYUPDATES___] += _NUMCLOCKCYCLES_[0][___CODE___APPLYUPDATES___];
	report_statistics[___CODE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___] += _NUMCLOCKCYCLES_[0][___CODE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___];
	report_statistics[___CODE___SAVE_DEST_PROPERTIES___] += _NUMCLOCKCYCLES_[0][___CODE___SAVE_DEST_PROPERTIES___];
	report_statistics[___CODE___GATHER_FRONTIERINFOS___] += _NUMCLOCKCYCLES_[0][___CODE___GATHER_FRONTIERINFOS___];
	
	report_statistics[___CODE___NUMBER_OF_EDGE_INSERTIONS___] += _NUMCLOCKCYCLES_[0][___CODE___NUMBER_OF_EDGE_INSERTIONS___];
	report_statistics[___CODE___NUMBER_OF_EDGE_UPDATINGS___] += _NUMCLOCKCYCLES_[0][___CODE___NUMBER_OF_EDGE_UPDATINGS___];
	report_statistics[___CODE___NUMBER_OF_EDGE_DELETIONS___] += _NUMCLOCKCYCLES_[0][___CODE___NUMBER_OF_EDGE_DELETIONS___];
}
// cout<<"~~~~~~~~~~~~~~ report_statistics[___CODE___READ_FRONTIER_PROPERTIES___]: "<<report_statistics[___CODE___READ_FRONTIER_PROPERTIES___]<<", _NUMCLOCKCYCLES_[0][___CODE___READ_FRONTIER_PROPERTIES___]: "<<_NUMCLOCKCYCLES_[0][___CODE___READ_FRONTIER_PROPERTIES___]<<endl;				
#endif 

#ifdef _DEBUGMODE_KERNELPRINTS//4
if(action.status == 1){
	float total_cycles_iter = 
	+ _NUMCLOCKCYCLES_[0][___CODE___RESETBUFFERSATSTART___]
	+ _NUMCLOCKCYCLES_[0][___CODE___PROCESSEDGES___]
	+ _NUMCLOCKCYCLES_[0][___CODE___READ_FRONTIER_PROPERTIES___]
	+ _NUMCLOCKCYCLES_[0][___CODE___VCPROCESSEDGES___]
	+ _NUMCLOCKCYCLES_[0][___CODE___ECPROCESSEDGES___]
	+ _NUMCLOCKCYCLES_[0][___CODE___SAVEVCUPDATES___]
	+ _NUMCLOCKCYCLES_[0][___CODE___COLLECTACTIVEDSTVIDS___]
	+ _NUMCLOCKCYCLES_[0][___CODE___APPLYUPDATESMODULE___]
	+ _NUMCLOCKCYCLES_[0][___CODE___READ_DEST_PROPERTIES___]
	+ _NUMCLOCKCYCLES_[0][___CODE___APPLYUPDATES___]
	+ _NUMCLOCKCYCLES_[0][___CODE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___]
	+ _NUMCLOCKCYCLES_[0][___CODE___SAVE_DEST_PROPERTIES___]
	+ _NUMCLOCKCYCLES_[0][___CODE___GATHER_FRONTIERINFOS___];
	float time_lapse = (total_cycles_iter * _NUMNANOSECONDS_PER_CLOCKCYCLE_) / 1000000;
	cout<<"acts_kernel SUMMARY:: number of clock cycles seen: "<<total_cycles_iter<<" ("<<time_lapse<<" milliseconds)"<<endl;
	float time_lapse1 = (total_cycles_iter / 1000000) * _NUMNANOSECONDS_PER_CLOCKCYCLE_;
	float time_lapse2 = (float)((total_cycles_iter * EDGE_PACK_SIZE) / 1000) / (float)ACTS_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC;
	float million_edges_per_sec = ((globalparams[GLOBALPARAMSCODE__PARAM__NUM_EDGES] / 1000000) / time_lapse) * 1000;
	float billion_edges_per_sec = million_edges_per_sec / 1000;
	cout<<"acts_kernel SUMMARY:: time elapsed for iteration "<<GraphIter<<": "<<time_lapse<<" milliseconds)"<<endl;
}
for(unsigned int t=0; t<16; t++){ total_cycles += _NUMCLOCKCYCLES_[0][t]; }
for(unsigned int t=0; t<16; t++){ _NUMCLOCKCYCLES_[0][t] = 0; }
#endif 
#endif	
	#endif 
	
	// save state: vertex updates 
	#ifndef ___ENABLE___DYNAMICGRAPHANALYTICS___
	if(action.module == PROCESS_EDGES_MODULE){ 
		SAVE_UPDATEPTRS_lOOP1B: for(unsigned int t=0; t<globalparams[GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS]; t++){	
		#pragma HLS PIPELINE II=1
			map_t map_data; map_data.offset = updatesptrs[t].offset; map_data.size = updatesptrs[t].size; 
			save_vupdate_map(globalparams[GLOBALPARAMSCODE__BASEOFFSET__UPDATESPTRS], t, map_data,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); 			
			#ifdef _DEBUGMODE_KERNELPRINTS//4
			if(action.fpga < num_prints){ cout<<"finish: updatesptrs["<<t<<"].offset: "<<updatesptrs[t].offset<<", updatesptrs["<<t<<"].size: "<<updatesptrs[t].size<<endl; }
			#endif 
		}
	}
	#endif 
	
	// save state: edges 
	#ifdef ___ENABLE___DYNAMICGRAPHANALYTICS___
	map_t edge_maps_s[NUM_VALID_PEs];
	map_t edgeu_maps_s[NUM_VALID_PEs];
	#pragma HLS ARRAY_PARTITION variable=edge_maps_s complete
	#pragma HLS ARRAY_PARTITION variable=edgeu_maps_s complete
	for(unsigned int p_u=action.start_pu; p_u<action.start_pu + action.size_pu + 4; p_u+=action.skip_pu){
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			#pragma HLS UNROLL
			edge_maps_s[n].offset = edge_maps_buffer[n][p_u].offset; edge_maps_s[n].size = edge_maps_buffer[n][p_u].size;
			#ifdef _DEBUGMODE_KERNELPRINTS4
			cout<<">>> acts : [save][edge_maps_s,edgeu_maps_s][p_u: "<<p_u<<"]: offset: "<<edge_maps_buffer[n][p_u].offset<<", proc: "<<edge_maps_buffer[n][p_u].size<<", rem: "<<edgeu_maps_buffer[n][p_u].size<<", maxsz: "<<(edge_maps_buffer[n][p_u + 1].offset - edge_maps_buffer[n][p_u].offset)<<endl;
			#endif 
		}
		save_edgemaps((p_u * MAX_NUM_LLPSETS) + 0, globalparams[GLOBALPARAMSCODE__BASEOFFSET__ACTPACKVPTRS], edge_maps_s,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); 
	}
	for(unsigned int p_u=action.start_pu; p_u<action.start_pu + action.size_pu + 4; p_u+=action.skip_pu){
		for(unsigned int n=0; n<NUM_VALID_PEs; n++){
			#pragma HLS UNROLL
			edgeu_maps_s[n].offset = edgeu_maps_buffer[n][p_u].offset; edgeu_maps_s[n].size = edgeu_maps_buffer[n][p_u].size;
		}
		save_edgemaps((p_u * MAX_NUM_LLPSETS) + 0, globalparams[GLOBALPARAMSCODE__BASEOFFSET__EDGEUPDATESPTRS], edgeu_maps_s,  HBM_EDGESA0, HBM_EDGESB0, HBM_EDGESA1, HBM_EDGESB1, HBM_EDGESA2, HBM_EDGESB2, HBM_EDGESA3, HBM_EDGESB3, HBM_EDGESA4, HBM_EDGESB4); 
	}
	#endif 
	
	// report result
	#ifdef _DEBUGMODE_CHECKS3
	if(action.status == 1){
float time_lapse = (total_cycles * _NUMNANOSECONDS_PER_CLOCKCYCLE_) / 1000000;
float million_edges_per_sec = (((globalparams[GLOBALPARAMSCODE__PARAM__NUM_EDGES] * globalparams[GLOBALPARAMSCODE__PARAM__NUM_RUNS]) / 1000000) / time_lapse) * 1000;
float billion_edges_per_sec = million_edges_per_sec / 1000;
#ifdef _DEBUGMODE_KERNELPRINTS//4
cout<<"acts_kernel SUMMARY:: total number of active vertices processed / HBM CHANNEL: "<<total_num_actvvs<<endl;
cout<<"acts_kernel SUMMARY:: total number of edges processed per run: "<<globalparams[GLOBALPARAMSCODE__PARAM__NUM_EDGES]<<endl;
cout<<"acts_kernel SUMMARY:: total number of runs: "<<globalparams[GLOBALPARAMSCODE__PARAM__NUM_RUNS]<<endl;
cout<<"acts_kernel SUMMARY:: number of DRAM clock cycles seen: "<<total_cycles<<" ("<<(total_cycles * _NUMNANOSECONDS_PER_CLOCKCYCLE_) / 1000000<<" milliseconds)"<<endl;
cout<< TIMINGRESULTSCOLOR << ">>> acts_kernel SUMMARY:: TIME ELAPSE: "<<time_lapse<<" ms. THROUGHPUT: "<<million_edges_per_sec<<" MTEPS; THROUGHPUT: "<<billion_edges_per_sec<<" BTEPS"<< RESET << endl;
#endif 
	
	}
	#endif	
	
	#ifdef ___RUNNING_FPGA_SYNTHESIS___
	return;
	#else 
	return 0;	
	#endif 
}
}		
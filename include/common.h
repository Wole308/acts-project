#ifndef COMMON_H
#define COMMON_H
#include <string.h> 
#include <cmath> 
#include <ap_int.h>
// #include "ap_fixed.h"	
#include <vector> 
#include<hls_vector.h> 
#include<hls_stream.h> 
#include <iostream> 
 

// only-for-synthesis-tests
#define POW_VALID_VDATA 1 // 1,0 // FIXME^

 
#define ___RUNNING_FPGA_SYNTHESIS___
 
// #define ___ENABLE___DYNAMICGRAPHANALYTICS___
#ifndef ___RUNNING_FPGA_SYNTHESIS___
	#define ___CREATE_ACTPACK_FROM_VECTOR___
#endif 
// #define PROOF_OF_CONCEPT_RUN ///////////////.............................................................
#define ENABLE_SHARING_SRCs
#define NUM_VALID_HBM_CHANNELS_PER_SLR (5 / 3)

#define MAX_NUM_FPGAS 8
#define RUN_IN_ASYNC_MODE 1

#define GRAPH_UPDATE_ONLY 1
#define GRAPH_UPDATE_AND_ANALYTICS 2
#define GRAPH_ANALYTICS_EXCLUDEVERTICES 3

////////////////////////////////

	

#define ALL_MODULES 222 
// #define PREPARE_EDGEUPDATES_MODULE 2220
#define PREPROCESSING_MODULE 2210
#define COMMIT_EDGEUPDATES_MODULE 2221
#define PROCESS_EDGES_MODULE 2220
#define APPLY_UPDATES_MODULE 2222
#define GATHER_FRONTIERS_MODULE 2223
#define APPLY_UPDATES_MODULE___AND___GATHER_DSTPROPERTIES_MODULE 2224

#define BAKE_PAGERANK_ALGORITHM_FUNCS
// #define BAKE_SPMV_ALGORITHM_FUNCS
// #define BAKE_HITS_ALGORITHM_FUNCS
// #define BAKE_SSSP_ALGORITHM_FUNCS

#define ___FORCE_SUCCESS___
#define ___FORCE_SUCCESS_SINGLE_CHANNEL___
// #define ADJUSTMENT_TO_MEET_VHLS_TIMING
#define ___PARTIAL___HBM___ACCESSES___

#define DRAM_ACCESS_LATENCY 8
#define _NUMNANOSECONDS_PER_CLOCKCYCLE_ (6956 / 1000) // 6.956 // {23M uint32 seen processed in 10ms}{10M uint32 seen processed in 4ms}
#define ACTS_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC ((23 * 1000) / 10) // ((23 / 10) * 1000) // {23M uint32 seen processed in 10ms}{10M uint32 seen processed in 4ms}

#define ___USE_AXI_CHANNEL___

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define HW // SWEMU, HW, *SW
#if (defined(SWEMU) || defined(HW)) 
#define FPGA_IMPL
#endif 

// #define RANKING_ALGORITHM_TYPE
// #define TRAVERSAL_ALGORITHM_TYPE
// #define ALL_ALGORITHM_TYPES

#if defined(BFS_ALGORITHM) || defined(SSSP_ALGORITHM)
#define CONFIG_ALGORITHMTYPE_RANDOMACTIVEVERTICES // { utility.cpp }
#endif

#define _DEBUGMODE_HEADER //
#if defined (FPGA_IMPL) // && defined (HW) // REMOVEME. 
#else
#define _DEBUGMODE_STATS // 
// #define _DEBUGMODE_CHECKS
#define _DEBUGMODE_CHECKS2 // 
#define _DEBUGMODE_CHECKS3 // 
// #define _DEBUGMODE_PRINTS
// #define _DEBUGMODE_KERNELPRINTS
// #define _DEBUGMODE_KERNELPRINTS2 //
// #define _DEBUGMODE_KERNELPRINTS3 //
#define _DEBUGMODE_KERNELPRINTS4 //
// #define _DEBUGMODE_RUNKERNELPRINTS // 
// #define _DEBUGMODE_PROCACTVVSPRINTS //
#endif
// #define _DEBUGMODE_HOSTCHECKS
#define _DEBUGMODE_HOSTCHECKS2 //
#define _DEBUGMODE_HOSTCHECKS3 //
// #define _DEBUGMODE_HOSTPRINTS 
// #define _DEBUGMODE_HOSTPRINTS2 //
// #define _DEBUGMODE_HOSTPRINTS3 //
#define _DEBUGMODE_HOSTPRINTS4 //
// #define _DEBUGMODE_TIMERS
#define _DEBUGMODE_TIMERS2
#define _DEBUGMODE_TIMERS3
#define GOCLKERNEL_DEBUGMODE_HOSTPRINTS3
#define HOST_PRINT_RESULTS

#ifndef HW
// #define _DEBUGMODE_KERNELPRINTS_TRACE3 //
#endif 

////////////////

#define PAGERANK 222
#define CF 333
#define HITS 444
#define SPMV 555
#define BFS 666
#define SSSP 777
#define CC 888
#define ALGORITHMCLASS_ALLVERTEXISACTIVE 2222 
#define ALGORITHMCLASS_NOTALLVERTEXISACTIVE 3333

////////////////

#define ___CODE___RESETBUFFERSATSTART___ 0
// #define ___CODE___NUMBER_OF_EDGE_INSERTIONS___ 1
#define ___CODE___PROCESSEDGES___ 1
#define ___CODE___READ_FRONTIER_PROPERTIES___ 2
#define ___CODE___VCPROCESSEDGES___ 3
#define ___CODE___ECPROCESSEDGES___ 4
#define ___CODE___SAVEVCUPDATES___ 5
#define ___CODE___COLLECTACTIVEDSTVIDS___ 6
#define ___CODE___APPLYUPDATESMODULE___ 7
	#define ___CODE___READ_DEST_PROPERTIES___ 8
	#define ___CODE___APPLYUPDATES___ 9
	#define ___CODE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES___ 10
	#define ___CODE___SAVE_DEST_PROPERTIES___ 11
#define ___CODE___GATHER_FRONTIERINFOS___ 12
#define ___CODE___IMPORT_FRONTIERINFOS___ 13
#define ___CODE___EXPORT_FRONTIERINFOS___ 14
#define ___CODE___NUMBER_OF_EDGE_INSERTIONS___ 15
#define ___CODE___NUMBER_OF_EDGE_UPDATINGS___ 16
#define ___CODE___NUMBER_OF_EDGE_DELETIONS___ 17
#define ___CODE___IMPORT_BATCH_SIZE___ 18
#define ___CODE___EXPORT_BATCH_SIZE___ 19

//////////////// 

#define MAX_NUM_PEs 12
#define MAX_GLOBAL_NUM_PEs (MAX_NUM_FPGAS * MAX_NUM_PEs)
#define NUM_PEs 5		
#define NUM_VALID_PEs 5
#define NUM_VALID_HBM_CHANNELS 5
#define EDGE_PACK_SIZE_POW 4 // 1 4*
#define EDGE_PACK_SIZE (1 << EDGE_PACK_SIZE_POW) // 2, 16*
#define HBM_CHANNEL_PACK_SIZE (EDGE_PACK_SIZE * 2) // 32*
#define HBM_AXI_PACK_SIZE (HBM_CHANNEL_PACK_SIZE / 2) // 16* // NEW**
#define HBM_AXI_PACK_BITSIZE (HBM_AXI_PACK_SIZE * 32) // 512* // NEW**
#define HBM_CHANNEL_BYTESIZE (1 << 28)
#define HBM_CHANNEL_INTSIZE (HBM_CHANNEL_BYTESIZE / 4)
#define HBM_CHANNEL_SIZE ((HBM_CHANNEL_BYTESIZE / 4) / EDGE_PACK_SIZE) // {4194304 EDGE_PACK_SIZEs, 67108864 uints, 256MB}
#define HBM_CENTER_SIZE ((HBM_CHANNEL_BYTESIZE / 4) / EDGE_PACK_SIZE) // {4194304 EDGE_PACK_SIZEs, 67108864 uints, 256MB}
#define FOLD_SIZE 1
// #define MAX_NUM_UPARTITIONS 512 // 1024 // 512	// NEWCHANGE.
#define MAX_NUM_UPARTITIONS 1024 	
// #define MAX_NUM_UPARTITIONS 512	
#define MAX_NUM_APPLYPARTITIONS 48 
#define MAX_NUM_LLPSETS 1 // 32 
#define NUM_LLP_PER_LLPSET EDGE_PACK_SIZE
#define MAX_NUM_LLP_PER_UPARTITION (MAX_NUM_LLPSETS * NUM_LLP_PER_LLPSET)
#define NAp 666
#define MAXNUMGRAPHITERATIONS 32 // 16
#define MAX_NUM_PARTITIONS 16 
#define INVALIDDATA 0xFFFFFFFF 
#define INVALIDMASK 0 
#define NUM_KERNEL_SUBLAUNCHES_PER_LAUNCH 1 // (NUM_PEs / NUM_VALID_PEs)

#define MAXNUMBITS2_ACTPACK_SRCVID 14 
#define MAXNUMBITS2_ACTPACK_DESTVID 14
#define MAXNUMBITS2_ACTPACK_EDGEID 4
#define MAXLOCALVALUE2_ACTPACK_SRCVID 0x3FFF // (2^14-1=16383)
#define MAXLOCALVALUE2_ACTPACK_DESTVID 0x3FFF
#define MAXLOCALVALUE2_ACTPACK_EDGEID 0xF 
#define INVALIDDATA_SRCVID MAXLOCALVALUE2_ACTPACK_SRCVID
#define INVALIDDATA_DESTVID MAXLOCALVALUE2_ACTPACK_DESTVID

#define MAX_UPARTITION_VECSIZE 8184 // NEW**
#define MAX_UPARTITION_SIZE (EDGE_PACK_SIZE * MAX_UPARTITION_VECSIZE) // 131072 
#define MAX_APPLYPARTITION_VECSIZE MAX_UPARTITION_VECSIZE
#define MAX_APPLYPARTITION_SIZE (EDGE_PACK_SIZE * MAX_APPLYPARTITION_VECSIZE) // 131072 

#define MAX_VDATA_SUBPARTITION_VECSIZE (MAX_UPARTITION_VECSIZE / NUM_PEs)
// #define MAX_NUM_SUBPARTITION_PER_PARTITION NUM_PEs

#define NUM_IMPORT_BUFFERS MAX_NUM_UPARTITIONS // 32 // FIXME. AUTOMATE. 
#define NUM_EXPORT_BUFFERS MAX_NUM_UPARTITIONS //32 
#define INVALID_IOBUFFER_ID (NUM_IMPORT_BUFFERS - 1) // 511

#define UPDATES_BUFFER_PACK_SIZE MAX(EDGE_PACK_SIZE, NUM_PEs)

#define MAX_CSRUPDATES_VECSIZE_PER__APPLYPARTITION 1024

#define BLOCKRAM_SIZE 512
#define DOUBLE_BLOCKRAM_SIZE (BLOCKRAM_SIZE * 2)
#ifdef ___FORCE_SUCCESS_SINGLE_CHANNEL___
#define VPTR_BUFFER_SIZE (512 * 8) // 16
#define EDGE_BUFFER_SIZE (512 * 8) // 16
#define UPDATES_BUFFER_SIZE (512 * 8) // 16
#define EDGEMAPS_BUFFER_SIZE 1024
#else 
#define VPTR_BUFFER_SIZE 512
#define EDGE_BUFFER_SIZE 512   //8192// 512 // FIXME.
#define UPDATES_BUFFER_SIZE 512	
#endif 	
#define VERTEXUPDATES_BUFFER_SIZE 8192

#define EDGE_UPDATES_PTR_MAXSIZE 2048
#define EDGE_UPDATES_CHUNKSZ 2048 // 512 // 512, 1024, 2048*, 4096, 8192  

// #define EDGE_UPDATES_DRAMBUFFER_LONGSIZE (8192 * 64) 
#define EDGE_UPDATES_DRAMBUFFER_LONGSIZE (8192 * 128) 
#define EDGE_UPDATES_DRAMBUFFER_SIZE 8192
#define EDGE_UPDATES_WORKBUFFER_SIZE 7000 // 7200
#define EDGE_LLPUPDATES_DRAMBUFFER_SIZE 512

//////////////// 

// #define MAX_IMPORT_BATCH_SIZE 64
// #define MAX_EXPORT_BATCH_SIZE NAp
#define MAX_IMPORT_BATCH_SIZE 96
#define MAX_EXPORT_BATCH_SIZE 192
// IMPORT_BATCH_SIZE

////////////////

#define GLOBALBUFFER_SIZE 96
#define GLOBALPARAMSCODE__BASEOFFSET__ACTIONS 0
#define GLOBALPARAMSCODE__BASEOFFSET__RAWEDGEUPDATESPTRS 1
#define GLOBALPARAMSCODE__BASEOFFSET__CSRVPTRS 2
#define GLOBALPARAMSCODE__BASEOFFSET__ACTPACKVPTRS 3
#define GLOBALPARAMSCODE__BASEOFFSET__ACTPACKVPTRS2 4
#define GLOBALPARAMSCODE__BASEOFFSET__EDGEUPDATESPTRS 5 //
#define GLOBALPARAMSCODE__BASEOFFSET__UPDATESPTRS 6
#define GLOBALPARAMSCODE__BASEOFFSET__RAWEDGEUPDATES 7 //
#define GLOBALPARAMSCODE__BASEOFFSET__PARTIALLYPROCESSEDEDGEUPDATES 8 //
#define GLOBALPARAMSCODE__BASEOFFSET__CSREDGES 9
#define GLOBALPARAMSCODE__BASEOFFSET__ACTPACKEDGES 10 //
#define GLOBALPARAMSCODE__BASEOFFSET__VERTEXUPDATES 11
#define GLOBALPARAMSCODE__BASEOFFSET__EDGEUPDATES 12 //
#define GLOBALPARAMSCODE__BASEOFFSET__VDATAS 13
#define GLOBALPARAMSCODE__BASEOFFSET__CFRONTIERSTMP 14
#define GLOBALPARAMSCODE__BASEOFFSET__NFRONTIERS 15

#define GLOBALPARAMSCODE__WWSIZE__ACTIONS 20
#define GLOBALPARAMSCODE__WWSIZE__RAWEDGEUPDATESPTRS 21
#define GLOBALPARAMSCODE__WWSIZE__CSRVPTRS 22
#define GLOBALPARAMSCODE__WWSIZE__ACTPACKVPTRS 23
#define GLOBALPARAMSCODE__WWSIZE__ACTPACKVPTRS2 24
#define GLOBALPARAMSCODE__WWSIZE__EDGEUPDATESPTRS 25
#define GLOBALPARAMSCODE__WWSIZE__UPDATESPTRS 26
#define GLOBALPARAMSCODE__WWSIZE__RAWEDGEUPDATES 27
#define GLOBALPARAMSCODE__WWSIZE__PARTIALLYPROCESSEDEDGEUPDATES 28 //
#define GLOBALPARAMSCODE__WWSIZE__CSREDGES 29
#define GLOBALPARAMSCODE__WWSIZE__ACTPACKEDGES 30
#define GLOBALPARAMSCODE__WWSIZE__VERTEXUPDATES 31
#define GLOBALPARAMSCODE__WWSIZE__EDGEUPDATES 32 ////
#define GLOBALPARAMSCODE__WWSIZE__VDATAS 33
#define GLOBALPARAMSCODE__WWSIZE__CFRONTIERSTMP 34
#define GLOBALPARAMSCODE__WWSIZE__NFRONTIERS 35

#define GLOBALPARAMSCODE__PARAM__NUM_VERTICES 40
#define GLOBALPARAMSCODE__PARAM__NUM_EDGES 41
#define GLOBALPARAMSCODE__PARAM__NUM_UPARTITIONS 42
#define GLOBALPARAMSCODE__PARAM__NUM_APPLYPARTITIONS 43
#define GLOBALPARAMSCODE__PARAM__NUM_ITERATIONS 44
#define GLOBALPARAMSCODE__PARAM__THRESHOLD__ACTIVEFRONTIERSFORCONTROLSWITCH 45
#define GLOBALPARAMSCODE__PARAM__MAXDEGREE 46
#define GLOBALPARAMSCODE__PARAM__ALGORITHM 47 
#define GLOBALPARAMSCODE__PARAM__ROOTVID 48
#define GLOBALPARAMSCODE__PARAM__RANGEPERCHANNEL 49
#define GLOBALPARAMSCODE__PARAM__THRESHOLD__ACTIVEDSTVID 50
#define GLOBALPARAMSCODE__PARAM__NUM_RUNS 51
#define GLOBALPARAMSCODE__PARAM__GLOBAL_NUM_PEs 52
#define GLOBALPARAMSCODE__PARAM__MAX_UPARTITION_VECSIZE 53 ////
#define GLOBALPARAMSCODE__PARAM__MAX_UPARTITION_SIZE 54
#define GLOBALPARAMSCODE__PARAM__MAX_APPLYPARTITION_VECSIZE 55
#define GLOBALPARAMSCODE__PARAM__MAX_APPLYPARTITION_SIZE 56

#define GLOBALPARAMSCODE___ENABLE___RESETBUFFERSATSTART 60
#define GLOBALPARAMSCODE___ENABLE___PREPAREEDGEUPDATES 61
#define GLOBALPARAMSCODE___ENABLE___PROCESSEDGEUPDATES 62
#define GLOBALPARAMSCODE___ENABLE___PROCESSEDGES 63
#define GLOBALPARAMSCODE___ENABLE___READ_FRONTIER_PROPERTIES 64
#define GLOBALPARAMSCODE___ENABLE___VCPROCESSEDGES 65
#define GLOBALPARAMSCODE___ENABLE___ECUPDATEEDGES 66
#define GLOBALPARAMSCODE___ENABLE___ECPROCESSEDGES 67
#define GLOBALPARAMSCODE___ENABLE___SAVEVCUPDATES 68
#define GLOBALPARAMSCODE___ENABLE___COLLECTACTIVEDSTVIDS 69
#define GLOBALPARAMSCODE___ENABLE___APPLYUPDATESMODULE 70
#define GLOBALPARAMSCODE___ENABLE___READ_DEST_PROPERTIES 71
#define GLOBALPARAMSCODE___ENABLE___APPLYUPDATES 72
#define GLOBALPARAMSCODE___ENABLE___COLLECT_AND_SAVE_FRONTIER_PROPERTIES 73
#define GLOBALPARAMSCODE___ENABLE___SAVE_DEST_PROPERTIES 74
#define GLOBALPARAMSCODE___ENABLE___EXCHANGEFRONTIERINFOS 75

#define GLOBALPARAMSCODE__ASYNC__BATCH 80
#define GLOBALPARAMSCODE__ASYNC__BATCHSIZE 81

#define GLOBALPARAMSCODE__COMMANDS__COMMAND0 85

////////////////

#define HBM_CHANNEL_READTHROUGHPUT 16 // KVDRAM -> BUFFER: how many uint512_dt wide-words can be read / sec?
#define HBM_CHANNEL_WRITETHROUGHPUT 16 // BUFFER -> KVDRAM: how many uint512_dt wide-words can be written / sec?
#define HBM_CHANNEL_READTHENWRITETHROUGHPUT 16 // KVDRAM(R) -> BUFFER -> KVDRAM(W): how many uint512_dt wide-words can be read / sec?
#define HBM_DRAM_ACCESS_LATENCY 16

#define RESET   "\033[0m"
#define BOLDWHITE   "\033[1m\033[37m"
#define TIMINGRESULTSCOLOR BOLDWHITE

#define __READY__FOR__IMPORT__ 1
#define __READY__FOR__PROCESS__ 2
#define __READY__FOR__EXPORT__ 3
#define __READY__FOR__TRANSPORT__ 4
#define __UNIDENTIFIED__STATUS__ 0

// https://stackoverflow.com/questions/11815894/how-to-read-write-arbitrary-bits-in-c-c

#define BitVal(data,y) ( (data>>y) & 1)      /** Return Data.Y value   **/
#define SetBit(data,y)    data |= (1 << y)    /** Set Data.Y   to 1    **/
#define ClearBit(data,y)  data &= ~(1 << y)   /** Clear Data.Y to 0    **/
#define TogleBit(data,y)     (data ^=BitVal(y))     /** Togle Data.Y  value  **/
#define Togle(data)   (data =~data )         /** Togle Data value     **/

/** uint8_t number = 0x05; //0b00000101
uint8_t bit_2 = BitVal(number,2); // bit_2 = 1
uint8_t bit_1 = BitVal(number,1); // bit_1 = 0
SetBit(number,1); // number =  0x07 => 0b00000111
ClearBit(number,2); // number =0x03 => 0b0000011 */

typedef unsigned int vertex_t;
typedef unsigned int edge_t;

typedef unsigned int keyy_t;
typedef unsigned int value_t;

typedef struct {
	unsigned int key;
	unsigned int value;
} keyvalue_t;

typedef keyvalue_t frontier_t;

typedef struct {
	keyy_t dstvid; 
} edge_type;

typedef struct {
	keyy_t srcvid;	
	keyy_t dstvid; 
} edge2_type;

typedef struct {
	keyy_t srcvid;
	keyy_t dstvid;
	unsigned int weight;
} edge3_type; 

typedef struct {
	edge2_type data[EDGE_PACK_SIZE]; 
} edge2_vec_dt;

typedef struct {
	edge3_type data[EDGE_PACK_SIZE];
} edge3_vec_dt;

typedef edge3_type edge_update_type; // NEW CHANGE

typedef struct {
	edge_update_type data[EDGE_PACK_SIZE];
} edge_update_vec_dt;

typedef struct {
	unsigned int offset;
	unsigned int size;
} map_t;

typedef struct {
	unsigned int offset;
	unsigned int size;
	unsigned int tmp_size;
} map2_t;

typedef struct {
	keyvalue_t data[EDGE_PACK_SIZE];
} uint512_vec_dt;

typedef struct {
	int data[HBM_CHANNEL_PACK_SIZE]; // 32
} uint512_ivec_dt;

typedef struct {
	int data[HBM_AXI_PACK_SIZE]; // 16
} uint512_axivec_dt;

typedef struct {
	int data[HBM_AXI_PACK_SIZE]; // 16 
} uint512_axiswvec_dt;

typedef struct {
	unsigned int begin;
	unsigned int end;
    unsigned int size;
	unsigned int vid;
	unsigned int prop;
} vtr_t;

typedef struct {
	unsigned int A;
    unsigned int B;
} tuple_t;

typedef struct {
	unsigned int local_id;
    unsigned int global_id;
} translator_t;

typedef unsigned int vdata_t;

typedef struct {
	unsigned int prop;
	unsigned int degree; 
} vprop_t;

typedef struct {
	unsigned int prop;
	unsigned int gvid;
} vprop_dest_t;

typedef struct {
	vprop_t data[EDGE_PACK_SIZE];
} vprop_vec_t;

typedef struct {
	unsigned int ALGORITHM; 
	unsigned int NUM_FPGAS_;
	unsigned int GLOBAL_NUM_PEs_;
	unsigned int NUM_ITERATIONS;
	unsigned int ROOTVID;
	
	unsigned int NUM_VERTICES;
	unsigned int NUM_EDGES;
	unsigned int AVERAGENUM_WORKEDGES_PER_CHANNEL;

	unsigned int NUM_UPARTITIONS;
	unsigned int NUM_APPLYPARTITIONS; // NUM_PEs
	
	unsigned int NUM_PARTITIONS;
	
	unsigned int _MAX_UPARTITION_VECSIZE;
	unsigned int _MAX_UPARTITION_SIZE;
	unsigned int _MAX_APPLYPARTITION_VECSIZE;
	unsigned int _MAX_APPLYPARTITION_SIZE;
} universalparams_t;

typedef struct {
	unsigned int BASEOFFSET_VPROP;
} globalparams_t;

typedef struct {
	unsigned int offset;
	unsigned int size;
	unsigned int count;
} offset_t;

#define LOCAL_OWNED_VERTEX 0
#define REMOTE_OWNED_VERTEX 1

typedef struct {
	unsigned int ptr;
	unsigned int msg;
	unsigned int graph_iteration;
} checkpoint_t;

typedef struct {
	unsigned int fpga; 
	unsigned int module; 
	unsigned int graph_iteration;
	unsigned int start_pu; 
	unsigned int size_pu; 
	unsigned int skip_pu; 
	unsigned int start_pv_fpga;
	unsigned int start_pv;
	unsigned int size_pv; 
	unsigned int start_llpset; 
	unsigned int size_llpset; 
	unsigned int start_llpid; 
	unsigned int size_llpid; 
	unsigned int start_gv_fpga;
	unsigned int start_gv; 
	unsigned int size_gv;
	unsigned int id_process;
	unsigned int id_import;
	unsigned int id_export;
	unsigned int size_import_export;
	unsigned int status;
	unsigned int numfpgas;
	unsigned int command;
} action_t; 

typedef struct {
	unsigned int status;
	unsigned int ready_for_import;
	unsigned int ready_for_export;
	unsigned int ready_for_process;
	unsigned int iteration;
} gas_t;

typedef struct {
	unsigned int ready_for_import;
	unsigned int iteration;
	unsigned int tmp_state;
} gas_import_t;

typedef struct {
	unsigned int ready_for_process;
	unsigned int iteration;
	unsigned int tmp_state;
} gas_process_t;

typedef struct {
	unsigned int ready_for_export;
	unsigned int iteration;
	unsigned int tmp_state;
} gas_export_t;

typedef struct {
	unsigned int fpga;
	unsigned int io_id;
	// bool data_transferred;
} mapping_t;

// HBM: {vptrs, edges, updatesptrs, updates, vertexprops, frontiers}
typedef uint512_ivec_dt HBM_channel_t;
typedef uint512_axiswvec_dt HBM_channelAXISW_t;
typedef uint512_axivec_dt HBM_channelAXI_t;
#endif

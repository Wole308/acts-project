#ifndef ACTSUTILITY_H
#define ACTSUTILITY_H
#include "../include/config_params.h"
#include "../include/common.h"
#ifndef HW
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include <map>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <mutex>
#include <string>
#include <iostream>
#include <string.h>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include <iomanip>
#include <cmath>
#include <fstream>
#endif 
#ifndef HW
#include "../host_srcs/utility.h"
#endif
using namespace std;

#define MYSTATSYSIZE 64 // 4//8//16//64

class actsutility {
public:

	actsutility();
	~actsutility();
	
	void checkoutofbounds(string message, unsigned int data, unsigned int upper_bound, unsigned int msgdata1, unsigned int msgdata2, unsigned int msgdata3);
	void checkoutofbounds(unsigned int enable, string message, unsigned int data, unsigned int upper_bound, unsigned int msgdata1, unsigned int msgdata2, unsigned int msgdata3);
	void checkforequal(string message, unsigned int data1, unsigned int data2, unsigned int msgdata1, unsigned int msgdata2, unsigned int msgdata3);
	void checkforequal(string message, unsigned int * data1, unsigned int * data2, unsigned int size, unsigned int msgdata1, unsigned int msgdata2, unsigned int msgdata3);
	void hcheckforequal(unsigned int enable, string message, unsigned int * data1, unsigned int * data2, unsigned int size, unsigned int msgdata1, unsigned int msgdata2, unsigned int msgdata3);
	void checkfornotequal(string message, unsigned int data1, unsigned int data2, unsigned int msgdata1, unsigned int msgdata2, unsigned int msgdata3);
	void checkfornotequalbyerrorwindow(string message, unsigned int data1, unsigned int data2, unsigned int errorwindow);
	void checkforoverlap(string message, keyvalue_t * keyvalues, unsigned int size);
	void checkforgreaterthan(string message, keyvalue_t * keyvalues1, keyvalue_t * keyvalues2, unsigned int size);
	void checkforgreaterthan(string message, value_t * values1, value_t * values2, unsigned int size);
	void checkforequal(string message, keyvalue_t * keyvalues1, keyvalue_t * keyvalues2, unsigned int size);
	void checkforlessthanthan(string message, unsigned int data1, unsigned int data2);
	void checkforlessthanthan(string message, unsigned int data1, unsigned int data2, unsigned int tolerance);
	void checkfordivisibleby(string message, unsigned int data, unsigned int n);
	void checkfordivisibleby(unsigned int enable, string message, unsigned int data, unsigned int n);
	void print1(string messagea, unsigned int dataa);
	void print2(string messagea, string messageb, unsigned int dataa, unsigned int datab);
	void print3(string messagea, string messageb, string messagec, unsigned int dataa, unsigned int datab, unsigned int datac);
	void print4(string messagea, string messageb, string messagec, string messaged, unsigned int dataa, unsigned int datab, unsigned int datac, unsigned int datad);
	void print5(string messagea, string messageb, string messagec, string messaged, string messagee, unsigned int dataa, unsigned int datab, unsigned int datac, unsigned int datad, unsigned int datae);
	void print6(string messagea, string messageb, string messagec, string messaged, string messagee, string messagef, unsigned int dataa, unsigned int datab, unsigned int datac, unsigned int datad, unsigned int datae, unsigned int datef);
	void print7(string messagea, string messageb, string messagec, string messaged, string messagee, string messagef, string messageg, unsigned int dataa, unsigned int datab, unsigned int datac, unsigned int datad, unsigned int datae, unsigned int dataf, unsigned int datag);
	void print8(string messagea, string messageb, string messagec, string messaged, string messagee, string messagef, string messageg, string messageh, unsigned int dataa, unsigned int datab, unsigned int datac, unsigned int datad, unsigned int datae, unsigned int dataf, unsigned int datag, unsigned int datah);
	void printkeyvalues(string message, keyvalue_t * keyvalues, unsigned int size);
	void printkeyvalues(string message, keyvalue_t * keyvalues, unsigned int size, unsigned int skipsize);
	void printkeyvalues(string message, keyvalue_t * keyvalues1, keyvalue_t * keyvalues2, unsigned int size);
	void printkeyvalues(string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], unsigned int size);
	void printkeyvalues(string message, keyvalue_buffer_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], unsigned int size);
	void printkeyvalues(string message, keyvalue_t keyvalues[VECTOR_SIZE][DOUBLE_BLOCKRAM_SIZE], unsigned int size);
	void printkeyvalues(string message, keyvalue_t keyvalues[MAX_NUM_PARTITIONS][BLOCKRAM_SIZE], unsigned int numcols, unsigned int size);
	void printkeyvalues(string message, keyvalue1_type keyvalues[MAX_NUM_PARTITIONS][BLOCKRAM_SIZE], unsigned int numcols, unsigned int size);
	void printkeyvalues(string message, uint512_dt * keyvalues, unsigned int size_kvs);
	void printvaluecount(string message, keyvalue_t * keyvalues, unsigned int size);
	void printvalues(string message, unsigned int * values, unsigned int size);
	void printvalueslessthan(string message, value_t * values, unsigned int size, unsigned int data);
	void printparameters();
	void printglobalvars();
	void printglobalvars2(unsigned int instID, string messagea, string messageb, string messagec, string messaged, unsigned int varA, unsigned int varB, unsigned int varC, unsigned int varD);
	void printglobalparameters(string message, globalparams_t globalparams);
	void printpartitionresult(unsigned int enable, uint512_dt * kvdram, keyvalue_t * globaldestoffsets, keyvalue_t * globalstatsbuffer, sweepparams_t sweepparams);
	void printpartitionresult2(unsigned int enable, uint512_dt * kvdram, keyvalue_t * globalstatsbuffer, sweepparams_t sweepparams);
	
	unsigned int ugetvaluecount(keyvalue_t * keyvalues, unsigned int size);
	unsigned int getvaluecountexcept(keyvalue_t * keyvalues, unsigned int size, unsigned int exceptvalue);
	unsigned int getvaluecountexcept(string message, keyvalue_t * keyvalues, unsigned int size, unsigned int exceptvalue);
	void setkeyvalues(string message, keyvalue_t * keyvalues, unsigned int size, keyvalue_t keyvalue);
	void clearglobalvars();
	void IsEqual(keyvalue_t ** data1, keyvalue_t ** data2, unsigned int _1stdimsize, unsigned int _2nddimsize);
	void scankeyvalues(string message, keyvalue_t * keyvalues, keyvalue_t * stats, unsigned int numberofpartitions, unsigned int rangeperpartition, unsigned int upperlimit);
	unsigned int geterrorkeyvalues(keyvalue_t * keyvalues, unsigned int begin, unsigned int end, unsigned int lowerrangeindex, unsigned int upperrangeindex);
	void setstructs(config_t _config, sweepparams_t _sweepparams, travstate_t _travstate);
	config_t getconfig();
	sweepparams_t getsweepparams();
	travstate_t gettravstate();
	unsigned int countvalues(string message, value_t * values, unsigned int size);
	void countvalueslessthan(string message, value_t * values, unsigned int size, unsigned int data);
	void checkgraph(keyvalue_t * vertexptrs, keyvalue_t * edges, unsigned int edgessize);
	void checkptr(string message, unsigned int beginsrcvid, unsigned int endsrcvid, unsigned int beginvptr, unsigned int endvptr, keyvalue_t * edges);
	void checkptr(string message, unsigned int beginsrcvid, unsigned int endsrcvid, unsigned int beginvptr, unsigned int endvptr, keyvalue_t * edges, unsigned int numedges);
	void resetkeyvalues(keyvalue_t * keyvalues, unsigned int size);
	void resetvalues(keyvalue_t * keyvalues, unsigned int size);
	void copykeyvalues(keyvalue_t * keyvalues1, keyvalue_t * keyvalues2, unsigned int size);
	
	void globalstats_countkvstatsread(unsigned int count);
	void globalvar_collectstats_counttotalkvsread(unsigned int count);
	void globalvar_extractcapsules_counttotalkvsread(unsigned int count);
	void globalvar_organizekeyvalues_counttotalkvsread(unsigned int count);
	void globalvar_savestats_counttotalstatswritten(unsigned int count);
	void globalvar_savepartitions_countinvalids(unsigned int count);
	void globalvar_inmemory_counttotalvalidkeyvalues(unsigned int count);
	void globalstats_countkvsread(unsigned int count);
	void globalstats_countkvswritten(unsigned int count);
	void globalstats_countkvsread_partition(unsigned int count);
	void globalstats_countkvswritten_partition(unsigned int count);
	void globalstats_countkvsread_reduce(unsigned int count);
	void globalstats_countkvswritten_reduce(unsigned int count);
	void globalstats_countvsread(unsigned int count);
	void globalstats_countvswritten(unsigned int count);
	void globalstats_countkvspartitionswritten(unsigned int instID, unsigned int count);
	void globalstats_countkvspartitionswritten_actual(unsigned int count);
	void globalstats_countkvspartitioned(unsigned int count);
	void globalstats_countkvsreduced(unsigned int instID, unsigned int count);
	void globalstats_reduce_countvalidkvsreduced(unsigned int instID, unsigned int count);
	void globalstats_reduce_countvar1(unsigned int count);
	void globalstats_countkvsreducewritten(unsigned int instID, unsigned int count);
	void globalstats_countkvsprocessed(unsigned int instID, unsigned int count);
	void globalstats_processedges_countvalidkvsprocessed(unsigned int instID, unsigned int count);
	void globalstats_countkvsreadV(unsigned int count);
	void globalstats_counterrorsingetpartition(unsigned int count);
	void globalstats_counterrorsinreduce(unsigned int count);
	void globalstats_counterrorsinprocessedges(unsigned int count);
	void globalstats_countkvsmerged(unsigned int count);
	void globalstats_countactvvsseen(unsigned int count);
	
	unsigned int globalstats_getcountvalidkvsprocessed(unsigned int instID);
	unsigned int globalstats_getcounterrorsinreduce();
	unsigned int globalstats_getcountkvsreduced(unsigned int instID);
	unsigned int globalstats_getcountvalidkvsreduced(unsigned int instID);
	unsigned int globalstats_getcountkvspartitionswritten(unsigned int instID);
	unsigned int globalstats_getreducevar1();
	unsigned int globalstats_getcounterrorsinprocessedges();
	unsigned int globalstats_getcountnumvalidprocessedges(unsigned int instID);
	unsigned int globalstats_getactvvsseen();
	
	void globalstats_setactvvsseen(unsigned int val);
	
	partition_type getpartition(string message, keyvalue_t keyvalue, step_type currentLOP, vertex_t upperlimit, unsigned int batch_range_pow);
	void checkn(unsigned int enable, string message, keyvalue_t * kv, unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int n);
	void concatenate2keyvalues(string message, keyvalue_t * keyvalues1, keyvalue_t * keyvalues2, keyvalue_t * stats, keyvalue_t * BIGkeyvalues, keyvalue_t * BIGstats, unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow);
	void concatenate4keyvalues(string message, keyvalue_t * keyvalues1, keyvalue_t * keyvalues2, keyvalue_t * keyvalues3, keyvalue_t * keyvalues4, keyvalue_t * stats, keyvalue_t * BIGkeyvalues, keyvalue_t * BIGstats, unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow);						
	void concatenate8keyvalues(string message, keyvalue_t * keyvalues1, keyvalue_t * keyvalues2, keyvalue_t * keyvalues3, keyvalue_t * keyvalues4, keyvalue_t * keyvalues5, keyvalue_t * keyvalues6, keyvalue_t * keyvalues7, keyvalue_t * keyvalues8, keyvalue_t * stats, keyvalue_t * BIGkeyvalues, keyvalue_t * BIGstats, unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow);
	
	void printprofileso1(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[8][MAX_NUM_PARTITIONS], unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int partitioncount[MAX_NUM_PARTITIONS]);
	void printprofileso2(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[4][MAX_NUM_PARTITIONS], unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int partitioncount[MAX_NUM_PARTITIONS]);
	void printprofileso4(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[2][MAX_NUM_PARTITIONS], unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int partitioncount[MAX_NUM_PARTITIONS]);
	void printprofileso8(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[MAX_NUM_PARTITIONS], unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int partitioncount[MAX_NUM_PARTITIONS]);
	void printprofile(unsigned int enable, string message, keyvalue_t * keyvalues, keyvalue_t * stats, unsigned int size, unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int partitioncount[MAX_NUM_PARTITIONS]);
	void printprofile(unsigned int enable, string message, keyvalue_t * buffer, unsigned int size, unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow);
	
	void getprofileso1(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[8][MAX_NUM_PARTITIONS], unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int partitioncount[MAX_NUM_PARTITIONS]);
	void getprofileso2(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[4][MAX_NUM_PARTITIONS], unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int partitioncount[MAX_NUM_PARTITIONS]);
	void getprofileso4(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[2][MAX_NUM_PARTITIONS], unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int partitioncount[MAX_NUM_PARTITIONS]);
	void getprofileso8(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[MAX_NUM_PARTITIONS], unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int partitioncount[MAX_NUM_PARTITIONS]);
	void checkprofile(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], unsigned int size_kvs, unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow, unsigned int factor, unsigned int totalnum);
	void checkbufferprofile(unsigned int enable, string message, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[MAX_NUM_PARTITIONS], unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow);
	void compareprofiles(unsigned int enable, string message, keyvalue_t * buffer1, keyvalue_t * buffer2, unsigned int size, unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow);
	unsigned int countkeysbelongingtopartition(unsigned int p, keyvalue_t * buffer, unsigned int size, unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow);
						
	void collectstats(unsigned int enable, keyvalue_t sourcebuffer[VECTOR_SIZE][BLOCKRAM_SIZE], unsigned int chunksize_kvs, step_type currentLOP, vertex_t upperlimit, unsigned int batch_range_pow, unsigned int x, unsigned int y);
	void collectstats(unsigned int enable, keyvalue_t keyvalues[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t stats[MAX_NUM_PARTITIONS], step_type currentLOP, vertex_t upperlimit, unsigned int batch_range_pow, unsigned int x, unsigned int y);
	void collectstats(unsigned int enable, keyvalue_t * keyvalues, keyvalue_t localstats[MAX_NUM_PARTITIONS], step_type currentLOP, vertex_t upperlimit, unsigned int batch_range_pow, unsigned int x, unsigned int y);
	void collectstats(unsigned int enable, keyvalue_t localcapsule[MAX_NUM_PARTITIONS], unsigned int x, unsigned int y);
	void collectstats(unsigned int enable, keyvalue_t * kvdram, unsigned int size, step_type currentLOP, vertex_t upperlimit, unsigned int batch_range_pow, unsigned int x, unsigned int y);
	unsigned int * getstats(unsigned int x, unsigned int y);
	void clearallstats();
	void clearstats(unsigned int x);
	keyvalue_t * getmykeyvalues(unsigned int x);
	unsigned int getmincutoffseen();
	unsigned int getmaxcutoffseen();
	void updatemincutoffseen(unsigned int val);
	unsigned int updatemaxcutoffseen(unsigned int val);
	
	void intrarunpipelinecheck_shifting(unsigned int enable, string message, 
							keyvalue_t bufferA[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t buffer1capsule[8][MAX_NUM_PARTITIONS], 
							keyvalue_t bufferB[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t bufferBcapsule[4][MAX_NUM_PARTITIONS],
							keyvalue_t bufferC[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t bufferCcapsule[2][MAX_NUM_PARTITIONS],
							keyvalue_t bufferD[VECTOR_SIZE][BLOCKRAM_SIZE], keyvalue_t bufferDcapsule[MAX_NUM_PARTITIONS],
							unsigned int partitioncountso1[MAX_NUM_PARTITIONS], 
							unsigned int partitioncountso2[MAX_NUM_PARTITIONS],
							unsigned int partitioncountso4[MAX_NUM_PARTITIONS],
							unsigned int partitioncountso8[MAX_NUM_PARTITIONS],
							unsigned int currentLOP, unsigned int upperlimit, unsigned int batch_range_pow);
	void intrapartitioncheck();
	void postpartitioncheck(uint512_dt * kvdram, keyvalue_t globalstatsbuffer[MAX_NUM_PARTITIONS], travstate_t ptravstate, sweepparams_t sweepparams, globalparams_t globalparams);
	
	// compact graph utilities
	void DECTOBINARY(int n);
	void ULONGTOBINARY(ulong_dt n);
	void ULONGTOBINARY(keyvalue_t keyvalue);
	void UINTTOBINARY(unsigned int n);
	ulong_dt CONVERTTOLONG_KV(keyvalue_t keyvalue);
	keyvalue_t CONVERTTOKV_ULONG(ulong_dt data);
	unsigned int GETMASK_UINT(unsigned int index, unsigned int size);
	ulong_dt GETMASK_ULONG(ulong_dt index, ulong_dt size);
	unsigned int READFROM_UINT(unsigned int data, unsigned int index, unsigned int size);
	unsigned int READFROM_ULONG(ulong_dt data, ulong_dt index, ulong_dt size);
	unsigned int READFROM_ULONG(keyvalue_t keyvalue, ulong_dt index, ulong_dt size);
	void WRITETO_ULONG(ulong_dt * data, ulong_dt index, ulong_dt size, ulong_dt value);
	void WRITETO_ULONG(keyvalue_t * keyvalue, ulong_dt index, ulong_dt size, ulong_dt value);
	// void PUSH(uuint64_dt * longword, unsigned int data, unsigned int databitsz);
	void PARSE(string message, ulong_dt longword);
	void PARSE(string message, keyvalue_t keyvalue);
	unsigned int PARSE(ulong_dt longword, unsigned int * _items);
	unsigned int PARSE(keyvalue_t keyvalue, unsigned int * _items);
	unsigned int GETKEY(ulong_dt longword);
	
	void reducehelper_checkreduceloc(unsigned int i, unsigned int loc, keyvalue_t keyvalue, sweepparams_t sweepparams, globalparams_t globalparams);
	
private:
	unsigned int globalvar_totalkvstatsread;
	unsigned int globalvar_totalkvsreadV;
	unsigned int globalvar_collectstats_totalkvsread;
	unsigned int globalvar_extractcapsules_totalkvsread;
	unsigned int globalvar_organizekeyvalues_totalkvsread;
	unsigned int globalvar_savestats_totalstatswritten;
	unsigned int globalvar_savepartitions_invalids;
	unsigned int globalvar_inmemory_totalvalidkeyvalues;
	
	unsigned int globalvar_totalkvsread;
	unsigned int globalvar_totalkvswritten;
	unsigned int globalvar_totalkvsread_partition;
	unsigned int globalvar_totalkvswritten_partition;
	unsigned int globalvar_totalkvsread_reduce;
	unsigned int globalvar_totalkvswritten_reduce;
	unsigned int globalvar_totalvsread;
	unsigned int globalvar_totalvswritten;
	
	unsigned int globalstats_totalkvspartitioned;
	unsigned int globalstats_totalkvspartitionswritten[64];
	unsigned int globalstats_totalkvspartitionswritten_actual;
	unsigned int globalstats_totalkvsreduced[64];
	unsigned int globalstats_reduce_validkvsreduced[64];
	unsigned int globalstats_reduce_var1;
	unsigned int globalstats_totalkvsprocessed[64];
	unsigned int globalstats_processedges_validkvsprocessed[64];
	unsigned int globalstats_totalkvsreducewritten[64];
	unsigned int globalvar_errorsingetpartition;
	unsigned int globalvar_errorsinreduce;
	unsigned int globalvar_errorsinprocessedges;
	unsigned int globalstats_totalkvsmerged;
	unsigned int globalvar_totalactvvsseen;
	
	config_t config;
	sweepparams_t sweepparams;
	travstate_t travstate;
	
	unsigned int mystats[8][MYSTATSYSIZE][MAX_NUM_PARTITIONS];
	keyvalue_t mykeyvalues[8][MAX_NUM_PARTITIONS];
	unsigned int mincutoffseen;
	unsigned int maxcutoffseen;
};
#endif








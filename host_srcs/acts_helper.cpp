#include "acts_helper.h"
using namespace std;

#define ACTS_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC ((23 * 1000) / 10) // ((23 / 10) * 1000) // {23M uint32 seen processed in 10ms}{10M uint32 seen processed in 4ms}
// #define GRAPHLILY_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC ((49 * 1000) / 36) // {49M uint32 seen processed in 36ms}
#define GRAPHLILY_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC ((244 * 1000) / 36) // {244M uint32 seen processed in 36ms in all 17 channels}

acts_helper::acts_helper(universalparams_t _universalparams){
	utilityobj = new utility(_universalparams);
	myuniversalparams = _universalparams;
}
acts_helper::~acts_helper(){} 

unsigned int acts_helper::gethash(unsigned int vid){
	return vid % NUM_PEs;
}
unsigned int acts_helper::getlocalvid(unsigned int vid){
	unsigned int s = gethash(vid);
	return (vid - s) / NUM_PEs; 
}

void acts_helper::set_edgeblock_headers(int GraphIter, unsigned int v_p, 
	uint512_ivec_dt * tempvdram, uint512_ivec_dt * tempkvdram[NUM_PEs], unsigned int vdram_BASEOFFSETKVS_ACTIVEEDGEBLOCKS, unsigned int kvdram_BASEOFFSETKVS_ACTIVEEDGEBLOCKS,
		unsigned int * indexes[MAXNUM_PEs]){
	unsigned int num_LLPs = myuniversalparams.NUMREDUCEPARTITIONS * myuniversalparams.NUM_PARTITIONS; 
	unsigned int num_LLPset = (num_LLPs + (myuniversalparams.NUM_PARTITIONS - 1)) / myuniversalparams.NUM_PARTITIONS;
	
	for(unsigned int H=0; H<NUM_PEs; H++){
		if(utilityobj->isbufferused(H) == false){ continue; }
		for(unsigned int llp_set=0; llp_set<num_LLPset; llp_set++){
			#ifdef _DEBUGMODE_HOSTPRINTS3//4 // 4*
			if(v_p == 0){ cout<<"extract_stats++:: iter: "<<GraphIter + 1<<", indexes["<<H<<"]["<<v_p<<"]["<<llp_set<<"]: "<<indexes[H][llp_set]<<endl; } //if(v_p == 0){ }
			#endif 
			unsigned int offset = ((v_p * MAXNUMGRAPHITERATIONS * MAXNUM_EDGEBLOCKS_PER_VPARTITION) + ((GraphIter + 1) * MAXNUM_EDGEBLOCKS_PER_VPARTITION));
			tempkvdram[H][kvdram_BASEOFFSETKVS_ACTIVEEDGEBLOCKS + offset + 0].data[llp_set] = indexes[H][llp_set]; // indexes[H][llp_set], 0xFFFFFFFF
		}
	}	
}

unsigned int acts_helper::extract_stats(uint512_vec_dt * vdram, uint512_vec_dt * kvbuffer[MAXNUM_PEs], 
		vector<vertex_t> &srcvids, vector<edge_t> &vertexptrbuffer, vector<edge2_type> &edgedatabuffer, 
			long double edgesprocessed_totals[128], tuple_t * vpartition_stats[MAXNUMGRAPHITERATIONS], unsigned int num_edges_processed[MAXNUMGRAPHITERATIONS], globalparams_TWOt globalparams){						
	if(myuniversalparams.ALGORITHM == BFS || myuniversalparams.ALGORITHM == SSSP){} else {
		for(unsigned int iter=0; iter<MAXNUMGRAPHITERATIONS; iter++){ for(unsigned int t=0; t<myuniversalparams.NUMPROCESSEDGESPARTITIONS; t++){ vpartition_stats[iter][t].A = 10; vpartition_stats[iter][t].B = 11; }}
		edgesprocessed_totals[0] = edgedatabuffer.size(); 
		num_edges_processed[0] = edgedatabuffer.size(); 
		return 1; }
	#ifdef _DEBUGMODE_HOSTPRINTS3
	cout<<endl<<"acts_helper:: running traditional sssp... "<<endl;
	#endif 
	
	unsigned int num_vPs = myuniversalparams.NUMPROCESSEDGESPARTITIONS;
	unsigned int vsize_vP = myuniversalparams.PROCESSPARTITIONSZ;
	unsigned int num_LLPs = myuniversalparams.NUMREDUCEPARTITIONS * myuniversalparams.NUM_PARTITIONS; 
	unsigned int vsize_LLP = myuniversalparams.REDUCEPARTITIONSZ_KVS2;
	unsigned int vsize_LLPset = vsize_LLP * VDATA_PACKINGSIZE;
	unsigned int num_LLPset = (num_LLPs + (myuniversalparams.NUM_PARTITIONS - 1)) / myuniversalparams.NUM_PARTITIONS;
	
	uint512_ivec_dt * tempvdram = (uint512_ivec_dt *)vdram;	
	unsigned int * tempvdram_u32 = (unsigned int *)vdram;	
	uint512_ivec_dt * tempkvdram[NUM_PEs]; for(unsigned int i=0; i<NUM_PEs; i++){ tempkvdram[i] = (uint512_ivec_dt *)kvbuffer[i]; }
	unsigned int * tempkvdram_u32[NUM_PEs]; for(unsigned int i=0; i<NUM_PEs; i++){ tempkvdram_u32[i] = (unsigned int *)kvbuffer[i]; }
	
	unsigned int * upropblock_stats[MAXNUMGRAPHITERATIONS][MAXNUM_VPs]; 
	for(unsigned int iter=0; iter<MAXNUMGRAPHITERATIONS; iter++){ 
		for(unsigned int v_p=0; v_p<MAXNUM_VPs; v_p++){ 
			upropblock_stats[iter][v_p] = new unsigned int[myuniversalparams.KVDATA_RANGE / NUM_VERTICES_PER_UPROPBLOCK];
			for(unsigned int t=0; t<myuniversalparams.KVDATA_RANGE / NUM_VERTICES_PER_UPROPBLOCK; t++){ upropblock_stats[iter][v_p][t] = 0; }
		}
	}
	
	vector<value_t> actvvs;
	vector<value_t> actvvs_nextit;
	unsigned int * vdatas = new unsigned int[myuniversalparams.KVDATA_RANGE];
	unsigned int * vmasks = new unsigned int[myuniversalparams.KVDATA_RANGE];
	unsigned int * vdatas_tmp = new unsigned int[myuniversalparams.KVDATA_RANGE];
	unsigned int * indexes[MAXNUM_PEs]; for(unsigned int i=0; i<MAXNUM_PEs; i++){ indexes[i] = new unsigned int[MAXNUM_LLPSETs]; }
	tuple_t * iteration_stats[MAXNUMGRAPHITERATIONS]; for(unsigned int i=0; i<MAXNUMGRAPHITERATIONS; i++){ iteration_stats[i] = new tuple_t[myuniversalparams.NUMPROCESSEDGESPARTITIONS]; }
	
	for(unsigned int i=0; i<myuniversalparams.KVDATA_RANGE; i++){ vdatas[i] = 0xFFFFFFFF; vdatas_tmp[i] = 0xFFFFFFFF; vmasks[i] = 0; }
	unsigned int total_edges_processed = 0;
	for(unsigned int i=0; i<128; i++){ edgesprocessed_totals[i] = 0; }
	for(unsigned int i=0; i<srcvids.size(); i++){ actvvs.push_back(srcvids[i]); }
	#ifdef _DEBUGMODE_HOSTPRINTS3
	cout<<"acts_helper: number of active vertices for iteration 0: 1"<<endl;
	#endif 
	for(unsigned int i=0; i<actvvs.size(); i++){ vdatas[actvvs[i]] = 0; }
	unsigned int GraphIter=0;
	vpartition_stats[0][0].A = 1; 
	upropblock_stats[0][0][actvvs[0] / NUM_VERTICES_PER_UPROPBLOCK] = 1;
	for(unsigned int i=0; i<MAXNUM_PEs; i++){ for(unsigned int t=0; t<MAXNUM_LLPSETs; t++){ indexes[i][t] = 0; }}
	for(unsigned int iter=0; iter<MAXNUMGRAPHITERATIONS; iter++){ num_edges_processed[iter] = 0; }
	unsigned int num_iters = MAXNUMGRAPHITERATIONS; if(myuniversalparams.NUM_ITERATIONS < 5){ num_iters = myuniversalparams.NUM_ITERATIONS; }
	for(unsigned int iter=0; iter<MAXNUMGRAPHITERATIONS; iter++){ for(unsigned int t=0; t<myuniversalparams.NUMPROCESSEDGESPARTITIONS; t++){ iteration_stats[iter][t].A = 0; iteration_stats[iter][t].B = 0; }}
	bool onetime = false;

	for(GraphIter=0; GraphIter<MAXNUMGRAPHITERATIONS; GraphIter++){ // MAXNUMGRAPHITERATIONS
		for(unsigned int i=0; i<MAXNUM_PEs; i++){ for(unsigned int t=0; t<MAXNUM_LLPSETs; t++){ indexes[i][t] = 0; }}
		
		// processing phase 
		for(unsigned int i=0; i<actvvs.size(); i++){
			unsigned int vid = actvvs[i];
			
			edge_t vptr_begin = vertexptrbuffer[vid];
			edge_t vptr_end = vertexptrbuffer[vid+1];
			edge_t edges_size = vptr_end - vptr_begin;
			if(vptr_end < vptr_begin){ continue; } // FIXME.
			#ifdef _DEBUGMODE_CHECKS3
			if(vptr_end < vptr_begin){ cout<<"ERROR: vptr_end("<<vptr_end<<") < vptr_begin("<<vptr_begin<<"). exiting..."<<endl; exit(EXIT_FAILURE); }
			#endif
			
			num_edges_processed[GraphIter] += edges_size;
			vpartition_stats[GraphIter][vid / myuniversalparams.PROCESSPARTITIONSZ].B += edges_size; // 
			
			for(unsigned int k=0; k<edges_size; k++){
				unsigned int dstvid = edgedatabuffer[vptr_begin + k].dstvid;
				// if(GraphIter<2){ cout<<"---- acts_helper:: iter: "<<GraphIter<<", srcvid: "<<vid<<", dstvid: "<<dstvid<<" ----"<<endl; }
				
				// if(universalparams.ALGORITHM == BFS || universalparams.ALGORITHM == SSSP)
				if(myuniversalparams.ALGORITHM == BFS){
					unsigned int res = NAp; // vdatas[vid] + 1;
					value_t vprop = vdatas[dstvid];
					value_t vtemp = min(vprop, GraphIter);
					vdatas_tmp[dstvid] = vtemp;	
				} else if(myuniversalparams.ALGORITHM == SSSP){
					unsigned int res = vdatas[vid] + 1;
					value_t vprop = vdatas[dstvid];
					value_t vtemp = min(vprop, res);
					vdatas_tmp[dstvid] = vtemp;	
				} else {
					unsigned int res = vdatas[vid] + 1;
					value_t vprop = vdatas[dstvid];
					value_t vtemp = min(vprop, res);
					vdatas_tmp[dstvid] = vtemp;	
				}
				
				total_edges_processed += 1;
				edgesprocessed_totals[GraphIter] += 1; 
			}			
		}
		// exit(EXIT_SUCCESS);
		
		// apply phase 
		unsigned int v_p = 0;
		for(unsigned int vid=0; vid<myuniversalparams.KVDATA_RANGE; vid++){
			// resets 
			if(vid % myuniversalparams.PROCESSPARTITIONSZ == 0){ 
				for(unsigned int H=0; H<NUM_PEs; H++){ for(unsigned int llp_set=0; llp_set<myuniversalparams.NUMREDUCEPARTITIONS; llp_set++){ indexes[H][llp_set] = 0; }}
			}
				
			// apply 
			if(vdatas_tmp[vid] != vdatas[vid]){
				vdatas[vid] = vdatas_tmp[vid];
				#ifdef CONFIG_PRELOADEDVERTEXMASKS
				unsigned int stored_vdata = tempvdram_u32[(globalparams.globalparamsV.BASEOFFSETKVS_SRCVERTICESDATA * VECTOR2_SIZE) + vid];
				tempvdram_u32[(globalparams.globalparamsV.BASEOFFSETKVS_SRCVERTICESDATA * VECTOR2_SIZE) + vid] = (stored_vdata | (1 << GraphIter));
				#endif 
				actvvs_nextit.push_back(vid);
				utilityobj->checkoutofbounds("acts_helper:: ERROR 20", vid / myuniversalparams.PROCESSPARTITIONSZ, myuniversalparams.NUMPROCESSEDGESPARTITIONS, vid, vid, vid);
			
				// load vpartition & upropblock stats
				unsigned int v_p_ = vid / myuniversalparams.PROCESSPARTITIONSZ;
				unsigned int lvid = vid - (v_p_ * myuniversalparams.PROCESSPARTITIONSZ);
				// if(GraphIter<2){ cout<<"---- acts_helper:: iter: "<<GraphIter<<", vid: "<<vid<<", lvid: "<<lvid<<", v_p_: "<<v_p_<<", -: "<<NAp<<endl; }
			
				if(upropblock_stats[GraphIter+1][v_p_][lvid / NUM_VERTICES_PER_UPROPBLOCK] == 0
					// && H==7
				){ 
					upropblock_stats[GraphIter+1][v_p_][lvid / NUM_VERTICES_PER_UPROPBLOCK] = 1; 
					vpartition_stats[GraphIter+1][vid / myuniversalparams.PROCESSPARTITIONSZ].A += 1; 
				}
				iteration_stats[GraphIter+1][vid / myuniversalparams.PROCESSPARTITIONSZ].A += 1; 	
	
				// load edgeblock stats
				#ifdef _DEBUGMODE_HOSTPRINTS4
				if(onetime == false){ cout<<">>> extract_stats: populating metadata (edgeblock stats) ... "<<endl; onetime = true; }
				#endif
			}
		}
		
		// check for finish
		#ifdef _DEBUGMODE_HOSTPRINTS3
		cout<<"acts_helper: number of active vertices for iteration "<<GraphIter + 1<<": "<<actvvs_nextit.size()<<""<<endl;
		#endif 
		if(actvvs_nextit.size() == 0 || GraphIter >= MAXNUMGRAPHITERATIONS){ 
			#ifdef _DEBUGMODE_HOSTPRINTS3
			cout<<"no more activer vertices to process. breaking out... "<<endl; 
			#endif 
			break; 
		}
	
		actvvs.clear();
		for(unsigned int i=0; i<actvvs_nextit.size(); i++){ actvvs.push_back(actvvs_nextit[i]); }
		actvvs_nextit.clear();
	}
	// exit(EXIT_SUCCESS);
	
	// load vpartition stats
	#ifdef _DEBUGMODE_HOSTPRINTS4
	cout<<">>> extract_stats: populating metadata (vpartition_stats) ... "<<endl;		
	#endif
	unsigned int vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK = vdram[BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_BASEOFFSETKVS_VERTICESPARTITIONMASK].data[0].key;
	#ifdef NOT____USED
	for(unsigned int iter=0; iter<MAXNUMGRAPHITERATIONS; iter++){
		for(unsigned int v_p=0; v_p<myuniversalparams.NUMPROCESSEDGESPARTITIONS; v_p++){
			tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + v_p].data[iter] = vpartition_stats[iter][v_p].A;
			if(iter==0 && v_p < 16 && false){ cout<<"~~~ acts_helper: tempvdram["<<vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + v_p<<"].data["<<iter<<"]: "<<tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + v_p].data[iter]<<endl; }
			// if(iter==0){ cout<<"~~~ tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + "<<v_p<<"].data["<<iter<<"]: "<<tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + v_p].data[iter]<<endl; }
		}
	}
	#endif 

	// load upropblock stats
	#ifdef _DEBUGMODE_HOSTPRINTS4
	cout<<">>> extract_stats: populating metadata (upropblock_stats) ... "<<endl;		
	#endif 
	unsigned int vdram_BASEOFFSETKVS_ACTIVEUPROPBLOCKS = vdram[BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_BASEOFFSETKVS_ACTIVEUPROPBLOCKS].data[0].key;
	unsigned int kvdram_BASEOFFSETKVS_ACTIVEUPROPBLOCKS = kvbuffer[0][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_BASEOFFSETKVS_ACTIVEUPROPBLOCKS].data[0].key;
	for(unsigned int v_p=0; v_p<myuniversalparams.NUMPROCESSEDGESPARTITIONS; v_p++){ 
		for(unsigned int iter=0; iter<MAXNUMGRAPHITERATIONS; iter++){
			unsigned int index = 0, active_index = 0;
			unsigned int offset = ((v_p * MAXNUMGRAPHITERATIONS * MAXNUM_UPROPBLOCKS_PER_VPARTITION) + (iter * MAXNUM_UPROPBLOCKS_PER_VPARTITION)) / VECTOR2_SIZE;
			tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + v_p].data[iter] = 0; 
			for(unsigned int t=0; t<NUM_UPROPBLOCKS_PER_VPARTITION; t++){ // (myuniversalparams.KVDATA_RANGE / NUM_VERTICES_PER_UPROPBLOCK), NUM_UPROPBLOCKS_PER_VPARTITION
				if(upropblock_stats[iter][v_p][t] == 1){ // > 0
					if(index < NUM_UPROPBLOCKS_PER_VPARTITION){
						
						#ifdef XXXXXXXXXXXXXXXXXXx
						unsigned int block_id = (v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t;
						tempvdram[vdram_BASEOFFSETKVS_ACTIVEUPROPBLOCKS + offset + (index / VECTOR2_SIZE)].data[index % VECTOR2_SIZE] = block_id; 
						for(unsigned int i=0; i<NUM_PEs; i++){ 
							// if((v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t > NUM_UPROPBLOCKS_PER_VPARTITION){ cout<<"acts_helper.cpp. [(v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t](="<<(v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t<<") > NUM_UPROPBLOCKS_PER_VPARTITION("<<NUM_UPROPBLOCKS_PER_VPARTITION<<"): "<<((v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t > NUM_UPROPBLOCKS_PER_VPARTITION)<<". EXITING..."<<endl; exit(EXIT_FAILURE); }				
							// if((v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t == 1920){ cout<<"acts_helper.cpp. [(v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t]=1920: index: "<<((v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t)<<", v_p: "<<v_p<<", t: "<<t<<". EXITING..."<<endl; } // exit(EXIT_FAILURE);	
							tempkvdram[i][kvdram_BASEOFFSETKVS_ACTIVEUPROPBLOCKS + offset + (index / VECTOR2_SIZE)].data[index % VECTOR2_SIZE] = block_id; 
						}
						// if((v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t > 1294967294){ cout<<"acts_helper.cpp. [(v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t] > 1294967294: index: "<<((v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t)<<", v_p: "<<v_p<<", t: "<<t<<". EXITING..."<<endl; exit(EXIT_FAILURE); } // exit(EXIT_FAILURE);	
						// if(block_id == 9 && v_p == 1){ cout<<"acts_helper.cpp. [block_id == 9 && v_p == 1], v_p: "<<v_p<<", t: "<<t<<". EXITING..."<<endl; exit(EXIT_FAILURE); } // exit(EXIT_FAILURE);	
						
						// if(iter==14){ cout<<"acts_helper.cpp --------------------> t: "<<t<<endl; }
						/* tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + v_p].data[iter] += 1;
						active_index += 1; */
						#endif 
						
						
						unsigned int block_id = (v_p * NUM_UPROPBLOCKS_PER_VPARTITION) + t;
						// if(iter < 2){ cout<<"################ acts_helper: iter: "<<iter<<", vertex_block_id: "<<block_id<<endl; }
						tempvdram[vdram_BASEOFFSETKVS_ACTIVEUPROPBLOCKS + offset + (index / VECTOR2_SIZE)].data[index % VECTOR2_SIZE] = block_id; 
						for(unsigned int i=0; i<NUM_PEs; i++){ 
							tempkvdram[i][kvdram_BASEOFFSETKVS_ACTIVEUPROPBLOCKS + offset + (index / VECTOR2_SIZE)].data[index % VECTOR2_SIZE] = block_id; 
						}
						
						#ifdef _DEBUGMODE_CHECKS3
						utilityobj->checkoutofbounds("acts_helper:: ERROR 21d", tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + v_p].data[iter], NUM_UPROPBLOCKS_PER_VPARTITION+1, v_p, NAp, NAp);
						#endif
					}
					tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + v_p].data[iter] += 1;
					active_index += 1;
					index += 1;
				}
			}
			// cout<<">>>>>>>>>>>>>>>>>>>> iter: "<<iter<<", v_p: "<<v_p<<endl;
			// if(iter==9){ cout<<"tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + "<<v_p<<"].data["<<iter<<"]: "<<tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + v_p].data[iter]<<endl; }
		}		
	}
	
	#ifdef _DEBUGMODE_HOSTPRINTS//4 // debug - print uprop block stats
	for(unsigned int iter=0; iter<GraphIter+1; iter++){
		cout<<"acts_helper: printing upropblock_stats for iteration "<<iter<<endl;
		for(unsigned int v_p=0; v_p<myuniversalparams.NUMPROCESSEDGESPARTITIONS; v_p++){ 
			cout<<"acts_helper: printing upropblock_stats for iteration "<<iter<<", v_partition: "<<v_p<<endl;
			for(unsigned int t=0; t<MAXNUM_EDGEBLOCKS_PER_VPARTITION; t++){ 
				if(upropblock_stats[iter][v_p][t] > 0){ cout<<t<<", "; }
				utilityobj->checkoutofbounds("acts_helper:: ERROR 25", upropblock_stats[iter][v_p][t], 2, NAp, NAp, NAp);
			}
			cout<<endl;
		} 
	}
	#endif
	#ifdef _DEBUGMODE_HOSTPRINTS4
	for(unsigned int iter=0; iter<GraphIter+1; iter++){
		unsigned int tot = 0, tot2 = 0, tot3 = 0;
		unsigned int total_num_actv_partitions = 0; for(unsigned int t=0; t<myuniversalparams.NUMPROCESSEDGESPARTITIONS; t++){ if(vpartition_stats[iter][t].A > 0){ total_num_actv_partitions += 1; }}
		for(unsigned int t=0; t<myuniversalparams.NUMPROCESSEDGESPARTITIONS; t++){ tot2 += iteration_stats[iter][t].A; tot += vpartition_stats[iter][t].A; tot3 += tempvdram[vdram_BASEOFFSETKVS_VERTICESPARTITIONMASK + t].data[iter]; }
		// cout<<"acts_helper: number of active vertices for iteration "<<iter<<": "<<tot2<<", (UB:"<<tot<<"), (EB:"<<tot3<<") (E:"<<(unsigned int)edgesprocessed_totals[iter]<<")"<<endl;
		cout<<"acts_helper: number of active vertices for iteration "<<iter<<": "<<tot2<<", (UP: "<<total_num_actv_partitions<<"/"<<myuniversalparams.NUMPROCESSEDGESPARTITIONS<<"), (UB:"<<tot<<"), (E:"<<(unsigned int)edgesprocessed_totals[iter]<<")"<<endl;
	}
	#endif 
	#ifdef _DEBUGMODE_HOSTPRINTS
	cout<<">>> acts_helper: FINISHED. "<<GraphIter+1<<" iterations required."<<endl;
	#endif 
	#ifdef _DEBUGMODE_HOSTPRINTS3
	cout<<">>> acts_helper: printing pmasks for process-partition-reduce... "<<endl;
	for(unsigned int iter=0; iter<GraphIter+1; iter++){
		unsigned int num_actvps = 0;
		unsigned int total_num_actvvs = 0;	
		for(unsigned int t=0; t<myuniversalparams.NUMPROCESSEDGESPARTITIONS; t++){
			if(vpartition_stats[iter][t].A > 0  && t < 16){ cout<<t<<", "; }
			if(vpartition_stats[iter][t].A > 0){ num_actvps += 1; }
			total_num_actvvs += vpartition_stats[iter][t].A;
		}
		cout<<" (num active partitions: "<<num_actvps<<", total num partitions: "<<myuniversalparams.NUMPROCESSEDGESPARTITIONS<<" iter: "<<iter<<")"<<endl;	
		cout<<"+++ total number of active vertices in all HBM channels (varA): "<<total_num_actvvs<<" ("<<num_actvps<<" active partitions +++"<<endl;
	}
	
	for(unsigned int iter=0; iter<GraphIter+1; iter++){
		unsigned int num_actv_edges = 0;
		for(unsigned int t=0; t<myuniversalparams.NUMPROCESSEDGESPARTITIONS; t++){
			// cout<<"^^****************^ iter "<<iter<<": t: "<<t<<" ^^^"<<endl;
			num_actv_edges += vpartition_stats[iter][t].B;
		}
		cout<<"^^^ iter "<<iter<<": total number of active edges in all HBM channel (varB): "<<num_actv_edges<<" ^^^"<<endl;
	}
	
	for(unsigned int iter=0; iter<GraphIter+1; iter++){
		unsigned int tot = 0, tot2 = 0;
		for(unsigned int t=0; t<myuniversalparams.NUMPROCESSEDGESPARTITIONS; t++){ tot += vpartition_stats[iter][t].A; tot2 += iteration_stats[iter][t].A; }
		cout<<"--- acts_helper: number of active vertices for iteration "<<iter<<": "<<tot2<<", "<<tot<<endl;
	}
	// exit(EXIT_SUCCESS);
	#endif 
	// exit(EXIT_SUCCESS);
	return GraphIter+1;
}

float acts_helper::get_results(string message, string graphpath, uint512_vec_dt * vdram, uint512_vec_dt * vdramtemp0, uint512_vec_dt * vdramtemp1, uint512_vec_dt * vdramtemp2, uint512_vec_dt * kvbuffer[NUM_PEs], universalparams_t universalparams){
	#ifdef _DEBUGMODE_HOSTPRINTS3
	cout<<endl<<"acts_helper::get_results: getting feedback... "<<endl;
	#endif
	
	unsigned int F0 = 0;
	unsigned int F1 = 1;
	unsigned int F2 = 2;
	float total__latency_ms = 0;
	
	unsigned int num_iters_toprint = universalparams.NUM_ITERATIONS; // MAXNUMGRAPHITERATIONS;
	if(universalparams.ALGORITHM != BFS && universalparams.ALGORITHM != SSSP){ num_iters_toprint = 1; }
	
	uint512_ivec_dt * tempkvbuffer[NUM_PEs]; for(unsigned int i=0; i<NUM_PEs; i++){ tempkvbuffer[i] = (uint512_ivec_dt *)kvbuffer[i]; }
	
	#ifdef _DEBUGMODE_HOSTPRINTS4
	for(unsigned int GraphIter=0; GraphIter<num_iters_toprint; GraphIter++){ 
		for(unsigned int i=0; i<1; i++){
			unsigned int PROCESSINGPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[PROCESSINGPHASE_TRANSFSZ_COLLECTIONID];	
			unsigned int PARTITIONINGPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[PARTITIONINGPHASE_TRANSFSZ_COLLECTIONID];
			unsigned int REDUCEPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[REDUCEPHASE_TRANSFSZ_COLLECTIONID];
			unsigned int BROADCASTPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[0][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[BROADCASTPHASE_TRANSFSZ_COLLECTIONID];
			unsigned int SYNCPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[0][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[SYNCPHASE_TRANSFSZ_COLLECTIONID];
			SYNCPHASE_TRANSFSZ_COLLECTIONID__ += tempkvbuffer[NUMCOMPUTEUNITS_SLR2][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[SYNCPHASE_TRANSFSZ_COLLECTIONID];
			SYNCPHASE_TRANSFSZ_COLLECTIONID__ += tempkvbuffer[NUMCOMPUTEUNITS_SLR2 + NUMCOMPUTEUNITS_SLR1][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[SYNCPHASE_TRANSFSZ_COLLECTIONID];
				
			cout<<"[PE:"<<i<<"][Iter: "<<GraphIter<<"]:: processing phase transfsz: "<<PROCESSINGPHASE_TRANSFSZ_COLLECTIONID__<<", reduce phase transfsz: "<<REDUCEPHASE_TRANSFSZ_COLLECTIONID__<<", broadcast phase transfsz: "<<BROADCASTPHASE_TRANSFSZ_COLLECTIONID__<<", sync phase transfsz: "<<SYNCPHASE_TRANSFSZ_COLLECTIONID__<<endl;				
		}
	}
	#endif 
	
	#ifdef _DEBUGMODE_HOSTPRINTS4
	for(unsigned int GraphIter=0; GraphIter<num_iters_toprint; GraphIter++){ 
		for(unsigned int i=0; i<1; i++){
			unsigned int NUMEDGESPROCESSED_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[NUMEDGESPROCESSED_COLLECTIONID];	
			unsigned int NUMVERTICESPROCESSED_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[NUMVERTICESPROCESSED_COLLECTIONID];
			
			cout<<"[PE:"<<i<<"][Iter: "<<GraphIter<<"]:: num edges processed: "<<NUMEDGESPROCESSED_COLLECTIONID__<<", num vertices processed: "<<NUMVERTICESPROCESSED_COLLECTIONID__<<"."<<" "<<endl;				
		}
	}
	#endif 
	
	#ifdef _DEBUGMODE_HOSTPRINTS4
	for(unsigned int GraphIter=0; GraphIter<num_iters_toprint; GraphIter++){ 
		for(unsigned int i=0; i<1; i++){
			unsigned int NUMREADSFROMDRAM_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[NUMREADSFROMDRAM_COLLECTIONID];	
			unsigned int NUMWRITESTODRAM_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[NUMWRITESTODRAM_COLLECTIONID];
			
			cout<<"[PE:"<<i<<"][Iter: "<<GraphIter<<"]:: num reads from dram: "<<NUMREADSFROMDRAM_COLLECTIONID__<<", num writes to dram: "<<NUMWRITESTODRAM_COLLECTIONID__<<"."<<" "<<endl;				
		}
	}
	#endif 
	
	#ifdef _DEBUGMODE_HOSTPRINTS4
	for(unsigned int GraphIter=0; GraphIter<num_iters_toprint; GraphIter++){ 
		for(unsigned int i=0; i<1; i++){
			unsigned int PROCESSINGPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[PROCESSINGPHASE_TRANSFSZ_COLLECTIONID];	
			unsigned int PARTITIONINGPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[PARTITIONINGPHASE_TRANSFSZ_COLLECTIONID];
			unsigned int REDUCEPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[i][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[REDUCEPHASE_TRANSFSZ_COLLECTIONID];
			unsigned int BROADCASTPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[0][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[BROADCASTPHASE_TRANSFSZ_COLLECTIONID];
			unsigned int SYNCPHASE_TRANSFSZ_COLLECTIONID__ = tempkvbuffer[0][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[SYNCPHASE_TRANSFSZ_COLLECTIONID];
			SYNCPHASE_TRANSFSZ_COLLECTIONID__ += tempkvbuffer[NUMCOMPUTEUNITS_SLR2][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[SYNCPHASE_TRANSFSZ_COLLECTIONID];
			SYNCPHASE_TRANSFSZ_COLLECTIONID__ += tempkvbuffer[NUMCOMPUTEUNITS_SLR2 + NUMCOMPUTEUNITS_SLR1][BASEOFFSET_MESSAGESDATA_KVS + MESSAGES_RETURNVALUES + MESSAGES_RETURNVALUES_STATSCOLLECTED + GraphIter].data[SYNCPHASE_TRANSFSZ_COLLECTIONID];

			float PROCESSINGPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS = (float)(PROCESSINGPHASE_TRANSFSZ_COLLECTIONID__ / 1000) / (float)ACTS_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC;
			float PARTITIONINGPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS = (float)(PARTITIONINGPHASE_TRANSFSZ_COLLECTIONID__ / 1000) / (float)ACTS_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC;
			float REDUCEPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS = (float)(REDUCEPHASE_TRANSFSZ_COLLECTIONID__ / 1000) / (float)ACTS_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC;
			float BROADCASTPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS = (float)(BROADCASTPHASE_TRANSFSZ_COLLECTIONID__ / 1000) / (float)ACTS_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC;
			float SYNCPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS = (float)(SYNCPHASE_TRANSFSZ_COLLECTIONID__ / 1000) / (float)ACTS_AVERAGE_MEMACCESSTHROUGHPUT_SINGLEHBMCHANNEL_MILIONEDGESPERSEC;
			float total__iteration_latency_ms = PROCESSINGPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS + PARTITIONINGPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS + REDUCEPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS + BROADCASTPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS + SYNCPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS;
			total__latency_ms += total__iteration_latency_ms;

			cout<<"[PE:"<<i<<"][Iter: "<<GraphIter<<"][latencies]:: total latency (ms): "<<total__iteration_latency_ms<<"ms [processing phase: "<<PROCESSINGPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS<<"ms, reduce phase: "<<REDUCEPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS<<"ms, broadcast phase: "<<BROADCASTPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS<<"ms, sync phase: "<<SYNCPHASE_TRANSFSZ_COLLECTIONID__LATENCY_MS<<"ms]"<<endl;				
		}
	}
	#endif
	
	// predict acts performance 
	#ifdef _DEBUGMODE_HOSTPRINTS4
	unsigned int total_edges_processed = universalparams.NUM_EDGES;
	cout<<endl<<">>> acts_helper::get_results::acts: total_edges_processed: "<<total_edges_processed<<" edges ("<<total_edges_processed/1000000<<" million edges)"<<endl;
	cout<< TIMINGRESULTSCOLOR <<">>> acts_helper::get_results::acts: total_time_elapsed: "<<total__latency_ms<<" ms ("<<total__latency_ms/1000<<" s)"<< RESET <<endl;
	cout<< TIMINGRESULTSCOLOR <<">>> acts_helper::get_results::acts: throughput: "<<((total_edges_processed / total__latency_ms) * (1000))<<" Edges / sec, "<<((total_edges_processed / total__latency_ms) / (1000))<<" Million edges / sec, "<<((total_edges_processed / total__latency_ms) / (1000000))<<" Billion edges / sec"<< RESET <<endl;	
	cout<< TIMINGRESULTSCOLOR <<">>> acts_helper::get_results::acts: throughput projection for 32 workers: "<<((((total_edges_processed / total__latency_ms) / (1000)) * 32) / NUM_PEs)<<" Million edges / sec"<< RESET <<endl;
	#endif 
	
	// predict graphlily's performance 
	#ifdef _DEBUGMODE_HOSTPRINTS4
	unsigned int out_buf_len = 1024000;
	unsigned int vec_buf_len = 30720;
	
	// model params (soc-orkut example)
	unsigned int num_vertices___ = 3071442;
	unsigned int num_edges___ = 234372166;
	unsigned int num_col_partitions___ = (num_vertices___ + (vec_buf_len - 1)) / vec_buf_len;
	unsigned int num_row_partitions___ = (num_vertices___ + (out_buf_len - 1)) / out_buf_len;
	unsigned long num_processed_overall_srcvs_int32___ = num_vertices___ * num_row_partitions___ * 2; 
	unsigned long num_processed_overall_edges_int32___ = (num_edges___ / num_row_partitions___) * num_row_partitions___ * 2; 
	unsigned long num_processed_overall_dstvs_int32___ = out_buf_len * num_row_partitions___ * 2; 
	unsigned long num_processed_overall_int32___ = num_processed_overall_srcvs_int32___ + num_processed_overall_edges_int32___ + num_processed_overall_dstvs_int32___;
	double total__iteration_latency_ms___ = 36;
	
	// dataset params
	unsigned int num_col_partitions = (universalparams.NUM_VERTICES + (vec_buf_len - 1)) / vec_buf_len;
	unsigned int num_row_partitions = (universalparams.NUM_VERTICES + (out_buf_len - 1)) / out_buf_len;
	unsigned long num_processed_overall_srcvs_int32 = universalparams.NUM_VERTICES * num_row_partitions * 2; // source vertices
	unsigned long num_processed_overall_edges_int32 = (universalparams.NUM_EDGES / num_row_partitions) * num_row_partitions * 2; // edges ('* 2' because each edge is a tuple of srcvid and dstvid)
	unsigned long num_processed_overall_dstvs_int32 = out_buf_len * num_row_partitions * 2; // dest vertices ('* 2' because they are read and written)
	unsigned long num_processed_overall_int32 = num_processed_overall_srcvs_int32 + num_processed_overall_edges_int32 + num_processed_overall_dstvs_int32;
	
	double total__iteration_latency_ms = (num_processed_overall_int32 * total__iteration_latency_ms___) / num_processed_overall_int32___;
	
	#ifdef _DEBUGMODE_HOSTPRINTS4
	cout<<"acts_helper:: num_vertices___: "<<num_vertices___<<endl;
	cout<<"acts_helper:: num_edges___: "<<num_edges___<<endl;
	cout<<"acts_helper:: num_col_partitions___: "<<num_col_partitions___<<endl;
	cout<<"acts_helper:: num_row_partitions___: "<<num_row_partitions___<<endl;
	cout<<"acts_helper:: num_processed_overall_srcvs_int32___: "<<num_processed_overall_srcvs_int32___<<endl;
	cout<<"acts_helper:: num_processed_overall_edges_int32___: "<<num_processed_overall_edges_int32___<<endl;
	cout<<"acts_helper:: num_processed_overall_dstvs_int32___: "<<num_processed_overall_dstvs_int32___<<endl;
	cout<<"acts_helper:: num_processed_overall_int32___: "<<num_processed_overall_int32___<<endl;
	cout<<"acts_helper:: total__iteration_latency_ms___: "<<total__iteration_latency_ms___<<endl;
	
	cout<<"acts_helper:: universalparams.NUM_VERTICES: "<<universalparams.NUM_VERTICES<<endl;
	cout<<"acts_helper:: universalparams.NUM_EDGES: "<<universalparams.NUM_EDGES<<endl;
	cout<<"acts_helper:: num_col_partitions: "<<num_col_partitions<<endl;
	cout<<"acts_helper:: num_row_partitions: "<<num_row_partitions<<endl;
	cout<<"acts_helper:: num_processed_overall_srcvs_int32: "<<num_processed_overall_srcvs_int32<<endl;
	cout<<"acts_helper:: num_processed_overall_edges_int32: "<<num_processed_overall_edges_int32<<endl;
	cout<<"acts_helper:: num_processed_overall_dstvs_int32: "<<num_processed_overall_dstvs_int32<<endl;
	cout<<"acts_helper:: num_processed_overall_int32: "<<num_processed_overall_int32<<endl;
	cout<<"acts_helper:: total__iteration_latency_ms: "<<total__iteration_latency_ms<<endl;
	#endif 
	
	if(universalparams.ALGORITHM == PAGERANK){
		total__latency_ms = total__iteration_latency_ms;
	} else if(universalparams.ALGORITHM == SPMV){
		total__latency_ms = total__iteration_latency_ms;
	} else if(universalparams.ALGORITHM == BFS){
		total__latency_ms = total__iteration_latency_ms * universalparams.NUM_ITERATIONS * (4286 / 3581);
	} else if(universalparams.ALGORITHM == SSSP){
		total__latency_ms = total__iteration_latency_ms * universalparams.NUM_ITERATIONS * (30 / 20);
	} 
	
	cout<< TIMINGRESULTSCOLOR <<">>> acts_helper::get_results::graphlily: total_time_elapsed: "<<total__latency_ms<<" ms ("<<total__latency_ms/1000<<" s)"<< RESET <<endl;
	#endif 
	
	return total__latency_ms;
}

void acts_helper::verifyresults(uint512_vec_dt * vbuffer, globalparams_t globalparams, universalparams_t universalparams){
	#ifdef _DEBUGMODE_HOSTPRINTS3
	cout<<endl<<"acts_helper::verifyresults: verifying results... "<<endl;
	#endif
	
	if(universalparams.ALGORITHM == BFS || universalparams.ALGORITHM == SSSP){} else { return; }
	
	unsigned int vdatas[64]; for(unsigned int k=0; k<64; k++){ vdatas[k] = 0; } 
	for(unsigned int i=0; i<NUM_PEs; i++){
		for(unsigned int partition=0; partition<universalparams.NUMREDUCEPARTITIONS; partition++){
			
			unsigned int reducepsz_kvs = universalparams.REDUCEPARTITIONSZ_KVS2; 
			if(universalparams.NUMREDUCEPARTITIONS > 1){ if(partition == universalparams.NUMREDUCEPARTITIONS-1){ reducepsz_kvs = (universalparams.BATCH_RANGE / VECTOR2_SIZE) % universalparams.REDUCEPARTITIONSZ_KVS2; } else { reducepsz_kvs = universalparams.REDUCEPARTITIONSZ_KVS2; }}
			
			for(unsigned int k=0; k<reducepsz_kvs; k++){
				for(unsigned int v=0; v<VECTOR_SIZE; v++){
					unsigned int combo1 = vbuffer[globalparams.BASEOFFSETKVS_SRCVERTICESDATA + (i * universalparams.NUMREDUCEPARTITIONS * universalparams.REDUCEPARTITIONSZ_KVS2) + (partition * universalparams.REDUCEPARTITIONSZ_KVS2) + k].data[v].key;
					unsigned int combo2 = vbuffer[globalparams.BASEOFFSETKVS_SRCVERTICESDATA + (i * universalparams.NUMREDUCEPARTITIONS * universalparams.REDUCEPARTITIONSZ_KVS2) + (partition * universalparams.REDUCEPARTITIONSZ_KVS2) + k].data[v].value;
					
					#ifdef CONFIG_PRELOADEDVERTEXMASKS
					value_t vdata1 = combo1 >> 0xFFFF; value_t mask1 = combo1 & 0xFFFF; 
					value_t vdata2 = combo2 >> 0xFFFF; value_t mask2 = combo2 & 0xFFFF; 
					#else 
					value_t vdata1 = combo1 >> 1; value_t mask1 = combo1 & 0x1; 
					value_t vdata2 = combo2 >> 1; value_t mask2 = combo2 & 0x1; 	
					#endif 
	
					unsigned int lvid1 = ((partition * universalparams.REDUCEPARTITIONSZ_KVS2 * VECTOR_SIZE) + (k * VECTOR_SIZE)) + 2*v;
					unsigned int lvid2 = lvid1 + 1;
					unsigned int vid1 = utilityobj->UTIL_GETREALVID(lvid1, i);
					unsigned int vid2 = utilityobj->UTIL_GETREALVID(lvid2, i);
					
					if(vdata1 < 64){
						#ifdef _DEBUGMODE_HOSTPRINTS
						cout<<"acts_helper::verifyresults: vid1: "<<vid1<<", vdata1: "<<vdata1<<", i: "<<i<<", partition: "<<partition<<", k: "<<k<<", v: "<<v<<endl;
						#endif
						vdatas[vdata1] += 1; 
					}
					if(vdata2 < 64){
						#ifdef _DEBUGMODE_HOSTPRINTS
						cout<<"acts_helper::verifyresults: vid2: "<<vid2<<", vdata2: "<<vdata2<<", i: "<<i<<", partition: "<<partition<<", k: "<<k<<", v: "<<v<<endl;
						#endif
						vdatas[vdata2] += 1; 
					}
				}
			}
		}
	}
	#ifdef _DEBUGMODE_HOSTPRINTS4
	utilityobj->printvalues("acts_helper::verifyresults: results after kernel run", vdatas, 16);
	#endif 
	return;
}

void acts_helper::verifyresults2(uint512_vec_dt * vbuffer, globalparams_t globalparams, universalparams_t universalparams){
	#ifdef _DEBUGMODE_HOSTPRINTS3
	cout<<endl<<"acts_helper::verifyresults: verifying results... "<<endl;
	#endif
	
	if(universalparams.ALGORITHM == BFS || universalparams.ALGORITHM == SSSP){} else { return; }
	
	unsigned int * vbufferINT32 = (unsigned int *)&vbuffer[globalparams.BASEOFFSETKVS_SRCVERTICESDATA];
	unsigned int vdatas[64]; for(unsigned int k=0; k<64; k++){ vdatas[k] = 0; } 
	for(unsigned int t=0; t<globalparams.SIZE_SRCVERTICESDATA; t++){
		unsigned int vid = vbufferINT32[t];
		
		if(vid < 64){
			#ifdef _DEBUGMODE_HOSTPRINTS
			cout<<"acts_helper::verifyresults: vid: "<<vid<<endl;
			#endif
			vdatas[vid] += 1; 
		}
	}
	#ifdef _DEBUGMODE_HOSTPRINTS4
	utilityobj->printvalues("acts_helper::verifyresults: results after kernel run", vdatas, 16);
	#endif 
	return;
}

void acts_helper::verifyresults3(uint512_vec_dt * vbuffer, globalparams_t globalparams, universalparams_t universalparams){
	#ifdef _DEBUGMODE_HOSTPRINTS3
	cout<<endl<<"acts_helper::verifyresults: verifying results... "<<endl;
	#endif
	
	if(universalparams.ALGORITHM == BFS || universalparams.ALGORITHM == SSSP){} else { return; }
	
	unsigned int * vbufferINT32 = (unsigned int *)&vbuffer[globalparams.BASEOFFSETKVS_SRCVERTICESDATA];
	#ifdef _DEBUGMODE_HOSTPRINTS4
	utilityobj->printvalues("acts_helper::verifyresults: results after kernel run", vbufferINT32, 16);
	#endif 
	return;
}















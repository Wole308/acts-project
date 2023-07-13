#!/bin/bash
#!/bin/bash

# srun -w lynx07 -p gpu --reservation=YOURRESEVATION --gres=gpu:4 --pty bash -i -l -
# srun -w cheetah04 -p gpu --reservation=oj2zf_71 --gres=gpu:4 --pty bash -i -l -
# srun -w jaguar01 -p gpu --reservation=bg9qq_72 --gres=gpu:4 --pty bash -i -l -
# source /etc/profile.d/modules.sh
# module load cmake-3.24.0
# module load cuda-toolkit-11.7.0
# /u/bg9qq/gunrock/build/./bin/pr /u/bg9qq/gunrock/datasets/it-2004/it-2004.mtx
# /u/bg9qq/gunrock/build/./bin/pr /u/bg9qq/datasets/flickr/flickr.mtx
# /u/bg9qq/gunrock/build/./bin/pr /u/bg9qq/datasets/flickr/flickr.mtx
# /u/bg9qq/gunrock/build/./bin/pr /u/bg9qq/datasets/rmat_4m_64m.mtx
# /u/bg9qq/gunrock/build/./bin/pr /u/bg9qq/datasets/flickr/flickr.mtx
# /u/bg9qq/gunrock/build/./bin/pr /u/bg9qq/datasets/rmat_8m_512m.mtx

# https://networkrepository.com/soc-sinaweibo.php

# make build TARGET=hw PLATFORM=/opt/xilinx/platforms/xilinx_u280_gen3x16_xdma_1_202211_1/xilinx_u280_gen3x16_xdma_1_202211_1.xpfm
# make build TARGET=hw PLATFORM=/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_3_202210_1/xilinx_u55c_gen3x16_xdma_3_202210_1.xpfm
# make build TARGET=hw PLATFORM=/opt/xilinx/platforms/xilinx_u280_xdma_201920_3/xilinx_u280_xdma_201920_3.xpfm

# make host PLATFORM=/opt/xilinx/platforms/xilinx_u55c_gen3x16_xdma_3_202210_1/xilinx_u55c_gen3x16_xdma_3_202210_1.xpfm
# make host PLATFORM=/opt/xilinx/platforms/xilinx_u280_xdma_201920_3/xilinx_u280_xdma_201920_3.xpfm
# ./overlap build_dir.hw.xilinx_u280_xdma_201920_3/vector_addition.xclbin
# ./overlap vector_addition.xclbin

# rmat_16_1024: 16778216 vertices, 1073742824 edges
# rmat_32_2048: 33555432 vertices, 2147484648 edges
# uk-2005: 39460925 vertices, 936365282 edges
# twitter: 41653230 vertices, 1468366182 edges
# sk-2005: 50637154 vertices, 1949413601 edges
# friendster: 65609366 vertices, 1806068135 edges

# "USAGE: ./host [--algo] [--num fpgas] [--rootvid] [--direction] [--numiterations] [--XCLBIN] [--graph_path]"
# ./evaluate_datasets.sh 0 1 1
# ./host pr 1 1 0 1 /home/oj2zf/Documents/acts-project-debug/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/kron_g500-logn20.mtx	
# ./host pr 1 1 0 1 /home/oj2zf/Documents/acts-project-debug/outputs/vector_addition_hbm.xclbin /home/oj2zf/Documents/dataset/rmat_16m_256m.mtx	
# ./host pr 8 1 0 1 /home/oj2zf/Documents/acts-project-debug/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/rmat_16m_1024m/rmat_16m_1024m.mtx
# ./host pr 8 1 0 1 /home/oj2zf/Documents/acts-project-debug/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/rmat_32m_2048m/rmat_32m_2048m.mtx
# ./host pr 2 1 0 1 /home/oj2zf/Documents/acts-project-debug/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/rmat_4m_64m.mtx
# ./host pr 8 1 0 1 /home/oj2zf/Documents/acts-project-debug/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/rmat_16m_1024m_unbalanced/rmat_16m_1024m.mtx
# ./host pr 8 1 0 20 /home/oj2zf/Documents/acts-project-debug/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/sk-2005/sk-2005.mtx
# ./host pr 8 1 0 20 /home/oj2zf/Documents/acts-project-debug/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/uk-2005/uk-2005.mtx
# exit

# ./burst_rw kernels_grasu/GraSU.xclbin /home/oj2zf/dataset/com-Orkut/com-Orkut.mtx result.out (48 MTEPS, 2428 ms)
# ./burst_rw kernels_grasu/GraSU.xclbin /home/oj2zf/dataset/soc-LiveJournal1/soc-LiveJournal1.mtx result.out (48 MTEPS, 1429 ms)
# ./burst_rw kernels_grasu/GraSU.xclbin /home/oj2zf/dataset/soc-Pokec/soc-Pokec.mtx result.out (48 MTEPS, 635 ms)
# ./burst_rw kernels_grasu/GraSU.xclbin /home/oj2zf/dataset/sx-stackoverflow/sx-stackoverflow.mtx result.out (48 MTEPS, 751 ms)
# ./burst_rw kernels_grasu/GraSU.xclbin /home/oj2zf/dataset/ljournal-2008/ljournal-2008.mtx result.out (48 MTEPS, 1636 ms)
# ./burst_rw kernels_grasu/GraSU.xclbin /home/oj2zf/dataset/kron_g500-logn20/kron_g500-logn20.mtx result.out (48 MTEPS, 925 ms)
# ./burst_rw kernels_grasu/GraSU.xclbin /home/oj2zf/dataset/wiki-Vote/wiki-Vote.mtx result.out (27 MTEPS, 3.7 ms)

# xilinx_u280_gen3x16_xdma_base_1

# Synthesize to generate xclbin...
# "USAGE: ./host [--xware] [--num_pes] [--running synthesis]"
# ./evaluate_datasets.sh 2 1 1
# ./evaluate_datasets.sh 2 12 1
# vi ~/.bashrc
# make build TARGET=hw PLATFORM=/opt/xilinx/platforms/xilinx_u280_gen3x16_xdma_1_202211_1/xilinx_u280_gen3x16_xdma_1_202211_1.xpfm

# make build TARGET=hw PLATFORM=/opt/xilinx/platforms/xilinx_u280_gen3x16_xdma_1_202211_1/xilinx_u280_gen3x16_xdma_1_202211_1.xpfm
# make host PLATFORM=/opt/xilinx/platforms/xilinx_u280_xdma_201920_3/xilinx_u280_xdma_201920_3.xpfm

# DATASET_BASEDIR="/home/oj2zf/dataset"
DATASET_BASEDIR="/home/oj2zf/Documents/dataset"
		
DATSETS=(
		# com-Orkut
		# soc-LiveJournal1
		# soc-Pokec
		# sx-stackoverflow
		# ljournal-2008
		# kron_g500-logn20 
		
		# uk-2002
		
		rmat_16_28 # 268
		rmat_16_29 # 512
		rmat_16_30 # 1024
		rmat_16_31 # 2048

		# kron_g500-logn20 
		# rmat_16m_256m 
		# it-2004 
		# GAP-twitter
		
		# indochina-2004 
		# twitter7 
		# uk-2005 
		# soc-sinaweibo
		# webbase-2001
		# rmat_8m_1024m
		# rmat_16m_1024m
		# rmat_32m_1024m
		)
		
NUM_FPGAS=(
		1
		2
		4
		8
		)
		
NUM_PES=(
		# 1 
		# 6
		12
		)
		
XCLBINS=(
		# "outputs/vector_addition_dynamic_x1.xclbin"
		"outputs/vector_addition_static_x1_tmp.xclbin"	
		# "outputs/vector_addition.xclbin"
		)
		
RUN_IN_ASYNC_MODE=(
		1	
		# 0
		)
		
GRAPH_IS_UNDIRECTED=0 #1
	
XWARE_ID=1 # 0, 1
MAX_NUM_ITERATIONS=16

# "USAGE: ./host [--algo] [--num fpgas] [--rootvid] [--direction] [--numiterations] [--graph_path] [--XCLBINS...] "
for ((c = 0; c < ${#NUM_FPGAS[@]}; c++)) do	
	for ((n = 0; n < ${#RUN_IN_ASYNC_MODE[@]}; n++)) do	
		for ((k = 0; k < ${#NUM_PES[@]}; k++)) do	
			./evaluate_datasets.sh $XWARE_ID ${NUM_PES[k]} ${RUN_IN_ASYNC_MODE[n]}
			
			for ((i = 0; i < ${#DATSETS[@]}; i++)) do
				echo pagerank algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[c]}, xclbin: ${XCLBINS[k]} num_iterations: $MAX_NUM_ITERATIONS
				./host pr ${NUM_FPGAS[c]} 1 $GRAPH_IS_UNDIRECTED $MAX_NUM_ITERATIONS ${DATASET_BASEDIR}/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} > results/graph-analytics-scaling/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.out			
				sleep 2
				cp -rf summary.csv results/results_pr/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.csv
				# exit
			done	
			
			# for ((i = 0; i < ${#DATSETS[@]}; i++)) do
				# echo pagerank algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[c]}, xclbin: ${XCLBINS[k]} num_iterations: $MAX_NUM_ITERATIONS
				# ./host spmv ${NUM_FPGAS[c]} 1 0 $MAX_NUM_ITERATIONS ${DATASET_BASEDIR}/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} #> results/results_spmv/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.out
				# sleep 2
				# cp -rf summary.csv results/results_spmv/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.csv
				# exit
			# done	
			
			# for ((i = 0; i < ${#DATSETS[@]}; i++)) do
				# echo hits algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[c]}, xclbin: ${XCLBINS[k]} num_iterations: $MAX_NUM_ITERATIONS
				# ./host hits ${NUM_FPGAS[c]} 1 0 $MAX_NUM_ITERATIONS ${DATASET_BASEDIR}/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} > results/results_hits/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.out
				# sleep 2
				# cp -rf summary.csv results/results_hits/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.csv
				# exit
			# done
			
			# for ((i = 0; i < ${#DATSETS[@]}; i++)) do
				# echo pagerank algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[c]}, xclbin: ${XCLBINS[k]} num_iterations: $MAX_NUM_ITERATIONS
				# ./host pr ${NUM_FPGAS[c]} 1 $GRAPH_IS_UNDIRECTED $MAX_NUM_ITERATIONS ${DATASET_BASEDIR}/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} > results/results_edgeupdates/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.out
				# sleep 2
				# cp -rf summary.csv results/results_pr/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.csv
				# exit
			# done
		done 
	done
done


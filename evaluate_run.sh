#!/bin/bash
#!/bin/bash

# rm -rf /home/oj2zf/Documents/acts-project/results_sw_pr/*
# rm -rf /home/oj2zf/Documents/acts-project/results_sw_sssp/*
# rm -rf /home/oj2zf/Documents/acts-project/results_hw_sync_pr/*
# rm -rf /home/oj2zf/Documents/acts-project/results_hw_sync_sssp/*
# rm -rf /home/oj2zf/Documents/acts-project/results_hw_async_pr/*
# rm -rf /home/oj2zf/Documents/acts-project/results_hw_async_sssp/*
# exit 

# "USAGE: ./host [--algo] [--num fpgas] [--rootvid] [--direction] [--numiterations] [--XCLBIN] [--graph_path]"
# ./evaluate_datasets.sh 0 1 1
# ./host pr 1 1 0 1 /home/oj2zf/Documents/acts-project/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/kron_g500-logn20.mtx	
# ./host pr 1 1 0 1 /home/oj2zf/Documents/acts-project/outputs/vector_addition_hbm.xclbin /home/oj2zf/Documents/dataset/rmat_16m_256m.mtx	
# ./host pr 8 1 0 1 /home/oj2zf/Documents/acts-project/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/rmat_16m_1024m/rmat_16m_1024m.mtx
# ./host pr 8 1 0 1 /home/oj2zf/Documents/acts-project/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/rmat_32m_2048m/rmat_32m_2048m.mtx
# ./host pr 2 1 0 1 /home/oj2zf/Documents/acts-project/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/rmat_4m_64m.mtx
# ./host pr 8 1 0 1 /home/oj2zf/Documents/acts-project/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/rmat_16m_1024m_unbalanced/rmat_16m_1024m.mtx
# ./host pr 8 1 0 20 /home/oj2zf/Documents/acts-project/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/sk-2005/sk-2005.mtx
# ./host pr 8 1 0 20 /home/oj2zf/Documents/acts-project/outputs/vector_addition.xclbin /home/oj2zf/Documents/dataset/uk-2005/uk-2005.mtx
# exit

# "USAGE: ./host [--xware] [--running synthesis] [--num_pes]"
# ./evaluate_datasets.sh 2 1 1
		
DATSETS=(
		kron_g500-logn20 
		# rmat_16m_256m 
		# twitter7 
		# rmat_16m_1024m
		# rmat_32m_2048m 
		# sk-2005 
		# uk-2005 
		# com-Friendster
		)
		
NUM_FPGAS=(
		1 
		2
		8
		8
		8
		8
		8 
		8
		)
		
NUM_PES=(
		1 
		# 12
		)
		
XCLBINS=(
		"/home/oj2zf/Documents/acts-project/outputs/vector_addition_ddr.xclbin"
		"/home/oj2zf/Documents/acts-project/outputs/vector_addition_hbm.xclbin"		
		)
		
RUN_IN_ASYNC_MODE=(
		1
		# 0
		)
	
XWARE_ID=1 # 0, 1
MAX_NUM_ITERATIONS=1

# "USAGE: ./host [--algo] [--num fpgas] [--rootvid] [--direction] [--numiterations] [--graph_path] [--XCLBINS...] "
for ((n = 0; n < ${#RUN_IN_ASYNC_MODE[@]}; n++)) do	
	for ((k = 0; k < ${#NUM_PES[@]}; k++)) do	
		./evaluate_datasets.sh $XWARE_ID ${NUM_PES[k]} ${RUN_IN_ASYNC_MODE[n]}
		
		for ((i = 0; i < ${#DATSETS[@]}; i++)) do
			echo pagerank algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[i]}, xclbin: ${XCLBINS[k]} num_iterations: 50
			./host pr ${NUM_FPGAS[i]} 1 0 $MAX_NUM_ITERATIONS /home/oj2zf/Documents/dataset/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} #> results/results_${XWARE_ID}_pr/${DATSETS[i]}_${RUN_IN_ASYNC_MODE}.out
			exit
		done

		# for ((i = 0; i < ${#DATSETS[@]}; i++)) do
			# echo bfs algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[i]}, xclbin: ${XCLBINS[k]} num_iterations: 50
			# ./host bfs ${NUM_FPGAS[i]} 1 0 $MAX_NUM_ITERATIONS /home/oj2zf/Documents/dataset/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} > results/results_${XWARE}_bfs/${DATSETS[i]}_${RUN_IN_ASYNC_MODE}.out
		# done

		# for ((i = 0; i < ${#DATSETS[@]}; i++)) do
			# echo sssp algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[i]}, xclbin: ${XCLBINS[k]} num_iterations: 50
			# ./host sssp ${NUM_FPGAS[i]} 1 0 $MAX_NUM_ITERATIONS /home/oj2zf/Documents/dataset/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} > results/results_${XWARE}_sssp/${DATSETS[i]}_${RUN_IN_ASYNC_MODE}.out
		# done	
	done
done


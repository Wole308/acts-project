#!/bin/bash
#!/bin/bash

# rm -rf /home/oj2zf/Documents/acts-project-debug/results_sw_pr/*
# rm -rf /home/oj2zf/Documents/acts-project-debug/results_sw_sssp/*
# rm -rf /home/oj2zf/Documents/acts-project-debug/results_hw_sync_pr/*
# rm -rf /home/oj2zf/Documents/acts-project-debug/results_hw_sync_sssp/*
# rm -rf /home/oj2zf/Documents/acts-project-debug/results_hw_async_pr/*
# rm -rf /home/oj2zf/Documents/acts-project-debug/results_hw_async_sssp/*
# exit 

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

# Synthesize to generate xclbin...
# "USAGE: ./host [--xware] [--num_pes] [--running synthesis]"
# ./evaluate_datasets.sh 2 1 1
# ./evaluate_datasets.sh 2 12 1
		
DATSETS=(
		kron_g500-logn20 
		# rmat_16m_256m 
		# uk-2002 
		# rmat_32m_256m
		# rmat_64m_256m
		# twitter7 
		# rmat_16m_1024m
		# rmat_32m_2048m 
		# sk-2005 
		# uk-2005 
		# com-Friendster
		)
		
NUM_FPGAS=(
		# 1
		2 
		# 4
		# 8
		)
		
NUM_PES=(
		# 1 
		12
		)
		
XCLBINS=(
		# "outputs/vector_addition.xclbin"
		# "outputs/vector_addition_ddr.xclbin"
		"outputs/vector_addition_hbm.xclbin"	
		# "outputs/vector_addition_12hbm.xclbin"			
		)
		
RUN_IN_ASYNC_MODE=(
		# 1
		0
		)
	
XWARE_ID=1 # 0, 1
MAX_NUM_ITERATIONS=1

# "USAGE: ./host [--algo] [--num fpgas] [--rootvid] [--direction] [--numiterations] [--graph_path] [--XCLBINS...] "
for ((c = 0; c < ${#NUM_FPGAS[@]}; c++)) do	
	for ((n = 0; n < ${#RUN_IN_ASYNC_MODE[@]}; n++)) do	
		for ((k = 0; k < ${#NUM_PES[@]}; k++)) do	
			./evaluate_datasets.sh $XWARE_ID ${NUM_PES[k]} ${RUN_IN_ASYNC_MODE[n]}
			
			for ((i = 0; i < ${#DATSETS[@]}; i++)) do
				echo pagerank algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[c]}, xclbin: ${XCLBINS[k]} num_iterations: $MAX_NUM_ITERATIONS
				./host pr ${NUM_FPGAS[c]} 1 0 $MAX_NUM_ITERATIONS /home/oj2zf/Documents/dataset/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} #> results/results_pr/${NUM_FPGAS[c]}_fpgas/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.out
				sleep 2
				# cp -rf summary.csv results/results_pr/${NUM_FPGAS[c]}_fpgas/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.csv
				# exit
			done

			# for ((i = 0; i < ${#DATSETS[@]}; i++)) do
				# echo bfs algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[c]}, xclbin: ${XCLBINS[k]} num_iterations: $MAX_NUM_ITERATIONS
				# ./host bfs ${NUM_FPGAS[c]} 1 0 $MAX_NUM_ITERATIONS /home/oj2zf/Documents/dataset/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} > results/results_bfs/${NUM_FPGAS[c]}_fpgas/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.out
				# sleep 2
				# cp -rf summary.csv results/results_bfs/${NUM_FPGAS[c]}_fpgas/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.csv
				# exit
			# done

			# for ((i = 0; i < ${#DATSETS[@]}; i++)) do
				# echo sssp algorithm running...: dataset: ${DATSETS[i]}, num fpgas: ${NUM_FPGAS[c]}, xclbin: ${XCLBINS[k]} num_iterations: $MAX_NUM_ITERATIONS
				# ./host sssp ${NUM_FPGAS[c]} 1 0 $MAX_NUM_ITERATIONS /home/oj2zf/Documents/dataset/${DATSETS[i]}/${DATSETS[i]}.mtx ${XCLBINS[k]} > results/results_sssp/${NUM_FPGAS[c]}_fpgas/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.out			
				# sleep 2
				# cp -rf summary.csv results/results_sssp/${NUM_FPGAS[c]}_fpgas/${DATSETS[i]}_fpgas${NUM_FPGAS[c]}_pes${NUM_PES[k]}_async${RUN_IN_ASYNC_MODE[n]}.csv
				# exit
			# done	
			# exit
		done 
	done
done


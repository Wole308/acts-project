# ACTS: A Near-Memory FPGA Graph Processing Framework

**ACTS** is a graph processing accelerator designed specifically for the FPGA. It uses a **high-level**, **bulk-synchronous/asynchronous**, **data-centric abstraction** focused on operations on vertex or edge frontiers. To tackle the challenge of random accesses when sending/receiving messages (i.e., vertex updates) between vertices, ACTS' decouples the generation of updates (at source vertices) from the applying these updates at their respective destination vertices. This allows the opportunity to restructure the BRAM locality of the vertex updates in real-time. To tackle the dependency hazards associated with neigboring edges accessing the same URAM on-chip, ACTS employs a novel edge-packing scheme (ACTPACK) that packs edges into edgeblocks and allow parallel URAM accesses during processing. To accomodate processing sparse frontiers, ACTS employs a hybrid scheme where sparse vertex intervals (i.e., consisting few active vertices) are processed with the CSR format, while dense vertex intervals are processed with ACTPACK.

## Quick Start Guide
Before building ACTS make sure you have **Jinja 2 Toolkit**[^2] installed on your system. Jinja 2 is a template generator that generates source code tailored to hardware-specific parameters (e.g., number of processing elements to be synthesized on the FPGA). 

```shell
git clone https://github.com/Wole308/acts-project.git
cd acts-project
mkdir build && cd build
cmake .. 
make sssp # or for all algorithms, use: make -j$(nproc)
bin/sssp ../datasets/chesapeake/chesapeake.mtx
```

## Sythesizing ACTS using Vitis HLS
To synthesize ACTS on the FPGA, run the ./evaluate_datasets.sh, specifying the number of PEs in the second parameter as shown
```cpp
"USAGE: ./host [--xware] [--num_pes] [--running synthesis]"
./evaluate_datasets.sh 2 1 1
```

## Running ACTS
Run ACTS using the commands shown below. More examples shown in ./evaluate_run.sh
```cpp
"USAGE: ./host [--xware] [--num_pes] [--run-in-async-mode?]"
./evaluate_datasets.sh 1 12 1

"USAGE: ./host [--algo] [--num fpgas] [--rootvid] [--direction] [--numiterations] [--graph_path] [--XCLBINS...] "
./host pr 8 1 0 50 path_to_graph path_to_xclbin
```		

## How to Cite ACTS & Essentials
Thank you for citing our work.

```bibtex
@inproceedings{inproceedings,
author = {Jaiyeoba, Wole and Elyasi, Nima and Choi, Changho and Skadron, Kevin},
year = {2023},
month = {02},
pages = {79-89},
title = {ACTS: A Near-Memory FPGA Graph Processing Framework},
doi = {10.1145/3543622.3573180}
}
```

## Copyright & License

Gunrock is copyright The Regents of the University of California. The library, examples, and all source code are released under [Apache 2.0](https://github.com/gunrock/gunrock/blob/main/LICENSE).

[^1]: This repository has been moved from https://github.com/gunrock/essentials and the previous history is preserved with tags and under `master` branch. Read more about gunrock and essentials in our vision paper: [Essentials of Parallel Graph Analytics](https://escholarship.org/content/qt2p19z28q/qt2p19z28q_noSplash_38a658bccc817ba025517311a776840f.pdf).
[^2]: Recommended **CUDA v11.5.1 or higher** due to support for stream ordered memory allocators.

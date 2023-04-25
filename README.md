# ACTS: A Near-Memory FPGA Graph Processing Framework

**ACTS** is a graph processing accelerator designed specifically for the FPGA. It uses a **high-level**, **bulk-synchronous/asynchronous**, **data-centric abstraction** focused on operations on vertex or edge frontiers. To tackle the random accesses problem when sending/receiving messages (i.e., vertex updates) between vertices, ACTS' decouples the generation of updates (at source vertices) from the applying these updates (at destination vertices), to restructure their URAM locality in real-time. To tackle the dependency hazards when multiple edges access the same URAM on-chip to read/write vertex properties, ACTS employs a novel edge-packing scheme (ACTPACK) that allow parallel accesses to unique URAMs across the entire processing flow.

## Quick Start Guide
Before building ACTS make sure you have **Jinja 2 Toolkit** installed on your system. Jinja 2 is a template generator that generates source code tailored to hardware-specific parameters (e.g., number of processing elements to be synthesized on the FPGA). 

```shell
git clone https://github.com/Wole308/acts-project.git
cd acts-project
```

## Sythesizing ACTS using Vitis HLS
To synthesize ACTS on the FPGA, run the ./evaluate_datasets.sh, specifying the number of PEs in the second parameter as shown
```cpp
"USAGE: ./host [--xware] [--num_pes] [--running synthesis]"
./evaluate_datasets.sh 2 1 1
```

## Configure ACTS
Configure ACTS environment using the commands shown below. 

Here you specify the (1) platform to run acts: 0 for cpu and 1 for fpga, (2) number of processing elements per FPGA, which should equal the number of processing elements synthesized unto the FPGA as shown above (3) whether ACTS should run in synchronous or asynchronous mode. More examples shown in ./evaluate_run.sh
```cpp
"USAGE: ./host [--xware] [--num_pes] [--run-in-async-mode?]"
./evaluate_datasets.sh 1 12 1
```		

## Running ACTS 
Run ACTS using the commands shown below. Here you specify the (1) algorithm, (2) number of FPGAs, (3) root vertex id (for traversal based algorithms like BFS, SSSP), (4) whether the graph is unidirectional of bidirectional, (5) the maximum number of iterations to run, (6) the path to graph dataset, and (7) the path to xclbin. More examples shown in ./evaluate_run.sh
```cpp
"USAGE: ./host [--algo] [--num fpgas] [--rootvid] [--graph-direction] [--number-of-iterations] [--path-to-graph] [--path-to-XCLBIN] "
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
ACTS is released under the [OpenBSD](https://github.com/Wole308/acts-project/blob/main/LICENSE) license


# ACTS: A Near-Memory FPGA Graph Processing Framework

**ACTS** is a graph processing accelerator designed specifically for the FPGA. It uses a **high-level**, **bulk-synchronous/asynchronous**, **data-centric abstraction** focused on operations on vertex or edge frontiers. To tackle the random accesses problem when sending/receiving messages (i.e., vertex updates) between vertices, ACTS' decouples the generation of updates (at source vertices) from the applying these updates (at destination vertices), to restructure their URAM locality in real-time using fast FPGA resources. To tackle the dependency hazards when multiple edges access the same URAM on-chip to read/write vertex properties, ACTS employs a novel edge-packing scheme (ACTPACK) that allow parallel accesses to unique URAMs across the entire processing flow. ACTS was published at the ACM/SIGDA International Symposium on Field-Programmable Gate Arrays (FPGA '23). Please see our paper at https://dl.acm.org/doi/abs/10.1145/3543622.3573180 

## Dependencies
Jinja 2 Toolkit
Vitis HLS
Xilinx FPGA

## Configuring ACTS
ACTS can be configured to specify a number of parameters such as (1) platform to run acts: 0 for cpu and 1 for fpga, (2) number of processing elements per FPGA, which should equal the number of processing elements synthesized unto the FPGA as shown above (3) whether ACTS should run in synchronous or asynchronous mode, etc.  

```cpp
"USAGE: ./host [--xware] [--num_pes] [--run-in-async-mode?]"
./evaluate_datasets.sh 1 12 1
```	

## Sythesizing ACTS (using Vitis HLS)
ACTS is synthesized with Vitis HLS for the Xilinx Alveo U280 FPGA. To synthesize ACTS, run the ./evaluate_synthesize.sh, specifying the number of PEs in the second parameter as shown. It would synthesize the required number of PEs for ACTS. When running ACTS across multiple FPGAS (as used in the Swift Project), the number of FPGAs need to be specified when running ACTS (see next section below)

```cpp
./evaluate_synthesize.sh
```

## Running ACTS on the FPGA 
To run ACTS using the commands shown below, use the command below

```cpp
"USAGE: ./host [--algo] [--num fpgas] [--root vertex ID] [--graph-direction] [--number-of-iterations] [--path-to-graph] [--path-to-XCLBIN] "
For example: ./host pr 8 1 0 50 path_to_graph path_to_xclbin
```

Here you specify the (1) algorithm, (2) number of FPGAs, (3) root vertex id (for traversal based algorithms like BFS, SSSP), (4) whether the graph is unidirectional of bidirectional, (5) the maximum number of iterations to run, (6) the path to graph dataset, and (7) the path to xclbin. Sample examples are shown in ./evaluate_run.sh

## How to Cite ACTS
Thank you for citing our work.

```bibtex
  @inproceedings{10.1145/3543622.3573180,
  author = {Jaiyeoba, Wole and Elyasi, Nima and Choi, Changho and Skadron, Kevin},
  title = {ACTS: A Near-Memory FPGA Graph Processing Framework},
  year = {2023},
  isbn = {9781450394178},
  publisher = {Association for Computing Machinery},
  address = {New York, NY, USA},
  url = {https://doi.org/10.1145/3543622.3573180},
  doi = {10.1145/3543622.3573180},
  booktitle = {Proceedings of the 2023 ACM/SIGDA International Symposium on Field Programmable Gate Arrays},
  pages = {79–89},
  numpages = {11},
  keywords = {fpga, graph analytics, high memory bandwidth (hbm)},
  location = {<conf-loc>, <city>Monterey</city>, <state>CA</state>, <country>USA</country>, </conf-loc>},
  series = {FPGA '23}
}
```

## Copyright & License
ACTS is released under the [OpenBSD](https://github.com/Wole308/acts-project/blob/main/LICENSE) license


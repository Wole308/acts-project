# ACTS: A Near-Memory FPGA Graph Processing Framework
[![Ubuntu](https://github.com/gunrock/gunrock/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/gunrock/gunrock/actions/workflows/ubuntu.yml) [![Windows](https://github.com/gunrock/gunrock/actions/workflows/windows.yml/badge.svg)](https://github.com/gunrock/gunrock/actions/workflows/windows.yml) [![Code Quality](https://github.com/gunrock/gunrock/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/gunrock/gunrock/actions/workflows/codeql-analysis.yml)

| [**Examples**](https://github.com/gunrock/gunrock/tree/main/examples/algorithms) | [**Project Template**](https://github.com/gunrock/template) | [**Documentation**](https://github.com/gunrock/gunrock/wiki) | [**GitHub Actions**](https://github.com/gunrock/gunrock/actions) |
|--------------|----------------------|-------------------|-------------------|

**ACTS**[^1] is a graph processing accelerator designed specifically for the FPGA. It uses a **high-level**, **bulk-synchronous/asynchronous**, **data-centric abstraction** focused on operations on vertex or edge frontiers. To tackle the challenge of random accesses when sending/receiving messages (i.e., vertex updates) between vertices, ACTS' decouples the generation of updates (at source vertices) from the applying these updates at their respective destination vertices. This allows the opportunity to restructure the BRAM locality of the vertex updates in real-time. To tackle the dependency hazards associated with neigboring edges accessing the same URAM on-chip, ACTS employs a novel edge-packing scheme (ACTPACK) that packs edges into edgeblocks and allow parallel URAM accesses during processing. To accomodate processing sparse frontiers, ACTS employs a hybrid scheme where sparse vertex intervals (i.e., consisting few active vertices) are processed with the CSR format, while dense vertex intervals are processed with ACTPACK.

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

## Implementing Graph Algorithms
"USAGE: ./host [--xware] [--num_pes] [--running synthesis]"
./evaluate_datasets.sh 2 1 1

## Implementing Graph Algorithms
For a detailed explanation, please see the full [documentation](https://github.com/gunrock/gunrock/wiki/How-to-write-a-new-graph-algorithm). The following example shows simple APIs using Gunrock's data-centric, bulk-synchronous programming model, we implement Breadth-First Search on GPUs. This example skips the setup phase of creating a `problem_t` and `enactor_t` struct and jumps straight into the actual algorithm.

We first prepare our frontier with the initial source vertex to begin
push-based BFS traversal. A simple `f->push_back(source)` places
the initial vertex we will use for our first iteration.
```cpp
void prepare_frontier(frontier_t* f,
                      gcuda::multi_context_t& context) override {
  auto P = this->get_problem();
  f->push_back(P->param.single_source);
}
```
We then begin our iterative loop, which iterates until a convergence condition has been met. If no condition has been specified, the loop converges when the frontier is empty.
```cpp
void loop(gcuda::multi_context_t& context) override {
  auto E = this->get_enactor();   // Pointer to enactor interface.
  auto P = this->get_problem();   // Pointer to problem (data) interface.
  auto G = P->get_graph();        // Graph that we are processing.

  auto single_source = P->param.single_source;  // Initial source node.
  auto distances = P->result.distances;         // Distances array for BFS.
  auto visited = P->visited.data().get();       // Visited map.
  auto iteration = this->iteration;             // Iteration we are on.

  // Following lambda expression is applied on every source,
  // neighbor, edge, weight tuple during the traversal.
  // Our intent here is to find and update the minimum distance when found.
  // And return which neighbor goes in the output frontier after traversal.
  auto search = [=] __host__ __device__(
                      vertex_t const& source,    // ... source
                      vertex_t const& neighbor,  // neighbor
                      edge_t const& edge,        // edge
                      weight_t const& weight     // weight (tuple).
                      ) -> bool {
    auto old_distance =
      math::atomic::min(&distances[neighbor], iteration + 1);
    return (iteration + 1 < old_distance);
  };

  // Execute advance operator on the search lambda expression.
  // Uses load_balance_t::block_mapped algorithm (try others for perf. tuning.)
  operators::advance::execute<operators::load_balance_t::block_mapped>(
    G, E, search, context);
}
```
[include/gunrock/algorithms/bfs.hxx](include/gunrock/algorithms/bfs.hxx)

## How to Cite Gunrock & Essentials
Thank you for citing our work.

```bibtex
@article{Wang:2017:GGG,
  author =	 {Yangzihao Wang and Yuechao Pan and Andrew Davidson
                  and Yuduo Wu and Carl Yang and Leyuan Wang and
                  Muhammad Osama and Chenshan Yuan and Weitang Liu and
                  Andy T. Riffel and John D. Owens},
  title =	 {{G}unrock: {GPU} Graph Analytics},
  journal =	 {ACM Transactions on Parallel Computing},
  year =	 2017,
  volume =	 4,
  number =	 1,
  month =	 aug,
  pages =	 {3:1--3:49},
  doi =		 {10.1145/3108140},
  ee =		 {http://arxiv.org/abs/1701.01170},
  acmauthorize = {https://dl.acm.org/doi/10.1145/3108140?cid=81100458295},
  url =		 {http://escholarship.org/uc/item/9gj6r1dj},
  code =	 {https://github.com/gunrock/gunrock},
  ucdcite =	 {a115},
}
```

```bibtex
@InProceedings{Osama:2022:EOP,
  author =	 {Muhammad Osama and Serban D. Porumbescu and John D. Owens},
  title =	 {Essentials of Parallel Graph Analytics},
  booktitle =	 {Proceedings of the Workshop on Graphs,
                  Architectures, Programming, and Learning},
  year =	 2022,
  series =	 {GrAPL 2022},
  month =	 may,
  pages =	 {314--317},
  doi =		 {10.1109/IPDPSW55747.2022.00061},
  url =          {https://escholarship.org/uc/item/2p19z28q},
}
```

## Copyright & License

Gunrock is copyright The Regents of the University of California. The library, examples, and all source code are released under [Apache 2.0](https://github.com/gunrock/gunrock/blob/main/LICENSE).

[^1]: This repository has been moved from https://github.com/gunrock/essentials and the previous history is preserved with tags and under `master` branch. Read more about gunrock and essentials in our vision paper: [Essentials of Parallel Graph Analytics](https://escholarship.org/content/qt2p19z28q/qt2p19z28q_noSplash_38a658bccc817ba025517311a776840f.pdf).
[^2]: Recommended **CUDA v11.5.1 or higher** due to support for stream ordered memory allocators.

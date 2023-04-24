#!/bin/bash
#!/bin/bash

./evaluate.sh
make cleanall
rm -rf acts_vitishls3
faketime -f "-1y" vitis_hls -f script.tcl





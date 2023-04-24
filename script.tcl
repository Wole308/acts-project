############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
## vivado_hls -f script.tcl
############################################################

# faketime -f "-1y" vitis_hls -f script.tcl

# bash ./evaluate.sh

open_project vhls_proj
set_top top_function

add_files acts_templates/acts_kernel.cpp

open_solution "solution1"
# set_part {xcvu9p-fsgd2104-3-e}
set_part {xcvu9p-fsgd2104-2-i}
create_clock -period 10 -name default
#source "./jp_hw/solution1/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -format ip_catalog

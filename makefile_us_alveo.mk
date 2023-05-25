#
# Copyright 2019-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# makefile-generator v1.0.3
#

############################## Help Section ##############################
ifneq ($(findstring Makefile, $(MAKEFILE_LIST)), Makefile)
help:
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
	$(ECHO) "  make host"
	$(ECHO) "      Command to build host application."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""

endif

############################## ??? ##############################

# RELREF = ../
KERNEL_TOP_ALL += $(RELREF)acts_templates/acts_kernel.cpp
HOST_SRCS_ACTS += $(RELREF)host_srcs/hostprocess.cpp
HOST_SRCS_ACTS += $(RELREF)host_srcs/prepare_graph.cpp
# HOST_SRCS_ACTS += $(RELREF)host_srcs/make_graph.cpp
HOST_SRCS_ACTS += $(RELREF)host_srcs/act_pack.cpp
# HOST_SRCS_ACTS += $(RELREF)host_srcs/act_pack_new.cpp
HOST_SRCS_ACTS += $(RELREF)host_srcs/create_act_pack.cpp
# HOST_SRCS_ACTS += $(RELREF)acts_templates/transform_to_actpack.cpp
HOST_SRCS_ACTS += $(RELREF)host_srcs/app.cpp
HOST_SRCS_ACTS += $(RELREF)host_srcs/utility.cpp
HOST_SRCS_ACTS += $(RELREF)host_srcs/algorithm.cpp
# HOST_SRCS_ACTS += $(RELREF)host_srcs/app_hw.cpp
HOST_SRCS_ACTS += $(RELREF)host_srcs/host.cpp
# HOST_SRCS_ACTS += $(RELREF)host_srcs/host_fpga.cpp
# HOST_SRCS_ACTS += $(RELREF)host_srcs/host_fpga_async.cpp

############################## Setting up Project Variables ##############################
TARGET := hw
VPP_LDFLAGS :=
include ./utils.mk

XF_PROJ_ROOT = /home/oj2zf/Documents/Vitis_Accel_Examples-master

TEMP_DIR := ./_x.$(TARGET).$(XSA)
BUILD_DIR := ./build_dir.$(TARGET).$(XSA)

LINK_OUTPUT := $(BUILD_DIR)/vector_addition.link.xclbin
PACKAGE_OUT = ./package.$(TARGET)

VPP_PFLAGS := 
CMD_ARGS = $(BUILD_DIR)/vector_addition.xclbin
CXXFLAGS += -I$(XILINX_XRT)/include -I$(XILINX_VIVADO)/include -Wall -O0 -g -std=c++1y
LDFLAGS += -L$(XILINX_XRT)/lib -pthread -lOpenCL

########################## Checking if PLATFORM in allowlist #######################
PLATFORM_BLOCKLIST += nodma 
############################## Setting up Host Variables ##############################
#Include Required Host Source Files
CXXFLAGS += -I$(XF_PROJ_ROOT)/common/includes/xcl2
HOST_SRCS += $(XF_PROJ_ROOT)/common/includes/xcl2/xcl2.cpp $(HOST_SRCS_ACTS)
# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 

############################## Setting up Kernel Variables ##############################
# Kernel compiler global settings
VPP_FLAGS += --save-temps 

# Kernel linker flags
# VPP_LDFLAGS_krnl_vadd += --config ./krnl_acts1.cfg
VPP_LDFLAGS_krnl_vadd += --config ./krnl_acts12.cfg
EXECUTABLE = ./host
EMCONFIG_DIR = $(TEMP_DIR)

############################## Setting Targets ##############################
.PHONY: all clean cleanall docs emconfig
all: check-platform check-device check-vitis $(EXECUTABLE) $(BUILD_DIR)/vector_addition.xclbin emconfig

.PHONY: host
host: $(EXECUTABLE)

.PHONY: build
build: check-vitis check-device $(BUILD_DIR)/vector_addition.xclbin

.PHONY: xclbin
xclbin: build

############################## Setting Rules for Binary Containers (Building Kernels) ##############################
$(TEMP_DIR)/vadd.xo: $(KERNEL_TOP_ALL)
	mkdir -p $(TEMP_DIR)
	v++ -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k top_function --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'

$(BUILD_DIR)/vector_addition.xclbin: $(TEMP_DIR)/vadd.xo
	mkdir -p $(BUILD_DIR)
	v++ -l $(VPP_FLAGS) $(VPP_LDFLAGS) -t $(TARGET) --platform $(PLATFORM) --temp_dir $(TEMP_DIR) $(VPP_LDFLAGS_krnl_vadd) -o'$(LINK_OUTPUT)' $(+)
	v++ -p $(LINK_OUTPUT) $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) --package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/vector_addition.xclbin

############################## Setting Rules for Host (Building Host Executable) ##############################
# LDFLAGS += -I/opt/xilinx/xrt/include/ -I/tools/Xilinx/./Vitis_HLS/2021.2/include/ -I/tools/Xilinx/Vitis/2021.2/runtime/ -I/tools/Xilinx/Vivado/2021.2/include/ -pthread -march=native -lrt xcl.c -L/opt/Xilinx/SDx/2021.2/runtime/lib/x86_64 -lOpenCL -pthread -lrt 			
# $(EXECUTABLE): $(HOST_SRCS) $(KERNEL_TOP_ALL) | check-xrt
		# g++ -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
LDFLAGS += -I/opt/xilinx/xrt/include/ -I/tools/Xilinx/./Vitis_HLS/2021.2/include/ -I/tools/Xilinx/Vitis/2021.2/runtime/ -I/tools/Xilinx/Vivado/2021.2/include/ -pthread -march=native -lrt xcl.c -L/opt/Xilinx/SDx/2021.2/runtime/lib/x86_64 -lOpenCL -pthread -lrt 			
$(EXECUTABLE): $(HOST_SRCS) | check-xrt
		g++ -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
		
emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(PLATFORM) --od $(EMCONFIG_DIR)
	
############################## CPU Multithreaded Implementation ##############################
actsobj: clean build_acts 
actsGP_debug: clean build_acts run_nthreads_debug

build_acts:
	g++ $(HOST_SRCS_ACTS) $(KERNEL_TOP_ALL) -std=c++11 -pthread -march=native -lrt -o host
run_nthreads:
	./actsobj
run_nthreads_debug:
	gdb ./actsobj
	
############################## Generating source files (Jinja 2) ##############################
generatesrcs:
	python gen.py $(XWARE) $(RUNNING_SYNTHESIS) $(NUM_PEs) $(RUN_IN_ASYNC_MODE)

############################## Setting Essential Checks and Running Rules ##############################
run: all
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	cp -rf $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif

.PHONY: test
test: $(EXECUTABLE)
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif

############################## Cleaning Rules ##############################
# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv 
	-$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) build_dir*
	-$(RMDIR) package.*
	-$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh
	# -$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin


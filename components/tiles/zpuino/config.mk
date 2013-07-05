
###
# This file MUST set up ISHW_CROSS_TOOLCHAIN_BASE_DIR to point to the
# the base directory of the cross compilation toolchain appropriate to
# this type of tile.  For ISHW to find them, the standard cross tools
# (c++, ld, ar, ...) must be in $(ISHW_CROSS_TOOLCHAIN_BASE_DIR)/bin

ISHW_CROSS_TOOLCHAIN_BASE_DIR:=$(ISHW_BASE_DIR)/components/tiles/zpuino/cross/pristine/3.4.2/zpu-elf

###
# This file SHOULD add some text to ISHW_TARGETS_HELP suggesting make
# targets are available and what they might do

ISHW_TARGETS_HELP+="make fpgasim\n\trun an fpga-level simulation. 'BIN_FILE_NAME=/path/to/bin; make fpgasim'\n"
# ISHW_TARGETS_HELP+="make synth-download\n\tdownload BIT_FILE_NAME=$(BIT_FILE_NAME) to SYNTH_DOWNLOADER_DEVICE=$(SYNTH_DOWNLOADER_DEVICE)\n"
ISHW_TARGETS_HELP+="make synth-clean\n\tclean up the synth work directory\n"
ISHW_TARGETS_HELP+="make synth-realclean\n\tdo synth-clean and also nuke bootloader for '$(BIT_FILE_NAME)'\n"
ISHW_TARGETS_HELP+="make bootloader\n\trebuild the bootloader for 'BIN_FILE_NAME=/path/to/bin; make bootloader'\n"

###
# This file MAY set up any other variables that are
# useful for internal operations within this subtree

# ISHW_SYNTH_TOOLCHAIN_BASE:=/open/ackley/14.4/ISE_DS
# ISHW_HW_TOOLCHAIN_BASE:=/home/ackley/papers/MF/asrepo/RT13/projects/V2ISHW/tools/git-zpuino
# ISHW_SW_TOOLCHAIN_BASE:=/home/ackley/papers/MF/asrepo/RT13/projects/V2ISHW/tools/zarduino

## ADDITIONAL TOOL WE NEED AND ASSOCIATED TARGETS
ISHW_CHECK_TARGETS+=check-synth-path
ISHW_REPORT_TARGETS+=report-synth-path

check-synth-path:	FORCE 
	@x=$(ZPUINO_SYNTH_PATH)/ISE/vhdl ; \
	if [ -d $$x ]; then \
	  exit 0; \
	else \
	  f=$$? ; \
	  echo check-synth-path: Failed to find ISE under: $$x ; \
	  exit $$f ; \
	fi

report-synth-path:	check-synth-path
	@echo " ZPUINO: Synth tool path $(ZPUINO_SYNTH_PATH)"


ZPU_ELF_TOOLS_BIN_DIR:=$(ISHW_CROSS_TOOLCHAIN_BASE_DIR)/../bin
BOARD_DIR:=$(ISHW_THIS_DIR)/board/zpu/hdl/zpuino/boards/papilio-pro/S6LX9

fpgasim:	report-synth-path bootloader copyfiles rebuildtestbench

bootloader:	FORCE
	@echo --- Generating bootloader for $(BIN_FILE_NAME)
	@cd $(BOARD_DIR)                                                              ;\
	export PATH=$(ZPU_ELF_TOOLS_BIN_DIR):$$PATH                                   ;\
	make -C ../../../bootloader/ -f Makefile.sim SKETCHBIN=$(BIN_FILE_NAME) > bootloader-generation.log 2>&1 ;\
	echo "  Wrote" `wc -c <bootloader-generation.log` bytes to bootloader-generation.log

copyfiles:	FORCE
	@echo --- Copying files
	@cp $(BOARD_DIR)/../../../bootloader/bootloader-sim.vhd $(BOARD_DIR)
	@cp $(BOARD_DIR)/../../../bootloader/sdram.srec $(BOARD_DIR)

rebuildtestbench:	FORCE
	@echo --- Rebuilding testbench
	@cd $(BOARD_DIR)                                                               ;\
	export XILINX=$(ZPUINO_SYNTH_PATH)/ISE                                         ;\
	make -f Makefile.sim > rebuildtestbench.log 2>&1                               ;\
	echo "  Wrote" `wc -c <rebuildtestbench.log` bytes to rebuildtestbench.log

synth-clean:	FORCE
	@cd $(BOARD_DIR)                                                              ;\
	make -f Makefile.sim clean

synth-realclean:	synth-clean
	@cd $(BOARD_DIR)                                                              ;\
	make -C ../../../bootloader/ clean

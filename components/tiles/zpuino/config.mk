###
# First, capture this directory path
_TILES_ZPUINO_DIR:=$(ISHW_THIS_DIR)

###
# Every tiles/*/config.mk file MUST set up ISHW_CROSS_TOOLCHAIN_PREFIX
# to point to the cross compilation toolchain appropriate to this type
# of tile.  The standard cross tools (c++, ld, ar, ...) must exist as
# $(ISHW_CROSS_TOOLCHAIN_PREFIX)c++ etc (note lack of '/' before c++:
# The prefix must include that if needed).

ISHW_CROSS_TOOLCHAIN_BIN_DIR:=$(_TILES_ZPUINO_DIR)/cross/pristine/3.4.2/bin
ISHW_CROSS_TOOLCHAIN_PREFIX:=$(ISHW_CROSS_TOOLCHAIN_BIN_DIR)/zpu-elf-

ISHW_CROSS_BUILD_DIR:=$(ISHW_BUILD_BASE_DIR)/components/tiles/zpuino

ISHW_CROSS_INCLUDES+=-I$(_TILES_ZPUINO_DIR)/core/zpu20/cores/zpuino
ISHW_CROSS_CFLAGS:=-DZPU -DZPUINO_ACCEL -Wall -O2 -fno-reorder-blocks -fno-reorder-blocks-and-partition -fno-prefetch-loop-arrays -fno-gcse -ffunction-sections -fdata-sections -nostartfiles -mmult -mdiv -mno-callpcrel -mno-pushspadd -mno-poppcrel -DARDUINO=152 -D__ZPUINO_PAPILIO_PRO__ -DBOARD_ID=0xA5041700 -DBOARD_MEMORYSIZE=0x800000 -nostartfiles
ISHW_CROSS_CPPFLAGS:=-fno-exceptions -fno-rtti $(ISHW_CROSS_CFLAGS)
ISHW_CROSS_ASMFLAGS+= -DASSEMBLY $(ISHW_CROSS_CFLAGS)
ISHW_CROSS_ARFLAGS+=crs
ISHW_CROSS_LDFLAGS+=-O2 -nostartfiles -Wl,-T -Wl,$(_TILES_ZPUINO_DIR)/core/zpu20/cores/zpuino/zpuino.lds -Wl,--relax -Wl,--gc-sections
ISHW_CROSS_OBJCOPYFLAGS+=-O binary

ISHW_CROSS_ASM_SOURCES+=$(_TILES_ZPUINO_DIR)/core/zpu20/cores/zpuino/start.S
ISHW_CROSS_ASM_SOURCES+=$(_TILES_ZPUINO_DIR)/core/zpu20/cores/zpuino/zpuino-accel.S
ISHW_CROSS_C_SOURCES+=$(_TILES_ZPUINO_DIR)/core/zpu20/cores/zpuino/crt-c.c
ISHW_CROSS_CPP_SOURCES+=$(wildcard $(_TILES_ZPUINO_DIR)/core/zpu20/cores/zpuino/*.cpp)
ISHW_CROSS_LD_SOURCES+=$(_TILES_ZPUINO_DIR)/core/zpu20/cores/zpuino/zpuino.lds

ISHW_CROSS_ASM_OBJS:=$(patsubst $(_TILES_ZPUINO_DIR)/%.S,$(ISHW_CROSS_BUILD_DIR)/%.o,$(ISHW_CROSS_ASM_SOURCES))
ISHW_CROSS_C_OBJS:=$(patsubst $(_TILES_ZPUINO_DIR)/%.c,$(ISHW_CROSS_BUILD_DIR)/%.o,$(ISHW_CROSS_C_SOURCES))
ISHW_CROSS_CPP_OBJS:=$(patsubst $(_TILES_ZPUINO_DIR)/%.cpp,$(ISHW_CROSS_BUILD_DIR)/%.o,$(ISHW_CROSS_CPP_SOURCES))

ISHW_CROSS_OBJS:=$(ISHW_CROSS_CPP_OBJS) $(ISHW_CROSS_C_OBJS) $(ISHW_CROSS_ASM_OBJS)

###
# Every config.mk file SHOULD add some text to ISHW_TARGETS_HELP
# suggesting make targets are available and what they might do

ISHW_TARGETS_HELP+="make build-fpga-sim\n\tbuild the testbench for an fpga-level simulation. 'BIN_FILE_NAME=/path/to/bin; make build-fpga-sim'\n"
ISHW_TARGETS_HELP+="make run-fpga-sim\n\trun an fpga-level simulation. 'BIN_FILE_NAME=/path/to/bin; make run-fpga-sim'\n"
# ISHW_TARGETS_HELP+="make synth-download\n\tdownload BIT_FILE_NAME=$(BIT_FILE_NAME) to SYNTH_DOWNLOADER_DEVICE=$(SYNTH_DOWNLOADER_DEVICE)\n"
ISHW_TARGETS_HELP+="make sim-clean\n\tclean simulation-related material\n"
ISHW_TARGETS_HELP+="make synth\n\trun the synthesis\n"
ISHW_TARGETS_HELP+="make synth-clean\n\tclean up the synth work directory\n"
ISHW_TARGETS_HELP+="make synth-realclean\n\tdo synth-clean and also nuke bootloader for '$(BIT_FILE_NAME)'\n"
ISHW_TARGETS_HELP+="make generate-bootloader\n\trebuild the bootloader for 'BIN_FILE_NAME=/path/to/bin; make generate-bootloader'\n"
ISHW_TARGETS_HELP+="make zpu-core-library\n\tbuild libzpucore.a, containing low-level zpuino routines\n"

###
# This file MAY set up any other variables and targets that are useful
# for internal operations within this subtree.  Any such internal
# variables and targets MUST use a prefix formed by '_' plus the path
# below 'components' (e.g., _tiles_zpuino_foo, _TILES_ZPUINO_BAR)

_TILES_ZPUINO_BOARD_DIR:=$(_TILES_ZPUINO_DIR)/board/zpu/hdl/zpuino/boards/papilio-pro/S6LX9

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

build-fpga-sim:	report-synth-path generate-bootloader copy-files rebuild-test-bench

zpu-core-library:	$(ISHW_CROSS_BUILD_DIR)/libzpucore.a

$(ISHW_CROSS_BUILD_DIR)/libzpucore.a:	$(ISHW_CROSS_BUILD_DIR)/.exists $(ISHW_CROSS_OBJS) $(ISHW_ALL_DEP)
	@$(CROSS_AR) $(ISHW_CROSS_ARFLAGS) $@ $(ISHW_CROSS_OBJS) >$@.log
	@echo Built $@

generate-bootloader:	FORCE
	@echo --- Generating bootloader for $(BIN_FILE_NAME)
	@cd $(_TILES_ZPUINO_BOARD_DIR)                                                               ;\
	export PATH=$(ISHW_CROSS_TOOLCHAIN_BIN_DIR):$$PATH                             ;\
	make -C ../../../bootloader/ -f Makefile.sim clean  > bootloader-generation.log 2>&1 ;\
	make -C ../../../bootloader/ -f Makefile.sim SKETCHBIN=$(BIN_FILE_NAME)  >> bootloader-generation.log 2>&1;\
	echo "  Wrote" `wc -c <bootloader-generation.log` bytes to bootloader-generation.log

copy-files:	generate-bootloader
	@echo --- Copying files
	@cp $(_TILES_ZPUINO_BOARD_DIR)/../../../bootloader/bootloader-sim.vhd $(_TILES_ZPUINO_BOARD_DIR)
	@cp $(_TILES_ZPUINO_BOARD_DIR)/../../../bootloader/sdram.srec $(_TILES_ZPUINO_BOARD_DIR)

synth:	report-synth-path
	@echo --- Starting synthesis at `date`
	@cd $(_TILES_ZPUINO_BOARD_DIR)                                                               ;\
	start=$$(date +"%s")                                                           ;\
	export PATH=$(ISHW_CROSS_TOOLCHAIN_BIN_DIR):$$PATH                             ;\
	. "$(ZPUINO_SYNTH_PATH)/ISE/.settings64.sh" "$(ZPUINO_SYNTH_PATH)/ISE"         ;\
	make -f Makefile > synthesis.log 2>&1                                          ;\
	stop=$$(date +"%s")                                                            ;\
	diff=$$(($$stop-$$start))                                                      ;\
	echo "  Wrote" `wc -c <synthesis.log` bytes to synthesis.log                   ;\
	echo -e --- Synthesis took `date -u -d @"$$diff" +'%-Mm %-Ss'` "\n"

rebuild-test-bench:	copy-files
	@echo --- Rebuilding testbench
	@cd $(_TILES_ZPUINO_BOARD_DIR)                                                               ;\
	export XILINX=$(ZPUINO_SYNTH_PATH)/ISE                                         ;\
	make -f Makefile.sim > rebuild-test-bench.log 2>&1                             ;\
	echo "  Wrote" `wc -c <rebuild-test-bench.log` bytes to rebuild-test-bench.log

run-fpga-sim:	rebuild-test-bench
	@echo --- Running testbench
	@cd $(_TILES_ZPUINO_BOARD_DIR)                                                               ;\
	./tb --ieee-asserts=disable --wave=out.ghw --stop-time=1000us

sim-clean:	FORCE
	@cd $(_TILES_ZPUINO_BOARD_DIR)                                                               ;\
	make -f Makefile.sim clean

synth-clean:	FORCE
	@cd $(_TILES_ZPUINO_BOARD_DIR)                                                               ;\
	make clean

synth-realclean:	synth-clean
	@cd $(_TILES_ZPUINO_BOARD_DIR)                                                               ;\
	make -C ../../../bootloader/ clean

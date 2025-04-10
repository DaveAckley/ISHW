###
# First, capture this directory path
_TILES_ZPUINO.DIR:=$(ISHW_THIS_DIR)

###
# Every tiles/*/config.mk file MUST set up ISHW_CROSS_TOOLCHAIN_PREFIX
# to point to the cross compilation toolchain appropriate to this type
# of tile.  The standard cross tools (c++, ld, ar, ...) must exist as
# $(ISHW_CROSS_TOOLCHAIN_PREFIX)c++ etc (note lack of '/' before c++:
# The prefix must include that if needed).

ISHW_CROSS_TOOLCHAIN_BIN_DIR:=$(_TILES_ZPUINO.DIR)/cross/pristine/3.4.2/bin
ISHW_CROSS_TOOLCHAIN_PREFIX:=$(ISHW_CROSS_TOOLCHAIN_BIN_DIR)/zpu-elf-

ISHW_CROSS_BUILD_DIR:=$(ISHW_BUILD_BASE_DIR)/components/tiles/zpuino

ISHW_CROSS_INCLUDES+=-I$(_TILES_ZPUINO.DIR)/core/zpu20/cores/zpuino
ISHW_CROSS_CFLAGS:=-DZPU -DZPUINO_ACCEL -Wall -O2 -fno-reorder-blocks -fno-reorder-blocks-and-partition -fno-prefetch-loop-arrays -fno-gcse -ffunction-sections -fdata-sections -mmult -mdiv -mno-callpcrel -mno-pushspadd -mno-poppcrel -DARDUINO=152 -D__ZPUINO_PAPILIO_PRO__ -DBOARD_ID=0xA5041700 -DBOARD_MEMORYSIZE=0x800000 -nostartfiles
ISHW_CROSS_CPPFLAGS:=-fno-exceptions -fno-rtti $(ISHW_CROSS_CFLAGS)
ISHW_CROSS_ASMFLAGS+= -DASSEMBLY $(ISHW_CROSS_CFLAGS)
ISHW_CROSS_ARFLAGS+=crs
ISHW_CROSS_LDFLAGS+=-O2 -nostartfiles -Wl,-T -Wl,$(_TILES_ZPUINO.DIR)/core/zpu20/cores/zpuino/zpuino.lds -Wl,--relax -Wl,--gc-sections
ISHW_CROSS_OBJCOPYFLAGS+=-O binary

ISHW_CROSS_ASM_SOURCES+=$(_TILES_ZPUINO.DIR)/core/zpu20/cores/zpuino/start.S
ISHW_CROSS_ASM_SOURCES+=$(_TILES_ZPUINO.DIR)/core/zpu20/cores/zpuino/zpuino-accel.S
ISHW_CROSS_C_SOURCES+=$(_TILES_ZPUINO.DIR)/core/zpu20/cores/zpuino/crt-c.c
ISHW_CROSS_CPP_SOURCES+=$(wildcard $(_TILES_ZPUINO.DIR)/core/zpu20/cores/zpuino/*.cpp)
ISHW_CROSS_LD_SOURCES+=$(_TILES_ZPUINO.DIR)/core/zpu20/cores/zpuino/zpuino.lds

ISHW_CROSS_ASM_OBJS:=$(patsubst $(_TILES_ZPUINO.DIR)/%.S,$(ISHW_CROSS_BUILD_DIR)/%.o,$(ISHW_CROSS_ASM_SOURCES))
ISHW_CROSS_C_OBJS:=$(patsubst $(_TILES_ZPUINO.DIR)/%.c,$(ISHW_CROSS_BUILD_DIR)/%.o,$(ISHW_CROSS_C_SOURCES))
ISHW_CROSS_CPP_OBJS:=$(patsubst $(_TILES_ZPUINO.DIR)/%.cpp,$(ISHW_CROSS_BUILD_DIR)/%.o,$(ISHW_CROSS_CPP_SOURCES))

ISHW_CROSS_OBJS:=$(ISHW_CROSS_CPP_OBJS) $(ISHW_CROSS_C_OBJS) $(ISHW_CROSS_ASM_OBJS)

###
# This file MAY set up any other variables and targets that are useful
# for internal operations within this subtree.  Any such internal
# variables and targets MUST use a prefix formed by '_' plus the path
# below 'components' (e.g., _tiles_zpuino_foo, _TILES_ZPUINO_BAR)

_TILES_ZPUINO.HDL_DIR:=$(_TILES_ZPUINO.DIR)/board/zpu/hdl
_TILES_ZPUINO.BOARD_DIR:=$(_TILES_ZPUINO.HDL_DIR)/zpuino/boards/papilio-pro/S6LX9
_TILES_ZPUINO.BOARD_NAME:=papilio_pro

_TILES_ZPUINO.PROGRAMMER_SRC_DIR:=$(_TILES_ZPUINO.DIR)/board/zpu/hdl/zpuino/programmer

_TILES_ZPUINO.PROGRAMMER_BUILD_DIR:=$(ISHW_BUILD_BASE_DIR)/programmer
_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR:=$(ISHW_BUILD_BASE_DIR)/bin

_TILES_ZPUINO.PAPILIO_PROG_BASE_DIR:=$(_TILES_ZPUINO.DIR)/synth/Papilio-Loader
_TILES_ZPUINO.PAPILIO_PROG_SRC_DIR:=$(_TILES_ZPUINO.PAPILIO_PROG_BASE_DIR)/papilio-prog
_TILES_ZPUINO.PAPILIO_PROG_BSCAN_FILE:=$(_TILES_ZPUINO.PAPILIO_PROG_BASE_DIR)/Java-GUI/programmer/bscan_spi_xc6slx9.bit

_TILES_ZPUINO.SYNTH_BUILD_DIR:=$(ISHW_BUILD_BASE_DIR)/synth
_TILES_ZPUINO.SYNTH_BITFILE_PATH:=$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)/$(_TILES_ZPUINO.BOARD_NAME)_routed.bit


###
# Every config.mk file SHOULD add some text to ISHW_TARGETS_HELP
# suggesting make targets are available and what they might do

ISHW_TARGETS_HELP+="make build-fpga-sim\n\tbuild the testbench for an fpga-level simulation. 'BIN_FILE_NAME=/path/to/bin; make build-fpga-sim'\n"
ISHW_TARGETS_HELP+="make run-fpga-sim\n\trun an fpga-level simulation. 'BIN_FILE_NAME=/path/to/bin; make run-fpga-sim'\n"
# ISHW_TARGETS_HELP+="make synth-download\n\tdownload BIT_FILE_NAME=$(BIT_FILE_NAME) to SYNTH_DOWNLOADER_DEVICE=$(SYNTH_DOWNLOADER_DEVICE)\n"
ISHW_TARGETS_HELP+="make sim-clean\n\tclean simulation-related material\n"
ISHW_TARGETS_HELP+="make download-synth-file\n\tdownload $(_TILES_ZPUINO.SYNTH_BITFILE_PATH) to ZPUINO_DOWNLOAD_DEVICE ($(ZPUINO_DOWNLOAD_DEVICE))\n"
ISHW_TARGETS_HELP+="make synth\n\trun the synthesis\n"
ISHW_TARGETS_HELP+="make synth-clean\n\tclean up the synth work directory\n"
ISHW_TARGETS_HELP+="make synth-realclean\n\tdo synth-clean and also nuke bootloader for '$(BIT_FILE_NAME)'\n"
ISHW_TARGETS_HELP+="make generate-bootloader\n\trebuild the bootloader for 'BIN_FILE_NAME=/path/to/bin; make generate-bootloader'\n"
ISHW_TARGETS_HELP+="make zpu-core-library\n\tbuild libzpucore.a, containing low-level zpuino routines\n"

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
	@cd $(_TILES_ZPUINO.BOARD_DIR)                                                               ;\
	export PATH=$(ISHW_CROSS_TOOLCHAIN_BIN_DIR):$$PATH                                           ;\
	make -C ../../../bootloader/ -f Makefile.sim clean  > bootloader-generation.log 2>&1         ;\
	make -C ../../../bootloader/ -f Makefile.sim SKETCHBIN=$(BIN_FILE_NAME)  >> bootloader-generation.log 2>&1 || exit 11;\
	echo "[Wrote" `wc -c <bootloader-generation.log` "bytes to $(_TILES_ZPUINO.BOARD_DIR)/bootloader-generation.log]"

copy-files:	generate-bootloader
	@echo --- Copying files
	@cp $(_TILES_ZPUINO.BOARD_DIR)/../../../bootloader/bootloader-sim.vhd $(_TILES_ZPUINO.BOARD_DIR)
	@cp $(_TILES_ZPUINO.BOARD_DIR)/../../../bootloader/sdram.srec $(_TILES_ZPUINO.BOARD_DIR)

synth:	report-synth-path $(_TILES_ZPUINO.SYNTH_BITFILE_PATH)

$(_TILES_ZPUINO.SYNTH_BITFILE_PATH):	$(_TILES_ZPUINO.SYNTH_BUILD_DIR)/.exists
	@echo "[Starting synthesis at" `date` "logging to" $(_TILES_ZPUINO.SYNTH_BUILD_DIR)/synthesis.log "]"
	@cd $(_TILES_ZPUINO.BOARD_DIR)                                                 ;\
	start=$$(date +"%s")                                                           ;\
	export PATH=$(ISHW_CROSS_TOOLCHAIN_BIN_DIR):$$PATH                             ;\
	. "$(ZPUINO_SYNTH_PATH)/ISE/.settings64.sh" "$(ZPUINO_SYNTH_PATH)/ISE"         ;\
	make -f Makefile > $(_TILES_ZPUINO.SYNTH_BUILD_DIR)/synthesis.log 2>&1 || exit 11                               ;\
	cp $(_TILES_ZPUINO.BOARD_NAME)_routed.bit $(_TILES_ZPUINO.BOARD_NAME)_routed.bin $(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR) ;\
	stop=$$(date +"%s")                                                            ;\
	diff=$$(($$stop-$$start))                                                      ;\
	echo "  Wrote" `wc -c <synthesis.log` bytes to $(_TILES_ZPUINO.SYNTH_BUILD_DIR)/synthesis.log                   ;\
	echo -e --- Synthesis took `date -u -d @"$$diff" +'%-Mm %-Ss'` "\n"

download-synth-file:	$(_TILES_ZPUINO.SYNTH_BITFILE_PATH) $(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)/papilio-prog $(_TILES_ZPUINO.PAPILIO_PROG_BSCAN_FILE) kill-minicom
	$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)/papilio-prog -f $(_TILES_ZPUINO.SYNTH_BITFILE_PATH) -sa -r -b $(_TILES_ZPUINO.PAPILIO_PROG_BSCAN_FILE)

kill-minicom:	FORCE
	@ps cax | grep minicom &&\
	echo -n "[Shooting minicom and waiting for it to die.." &&\
	killall -w -15 minicom &&\
	echo " Done]" ; true

rebuild-test-bench:	copy-files
	@echo --- Rebuilding testbench
	@cd $(_TILES_ZPUINO.BOARD_DIR)                                                 ;\
	make -f Makefile.sim clean > rebuild-test-bench.log 2>&1 || exit 11            ;\
	export XILINX=$(ZPUINO_SYNTH_PATH)/ISE                                         ;\
	make -f Makefile.sim >> rebuild-test-bench.log 2>&1 || exit 11                 ;\
	echo "  Wrote" `wc -c <rebuild-test-bench.log` bytes to rebuild-test-bench.log

run-fpga-sim:	rebuild-test-bench
	@echo --- Running testbench
	@cd $(_TILES_ZPUINO.BOARD_DIR)                                                 ;\
	./tb --ieee-asserts=disable --wave=out.ghw --stop-time=1000us

download-bin-file:	$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)/zpuinoprogrammer kill-minicom
	@echo --- Downloading $(BIN_FILE_NAME)
	$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)/zpuinoprogrammer -v -R -s 1000000 -d $(ZPUINO_DOWNLOAD_DEVICE) -b $(BIN_FILE_NAME)


sim-clean:	FORCE
	@cd $(_TILES_ZPUINO.BOARD_DIR)                                                               ;\
	make -f Makefile.sim clean

synth-clean:	FORCE
	@rm -f $(_TILES_ZPUINO.SYNTH_BITFILE_PATH)
	@cd $(_TILES_ZPUINO.BOARD_DIR)                                                               ;\
	make clean

synth-realclean:	synth-clean
	@cd $(_TILES_ZPUINO.BOARD_DIR)                                                               ;\
	make -C ../../../bootloader/ clean


build-zpuinoprogrammer:	$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)/zpuinoprogrammer

$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)/zpuinoprogrammer:	$(_TILES_ZPUINO.PROGRAMMER_SRC_DIR)/Makefile
	cd $(_TILES_ZPUINO.PROGRAMMER_SRC_DIR);make || exit 11 ;make install || exit 11

$(_TILES_ZPUINO.PROGRAMMER_SRC_DIR)/Makefile:	$(_TILES_ZPUINO.PROGRAMMER_SRC_DIR)/configure
	cd $(_TILES_ZPUINO.PROGRAMMER_SRC_DIR);./configure --bindir=$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)

$(_TILES_ZPUINO.PROGRAMMER_SRC_DIR)/configure:	$(_TILES_ZPUINO.PROGRAMMER_SRC_DIR)/configure.ac $(ISHW_ALL_DEP)
	cd $(_TILES_ZPUINO.PROGRAMMER_SRC_DIR);autoreconf;automake --add-missing


build-papilio-prog:	$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)/papilio-prog

$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)/papilio-prog:	$(_TILES_ZPUINO.PAPILIO_PROG_SRC_DIR)/Makefile
	cd $(_TILES_ZPUINO.PAPILIO_PROG_SRC_DIR);make || exit 11;make install || exit 11

$(_TILES_ZPUINO.PAPILIO_PROG_SRC_DIR)/Makefile:	$(_TILES_ZPUINO.PAPILIO_PROG_SRC_DIR)/configure
	cd $(_TILES_ZPUINO.PAPILIO_PROG_SRC_DIR);./configure --bindir=$(_TILES_ZPUINO.PROGRAMMER_INSTALL_DIR)

$(_TILES_ZPUINO.PAPILIO_PROG_SRC_DIR)/configure:	$(_TILES_ZPUINO.PAPILIO_PROG_SRC_DIR)/configure.ac $(ISHW_ALL_DEP)
	cd $(_TILES_ZPUINO.PAPILIO_PROG_SRC_DIR);sh ./autogen.sh

# Include D8FW HW configuration and generation stuff
include $(_TILES_ZPUINO.DIR)/d8fw/config.mk

###
# This file MUST set up ISHW_CROSS_TOOLCHAIN_BASE_DIR to point to the
# the base directory of the cross compilation toolchain appropriate to
# this type of tile.  For ISHW to find them, the standard cross tools
# (c++, ld, ar, ...) must be in $(ISHW_CROSS_TOOLCHAIN_BASE_DIR)/bin

ISHW_CROSS_TOOLCHAIN_BASE_DIR:=$(ISHW_BASE_DIR)/components/tiles/zpuino/cross/pristine/3.4.2/zpu-elf

###
# This file SHOULD add some text to ISHW_TARGETS_HELP suggesting make
# targets are available and what they might do

ISHW_TARGETS_HELP+="make synth\n\trun a synthesis, producing BIT_FILE_NAME=$(BIT_FILE_NAME)\n"
ISHW_TARGETS_HELP+="make synth-download\n\tdownload BIT_FILE_NAME=$(BIT_FILE_NAME) to SYNTH_DOWNLOADER_DEVICE=$(SYNTH_DOWNLOADER_DEVICE)\n"
ISHW_TARGETS_HELP+="make synth-clean\n\tclean up the synth work directory\n"
ISHW_TARGETS_HELP+="make synth-realclean\n\tdo synth-clean and also nuke '$(BIT_FILE_NAME)'\n"
ISHW_TARGETS_HELP+="make bootloader\n\trebuild the bootloader\n"

###
# This file MAY set up any other variables that are
# useful for internal operations within this subtree

# ISHW_SYNTH_TOOLCHAIN_BASE:=/open/ackley/14.4/ISE_DS
# ISHW_HW_TOOLCHAIN_BASE:=/home/ackley/papers/MF/asrepo/RT13/projects/V2ISHW/tools/git-zpuino
# ISHW_SW_TOOLCHAIN_BASE:=/home/ackley/papers/MF/asrepo/RT13/projects/V2ISHW/tools/zarduino


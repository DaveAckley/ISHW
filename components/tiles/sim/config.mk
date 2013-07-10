###
# First, capture this directory path
_TILES_SIM_DIR:=$(ISHW_THIS_DIR)

###
# Every tiles/*/config.mk file MUST set up ISHW_CROSS_TOOLCHAIN_PREFIX
# to point to the cross compilation toolchain appropriate to this type
# of tile.  The standard cross tools (c++, ld, ar, ...) must exist as
# $(ISHW_CROSS_TOOLCHAIN_PREFIX)c++ etc (note lack of '/' before c++:
# The prefix must include that if needed).

## In the sim tile, we use the native toolchain, or whatever the user
## sets up in PATH for 'which c++' to find.

ISHW_CROSS_TOOLCHAIN_BASE_DIR:=$(patsubst %/bin,%,$(realpath $(dir $(shell which c++))))
ISHW_CROSS_TOOLCHAIN_BIN_DIR:=$(ISHW_CROSS_TOOLCHAIN_BASE_DIR)/bin
ISHW_CROSS_TOOLCHAIN_PREFIX:=$(ISHW_CROSS_TOOLCHAIN_BIN_DIR)/

###
# This file SHOULD add some text to ISHW_TARGETS_HELP suggesting make
# targets are available and what they might do

ISHW_TARGETS_HELP+="make tile\n\tbuild the sim tile simulator\n"


# In addition, this file MAY set up any other variables that are
# useful for internal operations within this subtree

# 
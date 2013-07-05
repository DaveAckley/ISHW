# This file MUST set up ISHW_CROSS_TOOLCHAIN_BASE_DIR to point to the
# the base directory of the cross compilation toolchain appropriate to
# this type of tile.  For ISHW to find them, the standard cross tools
# (c++, ld, ar, ...) must be in $(ISHW_CROSS_TOOLCHAIN_BASE_DIR)/bin

## In the sim tile, we use the native toolchain, or whatever the user
## sets up in PATH for 'which c++' to find.

ISHW_CROSS_TOOLCHAIN_BASE_DIR:=$(patsubst %/bin,%,$(realpath $(dir $(shell which c++))))

###
# This file SHOULD add some text to ISHW_TARGETS_HELP suggesting make
# targets are available and what they might do

ISHW_TARGETS_HELP+="make tile\n\tbuild the sim tile simulator\n"


# In addition, this file MAY set up any other variables that are
# useful for internal operations within this subtree

# 
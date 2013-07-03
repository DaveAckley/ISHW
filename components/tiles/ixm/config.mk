# This file MUST set up ISHW_CROSS_TOOLCHAIN_BASE_DIR to point to the
# the base directory of the cross compilation toolchain appropriate to
# this type of tile.  For ISHW to find them, the standard cross tools
# (c++, ld, ar, ...) must be in $(ISHW_CROSS_TOOLCHAIN_BASE_DIR)/bin

ISHW_CROSS_TOOLCHAIN_BASE_DIR:=$(ISHW_BASE_DIR)/components/tiles/ixm/cross/arm-none-eabi


# In addition, this file MAY set up any other variables that are
# useful for internal operations within this subtree

# 
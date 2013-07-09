# This file MUST set up ISHW_CROSS_TOOLCHAIN_PREFIX to point to the
# cross compilation toolchain appropriate to this type of tile.  The
# standard cross tools (c++, ld, ar, ...) must exist as
# $(ISHW_CROSS_TOOLCHAIN_PREFIX)c++ etc (note lack of '/' before c++:
# The prefix must include that if needed).

ISHW_CROSS_TOOLCHAIN_PREFIX:=$(ISHW_BASE_DIR)/components/tiles/ixm/cross/arm-none-eabi/bin/


# In addition, this file MAY set up any other variables that are
# useful for internal operations within this subtree

# 
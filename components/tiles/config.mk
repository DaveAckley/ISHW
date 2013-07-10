###
# First, capture this directory path
_TILES.DIR:=$(ISHW_THIS_DIR)

_TILES.INCLUDE_DIR:=$(_TILES.DIR)/include

ISHW_TARGETS_HELP+="\nTILE TARGETS (type $(CONFIG_TILE_TYPE))\n"

include $(_TILES.DIR)/$(CONFIG_TILE_TYPE)/config.mk

ISHW_CROSS_INCLUDES+=-I$(_TILES.INCLUDE_DIR)

check-tiles-path:	FORCE 
	@x=$(ISHW_CROSS_TOOLCHAIN_PREFIX)c++ ; \
	if [ -x $$x ]; then \
	  exit 0; \
	else \
	  f=$$? ; \
	  echo check-tiles-path: Failed to find executable: $$x ; \
	  exit $$f ; \
	fi

report-tiles-path:	check-tiles-path
	@echo TILE: Building type $(CONFIG_TILE_TYPE)

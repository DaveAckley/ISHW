ISHW_TARGETS_HELP+="\nTILE TARGETS (type $(CONFIG_TILE_TYPE))\n"

ISHW_TILES_DIR:=$(ISHW_COMPONENT_DIR)/tiles
ISHW_TILE_INCLUDE_DIR:=$(ISHW_COMPONENT_DIR)/tiles/include
include $(ISHW_TILES_DIR)/$(CONFIG_TILE_TYPE)/config.mk

ISHW_CROSS_INCLUDES+=-I$(ISHW_TILES_DIR)/include

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

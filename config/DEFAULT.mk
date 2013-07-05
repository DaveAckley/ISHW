ifndef CONFIG_TILE_TYPE
  CONFIG_TILE_TYPE:=zpuino
# CONFIG_TILE_TYPE:=ixm
# CONFIG_TILE_TYPE:=sim
endif

ifeq ($(CONFIG_TILE_TYPE),zpuino)
  ZPUINO_SYNTH_PATH:=/open/ackley/14.4/ISE_DS
endif

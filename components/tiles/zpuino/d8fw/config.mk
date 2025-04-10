###
# Capture this directory path
_TILES_ZPUINO_D8FW.DIR:=$(ISHW_THIS_DIR)

ISHW_TARGETS_HELP+="make build-d8fw\n\tbring up-to-date files associated with the double eight-fold wing\n"
ISHW_TARGETS_HELP+="make clean-d8fw\n\tdiscard files generated by 'make build-d8fw'\n"
ISHW_TARGETS_HELP+="make realclean-d8fw\n\tpossibly try harder to clean up d8fw-related material'\n"

_TILES_ZPUINO_D8FW.ZPUINOPKG_DIR:=$(_TILES_ZPUINO.HDL_DIR)/zpuino
_TILES_ZPUINO_D8FW.BUILD_DIR:=$(ISHW_CROSS_BUILD_DIR)/d8fw
_TILES_ZPUINO_D8FW.DOC_DIR:=$(ISHW_BASE_DIR)/doc/tile/zpuino/data
_TILES_ZPUINO_D8FW.INPUTS:=
_TILES_ZPUINO_D8FW.INPUTS+=$(_TILES_ZPUINO_D8FW.DIR)/D8FW_gen.pl
_TILES_ZPUINO_D8FW.INPUTS+=$(_TILES_ZPUINO_D8FW.DIR)/D8FW_names.dat
_TILES_ZPUINO_D8FW.INPUTS+=$(_TILES_ZPUINO_D8FW.DIR)/D8FW_connections.dat
_TILES_ZPUINO_D8FW.INPUTS+=$(_TILES_ZPUINO_D8FW.DIR)/papilio_pro_top.vhd.dat
_TILES_ZPUINO_D8FW.INPUTS+=$(_TILES_ZPUINO_D8FW.DIR)/wbarbN_1.vhd.pl
_TILES_ZPUINO_D8FW.INPUTS+=$(_TILES_ZPUINO_D8FW.DIR)/zpuinopkg.vhd.pl

_TILES_ZPUINO_D8FW.FIRST_OUTPUT:=$(_TILES_ZPUINO_D8FW.DOC_DIR)/d8fw.png
_TILES_ZPUINO_D8FW.OUTPUTS:=
_TILES_ZPUINO_D8FW.OUTPUTS+=$(_TILES_ZPUINO_D8FW.FIRST_OUTPUT) 
_TILES_ZPUINO_D8FW.OUTPUTS+=$(_TILES_ZPUINO_D8FW.DOC_DIR)/D8FWPin2Pin.csv
_TILES_ZPUINO_D8FW.OUTPUTS+=$(_TILES_ZPUINO_D8FW.DOC_DIR)/D8FWByPin.csv
_TILES_ZPUINO_D8FW.OUTPUTS+=$(_TILES_ZPUINO_D8FW.DOC_DIR)/D8FWByFunc.csv
_TILES_ZPUINO_D8FW.OUTPUTS+=$(_TILES_ZPUINO_D8FW.BUILD_DIR)/debug.log
_TILES_ZPUINO_D8FW.OUTPUTS+=$(_TILES_ZPUINO.BOARD_DIR)/papilio_pro_top.vhd
_TILES_ZPUINO_D8FW.OUTPUTS+=$(_TILES_ZPUINO.BOARD_DIR)/wbarb8_1.vhd
_TILES_ZPUINO_D8FW.OUTPUTS+=$(_TILES_ZPUINO_D8FW.ZPUINOPKG_DIR)/zpuinopkg.vhd


build-d8fw:	$(_TILES_ZPUINO_D8FW.FIRST_OUTPUT)

$(_TILES_ZPUINO_D8FW.FIRST_OUTPUT):	$(_TILES_ZPUINO_D8FW.INPUTS) $(_TILES_ZPUINO_D8FW.BUILD_DIR)/.exists
	cd $(_TILES_ZPUINO_D8FW.DIR) ;\
	./D8FW_gen.pl $(_TILES_ZPUINO_D8FW.OUTPUTS)

clean-d8fw:	
	rm -f $(_TILES_ZPUINO_D8FW.OUTPUTS)

realclean-d8fw:	clean-d8fw


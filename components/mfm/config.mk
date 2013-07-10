###
# First, capture this directory path
_MFM.DIR:=$(ISHW_THIS_DIR)

###
# Every config.mk file SHOULD add some text to ISHW_TARGETS_HELP
# suggesting make targets are available and what they might do

ISHW_TARGETS_HELP+="\nMFM TARGETS\n"
ISHW_TARGETS_HELP+="make demo-sketch\n\tbuild a hello world sketch to test the build system\n"
ISHW_TARGETS_HELP+="make mfm-sketch\n\tbuild a simple sketch using mfm features to test OS interaction\n"
ISHW_TARGETS_HELP+="make fpga-sim-demo-sketch\n\tsimulate the demo sketch to test the fpga simulation system\n"
ISHW_TARGETS_HELP+="make fpga-sim-mfm-sketch\n\tsimulate the mfm sketch to test the fpga simulation system\n"
ISHW_TARGETS_HELP+="make my-sketch\n\tbuild a given sketch in place with 'MY_SKETCH=/path/to/sketch.cpp make my-sketch\n"
ISHW_TARGETS_HELP+="make mfm\n\tbuild the basic MFM execution environment\n"

_MFM.SKETCH_BUILD_DIR:=$(_MFM.DIR)/sketch
_MFM.SKETCHES:=demo-sketch mfm-sketch

check-mfm-path:	check-os-path

report-mfm-path:	check-mfm-path
	@echo MFM:

mfm:	FORCE
	@echo "Target $@ not yet implemented"
	@exit 99

my-sketch:	FORCE
	@if [ "x$(MY_SKETCH)" = "x" -o ! -r "$(MY_SKETCH)" ]       ;\
	then echo 'MY_SKETCH ($(MY_SKETCH)) unset or unreadable'   ;\
	exit 9 ; fi
	cp $(MY_SKETCH) $(_MFM.SKETCH_BUILD_DIR)/mySketch.cpp
	make fpga-sim-mySketch

fpga-sim-%:	$(_MFM.SKETCH_BUILD_DIR)/%.bin 
	BIN_FILE_NAME=$< make run-fpga-sim

.PHONY:	$(_MFM.SKETCHES)

.SECONDEXPANSION:
$(_MFM.SKETCHES):	$(_MFM.SKETCH_BUILD_DIR)/$$@.bin $$@-size

%-sketch-size:	$(_MFM.SKETCH_BUILD_DIR)/%-sketch.elf
	@$(CROSS_SIZE) $<

$(_MFM.SKETCH_BUILD_DIR)/%.bin:	$(_MFM.SKETCH_BUILD_DIR)/%.elf $(ISHW_ALL_DEP)
	@$(CROSS_OBJCOPY) $(ISHW_CROSS_OBJCOPYFLAGS) $< $@ >$@.log
	@echo Built $@

$(_MFM.SKETCH_BUILD_DIR)/%.elf:	$(_MFM.SKETCH_BUILD_DIR)/%.o zpu-core-library $(ISHW_CROSS_LD_SOURCES) $(ISHW_ALL_DEP)
	@$(CROSS_CPP) $(ISHW_CROSS_LDFLAGS) -Wl,-Map,$(patsubst %.elf,%.map,$@) -o $@ -L$(ISHW_CROSS_BUILD_DIR) -lm -Wl,--warn-common -Wl,--warn-section-align -Wl,--whole-archive $< $(ISHW_CROSS_BUILD_DIR)/libzpucore.a -Wl,--no-whole-archive >$@.log
	@echo Built $@

$(_MFM.SKETCH_BUILD_DIR)/%.o:	$(_MFM.SKETCH_BUILD_DIR)/%.cpp $(ISHW_ALL_DEP)
	@$(CROSS_CPP) -c $(ISHW_CROSS_CPPFLAGS) $(ISHW_CROSS_INCLUDES) $< -o $@ >$@.log
	@echo Built $@

$(_MFM.SKETCH_BUILD_DIR)/demo-sketch.cpp: $(_MFM.SKETCH_BUILD_DIR)/.exists $(ISHW_ALL_DEP)
	@echo -e '#include <Arduino.h>\nvoid setup() { Serial.begin(115200); }\nvoid loop() { static int count = 0; Serial.print("ds' `date +%s` '"); Serial.println(++count); }\n' > $@
	@echo Built $@

$(_MFM.SKETCH_BUILD_DIR)/mfm-sketch.cpp: $(_MFM.SKETCH_BUILD_DIR)/.exists $(ISHW_ALL_DEP)
	@echo -e '#include <MFM.h>\nvoid setup() { Serial.begin(115200); }\nvoid loop() { static int count = 0; Serial.print("mfm' `date +%s` '"); Serial.println(++count); }\n' > $@
	@echo Built $@

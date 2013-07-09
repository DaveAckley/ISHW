ISHW_TARGETS_HELP+="\nMFM TARGETS\n"
ISHW_TARGETS_HELP+="make demo-sketch\n\tbuild a hello world sketch to test the build system\n"
ISHW_TARGETS_HELP+="make fpga-sim-demo-sketch\n\tsimulate the demo sketch to test the fpga simulation system\n"
ISHW_TARGETS_HELP+="make mfm\n\tbuild the basic MFM execution environment\n"

ISHW_SKETCH_BUILD_DIR:=$(ISHW_BUILD_BASE_DIR)/components/mfm/sketch

check-mfm-path:	check-os-path

report-mfm-path:	check-mfm-path
	@echo MFM:

mfm:	FORCE
	@echo "Target $@ not yet implemented"
	@exit 99

fpga-sim-demo-sketch:	demo-sketch
	BIN_FILE_NAME=$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.bin make run-fpga-sim

demo-sketch:	$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.bin demo-sketch-size

demo-sketch-size:	$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.elf
	@$(CROSS_SIZE) $(ISHW_SKETCH_BUILD_DIR)/demo-sketch.elf

$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.bin:	$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.elf $(ISHW_ALL_DEP)
	@$(CROSS_OBJCOPY) $(ISHW_CROSS_OBJCOPYFLAGS) $< $@ >$@.log
	@echo Built $@

$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.elf:	$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.o zpu-core-library $(ISHW_CROSS_LD_SOURCES) $(ISHW_ALL_DEP)
	@$(CROSS_CPP) $(ISHW_CROSS_LDFLAGS) -Wl,-Map,$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.map -o $@ -L$(ISHW_CROSS_BUILD_DIR) -lm -Wl,--warn-common -Wl,--warn-section-align -Wl,--whole-archive $< $(ISHW_CROSS_BUILD_DIR)/libzpucore.a -Wl,--no-whole-archive >$@.log
	@echo Built $@

$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.o:	$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.cpp $(ISHW_ALL_DEP)
	@$(CROSS_CPP) -c $(ISHW_CROSS_CPPFLAGS) $(ISHW_CROSS_INCLUDES) $< -o $@ >$@.log
	@echo Built $@

$(ISHW_SKETCH_BUILD_DIR)/demo-sketch.cpp: $(ISHW_SKETCH_BUILD_DIR)/.exists $(ISHW_ALL_DEP)
	@echo -e '#include <Arduino.h>\nvoid setup() { Serial.begin(115200); }\nvoid loop() { static int count = 0; Serial.print("ds' `date +%s` '"); Serial.println(++count); }\n' > $(ISHW_SKETCH_BUILD_DIR)/demo-sketch.cpp
	@echo Built $@

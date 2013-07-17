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
ISHW_TARGETS_HELP+="make my-sketch\n\tbuild a given sketch with 'MY_SKETCH=/path/to/sketch.cpp make my-sketch\n"
ISHW_TARGETS_HELP+="make my-sketch-dir\n\tbuild a given sketch dir with 'MY_SKETCH_DIR=/path/to/sketchdir make my-sketch-dir\n"
ISHW_TARGETS_HELP+="make sim-my-sketch\n\tbuild and simulate a given sketch with 'MY_SKETCH=/path/to/sketch.cpp make sim-my-sketch\n"
ISHW_TARGETS_HELP+="make download-demo-sketch\n\tdownload the demo sketch to test the downloading system\n"
ISHW_TARGETS_HELP+="make download-mfm-sketch\n\tdownload the mfm sketch to test the downloading system\n"
ISHW_TARGETS_HELP+="make download-my-sketch\n\tbuild and download a given sketch with 'MY_SKETCH=/path/to/sketch.cpp make download-my-sketch\n"
ISHW_TARGETS_HELP+="make download-bin-file\n\tdownload a bin file with 'BIN_FILE_NAME=/path/to/binfile make download-bin-file'\n"
ISHW_TARGETS_HELP+="make mfm\n\tbuild the basic MFM execution environment\n"

_MFM_SKETCH_BUILD_DIR:=$(ISHW_BUILD_BASE_DIR)/components/mfm/sketch
_MFM.SKETCHES:=demo-sketch mfm-sketch 

check-mfm-path:	check-os-path

report-mfm-path:	check-mfm-path
	@echo MFM:

mfm:	FORCE
	@echo "Target $@ not yet implemented"
	@exit 99

my-sketch:	copy-my-sketch  $(_MFM_SKETCH_BUILD_DIR)/libmysketch.a

copy-my-sketch:	init-mySketch.cpp
	@if [ "x$(MY_SKETCH)" = "x" -o ! -r "$(MY_SKETCH)" ]       ;\
	then echo 'MY_SKETCH ($(MY_SKETCH)) unset or unreadable'   ;\
	exit 9 ; fi
	cat $(MY_SKETCH) >>$(_MFM_SKETCH_BUILD_DIR)/mySketch.cpp

init-mySketch.cpp:	FORCE
	@rm -rf $(_MFM_SKETCH_BUILD_DIR)                                      
	@mkdir -p $(_MFM_SKETCH_BUILD_DIR)                                     
	@echo "#include <Arduino.h>" > $(_MFM_SKETCH_BUILD_DIR)/mySketch.cpp

my-sketch-dir:	copy-my-sketch-dir $(_MFM_SKETCH_BUILD_DIR)/libmysketch.a

copy-my-sketch-dir:	init-mySketch.cpp
	@if [ "x$(MY_SKETCH_DIR)" = "x" -o ! -d "$(MY_SKETCH_DIR)" ]           ;\
	then echo 'MY_SKETCH_DIR ($(MY_SKETCH_DIR)) unset or not a directory'  ;\
	exit 9 ; fi
	@cat $(MY_SKETCH_DIR)/*.ino >>$(_MFM_SKETCH_BUILD_DIR)/mySketch.cpp
	shopt -s nullglob;for f in $(MY_SKETCH_DIR)/*.c $(MY_SKETCH_DIR)/*.cpp $(MY_SKETCH_DIR)/*.h ;do \
	cp $$f $(_MFM_SKETCH_BUILD_DIR); done

$(_MFM_SKETCH_BUILD_DIR)/libmysketch.a:	FORCE
	cd $(_MFM_SKETCH_BUILD_DIR) ;\
	CPPS=`echo *.cpp`;\
	OBJCPPS=;\
	if [ "$$CPPS" != "*.cpp" ] ;\
	then \
	  $(CROSS_CPP) -c $(ISHW_CROSS_CPPFLAGS) $(ISHW_CROSS_INCLUDES) $$CPPS ;\
	  for s in $$CPPS ; do echo "EE($$s)";OBJCPPS="$$OBJCPPS $${s%.cpp}.o" ; done \
	fi ;\
	CS=`echo *.c` ;\
	OBJCS= ;\
	if [ "$$CS" != "*.c" ] ;\
	then \
	  $(CROSS_GCC) -c $(ISHW_CROSS_CFLAGS) $(ISHW_CROSS_INCLUDES) $$CS ;\
	  for s in $$CS ; do echo "DOC($$s)"; OBJCS="$$OBJCS $${s%.c}.o" ; done \
	fi ;\
	OBJS="$$OBJCPPS $$OBJCS" ;\
	echo "OBJS=$$OBJS" ;\
	$(CROSS_AR) $(ISHW_CROSS_ARFLAGS) $@ $$OBJS >$@.log
	@echo Built $@

sim-my-sketch:	my-sketch
	make fpga-sim-mySketch

sim-my-sketch-dir:	my-sketch-dir
	make fpga-sim-mySketch

download-my-sketch:	my-sketch
	make download-mySketch

download-my-sketch-dir:	my-sketch-dir
	make download-mySketch

download-%:	$(_MFM_SKETCH_BUILD_DIR)/%.bin 
	BIN_FILE_NAME=$< make download-bin-file

fpga-sim-%:	$(_MFM_SKETCH_BUILD_DIR)/%.bin 
	BIN_FILE_NAME=$< make run-fpga-sim

fpga-build-%:	$(_MFM_SKETCH_BUILD_DIR)/%.bin 
	BIN_FILE_NAME=$< make build-fpga-sim

.PHONY:	$(_MFM.SKETCHES)

.SECONDEXPANSION:
$(_MFM.SKETCHES):	$(_MFM_SKETCH_BUILD_DIR)/$$@.bin $$@-size

%-sketch-size:	$(_MFM_SKETCH_BUILD_DIR)/%-sketch.elf
	@$(CROSS_SIZE) $<

$(_MFM_SKETCH_BUILD_DIR)/%.bin:	$(_MFM_SKETCH_BUILD_DIR)/%.elf $(ISHW_ALL_DEP)
	@$(CROSS_OBJCOPY) $(ISHW_CROSS_OBJCOPYFLAGS) $< $@ >$@.log
	@echo Built $@

$(_MFM_SKETCH_BUILD_DIR)/%.elf:	$(_MFM_SKETCH_BUILD_DIR)/libmysketch.a zpu-core-library $(ISHW_CROSS_LD_SOURCES) $(ISHW_ALL_DEP)
	$(CROSS_CPP) $(ISHW_CROSS_LDFLAGS) -Wl,-Map,$(patsubst %.elf,%.map,$@) -o $@ -L$(ISHW_CROSS_BUILD_DIR) -lm -Wl,--warn-common -Wl,--warn-section-align -Wl,--whole-archive $< $(ISHW_CROSS_BUILD_DIR)/libzpucore.a -Wl,--no-whole-archive >$@.log
	@echo Built $@

$(_MFM_SKETCH_BUILD_DIR)/%.o:	$(_MFM_SKETCH_BUILD_DIR)/%.cpp $(ISHW_ALL_DEP)
	@$(CROSS_CPP) -c $(ISHW_CROSS_CPPFLAGS) $(ISHW_CROSS_INCLUDES) $< -o $@ >$@.log
	@echo Built $@

$(_MFM_SKETCH_BUILD_DIR)/%.o:	$(_MFM_SKETCH_BUILD_DIR)/%.c $(ISHW_ALL_DEP)
	@$(CROSS_GCC) -c $(ISHW_CROSS_CFLAGS) $(ISHW_CROSS_INCLUDES) $< -o $@ >$@.log
	@echo Built $@

$(_MFM_SKETCH_BUILD_DIR)/demo-sketch.cpp: $(_MFM_SKETCH_BUILD_DIR)/.exists $(ISHW_ALL_DEP)
	@echo -e '#include <Arduino.h>\nvoid setup() { Serial.begin(115200); }\nvoid loop() { static int count = 0; Serial.print("ds' `date +%s` '"); Serial.println(++count); }\n' > $@
	@echo Built $@

$(_MFM_SKETCH_BUILD_DIR)/mfm-sketch.cpp: $(_MFM_SKETCH_BUILD_DIR)/.exists $(ISHW_ALL_DEP)
	@echo -e '#include <MFM.h>\nvoid setup() { Serial.begin(115200); }\nvoid loop() { static int count = 0; Serial.print("mfm' `date +%s` '"); Serial.println(++count); }\n' > $@
	@echo Built $@

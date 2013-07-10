###
# First, capture this directory path
_OS_DIR:=$(ISHW_THIS_DIR)

###
# Every config.mk file SHOULD add some text to ISHW_TARGETS_HELP
# suggesting make targets are available and what they might do

ISHW_TARGETS_HELP+="\nOS TARGETS\n"

check-os-path:	check-tiles-path

report-os-path:	check-os-path
	@echo OS: Cross toolchain prefix in use: $(ISHW_CROSS_TOOLCHAIN_PREFIX)

include $(_OS_DIR)/src/config.mk


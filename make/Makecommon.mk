# Export everything to everybody.  We just can't shut up.
#export

ISHW_CONFIG_DIR:=$(ISHW_BASE_DIR)/config

ISHW_COMPONENT_DIR:=$(ISHW_BASE_DIR)/components

## Find the key pieces of the build system
ISHW_ALL_DEP:=$(realpath $(shell find .. -type f -name 'config.mk' -o -name 'Makefile*'))
#ISHW_ALL_DEP:=$(wildcard $(ISHW_MAKE_DIR)/*.mk) $(wildcard $(ISHW_COMPONENT_DIR)/*/config.mk)

ISHW_COMPONENTS:=$(notdir $(wildcard $(ISHW_COMPONENT_DIR)/*))
ISHW_CHECK_TARGETS:=$(patsubst %,check-%-path,$(ISHW_COMPONENTS))
ISHW_REPORT_TARGETS:=$(patsubst %,report-%-path,$(ISHW_COMPONENTS))

ISHW_BUILD_BASE_DIR:=$(ISHW_BASE_DIR)/build/$(CONFIG_TILE_TYPE)

# Shared var inits
ISHW_TARGETS_HELP:=""

all:	default-task

default-task:	help

help:	FORCE
	@echo '------------------------------------------------'
	@echo 'ISHW: Indefinitely Scalable Hardware Make System'
	@echo ''
	@echo 'Top-level targets:'
	@echo ''
	@echo '  make help     -- get help with ISHW'
	@echo '  make check    -- check that your ISHW is configured plausibly'
	@echo '  make report   -- print some configuration info (implies check)'
	@echo ''
	@echo 'Component-specific target help:'
	@echo -e $(ISHW_TARGETS_HELP)

include $(wildcard $(ISHW_COMPONENT_DIR)/*/config.mk)

.PHONY:	FORCE

# Checking and reporting configuration info

check:	$(ISHW_CHECK_TARGETS)
	@echo Paths look OK

report:	$(ISHW_REPORT_TARGETS)
	@echo Component configurations look plausible

#init-build:	FORCE
#	@mkdir -p $(ISHW_BUILD_BASE_DIR)

# Pattern rule to establish build directories
$(ISHW_BUILD_BASE_DIR)/%/.exists:
	@echo "[Creating directory $(dir $@)]"
	@mkdir -p $(dir $@)
	@date > $@

# Pattern rule to build empty directories
$(ISHW_BUILD_BASE_DIR)/%/.new:
	@echo "[Recreating directory $(dir $@)]"
	@rm -rf $(dir $@)
	@mkdir -p $(dir $@)
	@date > $@

# After we make dir flag files, let's keep them
.PRECIOUS:	$(ISHW_BUILD_BASE_DIR)/%/.exists

CROSS_GCC=$(ISHW_CROSS_TOOLCHAIN_PREFIX)gcc
CROSS_AS=$(ISHW_CROSS_TOOLCHAIN_PREFIX)gcc
CROSS_CPP=$(ISHW_CROSS_TOOLCHAIN_PREFIX)c++
CROSS_AR=$(ISHW_CROSS_TOOLCHAIN_PREFIX)ar
CROSS_OBJCOPY=$(ISHW_CROSS_TOOLCHAIN_PREFIX)objcopy
CROSS_SIZE=$(ISHW_CROSS_TOOLCHAIN_PREFIX)size

# Pattern rules for building stuff
.SECONDEXPANSION:
$(ISHW_BUILD_BASE_DIR)/%.o:	$(ISHW_BASE_DIR)/%.c $$(@D)/.exists $(ISHW_ALL_DEP)
	@$(CROSS_GCC) -c $(ISHW_CROSS_CFLAGS) $(ISHW_CROSS_INCLUDES) $< -o $@

.SECONDEXPANSION:
$(ISHW_BUILD_BASE_DIR)/%.o:	$(ISHW_BASE_DIR)/%.S $$(@D)/.exists $(ISHW_ALL_DEP)
	@$(CROSS_AS) -c $(ISHW_CROSS_ASMFLAGS) $(ISHW_CROSS_INCLUDES) $< -o $@

.SECONDEXPANSION:
$(ISHW_BUILD_BASE_DIR)/%.o:	$(ISHW_BASE_DIR)/%.cpp $$(@D)/.exists $(ISHW_ALL_DEP)
	$(CROSS_CPP) -c $(ISHW_CROSS_CPPFLAGS) $(ISHW_CROSS_INCLUDES) $< -o $@

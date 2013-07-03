ISHW_CONFIG_DIR:=$(ISHW_BASE_DIR)/config

ISHW_COMPONENT_DIR:=$(ISHW_BASE_DIR)/components
ISHW_COMPONENTS:=$(notdir $(wildcard $(ISHW_COMPONENT_DIR)/*))
ISHW_CHECK_TARGETS:=$(patsubst %,check-%-path,$(ISHW_COMPONENTS))
ISHW_REPORT_TARGETS:=$(patsubst %,report-%-path,$(ISHW_COMPONENTS))

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
	@echo $(ISHW_TARGETS_HELP)

include $(wildcard $(ISHW_COMPONENT_DIR)/*/config.mk)

.PHONY:	FORCE

# Checking and reporting configuration info

check:	$(ISHW_CHECK_TARGETS)
	@echo Paths look OK

report:	$(ISHW_REPORT_TARGETS)
	@echo Component configurations look plausible


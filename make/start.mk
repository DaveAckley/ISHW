## Find our face
ISHW_MAKE_DIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
ISHW_BASE_DIR:=$(realpath $(ISHW_MAKE_DIR)/..)
ISHW_CONFIG_DIR:=$(ISHW_BASE_DIR)/config

## Get the default local configuration
include $(ISHW_CONFIG_DIR)/DEFAULT.mk

## Do the work
include $(ISHW_MAKE_DIR)/Makecommon.mk

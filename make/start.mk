## Let's not screw around with sh.  For example, the Xilinx settings
## scripts seem to die with sh, for some reason.
SHELL:=/bin/bash

## This delayed evaluation rule, when evaluated BEFORE ANY INCLUDES in
## a makefile, gives the directory of the makefile it was evaluated
## in.  In general it should be used only on the right hand side of :=
## variable assignments (see e.g. ISHW_MAKE_DIR, below), so that its
## value is forced at clearly understood times.
ISHW_THIS_DIR=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))

## Capture our own directory path
_MAKE_DIR:=$(ISHW_THIS_DIR)

## Initialize global variables
include $(_MAKE_DIR)/Makeglobals.mk

## Find our face
ISHW_MAKE_DIR:=$(ISHW_THIS_DIR)
ISHW_BASE_DIR:=$(realpath $(ISHW_MAKE_DIR)/..)
ISHW_CONFIG_DIR:=$(ISHW_BASE_DIR)/config

## Get the default local configuration
include $(ISHW_CONFIG_DIR)/DEFAULT.mk

## Do the work
include $(ISHW_MAKE_DIR)/Makecommon.mk

# Set our name
PROJECT_NAME:=$(notdir $(CURDIR))

# Find our directory no matter where we were run from
_DIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# Do the thing
include $(_DIR)/make/start.mk


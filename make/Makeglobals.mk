###
# List of variables that are (notionally) global and shared among all
# config.mk files.  All must agree on the purpose of these variables,
# and not poach or modify them outside of the appropriate jurisdiction

##
# What: Text to print during 'make help'.
# Written by: All, append
# Used by: make/Makecommon.mk
ISHW_TARGETS_HELP:=''     

##
# Targets that _ALL_ make targets should depend on
ISHW_ALL_DEP:=''     

##
# What: Path to make files
# Written by: start.mk
# Used by: Any as needed
ISHW_MAKE_DIR:=''
ISHW_BASE_DIR:=''
ISHW_CONFIG_DIR:=''

###
# List of variables that are (notionally) global and shared among all
# config.mk files.  All must agree on the purpose of these variables,
# and not poach or modify them outside of the appropriate jurisdiction

##
# What: Path to the entire ISHW tree
# Written by: start.mk
# Used by: Any as needed
ISHW_BASE_DIR:=

##
# What: Paths to various major subdirectories
# Written by: start.mk
# Used by: Any as needed
ISHW_MAKE_DIR:=
ISHW_CONFIG_DIR:=

##
# What: Text to print during 'make help'.
# Written by: All, append
# Used by: make/Makecommon.mk
ISHW_TARGETS_HELP:=    

##
# What: Targets that all make targets should depend on, so that any
#       change in any of these cause basically everything to get
#       rebuilt.  This is the 'nuke it from orbit' option, basically
#       used for the make system
#       files themselves.
# Written by: make/Makecommon.mk
# Used by: all
ISHW_ALL_DEP:=  

##
# What: String to place before 'gcc', 'g++', etc, to form the full
#       path to the cross-toolchain tools
# Written by: components/tile/$(CONFIG_TILE_TYPE)/config.mk
# Used by: Any as needed
ISHW_CROSS_TOOLCHAIN_PREFIX:=

##
# What: Various typical compilation flags variables
# Written by: components/tile/$(CONFIG_TILE_TYPE)/config.mk or as needed
# Used by: Any as needed
ISHW_CROSS_CFLAGS:=
ISHW_CROSS_INCLUDES:=
ISHW_CROSS_ASMFLAGS:=
ISHW_CROSS_CPPFLAGS:=
ISHW_CROSS_DEFINES:=

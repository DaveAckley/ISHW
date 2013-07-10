###
# First, capture this directory path
_OS_SRC.DIR:=$(ISHW_THIS_DIR)

###
# Every config.mk file SHOULD add some text to ISHW_TARGETS_HELP
# suggesting make targets are available and what they might do.  
# Other ISHW_-level variables may need customization as well (see
# make/Makeglobals.mk)

ISHW_TARGETS_HELP+="make build-os\n\tbuild the OS library archive\n"

ISHW_CROSS_INCLUDES+=-I$(_OS_INCLUDE_DIR)

###
# This file MAY set up any other variables and targets that are useful
# for internal operations within this subtree.  Any such internal
# variables and targets MUST use a prefix formed by '_' plus the path
# below 'components' (e.g., _tiles_zpuino_foo, _TILES_ZPUINO_BAR)

_OS_SRC.DIR:=$(ISHW_THIS_DIR)
_OS_SRC.BUILD_DIR:=$(ISHW_BUILD_BASE_DIR)/components/os/src
_OS_SRC.INCLUDE_DIR:=$(realpath $(_OS_SRC.DIR)/../include)

#$(warning ISHW_CROSS_INCLUDES+=$(ISHW_CROSS_INCLUDES))

build-os:	$(_OS_SRC.BUILD_DIR)/libreflex.a

CPPFLAGS+=$(CORE_EXTRA_CPPFLAGS) -I$(_OS_SRC.INCLUDE_DIR) -I$(ISHW_TILE_INCLUDE_DIR)
CFLAGS+=$(CORE_EXTRA_CFLAGS) -I$(_OS_SRC.INCLUDE_DIR) -I$(ISHW_TILE_INCLUDE_DIR)

_OS_SRC.SRCFILES:=$(wildcard $(_OS_SRC.DIR)/*.cpp $(_OS_SRC.DIR)/*.c)
#$(warning "SRC" _OS_SRC.SRCFILES:=$(_OS_SRC.SRCFILES))

_OS_SRC.HDRFILES:=$(_OS_SRC.SRCFILES)
_OS_SRC.HDRFILES:=$(patsubst $(_OS_SRC.DIR)/%.cpp, $(_OS_SRC.INCLUDE_DIR)/%.h, $(_OS_SRC.HDRFILES))
_OS_SRC.HDRFILES:=$(patsubst $(_OS_SRC.DIR)/%.c, $(_OS_SRC.INCLUDE_DIR)/%.h, $(_OS_SRC.HDRFILES))
#$(warning "Sh" hdrFILES:=$(_OS_SRC.HDRFILES))

_OS_SRC.OBJDIR:=$(_OS_SRC.BUILD_DIR)

_OS_SRC.OBJFILES:=$(_OS_SRC.SRCFILES)
_OS_SRC.OBJFILES:=$(patsubst $(_OS_SRC.DIR)/%.cpp, $(_OS_SRC.OBJDIR)/%.o, $(_OS_SRC.OBJFILES))
_OS_SRC.OBJFILES:=$(patsubst $(_OS_SRC.DIR)/%.c, $(_OS_SRC.OBJDIR)/%.o, $(_OS_SRC.OBJFILES))
#$(warning "So" _OS_SRC.OBJFILES:=$(_OS_SRC.OBJFILES))

$(_OS_SRC.BUILD_DIR)/libreflex.a:	$(_OS_SRC.BUILD_DIR)/.exists  $(_OS_SRC.OBJFILES) 
	$(CROSS_AR) ruc "$@" $(_OS_SRC.OBJFILES)

# testPacketBuffer:	FORCE
# 	@rm -f SFBPacketBuffer.o
# 	DEFINES=-DTEST_PACKET_BUFFER make -k
# 	$(GCC) -o"$@" SFBPacketBuffer.o SFBUtils.o
# 	./"$@"

# testWeaksum:	FORCE
# 	@rm -f SFBWeaksum.o
# 	DEFINES=-DTEST_WEAKSUM make -k
# 	$(GCC) -o"$@" SFBWeaksum.o
# 	./"$@"


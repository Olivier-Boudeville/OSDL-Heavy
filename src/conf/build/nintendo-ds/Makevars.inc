# General variables for the cross-compilation of Nintendo DS binaries
# from GNU/Linux.

# Automatically included by Makerules.nds.inc.

# The NDS_DEV_ROOT variable can be set outside of these Makefiles.
# Either through a shell variable once the local definition has been commented
# out, ex: 
# export NDS_DEV_ROOT=$LOANI_INSTALLATIONS/Nintendo-DS-development
# or as a Make variable (no commenting out needed), ex:
# make NDS_DEV_ROOT=$LOANI_INSTALLATIONS/Nintendo-DS-development
# or directly hardcoded here:
# NDS_DEV_ROOT = $(LOANI_INSTALLATIONS)/Nintendo-DS-development
# Otherwise it will be guessed from LOANI-enforced directory layout:
LOANI_INSTALLATIONS = $(TOP)/../../../../LOANI-installations
NDS_DEV_ROOT        = $(LOANI_INSTALLATIONS)/Nintendo-DS-development


# Safety check:
ifeq ($(strip $(NDS_DEV_ROOT)),)
$(error Please set the NDS_DEV_ROOT variable)
endif


# Tool section.

devkitPro_DIR = $(NDS_DEV_ROOT)/devkitPro
devkitARM_DIR = $(devkitPro_DIR)/devkitARM
libnds_DIR    = $(devkitPro_DIR)/libnds
libfat_DIR    = $(libnds_DIR)

ceylan_used_version = 0.7
ceylan_DIR          = $(LOANI_INSTALLATIONS)/Ceylan-$(ceylan_used_version)

build_tool_DIR    = ${devkitARM_DIR}/bin
build_tool_PREFIX =arm-eabi-

ADDR2LINE = $(build_tool_DIR)/$(build_tool_PREFIX)addr2line
AR        = $(build_tool_DIR)/$(build_tool_PREFIX)ar
AS        = $(build_tool_DIR)/$(build_tool_PREFIX)as
CC        = $(build_tool_DIR)/$(build_tool_PREFIX)gcc
CXX       = $(build_tool_DIR)/$(build_tool_PREFIX)g++
LD        = $(build_tool_DIR)/$(build_tool_PREFIX)ld
NM        = $(build_tool_DIR)/$(build_tool_PREFIX)nm
OBJCOPY   = $(build_tool_DIR)/$(build_tool_PREFIX)objcopy
OBJDUMP   = $(build_tool_DIR)/$(build_tool_PREFIX)objdump
RANLIB    = $(build_tool_DIR)/$(build_tool_PREFIX)ranlib
STRIP     = $(build_tool_DIR)/$(build_tool_PREFIX)strip


# The version installed by LOANI is preferred here:
DLDITOOL_ROOT   = $(NDS_DEV_ROOT)/dldi
DLDITOOL        = $(DLDITOOL_ROOT)/dlditool

# Replace by your linker-specific script if not using SuperCard Lite:
DLDITOOL_SCRIPT = $(DLDITOOL_SC_SCRIPT)

DLDITOOL_SC_SCRIPT   = $(DLDITOOL_ROOT)/Sclt.dldi
DLDITOOL_R4_SCRIPT   = $(DLDITOOL_ROOT)/R4tf.dldi
DLDITOOL_FCSR_SCRIPT = $(DLDITOOL_ROOT)/fcsr.dldi



NDSTOOL = $(build_tool_DIR)/ndstool
DSBUILD = $(build_tool_DIR)/dsbuild

NO_CASH_GBA = $(NDS_DEV_ROOT)/NoCashGBA-w/NoCashGBA.EXE
DESMUME     = $(NDS_DEV_ROOT)/DeSmuME-0.4.0/NDeSmuME.exe

WINE = wine

# Location of the removable media where ROM can be transferred:
#TRANSFER_MOUNT_POINT = /media/disk
TRANSFER_MOUNT_POINT = /media/DS-homebrew



# Module settings.


OSDL_CODE_ROOT := $(TOP)/src/code

OSDL_AUDIO                 := $(OSDL_CODE_ROOT)/audio
OSDL_AUDIO_RENDERER        := $(OSDL_AUDIO)/audioRenderer

OSDL_BASIC                 := $(OSDL_CODE_ROOT)/basic
OSDL_ENGINE                := $(OSDL_CODE_ROOT)/engine
OSDL_EVENTS                := $(OSDL_CODE_ROOT)/events

OSDL_VIDEO                 := $(OSDL_CODE_ROOT)/video
OSDL_VIDEO_TWO_DIMENSIONAL := $(OSDL_VIDEO)/twoDimensional
OSDL_VIDEO_RENDERER        := $(OSDL_VIDEO)/videoRenderer



OSDL_CODE_ROOT_INC             := -I$(OSDL_CODE_ROOT)

OSDL_LOGS_INC                  := -I$(OSDL_LOGS)

OSDL_AUDIO_INC                 := -I$(OSDL_AUDIO)
OSDL_AUDIO_RENDERER_INC        := -I$(OSDL_AUDIO_RENDERER)

OSDL_BASIC_INC                 := -I$(OSDL_BASIC)
OSDL_ENGINE_INC                := -I$(OSDL_ENGINE)
OSDL_EVENTS_INC                := -I$(OSDL_EVENTS)

OSDL_VIDEO_INC                 := -I$(OSDL_VIDEO)
OSDL_VIDEO_TWO_DIMENSIONAL_INC := -I$(OSDL_VIDEO_TWO_DIMENSIONAL)
OSDL_VIDEO_RENDERER_INC        := -I$(OSDL_VIDEO_RENDERER)



# For OSDL versions and al:
include $(TOP)/src/conf/OSDLSettings.inc

OSDL_LT_CURRENT := $(OSDL_MAJOR_VERSION)
OSDL_LT_REVISION:= $(OSDL_MINOR_VERSION)
OSDL_LT_AGE     := $(shell expr $(OSDL_MAJOR_VERSION) - $(OSDL_MAJOR_ANCESTOR))

OSDL_VERSION:= $(OSDL_LT_CURRENT).$(OSDL_LT_REVISION)
OSDL_LT_VERSION:= $(OSDL_LT_CURRENT).$(OSDL_LT_REVISION).$(OSDL_LT_AGE)


# Architecture-related flags.

ARCH_FLAGS_COMMON = -mthumb -mthumb-interwork

# The -DARM7/-DARM flags are needed by libnds but set by OSDL DS header:
ARCH_FLAGS_ARM7   = $(ARCH_FLAGS_COMMON) -mcpu=arm7tdmi -mtune=arm7tdmi -DOSDL_RUNS_ON_ARM7=1

ARCH_FLAGS_ARM9   = $(ARCH_FLAGS_COMMON) -march=armv5te -mtune=arm946e-s -DOSDL_RUNS_ON_ARM9=1


# Determines which kind of build is expected:
#OSDL_BUILD_MODE = debug
OSDL_BUILD_MODE = release


# OSDL library names.

OSDL_BASE_LIB_NAME:= OSDL-$(OSDL_VERSION)

ifeq ($(OSDL_BUILD_MODE),debug)
	OSDL_BASE_LIB_NAME:= $(OSDL_BASE_LIB_NAME)-debug
else
	OSDL_BASE_LIB_NAME:= $(OSDL_BASE_LIB_NAME)-release
endif

OSDL_ARM7_LINK_NAME:= $(OSDL_BASE_LIB_NAME)-arm7
OSDL_ARM9_LINK_NAME:= $(OSDL_BASE_LIB_NAME)-arm9


OSDL_ARM7_LIB_NAME:= lib$(OSDL_ARM7_LINK_NAME).a
OSDL_ARM9_LIB_NAME:= lib$(OSDL_ARM9_LINK_NAME).a

OSDL_LIB_LOCATION:= $(TOP)/src/code

OSDL_ARM7_LIB_FULL_PATH:=$(OSDL_LIB_LOCATION)/$(OSDL_ARM7_LIB_NAME)
OSDL_ARM9_LIB_FULL_PATH:=$(OSDL_LIB_LOCATION)/$(OSDL_ARM9_LIB_NAME)


# Preprocessor flags.

CPP_DEFINES = \
	$(CPP_ARCH_DEFINES)    \
	$(CPP_FEATURE_DEFINES) \
	$(CPP_DEBUG_DEFINES)   \
	$(CPP_VERBOSE_DEFINES) \
	$(CPP_OTHER_DEFINES)
	

CPP_ARCH_DEFINES:= \
	-DOSDL_ARCH_NINTENDO_DS=1      \
	-DOSDL_ARCH_WINDOWS=0          \
	-DOSDL_ARCH_MACOSX=0           \
	-DOSDL_RUNS_ON_LITTLE_ENDIAN=1 


# Defined here rather than in OSDLConfigForNintendoDS.h as headers may mention
# these includes (ex: '#include "SDL.h"' for a necessary typedef):
CPP_FEATURE_DEFINES:= \
	-DOSDL_USES_LIBPNG=0    \
	-DOSDL_USES_OPENGL=0    \
	-DOSDL_USES_SDL=0       \
	-DOSDL_USES_SDL_GFX=0   \
	-DOSDL_USES_SDL_IMAGE=0 \
	-DOSDL_USES_SDL_TTF=0
		
		
CPP_DEBUG_DEFINES:= \
	-DOSDL_DEBUG_NINTENDO_DS=1       \
	-DOSDL_DEBUG=0                   \
	-DOSDL_DEBUG_AUDIO_RENDERER=0    \
	-DOSDL_DEBUG_BLIT=0              \
	-DOSDL_DEBUG_BOUNDING_BOX=0      \
	-DOSDL_DEBUG_BOUNDING_BOX=0      \
	-DOSDL_DEBUG_CACHED_STATES=0     \
	-DOSDL_DEBUG_COLOR=0             \
	-DOSDL_DEBUG_CONICS=0            \
	-DOSDL_DEBUG_EVENTS=0            \
	-DOSDL_DEBUG_FONT=0              \
	-DOSDL_DEBUG_IMAGE=0             \
	-DOSDL_DEBUG_PALETTE=0           \
	-DOSDL_DEBUG_PIXEL=0             \
	-DOSDL_DEBUG_RECTANGLES=0        \
	-DOSDL_DEBUG_RENDERER=0          \
	-DOSDL_DEBUG_SCHEDULER=0         \
	-DOSDL_DEBUG_SPRITE=0            \
	-DOSDL_DEBUG_SURFACE=0           \
	-DOSDL_DEBUG_VIDEO=0             \
	-DOSDL_DEBUG_VIDEO_RENDERER=0    \
	-DOSDL_DEBUG_WIDGET=0
	
	
CPP_VERBOSE_DEFINES	:= \
	-DOSDL_VERBOSE_CONTROLLER=0       \
	-DOSDL_VERBOSE_JOYSTICK=0         \
	-DOSDL_VERBOSE_JOYSTICK_HANDLER=0 \
	-DOSDL_VERBOSE_KEYBOARD_HANDLER=0 \
	-DOSDL_VERBOSE_MOUSE=0            \
	-DOSDL_VERBOSE_MOUSE_HANDLER=0


CPP_OTHER_DEFINES:= \
	-DCEYLAN_DLL=                                    \
	-DOSDL_DLL=                                      \
	-DOSDL_LIBTOOL_VERSION="\"$(OSDL_LT_VERSION)\""  \
	-DOSDL_COUNT_INSTANCES=0                         \
	-DOSDL_CACHE_OVERALL_SETTINGS


CPP_FLAGS_COMMON:= $(CPP_DEFINES) -I. $(CPP_FLAGS_COMMON)  \
	-I$(ceylan_DIR)/include/Ceylan -I$(libfat_DIR)/include \
	-I$(libnds_DIR)/include  


# Let the possibility for the user to define his own CPP_FLAGS:
CPP_FLAGS_ARM7  := $(CPP_FLAGS) $(CPP_FLAGS_COMMON)
CPP_FLAGS_ARM9  := $(CPP_FLAGS) $(CPP_FLAGS_COMMON)



# C++ Compile flags.

# Compile flags always used, for both ARM, both in debug and release modes:
CXX_FLAGS_COMMON:= -Winline -Wall -Werror -Woverloaded-virtual -W -Wfloat-equal -Wundef -Wendif-labels -Wshadow -Wpointer-arith -Wcast-qual -Wwrite-strings -Wconversion -Wsign-compare -Wmissing-format-attribute -Wredundant-decls -Wno-unused-parameter -Wno-unknown-pragmas -Wno-long-long

# Removed: 
#   -Wcast-align: too many warnings due to STL casts
#   -Wpacked: too many warnings about 'packed attribute causes inefficient
# alignment' in libnds headers


ifeq ($(OSDL_BUILD_MODE),debug)

# Flags common to the ARM7 and the ARM9 for compilation in debug mode:
CXX_FLAGS_COMMON:= $(CXX_FLAGS_COMMON) -g -O0 -DOSDL_DEBUG=1

else

# Flags common to the ARM7 and the ARM9 for compilation in release mode:
CXX_FLAGS_COMMON:= $(CXX_FLAGS_COMMON) -O3 -fomit-frame-pointer -ffast-math -DOSDL_DEBUG=0

endif


# Let the possibility for the user to define his own CXX_FLAGS:
CXX_FLAGS_ARM7  := $(CXX_FLAGS) $(CXX_FLAGS_COMMON) $(ARCH_FLAGS_ARM7)
CXX_FLAGS_ARM9  := $(CXX_FLAGS) $(CXX_FLAGS_COMMON) $(ARCH_FLAGS_ARM9)




# C++ Link flags.


# Generating a library:

AR_FLAGS_COMMON:= -rc

AR_FLAGS_ARM7:= $(AR_FLAGS_COMMON)
AR_FLAGS_ARM9:= $(AR_FLAGS_COMMON)


# Generating an executable is defined in trunk/test/cross-tests/GNUmakefile.



# Install section.

OSDL_INSTALL_ROOT := $(NDS_DEV_ROOT)/OSDL-$(OSDL_VERSION)
OSDL_INSTALL_INC  := $(OSDL_INSTALL_ROOT)/include
OSDL_INSTALL_LIB  := $(OSDL_INSTALL_ROOT)/lib

OSDL_ARM7_INSTALLED_LIB := $(OSDL_INSTALL_LIB)/$(OSDL_ARM7_LIB_NAME)
OSDL_ARM9_INSTALLED_LIB := $(OSDL_INSTALL_LIB)/$(OSDL_ARM9_LIB_NAME)


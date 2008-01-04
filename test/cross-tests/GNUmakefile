# This is a simple Makefile (not a Makefile.cross, not using overall 
# Makerules.inc rules so that tests are completely uncoupled from the 
# OSDL library).

# This way, these tests can be directly used as guidelines to build 
# one's application using the OSDL library.

TOP = ../..


.PHONY: test emulate emulate-with-NoCashGBA emulate-with-DeSmuME transfer \
	patch umount export clean info info-target info-flags info-debug info-run


CROSS_TARGET=nintendo-ds

# For tool locations, not for flags nor build rules:
include $(TOP)/src/conf/build/$(CROSS_TARGET)/Makevars.inc


# Test target section.


# Target linker can be sc (SuperCard) or r4 (R4 DS):
TARGET_LINKER = r4

TEST_PREFIX= $(OSDL_TESTED_VERSION)-$(OSDL_TESTED_BUILD_MODE)
TEST_EXEC_EXTENSION = $(TARGET_LINKER).nds

TEST_ARM9_SOURCES = $(wildcard *.arm9.cc)


#TEST_TARGETS = $(patsubst %.arm9.cc,%-$(TEST_PREFIX)-patched.$(TEST_EXEC_EXTENSION),$(TEST_ARM9_SOURCES))
TEST_TARGETS = testOSDLMusic-0.5-release-patched.$(TARGET_LINKER).nds

# Some are patched, the others not:
#TESTS_TO_TRANSFER = $(wildcard *.$(CURRENT_LINKER).nds)

RESOURCES = OSDL.osdl.sound

FILES_TO_TRANSFER = $(TEST_TARGETS) $(RESOURCES)

OSDL_TESTED_VERSION    = $(OSDL_VERSION)
OSDL_TESTED_BUILD_MODE = $(OSDL_BUILD_MODE)

# Use 'make emulate TEST_NAME=testOSDLTextConsole' for example to override:
TEST_NAME = testOSDLMusic

TEST_FULL_NAME = $(TEST_NAME)-$(TEST_PREFIX)


all: $(TEST_TARGETS)


test: emulate transfer


emulate: emulate-with-NoCashGBA 


emulate-with-NoCashGBA: $(TEST_FULL_NAME).ds.gba
	@echo "    Running $< thanks to NoCashGBA emulator"
	@$(WINE) $(NO_CASH_GBA) $<
	
	
emulate-with-DeSmuME: $(TEST_FULL_NAME).nds
	@echo "    Running $< thanks to DeSmuME emulator"
	@$(WINE) $(DESMUME) $<


patch: $(TEST_FULL_NAME).sc.nds
	@echo "    Patching $< with script $(DLDITOOL_SCRIPT)"
	@$(DLDITOOL) $(DLDITOOL_SCRIPT) $<
	
	
transfer: $(FILES_TO_TRANSFER)
	@echo "    Transferring $^ to $(TRANSFER_DIRECTORY)"
	@/bin/ls $(TRANSFER_DIRECTORY) 1>/dev/null 2>&1
	@cp -f $^ $(TRANSFER_DIRECTORY)


umount: 	
	@echo "    Unmounting $(TRANSFER_MOUNT_POINT)"
	@pumount $(TRANSFER_MOUNT_POINT)


export: transfer umount

	

info-target:
	@echo "TEST_ARM9_SOURCES = $(TEST_ARM9_SOURCES)"
	@echo "TEST_TARGETS = $(TEST_TARGETS)"
	@echo "TARGET_ROM_NAME_BASE = $(TARGET_ROM_NAME_BASE)"
	@echo "OSDL_ARM7_INSTALLED_LIB = $(OSDL_ARM7_INSTALLED_LIB)"
	@echo "OSDL_ARM9_INSTALLED_LIB = $(OSDL_ARM9_INSTALLED_LIB)"






# Build flags section.


# Build common section.

# By default, use THUMB mode (16 bit), not ARM one (32 bit):
TEST_ARCH_FLAGS_COMMON = -mthumb -mthumb-interwork



# Preprocessor section: exports TEST_CPP_FLAGS_ARM7 / TEST_CPP_FLAGS_ARM9

TEST_CPP_FLAGS_COMMON = -I. -I$(OSDL_INSTALL_INC) -I$(ceylan_DIR)/include   \
	-I$(libfat_DIR)/include -I$(libnds_DIR)/include                         \
	-DCEYLAN_ARCH_NINTENDO_DS -DOSDL_ARCH_NINTENDO_DS                       \
	-DOSDL_USES_SDL=0 -DOSDL_USES_SDL_GFX=0 -DOSDL_USES_SDL_IMAGE=0         \
	-DOSDL_USES_SDL_TTF=0 -DOSDL_USES_LIBPNG=0 -DCEYLAN_DLL= -DOSDL_DLL=

TEST_CPP_FLAGS_ARM7 = $(TEST_CPP_FLAGS_COMMON) -DCEYLAN_RUNS_ON_ARM7 \
	-DOSDL_RUNS_ON_ARM7

TEST_CPP_FLAGS_ARM9 = $(TEST_CPP_FLAGS_COMMON) -DCEYLAN_RUNS_ON_ARM9 \
	-DOSDL_RUNS_ON_ARM9



# Compiler section: exports TEST_CXX_FLAGS_ARM7 / TEST_CXX_FLAGS_ARM9


# Long but safe:
# (-Winline removed, caused warning when inlining of test methods failed)
TEST_CXX_FLAGS_COMMON = $(TEST_ARCH_FLAGS_COMMON) -Wall -Werror -Woverloaded-virtual -W -Wfloat-equal -Wundef -Wendif-labels -Wshadow -Wpointer-arith -Wcast-qual -Wwrite-strings -Wconversion -Wsign-compare -Wmissing-format-attribute -Wredundant-decls -Wno-unused-parameter -Wno-unknown-pragmas -Wno-long-long


# -Wconversion had to be removed because of incomprehensible warnings like:
# 'passing argument 1 of XXX with different width due to prototype'
TEST_CC_FLAGS_COMMON = $(TEST_ARCH_FLAGS_COMMON) -Winline -Wall -Werror  -W -Wfloat-equal -Wundef -Wendif-labels -Wshadow -Wpointer-arith -Wcast-qual -Wwrite-strings -Wsign-compare -Wmissing-format-attribute -Wredundant-decls -Wno-unused-parameter -Wno-unknown-pragmas -Wno-long-long

# Same as TEST_CC_FLAGS_COMMON, but with non-blocking warnings (no -Werror):
TEST_CC_FLAGS_COMMON_RELAXED = $(TEST_ARCH_FLAGS_COMMON) -Winline -Wall -W -Wfloat-equal -Wundef -Wendif-labels -Wshadow -Wpointer-arith -Wcast-qual -Wwrite-strings -Wsign-compare -Wmissing-format-attribute -Wredundant-decls -Wno-unused-parameter -Wno-unknown-pragmas -Wno-long-long

# Tests will follow the release mode of the OSDL library:
ifeq ($(OSDL_TESTED_BUILD_MODE),debug)

# Flags common to the ARM7 and the ARM9 for compilation in debug mode:
TEST_CXX_FLAGS_MODE = -g -O0
TEST_CC_FLAGS_MODE = -g -O0

else

# Flags common to the ARM7 and the ARM9 for compilation in release mode:
TEST_CXX_FLAGS_MODE = -O3 -fomit-frame-pointer -ffast-math

TEST_CC_FLAGS_MODE          = -O3 -fomit-frame-pointer -ffast-math
TEST_CC_FLAGS_MODE_FOR_SIZE = -Os -fomit-frame-pointer -ffast-math

endif


TEST_CXX_FLAGS_ARM7 = $(TEST_CXX_FLAGS_COMMON) $(TEST_CXX_FLAGS_MODE) -mcpu=arm7tdmi -mtune=arm7tdmi 

TEST_CC_FLAGS_ARM7 = $(TEST_CC_FLAGS_COMMON) $(TEST_CC_FLAGS_MODE) -mcpu=arm7tdmi -mtune=arm7tdmi 

TEST_CC_FLAGS_ARM7_RELAXED= $(TEST_CC_FLAGS_COMMON_RELAXED) $(TEST_CC_FLAGS_MODE_FOR_SIZE) -mcpu=arm7tdmi -mtune=arm7tdmi 

TEST_CXX_FLAGS_ARM9 = $(TEST_CXX_FLAGS_COMMON) $(TEST_CXX_FLAGS_MODE) -march=armv5te -mtune=arm946e-s 

TEST_CC_FLAGS_ARM9 = $(TEST_CXX_FLAGS_COMMON) $(TEST_CXX_FLAGS_MODE) -march=armv5te -mtune=arm946e-s 


# Linker section: exports TEST_LD_FLAGS_ARM7 / TEST_LD_FLAGS_ARM9,
# TEST_LIBPATHS_ARM7 / TEST_LIBPATHS_ARM9 and TEST_LIBS_ARM7 / TEST_LIBS_ARM9


# TEST_LD_FLAGS_ARM7 and TEST_LD_FLAGS_ARM9 section:
TEST_LD_FLAGS_COMMON:= $(TEST_ARCH_FLAGS_COMMON)

TEST_LD_FLAGS_ARM7:= -specs=ds_arm7.specs
TEST_LD_FLAGS_ARM9:= -specs=ds_arm9.specs 


ifeq ($(OSDL_TESTED_BUILD_MODE),debug)

TEST_LD_FLAGS_COMMON:= $(TEST_LD_FLAGS_COMMON) -g 

else

TEST_LD_FLAGS_COMMON:= $(TEST_LD_FLAGS_COMMON)

endif


TEST_LD_FLAGS_ARM7:= $(TEST_LD_FLAGS_COMMON) $(TEST_LD_FLAGS_ARM7)
TEST_LD_FLAGS_ARM9:= $(TEST_LD_FLAGS_COMMON) $(TEST_LD_FLAGS_ARM9)



# TEST_LIBPATHS_ARM7 and TEST_LIBPATHS_ARM9 section:

# Let the possibility for the user to define his own LIBPATHS:
TEST_LIBPATHS_COMMON = $(LIBPATHS) -L$(ceylan_DIR)/lib -L$(libfat_DIR)/lib -L$(libnds_DIR)/lib

TEST_LIBPATHS_ARM7 = -L$(OSDL_INSTALL_LIB) $(TEST_LIBPATHS_COMMON) 
TEST_LIBPATHS_ARM9 = -L$(OSDL_INSTALL_LIB) $(TEST_LIBPATHS_COMMON) 



# Ceylan section.


CEYLAN_BASE_LIB_NAME:= Ceylan-${CEYLAN_USED_VERSION}-release


CEYLAN_ARM7_LINK_NAME:= $(CEYLAN_BASE_LIB_NAME)-arm7
CEYLAN_ARM9_LINK_NAME:= $(CEYLAN_BASE_LIB_NAME)-arm9

CEYLAN_ARM7_LIB_NAME:= lib$(CEYLAN_ARM7_LINK_NAME).a
CEYLAN_ARM9_LIB_NAME:= lib$(CEYLAN_ARM9_LINK_NAME).a

CEYLAN_INSTALL_LIB := $(ceylan_DIR)/lib

CEYLAN_ARM7_INSTALLED_LIB := $(CEYLAN_INSTALL_LIB)/$(CEYLAN_ARM7_LIB_NAME)
CEYLAN_ARM9_INSTALLED_LIB := $(CEYLAN_INSTALL_LIB)/$(CEYLAN_ARM9_LIB_NAME)


# TEST_LIBS_ARM7 and TEST_LIBS_ARM9 section:


# Let the possibility for the user to define his own LIBS.
# Not libfat for the ARM7 yet (library too big), but Helix-OSDL:
TEST_BASE_LIBS_ARM7:= $(LIBS) -l$(CEYLAN_BASE_LIB_NAME)-arm7 -lHelix-OSDL-arm7 -lnds7
	
TEST_BASE_LIBS_ARM9:= $(LIBS) -l$(CEYLAN_BASE_LIB_NAME)-arm9 -lfat -lnds9


TEST_LIBS_ARM7 = \
       -lOSDL-$(OSDL_TESTED_VERSION)-$(OSDL_TESTED_BUILD_MODE)-arm7 \
       $(TEST_BASE_LIBS_ARM7)
	
TEST_LIBS_ARM9 = \
       -lOSDL-$(OSDL_TESTED_VERSION)-$(OSDL_TESTED_BUILD_MODE)-arm9 \
       $(TEST_BASE_LIBS_ARM9)


	
info: info-target info-flags info-debug info-run


info-flags:
	@echo "TEST_CPP_FLAGS_ARM7 = $(TEST_CPP_FLAGS_ARM7)"
	@echo "TEST_CPP_FLAGS_ARM9 = $(TEST_CPP_FLAGS_ARM9)"
	@echo
	@echo "TEST_CXX_FLAGS_ARM7 = $(TEST_CXX_FLAGS_ARM7)"
	@echo "TEST_CXX_FLAGS_ARM9 = $(TEST_CXX_FLAGS_ARM9)"
	@echo
	@echo "TEST_LD_FLAGS_ARM7 = $(TEST_LD_FLAGS_ARM7)"
	@echo "TEST_LD_FLAGS_ARM9 = $(TEST_LD_FLAGS_ARM9)"
	@echo
	@echo "TEST_LIBPATHS_ARM7 = $(TEST_LIBPATHS_ARM7)"
	@echo "TEST_LIBPATHS_ARM9 = $(TEST_LIBPATHS_ARM9)"
	@echo
	@echo "TEST_LIBS_ARM7 = $(TEST_LIBS_ARM7)"
	@echo "TEST_LIBS_ARM9 = $(TEST_LIBS_ARM9)"


info-debug:
	@echo "nm command-line: $(NM) --demangle "
	@echo "strip command-line: $(STRIP)"
		

info-run:
	@echo "NO_CASH_GBA = $(NO_CASH_GBA)"
	@echo "DESMUME = $(DESMUME)"
	

	


# Rules section.



# From source files specific to either ARM to object files:

# Exception rules to avoid otherwise blocking warnings:
testOSDLMusic-$(TEST_PREFIX).arm7.o: testOSDLMusic.arm7.c
	@echo "    Compiling (overriden rule) $< into $@"
	$(CC) $(TEST_CC_FLAGS_ARM7_RELAXED) $(TEST_CPP_FLAGS_ARM7) -c $< -o $@


%-$(TEST_PREFIX).arm7.o: %.arm7.c
	@echo "    Compiling $< into $@"
	$(CC) $(TEST_CC_FLAGS_ARM7) $(TEST_CPP_FLAGS_ARM7) -c $< -o $@

%-$(TEST_PREFIX).arm7.o: %.arm7.cc
	@echo "    Compiling $< into $@"
	@$(CXX) $(TEST_CXX_FLAGS_ARM7) $(TEST_CPP_FLAGS_ARM7) -c $< -o $@
	
%.arm7.o: %.arm7.cc
	@echo "    Compiling $< into $@"
	@$(CXX) $(TEST_CXX_FLAGS_ARM7) $(TEST_CPP_FLAGS_ARM7) -c $< -o $@
	
	
%-$(TEST_PREFIX).arm9.o: %.arm9.cc
	@echo "    Compiling $< into $@"
	$(CXX) $(TEST_CXX_FLAGS_ARM9) $(TEST_CPP_FLAGS_ARM9) -c $< -o $@


# From object files to fully linked ELF executables:


%.arm7.elf: %.arm7.o $(CEYLAN_ARM7_INSTALLED_LIB) $(HELIX_OSDL_ARM7_INSTALLED_LIB) $(OSDL_ARM7_INSTALLED_LIB)
	@echo "    Creating OSDL library-user ARM7 ELF executable $@ from $< and from Helix library"
	@$(CXX) $(TEST_LD_FLAGS_ARM7) -Wl,-Map,$@.map $< $(TEST_LIBPATHS_ARM7) $(TEST_LIBS_ARM7) -o $@


%.arm9.elf: %.arm9.o $(CEYLAN_ARM9_INSTALLED_LIB) $(OSDL_ARM9_INSTALLED_LIB)
	@echo "    Creating OSDL library-user ARM9 ELF executable $@ from $< and from Ceylan and OSDL ARM9 libraries"
	@$(CXX) $(TEST_LD_FLAGS_ARM9) -Wl,-Map,$@.map $< $(TEST_LIBPATHS_ARM9) $(TEST_LIBS_ARM9)  -o $@




# From ELF executable to DS bin executable format:


%.arm7.bin: %.arm7.elf
	@echo "    Creating ARM7 bin executable $@ from $<"
	@$(OBJCOPY) -O binary $< $@


%.arm9.bin: %.arm9.elf
	@echo "    Creating ARM9 bin executable $@ from $<"
	@$(OBJCOPY) -O binary $< $@



# Neither basic logo, wifi logo nor sentences embedded:
# (sed removes the first dependency, leaving only the ARM9 one)
%.nds: %.arm7.bin %.arm9.bin
	@echo "    Creating final slot-1 DS ROM $@ from $^"
	@$(NDSTOOL) -c $@ -7 $< -9 `echo $^| sed 's|^.* ||1'`


# Rules overriden to use sound-specific ARM7 executable:
testOSDLSound-$(TEST_PREFIX).nds: testOSDLSound-$(TEST_PREFIX).arm7.bin \
		testOSDLSound-$(TEST_PREFIX).arm9.bin
	@echo "    Creating final slot-1 DS ROM $@ from $^"
	@$(NDSTOOL) -c $@ -7 $< -9 `echo $^| sed 's|^.* ||1'`


# Neither basic logo, wifi logo nor sentences embedded:
%.nds: OSDLDefaultMain.arm7.bin %.arm9.bin
	@echo "    Creating final slot-1 DS ROM $@ from $^"
	@$(NDSTOOL) -c $@ -7 $< -9 `echo $^| sed 's|^.* ||1'`


%.ds.gba: %.nds
	@echo "    Creating final slot-2 DS ROM $@ from $^"
	@$(DSBUILD) $<


# Patching does not stop on error, as not all tests have to be patched.


# sc for SuperCard linker:

# .sc.nds not working, use .nds instead
#%-patched.sc.nds: %.sc.nds $(OSDL_ARM9_INSTALLED_LIB)
#	@echo "    Patching $< into $@ with DLDI patch $(DLDITOOL_SC_SCRIPT)"
#	@$(DLDITOOL) $(DLDITOOL_SC_SCRIPT) $< ; /bin/cp -f $< $@

%-patched.nds: %.nds $(OSDL_ARM9_INSTALLED_LIB)
	@echo "    Patching $< into $@ with DLDI patch $(DLDITOOL_SC_SCRIPT)"
	@$(DLDITOOL) $(DLDITOOL_SC_SCRIPT) $< ; /bin/cp -f $< $@


# r4 for R4 linker:
%-patched.r4.nds: %.nds $(OSDL_ARM9_INSTALLED_LIB)
	@echo "    Patching $< into $@ with DLDI patch $(DLDITOOL_R4_SCRIPT)"
	@$(DLDITOOL) $(DLDITOOL_R4_SCRIPT) $< ; /bin/cp -f $< $@


# em for FCSR-based ROM for emulators:
%-patched.em.nds: %.ds.gba $(OSDL_ARM9_INSTALLED_LIB)
	@echo "    Patching $< into $@ with DLDI patch $(DLDITOOL_FCSR_SCRIPT)"
	@$(DLDITOOL) $(DLDITOOL_FCSR_SCRIPT) $< ; /bin/cp -f $< $@


%.sc.nds: %.ds.gba
	@echo "    Renaming $< into $@ for final slot-2 DS ROM for SuperCard linker"
	@/bin/cp -f $< $@


clean:
	@echo "    Cleaning tests"
	@rm -f *.o *.elf *.map *.bin *.nds *.ds.gba


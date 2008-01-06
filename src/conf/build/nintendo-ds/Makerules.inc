# General build rules for the cross-compilation of Nintendo DS binaries
# from GNU/Linux.
# To be included by all the Makefiles after they define their relevant TOP
# variable, ex: 'TOP = ..', when the Makefile is in a direct subdirectory 
# of the source root. 

include $(TOP)/src/conf/build/nintendo-ds/Makevars.inc


.PHONY: all compile info info-paths info-tools info-switches info-files \
	info-osdl-users displayModule arm7objectfiles arm9objectfiles   \
	install install-headers install-recurse clean clean-recurse	


# Compilation.

# Generally, all ARM7 object files are to be built in 32-bit mode (ex: for
# Helix integration), whereas all ARM9 ones are in thumb mode.
# All prerequesites, including Ceylan, must respect this rule.


# For C++ source files common to both ARM:

# C++ for the ARM7 is in 32-bit mode by default (but ARM7 C++ library no used).
# C++ for the ARM9 is in thumb mode by default (ARM9 C++ library used).

%.arm7.o: %.cc %.h 
	@echo "      Compiling in 32-bit mode $^ into $@"
	@$(CXX) $(CXX_FLAGS_ARM7_32_BIT) $(CPP_FLAGS_ARM7) -c $< -o $@

%.arm9.o: %.cc %.h 
	@echo "      Compiling in thumb mode $^ into $@"
	@$(CXX) $(CXX_FLAGS_ARM9) $(CPP_FLAGS_ARM9) -c $< -o $@


%.arm7.o: %.cc
	@echo "      Compiling in 32-bit mode $^ into $@"
	@$(CXX) $(CXX_FLAGS_ARM7_32_BIT) $(CPP_FLAGS_ARM7) -c $< -o $@
	
%.arm9.o: %.cc
	@echo "      Compiling in thumb mode $^ into $@"
	@$(CXX) $(CXX_FLAGS_ARM9) $(CPP_FLAGS_ARM9) -c $< -o $@


# Object files from assembler:

%.arm7.o: %.s
	@echo "      Assembling $^ into $@"
	@$(CC) -x assembler-with-cpp $(ASM_FLAGS) -c $< -o $@

%.arm9.o: %.s
	@echo "      Assembling $^ into $@"
	@$(CC) -x assembler-with-cpp $(ASM_FLAGS) -c $< -o $@



# For C source files common to both ARM:

# C for the ARM7 is in 32-bit mode by default.
# C for the ARM9 is in Thumb mode by default.


OSDLARM7Base.arm7.o: OSDLARM7Base.c OSDLARM7Base.h 
	@echo "      Compiling in 32-bit non-fatal warning mode $^ into $@"
	@$(CC) $(CC_FLAGS_ARM7_32_BIT_RELAXED) $(CPP_FLAGS_ARM7) -c $< -o $@

%.arm7.o: %.c %.h 
	@echo "      Compiling in 32-bit mode $^ into $@"
	@$(CC) $(CC_FLAGS_ARM7_32_BIT) $(CPP_FLAGS_ARM7) -c $< -o $@
	
%.arm9.o: %.c %.h 
	@echo "      Compiling in thumb mode $^ into $@"
	@$(CC) $(CC_FLAGS_ARM9) $(CPP_FLAGS_ARM9) -c $< -o $@


%.arm7.o: %.c
	@echo "      Compiling in 32-bit mode $^ into $@"
	@$(CC) $(CC_FLAGS_ARM7_32_BIT) $(CPP_FLAGS_ARM7) -c $< -o $@
	
%.arm9.o: %.c
	@echo "      Compiling in thumb mode $^ into $@"
	@$(CC) $(CC_FLAGS_ARM9) $(CPP_FLAGS_ARM9) -c $< -o $@



# For source files specific to either ARM:

%.arm7.o: %.arm7.cc %.arm7.h 
	@echo "      Compiling in 32-bit mode $^ into $@"
	@$(CXX) $(CXX_FLAGS_ARM7_32_BIT) $(CPP_FLAGS_ARM7) -c $< -o $@
	
%.arm9.o: %.arm9.cc %.arm9.h 
	@echo "      Compiling in thumb mode $^ into $@"
	@$(CXX) $(CXX_FLAGS_ARM9) $(CPP_FLAGS_ARM9) -c $< -o $@


%.arm7.o: %.arm7.cc %.arm7.h 
	@echo "      Compiling in 32-bit mode $^ into $@"
	@$(CXX) $(CXX_FLAGS_ARM7_32_BIT) $(CPP_FLAGS_ARM7) -c $< -o $@
	
%.arm9.o: %.arm9.cc %.arm9.h 
	@echo "      Compiling in thumb mode $^ into $@"
	@$(CXX) $(CXX_FLAGS_ARM9) $(CPP_FLAGS_ARM9) -c $< -o $@


	


# For (static) libraries:
# Mostly examples (no generic rule found).
lib%.arm7.a: %.arm7.o 
	@echo "    Linking $^ into $@"
	@$(AR) $(AR_FLAGS_ARM7) $@ $^
	
lib%.arm9.a: %.arm9.o 
	@echo "    Linking $^ into $@"
	@$(AR) $(AR_FLAGS_ARM7) $@ $^
	

# Executable linking.	


# Only related object files are included:



# From object files to ELF:

# CXX must be used here, not LD, otherwise:
# arm-eabi-ld: unrecognised emulation mode: thumb-interwork

%.arm7.elf: %.arm7.o
	@echo "    Creating ARM7 ELF executable $@ from $<"
	@$(CXX) $(LD_FLAGS_ARM7) $^ $(LIBPATHS_ARM7) $(LIBS_ARM7) -o $@

%.arm9.elf: %.arm9.o
	@echo "    Creating ARM9 ELF executable $@ from $<"
	@$(CXX) $(LD_FLAGS_ARM9) $^ $(LIBPATHS_ARM9) $(LIBS_ARM9) -o $@


# From ELF to DS bin format:

%.arm7.bin: %.arm7.elf
	@echo "    Creating ARM7 bin executable $@ from $<"
	@$(OBJCOPY) -O binary $< $@

%.arm9.bin: %.arm9.elf
	@echo "    Creating ARM9 bin executable $@ from $<"
	@$(OBJCOPY) -O binary $< $@


# Neither basic logo, wifi logo nor sentences embedded:
%.nds: %.arm7.bin %.arm9.bin
	@echo "    Creating final slot-1 DS ROM $@ from $^"
	@$(NDSTOOL) -c $@ -7 $< -9 `echo $^| sed 's|^.* ||1'`


%.ds.gba: %.nds
	@echo "    Creating final slot-2 DS ROM $@ from $^"
	@$(DSBUILD) $<


%.sc.nds: %.ds.gba
	@echo "    Creating final slot-2 DS ROM $@ from $^"
	@/bin/cp -f $< $@


# Add here: 
# dlditool Sclt.dldi helloworld.sc.nds


all:
	@for d in $(NDS_SUBDIRS); do cd $$d ; if ! $(MAKE) -f Makefile.cross CROSS_TARGET=$(CROSS_TARGET) all; then return 1; false ; else cd ..; fi ; done


compile: displayModule arm7objectfiles arm9objectfiles


arm7objectfiles: $(ARM7_OBJ_FILES)
	@echo "    All ARM7 object files built"
	
arm9objectfiles: $(ARM9_OBJ_FILES)
	@echo "    All ARM9 object files built"

displayModule:
	@echo "    Building all ARM7 and ARM9 object files in "`basename $(PWD)`
	 

install: install-recurse

	
install-headers:
	@echo "    Installing headers from "$(shell basename `pwd`)
	@mkdir -p $(OSDL_INSTALL_INC)
	-@/bin/cp -f OSDL*.h $(OSDL_INSTALL_INC) 2>/dev/null
	
	
install-recurse:
	@for d in $(NDS_SUBDIRS); do cd $$d ; if ! $(MAKE) -f Makefile.cross CROSS_TARGET=$(CROSS_TARGET) install; then break; else cd ..; fi ; done

	
clean: clean-recurse
	@echo "    Cleaning in "$(shell basename `pwd`)
	@-rm -f *.o *.lo *.a *.la .so *.map *.bin *.elf *.nds *.ds.gba 
	
	
clean-recurse:
	@for d in $(NDS_SUBDIRS); do cd $$d ; $(MAKE) -f Makefile.cross CROSS_TARGET=$(CROSS_TARGET) clean; cd ..; done
	
	
	
# Cancel automatic rule:	
%.o: %.cc


info: info-paths info-tools info-switches

info-paths:
	@echo "NDS_DEV_ROOT    = $(NDS_DEV_ROOT)"
	@echo "devkitPro_DIR   = $(devkitPro_DIR)"
	@echo "devkitARM_DIR   = $(devkitARM_DIR)"
	@echo "libnds_DIR      = $(libnds_DIR)"


info-tools:
	@echo "ADDR2LINE       = $(ADDR2LINE)"
	@echo "AR              = $(AR)"
	@echo "AS              = $(AS)"
	@echo "CC              = $(CC)"
	@echo "CXX             = $(CXX)"
	@echo "LD              = $(LD)"
	@echo "NM              = $(NM)"
	@echo "OBJCOPY         = $(OBJCOPY)"
	@echo "OBJDUMP         = $(OBJDUMP)"
	@echo "RANLIB          = $(RANLIB)"
	@echo "STRIP           = $(STRIP)"
	@echo "DLDITOOL        = $(DLDITOOL)"
	@echo "DLDITOOL_SCRIPT = $(DLDITOOL_SCRIPT)"
	@echo "NDSTOOL         = $(NDSTOOL)"


info-switches:
	@echo "build_mode      = $(build_mode)"
	@echo "CXX_FLAGS_ARM7  = $(CXX_FLAGS_ARM7)"
	@echo "CXX_FLAGS_ARM9  = $(CXX_FLAGS_ARM9)"
	@echo "CPP_FLAGS_ARM7  = $(CPP_FLAGS_ARM7)"
	@echo "CPP_FLAGS_ARM9  = $(CPP_FLAGS_ARM9)"
	@echo "LD_FLAGS_ARM7   = $(LD_FLAGS_ARM7)"
	@echo "LD_FLAGS_ARM9   = $(LD_FLAGS_ARM9)"
	@echo "LIBPATHS_ARM7   = $(LIBPATHS_ARM7)"
	@echo "LIBPATHS_ARM9   = $(LIBPATHS_ARM9)"
	@echo "LIBS_ARM7       = $(LIBS_ARM7)"
	@echo "LIBS_ARM9       = $(LIBS_ARM9)"


info-files:
	@echo "ARM7_OBJ_FILES = $(ARM7_OBJ_FILES)"
	@echo "ARM9_OBJ_FILES = $(ARM9_OBJ_FILES)"
	
	
info-osdl-users:
	@echo "OSDL_LIBPATHS_ARM7 = $(OSDL_LIBPATHS_ARM7)"
	@echo "OSDL_LIBPATHS_ARM9 = $(OSDL_LIBPATHS_ARM9)"
	@echo "OSDL_LIBS_ARM7 = $(OSDL_LIBS_ARM7)"
	@echo "OSDL_LIBS_ARM9 = $(OSDL_LIBS_ARM9)"
	

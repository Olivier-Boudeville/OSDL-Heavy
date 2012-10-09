## OSDL animation-related media tools.


animationtoolsdir = @OSDL_TOOL_DIR@/media/video/animation-management

animationtools_CXXFLAGS = @AM_CXXFLAGS@


animationtools_PROGRAMS =     \
	pngToOSDLFrame.exe        \
	generateMasterPalette.exe


pngToOSDLFrame_exe_SOURCES				= pngToOSDLFrame.cc
generateMasterPalette_exe_SOURCES       = generateMasterPalette.cc


PALETTES = \
	master-palette-original.osdl.palette              \
	master-palette-quantized.osdl.palette             \
	master-palette-quantized-gamma-corrected.osdl.palette


PALETTE_IMAGES = $(patsubst %.osdl.palette,%.png,$(PALETTES))


.PHONY: all osdl-palettes png


%.osdl.palette:
	@echo "Generating OSDL palettes"
	@./generateMasterPalette.exe


# 256 = 16x16 colors, +5 is used to remove OSDL palette header (a colorkey
# is expected to be set, thus the header is made of a tag, a colorkey)
% indicator, a color index, thus it is 2+1+2=5 bytes).
%.png: %.osdl.palette
	@echo "Converting palette in $< into a color panel in $@"
	@convert -depth 8 -size 16x16+5 rgb:$< -scale 800% $@


all: osdl-palettes png

osdl-palettes: $(PALETTES)

png: $(PALETTE_IMAGES)


clean-local:
	@/bin/rm -f *.log *.pal *.osdl.palette

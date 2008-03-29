## OSDL animation-related media tools.


animationtoolsdir=@OSDL_TOOL_DIR@/media/video/animation-management

animationtools_CXXFLAGS = @AM_CXXFLAGS@


animationtools_PROGRAMS =     \
	pngToOSDLFrame.exe        \
	generateMasterPalette.exe


pngToOSDLFrame_exe_SOURCES				= pngToOSDLFrame.cc
generateMasterPalette_exe_SOURCES       = generateMasterPalette.cc


PALETTES = \
	master-palette-original.png              \
	master-palette-quantized.png             \
	master-palette-quantized-gamma-corrected.png


%.rgb: 
	@echo "Generating palettes"
	@./generateMasterPalette.exe	


# 256 = 16x16 colors, but without the colorkey we have here 255 = 17x15
%.png: %.rgb
	@echo "Converting palette in $< into a color panel in $@"
	@convert -depth 8 -size 17x15 $< -scale 800% $@ 
	

pal: $(PALETTES)	

clean-local: 
	@rm -f *.log *.pal *.rgb

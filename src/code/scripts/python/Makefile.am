
PYTHON_SCRIPTS_SET = \
	generate_master_palette.py

        
EXTRA_DIST = $(PYTHON_SCRIPTS_SET)		 
                  

PALETTES = \
	master-palette-gamma-corrected.png  \
	master-palette-original.png         \
	master-palette-quantized.png


%.rgb: 
	@echo "Generating palettes"
	@python generate_master_palette.py 1>/dev/null 2>&1
	

# 256 = 16x16 colors, but without the colorkey we have here 255 = 17x15
%.png: %.rgb
	@echo "Converting palette in $< into a color panel in $@"
	@convert -depth 8 -size 17x15 $< -scale 800% $@ 
	

pal: $(PALETTES)	


clean-local:
	@rm -f *.png *.rgb *.pal
	
	

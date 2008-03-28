#!/usr/bin/env python

# See http://en.wikipedia.org/wiki/List_of_software_palettes#6-8-5_levels_RGB

def quantize(component):
	"""Converts a color component in [0;255] to [0.31] (5-bit)."""
	return int( round(component/255.0*31) ) 
	
	
# BGR, 5 bits per component, thus in [0,31].
def convert_to_uint16(r,g,b):
	# Base is 0xb1000000000000000 = 32768
	base = 32768
	base = (base | r) | (g<<5) | (b<<10)
	return base
		
# See http://forum.gbadev.org/viewtopic.php?t=6484		
ds_gamma_correction_factor = 2.3


# Returns a 8-bit color component gamma-corrected for the DS.
def gamma_correct(component):
	# Let's suppose this is a proper way of dealing with gamma...
	# See also for example ImageMagick-6.4.0magick/enhance.c:
    #  gamma_map[i]=(MagickRealType) ScaleMapToQuantum((MagickRealType)
    #    (MaxMap*pow((double) i/MaxMap,1.0/gamma)));
	return int( round( ( (component/255.0)**(1/ds_gamma_correction_factor) ) * 255.0 ) )

		
r_range = 6
g_range = 8
b_range = 5


# 0x6a4c30 is in decimal:
color_key = ( 106, 76, 48 )

base_count= r_range*g_range*b_range

grey_range=8


print "\nGenerating a master palette of %s base colors (for index in [1..%s]), with %s levels of red, %s of green and %s of blue. Index #0 is reserved for the colorkey, which is %s. The %s remaining index are used to add %s true grays and %s specific recurrent colors in target images.\n" % ( base_count, base_count, r_range, g_range, b_range, color_key, 256 - base_count -1, grey_range, 256 - base_count -1 - grey_range )


base_palette = [ ( 
		int( round(255*i/ (r_range-1) )),
		int( round(255*j/ (g_range-1) )),
		int( round(255*k/ (b_range-1) )) ) 
	for i in range(r_range)  
	for j in range(g_range)  
	for k in range(b_range) ]

base_palette = [color_key] + base_palette


# 15 index left.

# Adding grey_range true greys:

# Index offset chosen to remove pure black and pure white:  
for grey in [ int( round(255*(g+1)/(grey_range+1)) ) for g in range(grey_range) ]:
	base_palette += [ (grey,grey,grey) ]

# Adding 15-8=7 colors:

# Two blue from the main character, Stan:
base_palette.append( (  6, 177, 220 ) )
base_palette.append( (  4, 147, 183 ) )

# One brown/black, for the hair:
base_palette.append( (  70, 59, 42 ) )

# Two orange/pink, for the flesh:
base_palette.append( ( 238, 190, 141 ) )
base_palette.append( ( 250, 205, 155 ) )

# One yellow for blonde hair:
base_palette.append( ( 255, 255, 111 ) )

# One last blue:
base_palette.append( (  28,  95, 139 ) )


# Finally displays the obtained full palette:
count=0
for c in base_palette:
	print "  + color index #%s: %s" % (count,c)
	count +=1

print "Palette has %s color index." % (len(base_palette))



original_palette_filename = "master-palette-original.rgb"

print " * Writing original master palette to '%s'." % (original_palette_filename)
palette_file = open( original_palette_filename, 'w' )

for c in base_palette:
	palette_file.write( "%c" % (c[0]) )
	palette_file.write( "%c" % (c[1]) )
	palette_file.write( "%c" % (c[2]) )

palette_file.close()

gamma_corrected_palette_filename = "master-palette-gamma-corrected.rgb"

print " * Writing gamma-corrected master palette to '%s'." % (gamma_corrected_palette_filename)
palette_file = open( gamma_corrected_palette_filename, 'w' )

for c in base_palette:
	palette_file.write( "%c" % ( gamma_correct( c[0]) ) )
	palette_file.write( "%c" % ( gamma_correct( c[1]) ) )
	palette_file.write( "%c" % ( gamma_correct( c[2]) ) )

palette_file.close()




quantized_palette_filename = "master-palette-quantized.rgb"

print " * Writing quantized master palette (non gamma-corrected) to '%s'." % (quantized_palette_filename)
palette_file = open( quantized_palette_filename, 'w' )

count=0
for c in base_palette:
	#print "  + quantized color index #%s: (%s, %s, %s)" % ( count,
	#	quantize(c[0]), quantize(c[1]), quantize(c[2]) )
	palette_file.write( "%c" % ( int( 255.0 /31 * quantize(c[0]) ) ) )
	palette_file.write( "%c" % ( int( 255.0 /31 * quantize(c[1]) ) ) )
	palette_file.write( "%c" % ( int( 255.0 /31 * quantize(c[2]) ) ) )
	count +=1

palette_file.close()



final_palette_filename = "master-palette.pal"

print " * Writing final master palette (gamma-corrected, then quantized, then encoded for the DS) to '%s'." % (final_palette_filename)
palette_file = open( final_palette_filename, 'w' )

for c in base_palette:
	converted_color = convert_to_uint16( 
		quantize( gamma_correct( c[0]) ),
		quantize( gamma_correct( c[1]) ),
		quantize( gamma_correct( c[2]) ) )
	palette_file.write( "%c" % ( ( converted_color & 0xFF00 ) >> 8 ) )
	palette_file.write( "%c" % (   converted_color & 0x00FF ) )

palette_file.close()



print "Master palette successfully generated."

print "Use '%s' for color-reduction of assets, and '%s' as the actual DS palette." % ( quantized_palette_filename, final_palette_filename)



#!/usr/bin/env python

# See http://en.wikipedia.org/wiki/List_of_software_palettes#6-8-5_levels_RGB

r_range = 6
g_range = 8
b_range = 5


# 0x6a4c30 is in decimal:
color_key = ( 106, 76, 48 )

base_count= r_range*g_range*b_range

print "Generating a master palette of %s base colors (for index in [1..%s]), with %s levels of red, %s of green and %s of blue. Index #0 is reserved for the colorkey, which is %s. The %s remaining index are used to add true grays and specific recurrent colors in target images." % ( base_count, base_count, r_range, g_range, b_range, color_key, 256 - base_count -1 )


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
grey_range=8

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


palette_filename = "master-palette.rgb"

print "Writing palette to '%s'." % (palette_filename)
palette_file = open( palette_filename, 'w' )

for c in base_palette:
	palette_file.write( "%c" % (c[0]) )
	palette_file.write( "%c" % (c[1]) )
	palette_file.write( "%c" % (c[2]) )

palette_file.close()

print "File written."


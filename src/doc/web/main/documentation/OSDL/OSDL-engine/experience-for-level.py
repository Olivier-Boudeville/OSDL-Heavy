#!/usr/bin/env python


from math import *

plot_file = open( "experience-for-level.dat", "w" )

# 0 is a special case:
plot_file.write( "0 0\n" )

for level in range(1,51):
	plot_file.write( "%s %s\n" % ( level, (level+4)**3 ) )
	
	

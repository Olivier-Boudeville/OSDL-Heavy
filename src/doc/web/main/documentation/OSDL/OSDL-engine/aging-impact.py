#!/usr/bin/env python

from math import *

plot_file = open( "aging-impact.dat", "w" )

# lp: life percent.
# ap: ability percent.

birth_lp=0
birth_ap=0

young_lp=30
young_ap=100

adult_lp=45
adult_ap=85

old_lp=75
old_ap=75

death_lp=100
death_ap=100

# (life_percent,ability_percent):
aging_curve = [ (0,0), (30,100), (45,85), (75,75), (100,0) ]

mean_ability_percent= 30*100/2 + 15*

for (life_percent,ability_percent) in aging_curve:
	plot_file.write( "%s %s\n" % ( life_percent, ability_percent ) )
	
	

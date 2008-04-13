#!/usr/bin/env python

# All probabilities are in [0,1].

from math import *

# This coefficient is chosen so that, with a base probability of 50%, a 
# modifier of p % leads roughly to an overall probability of 50+p %.
modifier_coeff=4.2


def p_modified(p_base,modifier):
	exp_m = exp( modifier * modifier_coeff )
	return p_base*exp_m/(1+p_base*(exp_m-1))
  
  
def p_twice_modified(p_base,first_modifier,second_modifier):
	return p_modified( p_modified(p_base,first_modifier), second_modifier )
  


def probability_to_string(p):
	return "%.4s %%" % (p*100,)
	
def modifier_to_string(m):
	"""Modifiers are not actually percentages."""
	return "%s %%" % (m*100,)
	

	
def display_for(p_base,modifier):
	print "  %s, modified probability is %s." % ( 
	modifier_to_string(modifier),
	probability_to_string( p_modified(p_base,modifier) ) )
	
	
def display_for_modifiers(p_base):
	print "For a base probability of success of %s, with a modifier of:" % ( probability_to_string(p_base), )
	display_for( p_base, -0.3 )
	display_for( p_base, -0.2 )
	display_for( p_base, -0.1 )
	display_for( p_base,  0.0 )
	display_for( p_base,  0.1 )
	display_for( p_base,  0.2 )
	display_for( p_base,  0.3 )
	print 


def display_for_two_modifiers(p_base,first_modifier,second_modifier):
	print "For a base probability of success of %s, with first modifier being %s, with second modifier being %s, modified probability is %s." % (
	probability_to_string(p_base),
	modifier_to_string(first_modifier),
	modifier_to_string(second_modifier),
	probability_to_string(
		p_twice_modified(p_base,first_modifier,second_modifier) ) )




print "Modifier coefficient is %s." % (modifier_coeff,)

display_for_modifiers( 0.0  )
display_for_modifiers( 0.25 )
display_for_modifiers( 0.50 )
display_for_modifiers( 0.75 )
display_for_modifiers( 1.0  )


display_for_two_modifiers( 0.6, 0.2,  0.15 )
display_for_two_modifiers( 0.6, 0.15, 0.2 )
print "For a base probability of success of 60.0 %, with a modifier of "
display_for( 0.6, 0.35 )

plot_file = open( "probability-modifier.p", "w" )

for base_p in range(0,100):
	plot_file.write( "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" % ( base_p, 
		100*p_modified( base_p/100.0, -1.0 ), 
		100*p_modified( base_p/100.0, -0.9 ), 
		100*p_modified( base_p/100.0, -0.8 ), 
		100*p_modified( base_p/100.0, -0.7 ), 
		100*p_modified( base_p/100.0, -0.6 ), 
		100*p_modified( base_p/100.0, -0.5 ), 
		100*p_modified( base_p/100.0, -0.4 ), 
		100*p_modified( base_p/100.0, -0.3 ), 
		100*p_modified( base_p/100.0, -0.2 ), 
		100*p_modified( base_p/100.0, -0.1 ), 
		100*p_modified( base_p/100.0, -0.0 ), 
		100*p_modified( base_p/100.0,  0.1 ), 
		100*p_modified( base_p/100.0,  0.2 ), 
		100*p_modified( base_p/100.0,  0.3 ), 
		100*p_modified( base_p/100.0,  0.4 ), 
		100*p_modified( base_p/100.0,  0.5 ), 
		100*p_modified( base_p/100.0,  0.6 ), 
		100*p_modified( base_p/100.0,  0.7 ), 
		100*p_modified( base_p/100.0,  0.8 ), 
		100*p_modified( base_p/100.0,  0.9 ), 
		100*p_modified( base_p/100.0,  1.0 )
	) )


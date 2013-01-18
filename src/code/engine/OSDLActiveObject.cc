/* 
 * Copyright (C) 2003-2013 Olivier Boudeville
 *
 * This file is part of the OSDL library.
 *
 * The OSDL library is free software: you can redistribute it and/or modify
 * it under the terms of either the GNU Lesser General Public License or
 * the GNU General Public License, as they are published by the Free Software
 * Foundation, either version 3 of these Licenses, or (at your option) 
 * any later version.
 *
 * The OSDL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License and the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License and of the GNU General Public License along with the OSDL library.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Olivier Boudeville (olivier.boudeville@esperide.com)
 *
 */


#include "OSDLActiveObject.h"

#include "OSDLEvents.h"           // for SimulationTick
#include "OSDLScheduler.h"        // for GetExistingScheduler




using std::string ;
using std::list ;


#include "Ceylan.h"               // for Log
using namespace Ceylan::Log ;


using namespace OSDL::Engine ;
using namespace OSDL::Events ;





ActiveObject::ActiveObject( ObjectSchedulingPolicy policy, Weight weight ) :
	_policy( policy ),
	_weight( weight ),
	_registered( false ),
	_birthTick( 0 )
{

	
}	



ActiveObject::~ActiveObject() throw()
{

	// Each child class should have taken care of the unregistering:
	if ( _registered )
		LogPlug::error( "ActiveObject destructor: object " + toString()
			+ " was still registered." ) ;
		
}





// Settings section.



ObjectSchedulingPolicy ActiveObject::getPolicy() const
{

	return _policy ;
	
}



Weight ActiveObject::getWeight() const
{

	return _weight ;
	
}


	
void ActiveObject::setBirthTick( SimulationTick birthSimulationTick )
{

	_birthTick = birthSimulationTick ;
	
}


		
SimulationTick ActiveObject::getBirthTick() const
{

	return _birthTick ;
	
}



void ActiveObject::onSkip( SimulationTick skippedStep ) 
{

	LogPlug::warning( "An active object (" 
		+ toString( Ceylan::low )+ ") had his simulation tick " 
		+ Ceylan::toString( skippedStep ) + " skipped." ) ; 
		
}



void ActiveObject::onImpossibleActivation( SimulationTick missedStep ) 
{

	throw SchedulingException( "Active object (" + toString( Ceylan::low )
		+ ") could not be activated for simulation tick " 
		+ Ceylan::toString( missedStep ) + "." ) ; 
		
}



const string ActiveObject::toString( Ceylan::VerbosityLevels level ) const
{	

	string policy ;
	
	switch( _policy )
	{
	
		case relaxed:
			policy = "relaxed" ;
			break ;
		
		case strict:
			policy = "strict" ;
			break ;
			
		default:
			policy = "unknown (abnormal)" ;	
			break ;
			
	}
	
	
	string birth ;
	
	if ( _birthTick == 0 )
		birth = "was never scheduled" ;
	else
		birth = "has been scheduled for the first time at simulation tick #"
			+ Ceylan::toString( _birthTick ) ;

	return "Active object, with the " + policy + " policy, a weight of "
		+ Ceylan::toString( _weight ) + ", which is "
		+ ( _registered ? "registered" : "not registered yet" )
		+ " and which " + birth ;
	
}


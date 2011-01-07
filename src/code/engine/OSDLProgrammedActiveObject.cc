/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#include "OSDLProgrammedActiveObject.h"


#include "OSDLScheduler.h"        // for GetExistingScheduler



using std::string ;
using std::list ;



#include "Ceylan.h"               // for Log
using namespace Ceylan::Log ;


using namespace OSDL::Engine ;
using namespace OSDL::Events ;




ProgrammedActiveObject::ProgrammedActiveObject( 
		const SimulationTickList & activationTicks,
		bool absolutelyDefined, 
		bool autoRegister,
		ObjectSchedulingPolicy policy, 
		Weight weight ) : 
	ActiveObject( policy, weight ),
	_absoluteTicks( true )
{

	_programmedTicks = activationTicks ;
	
	_programmedTicks.sort() ;

	if ( autoRegister )
		registerToScheduler() ;	
	
}



ProgrammedActiveObject::ProgrammedActiveObject( 
		Events::SimulationTick activationTick, 
		bool absolutelyDefined, 
		bool autoRegister,
		ObjectSchedulingPolicy policy, 
		Weight weight ) : 
	ActiveObject( policy, weight ),
	_absoluteTicks( true )
{

	_programmedTicks.push_back( activationTick ) ;
	
	if ( autoRegister )
		registerToScheduler() ;	

}	




ProgrammedActiveObject::~ProgrammedActiveObject() throw()
{

	if ( _registered )
	{
	
		try
		{
	
			unregisterFromScheduler() ;
			
		}
		catch( const SchedulingException & e )
		{
	
			LogPlug::error( "ProgrammedActiveObject destructor failed: "
				+ e.toString() ) ;
	
		}
		
	}
		
}




// Registering section.



void ProgrammedActiveObject::registerToScheduler()
{
	
	if ( _registered )
		throw SchedulingException( 
			"ProgrammedActiveObject::registerToScheduler failed: "
			"already registered." ) ;
			
	Scheduler & scheduler = Scheduler::GetExistingScheduler() ;
	
	// The onRegistering callback will set _registered:
	scheduler.registerProgrammedObject( *this ) ;
	
	/*
	 * This object keeps the list of its programmed ticks (although the 
	 * scheduler has now this information too), so that on unsubscription 
	 * the scheduler knows directly the ticks to update when unregistering this 
	 * object, instead of performing a full look-up in its hashtable from
	 * scratch.
	 *
	 */
}
	
				

void ProgrammedActiveObject::unregisterFromScheduler() 
{

	// LogPlug::trace( "ProgrammedActiveObject::unregisterFromScheduler" ) ;
	
	if ( ! _registered )
		throw SchedulingException( 
			"ProgrammedActiveObject::unregisterFromScheduler failed: "
			"not already registered." ) ;
			
	Scheduler & scheduler = Scheduler::GetExistingScheduler() ;
	
	scheduler.unregisterProgrammedObject( *this ) ;
	
	_registered = false ;

}





// Settings section.



bool ProgrammedActiveObject::areProgrammedActivationsAbsolute() const
{

	return _absoluteTicks ;
	
}



void ProgrammedActiveObject::absoluteProgrammedActivationsWanted( bool on )
{

	if ( _registered )
		throw SchedulingException(
			 "ProgrammedActiveObject::absoluteProgrammedActivationsWanted: "
			 "cannot be called once already registered." ) ;

	_absoluteTicks = on ;
	
}



const SimulationTickList & ProgrammedActiveObject::getProgrammedActivations()
	const
{
			
	return _programmedTicks ;
	
}



void ProgrammedActiveObject::setProgrammedActivations( 
	const SimulationTickList & newActivationsList ) 
{

	if ( _registered ) 
		throw SchedulingException( 
			"ProgrammedActiveObject::setProgrammedActivations failed: "
			"already registered to scheduler." ) ;
			
	_programmedTicks = newActivationsList ;
	
}



void ProgrammedActiveObject::addProgrammedActivations( 
	const SimulationTickList & additionalActivationsList ) 
{

	if ( _registered ) 
		throw SchedulingException( 
			"ProgrammedActiveObject::addProgrammedActivations failed: "
			"already registered to scheduler." ) ;
	
	/*
	 * Still ordered afterwards:
	 *
	 * Stupid STL will not merge a const list into a non-const list...
	 *
	 */
	_programmedTicks.merge( * const_cast<SimulationTickList *> ( 
		& additionalActivationsList ) ) ;
	
	// No duplicates wanted:
	_programmedTicks.unique() ;
	
}



void ProgrammedActiveObject::onRegistering()
{

	_registered = true ;
	
}



const string ProgrammedActiveObject::toString( Ceylan::VerbosityLevels level )
	const
{	

	return "Programmed " + ActiveObject::toString( level) 
		+ ", whose programmed ticks, which are "
		+ string( _absoluteTicks ? "absolutely" : "relatively" ) 
		+ " defined, are: " + Ceylan::toString( _programmedTicks ) ;
		
}


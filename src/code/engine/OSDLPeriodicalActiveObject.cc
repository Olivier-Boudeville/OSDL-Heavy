/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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


#include "OSDLPeriodicalActiveObject.h"


#include "OSDLScheduler.h"        // for GetExistingScheduler



using std::string ;
using std::list ;

using Ceylan::Maths::Hertz ;



#include "Ceylan.h"               // for Log
using namespace Ceylan::Log ;


using namespace OSDL::Engine ;
using namespace OSDL::Events ;




PeriodicalActiveObject::PeriodicalActiveObject( Period period,
		bool autoRegister, ObjectSchedulingPolicy policy, Weight weight ) : 
	ActiveObject( policy, weight ),
	_period( period ),
	_subslot( /* invalid by design: */ period + 1 )
{
	
	if ( period == 0 )
		throw SchedulingException( "PeriodicalActiveObject constructor failed: "
			"period cannot be null." ) ;
			
	if ( autoRegister )
		registerToScheduler() ;	
			
}



PeriodicalActiveObject::~PeriodicalActiveObject() throw()
{

	if ( _registered )
	{
	
		try
		{
	
			unregisterFromScheduler() ;
			
		}
		catch( const SchedulingException & e )
		{
	
			LogPlug::error( "PeriodicalActiveObject destructor failed: "
				+ e.toString() ) ;
	
		}
		
	}
	
}




// Registering section.



void PeriodicalActiveObject::registerToScheduler()
{
	
	if ( _registered )
		throw SchedulingException( 
			"PeriodicalActiveObject::registerToScheduler failed: "
			"already registered." ) ;
			
	Scheduler & scheduler = Scheduler::GetExistingScheduler() ;
	
	// The onRegistering callback will set _registered and _subslot:
	scheduler.registerPeriodicalObject( *this ) ;
	
}
	
				

void PeriodicalActiveObject::unregisterFromScheduler() 
{

	Ceylan::checkpoint( "PeriodicalActiveObject::unregisterFromScheduler" ) ;
	
	if ( ! _registered )
		throw SchedulingException( 
			"PeriodicalActiveObject::unregisterFromScheduler failed: "
			"not already registered." ) ;
			
	Scheduler & scheduler = Scheduler::GetExistingScheduler() ;
	
	scheduler.unregisterPeriodicalObject( *this ) ;
	
	_registered = false ;
	
}




// Settings section.



Period PeriodicalActiveObject::getPeriod() const
{

	return _period ;
	
}



void PeriodicalActiveObject::setPeriod( Period newPeriod ) 
{

	if ( _registered ) 
		throw SchedulingException( "PeriodicalActiveObject::setPeriod failed: "
			"already registered to scheduler." ) ;
			
	_period = newPeriod ;
	
}



Hertz PeriodicalActiveObject::setFrequency( Hertz newFrequency ) 
{

	if ( _registered ) 
		throw SchedulingException( 
			"PeriodicalActiveObject::setFrequency failed: "
			"already registered to scheduler." ) ;
			

	// Throw exception if scheduler not already existing:
	Scheduler & scheduler = Scheduler::GetExistingScheduler() ;
	
	/*
	 * Target period is 1/newFrequency, one needs to divide it by 
	 * simulation period duration to know how many simulation periods 
	 * it will need:
	 * _period = (1/newFrequency) / simulatickDuration 
	 * = simulatickDuration / newFrequency
	 *
	 * Microsecond simulatickDuration = 
	 * 	scheduler.getSimulationTickCount() * scheduler.getTimeSliceDuration() ;
	 *
	 */
	
	_period = static_cast<Period>( Ceylan::Maths::Round( 1000000.0f / 
	 	( newFrequency * scheduler.getSimulationTickCount() 
			* scheduler.getTimeSliceDuration() ) ) ) ;
	 
	 /*
	  * newFrequency = simulatickDuration / _period
	  *
	  */
	 Hertz actualFrequency = static_cast<Hertz>( 
	 	Ceylan::Maths::Round( 1000000.0f / 
	 		( _period * scheduler.getSimulationTickCount() 
				* scheduler.getTimeSliceDuration() ) ) ) ;
	 
	 // Clamp for too high frequencies:
	 if ( _period < 1 )
	 {
	 
	 	LogPlug::warning( "PeriodicalActiveObject::setFrequency: frequency "
			+ Ceylan::toString( newFrequency ) + "Hz had to be clampled to "
			+ Ceylan::toString( actualFrequency ) + " Hz." ) ;
			
	 	_period = 1 ;
	 
	 }
	 
	 return actualFrequency ;
		
}



Period PeriodicalActiveObject::getSubslotNumber() const 
{

	if ( _subslot >= _period )
		throw SchedulingException( "PeriodicalActiveObject::getSubslotNumber: "
			"invalid sub-slot, object probably not registered yet." ) ;
			
	return _subslot ;

}



void PeriodicalActiveObject::onRegistering( Events::Period subslot ) 
{

	if ( _registered ) 
		throw SchedulingException( 
			"PeriodicalActiveObject::onRegistering failed: "
			"already registered to scheduler." ) ;

	_subslot = subslot ;
	
	// Only called by the scheduler on registration:
	_registered = true ;
	
}



void PeriodicalActiveObject::setBirthTick( 
	Events::SimulationTick currentSimulationTick )
{

	/*
	 * Computing at which simulation tick this object will be activated
	 * for the first time.
	 *
	 */
	 
	/*
	 * Note the integer division: lastPeriodBegin = 
	 *   currentSimulationTick - ( currentSimulationTick % _period )
	 * (we compute the latest period beginning)
	 */
	Events::SimulationTick lastPeriodBegin = 
		_period * ( currentSimulationTick / _period ) ;
	
	// In the current period, are we after or before the object sub-slot?
	if ( currentSimulationTick % _period < _subslot )
	{
	
		// We are before, will be activated on:
		_birthTick = lastPeriodBegin + _subslot ;
	
	}
	else
	{
	
		// We are already past the sub-slot, will be activated on next one:
		_birthTick = lastPeriodBegin + _period + _subslot ;
	
	}
			
}


	
const string PeriodicalActiveObject::toString( Ceylan::VerbosityLevels level )
	const
{	

	string slot ;
	
	
	if ( _subslot == 0 )
		slot = ", with no subslot set" ;
	else
		slot = ", using subslot #" + Ceylan::toString( _subslot ) ;
			
			
	return "Periodical active object, whose period is " 
		+ Ceylan::toString( _period ) + slot + ". It is an " 
		+ ActiveObject::toString( level ) ;
		
}


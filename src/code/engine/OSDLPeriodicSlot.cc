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


#include "OSDLPeriodicSlot.h"


using namespace OSDL::Events ;
using namespace OSDL::Engine ;

using namespace Ceylan::Log ;

using std::string ;
using std::list ;




PeriodicSlot::PeriodicSlot( Period periodicity ) :
	_period( periodicity ),
	_currentSubSlot( 0 )
{

	/*
	 * This array will contain _period pointers to 
	 * ListOfPeriodicalActiveObjects instances:
	 *
	 */
	_subslots = new ListOfPeriodicalActiveObjects * [ _period ] ;
	
	_subslotWeights = new Weight[ _period ] ;
	
	// Initializes with null pointer for lists, and zero for weights:
	
	for ( Period i = 0; i < _period; i++ )
	{
		_subslots[i] = 0 ;
		_subslotWeights[ i ] = 0 ;
	}	
	
}



PeriodicSlot::~PeriodicSlot() throw() 
{

	// The lists are deleted, but not the active objects they may contain.
	
	for ( Period i = 0; i < _period; i++ )
	{
	
		if ( _subslots[i] != 0 )
		{
		
			Ceylan::Uint32 remainingCount = 0 ;
			
			// Not using size(), as null pointers must not be counted:
			for ( ListOfPeriodicalActiveObjects::const_iterator it =
				_subslots[i]->begin() ; it != _subslots[i]->end(); it++ )
			{
			
				if ( (*it) != 0 )
					remainingCount++ ;
			
			}
					
			if ( remainingCount != 0 )
				LogPlug::warning( "PeriodicSlot destructor: "
					"for slot in charge of period " 
					+ Ceylan::toString( _period ) + ", subslot #"
					+ Ceylan::toString( i ) + " was still containing "
					+ Ceylan::toString( remainingCount ) 
					+ " periodical object(s)." ) ;
				
			delete _subslots[i] ;
			
		}
			
	}
			
	delete [] _subslots ;

	delete [] _subslotWeights ;
	
}



Period PeriodicSlot::add( PeriodicalActiveObject & newObject )
{
	
	Period chosenSubSlot ;
	
	/*
	 * If strict policy is requested, schedules the object exactly at 
	 * next simulation step.
	 *
	 * If relaxed policy is allowed, schedules on least crowded slot.
	 *
	 */
	switch( newObject.getPolicy() )
	{
	
		case strict:
			chosenSubSlot = ( _currentSubSlot + 1 ) % _period ;
			break ;
			
		case relaxed:	
			chosenSubSlot = getLeastBusySubSlot() ;
			break ;
	
		default:
			throw SchedulingException( "PeriodicSlot::add: "
				"unexpected scheduling policy." ) ;
				
	}
			
	// Apply sub-slot choice for all policies:
	addInSubSlot( newObject, chosenSubSlot ) ;
	
	return chosenSubSlot ;
	
}



void PeriodicSlot::removeFromSubslot( PeriodicalActiveObject & object ) 
{

	Period subslot = object.getSubslotNumber() ;
	
	LogPlug::trace( "PeriodicSlot::removeFromSubslot: for object '"
		+ object.toString() + "', using subslot #" 
		+ Ceylan::toString(subslot) ) ;
		 
	/*
	 * The pointer to that object is replaced by a null pointer.
	 * The list element cannot be deleted yet, as this method might be
	 * called while iterating on that sub-slot.
	 *
	 */
	
	if ( _subslots[ subslot ] == 0 )
		throw SchedulingException( "PeriodicSlot::removeFromSubslot failed: "
			"sub-slot #" + Ceylan::toString( subslot ) + " does not exist." ) ;

	for ( list<PeriodicalActiveObject *>::iterator it = 
		_subslots[ subslot ]->begin(); it != _subslots[ subslot ]->end(); it++ )
	{
	
		if ( (*it) == & object )
		{
	 
	 		// Removes the first (and supposed only) occurence of this object:
			_subslotWeights[ subslot ] -= object.getWeight() ;
			*it = 0 ;
			
			LogPlug::trace( "PeriodicSlot::removeFromSubslot: "
				"removed from sub-slot #" + Ceylan::toString( subslot ) 
				+ ", new slot state is: " + toString() ) ;
				
			// No deletion here!
						
			return ;
			
		}
	}	

	throw SchedulingException( "PeriodicSlot::removeFromSubslot failed: "
		"periodical object " + object.toString() + " not found." ) ;
		
}

	

bool PeriodicSlot::onNextTick( SimulationTick newTick )
{
	
		
	/*
	 * Protects from multiple calls, if ever the scheduler duplicated ticks.
	 * 
	 * Unitary periods are special cases, since in their case the current slot
	 * never changes.
	 *
	 */
	Period deducedSubSlot = getSubSlotForSimulationTick( newTick ) ;

	/*
	LogPlug::trace( "PeriodicSlot::onNextTick for slot of period " 
		+ Ceylan::toString( _period ) + " for tick #"
		+ Ceylan::toString( newTick ) + ", using sub-slot "
		+ Ceylan::toString( deducedSubSlot ) ) ;
	 */
	 
	if ( _currentSubSlot != deducedSubSlot || _period == 1 )
	{
	
		// OK, new slot is a different one. But is it really the next slot?
		_currentSubSlot = ( _currentSubSlot + 1 ) % _period ;	

		if ( _currentSubSlot != deducedSubSlot )
			LogPlug::warning( 
				"PeriodicSlot::onNextTick: expected next sub-slot to be " 
				+ Ceylan::toString( _currentSubSlot ) + ", got " 
				+ Ceylan::toString( deducedSubSlot )
				+ " for simulation tick "
				+ Ceylan::toString( newTick ) + "." ) ;
		
		/*
		 * Nevermind, reset the current slot on all situations, and 
		 * activate accordingly:		
		 *
		 */
		_currentSubSlot = deducedSubSlot ;
		
		if ( ! activateAllObjectsInSubSlot( _currentSubSlot, newTick ) )
		{
		
			LogPlug::trace( "PeriodicSlot::onNextTick: removing sub-slot "
				+ Ceylan::toString( _currentSubSlot ) ) ; 
		
			// This sub-slot should be removed:
			delete _subslots[ _currentSubSlot ] ;
			_subslots[ _currentSubSlot ] = 0 ;

			if ( isEmpty() )
				return false ;
		
		}
		
		return true ;
		
	}
	else
	{
		
		/*
		 * First simulation tick is a special case (bootstrap), since 
		 * initial sub-slot is zero too.
		 *
		 */
		if ( newTick != 0 )
		{
		
			// The sub-slot has already been activated!
			LogPlug::warning( "PeriodicSlot::onNextTick: "
				"apparently called multiple times for simulation time " 
				+ Ceylan::toString( newTick ) 
				+ " (last activated sub-slot was "
				+ Ceylan::toString( _currentSubSlot ) 
				+ "), no more activation performed." ) ;
		}
		
		return true ;	
		
	}		
}



void PeriodicSlot::onSimulationSkipped( SimulationTick skipped ) 
{

	Period subSlot = getSubSlotForSimulationTick( skipped ) ;
	
	// Propagated the notification to all objects of the sub-slot:
	
	if ( _subslots[subSlot] != 0 )
	{
		for ( ListOfPeriodicalActiveObjects::iterator it =
				_subslots[subSlot]->begin() ;
				it != _subslots[subSlot]->end(); it++ )
			{	
			
				// Jump over any object just removed:	
				if ( *it != 0 )	
					(*it)->onSkip( skipped ) ;
				
			}		
	}
	
	// Avoidd to confuse onNextTick:
	_currentSubSlot = subSlot ;
	
}


					
Period PeriodicSlot::getPeriod() const
{

	return _period ;
	
}



bool PeriodicSlot::isEmpty() const
{

	for ( Period i = 0; i < _period; i++ )
	{	
	
		if ( _subslots[i] != 0 )
			if ( ! _subslots[i]->empty() )
				return false ;

	}
	
	return true ;
	
}



const string PeriodicSlot::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "Periodic slot whose period is " 
		+ Ceylan::toString( _period ) 
		+ ". Current sub-slot is " 
		+ Ceylan::toString( _currentSubSlot ) ; 
	
	LogPlug::trace( res ) ;
	
	if ( level == Ceylan::low )
		return res ;
	
	if ( level == Ceylan::medium )
	{
		
		Ceylan::Uint32 objectCount = 0 ;
		Weight weightCount = 0 ;
		
	
		for ( Period i = 0; i < _period; i++ )
		{	
			if ( _subslots[i] != 0 )
				objectCount += 
					static_cast<Ceylan::Uint32>( _subslots[i]->size() ) ;

			weightCount += _subslotWeights[i] ;
		}		
	
		return res + ". " + Ceylan::toString( objectCount ) 
			+ " active objects are registered in its slots, "
			"for a total weight of "
			+ Ceylan::toString( weightCount ) + "." ;
	}
		
	list<string> l ;
	
	for ( Period i = 0; i < _period; i++ )
	{	
		if ( ( _subslots[i] != 0 ) && ( ! _subslots[i]->empty() ) )
			l.push_back( "sub-slot #" + Ceylan::toString( i ) + " contains " 
				+ Ceylan::toString( 
					static_cast<Ceylan::Uint32>( _subslots[i]->size() ) ) 
				+ " active object(s), for a total weight of "
				+ Ceylan::toString( _subslotWeights[i] ) + "." ) ;
		else
			l.push_back( "subslot #" + Ceylan::toString( i ) + " is empty." ) ;	
	}		
	
	return res + Ceylan::formatStringList( l ) ;
		
}

						

Period PeriodicSlot::getSubSlotForSimulationTick( SimulationTick tick ) const
{

	return ( tick % _period ) ;
	
}



void PeriodicSlot::addInSubSlot( PeriodicalActiveObject & newObject, 
	Period targetSubslot )
{

	if ( _subslots[ targetSubslot ] == 0 )
		_subslots[ targetSubslot ] = new ListOfPeriodicalActiveObjects() ;
		
	_subslots[ targetSubslot ]->push_back( & newObject ) ;
	_subslotWeights[ targetSubslot ] += newObject.getWeight() ;
	
}



Period PeriodicSlot::getLeastBusySubSlot() const
{

	Weight minWeight = _subslotWeights[ 0 ] ;
	Period minIndex = 0 ;
	
	// Subslot #0 already taken into account:
	for ( Period i = 1; i < _period; i++ )
	{
		if ( _subslotWeights[ i ] < minWeight )
		{
			minIndex = i ;
			minWeight = _subslotWeights[ i ] ;
		}
	}
	
	return minIndex ;
	
}



bool PeriodicSlot::activateAllObjectsInSubSlot( Period subSlot,
	SimulationTick currentTime )
{

	if ( _subslots[ subSlot ] == 0 )
	{
	
		// Nothing in this sub-slot, nothing to do, no more reason of deletion:
		return true ;
		
	}
	
	/*
	 * There could be too a dynamic slip for heavy active objects with 
	 * relaxed policy.
	 *
	 */
	
	/*
	 * A while is used and the iterator is kept, as the activated object
	 * might decide to delete itself and be removed from that slot, while
	 * still iterating on this subslot.
	 *
	 */
	ListOfPeriodicalActiveObjects * subslotList = _subslots[ subSlot ] ;
	
	// while preferred as it++ must not happen at each iteration:
	ListOfPeriodicalActiveObjects::iterator it = subslotList->begin() ;
	
	/*
	LogPlug::trace( "PeriodicSlot::activateAllObjectsInSubSlot: there are "
		+ Ceylan::toString( subslotList->size() ) + " entries in sub-slot "
		+ Ceylan::toString( subSlot ) ) ;
	 */
	 	
	while( it != subslotList->end() )
	{	
		
		/*
		 * An object will return false if unsubscribing (ex: on deletion):
		 *
		 */
		if ( *it == 0 )
		{
		
			LogPlug::trace( "PeriodicSlot::activateAllObjectsInSubSlot: "
				"removing a previously emptied entry." ) ;
			
			/*
			 * The corresponding periodical object has been previously
			 * removed, let's suppress its container as well, now that we
			 * are iterating over that list (previously this could not be
			 * done as otherwise it could have invalidated a possibly in-use
			 * iterator):
			 *
			 */
			it = subslotList->erase( it ) ;
			
		}
		else
		{
		 
		 	(*it)->onActivation( currentTime ) ;
			it++ ;
			
		}
			
	}

	// Empty sub-slots should be removed:
	if ( subslotList->empty() )
		return false ;
			
	// Holds still at least one object:
	return true ;
	
}


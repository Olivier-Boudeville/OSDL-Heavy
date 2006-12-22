#include "OSDLPeriodicSlot.h"

using namespace Ceylan::Log ;
using namespace OSDL::Events ;
using namespace OSDL::Engine ;

using std::string ;
using std::list ;




PeriodicSlot::PeriodicSlot( Period periodicity ) throw() :
	_period( periodicity ),
	_currentSlot( 0 )
{

	// This array will contain _period pointers to listActiveObjects instances :
	_slots = new listActiveObjects *[ _period ] ;
	
	_slotWeights = new Weight[ _period ] ;
	
	// Initialize with null pointer for lists and zero for weights :
	
	for ( Period i = 0; i < _period; i++ )
	{
		_slots[i] = 0 ;
		_slotWeights[ i ] = 0 ;
	}	
	
}


PeriodicSlot::~PeriodicSlot() throw() 
{

	// The lists are deleted, but not the active objects they may contain.
	
	for ( Period i = 0; i < _period; i++ )
	{
		if ( _slots[i] != 0 )
			delete _slots[i] ;
	}
			
	delete [] _slots ;

	delete [] _slotWeights ;
	
	
}


void PeriodicSlot::add( ActiveObject & newObject ) throw()
{
	
	Period chosenSlot ;
	
	// If strict policy is requested, schedules the object exactly at next simulation step :
	if ( newObject.getPolicy() == strict )
		chosenSlot = ( _currentSlot + 1 ) % _period ;
	
	// If relaxed policy is allowed, schedule on least crowded slot :	
	if ( newObject.getPolicy() == relaxed )	
		chosenSlot = getLeastBusySlot() ;
	
	
	// Apply slot choice for all policies :
	addInSlot( newObject, chosenSlot ) ;
			
}


void PeriodicSlot::remove( ActiveObject & object ) throw( SchedulingException )
{
	bool removed = false ;
	
	for ( Period i = 0; i < _period; i++ )
	{
		if ( removeFromSlot( object, i ) )
			removed = true ;
	}
	
	if ( ! removed )
		throw SchedulingException( "PeriodicSlot::remove : all slots inspected for object ("
			+ object.toString( Ceylan::low ) + "), but was never found." ) ;
}


void PeriodicSlot::onNextTick( Events::SimulationTick newTick ) throw()
{
	
	/*
	 * Protect from multiple calls. Unitary periods are special cases, since the current slot 
	 * never changes.
	 *
	 */
	
	if ( _currentSlot != getSubSlotForSimulationTick( newTick ) || _period == 1 )
	{
		// OK, it is not the last slot again. But is it really the next slot ?
		_currentSlot = ( _currentSlot + 1 ) % _period ;	

		if ( _currentSlot != getSubSlotForSimulationTick( newTick ) )
			LogPlug::warning( "PeriodicSlot::onNextTick : expected next sub-slot to be " 
				+ Ceylan::toString( _currentSlot ) + ", got " 
				+ Ceylan::toString( getSubSlotForSimulationTick( newTick ) )
				+ " for simulation tick "
				+ Ceylan::toString( newTick ) + "." ) ;
		
		// Nevermind, reset the current slot on all situations, and activate accordingly :		
		_currentSlot = getSubSlotForSimulationTick( newTick ) ;
		activateAllObjectsInSubSlot( _currentSlot, newTick ) ;
	}
	else
	{
		
		// First simulation tick is a special case (bootstrap), since initial sub-slot is zero too.
		
		if ( newTick != 0 )
		{
			// The sub-slot has already been activated !
			LogPlug::warning( "PeriodicSlot::onNextTick : apparently called multiple times for "
				"simulation time " + Ceylan::toString( newTick ) + " (last activated sub-slot was "
				+ Ceylan::toString( _currentSlot ) 
				+ "), no more activation performed." ) ;
		}		
		
	}		
}


void PeriodicSlot::onSimulationSkipped( SimulationTick skipped ) throw( SchedulingException )
{

	Period subSlot = getSubSlotForSimulationTick( skipped ) ;
	
	// Propagate the notification to all objects of the sub-slot :
	
	if ( _slots[subSlot] != 0 )
	{
		for ( listActiveObjects::iterator it = _slots[subSlot]->begin() ;
				it != _slots[subSlot]->end(); it++ )
			(*it)->onSkip( skipped ) ;
	}
	
	// Avoid to confuse onNextTick :
	_currentSlot = subSlot ;
}

					
Period PeriodicSlot::getPeriod() throw()
{
	return _period ;
}


const string PeriodicSlot::toString( Ceylan::VerbosityLevels level ) const throw()
{

	string res = "Periodic slot whose period is " + Ceylan::toString( _period ) 
		+ ". Current sub-slot is " + Ceylan::toString( _currentSlot ) ; 
		
	if ( level == Ceylan::low )
		return res ;
	
	if ( level == Ceylan::medium )
	{
		
		unsigned int objectCount = 0 ;
		Weight weightCount = 0 ;
		
	
		for ( Period i = 0; i < _period; i++ )
		{	
			if ( _slots[i] != 0 )
				objectCount += _slots[i]->size() ;
			weightCount += _slotWeights[i] ;
		}		
	
		return res + ". " + Ceylan::toString( objectCount ) 
			+ " active objects are registered in its slots, for a total weight of "
			+ Ceylan::toString( weightCount ) + "." ;
	}
		
	list<string> l ;
	
	for ( Period i = 0; i < _period; i++ )
	{	
		if ( _slots[i] != 0 && _slots[i]->size() != 0 )
			l.push_back( "slot #" + Ceylan::toString( i ) + " contains " 
				+ Ceylan::toString( _slots[i]->size() ) 
				+ " active object(s), for a total weight of "
				+ Ceylan::toString( _slotWeights[i] ) + "." ) ;
		else
			l.push_back( "slot #" + Ceylan::toString( i ) + " is empty." ) ;	
	}		
	
	return res + Ceylan::formatStringList( l ) ;
		
}
						

Period PeriodicSlot::getSubSlotForSimulationTick( SimulationTick tick ) const throw()
{
	return ( tick % _period ) ;
}


void PeriodicSlot::addInSlot( ActiveObject & newObject, Period targetSlot ) throw()
{
	if ( _slots[ targetSlot ] == 0 )
		_slots[ targetSlot ] = new listActiveObjects() ;
		
	_slots[ targetSlot ]->push_back( & newObject ) ;
	_slotWeights[ targetSlot ] += newObject.getWeight() ;
}


bool PeriodicSlot::removeFromSlot( ActiveObject & object, Period targetSlot )
	throw( SchedulingException )
{
	
	if ( _slots[ targetSlot ] == 0 )
		return false ;
		
	unsigned int count = 0 ;
	
	/* 
	 * Removes all occurences of this object, must use two passes since otherwise an erase 
	 * would invalidate the iterator.
	 *
	 */
	for ( listActiveObjects::iterator it = _slots[ targetSlot ]->begin() ; 
		it != _slots[ targetSlot ]->end(); it++ )
	{
		if ( (*it) == & object )
			count++ ;
	}
	_slots[ targetSlot ]->remove( & object ) ;
	
	
	// An empty slot should be deleted :	
	if ( _slots[ targetSlot ]->empty() )
	{
		delete _slots[ targetSlot ] ;
		_slots[ targetSlot ] = 0 ;
		// _slotWeights will be zero on method return.
	}		
	
	// Update the precomputed weight :		
	_slotWeights[ targetSlot ] -= object.getWeight() * count ;
	
	return ( count != 0 ) ;

}


Period PeriodicSlot::getLeastBusySlot() const throw()
{

	Weight minWeight = _slotWeights[ 0 ] ;
	Period minIndex = 0 ;
	
	for ( Period i = 1; i < _period; i++ )
	{
		if ( _slotWeights[ i ] < minWeight )
		{
			minIndex = i ;
			minWeight = _slotWeights[ i ] ;
		}
	}
	
	return minIndex ;
	
}


void PeriodicSlot::activateAllObjectsInSubSlot( Period subSlot, Events::SimulationTick currentTime )
	throw()
{

	if ( _slots[ subSlot ] == 0 )
	{
		// Nothing in this sub-slot, nothing to do !
		return ;
	}
	
	// There could be too a dynamic slip for heavy active objects with relaxed policy.
	
	for ( listActiveObjects::iterator it = _slots[ subSlot ]->begin();
		it != _slots[ subSlot ]->end(); it++ )
	{	
		(*it)->onActivation( currentTime ) ;	
	}

}

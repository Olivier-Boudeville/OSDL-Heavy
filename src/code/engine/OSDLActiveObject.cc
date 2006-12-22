#include "OSDLActiveObject.h"


#include "OSDLScheduler.h"        // for GetExistingScheduler



using std::string ;
using std::list ;


#include "Ceylan.h"               // for Log
using namespace Ceylan::Log ;


using namespace OSDL::Engine ;
using namespace OSDL::Events ;



ActiveObject::ActiveObject( Period period, ObjectSchedulingPolicy policy, 
		Weight weight ) throw() : 
	_policy( policy ),
	_weight( weight ),
	_period( period ),
	_programmedTriggerTicks( 0 ),
	_absoluteTriggers( true ),
	_birthTime( 0 )
{
	
}


ActiveObject::ActiveObject( const list<SimulationTick> & triggeringTicks, 
		bool absolutlyDefined, ObjectSchedulingPolicy policy, Weight weight ) throw() :
	_policy( policy ),
	_weight( weight ),
	_period( 0 ),
	_programmedTriggerTicks( 0 ),
	_absoluteTriggers( absolutlyDefined ),
	_birthTime( 0 )
{
	_programmedTriggerTicks = new list<SimulationTick>( triggeringTicks ) ;
}	


ActiveObject::ActiveObject( SimulationTick triggerTick, bool absolutlyDefined, 
		ObjectSchedulingPolicy policy, Weight weight ) throw() :
	_policy( policy ),
	_weight( weight ),
	_period( 0 ),
	_programmedTriggerTicks( 0 ),
	_absoluteTriggers( absolutlyDefined ),
	_birthTime( 0 )

{
	_programmedTriggerTicks = new list<SimulationTick> ;
	_programmedTriggerTicks->push_back( triggerTick ) ;
}	


ActiveObject::~ActiveObject() throw()
{
	if ( _programmedTriggerTicks != 0 )
		delete _programmedTriggerTicks ;
}


ObjectSchedulingPolicy ActiveObject::getPolicy() const throw()
{
	return _policy ;
}


Weight ActiveObject::getWeight() const throw()
{
	return _weight ;
}


Period ActiveObject::getPeriod() const throw()
{
	return _period ;
}


void ActiveObject::setPeriod( Period newPeriod ) throw()
{
	_period = newPeriod ;
}


Hertz ActiveObject::setFrequency( Hertz newFrequency ) throw( SchedulingException )
{

	// Throw exception if scheduler not already existing :
	Scheduler & scheduler = Scheduler::GetExistingScheduler() ;
	
	/*
	 * Target period is 1/newFrequency, one needs to divide it by simulation period duration
	 * to know how many simulation periods it will need :
	 * _period = (1/newFrequency) / simulatickDuration = simulatickDuration / newFrequency
	 *
	 * Microsecond simulatickDuration = 
	 * 	scheduler.getSimulationTickCount() * scheduler.getTimeSliceDuration() ;
	 *
	 */
	
		
	_period = static_cast<Period>( Ceylan::Maths::Round( 1000000.0f / 
	 	( newFrequency * scheduler.getSimulationTickCount() * scheduler.getTimeSliceDuration() ) )
	) ;
	 
	 // Clamp for too high frequencies :
	 if ( _period < 1 )
	 	_period = 1 ;
	 
	 
	 /*
	  * newFrequency = simulatickDuration / _period
	  *
	  *
	  */
	 return static_cast<Hertz>( Ceylan::Maths::Round( 1000000.0f / 
	 	( _period * scheduler.getSimulationTickCount() * scheduler.getTimeSliceDuration() ) ) ) ;
		
}


bool ActiveObject::hasProgrammedActivations() const throw()
{
	return ! ( _programmedTriggerTicks == 0 || _programmedTriggerTicks->size() == 0 ) ;
}


bool ActiveObject::areProgrammedActivationsAbsolute() const throw()
{
	return _absoluteTriggers ;
}


void ActiveObject::absoluteProgrammedActivationsWanted( bool on ) throw()
{
	_absoluteTriggers = on ;
}


const list<SimulationTick> & ActiveObject::getProgrammedActivations()
	const throw( SchedulingException ) 
{
	if ( _programmedTriggerTicks == 0 )
		throw SchedulingException( "ActiveObject::getProgrammedActivations : "
			"no activation available." ) ;
			
	// Can be an empty list :		
	return * _programmedTriggerTicks ;
	
}


void ActiveObject::setProgrammedActivations( const list<SimulationTick> & newActivationsList )
	throw()	
{

	if ( _programmedTriggerTicks != 0 )
		delete _programmedTriggerTicks ;
	
	_programmedTriggerTicks = new list<SimulationTick>( newActivationsList ) ;
}


void ActiveObject::addProgrammedActivations( 
	const list<SimulationTick> & additionalActivationsList ) throw()	
{
	if ( _programmedTriggerTicks != 0 )
	{
		// Do not trust 'merge' to be efficient :
		for ( list<SimulationTick>::const_iterator it = _programmedTriggerTicks->begin();
				it != _programmedTriggerTicks->end(); it++ )
			_programmedTriggerTicks->push_back( *it ) ;
	}	
	else
		_programmedTriggerTicks = new list<SimulationTick>( additionalActivationsList ) ;
}


void ActiveObject::setUniqueProgrammedTicks() throw()
{
	if ( _programmedTriggerTicks != 0 )
		_programmedTriggerTicks->unique() ;
}

	
SimulationTick ActiveObject::getBirthTime() const throw()
{
	return _birthTime ;
}


void ActiveObject::setBirthTime( SimulationTick birthSimulationTick ) throw()
{
	_birthTime = birthSimulationTick ;
}


void ActiveObject::onSkip( SimulationTick skippedStep ) throw( SchedulingException )
{
	LogPlug::warning( "An active object (" + toString( Ceylan::low )
		+ ") had his simulation tick " + Ceylan::toString( skippedStep ) + " skipped." ) ; 
}


void ActiveObject::onImpossibleActivation( SimulationTick missedStep ) 
	throw( SchedulingException )
{
	throw SchedulingException( "Active object (" + toString( Ceylan::low )
		+ ") had his activation failed for simulation tick " + Ceylan::toString( missedStep ) 
		+ "." ) ; 
}


const string ActiveObject::toString( Ceylan::VerbosityLevels level ) const throw()
{	
	return "Active object, whose date of birth is " 
		+ Ceylan::toString( _birthTime ) ;
}


#include "OSDLActiveObject.h"


#include "OSDLScheduler.h"        // for GetExistingScheduler



using std::string ;
using std::list ;


#include "Ceylan.h"               // for Log
using namespace Ceylan::Log ;


using namespace OSDL::Engine ;
using namespace OSDL::Events ;




ActiveObject::ActiveObject( ObjectSchedulingPolicy policy, Weight weight )
		throw():
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


ObjectSchedulingPolicy ActiveObject::getPolicy() const throw()
{

	return _policy ;
	
}



Weight ActiveObject::getWeight() const throw()
{

	return _weight ;
	
}


	
void ActiveObject::setBirthTick( 
	Events::SimulationTick birthSimulationTick ) throw( SchedulingException )
{

	_birthTick = birthSimulationTick ;
	
}


		
SimulationTick ActiveObject::getBirthTick() const throw()
{

	return _birthTick ;
	
}



void ActiveObject::onSkip( SimulationTick skippedStep ) 
	throw( SchedulingException )
{

	LogPlug::warning( "An active object (" 
		+ toString( Ceylan::low )+ ") had his simulation tick " 
		+ Ceylan::toString( skippedStep ) + " skipped." ) ; 
		
}



void ActiveObject::onImpossibleActivation( SimulationTick missedStep ) 
	throw( SchedulingException )
{

	throw SchedulingException( "Active object (" + toString( Ceylan::low )
		+ ") could not be activated for simulation tick " 
		+ Ceylan::toString( missedStep ) + "." ) ; 
		
}



const string ActiveObject::toString( Ceylan::VerbosityLevels level )
	const throw()
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


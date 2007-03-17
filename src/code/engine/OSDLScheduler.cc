#include "OSDLScheduler.h"

#include "OSDLBasic.h"                // for getExistingCommonModule
#include "OSDLVideo.h"                // for redraw
#include "OSDLRenderer.h"             // for Renderer
#include "OSDLActiveObject.h"         // for ActiveObject


#include <iostream>                   // for ostringstream
using std::ostringstream ;

#include <iomanip> 				      // for std::ios::fixed



using namespace Ceylan::Log ;         // for LogPlug

// for time units and calls (ex : Millisecond, basicSleep) :
using namespace Ceylan::System ;      
 
using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;

using std::string ;
using std::list ;
using std::map ;


Scheduler * Scheduler::_internalScheduler = 0 ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>               // for OSDL_DEBUG_SCHEDULER and al
#endif // OSDL_USES_CONFIG_H



#if OSDL_DEBUG_SCHEDULER

#include <iostream>
#define OSDL_SCHEDULE_LOG(message) std::cerr << message << std::endl ;

#else // OSDL_DEBUG_SCHEDULER

#define OSDL_SCHEDULE_LOG(message)

#endif // OSDL_DEBUG_SCHEDULER



bool Scheduler::hasRenderer() const throw()
{

	return ( _renderer != 0 ) ;
	
}


Renderer & Scheduler::getRenderer() const throw( SchedulingException )
{

	if ( _renderer != 0 )
		return * _renderer ;
	else
		throw SchedulingException( 
			"Scheduler::getRenderer : no renderer available." ) ;
		
}


void Scheduler::setRenderer( Rendering::Renderer & newRenderer ) throw()
{

	if ( _renderer != 0 )
		delete _renderer ;
	
	_renderer = & newRenderer ;
		
}



void Scheduler::setScreenshotMode( bool on, const string & frameFilenamePrefix, 
	Hertz frameFrequency ) throw()
{

	_screenshotMode = on ;
	_frameFilenamePrefix = frameFilenamePrefix ;
	
}

					
void Scheduler::setTimeSliceDuration( Microsecond engineTickDuration ) throw()
{

	_engineTickDuration = engineTickDuration ;
	
	// Recomputes all the ticks that depends on the engine tick :
	setSimulationFrequency(   _desiredSimulationFrequency ) ;
	setRenderingFrequency(    _desiredRenderingFrequency ) ;
	setScreenshotFrequency(   _desiredScreenshotFrequency ) ;
	setInputPollingFrequency( _desiredInputFrequency ) ;
	
}


Microsecond Scheduler::getTimeSliceDuration() const throw()
{

	return _engineTickDuration ;
	
}


void Scheduler::setSimulationFrequency( Hertz frequency ) 
	throw( SchedulingException )
{

	/*
	 * f Hz correspond to a period of 1E6/f microseconds , therefore to
	 * 1E6 / ( f * _engineTickDuration ) engine ticks per period.
	 *
	 * For example, if f = 100 Hz and engine tick duration is 1000 microseconds,
	 * there are 1E6/(100*1000) = 10 engine ticks for each simulation tick.
	 *
	 */
	 
	 if ( frequency * _engineTickDuration > 1E6 )
	 	throw SchedulingException( "Scheduler::setSimulationFrequency : "
			" requested simulation frequency (" 
			+ Ceylan::toString( frequency ) 
			+ " Hz) is too high for engine tick,"
			" which lasts for " 
			+ Ceylan::toString( _engineTickDuration ) 
			+ " microseconds." ) ;
	
	_desiredSimulationFrequency = frequency	;
	_simulationPeriod = static_cast<Period>( 
		1E6 / ( frequency * _engineTickDuration ) ) ;

#if OSDL_DEBUG

	LogPlug::debug( "Scheduler::setSimulationFrequency : for a requested "
		"simulation frequency of " + Ceylan::toString( frequency ) 
		+ " Hz, the simulation period corresponds to " 
		+ Ceylan::toString( _simulationPeriod ) + " engine ticks." ) ;
		
#endif // OSDL_DEBUG
	
}


Period Scheduler::getSimulationTickCount() const throw()
{

	return _simulationPeriod ;
	
}


void Scheduler::setRenderingFrequency( Hertz frequency ) 
	throw( SchedulingException )
{

	/*
	 * f Hz correspond to 1E6/f microseconds, therefore to
	 * 1E6 / ( f * _engineTickDuration ) engine ticks.
	 *
	 * For example, if f = 40 Hz = 40 frames per seconds and engine tick
	 * duration is 1000 microseconds, there are 1E6/(40*1000) = 25 engine 
	 * ticks for each rendering tick.
	 *
	 */
	 
	 if ( frequency * _engineTickDuration > 1E6 )
	 	throw SchedulingException( "Scheduler::setRenderingFrequency : "
			"requested rendering frequency (" 
			+ Ceylan::toString( frequency ) 
			+ " Hz) is too high for engine tick,"
			" which lasts for " 
			+ Ceylan::toString( _engineTickDuration ) 
			+ " microseconds." ) ;
			
	_desiredRenderingFrequency = frequency ;		
	_renderingPeriod = static_cast<Period>( 
		1E6 / ( frequency * _engineTickDuration ) ) ;

#if OSDL_DEBUG

	LogPlug::debug( "Scheduler::setRenderingFrequency : for a requested "
		"rendering frequency of " + Ceylan::toString( frequency ) 
		+ " Hz, the rendering period corresponds to " 
		+ Ceylan::toString( _renderingPeriod ) + " engine ticks." ) ;
		
#endif // OSDL_DEBUG

}


Period Scheduler::getRenderingTickCount() const throw()
{

	return _renderingPeriod ;
	
}


void Scheduler::setScreenshotFrequency( Hertz frequency ) 
	throw( SchedulingException )
{

	/*
	 * f Hz correspond to a period of 1E6/f microseconds , therefore to
	 * 1E6 / ( f * _engineTickDuration ) engine ticks per period.
	 *
	 * For example, if f = 25 Hz = 25 frames per seconds, and engine tick
	 * duration is 1000 microseconds, there are 1E6/(25*1000) = 40 engine 
	 * ticks for each screenshot tick.
	 *
	 * Screenshot task will be implemented as a periodic object calling
	 * rendering methods.
	 *
	 */

	 
	 if ( frequency * _engineTickDuration > 1E6 )
	 	throw SchedulingException( "Scheduler::setScreenshotFrequency : "
			" requested screenshot frequency (" 
			+ Ceylan::toString( frequency )
			+ " Hz) is too high for engine tick,"
			" which lasts for " + Ceylan::toString( _engineTickDuration ) 
			+ " microseconds." ) ;
			
	_desiredScreenshotFrequency = frequency ;		
	_screenshotPeriod = static_cast<Period>( 
		1E6 / ( frequency * _engineTickDuration ) ) ;


#if OSDL_DEBUG

	LogPlug::debug( "Scheduler::setScreenshotFrequency : for a requested "
		"screenshot frequency of " + Ceylan::toString( frequency ) 
		+ " Hz, the screenshot period corresponds to " 
		+ Ceylan::toString( _screenshotPeriod ) + " engine ticks." ) ;
		
#endif // OSDL_DEBUG

}


Period Scheduler::getScreenshotTickCount() const throw()
{

	return _screenshotPeriod ;
	
}


void Scheduler::setInputPollingFrequency( Hertz frequency ) 
	throw( SchedulingException )
{

	/*
	 * f Hz correspond to a period of 1E6/f microseconds , therefore to
	 * 1E6 / ( f * _engineTickDuration ) engine ticks per period.
	 *
	 * For example, if f = 20 Hz = 20 frames per seconds and engine tick
	 * duration is 1000 microseconds, there are 10E6/(20*1000) = 50 engine 
	 * ticks for each input tick.
	 *
	 */
	 
	 if ( frequency * _engineTickDuration > 1E6 )
	 	throw SchedulingException( 
			"Scheduler::setInputPollingFrequency : requested input "
			"frequency (" + Ceylan::toString( frequency ) 
			+ " Hz) is too high for engine tick,"
			" which lasts for " 
			+ Ceylan::toString( _engineTickDuration ) + " microseconds." ) ;
			
	_desiredInputFrequency = frequency ;		
	_inputPeriod = static_cast<Period>( 
		1E6 / ( frequency * _engineTickDuration ) ) ;

#if OSDL_DEBUG

	LogPlug::debug( "Scheduler::setInputPollingFrequency : for a requested "
		"input frequency of " + Ceylan::toString( frequency ) 
		+ " Hz, the input period corresponds to " 
		+ Ceylan::toString( _inputPeriod ) + " engine ticks." ) ;
		
#endif // OSDL_DEBUG

}


void Scheduler::setIdleCallback( 
		Ceylan::System::Callback idleCallback, 
		void * callbackData, 
		Ceylan::System::Microsecond callbackExpectedMaxDuration )
	throw()
{

	_idleCallback            = idleCallback ;
	_idleCallbackData        = callbackData ;
	
	if ( callbackExpectedMaxDuration != 0 )
		_idleCallbackMaxDuration = callbackExpectedMaxDuration ;
	else
		_idleCallbackMaxDuration = EventsModule::EvaluateCallbackduration(
			idleCallback, callbackData ) ;
	
}
					
					
Period Scheduler::getInputPollingTickCount() const throw()
{

	return _inputPeriod ;
	
}


EngineTick Scheduler::getCurrentEngineTick() const throw() 
{

	return _currentEngineTick ;
	
}


SimulationTick Scheduler::getCurrentSimulationTick() const throw()
{

	return _currentSimulationTick ;
	
}


RenderingTick Scheduler::getCurrentRenderingTick() const throw()
{

	return _currentRenderingTick ;
	
}


RenderingTick Scheduler::getCurrentInputTick() const throw()
{

	return _currentInputTick ;
	
}


void Scheduler::registerObject( ActiveObject & objectToRegister ) throw()
{

#if OSDL_DEBUG
	if ( objectToRegister.getPeriod() == 0 
			&& ! objectToRegister.hasProgrammedActivations() )
		LogPlug::error( "Scheduler::registerObject : "
			"following object will never be activated : "
			+ objectToRegister.toString() ) ;
#endif // OSDL_DEBUG
	
	
	if ( objectToRegister.getPeriod() != 0 )
	{
		// This active object is to be periodically activated :
		PeriodicSlot & slot = getPeriodicSlotFor( 
			objectToRegister.getPeriod() ) ;
			
		slot.add( objectToRegister ) ;	

	}
	
	
	if ( objectToRegister.hasProgrammedActivations() )
	{
	
		// Declare the programmed ticks for this object :
		
		const list<SimulationTick> & programmed =
			objectToRegister.getProgrammedActivations() ;
		
		for ( list<SimulationTick>::const_iterator it = programmed.begin(); 
			it != programmed.end(); it++ )
		{
			programTriggerFor( objectToRegister, (*it) ) ;			
		}
			
	}
	
	objectToRegister.setBirthTime( _currentSimulationTick ) ;
	
}


void Scheduler::schedule() throw( SchedulingException )
{
	
	try
	{
	
		_eventsModule = & OSDL::getExistingCommonModule().getEventsModule() ;
		
	}
	catch( OSDL::Exception & e )
	{
		throw SchedulingException( "Scheduler::schedule : "
			"no events module available." ) ;
	}
	
	
	if ( _renderer != 0 )
	{
		send( "Scheduling now, using renderer : " + _renderer->toString() ) ;
	}
	else
	{
		// Retrieve video module, used instead of the renderer :
		try
		{
			_videoModule = & OSDL::getExistingCommonModule().getVideoModule() ;
		}
		catch( OSDL::Exception & e )
		{
			throw SchedulingException( "Scheduler::schedule : "
				"no renderer nor video module available." ) ;
		}
		
		send( "Scheduling now, using video module (no renderer)." ) ;
		
	}
	
		
	if ( _screenshotMode )
		scheduleNoDeadline() ;
	else
		scheduleBestEffort() ;	
		
}


void Scheduler::stop() throw()
{

	_stopRequested = true ;
	
}


const string Scheduler::toString( Ceylan::VerbosityLevels level ) const throw()
{	

	ostringstream buf ;
	buf.precision( 2 ) ;

	buf << setiosflags( std::ios::fixed ) 
		<< "Basic scheduler, whose internal frequency is " 
		<< 1E6 / _engineTickDuration
		<< " Hz (engine tick duration is " 
		+ Ceylan::toString( _engineTickDuration )
		+ " microseconds). Current engine tick is " 
		+ Ceylan::toString( _currentEngineTick ) 
		+ ", current simulation tick is "
		+ Ceylan::toString( _currentSimulationTick )
		+ ", current rendering tick is "
		+ Ceylan::toString( _currentRenderingTick ) 
		+ " and current input tick is " 
		+ Ceylan::toString( _currentInputTick )
		+ ". Screenshot mode is "
		+ ( _screenshotMode ? "on." : "off." ) ;
	
					
	if ( _scheduleStartingSecond == 0 && _scheduleStartingMicrosecond == 0 )
		buf << " Scheduler is stopped." ;
	else
	{
	
		Ceylan::System::Second sec ;
		Ceylan::System::Microsecond microsec ;
		Ceylan::System::getPreciseTime( sec, microsec ) ;
		
		buf << " Scheduler has been running for " 
			<< Ceylan::System::durationToString( _scheduleStartingSecond,
				_scheduleStartingMicrosecond, sec, microsec )
			<< "." ;

	}


	if ( _renderer != 0 )
		buf << " Using a renderer (" 
			+ _renderer->toString( Ceylan::low ) + ")" ;
	else
		buf << " No renderer registered, using directly video module" ;

	if ( _idleCallback == 0 )
		buf << ". Using atomic sleep idle callback" ;
	else
		buf << ". Using user-specified idle callback" ;
	
	if ( _idleCallbackData )
		if ( _idleCallback )
			buf << ", callback data has been set as well." ;
		else
			buf << ", callback data has been set, "
				" whereas it is not used by default idle callback (abnormal)." ;

	buf << ". The estimated upper-bound for idle callback duration is "
		+ Ceylan::toString( _idleCallbackMaxDuration )
		+ " microseconds" ;
				
				
	if ( level == Ceylan::low )
		return buf.str() ;
			
	// Do not use desired frequencies, compute them instead from the periods :
			
	buf <<  ". User-defined simulation frequency is " 
		+ Ceylan::toString( 
			1E6 / 
				( _simulationPeriod * _engineTickDuration ), 
			/* precision */ 2 )
		+ " Hz (a period of " 
		+ Ceylan::toString( _simulationPeriod ) 
		+ " engine ticks), rendering frequency is "
		+ Ceylan::toString( 
			1E6 / 
				( _renderingPeriod * _engineTickDuration ),
			/* precision */ 2 ) 
		+ " Hz (a period of " 
		+ Ceylan::toString( _renderingPeriod ) 
		+ " engine ticks), input polling frequency is "
		+ Ceylan::toString( 1E6 /
				( _inputPeriod * _engineTickDuration ),
			/* precision */ 2 ) 
		+ " Hz (a period of " 
		+ Ceylan::toString( _inputPeriod ) 
		+ " engine ticks). There are "
		+ Ceylan::toString( _periodicSlots.size() ) 
		+ " used periodic slot(s) and "
		+ Ceylan::toString( _programmedActivated.size() ) 
		+ " programmed object(s)" ;
		
		
	if ( level == Ceylan::medium )
		return buf.str() ;
		
		
	if ( ! _periodicSlots.empty() )
	{
		
		buf << ". Enumerating scheduling periodic slots : " ;
		
		std::list<string> slots ;
		
		for ( list<PeriodicSlot*>::const_iterator it = _periodicSlots.begin(); 
				it != _periodicSlots.end(); it++ )
			slots.push_back( (*it)->toString( level ) ) ;

		buf << Ceylan::formatStringList( slots ) ;
			
	}
	
	if ( ! _programmedActivated.empty() )
	{
	
		buf << ". Enumerating scheduling programmed slots : " ;
		
		std::list<string> programmed ;
		
		for( map<SimulationTick, ListOfActiveObjects>::const_iterator it 
				= _programmedActivated.begin(); it 
					!= _programmedActivated.end(); it++ )
			programmed.push_back( "For simulation tick #" 
				+ Ceylan::toString( (*it).first )
				+ ", there are " + Ceylan::toString( (*it).second.size() ) 
				+ " object(s) programmed" ) ;
				
		buf << Ceylan::formatStringList( programmed ) ;
		
	}	
	
	
	if ( _screenshotMode )
	{
		buf << ". Current movie frame period for screenshot mode is " 
			+ Ceylan::toString( _screenshotPeriod ) 
			+ " engine ticks, which corresponds to a frequency of "
			+ Ceylan::toString( 1E6 /
				( _screenshotPeriod * _engineTickDuration ) )
			+ " frames per second" ;	

		// No simulation nor rendering tick can be missed in screenshot mode.
		return buf.str() ;
	}
	
		
	if ( _missedSimulationTicks == 0 )
		buf << ". No simulation tick was missed" ;
	else
	{
	
		Ceylan::System::Second sec ;
		Ceylan::System::Microsecond microsec ;
		Ceylan::System::getPreciseTime( sec, microsec ) ;

		
		buf << ". " + Ceylan::toString( _missedSimulationTicks ) 
			+ " simulation ticks were missed, "
			"it sums up to an actual average simulation frequency of " 	
			<< 1E6 * ( _currentSimulationTick - _missedSimulationTicks ) / 
				static_cast<Ceylan::Float64>( 
					( sec - _scheduleStartingSecond ) * 1E6
					+ microsec - _scheduleStartingMicrosecond ) 
			<< " Hz" ;	
							
	}	
	 	
		
	if ( _missedRenderingTicks == 0 )
		buf << ". No rendering tick was missed" ;
	else
	{
	
		Ceylan::System::Second sec ;
		Ceylan::System::Microsecond microsec ;
		Ceylan::System::getPreciseTime( sec, microsec ) ;

		buf << ". " + Ceylan::toString( _missedRenderingTicks ) 
			+ " rendering ticks were missed, "
			"it sums up to an actual average rendering frequency of "
			<< 1E6 * ( _currentRenderingTick - _missedRenderingTicks ) / 
				static_cast<Ceylan::Float64>( 
					( sec - _scheduleStartingSecond ) * 1E6
					+ microsec - _scheduleStartingMicrosecond )
			<< " Hz" ;	
	}
		 	
			
	if ( _missedInputTicks == 0 )
		buf << ". No input tick was missed" ;
	else
	{
	
		Ceylan::System::Second sec ;
		Ceylan::System::Microsecond microsec ;
		Ceylan::System::getPreciseTime( sec, microsec ) ;

		buf << ". " + Ceylan::toString( _missedInputTicks ) 
			+ " input ticks were missed, "
			"it sums up to an actual average input frequency of "
			<< 1E6 * ( _currentInputTick - _missedInputTicks ) / 
				static_cast<Ceylan::Float64>( 
					( sec - _scheduleStartingSecond ) * 1E6
					+ microsec - _scheduleStartingMicrosecond ) 
			<< " Hz" ;	
	}	 	
	
	return buf.str() ;
				
}



// Static section.


Scheduler & Scheduler::GetExistingScheduler() throw( SchedulingException )
{

    if ( Scheduler::_internalScheduler == 0 )
		throw SchedulingException( 
			"Scheduler::GetExistingScheduler : no scheduler available." ) ;
			
    return * Scheduler::_internalScheduler ;

}


Scheduler & Scheduler::GetScheduler() throw()
{

    if ( Scheduler::_internalScheduler == 0 )
    {
	
        LogPlug::debug( "Scheduler::GetScheduler : "
			"no scheduler available, creating new one" ) ;
			
        Scheduler::_internalScheduler = new Scheduler() ;
		
    }
    else
    {
        LogPlug::debug( "Scheduler::GetScheduler : "
			"returning already constructed instance of scheduler, "
			"no creation." ) ;
    }

    LogPlug::debug( "Scheduler::GetScheduler : returning Scheduler instance "
		+ Ceylan::toString( Scheduler::_internalScheduler ) ) ;

    return * Scheduler::_internalScheduler ;

}


void Scheduler::DeleteExistingScheduler() throw( SchedulingException )
{

    if ( Scheduler::_internalScheduler != 0 )
		throw SchedulingException( "Scheduler::DeleteExistingScheduler : "
			"there was no already existing scheduler." ) ;
			
#if OSDL_DEBUG_SCHEDULER
    LogPlug::debug( 
		"Scheduler::DeleteExistingScheduler : effective deleting." ) ;
#endif // OSDL_DEBUG_SCHEDULER
		
    delete Scheduler::_internalScheduler ;
	Scheduler::_internalScheduler = 0 ;

}


void Scheduler::DeleteScheduler() throw()
{

    if ( Scheduler::_internalScheduler != 0 )
    {
	
#if OSDL_DEBUG_SCHEDULER
        LogPlug::debug( "Scheduler::DeleteScheduler : effective deleting." ) ;
#endif // OSDL_DEBUG_SCHEDULER
		
        delete Scheduler::_internalScheduler ;
		Scheduler::_internalScheduler = 0 ;
    }
    else
    {
	
#if OSDL_DEBUG_SCHEDULER
        LogPlug::debug( "Scheduler::DeleteScheduler : no deleting needed." ) ;
#endif // OSDL_DEBUG_SCHEDULER
		
    }

}




// Protected members below :


Scheduler::Scheduler() throw() :
	_screenshotMode( false ),
	_desiredScreenshotFrequency( DefaultMovieFrameFrequency ),
	_screenshotPeriod( 0 ),
	_periodicSlots(),
	_programmedActivated(),
	_engineTickDuration( 0 ),
	_currentEngineTick( 0 ),
	_currentSimulationTick( 0 ),
	_currentRenderingTick( 0 ),
	_currentInputTick( 0 ),
	_simulationPeriod( 0 ),
	_renderingPeriod( 0 ),
	_inputPeriod( 0 ),
	_desiredSimulationFrequency( DefaultSimulationFrequency ),
	_desiredRenderingFrequency( DefaultRenderingFrequency ),
	_desiredInputFrequency( DefaultInputFrequency ),
	_idleCallback( 0 ),
	_idleCallbackData( 0 ),
	_idleCallbackMaxDuration( 0 ),
	_idleCallsCount( 0 ),
	_stopRequested( false ),
	_scheduleStartingSecond( 0 ),
	_scheduleStartingMicrosecond( 0 ),
	_missedSimulationTicks( 0 ),
	_missedRenderingTicks( 0 ),
	_missedInputTicks( 0 ),
	_eventsModule( 0 ),
	_renderer( 0 ),
	_videoModule( 0 )
{

	// Force precomputation for scheduling granularity, since it takes time :
	send( "On scheduler creation, "
		"detected operating system scheduling granularity is about "
		+ Ceylan::toString( getSchedulingGranularity() ) + " microseconds." ) ;

	// Update _simulationPeriod, _renderingPeriod and _screenshotPeriod :
	setTimeSliceDuration( DefaultEngineTickDuration ) ;
	
	send( "Scheduler created." ) ;
	
}


Scheduler::~Scheduler() throw()
{

	for ( list<PeriodicSlot *>::iterator it = _periodicSlots.begin(); 
			it != _periodicSlots.end(); it++ )
		delete (*it ) ;
	
	if ( _renderer != 0 )
		delete _renderer ;
		
	send( "Scheduler deleted." ) ;
	
}


void Scheduler::scheduleBestEffort() throw( SchedulingException )
{

	/*
	 * Set up idle callback (with default strategy) if not done already :
	 * (will use atomic sleeps)
	 *
	 */
	if ( _idleCallback == 0 && _idleCallbackMaxDuration == 0 )
		_idleCallbackMaxDuration = 
			static_cast<Microsecond>( 1.1 * getSchedulingGranularity() ) ;

	EngineTick idleCallbackDuration = static_cast<EngineTick>( 
		Ceylan::Maths::Ceil( 
			static_cast<Ceylan::Float32>( _idleCallbackMaxDuration ) /
			_engineTickDuration ) ) ;
		
	_idleCallsCount = 0 ;
	
	send( "Scheduler starting in soft real-time best effort mode. " 
		"Scheduler informations : " + toString( Ceylan::low ) ) ;
	
	// Let's prepare to the run :
	
	_stopRequested = false ;
		
	_missedSimulationTicks = 0 ;
	_missedRenderingTicks  = 0 ;
	_missedInputTicks      = 0 ;
	
#if OSDL_DEBUG_SCHEDULER
	
	/*
	 * If OSDL_DEBUG_SCHEDULER is set, run-time informations about the 
	 * scheduler will be archived and analyzed after it stopped, like black
	 * boxes.
	 *
	 */
	 
	list<SimulationTick> metSimulations ;
	list<SimulationTick> missedSimulations ;
	
	list<RenderingTick>  metRenderings ;	
	list<RenderingTick>  missedRenderings ;
		
	list<InputTick>      metInputPollings ;	
	list<InputTick>      missedInputPollings ;	
	
#endif // OSDL_DEBUG_SCHEDULER

	EngineTick previousEngineTick ;
	
	
	/*
	 * Initial time is first measured and converted to engine, simulation,
	 * rendering and input times :
	 * _currentEngineTick, for an engine tick duration of 1000 (microseconds),
	 * should wrap around if the program runs for more than 49 days. 
	 * If engine tick duration is divided by two, then the period until
	 * wrap-around will be divided by two as well.
	 *
	 */	
	 
	// Starts with all zero	ticks :	
	_currentEngineTick     = 0 ;	
	
	_currentSimulationTick = 0 ;	
	_currentRenderingTick  = 0 ;
	_currentInputTick      = 0 ;
	
	OSDL_SCHEDULE_LOG( "Simulation period : " << _simulationPeriod ) ;
	OSDL_SCHEDULE_LOG( "Rendering period : "  << _renderingPeriod ) ;
	OSDL_SCHEDULE_LOG( "Input period : "      << _inputPeriod      ) ;
		
	EngineTick nextSimulationDeadline = _currentEngineTick + _simulationPeriod ;
	EngineTick nextRenderingDeadline  = _currentEngineTick + _renderingPeriod ;
	EngineTick nextInputDeadline      = _currentInputTick  + _inputPeriod ;
	
	EngineTick nextDeadline ;
	
	// Store scheduling starting time : 
	getPreciseTime( _scheduleStartingSecond, _scheduleStartingMicrosecond ) ;
	
	
	// Enter the schedule loop :
	
	/*
	 * @todo : by construction the actual frequencies can ony be less than the
	 * expected ones, which are seldom enforced (i.e. we have never 100 Hz,
	 * but often 95 Hz because of OS process switching). 
	 *
	 * The algorithm could check the delay D, if it is small (ex : D < 8 ms),
	 * then do as if it had not existed (do not cancel anything, activate as
	 * usual). 
	 * Otherwise, if the actions are really triggered too late, use the 
	 * on*Skip methods.
	 *
	 * Overloaded situations could be detected thanks to a kind of leaking 
	 * bucket : each delay of d ms adds d to a sum s, each rendering tick
	 * decreases s, if s exceeds say, 50, then the system is deemed overloaded
	 * and more skips are triggered.
	 *
	 */
	 
	while ( ! _stopRequested )
	{
			
		/* 
		 * Normally, for each simulation tick, either scheduleSimulation 
		 * or, if late, onSimulationSkipped are called.
		 *
		 * Similarly, for each rendering tick, either scheduleRendering 
		 * or, if late, onRenderingSkipped are called.
		 *
		 * It is soft real-time because the engine tick is derived directly
		 * from the system clock, it cannot drift.
		 *
		 */
		 	
		OSDL_SCHEDULE_LOG( "[ " << _currentEngineTick 
			<< " ; " << _currentSimulationTick
			<< " ; " << _currentRenderingTick		
			<< " ; " << _currentInputTick << " ]" ) ;
					
		// We hereby suppose we are just at the beginning of a new engine tick.
		previousEngineTick = _currentEngineTick ;
		
		
		// What is to be done on this new engine tick ?
		
		
		// Perform all scheduling actions for this tick :
		if ( _currentEngineTick == nextSimulationDeadline )
		{
			//OSDL_SCHEDULE_LOG( "--> Simulation deadline met" ) ;
				
#if OSDL_DEBUG_SCHEDULER
			metSimulations.push_back( _currentSimulationTick ) ;
#endif // OSDL_DEBUG_SCHEDULER
			
			scheduleSimulation( _currentSimulationTick ) ;
			
			_currentSimulationTick++ ;
			nextSimulationDeadline += _simulationPeriod ;
		}
		// Time has elapsed....


		if ( _currentEngineTick == nextRenderingDeadline )
		{
			//OSDL_SCHEDULE_LOG( "--> Rendering deadline met" ) ;
				
#if OSDL_DEBUG_SCHEDULER
			metRenderings.push_back( _currentRenderingTick ) ;
#endif // OSDL_DEBUG_SCHEDULER

			scheduleRendering( _currentRenderingTick ) ;

			_currentRenderingTick++ ;
			nextRenderingDeadline += _renderingPeriod ;
		}
		// Still more time elapsed....


		if ( _currentEngineTick == nextInputDeadline )
		{
			//OSDL_SCHEDULE_LOG( "--> Input deadline met" ) ;
				
#if OSDL_DEBUG_SCHEDULER
			metInputPollings.push_back( _currentInputTick ) ;
#endif // OSDL_DEBUG_SCHEDULER

			scheduleInput( _currentInputTick ) ;

			_currentInputTick++ ;
			nextInputDeadline += _inputPeriod ;
		}
		// Everything done, hoping not too much time elapsed....

		
		/*
		 * Check we did not miss any deadline : if these scheduling actions
		 * (notably, the rendering) last more than the current engine 
		 * tick, it is not a problem.
		 *
		 * What must not occur is that these actions lasted so long that 
		 * the first next deadline (either simulation, rendering or input), 
		 * was missed.
		 *
		 * The next '+1' is here because the soonest possible deadlines 
		 * will be on next engine tick.
		 * Engine time is regularly updated since any call may last for 
		 * non negligible durations.
		 * 
		 * As multiple steps can be missed, a while structure is required.
		 *
		 */
		_currentEngineTick = computeEngineTickFromCurrentTime() ;
		
		while ( nextSimulationDeadline < _currentEngineTick + 1 )	
		{			
		
			// Cancel all missed simulation steps and warn :
			EngineTick missedTicks = 
				_currentEngineTick + 1 - nextSimulationDeadline ;
				
			OSDL_SCHEDULE_LOG( "##### Simulation deadline missed of "
				+ Ceylan::toString( missedTicks )
				+ " engine ticks (" 
				+ Ceylan::toString( missedTicks * _engineTickDuration )  
				+ " microseconds), cancelling activations." ) ;
			
			
#if OSDL_DEBUG_SCHEDULER
			missedSimulations.push_back( _currentSimulationTick ) ;
#endif // OSDL_DEBUG_SCHEDULER
			
			/*
			 * If ever this call is longer than the simulation period, the 	
			 * scheduler will go in an infinite loop :
			 *
			 */
			onSimulationSkipped( _currentSimulationTick ) ;
			
			_currentSimulationTick++ ;
			nextSimulationDeadline += _simulationPeriod ;
			
			_currentEngineTick = computeEngineTickFromCurrentTime() ;
						
		}
		
		
		/*
		 * As simulation ticks are skipped before rendering ticks, the 
		 * laters should be, on average, more frequently skipped than the
		 * formers, since skipping simulation ticks might take some time, 
		 * even as much time as if there had not been skipped, since the 
		 * objects can call their onActivation method for their onSkip method.
		 * 
		 * This is a way of setting a higher priority to simulation ticks,
		 * compared to rendering ticks.
		 *
		 */		 
		while ( nextRenderingDeadline < _currentEngineTick + 1 )	
		{			
		
			// Cancel all missed rendering steps and warn :
			EngineTick missedTicks = 
				_currentEngineTick + 1 - nextRenderingDeadline ;

			OSDL_SCHEDULE_LOG( "##### Rendering deadline missed of "
				+ Ceylan::toString( missedTicks )
				+ " engine ticks (" 
				+ Ceylan::toString( missedTicks * _engineTickDuration )  
				+ " microseconds), cancelling rendering." ) ;
			
#if OSDL_DEBUG_SCHEDULER
			missedRenderings.push_back( _currentRenderingTick ) ;
#endif // OSDL_DEBUG_SCHEDULER
			
			onRenderingSkipped( _currentRenderingTick ) ;

			_currentRenderingTick++ ;
			nextRenderingDeadline += _renderingPeriod ;
			
			_currentEngineTick = computeEngineTickFromCurrentTime() ;
						
		}
		
		
		/*
		 * Lastly, take care of the input polling.
		 *
		 */
		while ( nextInputDeadline < _currentEngineTick + 1 )	
		{			
		
			/*
			 * Cancel all missed input steps and warn (this is not too 
			 * serious skip) :
			 *
			 */
			EngineTick missedTicks = 
				_currentEngineTick + 1 - nextInputDeadline ;

			OSDL_SCHEDULE_LOG( "##### Input deadline missed of "
				+ Ceylan::toString( missedTicks )
				+ " engine ticks (" 
				+ Ceylan::toString( missedTicks * _engineTickDuration )  
				+ " microseconds), cancelling input polling." ) ;
			
#if OSDL_DEBUG_SCHEDULER
			missedInputPollings.push_back( _currentInputTick ) ;
#endif // OSDL_DEBUG_SCHEDULER
			
			onInputSkipped( _currentInputTick ) ;

			_currentInputTick++ ;
			nextInputDeadline += _inputPeriod ;
			
			_currentEngineTick = computeEngineTickFromCurrentTime() ;
						
		}
		 		

		/*
		 * Wait until next deadline, possibly skipping several engine ticks :
		 *
		 */
		nextDeadline = Ceylan::Maths::Min( nextSimulationDeadline,
			nextRenderingDeadline, nextInputDeadline ) ;
		
		OSDL_SCHEDULE_LOG( "Next deadline is " 
			+ Ceylan::toString( nextDeadline - _currentEngineTick )
			+ " engine tick(s) away." ) ;

		/*
		 * Do nothing : wait, first with idle callback (which results in
		 * atomic sleeps by default), then for fine-grain with soft busy
		 * waitings.
		 *
		 */
		
		// Atomic sleeps : 		 
		while ( _currentEngineTick + idleCallbackDuration < nextDeadline )
		{
			
			/*
			 * OSDL_SCHEDULE_LOG( 
			 * "Waiting for the end of engine tick " << _currentEngineTick ) ;
			 *
			 */
			onIdle() ;
			
			_currentEngineTick = computeEngineTickFromCurrentTime() ;
			
		}	
		
		
		// Then busy waiting :
		while ( _currentEngineTick < nextDeadline )
			_currentEngineTick = computeEngineTickFromCurrentTime() ;
		
		/*
		 * OSDL_SCHEDULE_LOG( 
		 * "End of schedule loop, engine tick being " << _currentEngineTick ) ;
		 *
		 */
		
	} // End of scheduler overall loop

	Second scheduleStoppingSecond ;
	Microsecond scheduleStoppingMicrosecond ;
	
	getPreciseTime( scheduleStoppingSecond, scheduleStoppingMicrosecond ) ;
	
	Microsecond totalRuntime = getDurationBetween( 
		_scheduleStartingSecond, _scheduleStartingMicrosecond,
		scheduleStoppingSecond,  scheduleStoppingMicrosecond ) ;
	
	string table = 
		"<table border=1>"
		"	<tr>"
		"		<th>Tick types</th>"
		" 		<th>Requested frequency</th>" 
		" 		<th>Agreed frequency</th>" 
		" 		<th>Measured frequency</th>" 
		" 		<th>Tick duration (microsec)</th>" 
		" 		<th>Tick duration (engine ticks)</th>" 
		" 		<th>Stopped at tick</th>" 
		" 		<th>Missed count</th>"
		" 		<th>Missed percentage</th>"
		"	</tr>" ;
	
	
	Ceylan::Uint8 precision = 2 /* digits after the dot */ ;
	
	// Engine :
	table += 
		"	<tr>"
		"		<td>Engine tick</td>"
		"		<td>" 
		+ Ceylan::toString( 1E6 / DefaultEngineTickDuration, precision ) 
		+ " Hz</td>"
		"		<td>" 
		+ Ceylan::toString( 1E6 / DefaultEngineTickDuration, precision ) 
		+ " Hz</td>"
		"		<td>" 
		+ Ceylan::toString( _currentEngineTick * 1E6 / totalRuntime, precision )
		+ " Hz</td>"
		"		<td>" + Ceylan::toString( _engineTickDuration ) + "</td>"
		"		<td>1</td>"
		"		<td>" + Ceylan::toString( _currentEngineTick ) + "</td>"
		"		<td>N/A</td>"		
		"		<td>N/A</td>"		
		"	</tr>" ;
		
		
	// Simulation :	
	table += 
		"	<tr>"
		"		<td>Simulation tick</td>"
		"		<td>" 
		+ Ceylan::toString( _desiredSimulationFrequency ) 
		+ " Hz</td>"
		"		<td>" 
		+ Ceylan::toString(	1E6 / ( _simulationPeriod * _engineTickDuration ), 
			precision ) + " Hz</td>"
		"		<td>"
		+ Ceylan::toString(	
			1E6 * ( _currentSimulationTick - _missedSimulationTicks ) / 
				totalRuntime, precision ) + " Hz</td>"
		"		<td>" + Ceylan::toString( 
			_simulationPeriod * _engineTickDuration ) + "</td>"
		"		<td>" + Ceylan::toString( _simulationPeriod ) + "</td>"
		"		<td>" + Ceylan::toString( _currentSimulationTick ) + "</td>"
		"		<td>" + Ceylan::toString( _missedSimulationTicks ) + "</td>"	
		"		<td>" + Ceylan::toString( 
			100.0f * _missedSimulationTicks / _currentSimulationTick, 
			precision ) 
		+ "%</td>"	
		"	</tr>" ;
		
		
	// Rendering :	
	table += 
		"	<tr>"
		"		<td>Rendering tick</td>"
		"		<td>" 
		+ Ceylan::toString( _desiredRenderingFrequency ) 
		+ " Hz</td>"
		"		<td>" 
		+ Ceylan::toString(	1E6 / ( _renderingPeriod * _engineTickDuration ), 
			precision ) + " Hz</td>"
		"		<td>"
		+ Ceylan::toString(	
			1E6 * ( _currentRenderingTick - _missedRenderingTicks ) / 
				totalRuntime, precision ) + " Hz</td>"
		"		<td>" + Ceylan::toString( 
			_renderingPeriod * _engineTickDuration ) + "</td>"
		"		<td>" + Ceylan::toString( _renderingPeriod ) + "</td>"
		"		<td>" + Ceylan::toString( _currentRenderingTick ) + "</td>"
		"		<td>" + Ceylan::toString( _missedRenderingTicks ) + "</td>"	
		"		<td>" + Ceylan::toString( 
			100.0f * _missedRenderingTicks / _currentRenderingTick, precision ) 
		+ "%</td>"	
		"	</tr>" ;
		
	// Input :	
	table += 
		"	<tr>"
		"		<td>Input polling tick</td>"
		"		<td>" 
		+ Ceylan::toString( _desiredInputFrequency ) 
		+ " Hz</td>"
		"		<td>" 
		+ Ceylan::toString(	1E6 / ( _inputPeriod * _engineTickDuration ), 
			precision ) + " Hz</td>"
		"		<td>"
		+ Ceylan::toString(	
			1E6 * ( _currentInputTick - _missedInputTicks ) / 
				totalRuntime, precision ) + " Hz</td>"
		"		<td>" + Ceylan::toString( 
			_inputPeriod * _engineTickDuration ) + "</td>"
		"		<td>" + Ceylan::toString( _inputPeriod ) + "</td>"
		"		<td>" + Ceylan::toString( _currentInputTick ) + "</td>"
		"		<td>" + Ceylan::toString( _missedInputTicks ) + "</td>"	
		"		<td>" + Ceylan::toString( 
			100.0f * _missedInputTicks / _currentInputTick, precision ) 
		+ "%</td>"	
		"	</tr>" ;
		
	table += "</table>" ;	
	
	send( table ) ;
	
	list<string> summary ;
	
	if ( scheduleStoppingSecond - _scheduleStartingSecond >
			MaximumDurationWithMicrosecondAccuracy )			
		summary.push_back( "The scheduler ran for " 
			+ Ceylan::toString( 
				scheduleStoppingSecond - _scheduleStartingSecond )
			+ " seconds." ) ;
	else
		summary.push_back( "The scheduler ran for " 
			+ Ceylan::toString( 
				scheduleStoppingSecond - _scheduleStartingSecond )
			+ " seconds (precisely : " + Ceylan::toString( totalRuntime )
			+ " microseconds)." ) ;
	
	summary.push_back( Ceylan::toString( _idleCallsCount ) 
		+ " idle calls have been made." ) ;

	summary.push_back( "Each idle call was expected to last for "
		+ Ceylan::toString( _idleCallbackMaxDuration ) + " microseconds, i.e. "
		+ Ceylan::toString( idleCallbackDuration ) + " engine ticks." ) ;
	
	send( "Scheduler stopping, run summary is : " 
		+ Ceylan::formatStringList( summary ) ) ;
	
	send( "Full scheduler infos : " + toString( Ceylan::high ) ) ;	

	_scheduleStartingSecond = 0 ;
	_scheduleStartingMicrosecond = 0 ;
	
			
#if OSDL_DEBUG_SCHEDULER
	
	/*
	 * Collect some informations, and check them to assess the scheduler
	 * behaviour.
	 *
	 */

	bool beVerbose = true ;
	bool beVeryVerbose = true ;
	
	
	
	// For simulation ticks :			
	
	Ceylan::Uint32 totalSimulationTicks = 
		metSimulations.size() + missedSimulations.size() ;
		
	
	LogPlug::debug( "Total simulation ticks : " 
		+ Ceylan::toString( totalSimulationTicks ) + "." ) ;
		
	LogPlug::debug( "Met simulation ticks : " 
		+ Ceylan::toString( metSimulations.size() ) 
		+ " (" + Ceylan::toString( 100.0f * metSimulations.size() 
			/ totalSimulationTicks, /* precision */ 2 ) + "%)." ) ;
				
	LogPlug::debug( "Missed simulation ticks : " 
		+ Ceylan::toString( missedSimulations.size() ) 
		+ " (" + Ceylan::toString( 100.0f * missedSimulations.size() 
			/ totalSimulationTicks, /* precision */ 2 ) + "%)." ) ;
	
	string res ;
	

	if ( beVerbose )
	{
	
		if ( beVeryVerbose )
		{
		
			for ( list<SimulationTick>::const_iterator it 
					= metSimulations.begin(); it != metSimulations.end(); it++ )
				res += 	Ceylan::toString( *it ) + " - " ;
		 
			LogPlug::debug( "Met simulation ticks : " + res ) ;

			res.clear() ;
	
		}
		
		for ( list<SimulationTick>::const_iterator it 
				= missedSimulations.begin(); it != missedSimulations.end(); 
				it++ )
			res += 	Ceylan::toString( *it ) + " - " ;

		LogPlug::debug( "Missed simulation ticks : " + res ) ;

	}
	
		
	/*
	 * Check that all simulation ticks were handled one and only one time, 
	 * one way or another (scheduled or skipped).
	 *
	 */
	SimulationTick newSimulationTick ;
	
	bool * simulationTicks = new bool[ _currentSimulationTick ] ;
	
	for ( Events::SimulationTick i = 0; i < _currentSimulationTick; i++ )
		simulationTicks[i] = false ;
		
	for ( list<SimulationTick>::const_iterator it = metSimulations.begin(); 
		it != metSimulations.end(); it++ )
	{
		newSimulationTick = (*it) ;
		
		if ( simulationTicks[ newSimulationTick ] == false )
			simulationTicks[ newSimulationTick ] = true ;
		else
			LogPlug::error( "Simulation tick #" 
				+ Ceylan::toString( newSimulationTick ) 
				+ " should not have been scheduled more than once." ) ;			
	}
	
	
	for ( list<SimulationTick>::const_iterator it = missedSimulations.begin(); 
		it != missedSimulations.end(); it++ )
	{
	
		newSimulationTick = (*it) ;
		
		if ( simulationTicks[ newSimulationTick ] == false )
			simulationTicks[ newSimulationTick ] = true ;
		else
			LogPlug::error( "Simulation tick #" 
				+ Ceylan::toString( newSimulationTick ) 
				+ " should not have been skipped, "
				"since had already been taken into account." ) ;
							
	}
	
	for ( Events::SimulationTick i = 0; i < _currentSimulationTick; i++ )
		if ( simulationTicks[ i ] == false )
			LogPlug::error( "Simulation tick #" + Ceylan::toString( i ) 
				+ " has never been scheduled." ) ;	
				
	delete simulationTicks ;
	

	// For rendering ticks :			

	Ceylan::Uint32 totalRenderingTicks = 
		metRenderings.size() + missedRenderings.size() ;

	LogPlug::debug( "Total rendering ticks : " 
		+ Ceylan::toString( totalRenderingTicks ) + "." ) ;
		
	LogPlug::debug( "Met rendering ticks : " 
		+ Ceylan::toString( metRenderings.size() ) 
		+ " (" + Ceylan::toString( 100.0f * metRenderings.size() 
			/ totalRenderingTicks, /* precision */ 2 ) + "%)." ) ;
				
	LogPlug::debug( "Missed rendering ticks : " 
		+ Ceylan::toString( missedRenderings.size() ) 
		+ " (" + Ceylan::toString( 100.0f * missedRenderings.size() 
			/ totalRenderingTicks, /* precision */ 2 ) + "%)." ) ;
				
	res.clear() ;
		
				
	if ( beVerbose )
	{
	
		if ( beVeryVerbose )
		{
		
			for ( list<RenderingTick>::const_iterator it 
					= metRenderings.begin(); it != metRenderings.end(); it++ )
				res += 	Ceylan::toString( *it ) + " - " ;
		 
			LogPlug::debug( "Met rendering ticks : " + res ) ;

			res.clear() ;
	
		}
		
		for ( list<RenderingTick>::const_iterator it 
				= missedRenderings.begin(); it != missedRenderings.end(); it++ )
			res += 	Ceylan::toString( *it ) + " - " ;

		LogPlug::debug( "Missed rendering ticks : " + res ) ;

	}
	
				
	/*
	 * Check that all rendering ticks were handled one and only one time, 
	 * one way or another :
	 * (scheduled or skipped).
	 *
	 */
	
	RenderingTick newRenderingTick ;
	
	bool * renderingTicks = new bool[ _currentRenderingTick ] ;
	
	for ( Events::RenderingTick i = 0; i < _currentRenderingTick; i++ )
		renderingTicks[i] = false ;
		
	for ( list<RenderingTick>::const_iterator it = metRenderings.begin(); 
		it != metRenderings.end(); it++ )
	{
	
		newRenderingTick = (*it) ;
		if ( renderingTicks[ newRenderingTick ] == false )
			renderingTicks[ newRenderingTick ] = true ;
		else
			LogPlug::error( "Rendering tick #" 
				+ Ceylan::toString( newRenderingTick ) 
				+ " should not have been scheduled more than one time." ) ;		
				
	}
	
	
	for ( list<RenderingTick>::const_iterator it = missedRenderings.begin(); 
		it != missedRenderings.end(); it++ )
	{
		newRenderingTick = (*it) ;
		if ( renderingTicks[ newRenderingTick ] == false )
			renderingTicks[ newRenderingTick ] = true ;
		else
			LogPlug::error( "Rendering tick #" 
				+ Ceylan::toString( newRenderingTick ) 
				+ " should not have been been skipped, "
				"since had already been taken into account." ) ;			
	}
	
	for ( Events::RenderingTick i = 0; i < _currentRenderingTick; i++ )
		if ( renderingTicks[ i ] == false )
			LogPlug::error( "Rendering tick #" + Ceylan::toString( i ) 
				+ " has never been scheduled." ) ;	

	delete renderingTicks ;



	// For input ticks :			


	Ceylan::Uint32 totalInputTicks = 
		metRenderings.size() + missedRenderings.size() ;

	LogPlug::debug( "Total input ticks : " 
		+ Ceylan::toString( totalInputTicks ) + "." ) ;
		
	LogPlug::debug( "Met input ticks : " 
		+ Ceylan::toString( metInputPollings.size() ) 
		+ " (" + Ceylan::toString( 100.0f * metInputPollings.size() 
			/ totalInputTicks, /* precision */ 2 ) + "%)." ) ;
				
	LogPlug::debug( "Missed input ticks : " 
		+ Ceylan::toString( missedInputPollings.size() ) 
		+ " (" + Ceylan::toString( 100.0f * missedInputPollings.size() 
			/ totalInputTicks, /* precision */ 2 ) + "%)." ) ;
				
	res.clear() ;
		
				
	if ( beVerbose )
	{
	
		if ( beVeryVerbose )
		{
		
			for ( list<InputTick>::const_iterator it 
					= metInputPollings.begin(); 
					it != metInputPollings.end(); it++ )
				res += 	Ceylan::toString( *it ) + " - " ;
		 
			LogPlug::debug( "Met input ticks : " + res ) ;

			res.clear() ;
	
		}
		
		for ( list<InputTick>::const_iterator it 
				= missedInputPollings.begin(); 
				it != missedInputPollings.end(); it++ )
			res += 	Ceylan::toString( *it ) + " - " ;

		LogPlug::debug( "Missed input ticks : " + res ) ;

	}
				
				
	/*
	 * Check that all input ticks were handled one and only one time, one 
	 * way or another :
	 * (scheduled or skipped).
	 *
	 */
	
	InputTick newInputTick ;
	
	bool * inputTicks = new bool[ _currentInputTick ] ;
	
	for ( Events::InputTick i = 0; i < _currentInputTick; i++ )
		inputTicks[i] = false ;
		
	for ( list<InputTick>::const_iterator it = metInputPollings.begin(); 
		it != metInputPollings.end(); it++ )
	{
	
		newInputTick = (*it) ;
		if ( inputTicks[ newInputTick ] == false )
			inputTicks[ newInputTick ] = true ;
		else
			LogPlug::error( "Input tick #" + Ceylan::toString( newInputTick ) 
				+ " should not have been scheduled more than once." ) ;
				
	}
	
	
	for ( list<InputTick>::const_iterator it = missedInputPollings.begin(); 
		it != missedInputPollings.end(); it++ )
	{
		newInputTick = (*it) ;
		if ( inputTicks[ newInputTick ] == false )
			inputTicks[ newInputTick ] = true ;
		else
			LogPlug::error( "Input tick #" + Ceylan::toString( newInputTick ) 
				+ " should not have been been skipped, "
				"since had already been taken into account." ) ;			
	}
	
	for ( Events::InputTick i = 0; i < _currentInputTick; i++ )
		if ( inputTicks[ i ] == false )
			LogPlug::error( "Input tick #" + Ceylan::toString( i ) 
				+ " has never been scheduled." ) ;	

	delete inputTicks ;
	
#endif // OSDL_DEBUG_SCHEDULER
	
}

	

void Scheduler::scheduleNoDeadline( bool pollInputs ) 
	throw( SchedulingException )
{

	send( "Scheduler starting, "
		"in no deadline mode (a.k.a screenshot mode). Scheduler infos : " 
		+ toString( Ceylan::high ) ) ;
	
	// Let's prepare to the run, for which no time is to be considered :
	
	_stopRequested = false ;	
	
#if OSDL_DEBUG_SCHEDULER
	
	/*
	 * If OSDL_DEBUG_SCHEDULER is set, run-time informations about 
	 * the scheduler will be archived and analyzed after it stopped, 
	 * like black boxes.
	 *
	 */
	 
	list<SimulationTick> metSimulations ;
	list<RenderingTick>  metRenderings ;	
	list<InputTick>      metInputPollings ;	
	
#endif // OSDL_DEBUG_SCHEDULER


	EngineTick previousEngineTick ;
	
	// Store scheduling starting time : 
	getPreciseTime( _scheduleStartingSecond, _scheduleStartingMicrosecond ) ;
		 
	// Starts with all zero	ticks :	
	_currentEngineTick     = 0 ;	
	_currentSimulationTick = 0 ;	
	_currentRenderingTick  = 0 ;
	_currentInputTick      = 0 ;
	
	OSDL_SCHEDULE_LOG( "Simulation period : " << _simulationPeriod ) ;
	OSDL_SCHEDULE_LOG( "Rendering period : "  << _renderingPeriod ) ;
	OSDL_SCHEDULE_LOG( "Input period : "      << _inputPeriod ) ;
		
	// No real deadline in this mode, just checkpoints that cannot be missed :	
	EngineTick nextSimulationDeadline = _currentEngineTick + _simulationPeriod ;
	EngineTick nextRenderingDeadline  = _currentEngineTick + _renderingPeriod ;
	EngineTick nextInputDeadline      = _currentEngineTick + _inputPeriod ;
	
	
	// Enter the schedule loop :
	
	while ( ! _stopRequested )
	{
			
		/* 
		 * Normally, for each simulation tick, one scheduleSimulation is called.
		 *
		 * Similarly, for each rendering tick, one scheduleRendering is called.
		 *
		 * Same thing for inputs.
		 *
		 */
		 	
		OSDL_SCHEDULE_LOG( "[ " << _currentEngineTick << " ; " 
			<< _currentSimulationTick
			<< " ; " << _currentRenderingTick << " ; " 
			<< _currentInputTick << " ]" ) ;
					
		// We hereby suppose we are just at the beginning of a new engine tick.
		previousEngineTick = _currentEngineTick ;
		
		// What is to be done on this new engine tick ?
		
		// Perform all scheduling actions for this tick :
		if ( _currentEngineTick == nextSimulationDeadline )
		{
		
			//OSDL_SCHEDULE_LOG( "--> Simulation deadline met" ) ;
				
#if OSDL_DEBUG_SCHEDULER
			metSimulations.push_back( _currentSimulationTick ) ;
#endif // OSDL_DEBUG_SCHEDULER
			
			scheduleSimulation( _currentSimulationTick ) ;
			
			_currentSimulationTick++ ;
			nextSimulationDeadline += _simulationPeriod ;
		}

		if ( _currentEngineTick == nextRenderingDeadline )
		{
		
			//OSDL_SCHEDULE_LOG( "--> Rendering deadline met" ) ;
				
#if OSDL_DEBUG_SCHEDULER
			metRenderings.push_back( _currentRenderingTick ) ;
#endif // OSDL_DEBUG_SCHEDULER

			scheduleRendering( _currentRenderingTick ) ;

			_currentRenderingTick++ ;
			nextRenderingDeadline += _renderingPeriod ;
		}
		
		if ( pollInputs && _currentEngineTick == nextInputDeadline )
		{
		
			//OSDL_SCHEDULE_LOG( "--> Input deadline met" ) ;
				
#if OSDL_DEBUG_SCHEDULER
			metInputPollings.push_back( _currentInputTick ) ;
#endif // OSDL_DEBUG_SCHEDULER

			scheduleInput( _currentInputTick ) ;

			_currentInputTick++ ;
			nextInputDeadline += _inputPeriod ;
		}

		_currentEngineTick++ ;
		
		/*
		 * OSDL_SCHEDULE_LOG( "End of schedule loop, engine tick being " 
		 * << _currentEngineTick ) ;
		 *
		 */
		
	}
	
	Second scheduleStoppingSecond ;
	Microsecond scheduleStoppingMicrosecond ;
	getPreciseTime( scheduleStoppingSecond, scheduleStoppingMicrosecond ) ;
	
	send( "Scheduler stopping. Scheduler infos : " 
		+ toString( Ceylan::high ) ) ;	
	
	// Beware to the overflows !
	Microsecond totalScheduleDuration = 
		( scheduleStoppingSecond - _scheduleStartingSecond ) * 1000000
		+ scheduleStoppingMicrosecond - _scheduleStartingMicrosecond ;

	ostringstream buf ;
	
	buf.precision( 4 ) ;
		
	buf	<< "Actual average engine frequency was "  
		<< 1E6 * _currentEngineTick / totalScheduleDuration
		<< " Hz, average simulation frequency was " 
		<< 1E6 * _currentSimulationTick / totalScheduleDuration
		<< " Hz, average rendering frequency was " 
		<< 1E6 * _currentRenderingTick / totalScheduleDuration
		<< " Hz, average input frequency was " 
		<< 1E6 * _currentInputTick / totalScheduleDuration 
		<< " Hz." ;
		 
	send( buf.str() ) ;
			
#if OSDL_DEBUG_SCHEDULER
	
	/*
	 * Collect some informations, and check them to assess the scheduler
	 * behaviour.
	 *
	 */
	
	LogPlug::debug( "Total simulation ticks : " 
		+ Ceylan::toString( metSimulations.size() ) + "." ) ;
			
	string res ;
	
	/*
	for ( list<SimulationTick>::const_iterator it = metSimulations.begin();
			it != metSimulations.end(); it++ )
		res += 	Ceylan::toString( *it ) + " - " ;
		 
	LogPlug::debug( "Met simulation ticks : " + res ) ;
	*/


	/*
	 * Check that all simulation ticks were scheduled one and only one time.
	 *
	 */
	SimulationTick newSimulationTick ;
	
	bool * simulationTicks = new bool[ _currentSimulationTick ] ;
	
	for ( Events::SimulationTick i = 0; i < _currentSimulationTick; i++ )
		simulationTicks[i] = false ;
		
	for ( list<SimulationTick>::const_iterator it = metSimulations.begin(); 
		it != metSimulations.end(); it++ )
	{
	
		newSimulationTick = (*it) ;
		if ( simulationTicks[ newSimulationTick ] == false )
			simulationTicks[ newSimulationTick ] = true ;
		else
			LogPlug::error( "Simulation tick #" 
				+ Ceylan::toString( newSimulationTick ) 
				+ " should not have been scheduled more than one time." ) ;		
				
	}
	
	
	for ( Events::SimulationTick i = 0; i < _currentSimulationTick; i++ )
		if ( simulationTicks[ i ] == false )
			LogPlug::error( "Simulation tick #" + Ceylan::toString( i ) 
				+ " has never been scheduled." ) ;	

	delete simulationTicks ;
				
	LogPlug::debug( "Total rendering ticks : " 
		+ Ceylan::toString( metRenderings.size() ) + "." ) ;
	
	/*	
	res = "" ;
				
	for ( list<RenderingTick>::const_iterator it = metRenderings.begin();
			it != metRenderings.end(); it++ )
		res += 	Ceylan::toString( *it ) + " - " ;
		 
	LogPlug::debug( "Met rendering ticks : " + res ) ;
	*/
				
				
	/*
	 * Check that all rendering ticks were scheduled one and only one time.
	 *
	 */
	
	RenderingTick newRenderingTick ;
	
	bool * renderingTicks = new bool[ _currentRenderingTick ] ;
	
	for ( Events::RenderingTick i = 0; i < _currentRenderingTick; i++ )
		renderingTicks[i] = false ;
		
	for ( list<RenderingTick>::const_iterator it = metRenderings.begin(); 
		it != metRenderings.end(); it++ )
	{
		newRenderingTick = (*it) ;
		if ( renderingTicks[ newRenderingTick ] == false )
			renderingTicks[ newRenderingTick ] = true ;
		else
			LogPlug::error( "Rendering tick #" 
				+ Ceylan::toString( newRenderingTick ) 
				+ " should not have been scheduled more than one time." ) ;		
				
	}
	
	
	
	for ( Events::RenderingTick i = 0; i < _currentRenderingTick; i++ )
		if ( renderingTicks[ i ] == false )
			LogPlug::error( "Rendering tick #" + Ceylan::toString( i ) 
				+ " has never been scheduled." ) ;	

	delete renderingTicks ;
	

	/*
	 * Check that all input ticks were scheduled one and only one time.
	 *
	 */
	if ( pollInputs )
	{
	
		InputTick newInputTick ;
	
		bool * inputTicks = new bool[ _currentInputTick ] ;
		
		for ( Events::InputTick i = 0; i < _currentInputTick; i++ )
			inputTicks[i] = false ;
		
		for ( list<InputTick>::const_iterator it = metInputPollings.begin(); 
			it != metInputPollings.end(); it++ )
		{
			newInputTick = (*it) ;
			if ( inputTicks[ newInputTick ] == false )
				inputTicks[ newInputTick ] = true ;
			else
				LogPlug::error( "Input tick #" 
					+ Ceylan::toString( newInputTick ) 
					+ " should not have been scheduled more than one time." ) ;	
					
		}
		
		for ( Events::InputTick i = 0; i < _currentInputTick; i++ )
			if ( inputTicks[ i ] == false )
				LogPlug::error( "Input tick #" + Ceylan::toString( i ) 
					+ " has never been scheduled." ) ;	
		
		delete inputTicks ;
				
	}
						
#endif // OSDL_DEBUG_SCHEDULER

	_scheduleStartingSecond = 0 ;
	_scheduleStartingMicrosecond = 0 ;
	
}



EngineTick Scheduler::computeEngineTickFromCurrentTime() throw()
{

	Second currentSecond ;
	Microsecond currentMicrosecond ;
	getPreciseTime( currentSecond, currentMicrosecond ) ;
	
	return static_cast<EngineTick>( 
			( currentSecond - _scheduleStartingSecond ) * 1000000 
			+ currentMicrosecond - _scheduleStartingMicrosecond ) 
		/ _engineTickDuration ;
		
}


void Scheduler::scheduleSimulation( SimulationTick current ) throw()
{

	//OSDL_SCHEDULE_LOG( "--> current simulation tick : " << current ) ;
	
	OSDL_SCHEDULE_LOG( "--- simulating ! " ) ;
		
	// Activate all objects programmed for this specific time :
	scheduleProgrammedObjects( current ) ;
		
	// Activate all objects registered in this periodic slot :
	schedulePeriodicObjects( current ) ;

}


void Scheduler::scheduleProgrammedObjects( 
	SimulationTick currentSimulationTick ) throw()
{

	map<SimulationTick, ListOfActiveObjects>::iterator it 
		= _programmedActivated.find( currentSimulationTick ) ;
		
	if ( it != _programmedActivated.end() )
	{
		scheduleActiveObjectList( currentSimulationTick, (*it).second ) ;
		
		// Empty this simulation tick when done with it :
		_programmedActivated.erase( it ) ;
	}
	
	// else : no key for this simulation tick ? Nothing to do !	 
	
}


void Scheduler::schedulePeriodicObjects( SimulationTick current ) throw()
{

	// Request each periodic slot to activate relevant objects :
	
	for ( list<PeriodicSlot*>::iterator it = _periodicSlots.begin(); 
		it != _periodicSlots.end();	it++ )
	{
		(*it)->onNextTick( current ) ;
	}
		
}


void Scheduler::scheduleRendering( RenderingTick current ) throw()
{

	OSDL_SCHEDULE_LOG( "--- rendering !" ) ;

	if ( _renderer != 0 )
		_renderer->render( current ) ;
	else
	{

#if OSDL_DEBUG
		if ( _videoModule == 0 )
			Ceylan::emergencyShutdown( "Scheduler::scheduleRendering : "
				"no video module available." ) ;		
#endif // OSDL_DEBUG
		_videoModule->redraw() ;	
	}	
		
}


void Scheduler::scheduleInput( InputTick current ) throw()
{

	OSDL_SCHEDULE_LOG( "--- input polling !" ) ;

#if OSDL_DEBUG
	if ( _eventsModule == 0 )
		Ceylan::emergencyShutdown( "Scheduler::scheduleRendering : "
			"no events module available." ) ;		
#endif // OSDL_DEBUG
	
	_eventsModule->updateInputState() ;	
				
}


void Scheduler::scheduleActiveObjectList( 
	Events::RenderingTick currentSimulationTick, 
	ListOfActiveObjects & objectList ) throw()
{

	for ( ListOfActiveObjects::iterator it = objectList.begin(); 
		it != objectList.end(); it++ )
	{
		(*it)->onActivation( currentSimulationTick ) ;
	}
		
}

	 
void Scheduler::onSimulationSkipped( SimulationTick skipped ) 
	throw( SchedulingException )
{

	_missedSimulationTicks++ ;
	
		
	/*
	 * First, check objects which should have been triggered because 
	 * of periodic activation :
	 *
	 */
	
	/*
	 * Request each periodic slot to propagate the news to its relevant 
	 * objects :
	 *
	 */
	
	for ( list<PeriodicSlot*>::iterator it = _periodicSlots.begin(); 
		it != _periodicSlots.end();	it++ )
	{
		(*it)->onSimulationSkipped( skipped ) ;
	}
	
	// Second, send notification to programmed objects :

	map<SimulationTick, ListOfActiveObjects>::iterator it 
		= _programmedActivated.find( skipped ) ;
		
	if ( it != _programmedActivated.end() )
	{
		for ( ListOfActiveObjects::iterator itObjects = (*it).second.begin() ;
				itObjects != (*it).second.end(); itObjects++ )
			(*itObjects)->onSkip( skipped ) ;
	}
	
#if OSDL_DEBUG_SCHEDULER	
	LogPlug::warning( "Simulation tick " + Ceylan::toString( skipped ) 
		+ " had to be skipped." ) ;
#endif // OSDL_DEBUG_SCHEDULER
	
}


void Scheduler::onRenderingSkipped( RenderingTick skipped ) 
	throw( SchedulingException ) 
{

	_missedRenderingTicks++ ;
	
	if ( _renderer != 0 )
		_renderer->onRenderingSkipped( skipped ) ;
		
#if OSDL_DEBUG_SCHEDULER
	LogPlug::warning( "Rendering tick " + Ceylan::toString( skipped ) 
		+ " had to be skipped." ) ;
#endif // OSDL_DEBUG_SCHEDULER
		
}


void Scheduler::onInputSkipped( InputTick skipped ) 
	throw( SchedulingException ) 
{

	_missedInputTicks++ ;
		
	// Do nothing else, skipped inputs do not matter that much.
	
}


void Scheduler::onIdle() throw()
{

	OSDL_SCHEDULE_LOG( "--- idle callback called !" ) ;

	_idleCallsCount++ ;

	if ( _idleCallback != 0 )
	{
	
		// Call the user-supplied idle callback :
		(*_idleCallback)( _idleCallbackData ) ;
		
	}
	else
	{
	
		/*
		 * Issues an atomic sleep, chosen so that the minimum real predictable 
		 * sleeping time can be performed, scheduler-wise :
		 *
		 */
		 Ceylan::System::atomicSleep() ;
		 
	}	
	
}


void Scheduler::programTriggerFor( ActiveObject & objectToProgram, 
	SimulationTick targetTick ) throw()
{

	// Add the offset of current simulation tick if triggers are not absolute :
	if ( ! objectToProgram.areProgrammedActivationsAbsolute() )
		targetTick += _currentSimulationTick ;
		
	map<SimulationTick, ListOfActiveObjects>::iterator it 
		= _programmedActivated.find( targetTick ) ;
		
	if ( it != _programmedActivated.end() )
	{
		// There is already an object list available for this tick :
		(*it).second.push_back( & objectToProgram ) ;
	}
	else
	{
		// First object for this simulation tick :
		
		ListOfActiveObjects newList ;
		newList.push_back( & objectToProgram ) ; 
		_programmedActivated[ targetTick ] = newList ;
				
	}
	
}
					
					
PeriodicSlot & Scheduler::getPeriodicSlotFor( Events::Period period ) throw()
{

	for ( list<PeriodicSlot*>::iterator it = _periodicSlots.begin(); 
		it != _periodicSlots.end();	it++ )
	{
	
		if ( (*it)->getPeriod() == period )
			return * (*it) ;
		
		if ( (*it)->getPeriod() > period )
		{
		
			/*
			 * No slot already available, create it and insert it 
			 * <b>before</b> current one.
			 *
			 */
			PeriodicSlot * newSlot = new PeriodicSlot( period ) ;
			_periodicSlots.insert( it, newSlot ) ;
			return * newSlot ; 
		}	

	}	
	
	/*
	 * Period not found, higher than all others : create it at the end 
	 * of the list :
	 *
	 */
	PeriodicSlot * newSlot = new PeriodicSlot( period ) ;
	_periodicSlots.push_back( newSlot ) ;
	
	return * newSlot ; 
	
}



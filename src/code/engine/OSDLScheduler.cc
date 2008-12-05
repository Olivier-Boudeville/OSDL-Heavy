#include "OSDLScheduler.h"

#include "OSDLBasic.h"                // for getExistingCommonModule
#include "OSDLVideo.h"                // for redraw
#include "OSDLRenderer.h"             // for Renderer
#include "OSDLActiveObject.h"         // for ActiveObject


#include <iostream>                   // for ostringstream
using std::ostringstream ;

#include <iomanip> 				      // for std::ios::fixed



using namespace Ceylan::Log ;         // for LogPlug

// for time units and calls (ex: Millisecond, atomicSleep):
using namespace Ceylan::System ;      


using namespace OSDL ;
using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;

using std::string ;
using std::list ;
using std::map ;

using Ceylan::Maths::Hertz ;


Scheduler * Scheduler::_internalScheduler = 0 ;

const Scheduler::Delay Scheduler::ShutdownBucketLevel = 100000 ;


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
			"Scheduler::getRenderer: no renderer available." ) ;
		
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
	
	// Recomputes all the ticks that depends on the engine tick:
	setSimulationFrequency(   _desiredSimulationFrequency ) ;
	setRenderingFrequency(    _desiredRenderingFrequency ) ;
	setScreenshotFrequency(   _desiredScreenshotFrequency ) ;
	setInputPollingFrequency( _desiredInputFrequency ) ;
	
	_secondToEngineTick = /* microsecond */ 1000000 / _engineTickDuration ;
	
	OSDL_SCHEDULE_LOG( 
		"Multiplicative factor to convert seconds into engine ticks: "
		+ Ceylan::toString( _secondToEngineTick ) ) ;
		
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
	 	throw SchedulingException( "Scheduler::setSimulationFrequency: "
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

	LogPlug::debug( "Scheduler::setSimulationFrequency: for a requested "
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
	 	throw SchedulingException( "Scheduler::setRenderingFrequency: "
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

	LogPlug::debug( "Scheduler::setRenderingFrequency: for a requested "
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
	 	throw SchedulingException( "Scheduler::setScreenshotFrequency: "
			" requested screenshot frequency (" 
			+ Ceylan::toString( frequency )
			+ " Hz) is too high for engine tick,"
			" which lasts for " + Ceylan::toString( _engineTickDuration ) 
			+ " microseconds." ) ;
			
	_desiredScreenshotFrequency = frequency ;		
	_screenshotPeriod = static_cast<Period>( 
		1E6 / ( frequency * _engineTickDuration ) ) ;


#if OSDL_DEBUG

	LogPlug::debug( "Scheduler::setScreenshotFrequency: for a requested "
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
			"Scheduler::setInputPollingFrequency: requested input "
			"frequency (" + Ceylan::toString( frequency ) 
			+ " Hz) is too high for engine tick,"
			" which lasts for " 
			+ Ceylan::toString( _engineTickDuration ) + " microseconds." ) ;
			
	_desiredInputFrequency = frequency ;		
	_inputPeriod = static_cast<Period>( 
		1E6 / ( frequency * _engineTickDuration ) ) ;

#if OSDL_DEBUG

	LogPlug::debug( "Scheduler::setInputPollingFrequency: for a requested "
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
		LogPlug::error( "Scheduler::registerObject: "
			"following object will never be activated: "
			+ objectToRegister.toString() ) ;
#endif // OSDL_DEBUG
	
	
	if ( objectToRegister.getPeriod() != 0 )
	{
		// This active object is to be periodically activated:
		PeriodicSlot & slot = getPeriodicSlotFor( 
			objectToRegister.getPeriod() ) ;
			
		slot.add( objectToRegister ) ;	

	}
	
	
	if ( objectToRegister.hasProgrammedActivations() )
	{
	
		// Declare the programmed ticks for this object:
		
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
		throw SchedulingException( "Scheduler::schedule: "
			"no events module available." ) ;
	}
	
	
	if ( _renderer != 0 )
	{
		send( "Scheduling now, using renderer: " + _renderer->toString() ) ;
	}
	else
	{
		// Retrieve video module, used instead of the renderer:
		try
		{
			_videoModule = & OSDL::getExistingCommonModule().getVideoModule() ;
		}
		catch( OSDL::Exception & e )
		{
			throw SchedulingException( "Scheduler::schedule: "
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
		<< "Scheduler, whose internal frequency is " 
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
		+ ( _screenshotMode ? "on.": "off." ) ;
	
					
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
	{
	
		if ( _idleCallback )
			buf << ", callback data has been set as well." ;
		else
			buf << ", callback data has been set, "
				" whereas it is not used by default idle callback (abnormal)." ;
				
	}
	
	
	buf << ". The estimated upper-bound for idle callback duration is "
		+ Ceylan::toString( _idleCallbackMaxDuration )
		+ " microseconds" ;
				
				
	if ( level == Ceylan::low )
		return buf.str() ;
			
	// Do not use desired frequencies, compute them instead from the periods:
			
	buf <<  ". User-defined simulation frequency is " 
		+ Ceylan::toString( 1E6 / ( _simulationPeriod * _engineTickDuration ), 
			/* precision */ 2 )
		+ " Hz (a period of " 
		+ Ceylan::toString( _simulationPeriod ) 
		+ " engine ticks), rendering frequency is "
		+ Ceylan::toString( 1E6 / ( _renderingPeriod * _engineTickDuration ),
			/* precision */ 2 ) 
		+ " Hz (a period of " 
		+ Ceylan::toString( _renderingPeriod ) 
		+ " engine ticks), input polling frequency is "
		+ Ceylan::toString( 1E6 / ( _inputPeriod * _engineTickDuration ),
			/* precision */ 2 ) 
		+ " Hz (a period of " 
		+ Ceylan::toString( _inputPeriod ) 
		+ " engine ticks). There are "
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( _periodicSlots.size() ) ) 
		+ " used periodic slot(s) and "
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( _programmedActivated.size() ) )
		+ " programmed object(s)" ;
		
		
	if ( level == Ceylan::medium )
		return buf.str() ;
		
		
	if ( ! _periodicSlots.empty() )
	{
		
		buf << ". Enumerating scheduling periodic slots: " ;
		
		std::list<string> slots ;
		
		for ( list<PeriodicSlot*>::const_iterator it = _periodicSlots.begin(); 
				it != _periodicSlots.end(); it++ )
			slots.push_back( (*it)->toString( level ) ) ;

		buf << Ceylan::formatStringList( slots ) ;
			
	}
	
	if ( ! _programmedActivated.empty() )
	{
	
		buf << ". Enumerating scheduling programmed slots: " ;
		
		std::list<string> programmed ;
		
		for( map<SimulationTick, ListOfActiveObjects>::const_iterator it 
				= _programmedActivated.begin(); it 
					!= _programmedActivated.end(); it++ )
			programmed.push_back( "For simulation tick #" 
				+ Ceylan::toString( (*it).first ) + ", there are " 
				+ Ceylan::toString( 
					static_cast<Ceylan::Uint32>( (*it).second.size() ) )
				+ " object(s) programmed" ) ;
				
		buf << Ceylan::formatStringList( programmed ) ;
		
	}	
	
	
	if ( _screenshotMode )
	{
	
		buf << ". Current movie frame period for screenshot mode is " 
			+ Ceylan::toString( _screenshotPeriod ) 
			+ " engine ticks, which corresponds to a frequency of "
			+ Ceylan::toString( 1E6 /
				( _screenshotPeriod * _engineTickDuration ), /* precision */ 2 )
			+ " frames per second" ;	

		// No simulation nor rendering tick can be missed in screenshot mode.
		return buf.str() ;
	}
	
		
	if ( _missedSimulationTicks == 0 )
	{
	
		buf << ". No simulation tick was missed" ;
	
	}	
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
		 	
			
	if ( _missedInputPollingTicks == 0 )
		buf << ". No input tick was missed" ;
	else
	{
	
		Ceylan::System::Second sec ;
		Ceylan::System::Microsecond microsec ;
		Ceylan::System::getPreciseTime( sec, microsec ) ;

		buf << ". " + Ceylan::toString( _missedInputPollingTicks ) 
			+ " input ticks were missed, "
			"it sums up to an actual average input frequency of "
			<< 1E6 * ( _currentInputTick - _missedInputPollingTicks ) / 
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
			"Scheduler::GetExistingScheduler: no scheduler available." ) ;
			
    return * Scheduler::_internalScheduler ;

}



Scheduler & Scheduler::GetScheduler() throw( SchedulingException )
{

    if ( Scheduler::_internalScheduler == 0 )
    {
	
        LogPlug::debug( "Scheduler::GetScheduler: "
			"no scheduler available, creating new one" ) ;
			
        Scheduler::_internalScheduler = new Scheduler() ;
		
    }
    else
    {
        LogPlug::debug( "Scheduler::GetScheduler: "
			"returning already constructed instance of scheduler, "
			"no creation." ) ;
    }

    LogPlug::debug( "Scheduler::GetScheduler: returning Scheduler instance "
		+ Ceylan::toString( Scheduler::_internalScheduler ) ) ;

    return * Scheduler::_internalScheduler ;

}



void Scheduler::DeleteExistingScheduler() throw( SchedulingException )
{

    if ( Scheduler::_internalScheduler != 0 )
		throw SchedulingException( "Scheduler::DeleteExistingScheduler: "
			"there was no already existing scheduler." ) ;
			
#if OSDL_DEBUG_SCHEDULER
    LogPlug::debug( 
		"Scheduler::DeleteExistingScheduler: effective deleting." ) ;
#endif // OSDL_DEBUG_SCHEDULER
		
    delete Scheduler::_internalScheduler ;
	Scheduler::_internalScheduler = 0 ;

}



void Scheduler::DeleteScheduler() throw()
{

    if ( Scheduler::_internalScheduler != 0 )
    {
	
#if OSDL_DEBUG_SCHEDULER
        LogPlug::debug( "Scheduler::DeleteScheduler: effective deleting." ) ;
#endif // OSDL_DEBUG_SCHEDULER
		
        delete Scheduler::_internalScheduler ;
		Scheduler::_internalScheduler = 0 ;
    }
    else
    {
	
#if OSDL_DEBUG_SCHEDULER
        LogPlug::debug( "Scheduler::DeleteScheduler: no deleting needed." ) ;
#endif // OSDL_DEBUG_SCHEDULER
		
    }

}





// Protected members below:


Scheduler::Scheduler() throw( SchedulingException ):
	_screenshotMode( false ),
	_desiredScreenshotFrequency( DefaultMovieFrameFrequency ),
	_screenshotPeriod( 0 ),
	_periodicSlots(),
	_programmedActivated(),
	_engineTickDuration( 0 ),
	_secondToEngineTick( 0 ),
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
	_recoveredSimulationTicks( 0 ),
	_missedSimulationTicks( 0 ),
	_recoveredRenderingTicks( 0 ),
	_missedRenderingTicks( 0 ),
	_recoveredInputPollingTicks( 0 ),
	_missedInputPollingTicks( 0 ),
	_scheduleFailureCount( 0 ),
	_eventsModule( 0 ),
	_renderer( 0 ),
	_videoModule( 0 )
{

	_subSecondSleepsAvailable = Ceylan::System::areSubSecondSleepsAvailable() ;

	if ( ! _subSecondSleepsAvailable )
		throw SchedulingException( "Scheduler constructor: "
			"scheduler could not be created as no subsecond sleep is "
			"available on this platform." ) ;
		
	// Force precomputation for scheduling granularity, since it takes time:
	send( "On scheduler creation, "
		"detected operating system scheduling granularity is about "
		+ Ceylan::toString( getSchedulingGranularity() ) + " microseconds." ) ;

	// Update _simulationPeriod, _renderingPeriod and _screenshotPeriod:
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
	 * Set up idle callback (with default strategy) if not done already:
	 * (will use atomic sleeps, with a 10% margin in anticipated delay)
	 *
	 */
	if ( _idleCallback == 0 && _idleCallbackMaxDuration == 0 )
		_idleCallbackMaxDuration = 
			static_cast<Microsecond>( 1.1 * getSchedulingGranularity() ) ;

	/*
	 * This is the default value for the duration of an idle callback,
	 * it will be modulated according to runtime measures to be more accurate.
	 *
	 * Even under heavy loads, the measured scheduling granularity is stable
	 * and accurate, hence it is a reliable reference. Let's call it Dstart.
	 *
	 * During the scheduler run, the machine can become more loaded than at
	 * the scheduler startup. On average, each atomic sleep will last for
	 * longer, and the scheduler has to adapt. Hence it measures the duration
	 * Dlast of the latest atomic sleep as an indicator for next sleep.
	 *
	 * The forecast duration D for next atomic sleep will be:
	 * D = (new Dlast) = ( Dstart + 5 * (previous Dlast) ) / 6 
	 * (evaluated only when the scheduler is in the position of having time
	 * to spend before next deadline)
	 *
	 * It allows to converge towards Dstart even if there were a punctual huge
	 * artefact-like Dlast: otherwise the scheduler would remember only the
	 * past artefact and would never trigger an idle call, as it would never
	 * have a next deadline distant enough  to allow for such a duration.
	 *
	 * What is true for atomic sleeps is deemed true too for user-supplied
	 * idle callbacks.
	 *
	 */
	
	// Corresponds to Dstart:
	EngineTick baseIdleCallbackDuration = static_cast<EngineTick>( 
		Ceylan::Maths::Ceil( 
			static_cast<Ceylan::Float32>( _idleCallbackMaxDuration ) /
			_engineTickDuration ) ) ;
	
	// Corresponds to D (new Dlast):
	EngineTick forecastIdleCallbackDuration = baseIdleCallbackDuration ;
		
	Ceylan::System::Second idleStartingSecond, idleStoppingSecond ;
	
	Ceylan::System::Microsecond idleStartingMicrosecond, 
		idleStoppingMicrosecond ;
	
	
	_idleCallsCount = 0 ;
	
	send( "Scheduler starting in soft real-time best effort mode. " 
		"Scheduler informations: " + toString( Ceylan::low ) ) ;
	
	
	// Let's prepare to the run:
	
	_stopRequested = false ;
	
	_recoveredSimulationTicks = 0 ;
	_missedSimulationTicks    = 0 ;
	
	_recoveredRenderingTicks = 0 ;
	_missedRenderingTicks    = 0 ;
	
	_recoveredInputPollingTicks = 0 ;
	_missedInputPollingTicks    = 0 ;
	
	
#if OSDL_DEBUG_SCHEDULER
	
	/*
	 * If OSDL_DEBUG_SCHEDULER is set, run-time informations about the 
	 * scheduler will be archived and analyzed after it stopped, like black
	 * boxes.
	 *
	 */
	 
	list<SimulationTick> metSimulations ;
	list<SimulationTick> recoveredSimulations ;
	list<SimulationTick> missedSimulations ;
	
	list<RenderingTick>  metRenderings ;	
	list<RenderingTick>  recoveredRenderings ;	
	list<RenderingTick>  missedRenderings ;
		
	list<InputTick>      metInputPollings ;	
	list<InputTick>      recoveredInputPollings ;	
	list<InputTick>      missedInputPollings ;	
	
#endif // OSDL_DEBUG_SCHEDULER
	
	
	/*
	 * Initial time is first measured and converted to engine, simulation,
	 * rendering and input times:
	 * _currentEngineTick, for an engine tick duration of 1000 (microseconds),
	 * should wrap around if the program runs for more than 48 days. 
	 *
	 * If engine tick duration is divided by two, then the period until
	 * wrap-around will be divided by two as well.
	 *
	 */	
	getPreciseTime( _scheduleStartingSecond, _scheduleStartingMicrosecond ) ;
	 
	/*
	 * Starts with all zero	ticks:	
	 * (not using 0 constants allows to tweak the engine to make it start at
	 * arbitrary engine tick,for debugging).
	 *
	 */
	_currentEngineTick     = computeEngineTickFromCurrentTime() ;	
	
	_currentSimulationTick = _currentEngineTick / _simulationPeriod  ;	
	_currentRenderingTick  = _currentEngineTick / _renderingPeriod;
	_currentInputTick      = _currentEngineTick / _inputPeriod;
	
	OSDL_SCHEDULE_LOG( "Simulation period: " << _simulationPeriod ) ;
	OSDL_SCHEDULE_LOG( "Rendering period: "  << _renderingPeriod  ) ;
	OSDL_SCHEDULE_LOG( "Input period: "      << _inputPeriod      ) ;
		
	EngineTick nextSimulationDeadline = _currentEngineTick + _simulationPeriod ;
	EngineTick nextRenderingDeadline  = _currentEngineTick + _renderingPeriod ;
	EngineTick nextInputDeadline      = _currentEngineTick + _inputPeriod ;
	
	EngineTick nextDeadline = Ceylan::Maths::Min( nextSimulationDeadline,
		nextRenderingDeadline, nextInputDeadline ) ;
	
	
	/*
	 * By construction the actual frequencies can only be less than the
	 * expected ones, which are difficult to enforce: we would never have 
	 * 100 Hz, but often 95 Hz or so, because of OS process switching, if no 
	 * tolerance was used for deadlines. 
	 *
	 * The algorithm checks the delay D (time exceeding a missed deadline) for
	 * a given tick, if the delay is small enough
	 * (ex for rendering ticks: D < rendering tolerance = period / 4 = 6 ms ),
	 * then the scheduler does as if the delay had not existed: it does not
	 * cancel or skip anything, instead it activates the rendering as usual,
	 * despite the delay. 
	 *
	 * Otherwise, if the actions are really triggered too late (delay higher
	 * than the tolerance), then the scheduler use the on*Skip methods.
	 *
	 *
	 * Overloaded situations are detected thanks to a kind of leaking 
	 * bucket: each delay of d ms fills the bucket by an amount that increase
	 * with d (depending on the tolerance being exceeded or not), each elapsed
	 * engine tick  makes the bucket leak a bit. 
	 *
	 * If the bucket gets filled up to a given threshold, then the system is
	 * deemed overloaded (its ressources cannot keep up with what the 
	 * application demands) and counter-measures are triggered (ex: more skips 
	 * are triggered).
	 *
	 */
	
	// Delays will fill it, whereas it leaks regularly:
	Delay delayBucket = 0 ;
	
	// Maximum fill bucket encountered:
	Delay maxDelayBucket = 0 ;

	/*
	 * Records all delays to compute average fill level 
	 * (beware to the overflow):
	 *
	 */
	Delay delayCumulativeBucket = 0 ;
	
	// When the bucket reaches that level, the machine is actually overloaded:
	const Delay bucketFillThreshold = 750 ;

	/*
	 * Leaking factor: at each engine tick, 
	 * (current fill level) = (previous fill level) * bucketLeakingFactor
	 *
	 */
	const Ceylan::Float32 bucketLeakingFactor = 0.9f ;
	
	_scheduleFailureCount = 0 ;
	
	
	/*
	 * If a simulation tick is late of up to 50 ms, it is considered on time
	 * neverthess, and activation takes place as normal.
	 *
	 * Higher delays will lead to simulation skips.
	 *
	 * The threshold is quite high, as we really want to avoid skipping 
	 * simulation steps. 
	 *
	 * For simulation, the threshold is based on an actual (soft-realtime)
	 * delay, not a fraction of the simulation period.
	 *
	 */
	const Microsecond simulationToleranceTime = 50000 ;
	
	const EngineTick simulationToleranceTick = static_cast<EngineTick>( 
		simulationToleranceTime / _engineTickDuration ) ; 
	
	send( "Simulation tolerance time is " 
		+ Ceylan::toString( simulationToleranceTime ) + " microseconds, i.e. "
		+ Ceylan::toString( simulationToleranceTick ) + " engine ticks." ) ;
		
		
	/*
	 * If a rendering tick is late of up to 2 ms, it is considered on time
	 * neverthess, and activation takes place as normal.
	 *
	 * Higher delays will lead to rendering skips.
	 *
	 * The threshold is quite low, as skipping rendering steps is not that
	 * serious: next rendering will replace it, we do not <b>have</b> to
	 * force this particular rendering to happen.
	 *
	 * For rendering, the threshold is based on a fraction of the rendering
	 * period, as what it searched for is stability between renderings.
	 *
	 */
	
	/*
	 * A rendering is accepted if it would be no more late than one quarter 
	 * of its period:
	 *
	 */
	const EngineTick renderingToleranceTick = _renderingPeriod / 4 ;
	
	send( "Rendering tolerance is "
		+ Ceylan::toString( renderingToleranceTick ) 
		+ " engine ticks, which translates into a tolerance of " 
		+ Ceylan::toString( renderingToleranceTick * _engineTickDuration )
		+ " microseconds." ) ;
		
		
	/*
	 * If an input tick is late of up to 2 ms, it is considered on time
	 * neverthess, and activation takes place as normal.
	 *
	 * Higher delays will lead to input skips.
	 *
	 * The threshold is quite low, as skipping input steps is not that
	 * serious: next input step will replace it, we do not <b>have</b> to
	 * force this particular input polling to happen.
	 *
	 * For input polling, the threshold is based on a fraction of the input
	 * period, as what it searched for is stability between renderings.
	 *
	 */

	/*
	 * A rendering is accepted if it would be no more late than half 
	 * of its period:
	 *
	 */
	const EngineTick inputToleranceTick = _inputPeriod / 2 ;
	
	send( "Input polling tolerance is "
		+ Ceylan::toString( inputToleranceTick ) 
		+ " engine ticks, which translates into a tolerance of " 
		+ Ceylan::toString( inputToleranceTick * _engineTickDuration )
		+ " microseconds." ) ;
		
		
	// Store scheduling starting time: 
	getPreciseTime( _scheduleStartingSecond, _scheduleStartingMicrosecond ) ;
	
	
	// Enter the schedule loop:
	
	while ( ! _stopRequested )
	{
		
		
		OSDL_SCHEDULE_LOG( "[ E: " << _currentEngineTick 
			<< " ; S: " << _currentSimulationTick
			<< " ; R: " << _currentRenderingTick		
			<< " ; I: " << _currentInputTick 
			<< " ; B: " << delayBucket << " ]" ) ;


#if OSDL_DEBUG_SCHEDULER

		// Will default settings, one log per 1s:
		if ( _currentEngineTick % 1000 == 0 )
			LogPlug::debug( "[ E: " + Ceylan::toString( _currentEngineTick )
				+ " ; S: " + Ceylan::toString( _currentSimulationTick )
				+ " ; R: " + Ceylan::toString( _currentRenderingTick )
				+ " ; I: " + Ceylan::toString( _currentInputTick )
				+ " ; B: " + Ceylan::toString( delayBucket )
				+ " ]" ) ;			
				
#endif // OSDL_DEBUG_SCHEDULER

		
		delayCumulativeBucket += delayBucket ;
		
		// Bucket leaks regularly:
		delayBucket = static_cast<Delay>( delayBucket * bucketLeakingFactor ) ;
				
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
		 						
		// We hereby suppose we are just at the beginning of a new engine tick.
		
		
		// What is to be done on this new engine tick ?
		
		
		// Perform all scheduling actions for this tick:
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
		// Time has elapsed during simulation....


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
		// Still more time elapsed in the rendering....


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
		 * Check we did not miss any deadline: if these scheduling actions
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
		
		
		/*
		 * Should a computer be unable to stand the requested load, it
		 * may be unable to catch up with the clock, using the delay bucket to
		 * escape from this loop on desesperate cases:
		 *
		 */
		while ( nextSimulationDeadline < _currentEngineTick + 1 
			&& delayBucket < ShutdownBucketLevel )	
		{			
		
		
			// Manage all missed simulation steps and warn:
			EngineTick missedTicks = 
				_currentEngineTick + 1 - nextSimulationDeadline ;
			
			if ( missedTicks > simulationToleranceTick )
			{
			
				OSDL_SCHEDULE_LOG( "##### Simulation deadline #"
					+ Ceylan::toString( _currentSimulationTick ) 
					+ " missed of "	+ Ceylan::toString( missedTicks )
					+ " engine ticks (" 
					+ Ceylan::toString( missedTicks * _engineTickDuration )  
					+ " microseconds), cancelling activations." ) ;
			
				/*
				 * Missing a simulation deadline is serious, so the bucket
				 * needs to fill quickly if tolerance is exceeded. 
				 *
				 * However there happens sometimes rare glitches (Dirac peaks)
				 * that lead to very significant delays.
				 *
				 * They are one-shot, and with an affine delay function they
				 * would saturate for too long the bucket, whereas the 
				 * computer can actually face the average load quite well.
				 *
				 * Hence we chose an increasing delay function which does not
				 * lead to too explosive values for high arguments: the square
				 * root, more precisely f: d -> 35.sqrt( 2 * d )
				 * f(500) is about 1100.
				 * 
				 * Check with gnuplot:
				 * set xrange [0:1000]
				 * set autoscale
				 * plot 35 * sqrt( 2 * x )
				 *
				 * We tried also f: d -> 20.log( 2*d ), but the log smoothes
				 * too much the delay curve, which leads to a scheduler 
				 * stopping too late.
				 * For example, when making the scheduler fail because of logs 
				 * synchronously written to disk with the classical plug, it
				 * fails with a logarithm-based delay after roughly one minute
				 * (far too long), whereas with square root it lasts for 
				 * about 5 seconds (ok).
				 * 
				 * Previous setting: plot 20 * log( 2 * x )
				 *
				 */
				delayBucket += 35 * static_cast<Delay>( 
					Ceylan::Maths::Sqrt( 2.0f * missedTicks ) ) ;

#if OSDL_DEBUG_SCHEDULER
				missedSimulations.push_back( _currentSimulationTick ) ;
#endif // OSDL_DEBUG_SCHEDULER
			
				
				/*
				 * If ever this call is longer than the simulation period, the 	
				 * scheduler will go in an infinite loop:
				 *
				 */
				onSimulationSkipped( _currentSimulationTick ) ;
			
				
			}	
			else
			{
			
				// No onSimulationRecovered really needed:
				_recoveredSimulationTicks++ ;
				
				// This will not be resolved on next engine tick finally:
				missedTicks-- ;
				
				OSDL_SCHEDULE_LOG( "@@@@@ Simulation deadline #"
					+ Ceylan::toString( _currentSimulationTick ) 
					+ " recovered from "
					+ Ceylan::toString( missedTicks )
					+ " engine ticks delay (" 
					+ Ceylan::toString( missedTicks * _engineTickDuration )  
					+ " microseconds)." ) ;
				
				// Small constant penalty:	
				delayBucket += 20 ;

#if OSDL_DEBUG_SCHEDULER
				recoveredSimulations.push_back( _currentSimulationTick ) ;
#endif // OSDL_DEBUG_SCHEDULER
			
				scheduleSimulation( _currentSimulationTick ) ;
			
			}

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
		while ( nextRenderingDeadline < _currentEngineTick + 1
			&& delayBucket < ShutdownBucketLevel )	
		{			
		
			// Manage all missed rendering steps and warn:
			EngineTick missedTicks = 
				_currentEngineTick + 1 - nextRenderingDeadline ;

			if ( missedTicks > renderingToleranceTick )
			{

				OSDL_SCHEDULE_LOG( "##### Rendering deadline #"
					+ Ceylan::toString( _currentRenderingTick ) 
					+ " missed of "	+ Ceylan::toString( missedTicks )
					+ " engine ticks (" 
					+ Ceylan::toString( missedTicks * _engineTickDuration )  
					+ " microseconds), cancelling rendering." ) ;

				// Missing a rendering deadline is annoying:
				delayBucket += 5 * static_cast<Delay>( 
					Ceylan::Maths::Sqrt( 2.0f * missedTicks ) ) ;
					
								
#if OSDL_DEBUG_SCHEDULER
				missedRenderings.push_back( _currentRenderingTick ) ;
#endif // OSDL_DEBUG_SCHEDULER
			
				onRenderingSkipped( _currentRenderingTick ) ;
			
			}
			else
			{	
			
				// No onRenderingRecovered really needed:
				_recoveredRenderingTicks++ ;
			
				// This will not be resolved on next engine tick finally:
				missedTicks-- ;

				OSDL_SCHEDULE_LOG( "@@@@@ Rendering deadline #"
					+ Ceylan::toString( _currentRenderingTick ) 
					+ " recovered from "	+ Ceylan::toString( missedTicks )
					+ " engine ticks delay (" 
					+ Ceylan::toString( missedTicks * _engineTickDuration )  
					+ " microseconds)." ) ;
				
				// Small constant penalty:	
				delayBucket += 10 ;

#if OSDL_DEBUG_SCHEDULER
				recoveredRenderings.push_back( _currentRenderingTick ) ;
#endif // OSDL_DEBUG_SCHEDULER

				scheduleRendering( _currentRenderingTick ) ;
			
			}
			
			_currentRenderingTick++ ;
			
			nextRenderingDeadline += _renderingPeriod ;
			
			_currentEngineTick = computeEngineTickFromCurrentTime() ;
						
		}
		
		
		/*
		 * Lastly, take care of the input polling:
		 *
		 */
		while ( nextInputDeadline < _currentEngineTick + 1	
			&& delayBucket < ShutdownBucketLevel )	
		{			
		
			/*
			 * Manage all missed input steps and warn (this is not too 
			 * serious skip):
			 *
			 */
			EngineTick missedTicks = 
				_currentEngineTick + 1 - nextInputDeadline ;

			if ( missedTicks > inputToleranceTick )
			{
			
				OSDL_SCHEDULE_LOG( "##### Input deadline #"
					+ Ceylan::toString( _currentInputTick ) 
					+ " missed of "	+ Ceylan::toString( missedTicks )
					+ " engine ticks (" 
					+ Ceylan::toString( missedTicks * _engineTickDuration )  
					+ " microseconds), cancelling input polling." ) ;

				// Missing an input deadline should be avoided:
				delayBucket += static_cast<Delay>( 
					Ceylan::Maths::Sqrt( 2.0f * missedTicks ) ) ;
			
#if OSDL_DEBUG_SCHEDULER
				missedInputPollings.push_back( _currentInputTick ) ;
#endif // OSDL_DEBUG_SCHEDULER
			
				onInputSkipped( _currentInputTick ) ;
			
			}
			else
			{
			
				// No onInputPollingRecovered really needed:
				_recoveredInputPollingTicks++ ;
				
				// This will not be resolved on next engine tick finally:
				missedTicks-- ;

				OSDL_SCHEDULE_LOG( "@@@@@ Input deadline #"
					+ Ceylan::toString( _currentInputTick ) 
					+ " recovered from " + Ceylan::toString( missedTicks )
					+ " engine ticks delay (" 
					+ Ceylan::toString( missedTicks * _engineTickDuration )  
					+ " microseconds)." ) ;
				
				// Small constant penalty:	
				delayBucket += 5 ;

#if OSDL_DEBUG_SCHEDULER
				recoveredInputPollings.push_back( _currentInputTick ) ;
#endif // OSDL_DEBUG_SCHEDULER

				scheduleInput( _currentInputTick ) ;
			
			
			}	

			_currentInputTick++ ;
			
			nextInputDeadline += _inputPeriod ;
			
			_currentEngineTick = computeEngineTickFromCurrentTime() ;
						
		}
		 		
		if ( maxDelayBucket < delayBucket )
			maxDelayBucket = delayBucket ;

		// Maybe counter-measures will be taken here:
		if ( delayBucket > bucketFillThreshold )
			onScheduleFailure( delayBucket ) ;

		/*
		 * Will wait until next deadline, possibly skipping several engine
		 * ticks:
		 *
		 */
		nextDeadline = Ceylan::Maths::Min( nextSimulationDeadline,
			nextRenderingDeadline, nextInputDeadline ) ;

		if ( nextDeadline < _currentEngineTick )
		{
		
			string message = "We are at engine tick #" 
				+ Ceylan::toString( _currentEngineTick ) 
				+ ", already late for next deadline, which is at "
				+ Ceylan::toString( nextDeadline ) + "." ;
			
			OSDL_SCHEDULE_LOG( message ) ;
			LogPlug::warning( message ) ;
			
			// Delay will be taken care of at next iteration:
			continue ;
		}
		else if ( nextDeadline <= _currentEngineTick + 1 )
		{
			
			/*
			 * nextDeadline is this current engine tick, or the next one.
			 * In both cases it is to be handled immediately, no special
			 * jump to perform:
			 *
			 */
			OSDL_SCHEDULE_LOG( 
				"No jump could be performed, just continuing." ) ;
			
		}
		else
		{
		
			// Here nextDeadline > _currentEngineTick + 1, hence jumpLength > 0.
		
			/*
			 * Computes the intermediate delays we are jumping over:
			 *
			 */
			EngineTick jumpLength = nextDeadline - _currentEngineTick - 1 ;

			OSDL_SCHEDULE_LOG( "Next deadline is " 
				+ Ceylan::toString( jumpLength + 1 ) 
				+ " engine tick(s) away." ) ;


			// Leaks as if there had been no jump in engine ticks:
			while ( jumpLength > 0 )
			{
		
				delayBucket = static_cast<Delay>( 
					delayBucket * bucketLeakingFactor ) ;
				delayCumulativeBucket += delayBucket ;
				jumpLength-- ;
			
			} ;

		}
		
		/*
		 * Do nothing: wait, first with idle callback (which results in
		 * atomic sleeps by default), then for fine-grain with soft busy
		 * waitings.
		 *
		 */
		 
		 // Will force to decrease even though no sleep is performed this time:
		forecastIdleCallbackDuration = static_cast<EngineTick>(
				( baseIdleCallbackDuration + 5 * forecastIdleCallbackDuration )
					/ 6.0f ) ;
	 	
		OSDL_SCHEDULE_LOG( "Newly forecast idle callback duration is "
			+ Ceylan::toString( static_cast<Ceylan::Uint32>( 
					forecastIdleCallbackDuration ) ) 
			+ " engine ticks." ) ;
				
		// Atomic sleeps: 		 
		while ( _currentEngineTick + forecastIdleCallbackDuration 
			< nextDeadline )
		{
		
			getPreciseTime( idleStartingSecond, idleStartingMicrosecond ) ;
					
			/*
			 * OSDL_SCHEDULE_LOG( 
			 * "Waiting for the end of engine tick " << _currentEngineTick ) ;
			 *
			 */
			onIdle() ;

			getPreciseTime( idleStoppingSecond, idleStoppingMicrosecond ) ;
			
			Microsecond idleDuration = getDurationBetween( 
				idleStartingSecond, idleStartingMicrosecond,
				idleStoppingSecond, idleStoppingMicrosecond ) ;
				
			OSDL_SCHEDULE_LOG( "Measured idle duration is "
				+ Ceylan::toString( static_cast<Ceylan::Uint32>( 
					idleDuration ) ) + " microseconds." ) ;
					
			/*
			 * In this loop, previous idle duration is considered the best
			 * estimate for next one:
			 *
			 */	
			forecastIdleCallbackDuration = static_cast<EngineTick>(
				Ceylan::Maths::Ceil( static_cast<Ceylan::Float32>(
					idleDuration / _engineTickDuration ) ) ) ;
			
			_currentEngineTick = computeEngineTickFromCurrentTime() ;
			
		}	
		
		
		// Then busy waiting (still quite soft due to getPreciseTime calls):
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
	
	// It is equal to 1E6 / ( total runtime in microseconds ):
	Ceylan::Float64 totalRuntimeFactor ;
	
	// Avoid overflows:
	if ( scheduleStoppingSecond - _scheduleStartingSecond <
			MaximumDurationWithMicrosecondAccuracy )
	{	
		 
		totalRuntimeFactor = 1E6 / getDurationBetween( 
			_scheduleStartingSecond, _scheduleStartingMicrosecond,
			scheduleStoppingSecond, scheduleStoppingMicrosecond ) ;
			
	}
	else
	{
	
		// It is just a (rather good , error below 1/4100) approximation:
		totalRuntimeFactor =  1.0f / 
			( scheduleStoppingSecond - _scheduleStartingSecond ) ;
		
	}
	
	
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
		" 		<th>Recovered count</th>"
		" 		<th>Recovered percentage</th>"
		" 		<th>Missed count</th>"
		" 		<th>Missed percentage</th>"
		"	</tr>" ;
	
	
	Ceylan::Uint8 precision = 2 /* digits after the dot */ ;
	
	// Engine:
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
		+ Ceylan::toString( _currentEngineTick * totalRuntimeFactor,
			precision )
		+ " Hz</td>"
		"		<td>" + Ceylan::toString( _engineTickDuration ) + "</td>"
		"		<td>1</td>"
		"		<td>" + Ceylan::toString( _currentEngineTick ) + "</td>"
		"		<td>N/A</td>"		
		"		<td>N/A</td>"		
		"		<td>N/A</td>"		
		"		<td>N/A</td>"		
		"	</tr>" ;
		
		
	// Simulation:	
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
			( _currentSimulationTick - _missedSimulationTicks ) *
				totalRuntimeFactor, precision ) + " Hz</td>"
		"		<td>" + Ceylan::toString( 
			_simulationPeriod * _engineTickDuration ) + "</td>"
		"		<td>" + Ceylan::toString( _simulationPeriod ) + "</td>"
		"		<td>" + Ceylan::toString( _currentSimulationTick ) + "</td>"
		"		<td>" + Ceylan::toString( _recoveredSimulationTicks ) + "</td>"	
		"		<td>" + Ceylan::toString( 
			100.0f * _recoveredSimulationTicks / _currentSimulationTick, 
			precision ) 
		+ "%</td>"	
		"		<td>" + Ceylan::toString( _missedSimulationTicks ) + "</td>"	
		"		<td>" + Ceylan::toString( 
			100.0f * _missedSimulationTicks / _currentSimulationTick, 
			precision ) 
		+ "%</td>"	
		"	</tr>" ;
		
		
	// Rendering:	
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
			( _currentRenderingTick - _missedRenderingTicks ) * 
				totalRuntimeFactor, precision ) + " Hz</td>"
		"		<td>" + Ceylan::toString( 
			_renderingPeriod * _engineTickDuration ) + "</td>"
		"		<td>" + Ceylan::toString( _renderingPeriod ) + "</td>"
		"		<td>" + Ceylan::toString( _currentRenderingTick ) + "</td>"
		"		<td>" + Ceylan::toString( _recoveredRenderingTicks ) + "</td>"	
		"		<td>" + Ceylan::toString( 
			100.0f * _recoveredRenderingTicks / _currentRenderingTick, 
				precision ) 
		+ "%</td>"	
		"		<td>" + Ceylan::toString( _missedRenderingTicks ) + "</td>"	
		"		<td>" + Ceylan::toString( 
			100.0f * _missedRenderingTicks / _currentRenderingTick, precision ) 
		+ "%</td>"	
		"	</tr>" ;
		
	// Input:	
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
			( _currentInputTick - _missedInputPollingTicks ) * 
				totalRuntimeFactor, precision ) + " Hz</td>"
		"		<td>" + Ceylan::toString( 
			_inputPeriod * _engineTickDuration ) + "</td>"
		"		<td>" + Ceylan::toString( _inputPeriod ) + "</td>"
		"		<td>" + Ceylan::toString( _currentInputTick ) + "</td>"
		"		<td>" + Ceylan::toString( _recoveredInputPollingTicks ) 
		+ "</td>"	
		"		<td>" + Ceylan::toString( 
			100.0f * _recoveredInputPollingTicks / _currentInputTick, 
				precision ) 
		+ "%</td>"	
		"		<td>" + Ceylan::toString( _missedInputPollingTicks ) + "</td>"	
		"		<td>" + Ceylan::toString( 
			100.0f * _missedInputPollingTicks / _currentInputTick, precision ) 
		+ "%</td>"	
		"	</tr>" ;
		
	table += "</table>" ;	
	
	if ( Ceylan::TextDisplayable::GetOutputFormat() ==
		Ceylan::TextDisplayable::html )
	send( table ) ;
	
	list<string> summary ;
	
	string durationComment = "The scheduler ran for " 
		+ Ceylan::Timestamp::DescribeDuration( 
			scheduleStoppingSecond - _scheduleStartingSecond ) ;
			
	
	if ( scheduleStoppingSecond - _scheduleStartingSecond <
			MaximumDurationWithMicrosecondAccuracy )			
		durationComment += ", more precisely " 
			+ Ceylan::toString( getDurationBetween(
 				_scheduleStartingSecond, _scheduleStartingMicrosecond,
 	 			scheduleStoppingSecond, scheduleStoppingMicrosecond ) )
			+ " microseconds." ;
	else
		durationComment += "." ;
			
	summary.push_back( durationComment ) ;
	
	summary.push_back( Ceylan::toString( _idleCallsCount ) 
		+ " idle calls have been made." ) ;

	summary.push_back( "Each idle call was expected to last for "
		+ Ceylan::toString( _idleCallbackMaxDuration ) + " microseconds, i.e. "
		+ Ceylan::toString( baseIdleCallbackDuration ) + " engine ticks." ) ;

	summary.push_back( "Average bucket level has been " 
		+ Ceylan::toString( 
			( 1.0f * delayCumulativeBucket ) / _currentEngineTick,
			/* precision */ 2 ) 
		+ ", maximum bucket level has been "
		+ Ceylan::toString( maxDelayBucket ) 
		+ ", bucket fill threshold is " 
		+ Ceylan::toString( bucketFillThreshold )
		+ ", this threshold has been reached "
		+ Ceylan::toString( _scheduleFailureCount )
		+ " times, shutdown bucket level is "
		+ Ceylan::toString( ShutdownBucketLevel) + "." ) ;
	
	send( "Scheduler stopping, run summary is: " 
		+ Ceylan::formatStringList( summary ) ) ;
	
	send( "Full scheduler infos: " + toString( Ceylan::high ) ) ;	

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
	
	
	
	// For simulation ticks:			
	
	SimulationTick totalSimulationTicks = static_cast<SimulationTick>( 
		metSimulations.size() + recoveredSimulations.size() 
		+ missedSimulations.size() ) ;
		
	LogPlug::debug( "Total simulation ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( totalSimulationTicks ) ) 
		+ "." ) ;

	LogPlug::debug( "Directly met simulation ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( metSimulations.size() ) )
		+ " (" + Ceylan::toString( 100.0f * metSimulations.size() 
			/ totalSimulationTicks, /* precision */ 2 ) + "%)." ) ;
						
	LogPlug::debug( "Recovered (indirectly met) simulation ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( recoveredSimulations.size() ) )
		+ " (" + Ceylan::toString( 100.0f * recoveredSimulations.size() 
			/ totalSimulationTicks, /* precision */ 2 ) + "%)." ) ;

	if ( recoveredSimulations.size() != _recoveredSimulationTicks )
		LogPlug::error( "Inconsistency in recovered simulation count." ) ;

				
	LogPlug::debug( "Missed simulation ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( missedSimulations.size() ) )
		+ " (" + Ceylan::toString( 100.0f * missedSimulations.size() 
			/ totalSimulationTicks, /* precision */ 2 ) + "%)." ) ;
	
	if ( missedSimulations.size() != _missedSimulationTicks )
		LogPlug::error( "Inconsistency in missed simulation count." ) ;

	if ( _currentSimulationTick - _recoveredSimulationTicks 
			- _missedSimulationTicks != metSimulations.size() )
		LogPlug::error( "Inconsistency in overall simulation count." ) ;
			
			 
	string res ;
	

	if ( beVerbose )
	{
	
		if ( beVeryVerbose )
		{
		
			for ( list<SimulationTick>::const_iterator it 
					= metSimulations.begin(); it != metSimulations.end(); it++ )
				res += 	Ceylan::toString( *it ) + " - " ;
		 
			LogPlug::debug( "Met simulation ticks: " + res ) ;
			res.clear() ;
	
		}
		
		
		for ( list<SimulationTick>::const_iterator it 
				= recoveredSimulations.begin(); 
					it != recoveredSimulations.end(); it++ )
			res += 	Ceylan::toString( *it ) + " - " ;

		LogPlug::debug( "Recovered simulation ticks: " + res ) ;
		res.clear() ;
		
		for ( list<SimulationTick>::const_iterator it 
				= missedSimulations.begin(); it != missedSimulations.end(); 
				it++ )
			res += 	Ceylan::toString( *it ) + " - " ;

		LogPlug::debug( "Missed simulation ticks: " + res ) ;
		res.clear() ;
	}
	
		
	/*
	 * Check that all simulation ticks were handled one and only one time, 
	 * one way or another (scheduled: met or recovered, or skipped).
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
			LogPlug::error( "Met simulation tick #" 
				+ Ceylan::toString( newSimulationTick ) 
				+ " should not have been scheduled more than once." ) ;	
						
	}
	
	
	for ( list<SimulationTick>::const_iterator it =
		recoveredSimulations.begin(); it != recoveredSimulations.end(); it++ )
	{
	
		newSimulationTick = (*it) ;
		
		if ( simulationTicks[ newSimulationTick ] == false )
			simulationTicks[ newSimulationTick ] = true ;
		else
			LogPlug::error( "Recovered simulation tick #" 
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
			LogPlug::error( "Missed simulation tick #" 
				+ Ceylan::toString( newSimulationTick ) 
				+ " should not have been skipped, "
				"since had already been taken into account." ) ;
							
	}
	
	for ( Events::SimulationTick i = 0; i < _currentSimulationTick; i++ )
		if ( simulationTicks[ i ] == false )
			LogPlug::error( "Simulation tick #" + Ceylan::toString( i ) 
				+ " has never been scheduled." ) ;	
				
	delete simulationTicks ;
	


	// For rendering ticks:			

	RenderingTick totalRenderingTicks = static_cast<RenderingTick>( 
		metRenderings.size() + recoveredRenderings.size() 
		+ missedRenderings.size() ) ;

	LogPlug::debug( "Total rendering ticks: " 
		+ Ceylan::toString( totalRenderingTicks ) + "." ) ;
		
	LogPlug::debug( "Directly met rendering ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( metRenderings.size() ) )
		+ " (" + Ceylan::toString( 100.0f * metRenderings.size() 
			/ totalRenderingTicks, /* precision */ 2 ) + "%)." ) ;
				
	LogPlug::debug( "Recovered (indirectly met) rendering ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( recoveredRenderings.size() ) ) 
		+ " (" + Ceylan::toString( 100.0f * recoveredRenderings.size() 
			/ totalRenderingTicks, /* precision */ 2 ) + "%)." ) ;

	if ( recoveredRenderings.size() != _recoveredRenderingTicks )
		LogPlug::error( "Inconsistency in recovered rendering count." ) ;

				
	LogPlug::debug( "Missed rendering ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( missedRenderings.size() ) ) 
		+ " (" + Ceylan::toString( 100.0f * missedRenderings.size() 
			/ totalRenderingTicks, /* precision */ 2 ) + "%)." ) ;

	if ( missedRenderings.size() != _missedRenderingTicks )
		LogPlug::error( "Inconsistency in missed rendering count." ) ;

	if ( _currentRenderingTick - _recoveredRenderingTicks 
			- _missedRenderingTicks != metRenderings.size() )
		LogPlug::error( "Inconsistency in overall rendering count." ) ;
				
		
				
	if ( beVerbose )
	{
	
		if ( beVeryVerbose )
		{
		
			for ( list<RenderingTick>::const_iterator it 
					= metRenderings.begin(); it != metRenderings.end(); it++ )
				res += 	Ceylan::toString( *it ) + " - " ;
		 
			LogPlug::debug( "Met rendering ticks: " + res ) ;
			res.clear() ;
	
		}
		
		for ( list<RenderingTick>::const_iterator it 
				= recoveredRenderings.begin(); 
				it != recoveredRenderings.end(); it++ )
			res += 	Ceylan::toString( *it ) + " - " ;

		LogPlug::debug( "Recovered rendering ticks: " + res ) ;
		res.clear() ;

		for ( list<RenderingTick>::const_iterator it 
				= missedRenderings.begin(); it != missedRenderings.end(); it++ )
			res += 	Ceylan::toString( *it ) + " - " ;

		LogPlug::debug( "Missed rendering ticks: " + res ) ;
		res.clear() ;

	}
	
				
	/*
	 * Check that all rendering ticks were handled one and only one time, 
	 * one way or another (scheduled: met or recovered, or skipped).
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
			LogPlug::error( "Met rendering tick #" 
				+ Ceylan::toString( newRenderingTick ) 
				+ " should not have been scheduled more than once." ) ;		
				
	}
	
	for ( list<RenderingTick>::const_iterator it = 
		recoveredRenderings.begin(); it != recoveredRenderings.end(); it++ )
	{
	
		newRenderingTick = (*it) ;
		if ( renderingTicks[ newRenderingTick ] == false )
			renderingTicks[ newRenderingTick ] = true ;
		else
			LogPlug::error( "Recovered rendering tick #" 
				+ Ceylan::toString( newRenderingTick ) 
				+ " should not have been scheduled more than once." ) ;		
				
	}
	
	
	for ( list<RenderingTick>::const_iterator it = missedRenderings.begin(); 
		it != missedRenderings.end(); it++ )
	{
		newRenderingTick = (*it) ;
		if ( renderingTicks[ newRenderingTick ] == false )
			renderingTicks[ newRenderingTick ] = true ;
		else
			LogPlug::error( "Missed rendering tick #" 
				+ Ceylan::toString( newRenderingTick ) 
				+ " should not have been been skipped, "
				"since had already been taken into account." ) ;
							
	}
	
	for ( Events::RenderingTick i = 0; i < _currentRenderingTick; i++ )
		if ( renderingTicks[ i ] == false )
			LogPlug::error( "Rendering tick #" + Ceylan::toString( i ) 
				+ " has never been scheduled." ) ;	

	delete renderingTicks ;



	// For input ticks:			


	InputTick totalInputTicks = static_cast<InputTick>( 
		metInputPollings.size() + recoveredInputPollings.size() 
		+ missedInputPollings.size() ) ;

	LogPlug::debug( "Total input ticks: " 
		+ Ceylan::toString( totalInputTicks ) + "." ) ;
		
	LogPlug::debug( "Directly met input ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( metInputPollings.size() ) ) 
		+ " (" + Ceylan::toString( 100.0f * metInputPollings.size() 
			/ totalInputTicks, /* precision */ 2 ) + "%)." ) ;
				
	LogPlug::debug( "Recovered (indirectly met) input ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( recoveredInputPollings.size() ) ) 
		+ " (" + Ceylan::toString( 100.0f * recoveredInputPollings.size() 
			/ totalInputTicks, /* precision */ 2 ) + "%)." ) ;

	if ( recoveredInputPollings.size() != _recoveredInputPollingTicks )
		LogPlug::error( "Inconsistency in recovered input polling count." ) ;
			
				
	LogPlug::debug( "Missed input ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( missedInputPollings.size() ) ) 
		+ " (" + Ceylan::toString( 100.0f * missedInputPollings.size() 
			/ totalInputTicks, /* precision */ 2 ) + "%)." ) ;
				
	if ( missedInputPollings.size() != _missedInputPollingTicks )
		LogPlug::error( "Inconsistency in missed input polling count." ) ;

	if ( _currentInputTick - _recoveredInputPollingTicks 
			- _missedInputPollingTicks != metInputPollings.size() )
		LogPlug::error( "Inconsistency in overall input polling count." ) ;
		
				
	if ( beVerbose )
	{
	
		if ( beVeryVerbose )
		{
		
			for ( list<InputTick>::const_iterator it 
					= metInputPollings.begin(); 
					it != metInputPollings.end(); it++ )
				res += 	Ceylan::toString( *it ) + " - " ;
		 
			LogPlug::debug( "Met input ticks: " + res ) ;
			res.clear() ;
	
		}
		
		for ( list<InputTick>::const_iterator it 
				= recoveredInputPollings.begin(); 
				it != recoveredInputPollings.end(); it++ )
			res += 	Ceylan::toString( *it ) + " - " ;

		LogPlug::debug( "Recovered input ticks: " + res ) ;
		res.clear() ;

		for ( list<InputTick>::const_iterator it 
				= missedInputPollings.begin(); 
				it != missedInputPollings.end(); it++ )
			res += 	Ceylan::toString( *it ) + " - " ;

		LogPlug::debug( "Missed input ticks: " + res ) ;
		res.clear() ;

	}
				
				
	/*
	 * Check that all input ticks were handled one and only one time, one 
	 * way or another:
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
			LogPlug::error( "Met input tick #" 
				+ Ceylan::toString( newInputTick ) 
				+ " should not have been scheduled more than once." ) ;
				
	}
	
	for ( list<InputTick>::const_iterator it = recoveredInputPollings.begin(); 
		it != recoveredInputPollings.end(); it++ )
	{
	
		newInputTick = (*it) ;
		if ( inputTicks[ newInputTick ] == false )
			inputTicks[ newInputTick ] = true ;
		else
			LogPlug::error( "Recovered input tick #" 
				+ Ceylan::toString( newInputTick ) 
				+ " should not have been scheduled more than once." ) ;
				
	}
	
	
	for ( list<InputTick>::const_iterator it = missedInputPollings.begin(); 
		it != missedInputPollings.end(); it++ )
	{
		newInputTick = (*it) ;
		if ( inputTicks[ newInputTick ] == false )
			inputTicks[ newInputTick ] = true ;
		else
			LogPlug::error( "Missed input tick #" 
				+ Ceylan::toString( newInputTick ) 
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
	
	// Let's prepare to the run, for which no time is to be considered:
	
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

	
	// Store scheduling starting time: 
	getPreciseTime( _scheduleStartingSecond, _scheduleStartingMicrosecond ) ;
		 
	// Starts with all zero	ticks:	
	_currentEngineTick     = 0 ;
		
	_currentSimulationTick = 0 ;	
	_currentRenderingTick  = 0 ;
	_currentInputTick      = 0 ;
	
	OSDL_SCHEDULE_LOG( "Simulation period: " << _simulationPeriod ) ;
	OSDL_SCHEDULE_LOG( "Rendering period: "  << _renderingPeriod ) ;
	OSDL_SCHEDULE_LOG( "Input period: "      << _inputPeriod ) ;
		
	// No real deadline in this mode, just checkpoints that cannot be missed:	
	EngineTick nextSimulationDeadline = _currentEngineTick + _simulationPeriod ;
	EngineTick nextRenderingDeadline  = _currentEngineTick + _renderingPeriod ;
	EngineTick nextInputDeadline      = _currentEngineTick + _inputPeriod ;
	
	EngineTick countBeforeSleep = 0 ;
	
	// Enter the schedule loop:
	
	while ( ! _stopRequested )
	{
			
		OSDL_SCHEDULE_LOG( "[ E: " << _currentEngineTick 
			<< " ; S: " << _currentSimulationTick
			<< " ; R: " << _currentRenderingTick		
			<< " ; I: " << _currentInputTick << " ]" ) ;

		countBeforeSleep++ ;
		
		/* 
		 * Normally, for each simulation tick, one scheduleSimulation is called.
		 *
		 * Similarly, for each rendering tick, one scheduleRendering is called.
		 *
		 * Same thing for inputs.
		 *
		 */
		 	
					
		// We hereby suppose we are just at the beginning of a new engine tick.
		
		// What is to be done on this new engine tick ?
		
		// Perform all scheduling actions for this tick:
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
		
		// Be nice with the operating system:
		if ( _subSecondSleepsAvailable && countBeforeSleep == 500 )
		{
			countBeforeSleep = 0 ;
			atomicSleep() ;
		}	
		
		/*
		 * OSDL_SCHEDULE_LOG( "End of schedule loop, engine tick being " 
		 * << _currentEngineTick ) ;
		 *
		 */
		
	}
	
	Second scheduleStoppingSecond ;
	Microsecond scheduleStoppingMicrosecond ;
	
	getPreciseTime( scheduleStoppingSecond, scheduleStoppingMicrosecond ) ;


	// It is equal to 1E6 / ( total runtime in microseconds ):
	Ceylan::Float64 totalRuntimeFactor ;
	
	// Avoid overflows:
	if ( scheduleStoppingSecond - _scheduleStartingSecond <
			MaximumDurationWithMicrosecondAccuracy )
	{	
		 
		totalRuntimeFactor = 1E6 / getDurationBetween( 
			_scheduleStartingSecond, _scheduleStartingMicrosecond,
			scheduleStoppingSecond, scheduleStoppingMicrosecond ) ;
			
	}
	else
	{
	
		// It is just a (rather good , error below 1/4100) approximation:
		totalRuntimeFactor =  1.0f / 
			( scheduleStoppingSecond - _scheduleStartingSecond ) ;
		
	}

	
	send( "Scheduler stopping. Scheduler infos: " 
		+ toString( Ceylan::high ) ) ;	
	

	ostringstream buf ;
	
	buf.precision( 4 ) ;
		
	buf	<< "Actual average engine frequency was "  
		<< Ceylan::toString( totalRuntimeFactor * _currentEngineTick, 
			/* precision */ 2 )
		<< " Hz, average simulation frequency was " 
		<< Ceylan::toString( totalRuntimeFactor * _currentSimulationTick,
			/* precision */ 2 )
		<< " Hz, average rendering frequency was " 
		<< Ceylan::toString( totalRuntimeFactor * _currentRenderingTick,
			/* precision */ 2 )
		<< " Hz, average input frequency was " 
		<< Ceylan::toString( totalRuntimeFactor * _currentInputTick, 
			/* precision */ 2 )
		<< " Hz." ;
		 
	send( buf.str() ) ;
			
#if OSDL_DEBUG_SCHEDULER
	
	
	/*
	 * Collect some informations, and check them to assess the scheduler
	 * behaviour.
	 *
	 */
	
	LogPlug::debug( "Total simulation ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( metSimulations.size() ) ) 
		+ "." ) ;
			

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
				+ " should not have been scheduled more than once." ) ;		
				
	}	
	
	for ( Events::SimulationTick i = 0; i < _currentSimulationTick; i++ )
		if ( simulationTicks[ i ] == false )
			LogPlug::error( "Simulation tick #" + Ceylan::toString( i ) 
				+ " has never been scheduled." ) ;	

	delete simulationTicks ;
				
				
				
				
	LogPlug::debug( "Total rendering ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( metRenderings.size() ) ) + "." ) ;
				
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
				+ " should not have been scheduled more than once." ) ;
				
	}
	
	for ( Events::RenderingTick i = 0; i < _currentRenderingTick; i++ )
		if ( renderingTicks[ i ] == false )
			LogPlug::error( "Rendering tick #" + Ceylan::toString( i ) 
				+ " has never been scheduled." ) ;	

	delete renderingTicks ;
	
	

	LogPlug::debug( "Total input polling ticks: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( metInputPollings.size() ) ) 
		+ "." ) ;

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
					+ " should not have been scheduled more than once." ) ;
					
		}
		
		for ( Events::InputTick i = 0; i < _currentInputTick; i++ )
			if ( inputTicks[ i ] == false )
				LogPlug::error( "Input tick #" + Ceylan::toString( i ) 
					+ " has never been scheduled." ) ;	
		
		delete inputTicks ;
				
	}
						
#endif // OSDL_DEBUG_SCHEDULER

	
}



EngineTick Scheduler::computeEngineTickFromCurrentTime() throw()
{

	Second currentSecond ;
	Microsecond currentMicrosecond ;
	getPreciseTime( currentSecond, currentMicrosecond ) ;
	
	/*
	 * _secondToEngineTick necessary to avoid overflow when duration in 
	 * seconds exceeds 4200 !
	 *
	
	LogPlug::debug( "Current: " + Ceylan::toString( currentSecond ) 
		+ "s and " + Ceylan::toString( currentMicrosecond ) 
		+ " microsec, started: " + Ceylan::toString( _scheduleStartingSecond ) 
		+ "s and " + Ceylan::toString( _scheduleStartingMicrosecond ) 
		+ "microsec." ) ;
		
	 */
	
	/*
	 * Avoids that 'currentMicrosecond - _scheduleStartingMicrosecond'
	 * becomes negative and overflows:
	 *
	 */
	if ( currentMicrosecond < _scheduleStartingMicrosecond )
	{
		
		// Normally that cannot happen at second #0:
#if OSDL_DEBUG_SCHEDULER
		if ( currentSecond == 0 )
		{
		
			LogPlug::fatal( "Scheduler::computeEngineTickFromCurrentTime: "
				"abnormal clock skew." ) ;
				
			_stopRequested = true ;
				
		}	
#endif // OSDL_DEBUG_SCHEDULER
		
		currentSecond-- ;
		currentMicrosecond += 1000000 ;
		
	}
		
	return static_cast<EngineTick>( 
		( currentSecond - _scheduleStartingSecond ) * _secondToEngineTick
		+ ( currentMicrosecond - _scheduleStartingMicrosecond ) 
			/ _engineTickDuration ) ;
		
}



void Scheduler::scheduleSimulation( SimulationTick current ) throw()
{

	//OSDL_SCHEDULE_LOG( "--> current simulation tick: " << current ) ;
	
	OSDL_SCHEDULE_LOG( "--- simulating ! " ) ;
		
	// Activate all objects programmed for this specific time:
	scheduleProgrammedObjects( current ) ;
		
	// Activate all objects registered in this periodic slot:
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
		
		// Empty this simulation tick when done with it:
		_programmedActivated.erase( it ) ;
	}
	
	// else: no key for this simulation tick ? Nothing to do !	 
	
}



void Scheduler::schedulePeriodicObjects( SimulationTick current ) throw()
{

	// Request each periodic slot to activate relevant objects:
	
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
			Ceylan::emergencyShutdown( "Scheduler::scheduleRendering: "
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
		Ceylan::emergencyShutdown( "Scheduler::scheduleRendering: "
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
	 * of periodic activation:
	 *
	 */
	
	/*
	 * Request each periodic slot to propagate the news to its relevant 
	 * objects:
	 *
	 */
	
	for ( list<PeriodicSlot*>::iterator it = _periodicSlots.begin(); 
		it != _periodicSlots.end();	it++ )
	{
		(*it)->onSimulationSkipped( skipped ) ;
	}
	
	// Second, send notification to programmed objects:

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

	_missedInputPollingTicks++ ;
		
	// Do nothing else, skipped inputs do not matter that much.
	
}



void Scheduler::onIdle() throw()
{

	OSDL_SCHEDULE_LOG( "--- idle callback called !" ) ;

	_idleCallsCount++ ;

	if ( _idleCallback != 0 )
	{
	
		// Call the user-supplied idle callback:
		(*_idleCallback)( _idleCallbackData ) ;
		
	}
	else
	{
	
		/*
		 * Issues an atomic sleep, chosen so that the minimum real predictable 
		 * sleeping time can be performed, scheduler-wise:
		 *
		 */
		 if ( _subSecondSleepsAvailable )
		 	Ceylan::System::atomicSleep() ;
		 
	}	
	
}



void Scheduler::onScheduleFailure( Delay currentBucket ) 
	throw( SchedulingException )
{

	_scheduleFailureCount++ ;
	
	// One may loosen frequencies and simplify scheduled tasks as well:
	
	if ( currentBucket < ShutdownBucketLevel )
	{
	
		string message = "Non-fatal schedule failure for engine tick #"
			+ Ceylan::toString( _currentEngineTick )
			+ ", delay bucket reached level "
			+ Ceylan::toString( currentBucket ) 
			+ ", this computer does not seem able to satisfy "
			"the requested load." ;
			
		// Just warn:
		OSDL_SCHEDULE_LOG( "!!!!! " + message ) ;
		
		LogPlug::error( message) ;	
		
	}
	else
	{


		OSDL_SCHEDULE_LOG( "Scheduling failure at [ E: " 
			<< _currentEngineTick 
			<< " ; S: " << _currentSimulationTick
			<< " ; R: " << _currentRenderingTick		
			<< " ; I: " << _currentInputTick 
			<< " ; B: " << currentBucket << " ]" ) ;

		string message = "Fatal schedule failure for engine tick #"
			+ Ceylan::toString( _currentEngineTick )
			+ ", delay bucket reached level "
			+ Ceylan::toString( currentBucket ) 
			+ ", this computer cannot take in charge the requested load, "
			"stopping the scheduler now." ;
			
		// Warn and stop:
		OSDL_SCHEDULE_LOG( "XXXXX " + message ) ;
		
		LogPlug::fatal( message) ;	
		
		stop() ;
		
		throw SchedulingException( message ) ;
	
	}
	
}



void Scheduler::programTriggerFor( ActiveObject & objectToProgram, 
	SimulationTick targetTick ) throw()
{

	// Add the offset of current simulation tick if triggers are not absolute:
	if ( ! objectToProgram.areProgrammedActivationsAbsolute() )
		targetTick += _currentSimulationTick ;
		
	map<SimulationTick, ListOfActiveObjects>::iterator it 
		= _programmedActivated.find( targetTick ) ;
		
	if ( it != _programmedActivated.end() )
	{
		// There is already an object list available for this tick:
		(*it).second.push_back( & objectToProgram ) ;
	}
	else
	{
		// First object for this simulation tick:
		
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
	 * Period not found, higher than all others: create it at the end 
	 * of the list.
	 *
	 */
	PeriodicSlot * newSlot = new PeriodicSlot( period ) ;
	_periodicSlots.push_back( newSlot ) ;
	
	return * newSlot ; 
	
}


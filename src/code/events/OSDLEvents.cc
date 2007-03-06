#include "OSDLEvents.h"

#include "OSDLBasic.h"             // for OSDL::GetVersion
#include "OSDLVideo.h"             // for resize
#include "OSDLScheduler.h"         // for GetScheduler
#include "OSDLRenderer.h"          // for Renderer
#include "OSDLJoystickHandler.h"   // for JoystickHandler
#include "OSDLKeyboardHandler.h"   // for KeyboardHandler
#include "OSDLMouseHandler.h"      // for MouseHandler
#include "OSDLMouseCommon.h"       // for DefaultMouse


#include "Ceylan.h"                // for Flags, etc.

#include <list>


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>            // for OSDL_DEBUG and al (private header)
#endif // OSDL_USES_CONFIG_H


using std::string ;

using namespace Ceylan ;
using namespace Ceylan::Log ;

// for time units and calls (ex : Millisecond, basicSleep) :
using namespace Ceylan::System ;   


using namespace OSDL::Events ;
using namespace OSDL::Engine ;

//const Ceylan::Uint32 EventModule::FrameTimingSlots = 200 ;


#if OSDL_DEBUG_EVENTS

#include <iostream>
#define OSDL_EVENT_LOG( message ) std::cout << "[OSDL events] " << message << std::endl ;

#else // OSDL_DEBUG_EVENTS

#define OSDL_EVENT_LOG( message )

#endif // OSDL_DEBUG_EVENTS




bool EventsModule::_EventsInitialized = false ;

// @todo: wrap all SDL constants (ex : scan codes).


// Basic event types :
	
const BasicEventType EventsModule::ApplicationFocusChanged      =
	SDL_ACTIVEEVENT     ;
	
const BasicEventType EventsModule::KeyPressed                   = 
	SDL_KEYDOWN         ;
	
const BasicEventType EventsModule::KeyReleased                  = 
	SDL_KEYUP           ;
	
const BasicEventType EventsModule::MouseMoved                   =
	SDL_MOUSEMOTION     ;

const BasicEventType EventsModule::MouseButtonPressed           =
	SDL_MOUSEBUTTONDOWN ;

const BasicEventType EventsModule::MouseButtonReleased          =
	SDL_MOUSEBUTTONUP   ;

const BasicEventType EventsModule::JoystickAxisChanged          =
	SDL_JOYAXISMOTION   ;

const BasicEventType EventsModule::JoystickTrackballChanged     =
	SDL_JOYBALLMOTION   ;

const BasicEventType EventsModule::JoystickHatPositionChanged   =
	SDL_JOYHATMOTION    ;

const BasicEventType EventsModule::JoystickButtonPressed        =
	SDL_JOYBUTTONDOWN   ;

const BasicEventType EventsModule::JoystickButtonReleased       =
	SDL_JOYBUTTONUP     ;

const BasicEventType EventsModule::UserRequestedQuit            = 
	SDL_QUIT            ;

const BasicEventType EventsModule::SystemSpecificTriggered      = 
	SDL_SYSWMEVENT      ;

const BasicEventType EventsModule::UserResizedVideoMode         =
	SDL_VIDEORESIZE     ;

const BasicEventType EventsModule::ScreenNeedsRedraw            =
	SDL_VIDEOEXPOSE     ;


/*
 * User events can range from FirstUserEventTriggered to 
 * LastUserEventTriggered :
 *
 */

const BasicEventType EventsModule::FirstUserEventTriggered      = 
	SDL_USEREVENT       ;

// No SDL_MAXEVENTS available, using SDL_NUMEVENTS instead. 
const BasicEventType EventsModule::LastUserEventTriggered       = 
	SDL_NUMEVENTS - 1   ;



// User event types :

const UserEventType EventsModule::NoEvent                       = 0 ;
const UserEventType EventsModule::QuitRequested                 = 1 ;
 
 
// Focus flags :
 
const Ceylan::Sint16 EventsModule::_MouseFocus       = SDL_APPMOUSEFOCUS ;
const Ceylan::Sint16 EventsModule::_KeyboardFocus    = SDL_APPINPUTFOCUS ;
const Ceylan::Sint16 EventsModule::_ApplicationFocus = SDL_APPACTIVE ;


const string EventsModule::_MessageHeader = "[OSDL event] " ;
 
 
 
/// See http://sdldoc.csn.ul.ie/sdlenvvars.php
string EventsModule::_SDLEnvironmentVariables[] = 
{
		"SDL_MOUSE_RELATIVE",
		"SDL_MOUSEDEV",
		"SDL_MOUSEDEV_IMPS2",
		"SDL_MOUSEDRV",
		"SDL_NO_RAWKBD",
		"SDL_NOMOUSE"
		"SDL_JOYSTICK_DEVICE",
		"SDL_LINUX_JOYSTICK"
} ;


/// Includes left, middle, right, wheel up, wheel down buttons.
const Ceylan::Uint32 EventsModule::_MouseButtonCount = 5 ;



EventsModule::EventsModule( Flags eventsFlag ) throw( EventsException ) : 
	Ceylan::Module( 
		"OSDL events module",
		"This is the module of OSDL dedicated to events management",
		"http://osdl.sourceforge.net",
		"Olivier Boudeville",
		"olivier.boudeville@online.fr",
		OSDL::GetVersion(),
		"LGPL" ),
	_useScheduler( false ),	
	_keyboardHandler( 0 ),
	_joystickHandler( 0 ),
	_mouseHandler( 0 ),
	_quitRequested( false ),
	_loopTargetedFrequency( DefaultEventLoopTargetedFrequency ),
	_idleCallsCount( 0 ),
	_loopIdleCallback( 0 ),
	_loopIdleCallbackData( 0 )
{

	send( "Initializing events subsystem." ) ;
	
	
	// Activating selected input devices :
	
	if ( eventsFlag & CommonModule::UseJoystick )
	{
		_joystickHandler = new JoystickHandler( 
			/* useClassicalJoysticks */ true ) ;
	}	


	if ( eventsFlag & CommonModule::UseKeyboard )
	{
	
		// Keyboard comes in default state :
		_keyboardHandler = new KeyboardHandler( 
			/* initialMode */ Events::rawInput,
			/* useSmarterDefaultKeyHandler */ false ) ;
	}
		
	
	/*
	 * No 'if ( eventsFlag & CommonModule::UseMouse )...' test, as even if 
	 * the UseMouse flag is not set, we may receive mouse-related events
	 * (motion, focus, etc.), hence we need a handler for that :
	 *
	 */
	_mouseHandler = new MouseHandler( /* useClassicalMice */ true ) ;
		
	
	/* 
	 * Activating UseEventThread (SDL_INIT_EVENTTHREAD) :
	 * Preferring avoiding it on Windows and OSX, not clear under GNU/Linux,
	 * hence preferring no for all platforms, if not explicitly specified.
	 *
	 */
				
	send( "Events subsystem initialized." ) ;
	
	dropIdentifier() ;
	
}	


EventsModule::~EventsModule() throw()
{

	send( "Stopping events subsystem." ) ;
	
	if ( _joystickHandler != 0 )
	{
		delete _joystickHandler ;
		_joystickHandler = 0 ;
	}	
		
	if ( _keyboardHandler != 0 )
	{
		delete _keyboardHandler ;
		_keyboardHandler = 0 ;
	}	
	
	if ( _mouseHandler != 0 )
	{
		delete _mouseHandler ;
		_mouseHandler = 0 ;
	}	
	
	send( "Stopping and deleting any existing scheduler" ) ;
	Scheduler::DeleteScheduler() ;
	
	send( "Events subsystem stopped." ) ;
	
}


void EventsModule::waitForAnyKey( bool displayWaitingMessage ) const throw()
{


	/*
	 * No need to check that events and video are initialized, since we 
	 * have an EventModule here.
	 *
	 */
	
	SDL_Event currentEvent ;

	if ( displayWaitingMessage )
	{
		Ceylan::display( "< Press any key on OSDL window to continue >" ) ;
	}
	
	/*
	 * Simpler than integrating into the keyboard handler 
	 * (and works without it) :
	 *
	 */
	do 
	{
	
		// Waiting, not polling, avoids taking 100% of the CPU for nothing :	
		SDL_WaitEvent( & currentEvent ) ;
		
	} 
	while ( currentEvent.type != KeyPressed ) ;
	
}


bool EventsModule::sleepFor( Second seconds, Microsecond micros ) 
	const throw( EventsException )
{

	/*
	 * getSchedulingGranularity has already been called in the EventsModule
	 * constructor, no start-up overhead to fear.
	 *
	 */
	
	try
	{
	
		// Avoid owerflow for smartSleep :
		while ( seconds > 4100 )
		{
			Ceylan::System::smartSleep( 4100, 0 ) ;
			seconds -= 4100 ;
		}
			
		return Ceylan::System::smartSleep( seconds, micros ) ;
		
	}
	catch( const Ceylan::System::SystemException & e )
	{
		throw EventsException( "EventsModule::sleepFor : " + e.toString() ) ;
	}	
	
}


void EventsModule::useScheduler( bool on ) throw()
{

	if ( on && ! _useScheduler )
	{
		// Force scheduler creation :
		Scheduler::GetScheduler() ;
	}	
	
	if ( ! on && _useScheduler )
	{
		// Force scheduler removal :
		Scheduler::DeleteScheduler() ;	
	}	
	
	_useScheduler = on ;
	
}



void EventsModule::setIdleCallback( Ceylan::System::Callback idleCallback, 
	void * callbackData ) throw()
{

	if ( _useScheduler )
	{
		
		// Will create the scheduler if it is not already existing :
		Scheduler::GetScheduler().setIdleCallback( idleCallback, 
			callbackData ) ;

	}
	else // the basic event loop is used :
	{
		_loopIdleCallback     = idleCallback ;
		_loopIdleCallbackData = callbackData ;
	
	}
	
}


void EventsModule::setEventLoopTargetFrequency( Hertz targetFrequency ) throw()
{

	_loopTargetedFrequency = targetFrequency ;
	
}


void EventsModule::enterMainLoop() throw( EventsException )
{ 

	if ( ! OSDL::CommonModule::IsBackendInitialized() )
		throw EventsException( "EventsModule::enterMainLoop called "
			"whereas back-end not initialized." ) ;
	
		
	// If a scheduler is available, delegate the task to it :
	if ( _useScheduler )
	{
	
		LogPlug::debug( 
			"EventsModule::enterMainLoop : delegating to scheduler." ) ;
		
		try
		{
			// Will create a scheduler if necessary :
			Scheduler::GetScheduler().schedule() ;
		}
		catch( const SchedulingException & e )
		{
			throw EventsException( 
				"EventsModule::enterMainLoop : scheduler stopped on failure : "
				+ e.toString() ) ;
		}	
		LogPlug::debug( "EventsModule::enterMainLoop : scheduler returned." ) ;
	}
	else
	{
		LogPlug::debug( "EventsModule::enterMainLoop : "
			"no scheduler requested, using basic event loop." ) ;
		enterBasicMainLoop() ;	
			
		LogPlug::debug( 
			"EventsModule::enterMainLoop : exiting now from main loop." ) ;
			
	}	

}


void EventsModule::requestQuit() throw()
{

	send( "Quit has been requested, exiting from main event loop." ) ;
	
	_quitRequested = true ;
	
	if ( _useScheduler )
	{
	
		try
		{
			Scheduler::GetExistingScheduler().stop() ;
		}
		catch (	const SchedulingException & e )
		{
			LogPlug::error( "EventsModule::requestQuit : "
				"no scheduler found, none stopped, or stop failed : " 
				+ e.toString() ) ;
		}
		
	}	
	
}



// Handler subsection.


bool EventsModule::hasKeyboardHandler() const throw()
{

	return ( _keyboardHandler != 0 ) ;
	
}



KeyboardHandler & EventsModule::getKeyboardHandler() const 
	throw( EventsException )
{

	if ( _keyboardHandler == 0 )
		throw EventsException( 
			"EventsModule::getKeyboardHandler : no handler available : "
			"did you specify 'CommonModule::UseKeyboard' "
			"at the initialization of the common module ?" ) ;
	
	return * _keyboardHandler ;
	
}


void EventsModule::setKeyboardHandler( KeyboardHandler & newHandler ) throw()
{

	if ( _keyboardHandler != 0 )
		delete _keyboardHandler ;
		
	_keyboardHandler = & newHandler ;
	
}



bool EventsModule::hasJoystickHandler() const throw()
{

	return ( _joystickHandler != 0 ) ;
	
}


JoystickHandler & EventsModule::getJoystickHandler() const 
	throw( EventsException )
{

	if ( _joystickHandler == 0 )
		throw EventsException( 
			"EventsModule::getJoystickHandler : no handler available : "
			"did you specify 'CommonModule::UseJoystick' "
			"at the initialization of the common module ?" ) ;
	
	return * _joystickHandler ;
	
}


void EventsModule::setJoystickHandler( JoystickHandler & newHandler ) throw()
{

	if ( _joystickHandler != 0 )
		delete _joystickHandler ;
		
	_joystickHandler = & newHandler ;
	
}



bool EventsModule::hasMouseHandler() const throw()
{

	return ( _mouseHandler != 0 ) ;
	
}


MouseHandler & EventsModule::getMouseHandler() const 
	throw( EventsException )
{

	if ( _mouseHandler == 0 )
		throw EventsException( 
			"EventsModule::getMouseHandler : no handler available : "
			"did you specify 'CommonModule::UseMouse' "
			"at the initialization of the common module ?" ) ;
	
	return * _mouseHandler ;
	
}


void EventsModule::setMouseHandler( MouseHandler & newHandler ) throw()
{

	if ( _mouseHandler != 0 )
		delete _mouseHandler ;
		
	_mouseHandler = & newHandler ;
	
}




void EventsModule::updateInputState() throw()
{
		
	BasicEvent currentEvent ;
	
	
	// Checks for all pending events :

	while ( SDL_PollEvent( & currentEvent ) )
	{
	
		switch ( currentEvent.type )
		{


			// Focus section.
			case ApplicationFocusChanged:
				onApplicationFocusChanged( currentEvent.active ) ;
				break ;
					
			// Keyboard section.			
			case KeyPressed:
				onKeyPressed( currentEvent.key ) ;
				break ;
				
			case KeyReleased:
				onKeyReleased( currentEvent.key ) ;
				break ;
			
			// Mouse section.
			case MouseMoved:
				onMouseMotion( currentEvent.motion ) ;
				break ;
				
			case MouseButtonPressed:
				onMouseButtonPressed( currentEvent.button ) ; 
				break ;
				
			case MouseButtonReleased:			
				onMouseButtonReleased( currentEvent.button ) ;
				break ;
					
						
			/* 
			 * Joystick section.
			 *
			 * Joystick low level events should occur if and only if a 
			 * joystick handler is used, therefore there should be no need 
			 * for a handler check. 
			 *
			 */
			case JoystickAxisChanged:
				onJoystickAxisChanged( currentEvent.jaxis ) ;
				break ;
				
			case JoystickTrackballChanged:
				onJoystickTrackballChanged( currentEvent.jball ) ;
				break ;
				
			case JoystickHatPositionChanged:
				onJoystickHatChanged( currentEvent.jhat ) ;
				break ;
				
			case JoystickButtonPressed:
				onJoystickButtonPressed( currentEvent.jbutton ) ;
				break ;
				
			case JoystickButtonReleased:
				onJoystickButtonReleased( currentEvent.jbutton ) ;
				break ;			
			
			
			// Miscellaneous section.
			
			case UserRequestedQuit:
				onQuitRequested() ;
				break ;
			
			case SystemSpecificTriggered: 
				onSystemSpecificWindowManagerEvent( currentEvent.syswm ) ;
				break ;
			
			case UserResizedVideoMode:
				onResizedWindow( currentEvent.resize ) ;
				break ;
				
			case ScreenNeedsRedraw:
				onScreenNeedsRedraw() ;	
				break ;
				
			// User event and unknown section.
			default:
				if ( currentEvent.type >= FirstUserEventTriggered 
						|| currentEvent.type <= LastUserEventTriggered )
					onUserEvent( currentEvent.user ) ;
				else
					onUnknownEventType( currentEvent ) ;
				break ;
				
		}	
	}
}


const string EventsModule::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{
	
	string res = "Event module, " ;
	
	if ( _useScheduler )
		res += "using a scheduler, " ;
	else
		res += "not using any scheduler, " ;
		
	if ( _keyboardHandler != 0 )
		res += "using a keyboard handler, " ;
	else
		res += "not using any keyboard handler, " ;
	
	if ( _joystickHandler != 0 )
		res += "using a joystick handler, " ;
	else
		res += "not using any joystick handler, " ;
	
	if ( _mouseHandler != 0 )
		res += "using a mouse handler, " ;
	else
		res += "not using any mouse handler, " ;
	
	
	if ( _loopIdleCallback == 0 )
		res += "using micro-sleep idle callback" ;
	else
		res += "using user-specified idle callback" ;
					
	if ( level == Ceylan::low )
		return res ;
	
	std::list<string> returned ;
	
	returned.push_back( res ) ;
					
	returned.push_back( Ceylan::Module::toString() ) ;
	
	if ( level == Ceylan::medium)
		return Ceylan::formatStringList( returned ) ;
		
	if ( _keyboardHandler != 0 )
		returned.push_back( _keyboardHandler->toString() ) ;
	
	if ( _joystickHandler != 0 )
		returned.push_back( _joystickHandler->toString() ) ;
	
	if ( _mouseHandler != 0 )
		returned.push_back( _mouseHandler->toString() ) ;
	
	return Ceylan::formatStringList( returned ) ;
	
}



// Static section.		

			
string EventsModule::DescribeEnvironmentVariables() throw()
{

	Ceylan::Uint16 varCount = 
		sizeof( _SDLEnvironmentVariables ) / sizeof (char * ) ;
		
	string result = "Examining the " + Ceylan::toString( varCount )
		+ " events-related environment variables for SDL backend :" ;
	
	std::list<string> variables ;
		
	string var, value ;
	
	bool htmlFormat = TextDisplayable::GetOutputFormat() ;
	
	for ( Ceylan::Uint16 i = 0; i < varCount; i++ ) 
	{
	
		var = _SDLEnvironmentVariables[ i ] ;
		value = Ceylan::System::getEnvironmentVariable( var ) ;
		
		if ( value.empty() )
		{
			if ( htmlFormat )
			{
				variables.push_back( "<em>" + var + "</em> is not set." ) ;
			}
			else
			{
				variables.push_back( var + " is not set." ) ;			
			}	
		}
		else
		{			
			if ( htmlFormat )
			{
				variables.push_back( "<b>" + var + "</b> set to [" 
					+ value + "]." ) ;
			}
			else
			{
				variables.push_back( var + " set to [" + value + "]." ) ;
			}	
		}	
	
	}
	
	return result + Ceylan::formatStringList( variables ) ;
	
}



string EventsModule::DescribeEvent( BasicEvent anEvent ) throw() 
{

	switch( anEvent.type )
	{
	
		case ApplicationFocusChanged:
			return DescribeEvent( anEvent.active ) ;
			break ;
			
		case KeyPressed:
			return DescribeEvent( anEvent.key ) ;			
			break ;
	
		case KeyReleased:
			return DescribeEvent( anEvent.key ) ;			
			break ;
	
		case MouseMoved:
			return DescribeEvent( anEvent.motion ) ;			
			break ;
	
		case MouseButtonPressed:
			return DescribeEvent( anEvent.button ) ;			
			break ;
	
		case MouseButtonReleased:
			return DescribeEvent( anEvent.button ) ;			
			break ;
	
		case JoystickAxisChanged:
			return DescribeEvent( anEvent.jaxis ) ;			
			break ;
	
		case JoystickTrackballChanged:
			return DescribeEvent( anEvent.jball ) ;			
			break ;
	
		case JoystickHatPositionChanged:
			return DescribeEvent( anEvent.jhat ) ;			
			break ;
	
		case JoystickButtonPressed:
			return DescribeEvent( anEvent.jbutton ) ;			
			break ;
	
		case JoystickButtonReleased:
			return DescribeEvent( anEvent.jbutton ) ;			
			break ;
	
		case UserRequestedQuit:
			return DescribeEvent( anEvent.quit ) ;			
			break ;
	
		case SystemSpecificTriggered:
			return DescribeEvent( anEvent.syswm ) ;			
			break ;
	
		case UserResizedVideoMode:
			return DescribeEvent( anEvent.resize ) ;			
			break ;
		
		case ScreenNeedsRedraw:
			return DescribeEvent( anEvent.expose ) ;			
			break ;
	
		default:
			return "Unknow event type." ;
			break ; 
	}
	 
	return "Unexpected event type." ;
}	



Millisecond EventsModule::GetMillisecondsSinceStartup() 
	throw( EventsException ) 
{

	// Do not slow down too much on release mode :

#if OSDL_DEBUG
	if ( ! OSDL::CommonModule::IsBackendInitialized() )
		throw EventsException( 
			"EventsModule::getMillisecondsSinceStartup() called "
			"whereas back-end not initialized." ) ;
#endif // OSDL_DEBUG	
	
	return static_cast<Millisecond>( SDL_GetTicks() ) ;
	
}




// Protected section.



void EventsModule::enterBasicMainLoop() throw( EventsException )
{

	
	//Milliseconds * frameClock = 0 ;	
	Ceylan::Uint32 frameCount = 0 ;
	
	/*
	if ( GetFrameAccountingState() )
	{	
		frameClock = new Milliseconds[FrameTimingSlots] ;	
		for ( Ceylan::Uint32 i; i < FrameTimingSlots; i ++ )
			frameClock[i] = 0 ;
	}
	*/	
	
	/*
	 * This event loop will drive the rendering, by using the renderer 
	 * if available, otherwise by using the video module.
	 *
	 */
	 
	Video::VideoModule * video ;
	 
	try
	{
		video = & OSDL::getExistingCommonModule().getVideoModule() ;
	} 
	catch ( const OSDL::Exception & e )
	{	
		throw EventsException( 
			"EventsModule::enterBasicMainLoop : no video module available ("
			+ e.toString() ) ;
	}
	
	Rendering::Renderer * renderer = 0 ;
	
	if ( Rendering::Renderer::HasExistingRootRenderer() )
	{
		renderer = & Rendering::Renderer::GetExistingRootRenderer() ;
		LogPlug::debug( 
			"EventsModule::enterBasicMainLoop : using root renderer ("
			+ renderer->toString() + ")" ) ;
	}
	else
	{
		LogPlug::debug( "EventsModule::enterBasicMainLoop : "
			"not using any root renderer." ) ;	
	}
		
	
	/*
	 * No scheduler, using classical event loop.
	 *
	 * We believe we have a better accuracy than 'SDL_framerate' from 
	 * SDL_gfx with that mechanism :
	 *
	 */

	_idleCallsCount = 0 ;
	
	Microsecond startedMicrosec ;
	Second startedSec ;

	Microsecond lastMicrosec ;
	Second lastSec ;
	
	Microsecond nowMicrosec ;
	Second nowSec ;

	
	try
	{
	
		if ( _loopIdleCallback == 0 )
		{
		
			/*
			 * Force the scheduling granularity to be precomputed, to avoid
			 * hiccups at loop start-up when needing granularity to adjust
			 * delays :
			 *
			 */
			getSchedulingGranularity() ;
			
		}
	
	
		// Compute the event loop period :
		Microsecond	loopExpectedDuration 
			= static_cast<Microsecond>( 1000000.0f / _loopTargetedFrequency ) ; 
		
	
		getPreciseTime( lastSec, lastMicrosec ) ;
		
		startedMicrosec = lastMicrosec ;
		startedSec = lastSec ;
		
		
	    while ( ! _quitRequested )
		{
		
			// Checks for all pending events :
			updateInputState() ;
			
			/*
			 * With a basic main loop, objets are expected to be event-driven,
			 * hence updating the inputs should lead to updating their models,
			 * then their views, so they should be now ready to be rendered.
			 *
			 */
			if ( renderer != 0 )
				renderer->render() ;
			else
				video->redraw() ;
		
			frameCount++ ; 	
		
			getPreciseTime( nowSec, nowMicrosec ) ;
		
			while ( getDurationBetween( lastSec, lastMicrosec, nowSec,
				nowMicrosec ) < loopExpectedDuration )
			{
				onIdle() ;
				getPreciseTime( nowSec, nowMicrosec ) ;			
			}
			
			// Ready for next iteration
		
			lastSec = nowSec ;
			lastMicrosec = nowMicrosec ;
		
		}
	
	} 
	catch ( const SystemException & e )
	{	
		throw EventsException( "EventsModule::enterBasicMainLoop : " 
			+ e.toString() ) ;
	}
	
	if ( ( lastSec - startedSec ) > 4100 )
	{
	
		// Avoid overflow of getDurationBetween : 
		LogPlug::debug( "Exited from main loop after " 
			+ Ceylan::toString( frameCount ) 
			+ " frames, an average of " 
			+ Ceylan::toString( 
				static_cast<Ceylan::Float64>( _idleCallsCount ) / frameCount, 
				/* precision */ 3 )
			+ " idle calls per frame have been performed." ) ;
	}
	else
	{
		LogPlug::debug( "Exited from main loop after " 
			+ Ceylan::toString( frameCount ) 
			+ " frames, on average there were " 
			+ Ceylan::toString( 
				100.0f * 1000000 /* since microseconds */ * frameCount / 
					getDurationBetween( startedSec, startedMicrosec, lastSec,
						lastMicrosec ),
				/* precision */ 3 )
			+ " frames per second, an average of " 
			+ Ceylan::toString( 
				static_cast<Ceylan::Float64>( _idleCallsCount ) / frameCount, 
				/* precision */ 3 )
			+ " idle calls per frame have been performed." ) ;
	}		

}




// Keyboard section.


void EventsModule::onKeyboardFocusGained(
	const FocusEvent & keyboardFocusEvent ) throw()
{

	OSDL_EVENT_LOG( "Application gained keyboard focus." ) ;

#if OSDL_DEBUG
	if ( _keyboardHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onKeyboardFocusGained called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_keyboardHandler->focusGained( keyboardFocusEvent ) ;

}


void EventsModule::onKeyboardFocusLost(
	const FocusEvent & keyboardFocusEvent ) throw()
{

	OSDL_EVENT_LOG( "Application lost keyboard focus." ) ;

#if OSDL_DEBUG
	if ( _keyboardHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onKeyboardFocusLost called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_keyboardHandler->focusLost( keyboardFocusEvent ) ;

}


void EventsModule::onKeyPressed( const KeyboardEvent & keyboardEvent ) throw()
{

	OSDL_EVENT_LOG( "Key pressed." ) ;

#if OSDL_DEBUG
	if ( _keyboardHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onKeyPressed called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_keyboardHandler->keyPressed( keyboardEvent ) ;
	
}


void EventsModule::onKeyReleased( const KeyboardEvent & keyboardEvent ) throw()
{

	OSDL_EVENT_LOG( "Key released." ) ;

#if OSDL_DEBUG
	if ( _keyboardHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onKeyReleased called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_keyboardHandler->keyReleased( keyboardEvent ) ;
	
}



// Mouse section.


void EventsModule::onMouseFocusGained( 
	const FocusEvent & mouseFocusEvent ) throw()
{

	OSDL_EVENT_LOG( "Application gained mouse focus." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseFocusGained called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->focusGained( mouseFocusEvent ) ;
	
}


void EventsModule::onMouseFocusLost( 
	const FocusEvent & mouseFocusEvent ) throw()
{

	OSDL_EVENT_LOG( "Application lost mouse focus." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseFocusLost called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->focusLost( mouseFocusEvent ) ;

}


void EventsModule::onMouseMotion( const MouseMotionEvent & mouseEvent ) throw()
{

	OSDL_EVENT_LOG( "Mouse motion." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseMotion called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->mouseMoved( mouseEvent ) ;
	
}


void EventsModule::onMouseButtonPressed( const MouseButtonEvent & mouseEvent )
	throw()
{

	OSDL_EVENT_LOG( "Mouse button pressed." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseButtonPressed called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->buttonPressed( mouseEvent ) ;
	
}


void EventsModule::onMouseButtonReleased( const MouseButtonEvent & mouseEvent )
	throw()
{

	OSDL_EVENT_LOG( "Mouse button released." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseButtonReleased called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->buttonReleased( mouseEvent ) ;
	
}



// Joystick section.


void EventsModule::onJoystickAxisChanged( 
	const JoystickAxisEvent & joystickEvent ) throw()
{

	OSDL_EVENT_LOG( "Joystick axis moved." ) ;
	
#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onJoystickAxisChanged called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG
	
	_joystickHandler->axisChanged( joystickEvent ) ;
	
}


void EventsModule::onJoystickTrackballChanged( 
	const JoystickTrackballEvent & joystickEvent ) throw()
{

	OSDL_EVENT_LOG( "Joystick trackball moved." ) ;

#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( 
			"EventsModule::onJoystickTrackballChanged called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG
	
	_joystickHandler->trackballChanged( joystickEvent ) ;

}


void EventsModule::onJoystickHatChanged( 
	const JoystickHatEvent & joystickEvent ) throw()
{

	OSDL_EVENT_LOG( "Joystick hat moved." ) ;

#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onJoystickHatChanged called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG
	
	_joystickHandler->hatChanged( joystickEvent ) ;
	
}


void EventsModule::onJoystickButtonPressed( 
	const JoystickButtonEvent & joystickEvent ) throw()
{

	OSDL_EVENT_LOG( "Joystick button pressed." ) ;

#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( 
			"EventsModule::onJoystickButtonPressed called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG
	
	_joystickHandler->buttonPressed( joystickEvent ) ;

}


void EventsModule::onJoystickButtonReleased( 
	const JoystickButtonEvent & joystickEvent ) throw()
{

	OSDL_EVENT_LOG( "Joystick button released." ) ;

#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( 
			"EventsModule::onJoystickButtonReleased called "
			"whereas no handler is available." ) ;
#endif
	
	_joystickHandler->buttonReleased( joystickEvent ) ;

}



// Application-generic section.

		
void EventsModule::onIdle() throw() 
{		

	_idleCallsCount++ ;
	
	if ( _loopIdleCallback != 0 )
	{
		OSDL_EVENT_LOG( 
			"EventsModule::onIdle : calling now idle call-back." ) ;
		
		(*_loopIdleCallback)( _loopIdleCallbackData ) ;
		
		OSDL_EVENT_LOG( 
			"EventsModule::onIdle : returned from idle call-back." ) ;
		
	}
	else
	{
	
		/*
		 * Issues a basic sleep, chosen so that the minimum real sleeping 
		 * time can be performed.
		 *
		 */
		 basicSleep() ;
		
	}	
		
}


void EventsModule::onApplicationFocusChanged( 
	const FocusEvent & focusEvent ) throw()
{

	OSDL_EVENT_LOG( "Application focus changed." ) ;
	
	// Maybe multiple gains or losses could be sent in one event :

	if ( focusEvent.state & _MouseFocus )
	{
		
		// DefaultMouse : only one mouse supported for the moment.
		
		if ( focusEvent.gain == 1 )
			onMouseFocusGained( focusEvent ) ;
		else
			onMouseFocusLost( focusEvent ) ;	
	}
	
	if ( focusEvent.state & _KeyboardFocus )
	{
		if ( focusEvent.gain == 1 )
			onKeyboardFocusGained( focusEvent ) ;
		else
			onKeyboardFocusLost( focusEvent ) ;	
	}
	
	if ( focusEvent.state & _ApplicationFocus )
	{
		if ( focusEvent.gain == 1 )
			onApplicationRestored( focusEvent ) ;
		else
			onApplicationIconified( focusEvent ) ;	
	}
		
}


void EventsModule::onApplicationIconified( 
	const FocusEvent & focusEvent ) throw()
{

	OSDL_EVENT_LOG( "Application is iconified (no handler registered) : "
		+ DescribeEvent( focusEvent ) ) ;
	
}


void EventsModule::onApplicationRestored( 
	const FocusEvent & focusEvent ) throw()
{

	OSDL_EVENT_LOG( "Application is restored (no handler registered) : "
		+ DescribeEvent( focusEvent ) ) ;
	
}


void EventsModule::onQuitRequested() throw()
{

	OSDL_EVENT_LOG( "Application is requested to stop." ) ;

	requestQuit() ;
	
}


void EventsModule::onSystemSpecificWindowManagerEvent( 
	const SystemSpecificWindowManagerEvent & wmEvent ) throw()
{

	/*
	 * @see http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSysWMEvent to 
	 * obtain system-specific information about the window manager, 
	 * with SDL_GetWMInfo.
	 *
	 */
	OSDL_EVENT_LOG( "System specific window manager event received "
		"(no handler registered)." ) ;	 

}
	
					
void EventsModule::onResizedWindow( const WindowResizedEvent & resizeEvent )
	throw()
{

	OSDL_EVENT_LOG( "Resizing, new width is " << resizeEvent.w 
		<< ", new height is " << resizeEvent.h << "." ) ;
		
	try
	{	
		OSDL::getExistingCommonModule().getVideoModule().resize( 
			resizeEvent.w, resizeEvent.h ) ;
	} 
	catch( const Video::VideoException & e )
	{
		// Warn but continue nevertheless :
		LogPlug::error( 
			"EventsModule::onResizedWindow : error when resizing to "
			+ Ceylan::toString( resizeEvent.w ) + ", " 
			+ Ceylan::toString( resizeEvent.h ) + ") : "
			+ e.toString() ) ;
	}
	
}


void EventsModule::onScreenNeedsRedraw() throw()
{

	OSDL_EVENT_LOG( "Screen needs redraw." ) ;

	try
	{	
		OSDL::getExistingCommonModule().getVideoModule().redraw() ;
	} 
	catch( const Video::VideoException & e )
	{
		// Warn but continue nevertheless.
		LogPlug::error( "EventsModule::onScreenNeedsRedraw : "
			"error when requesting a redraw : "	+ e.toString() ) ;
	}

}


void EventsModule::onUserEvent( const UserEvent & userEvent ) throw() 
{

	OSDL_EVENT_LOG( "User event received, whose type is " << userEvent.type
		<< " (no handler registered)." ) ;
		
}
		
					
void EventsModule::onUnknownEventType( const BasicEvent & unknownEvent ) throw()
{

	OSDL_EVENT_LOG( "Unknown event, type is " << unknownEvent.type 
		<< " (no handler registered)." ) ;
	
}



// Static section :

string EventsModule::DescribeEvent( const FocusEvent & focusEvent )
{
	
	string res = "Focus changed" ;
	
	if ( focusEvent.state & _MouseFocus )
	{
		if ( focusEvent.gain == 1 )
			res += ", mouse gained focus" ;
		else
			res += ", mouse lost focus" ;
	}
	
	if ( focusEvent.state & _KeyboardFocus )
	{
		if ( focusEvent.gain == 1 )
			res += ", keyboard gained focus" ;
		else
			res += ", keyboard lost focus" ;
	}
	
	if ( focusEvent.state & _ApplicationFocus )
	{
		if ( focusEvent.gain == 1 )
			res += ", application restored." ;
		else
			res += ", application iconified." ;
	}
	
	return res ;
	
}


string EventsModule::DescribeEvent( const KeyboardEvent & keyboardEvent )
{
	
	string res = "Key " + KeyboardHandler::DescribeKey( 
		static_cast<KeyboardHandler::KeyIdentifier>( 
			keyboardEvent.keysym.sym ) ) 
		+ ", with modifier(s) : " 
		+ KeyboardHandler::DescribeModifier( 
			static_cast<KeyboardHandler::KeyModifier>( 
				keyboardEvent.keysym.mod ) )
		+ " (scancode :  " 
			+ Ceylan::toNumericalString( keyboardEvent.keysym.scancode ) 
			+ ") was " ;
	
	if ( keyboardEvent.type == KeyPressed )
		res += "pressed" ;
	else
		res += "released" ;

	if ( KeyboardHandler::GetMode() == textInput )
		res += ". " + KeyboardHandler::DescribeUnicode(
			keyboardEvent.keysym.unicode ) ;
		
	return res + "." ;
	
}


string EventsModule::DescribeEvent( const MouseMotionEvent & mouseMotionEvent )
{

	string res = "Mouse moved to (" + Ceylan::toString( mouseMotionEvent.x ) 
		+ ";"
		+ Ceylan::toString( mouseMotionEvent.y ) 
		+ ") after a relative movement of ("
		+ Ceylan::toString( mouseMotionEvent.xrel ) + ";" 
		+ Ceylan::toString( mouseMotionEvent.yrel ) + ")." ;
	
	for ( Ceylan::Uint8 i = 1; i <= _MouseButtonCount; i++ )
		if ( mouseMotionEvent.state & ( SDL_BUTTON( i ) ) )
			res += " Button #" + Ceylan::toNumericalString( i ) 
				+ " is pressed." ;
			
	return res ;
	
}


string EventsModule::DescribeEvent( const MouseButtonEvent & mouseButtonEvent )
{

	string res ;
	
	switch( mouseButtonEvent.button )
	{
	
		case SDL_BUTTON_LEFT:
			res = "Left mouse button " 
				+ ( mouseButtonEvent.state == SDL_PRESSED ) ? 
					"pressed" : "released" ;
			break ;
			
		case SDL_BUTTON_MIDDLE:
			res = "Middle mouse button " 
				+ ( mouseButtonEvent.state == SDL_PRESSED ) ? 
					"pressed" : "released" ;
			break ;
			
		case SDL_BUTTON_RIGHT:
			res = "Right mouse button " 
				+ ( mouseButtonEvent.state == SDL_PRESSED ) ? 
					"pressed" : "released" ;
			break ;
			
		case SDL_BUTTON_WHEELUP:
			res = "Mouse wheel up" ;
			break ;
			
		case SDL_BUTTON_WHEELDOWN:
			res = "Mouse wheel down" ;
			break ;
		
		default:
			res = "Unexpected mouse event" ;
			break ;	
	
	}
	
	
	res += " while mouse was located at (" 
		+ Ceylan::toString( mouseButtonEvent.x ) + ";" 
		+ Ceylan::toString( mouseButtonEvent.y ) + ")." ;
					
	return res ;

}


string EventsModule::DescribeEvent( const JoystickAxisEvent & axisEvent )
{

	return "Joystick #" + Ceylan::toNumericalString( axisEvent.which ) 
		+ " had axis #" + Ceylan::toNumericalString( axisEvent.axis ) 
		+ " moved to " 	+ Ceylan::toString( axisEvent.value ) + "." ; 
		
}


string EventsModule::DescribeEvent( const JoystickTrackballEvent & ballEvent )
{

	return "Joystick #" + Ceylan::toNumericalString( ballEvent.which ) 
		+ " had trackball #" + Ceylan::toNumericalString( ballEvent.ball ) 
		+ " moved of (" 
		+ Ceylan::toString( ballEvent.xrel ) + ";" 
		+ Ceylan::toString( ballEvent.yrel ) + ")." ; 

}


string EventsModule::DescribeEvent( const JoystickHatEvent & hatEvent )
{

	return "Joystick #" + Ceylan::toString( hatEvent.which ) + " had hat #"
		+ Ceylan::toNumericalString( hatEvent.hat ) + " set to position " 
		+ Ceylan::toString( hatEvent.value ) + "." ; 

}


string EventsModule::DescribeEvent( const JoystickButtonEvent & buttonEvent )
{

	return "Joystick #" + Ceylan::toNumericalString( buttonEvent.which ) 
		+ " had button #" + Ceylan::toNumericalString( buttonEvent.button ) 
		+ ( ( buttonEvent.state == SDL_PRESSED ) ? 
			" pressed." : " released." ) ;
		 
}


string EventsModule::DescribeEvent( const UserRequestedQuitEvent & quitEvent )  
{

	return "User requested to quit." ;
	
}


string EventsModule::DescribeEvent( 
	const SystemSpecificWindowManagerEvent & windowManagerEvent ) 
{

	/* 
	 * @todo add int SDL_GetWMInfo(SDL_SysWMinfo *info); 
	 * (see http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSysWMEvent)
	 *
	 */
	 
	return "System-specific window manager event occured." ;
	
}


string EventsModule::DescribeEvent( const WindowResizedEvent & resizeEvent )
{

	return "Window resized to (" + Ceylan::toString( resizeEvent.w )
		+ ";" + Ceylan::toString( resizeEvent.h ) + ")." ;
		
}


string EventsModule::DescribeEvent( const ScreenExposedEvent & redrawEvent )
{
	return "Window exposed, needs to be redrawn." ;
}


string EventsModule::DescribeEvent( const UserEvent & userEvent ) 
{
	return "User-defined event occured (code #" 
		+ Ceylan::toString( userEvent.code ) + ")." ;
}

 
bool EventsModule::IsEventsInitialized() throw()
{

	return _EventsInitialized ;
	
}


#include "OSDLEvents.h"

#include "OSDLBasic.h"               // for OSDL::GetVersion
#include "OSDLVideo.h"               // for resize
#include "OSDLScheduler.h"           // for GetScheduler
#include "OSDLRenderer.h"            // for Renderer
#include "OSDLJoystickHandler.h"     // for JoystickHandler
#include "OSDLKeyboardHandler.h"     // for KeyboardHandler
#include "OSDLMouseHandler.h"        // for MouseHandler
#include "OSDLMouseCommon.h"         // for DefaultMouse


#include "Ceylan.h"                  // for Flags, etc.

#include <list>


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>              // for OSDL_DEBUG and al (private header)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL

#include "SDL.h"                     // for SDL_ACTIVEEVENT, etc.

#endif // OSDL_USES_SDL


using std::string ;

using namespace Ceylan ;
using namespace Ceylan::Log ;

// for time units and calls (ex: Millisecond, basicSleep):
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


const std::string NoSDLSupportAvailable = "(no SDL support available)" ;


bool EventsModule::_EventsInitialized = false ;




#if OSDL_USES_SDL

// Basic event types:
	
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

const BasicEventType EventsModule::FirstUserEventTriggered      = 
	SDL_USEREVENT       ;

// No SDL_MAXEVENTS available, using SDL_NUMEVENTS instead. 
const BasicEventType EventsModule::LastUserEventTriggered       = 
	SDL_NUMEVENTS - 1   ;

// Focus flags:
 
const Ceylan::Sint16 EventsModule::_MouseFocus       = SDL_APPMOUSEFOCUS ;
const Ceylan::Sint16 EventsModule::_KeyboardFocus    = SDL_APPINPUTFOCUS ;
const Ceylan::Sint16 EventsModule::_ApplicationFocus = SDL_APPACTIVE ;


/// See http://sdldoc.csn.ul.ie/sdlenvvars.php
const string EventsModule::_SDLEnvironmentVariables[] = 
{
		"SDL_MOUSE_RELATIVE",
		"SDL_MOUSEDEV",
		"SDL_MOUSEDEV_IMPS2",
		"SDL_MOUSEDRV",
		"SDL_NO_RAWKBD",
		"SDL_NOMOUSE",
		"SDL_JOYSTICK_DEVICE",
		"SDL_LINUX_JOYSTICK"
} ;




#else // OSDL_USES_SDL



// Matches SDL defines:

const BasicEventType EventsModule::ApplicationFocusChanged      =  1 ;
const BasicEventType EventsModule::KeyPressed                   =  2 ;
const BasicEventType EventsModule::KeyReleased                  =  3 ;
const BasicEventType EventsModule::MouseMoved                   =  4 ;
const BasicEventType EventsModule::MouseButtonPressed           =  5 ;
const BasicEventType EventsModule::MouseButtonReleased          =  6 ;
const BasicEventType EventsModule::JoystickAxisChanged          =  7 ;
const BasicEventType EventsModule::JoystickTrackballChanged     =  8 ;
const BasicEventType EventsModule::JoystickHatPositionChanged   =  9 ;
const BasicEventType EventsModule::JoystickButtonPressed        = 10 ;
const BasicEventType EventsModule::JoystickButtonReleased       = 11 ;
const BasicEventType EventsModule::UserRequestedQuit            = 12 ;
const BasicEventType EventsModule::SystemSpecificTriggered      = 13 ;
// 14 reserved.
// 15 reserved.
const BasicEventType EventsModule::UserResizedVideoMode         = 16 ;
const BasicEventType EventsModule::ScreenNeedsRedraw            = 17 ;

const BasicEventType EventsModule::FirstUserEventTriggered      = 24 ;

// No SDL_MAXEVENTS available, using SDL_NUMEVENTS instead. 
const BasicEventType EventsModule::LastUserEventTriggered       = 31 ;

// Focus flags:
 
const Ceylan::Sint16 EventsModule::_MouseFocus        = 0x01 ;
const Ceylan::Sint16 EventsModule::_KeyboardFocus     = 0x02 ;
const Ceylan::Sint16 EventsModule::_ApplicationFocus  = 0x04 ;

/// See http://sdldoc.csn.ul.ie/sdlenvvars.php
const string EventsModule::_SDLEnvironmentVariables[] = {} ;


#endif // OSDL_USES_SDL



/*
 * User events can range from FirstUserEventTriggered to 
 * LastUserEventTriggered:
 *
 */



// User event types:

const UserEventType EventsModule::NoEvent       = 0 ;
const UserEventType EventsModule::QuitRequested = 1 ;
 
 


const string EventsModule::_MessageHeader = "[OSDL event] " ;
 
 
 


/// Includes left, middle, right, wheel up, wheel down buttons.
const Ceylan::Uint32 EventsModule::_MouseButtonCount = 5 ;



EventsModule::EventsModule( Flags eventsFlag ) throw( EventsException ): 
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
	
	
	// Activating selected input devices:
	
	if ( eventsFlag & CommonModule::UseJoystick )
	{
		_joystickHandler = new JoystickHandler( 
			/* useClassicalJoysticks */ true ) ;
	}	

	
	/*
	 * No 'if ( eventsFlag & CommonModule::UseKeyboard )...' test, as even if 
	 * the UseMouse flag is not set, we may receive mouse-related events
	 * (motion, focus, etc.), hence we need a handler for that:
	 *
	 */
	
	// Keyboard comes in default state:
	_keyboardHandler = new KeyboardHandler( /* initialMode */ Events::rawInput,
		/* useSmarterDefaultKeyHandler */ false ) ;
		
	
	/*
	 * No 'if ( eventsFlag & CommonModule::UseMouse )...' test, as even if 
	 * the UseMouse flag is not set, we may receive mouse-related events
	 * (motion, focus, etc.), hence we need a handler for that:
	 *
	 */
	_mouseHandler = new MouseHandler( /* useClassicalMice */ true ) ;
		
	
	/* 
	 * Activating UseEventThread (SDL_INIT_EVENTTHREAD):
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



void EventsModule::waitForAnyKey( bool displayWaitingMessage ) const 
	throw( EventsException )
{

#if OSDL_USES_SDL

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
	 * (and works without it):
	 *
	 */
	do 
	{
	
		// Waiting, not polling, avoids taking 100% of the CPU for nothing:	
		SDL_WaitEvent( & currentEvent ) ;
		
	} 
	while ( currentEvent.type != KeyPressed ) ;
	
#else // OSDL_USES_SDL


	// Reusing Ceylan counterpart:
	
	try
	{
	
		if ( displayWaitingMessage )
			Ceylan::waitForKey( "< Press any key to continue >" ) ;
		else	
			Ceylan::waitForKey( "" ) ;
	
	}
	catch( const UtilsException & e )
	{
	
		throw EventsException( "EventsModule::waitForAnyKey failed: "
			+ e.toString() ) ;	

	}
	
	
#endif // OSDL_USES_SDL

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
	
		// Avoid owerflow for smartSleep:
		while ( seconds > 4100 )
		{
			Ceylan::System::smartSleep( 4100, 0 ) ;
			seconds -= 4100 ;
		}
			
		return Ceylan::System::smartSleep( seconds, micros ) ;
		
	}
	catch( const Ceylan::System::SystemException & e )
	{
		throw EventsException( "EventsModule::sleepFor failed: " 
			+ e.toString() ) ;
	}	
	
}



void EventsModule::useScheduler( bool on ) throw()
{

	if ( on && ! _useScheduler )
	{
		// Force scheduler creation:
		Scheduler::GetScheduler() ;
	}	
	
	if ( ! on && _useScheduler )
	{
		// Force scheduler removal:
		Scheduler::DeleteScheduler() ;	
	}	
	
	_useScheduler = on ;
	
}



void EventsModule::setIdleCallback( 
		Ceylan::System::Callback idleCallback, 
		void * callbackData, 
		Ceylan::System::Microsecond callbackExpectedMaxDuration ) 
	throw()
{
	
	
	if ( _useScheduler )
	{
		
		// Will create the scheduler if it is not already existing:
		Scheduler::GetScheduler().setIdleCallback( idleCallback, 
			callbackData, callbackExpectedMaxDuration ) ;

	}
	else // the basic event loop is used:
	{
	
		_loopIdleCallback            = idleCallback ;
		_loopIdleCallbackData        = callbackData ;

		if ( callbackExpectedMaxDuration != 0 )
			_loopIdleCallbackMaxDuration = callbackExpectedMaxDuration ;
		else
			_loopIdleCallbackMaxDuration = EvaluateCallbackduration(
				idleCallback, callbackData ) ;
						
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
	
		
	// If a scheduler is available, delegate the task to it:
	if ( _useScheduler )
	{
	
		LogPlug::debug( 
			"EventsModule::enterMainLoop: delegating to scheduler." ) ;
		
		try
		{
		
			// Will create a scheduler if necessary:
			Scheduler::GetScheduler().schedule() ;
			
		}
		catch( const SchedulingException & e )
		{
			throw EventsException( 
				"EventsModule::enterMainLoop: scheduler stopped on failure: "
				+ e.toString() ) ;
		}	
		
		LogPlug::debug( "EventsModule::enterMainLoop: scheduler returned." ) ;
		
	}
	else
	{
	
		LogPlug::debug( "EventsModule::enterMainLoop: "
			"no scheduler requested, using basic event loop." ) ;
		
		enterBasicMainLoop() ;	
			
		LogPlug::debug( 
			"EventsModule::enterMainLoop: exiting now from main loop." ) ;
			
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
			LogPlug::error( "EventsModule::requestQuit: "
				"no scheduler found, none stopped, or stop failed: " 
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
			"EventsModule::getKeyboardHandler: no handler available: "
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
			"EventsModule::getJoystickHandler: no handler available: "
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
			"EventsModule::getMouseHandler: no handler available: "
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

#if OSDL_USES_SDL
		
	BasicEvent currentEvent ;
	
	
	// Checks for all pending events:

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

#endif // OSDL_USES_SDL

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

#if OSDL_USES_SDL

	Ceylan::Uint16 varCount = 
		sizeof( _SDLEnvironmentVariables ) / sizeof (string) ;
		
	string result = "Examining the " + Ceylan::toString( varCount )
		+ " events-related environment variables for SDL backend:" ;
	
	std::list<string> variables ;
		
	string var, value ;
	
	TextOutputFormat htmlFormat = TextDisplayable::GetOutputFormat() ;
	
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
	
#else // OSDL_USES_SDL

	return "(not using SDL)" ;

#endif // OSDL_USES_SDL
	
}



string EventsModule::DescribeEvent( BasicEvent anEvent ) throw() 
{

#if OSDL_USES_SDL

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

#else // OSDL_USES_SDL

	return "(not using SDL)" ;

#endif // OSDL_USES_SDL

}	



Millisecond EventsModule::GetMillisecondsSinceStartup() 
	throw( EventsException ) 
{

#if OSDL_USES_SDL

	// Do not slow down too much on release mode:

#if OSDL_DEBUG
	if ( ! OSDL::CommonModule::IsBackendInitialized() )
		throw EventsException( 
			"EventsModule::getMillisecondsSinceStartup() called "
			"whereas back-end not initialized." ) ;
#endif // OSDL_DEBUG	
	
	return static_cast<Millisecond>( SDL_GetTicks() ) ;
	
#else // OSDL_USES_SDL

	throw EventsException( "EventsModule::GetMillisecondsSinceStartup failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

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
			"EventsModule::enterBasicMainLoop: no video module available ("
			+ e.toString() ) ;
	}
	
	Rendering::Renderer * renderer = 0 ;
	
	if ( Rendering::Renderer::HasExistingRootRenderer() )
	{
		renderer = & Rendering::Renderer::GetExistingRootRenderer() ;
		LogPlug::debug( 
			"EventsModule::enterBasicMainLoop: using root renderer ("
			+ renderer->toString() + ")" ) ;
	}
	else
	{
		LogPlug::debug( "EventsModule::enterBasicMainLoop: "
			"not using any root renderer." ) ;	
	}
		
	
	/*
	 * No scheduler, using classical event loop.
	 *
	 * We believe we have a better accuracy than 'SDL_framerate' from 
	 * SDL_gfx with that mechanism:
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
			 * sleeping delays in default idle callback:
			 *
			 * (a 10% margin is added)
			 *
			 */
			_loopIdleCallbackMaxDuration = 
				static_cast<Microsecond>( 1.1 * getSchedulingGranularity() ) ;
			
		}
	
	
		// Compute the event loop period, in microsecond (default: 100 Hz):
		Microsecond	loopExpectedDuration 
			= static_cast<Microsecond>( 1000000.0f / _loopTargetedFrequency ) ; 
		
		// Default: 10ms, hence 10 000 microseconds:
		LogPlug::debug( "Loop expected duration is " 
			+ Ceylan::toString( loopExpectedDuration ) + " microseconds, "
			"and idle callback expected duration is "
			+ Ceylan::toString( _loopIdleCallbackMaxDuration ) 
			+ " microseconds." ) ;
	
		getPreciseTime( lastSec, lastMicrosec ) ;
		
		startedSec      = lastSec ;
		startedMicrosec = lastMicrosec ;
		
		
	    while ( ! _quitRequested )
		{
		
			// Checks for all pending events:
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
		
			// Do not call onIdle if it will likely make us miss the deadline:
			while ( getDurationBetween( lastSec, lastMicrosec, 
					nowSec,	nowMicrosec ) + _loopIdleCallbackMaxDuration 
				< loopExpectedDuration )
			{
				onIdle() ;
				getPreciseTime( nowSec, nowMicrosec ) ;			
			}
			
			/*
			 * Burn any last few microseconds with a 'soft' (thanks to 
			 * getPreciseTime) busy wait:
			 *
			 */
			while ( getDurationBetween( lastSec, lastMicrosec, nowSec,
					nowMicrosec ) < loopExpectedDuration )
				getPreciseTime( nowSec, nowMicrosec ) ;			
			
			/*
			 * Note that in all cases we stay equal or below to the requested
			 * frequency: we can be late (if the idle callback last for too
			 * long), but not early (we never stop before the current deadline),
			 * so on a run we are a bit late on average.
			 * 
			 */
			 
			// Ready for next iteration:
			lastSec      = nowSec ;
			lastMicrosec = nowMicrosec ;
		
		}
	
	} 
	catch ( const SystemException & e )
	{	
		throw EventsException( "EventsModule::enterBasicMainLoop: " 
			+ e.toString() ) ;
	}
	
	if ( ( lastSec - startedSec ) >
		Ceylan::System::MaximumDurationWithMicrosecondAccuracy )
	{
	
		// Avoid overflow of getDurationBetween, no average FPS computed: 
		LogPlug::debug( "Exited from main loop after " 
			+ Ceylan::toString( frameCount ) + " frames and about " 
			+ Ceylan::toString( lastSec - startedSec ) 
			+ " seconds, an average of " 
			+ Ceylan::toString( 
				static_cast<Ceylan::Float64>( _idleCallsCount ) / frameCount, 
				/* precision */ 3 )
			+ " idle calls per frame have been performed." ) ;
	}
	else
	{
		LogPlug::debug( "Exited from main loop after " 
			+ Ceylan::toString( frameCount ) + " frames and about " 
			+ Ceylan::toString( lastSec - startedSec ) 
			+ " seconds, on average there were " 
			+ Ceylan::toString( 
					1000000.0f /* since microseconds */ * frameCount / 
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

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Application gained keyboard focus." ) ;

#if OSDL_DEBUG
	if ( _keyboardHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onKeyboardFocusGained called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_keyboardHandler->focusGained( keyboardFocusEvent ) ;

#endif // OSDL_USES_SDL

}



void EventsModule::onKeyboardFocusLost(
	const FocusEvent & keyboardFocusEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Application lost keyboard focus." ) ;

#if OSDL_DEBUG
	if ( _keyboardHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onKeyboardFocusLost called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_keyboardHandler->focusLost( keyboardFocusEvent ) ;

#endif // OSDL_USES_SDL

}



void EventsModule::onKeyPressed( const KeyboardEvent & keyboardEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Key pressed." ) ;

#if OSDL_DEBUG
	if ( _keyboardHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onKeyPressed called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_keyboardHandler->keyPressed( keyboardEvent ) ;

#endif // OSDL_USES_SDL
	
}



void EventsModule::onKeyReleased( const KeyboardEvent & keyboardEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Key released." ) ;

#if OSDL_DEBUG
	if ( _keyboardHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onKeyReleased called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_keyboardHandler->keyReleased( keyboardEvent ) ;

#endif // OSDL_USES_SDL
	
}



// Mouse section.


void EventsModule::onMouseFocusGained( 
	const FocusEvent & mouseFocusEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Application gained mouse focus." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseFocusGained called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->focusGained( mouseFocusEvent ) ;

#endif // OSDL_USES_SDL
	
}



void EventsModule::onMouseFocusLost( 
	const FocusEvent & mouseFocusEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Application lost mouse focus." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseFocusLost called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->focusLost( mouseFocusEvent ) ;

#endif // OSDL_USES_SDL

}



void EventsModule::onMouseMotion( const MouseMotionEvent & mouseEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Mouse motion." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseMotion called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->mouseMoved( mouseEvent ) ;

#endif // OSDL_USES_SDL
	
}



void EventsModule::onMouseButtonPressed( const MouseButtonEvent & mouseEvent )
	throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Mouse button pressed." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseButtonPressed called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->buttonPressed( mouseEvent ) ;

#endif // OSDL_USES_SDL
	
}



void EventsModule::onMouseButtonReleased( const MouseButtonEvent & mouseEvent )
	throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Mouse button released." ) ;

#if OSDL_DEBUG
	if ( _mouseHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onMouseButtonReleased called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG

	_mouseHandler->buttonReleased( mouseEvent ) ;

#endif // OSDL_USES_SDL
	
}



// Joystick section.


void EventsModule::onJoystickAxisChanged( 
	const JoystickAxisEvent & joystickEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Joystick axis moved." ) ;
	
#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onJoystickAxisChanged called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG
	
	_joystickHandler->axisChanged( joystickEvent ) ;

#endif // OSDL_USES_SDL

}



void EventsModule::onJoystickTrackballChanged( 
	const JoystickTrackballEvent & joystickEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Joystick trackball moved." ) ;

#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( 
			"EventsModule::onJoystickTrackballChanged called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG
	
	_joystickHandler->trackballChanged( joystickEvent ) ;

#endif // OSDL_USES_SDL

}



void EventsModule::onJoystickHatChanged( 
	const JoystickHatEvent & joystickEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Joystick hat moved." ) ;

#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( "EventsModule::onJoystickHatChanged called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG
	
	_joystickHandler->hatChanged( joystickEvent ) ;

#endif // OSDL_USES_SDL
	
}



void EventsModule::onJoystickButtonPressed( 
	const JoystickButtonEvent & joystickEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Joystick button pressed." ) ;

#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( 
			"EventsModule::onJoystickButtonPressed called "
			"whereas no handler is available." ) ;
#endif // OSDL_DEBUG
	
	_joystickHandler->buttonPressed( joystickEvent ) ;

#endif // OSDL_USES_SDL

}



void EventsModule::onJoystickButtonReleased( 
	const JoystickButtonEvent & joystickEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Joystick button released." ) ;

#if OSDL_DEBUG
	if ( _joystickHandler == 0 )
		Ceylan::emergencyShutdown( 
			"EventsModule::onJoystickButtonReleased called "
			"whereas no handler is available." ) ;
#endif
	
	_joystickHandler->buttonReleased( joystickEvent ) ;

#endif // OSDL_USES_SDL

}



// Application-generic section.

		
void EventsModule::onIdle() throw() 
{		

#if OSDL_USES_SDL

	_idleCallsCount++ ;
	
	if ( _loopIdleCallback != 0 )
	{
		OSDL_EVENT_LOG( 
			"EventsModule::onIdle: calling now idle call-back." ) ;
		
		(*_loopIdleCallback)( _loopIdleCallbackData ) ;
		
		OSDL_EVENT_LOG( 
			"EventsModule::onIdle: returned from idle call-back." ) ;
		
	}
	else
	{
	
		/*
		 * Issues an atomic sleep, chosen so that the minimum real
		 * sleeping time can be performed, scheduler-wise.
		 *
		 */
		 Ceylan::System::atomicSleep() ;
		
	}	

#endif // OSDL_USES_SDL
		
}



void EventsModule::onApplicationFocusChanged( 
	const FocusEvent & focusEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Application focus changed." ) ;
	
	// Maybe multiple gains or losses could be sent in one event:

	if ( focusEvent.state & _MouseFocus )
	{
		
		// DefaultMouse: only one mouse supported for the moment.
		
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

#endif // OSDL_USES_SDL
		
}



void EventsModule::onApplicationIconified( 
	const FocusEvent & focusEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Application is iconified (no handler registered): "
		+ DescribeEvent( focusEvent ) ) ;

#endif // OSDL_USES_SDL
	
}



void EventsModule::onApplicationRestored( 
	const FocusEvent & focusEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Application is restored (no handler registered): "
		+ DescribeEvent( focusEvent ) ) ;
#endif // OSDL_USES_SDL
	
}



void EventsModule::onQuitRequested() throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Application is requested to stop." ) ;

	requestQuit() ;
	
#endif // OSDL_USES_SDL
	
}



void EventsModule::onSystemSpecificWindowManagerEvent( 
	const SystemSpecificWindowManagerEvent & wmEvent ) throw()
{

#if OSDL_USES_SDL

	/*
	 * @see http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSysWMEvent to 
	 * obtain system-specific information about the window manager, 
	 * with SDL_GetWMInfo.
	 *
	 */
	OSDL_EVENT_LOG( "System specific window manager event received "
		"(no handler registered)." ) ;	 
		
#endif // OSDL_USES_SDL

}

	
					
void EventsModule::onResizedWindow( const WindowResizedEvent & resizeEvent )
	throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Resizing, new width is " << resizeEvent.w 
		<< ", new height is " << resizeEvent.h << "." ) ;
		
	try
	{	
		OSDL::getExistingCommonModule().getVideoModule().resize( 
			resizeEvent.w, resizeEvent.h ) ;
	} 
	catch( const Video::VideoException & e )
	{
		// Warn but continue nevertheless:
		LogPlug::error( 
			"EventsModule::onResizedWindow: error when resizing to "
			+ Ceylan::toString( resizeEvent.w ) + ", " 
			+ Ceylan::toString( resizeEvent.h ) + "): "
			+ e.toString() ) ;
	}

#endif // OSDL_USES_SDL
	
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
		LogPlug::error( "EventsModule::onScreenNeedsRedraw: "
			"error when requesting a redraw: "	+ e.toString() ) ;
	}

}



void EventsModule::onUserEvent( const UserEvent & userEvent ) throw() 
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "User event received, whose type is " << userEvent.type
		<< " (no handler registered)." ) ;
		
#endif // OSDL_USES_SDL

}

		
					
void EventsModule::onUnknownEventType( const BasicEvent & unknownEvent ) throw()
{

#if OSDL_USES_SDL

	OSDL_EVENT_LOG( "Unknown event, type is " << unknownEvent.type 
		<< " (no handler registered)." ) ;
	
#endif // OSDL_USES_SDL

}




// Static section:


string EventsModule::DescribeEvent( const FocusEvent & focusEvent )
{
	
#if OSDL_USES_SDL

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
	
#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;
	
#endif // OSDL_USES_SDL

}



string EventsModule::DescribeEvent( const KeyboardEvent & keyboardEvent )
{
	
#if OSDL_USES_SDL

	string res = "Key " + KeyboardHandler::DescribeKey( 
		static_cast<KeyboardHandler::KeyIdentifier>( 
			keyboardEvent.keysym.sym ) ) 
		+ ", with modifier(s): " 
		+ KeyboardHandler::DescribeModifier( 
			static_cast<KeyboardHandler::KeyModifier>( 
				keyboardEvent.keysym.mod ) )
		+ " (scancode:  " 
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

#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;
	
#endif // OSDL_USES_SDL

}



string EventsModule::DescribeEvent( const MouseMotionEvent & mouseMotionEvent )
{

#if OSDL_USES_SDL

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
	
#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL
	
}



string EventsModule::DescribeEvent( const MouseButtonEvent & mouseButtonEvent )
{

#if OSDL_USES_SDL

	string res ;
	
	switch( mouseButtonEvent.button )
	{
	
		case SDL_BUTTON_LEFT:
			res = "Left mouse button " 
				+ ( mouseButtonEvent.state == SDL_PRESSED ) ? 
					"pressed": "released" ;
			break ;
			
		case SDL_BUTTON_MIDDLE:
			res = "Middle mouse button " 
				+ ( mouseButtonEvent.state == SDL_PRESSED ) ? 
					"pressed": "released" ;
			break ;
			
		case SDL_BUTTON_RIGHT:
			res = "Right mouse button " 
				+ ( mouseButtonEvent.state == SDL_PRESSED ) ? 
					"pressed": "released" ;
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

#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL

}



string EventsModule::DescribeEvent( const JoystickAxisEvent & axisEvent )
{

#if OSDL_USES_SDL

	return "Joystick #" + Ceylan::toNumericalString( axisEvent.which ) 
		+ " had axis #" + Ceylan::toNumericalString( axisEvent.axis ) 
		+ " moved to " 	+ Ceylan::toString( axisEvent.value ) + "." ; 

#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL
		
}



string EventsModule::DescribeEvent( const JoystickTrackballEvent & ballEvent )
{

#if OSDL_USES_SDL

	return "Joystick #" + Ceylan::toNumericalString( ballEvent.which ) 
		+ " had trackball #" + Ceylan::toNumericalString( ballEvent.ball ) 
		+ " moved of (" 
		+ Ceylan::toString( ballEvent.xrel ) + ";" 
		+ Ceylan::toString( ballEvent.yrel ) + ")." ; 

#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL

}



string EventsModule::DescribeEvent( const JoystickHatEvent & hatEvent )
{

#if OSDL_USES_SDL

	return "Joystick #" + Ceylan::toString( hatEvent.which ) + " had hat #"
		+ Ceylan::toNumericalString( hatEvent.hat ) + " set to position " 
		+ Ceylan::toString( hatEvent.value ) + "." ; 

#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL

}



string EventsModule::DescribeEvent( const JoystickButtonEvent & buttonEvent )
{

#if OSDL_USES_SDL

	return "Joystick #" + Ceylan::toNumericalString( buttonEvent.which ) 
		+ " had button #" + Ceylan::toNumericalString( buttonEvent.button ) 
		+ ( ( buttonEvent.state == SDL_PRESSED ) ? 
			" pressed.": " released." ) ;
		 
#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL

}



string EventsModule::DescribeEvent( const UserRequestedQuitEvent & quitEvent )  
{

#if OSDL_USES_SDL

	return "User requested to quit." ;
	
#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL

}



string EventsModule::DescribeEvent( 
	const SystemSpecificWindowManagerEvent & windowManagerEvent ) 
{

#if OSDL_USES_SDL

	/* 
	 * @todo add int SDL_GetWMInfo(SDL_SysWMinfo *info); 
	 * (see http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSysWMEvent)
	 *
	 */
	 
	return "System-specific window manager event occured." ;

#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL
	
}



string EventsModule::DescribeEvent( const WindowResizedEvent & resizeEvent )
{

#if OSDL_USES_SDL

	return "Window resized to (" + Ceylan::toString( resizeEvent.w )
		+ ";" + Ceylan::toString( resizeEvent.h ) + ")." ;
		
#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL

}



string EventsModule::DescribeEvent( const ScreenExposedEvent & redrawEvent )
{

#if OSDL_USES_SDL

	return "Window exposed, needs to be redrawn." ;
	
#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL

}



string EventsModule::DescribeEvent( const UserEvent & userEvent ) 
{

#if OSDL_USES_SDL

	return "User-defined event occured (code #" 
		+ Ceylan::toString( userEvent.code ) + ")." ;

#else // OSDL_USES_SDL

	return NoSDLSupportAvailable ;

#endif // OSDL_USES_SDL

}

 
 
bool EventsModule::IsEventsInitialized() throw()
{

	return _EventsInitialized ;
	
}



Ceylan::System::Microsecond EventsModule::EvaluateCallbackduration(
	Ceylan::System::Callback callback, void * callbackData ) throw()
{

	// Issue just a first and only run, to measure how long it takes:
	Microsecond startedMicrosec ;
	Second startedSec ;
		
	Microsecond endedMicrosec ;
	Second endedSec ;
		
	getPreciseTime( startedSec, startedMicrosec ) ;
			
	(*callback)( callbackData ) ;
		
	getPreciseTime( endedSec, endedMicrosec ) ;
			
	Microsecond callbackExpectedMaxDuration =
		static_cast<Microsecond>( 1.2 *
			getDurationBetween( startedSec,	startedMicrosec,
					endedSec, endedMicrosec ) ) ;
			
	LogPlug::debug( "EventsModule::EvaluateCallbackduration: duration for "
		"idle callback evaluated to " 
		+ Ceylan::toString( callbackExpectedMaxDuration )
		+ " microseconds." ) ;
	
	return callbackExpectedMaxDuration ;

}

	

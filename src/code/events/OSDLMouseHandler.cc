#include "OSDLMouseHandler.h"

#include "OSDLMouse.h"             // for Mouse
#include "OSDLController.h"        // for Controller



using std::string ;
using std::list ;


using namespace Ceylan::Log ;      // for LogPlug
using namespace OSDL::Events ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>            // for OSDL_VERBOSE_MOUSE_HANDLER and al
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS





#if OSDL_VERBOSE_MOUSE_HANDLER

#include <iostream>
#define OSDL_MOUSE_HANDLER_LOG( message ) std::cout << "[OSDL Mouse Handler] " << message << std::endl ;

#else // OSDL_VERBOSE_MOUSE_HANDLER

#define OSDL_MOUSE_HANDLER_LOG( message )

#endif // OSDL_VERBOSE_MOUSE_HANDLER




MouseHandler::MouseHandler( bool useClassicalMice )
		throw( InputDeviceHandlerException ):
	InputDeviceHandler(),
	_miceCount( 0 ),
	_mice( 0 ),
	_useClassicalMice( useClassicalMice )
{

#if OSDL_USES_SDL

	send( "Initializing Mouse subsystem." ) ;

	// No SDL_InitSubSystem for mice.

	_miceCount = GetAvailableMiceCount() ;
	
	_mice = new Mouse *[ _miceCount ] ;
	
	for ( MouseNumber i = 0; i < _miceCount ; i++ )
		_mice[i] = new Mouse( i, _useClassicalMice ) ;
	
	send( "Mouse subsystem initialized." ) ;

	dropIdentifier() ;
	
#else // OSDL_USES_SDL

	throw InputDeviceHandlerException( "MouseHandler constructor failed: "
		"no SDL support available" ) ;
		 
#endif // OSDL_USES_SDL

}



MouseHandler::~MouseHandler() throw()
{

	send( "Stopping Mouse subsystem." ) ;

	// Mouse instances are owned by the handler:
	blank() ;
	
	// No SDL_QuitSubSystem for Mouse.
	
	send( "Mouse subsystem stopped." ) ;
		
}



void MouseHandler::linkToController( OSDL::MVC::Controller & controller )
	throw( MouseException )
{
	
	linkToController( DefaultMouse, controller ) ;
			
}


					
void MouseHandler::linkToController( MouseNumber mouseIndex,
	OSDL::MVC::Controller & controller ) throw( MouseException )
{
	
	/*
	 * Beware, mouse list might be out of synch, test is not exactly
	 * equivalent to _miceCount.
	 *
	 */	
	if ( mouseIndex >= static_cast<MouseNumber>( GetAvailableMiceCount() ) )
		throw MouseException( "MouseHandler::linkToController: index " 
			+ Ceylan::toString( mouseIndex ) + " out of bounds (" 
			+ Ceylan::toString( GetAvailableMiceCount() )
			+ " mice attached)." ) ;
			
	if ( mouseIndex >= _miceCount )
		throw MouseException( "MouseHandler::linkToController: index " 
			+ Ceylan::toString( mouseIndex ) + " out of bounds (" 
			+ Ceylan::toString( _miceCount )
			+ " mice attached according to internal mouse list)." ) ;
	
#if OSDL_DEBUG
	if ( _mice[ mouseIndex ] == 0 )
		throw MouseException( "MouseHandler::linkToController: "
			"no known mouse for index " 
			+ Ceylan::toString( mouseIndex ) + "." ) ;
#endif // OSDL_DEBUG

	_mice[ mouseIndex ]->setController( controller ) ;

}
	

	
const string MouseHandler::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{

	string res = "Mouse handler " ;
	
	switch( _miceCount )
	{
	
		case 0:
			res += "does not manage any mouse" ;
			break ;
			
		case 1:
			res += "managing one mouse" ;
			break ;
		
		default:	
			res += "managing " + Ceylan::toNumericalString( _miceCount ) 
			+ " mice" ;
			
	}
	
	if ( level == Ceylan::low ) 	
		return res ;
	
	res += ". Listing detected mice: " ;
		
	list<string> mice ;
		
	for ( MouseNumber i = 0; i < _miceCount; i++ )
		if ( _mice[i] != 0 )
			mice.push_back( _mice[i]->toString( level ) ) ;
		else
			mice.push_back( "no mouse at index " + Ceylan::toString( i ) 
				+ " (abnormal)" ) ;
			
	return res + Ceylan::formatStringList( mice ) ;	
		
}



MouseNumber MouseHandler::GetAvailableMiceCount() throw() 
{

	/*
	 * No way of guessing with SDL 1.2.x:
	 *
	 */
	return static_cast<MouseNumber>( 1 ) ;
	
}




// Protected section.



void MouseHandler::focusGained( const FocusEvent & mouseFocusEvent ) 
	const throw()
{

#if OSDL_USES_SDL
	 
#if OSDL_DEBUG
	checkMouseAt( DefaultMouse ) ;
#endif // OSDL_DEBUG
	
	_mice[ DefaultMouse ]->focusGained( mouseFocusEvent ) ;
	
#endif // OSDL_USES_SDL

}



void MouseHandler::focusLost( const FocusEvent & mouseFocusEvent ) const throw()
{

#if OSDL_USES_SDL
	 
#if OSDL_DEBUG
	checkMouseAt( DefaultMouse ) ;
#endif // OSDL_DEBUG
	
	_mice[ DefaultMouse ]->focusLost( mouseFocusEvent ) ;

#endif // OSDL_USES_SDL
	
}



void MouseHandler::mouseMoved( const MouseMotionEvent & mouseMovedEvent ) 
	const throw()
{

#if OSDL_USES_SDL
	 
#if OSDL_DEBUG
	checkMouseAt( mouseMovedEvent.which ) ;
#endif // OSDL_DEBUG
	
	_mice[ mouseMovedEvent.which ]->mouseMoved( mouseMovedEvent ) ;
	
#endif // OSDL_USES_SDL

}



void MouseHandler::buttonPressed( 
		const MouseButtonEvent & mouseButtonPressedEvent ) 
	const throw()
{

#if OSDL_USES_SDL
	 
#if OSDL_DEBUG
	checkMouseAt( mouseButtonPressedEvent.which ) ;
#endif // OSDL_DEBUG
	
	_mice[ mouseButtonPressedEvent.which ]->buttonPressed(
		mouseButtonPressedEvent ) ;
	
#endif // OSDL_USES_SDL

}


void MouseHandler::buttonReleased( 
		const MouseButtonEvent & mouseButtonReleasedEvent ) 
	const throw()
{

#if OSDL_USES_SDL
	 
#if OSDL_DEBUG
	checkMouseAt( mouseButtonReleasedEvent.which ) ;
#endif // OSDL_DEBUG
	
	_mice[ mouseButtonReleasedEvent.which ]->buttonReleased(
		mouseButtonReleasedEvent ) ;
	
#endif // OSDL_USES_SDL

}



void MouseHandler::blank() throw()
{

	if ( _mice != 0 )
	{

		for ( MouseNumber c = 0; c < _miceCount; c++ )
		{
			delete _mice[c] ;
		}

		delete [] _mice ;
		_mice = 0 ;
	}
	
	_miceCount = 0 ;
	
}


void MouseHandler::checkMouseAt( MouseNumber index ) const throw()
{

	if ( index >= _miceCount )
		Ceylan::emergencyShutdown( 
			"MouseHandler::checkMouseAt: index "
			+ Ceylan::toNumericalString( index ) 
			+ " out of bounds (maximum value is "
			+ Ceylan::toNumericalString( _miceCount - 1 ) + ")." ) ; 
		
	if ( _mice[ index ] == 0 )
		Ceylan::emergencyShutdown( "MouseHandler::checkMouseAt: "
			"no mouse intance at index "
			+ Ceylan::toNumericalString( index ) + "." ) ;
	
}


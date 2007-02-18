#include "OSDLMouseHandler.h"

#include "OSDLMouse.h"             // for Mouse
#include "OSDLController.h"        // for Controller
//#include "OSDLEvents.h"          // for KeyPressed, EventsException

#include "SDL.h"                   // for SDL_Getmicetate, etc.


using std::string ;
using std::list ;
using std::map ;


using namespace Ceylan::Log ;    // for LogPlug
using namespace OSDL::Events ;





#ifdef OSDL_VERBOSE_MOUSE_HANDLER


#include <iostream>
#define OSDL_MOUSE_HANDLER_LOG( message ) std::cout << "[OSDL Mouse Handler] " << message << std::endl ;

#else // OSDL_VERBOSE_MOUSE_HANDLER

#define OSDL_MOUSE_HANDLER_LOG( message )

#endif // OSDL_VERBOSE_MOUSE_HANDLER




MouseHandler::MouseHandler( bool useClassicalMice )
		throw( InputDeviceHandlerException ) :
	InputDeviceHandler(),
	_miceCount( 0 ),
	_mice( 0 ),
	_useClassicalMice( useClassicalMice )
{

	send( "Initializing Mouse subsystem." ) ;

	// No SDL_InitSubSystem for mice.

	_miceCount = GetAvailableMiceCount() ;
	
	_mice = new Mouse *[ _miceCount ] ;
	
	for ( MouseNumber i = 0; i < _miceCount ; i++ )
		_mice[i] = new Mouse( _useClassicalMice ) ;
	
	send( "Mouse subsystem initialized." ) ;

	dropIdentifier() ;
	
}


MouseHandler::~MouseHandler() throw()
{

	send( "Stopping Mouse subsystem." ) ;

	// Mouse instances are owned by the handler :
	blank() ;
	
	// No SDL_QuitSubSystem for Mouse.
	
	send( "Mouse subsystem stopped." ) ;
		
}


void MouseHandler::linkToController( OSDL::MVC::Controller & controller )
	throw( MouseException )
{
	
	linkToController( DefaultMouse, controller ) ;
			
}

					
void MouseHandler::linkToController( MouseNumber index,
	OSDL::MVC::Controller & controller  ) throw()
{
	
	if ( mouseIndex >= _miceCount )
		throw MouseException( "MouseHandler::linkToController : "
			"mouse #" + Ceylan::toString( mouseIndex ) 
			+ " requested, whereas the total number of known mice is "
			+ Ceylan::toString( _miceCount ) ) ;
	
	/*
	 * Beware, mouse list might be out of synch, test is not exactly
	 * equivalent to _miceCount.
	 *
	 */	
	if ( mouseIndex >= static_cast<MouseNumber>( GetAvailableMouseCount() ) )
		throw MouseException( "MouseHandler::linkToController : index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( GetAvailableMouseCount() )
			+ " mice attached)." ) ;
			
	if ( index >= _miceCount )
		throw MouseException( "MouseHandler::linkToController : index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( _miceCount )
			+ " mice attached according to internal mouse list)." ) ;
	
#if OSDL_DEBUG
	if ( _mice[ index ] == 0 )
		throw MouseException( "MouseHandler::linkToController : "
			"no known mouse for index " + Ceylan::toString( index ) + "." ) ;
#endif // OSDL_DEBUG

	_mice[ index ]->setController( controller ) ;

}
	
	
const string MouseHandler::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{

	string res ;
	
	if ( _miceCount > 0 )
		res = "Mouse handler managing " 
		+ Ceylan::toString( _miceCount ) + " mice" ;
	else
		return "Mouse handler does not manage any mouse" ;

	if ( level == Ceylan::low ) 	
		return res ;
	
	res += ". Listing detected mice : " ;
		
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
	 * No way of guessing with SDL 1.2.x :
	 *
	 */
	return static_cast<MouseNumber>( 1 ) ;
	
}




// Protected section.



void MouseHandler::focusGained( MouseNumber mouse ) const throw()
{

	/*
	 * Warning : specified mouse index must match the SDL ones.
	 *
	 */
	 
#if OSDL_DEBUG
	checkMouseAt( mouse ) ;
#endif // OSDL_DEBUG
	
	_mice[ mouse ]->focusGained() ;
	
}


void MouseHandler::focusLost( MouseNumber mouse ) const throw()
{

	/*
	 * Warning : specified mouse index must match the SDL ones.
	 *
	 */
	 
#if OSDL_DEBUG
	checkMouseAt( mouse ) ;
#endif // OSDL_DEBUG
	
	_mice[ mouse ]->focusLost() ;
	
}


void MouseHandler:: mouseMoved( const MouseMotionEvent & mouseMovedEvent ) 
	const throw()
{
	 
#if OSDL_DEBUG
	checkMouseAt( mouseMovedEvent.which ) ;
#endif // OSDL_DEBUG
	
	_mice[ mouseEvent.which ]->mouseMoved( mouseMovedEvent ) ;
	
}


void MouseHandler:: buttonPressed( 
		const MouseButtonEvent & mouseButtonPressedEvent ) 
	const throw()
{
	 
#if OSDL_DEBUG
	checkMouseAt( mouseButtonPressedEvent.which ) ;
#endif // OSDL_DEBUG
	
	_mice[ mouseButtonPressedEvent.which ]->buttonPressed(
		mouseButtonPressedEvent ) ;
	
}


void MouseHandler:: buttonReleased( 
		const MouseButtonEvent & mouseButtonReleasedEvent ) 
	const throw()
{
	 
#if OSDL_DEBUG
	checkMouseAt( mouseButtonReleasedEvent.which ) ;
#endif // OSDL_DEBUG
	
	_mice[ mouseButtonReleasedEvent.which ]->buttonReleased(
		mouseButtonReleasedEvent ) ;
	
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
			"MouseHandler::checkMouseAt : index "
			+ Ceylan::toNumericalString( index ) 
			+ " out of bounds (maximum value is "
			+ Ceylan::toNumericalString( _miceCount - 1 ) + ")." ) ; 
		
	if ( _mice[ index ] == 0 )
		Ceylan::emergencyShutdown( "MouseHandler::checkMouseAt : "
			"no mouse intance at index "
			+ Ceylan::toNumericalString( index ) + "." ) ;
	
}


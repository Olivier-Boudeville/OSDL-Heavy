#include "OSDLMouse.h"       

#include "OSDLController.h"    // for mouseMoved, etc.

#include "SDL.h"               // for SDL_GetMouseState, etc.


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>       // for OSDL_DEBUG and al (private header)
#endif // OSDL_USES_CONFIG_H


using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL::Events ;
using namespace OSDL::MVC ;     // for mouseAxisChanged, etc.



#ifdef OSDL_VERBOSE_MOUSE

#include <iostream>
#define OSDL_MOUSE_LOG( message ) std::cout << "[OSDL Mouse] " << message << std::endl ;

#else // OSDL_VERBOSE_MOUSE

#define OSDL_MOUSE_LOG( message )

#endif // OSDL_VERBOSE_MOUSE



/*
 * Not used currently since event loop is prefered to polling :
 *   - SDL_MouseUpdate
 *	 - SDL_MouseEventState (used in OSDLmiceHandler)
 *
 */

Mouse::Mouse( MouseNumber index ) throw() :
	OSDL::Events::InputDevice(),
	_name( SDL_MouseName( index ) ),
	_index( index ),
	_internalMouse( 0 ),
	_axisCount( 0 ),
	_trackballCount( 0 ),
	_hatCount( 0 ),
	_buttonCount( 0 )
{

	if ( _name.empty() )
		_name = "(unknown mouse)" ;
	
}


Mouse::~Mouse() throw()
{

	if ( isOpen() )
		close() ;
		
	/*
	 * Does not seem to be owned :
	 *
	if ( _internalMouse != 0 )	
		::free( _internalMouse ) ;
	 *
	 */
	 	
}


const string & Mouse::getName() const throw()
{
	return _name ;
}


bool Mouse::isOpen() const throw()
{
	
	bool isOpened = ( SDL_MouseOpened( _index ) == 1 ) ;
	
#if OSDL_DEBUG
	if ( isOpened != ( _internalMouse != 0 ) ) 
		LogPlug::error( "Mouse::isOpen : conflicting status : "
			"back-end (makes authority) says " + Ceylan::toString( isOpened ) 
			+ " whereas internal status says " 
			+ Ceylan::toString( ( _internalMouse != 0 ) )
			+ "." ) ;			
#endif // OSDL_DEBUG 
	
	return isOpened ;
	
}


void Mouse::open() throw( MouseException ) 
{

	if ( isOpen() )
		throw MouseException( 
			"Mouse::open requested whereas was already open." ) ;
		
	_internalMouse = SDL_MouseOpen( _index ) ;
	
	update() ;
	
}

 
void Mouse::close() throw( MouseException )
{

	if ( ! isOpen() )
		throw MouseException( 
			"Mouse::close requested whereas was not open." ) ;
		
	SDL_MouseClose( _internalMouse ) ;
	
	// Should not be a memory leak, according to SDL doc :
	_internalMouse = 0 ;
	
}


void Mouse::axisChanged( const MouseAxisEvent & mouseEvent ) throw()
{
	
	if ( isLinkedToController() )
		getActualController().mouseAxisChanged( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Axis changed for mouse #" 
			+ Ceylan::toNumericalString( mouseEvent.which ) + " : "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}
			
}


void Mouse::trackballChanged( const MouseTrackballEvent & mouseEvent )
	throw()
{
	
	if ( isLinkedToController() )
		getActualController().mouseTrackballChanged( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Trackball changed for mouse #" 
			+ Ceylan::toNumericalString( mouseEvent.which ) + " : "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}
			
}


void Mouse::hatChanged( const MouseHatEvent & mouseEvent ) throw()
{
	
	if ( isLinkedToController() )
		getActualController().mouseHatChanged( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Hat changed for mouse #" 
			+ Ceylan::toNumericalString( mouseEvent.which ) + " : "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}		
			
}


void Mouse::buttonPressed( const MouseButtonEvent & mouseEvent )
	throw()
{
	
	if ( isLinkedToController() )
		getActualController().mouseButtonPressed( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Button pressed for mouse #" 
			+ Ceylan::toNumericalString( mouseEvent.which ) + " : "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}
		
}


void Mouse::buttonReleased( const MouseButtonEvent & mouseEvent )
	throw()
{
	
	if ( isLinkedToController() )
		getActualController().mouseButtonReleased( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Button released for mouse #" 
			+ Ceylan::toNumericalString( mouseEvent.which ) + " : "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}		
			
}


MouseAxesCount Mouse::getNumberOfAxes() const throw()
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Mouse::getNumberOfAxes : mouse not open." ) ;	
#endif // OSDL_DEBUG

	return _axisCount ;
	
}


MouseTrackballsCount Mouse::getNumberOfTrackballs() const throw()
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Mouse::getNumberOfTrackballs : mouse not open." ) ;	
#endif // OSDL_DEBUG

	return _trackballCount ;
	
}


MouseHatsCount Mouse::getNumberOfHats() const throw()
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Mouse::getNumberOfHats : mouse not open." ) ;	
#endif // OSDL_DEBUG
	
	return _hatCount ;
	
}


MouseButtonsCount Mouse::getNumberOfButtons() const throw()
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Mouse::getNumberOfButtons : mouse not open." ) ;	
#endif // OSDL_DEBUG
	
	return _buttonCount ;
	
}


AxisPosition Mouse::getAbscissaPosition() const throw() 
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Mouse::getAbscissaPosition : mouse not open." ) ;
		
	if ( _axisCount == 0 )
		Ceylan::emergencyShutdown( 
			"Mouse::getAbscissaPosition : no X axe available." ) ;
	
#endif // OSDL_DEBUG
	
	return SDL_MouseGetAxis( _internalMouse, 0 ) ;
	
}


AxisPosition Mouse::getOrdinatePosition() const throw() 
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Mouse::getOrdinatePosition : mouse not open." ) ;
		
	if ( _axisCount <= 1 )
		Ceylan::emergencyShutdown( 
			"Mouse::getOrdinatePosition : no Y axe available." ) ;
	
#endif // OSDL_DEBUG

	return SDL_MouseGetAxis( _internalMouse, 1 ) ;
	
}


AxisPosition Mouse::getPositionOfAxis( MouseAxesCount index ) 
	const throw( MouseException )
{

	if ( ! isOpen() )
		throw MouseException( 
			"Mouse::getPositionOfAxis : mouse not open." ) ;
	
	if ( index >= _axisCount )
		throw MouseException( "Mouse::getPositionOfAxis : axe index ( " 
			+ Ceylan::toString( index ) + ") out of bounds." ) ;
		
	return SDL_MouseGetAxis( _internalMouse, index ) ;
	
}


HatPosition Mouse::getPositionOfHat( MouseHatsCount index ) 
	const throw( MouseException )
{

	if ( ! isOpen() )
		throw MouseException( 
			"Mouse::getPositionOfHat : mouse not open." ) ;
	
	if ( index >= _hatCount )
		throw MouseException( "Mouse::getPositionOfHat : hat index ( " 
			+ Ceylan::toString( index ) + ") out of bounds." ) ;
		
	return SDL_MouseGetHat( _internalMouse, index ) ;
	
}


bool Mouse::isButtonPressed( MouseButtonsCount buttonNumber ) 
	const throw( MouseException )
{

	if ( ! isOpen() )
		throw MouseException( 
			"Mouse::isButtonPressed : mouse not open." ) ;
	
	if ( buttonNumber >= _buttonCount )
		throw MouseException( "Mouse::isButtonPressed : button number ( " 
			+ Ceylan::toString( buttonNumber ) + ") out of bounds." ) ;

	return ( SDL_MouseGetButton( _internalMouse, buttonNumber ) == 1 ) ;
	
}


bool Mouse::getPositionOfTrackball( MouseTrackballsCount ball, 
	BallMotion & deltaX, BallMotion & deltaY ) const throw( MouseException )				
{

	if ( ! isOpen() )
		throw MouseException( 
			"Mouse::getPositionOfTrackball : mouse not open." ) ;
	
	if ( ball >= _trackballCount )
		throw MouseException( 
			"Mouse::getPositionOfTrackball : trackball number ( " 
			+ Ceylan::toString( _trackballCount ) + ") out of bounds." ) ;
			
 	return ( SDL_MouseGetBall( 
		_internalMouse, ball, & deltaX, & deltaY ) == 0 ) ;

}

	
MouseNumber Mouse::getIndex() const throw()
{

	if ( _internalMouse == 0 )
		Ceylan::emergencyShutdown( 
			"Mouse::getIndex : no internal mouse registered." ) ;
	
	MouseNumber index = SDL_MouseIndex( _internalMouse ) ;
	
#if OSDL_DEBUG

	if ( index != _index ) 
		LogPlug::error( "Mouse::getIndex : conflicting status : "
			"back-end (makes authority) says index is " 
			+ Ceylan::toString( index ) 
			+ " whereas internal index says " 
			+ Ceylan::toString( _index ) + "." ) ;	
					
#endif // OSDL_DEBUG

	return index ;
	
}


void Mouse::update() throw()
{

	_axisCount      = SDL_MouseNumAxes(    _internalMouse ) ;
	_trackballCount = SDL_MouseNumBalls(   _internalMouse ) ;
	_hatCount       = SDL_MouseNumHats(    _internalMouse ) ;
	_buttonCount    = SDL_MouseNumButtons( _internalMouse ) ;
	
}


const string Mouse::toString( Ceylan::VerbosityLevels level ) const throw()
{

	if ( ! isOpen() )
		return "Non-opened mouse whose index is #" 
			+ Ceylan::toString( _index ) 
			+ ", named '" + _name + "'";
		
	return "Opened mouse whose index is #" + Ceylan::toString( _index ) 
		+ ", named '" + _name + "'. This mouse has "
		+ Ceylan::toString( _axisCount ) + " axis, " 
		+ Ceylan::toString( _trackballCount ) + " trackball(s), "
		+ Ceylan::toString( _hatCount ) + " hat(s), and "
		+ Ceylan::toString( _buttonCount ) + " button(s)" ;
				
}


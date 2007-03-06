#include "OSDLJoystick.h"       

#include "OSDLController.h"     // for joystickAxisChanged, etc.

#include "SDL.h"                // for SDL_JoystickOpened, etc.



using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL::Events ;
using namespace OSDL::MVC ;     // for joystickAxisChanged, etc.


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>         // for OSDL_VERBOSE_JOYSTICK and al 
#endif // OSDL_USES_CONFIG_H



#if OSDL_VERBOSE_JOYSTICK

#include <iostream>
#define OSDL_JOYSTICK_LOG( message ) std::cout << "[OSDL Joystick] " << message << std::endl ;

#else // OSDL_VERBOSE_JOYSTICK

#define OSDL_JOYSTICK_LOG( message )

#endif // OSDL_VERBOSE_JOYSTICK



/*
 * Not used currently since event loop is prefered to polling :
 *   - SDL_JoystickUpdate
 *	 - SDL_JoystickEventState (used in OSDLJoysticksHandler)
 *
 */

Joystick::Joystick( JoystickNumber index ) throw() :
	OSDL::Events::InputDevice(),
	_name( SDL_JoystickName( index ) ),
	_index( index ),
	_internalJoystick( 0 ),
	_axisCount( 0 ),
	_trackballCount( 0 ),
	_hatCount( 0 ),
	_buttonCount( 0 )
{

	if ( _name.empty() )
		_name = "(unknown joystick)" ;
	
}


Joystick::~Joystick() throw()
{

	if ( isOpen() )
		close() ;
		
	/*
	 * Does not seem to be owned :
	 *
	if ( _internalJoystick != 0 )	
		::free( _internalJoystick ) ;
	 *
	 */
	 	
}


const string & Joystick::getName() const throw()
{
	return _name ;
}


bool Joystick::isOpen() const throw()
{
	
	bool isOpened = ( SDL_JoystickOpened( _index ) == 1 ) ;
	
#if OSDL_DEBUG
	if ( isOpened != ( _internalJoystick != 0 ) ) 
		LogPlug::error( "Joystick::isOpen : conflicting status : "
			"back-end (makes authority) says " + Ceylan::toString( isOpened ) 
			+ " whereas internal status says " 
			+ Ceylan::toString( ( _internalJoystick != 0 ) )
			+ "." ) ;			
#endif // OSDL_DEBUG 
	
	return isOpened ;
	
}


void Joystick::open() throw( JoystickException ) 
{

	if ( isOpen() )
		throw JoystickException( 
			"Joystick::open requested whereas was already open." ) ;
		
	_internalJoystick = SDL_JoystickOpen( _index ) ;
	
	update() ;
	
}

 
void Joystick::close() throw( JoystickException )
{

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::close requested whereas was not open." ) ;
		
	SDL_JoystickClose( _internalJoystick ) ;
	
	// Should not be a memory leak, according to SDL doc :
	_internalJoystick = 0 ;
	
}


void Joystick::axisChanged( const JoystickAxisEvent & joystickEvent ) throw()
{
	
	if ( isLinkedToController() )
		getActualController().joystickAxisChanged( joystickEvent ) ;			
	else
	{
		OSDL_JOYSTICK_LOG( "Axis changed for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + " : "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}
			
}


void Joystick::trackballChanged( const JoystickTrackballEvent & joystickEvent )
	throw()
{
	
	if ( isLinkedToController() )
		getActualController().joystickTrackballChanged( joystickEvent ) ;		
	else
	{
		OSDL_JOYSTICK_LOG( "Trackball changed for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + " : "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}
			
}


void Joystick::hatChanged( const JoystickHatEvent & joystickEvent ) throw()
{
	
	if ( isLinkedToController() )
		getActualController().joystickHatChanged( joystickEvent ) ;			
	else
	{
		OSDL_JOYSTICK_LOG( "Hat changed for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + " : "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}		
			
}


void Joystick::buttonPressed( const JoystickButtonEvent & joystickEvent )
	throw()
{
	
	if ( isLinkedToController() )
		getActualController().joystickButtonPressed( joystickEvent ) ;			
	else
	{
		OSDL_JOYSTICK_LOG( "Button pressed for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + " : "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}
		
}


void Joystick::buttonReleased( const JoystickButtonEvent & joystickEvent )
	throw()
{
	
	if ( isLinkedToController() )
		getActualController().joystickButtonReleased( joystickEvent ) ;			
	else
	{
		OSDL_JOYSTICK_LOG( "Button released for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + " : "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}		
			
}


JoystickAxesCount Joystick::getNumberOfAxes() const throw()
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getNumberOfAxes : joystick not open." ) ;	
#endif // OSDL_DEBUG

	return _axisCount ;
	
}


JoystickTrackballsCount Joystick::getNumberOfTrackballs() const throw()
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getNumberOfTrackballs : joystick not open." ) ;	
#endif // OSDL_DEBUG

	return _trackballCount ;
	
}


JoystickHatsCount Joystick::getNumberOfHats() const throw()
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getNumberOfHats : joystick not open." ) ;	
#endif // OSDL_DEBUG
	
	return _hatCount ;
	
}


JoystickButtonsCount Joystick::getNumberOfButtons() const throw()
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getNumberOfButtons : joystick not open." ) ;	
#endif // OSDL_DEBUG
	
	return _buttonCount ;
	
}


AxisPosition Joystick::getAbscissaPosition() const throw() 
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getAbscissaPosition : joystick not open." ) ;
		
	if ( _axisCount == 0 )
		Ceylan::emergencyShutdown( 
			"Joystick::getAbscissaPosition : no X axe available." ) ;
	
#endif // OSDL_DEBUG
	
	return SDL_JoystickGetAxis( _internalJoystick, 0 ) ;
	
}


AxisPosition Joystick::getOrdinatePosition() const throw() 
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getOrdinatePosition : joystick not open." ) ;
		
	if ( _axisCount <= 1 )
		Ceylan::emergencyShutdown( 
			"Joystick::getOrdinatePosition : no Y axe available." ) ;
	
#endif // OSDL_DEBUG

	return SDL_JoystickGetAxis( _internalJoystick, 1 ) ;
	
}


AxisPosition Joystick::getPositionOfAxis( JoystickAxesCount index ) 
	const throw( JoystickException )
{

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::getPositionOfAxis : joystick not open." ) ;
	
	if ( index >= _axisCount )
		throw JoystickException( "Joystick::getPositionOfAxis : axe index ( " 
			+ Ceylan::toString( index ) + ") out of bounds." ) ;
		
	return SDL_JoystickGetAxis( _internalJoystick, index ) ;
	
}


HatPosition Joystick::getPositionOfHat( JoystickHatsCount index ) 
	const throw( JoystickException )
{

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::getPositionOfHat : joystick not open." ) ;
	
	if ( index >= _hatCount )
		throw JoystickException( "Joystick::getPositionOfHat : hat index ( " 
			+ Ceylan::toString( index ) + ") out of bounds." ) ;
		
	return SDL_JoystickGetHat( _internalJoystick, index ) ;
	
}


bool Joystick::isButtonPressed( JoystickButtonsCount buttonNumber ) 
	const throw( JoystickException )
{

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::isButtonPressed : joystick not open." ) ;
	
	if ( buttonNumber >= _buttonCount )
		throw JoystickException( "Joystick::isButtonPressed : button number ( " 
			+ Ceylan::toString( buttonNumber ) + ") out of bounds." ) ;

	return ( SDL_JoystickGetButton( _internalJoystick, buttonNumber ) == 1 ) ;
	
}


bool Joystick::getPositionOfTrackball( JoystickTrackballsCount ball, 
	BallMotion & deltaX, BallMotion & deltaY ) const throw( JoystickException )				
{

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::getPositionOfTrackball : joystick not open." ) ;
	
	if ( ball >= _trackballCount )
		throw JoystickException( 
			"Joystick::getPositionOfTrackball : trackball number ( " 
			+ Ceylan::toString( _trackballCount ) + ") out of bounds." ) ;
			
 	return ( SDL_JoystickGetBall( 
		_internalJoystick, ball, & deltaX, & deltaY ) == 0 ) ;

}

	
JoystickNumber Joystick::getIndex() const throw()
{

	if ( _internalJoystick == 0 )
		Ceylan::emergencyShutdown( 
			"Joystick::getIndex : no internal joystick registered." ) ;
	
	JoystickNumber index = SDL_JoystickIndex( _internalJoystick ) ;
	
#if OSDL_DEBUG

	if ( index != _index ) 
		LogPlug::error( "Joystick::getIndex : conflicting status : "
			"back-end (makes authority) says index is " 
			+ Ceylan::toString( index ) 
			+ " whereas internal index says " 
			+ Ceylan::toString( _index ) + "." ) ;	
					
#endif // OSDL_DEBUG

	return index ;
	
}


void Joystick::update() throw()
{

	_axisCount      = SDL_JoystickNumAxes(    _internalJoystick ) ;
	_trackballCount = SDL_JoystickNumBalls(   _internalJoystick ) ;
	_hatCount       = SDL_JoystickNumHats(    _internalJoystick ) ;
	_buttonCount    = SDL_JoystickNumButtons( _internalJoystick ) ;
	
}


const string Joystick::toString( Ceylan::VerbosityLevels level ) const throw()
{

	if ( ! isOpen() )
		return "Non-opened joystick whose index is #" 
			+ Ceylan::toString( _index ) 
			+ ", named '" + _name + "'";
		
	return "Opened joystick whose index is #" + Ceylan::toString( _index ) 
		+ ", named '" + _name + "'. This joystick has "
		+ Ceylan::toString( _axisCount ) + " axis, " 
		+ Ceylan::toString( _trackballCount ) + " trackball(s), "
		+ Ceylan::toString( _hatCount ) + " hat(s), and "
		+ Ceylan::toString( _buttonCount ) + " button(s)" ;
				
}


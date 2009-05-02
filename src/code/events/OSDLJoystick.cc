/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
 *
 * This file is part of the OSDL library.
 *
 * The OSDL library is free software: you can redistribute it and/or modify
 * it under the terms of either the GNU Lesser General Public License or
 * the GNU General Public License, as they are published by the Free Software
 * Foundation, either version 3 of these Licenses, or (at your option) 
 * any later version.
 *
 * The OSDL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License and the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License and of the GNU General Public License along with the OSDL library.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Olivier Boudeville (olivier.boudeville@esperide.com)
 *
 */


#include "OSDLJoystick.h"       

#include "OSDLController.h"     // for joystickAxisChanged, etc.


using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL::Events ;
using namespace OSDL::MVC ;     // for joystickAxisChanged, etc.


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"         // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS





#if OSDL_VERBOSE_JOYSTICK

#include <iostream>
#define OSDL_JOYSTICK_LOG( message ) std::cout << "[OSDL Joystick] " << message << std::endl ;

#else // OSDL_VERBOSE_JOYSTICK

#define OSDL_JOYSTICK_LOG( message )

#endif // OSDL_VERBOSE_JOYSTICK



/** 
 * Dummy values will be returned by non-static methods in case there is no
 * SDL support available.
 *
 * It is not a problem as these methods cannot be called: constructors always
 * throw exceptions in that case, thus no instance can be available for these
 * method calls.
 *
 */ 


/*
 * Not used currently since event loop is prefered to polling:
 *   - SDL_JoystickUpdate
 *	 - SDL_JoystickEventState (used in OSDLJoysticksHandler)
 *
 */


Joystick::Joystick( JoystickNumber index ) :
	OSDL::Events::InputDevice(),
#if OSDL_USES_SDL
	_name( SDL_JoystickName( index ) ),
#else // OSDL_USES_SDL
	_name(),
#endif // OSDL_USES_SDL
	_index( index ),
	_internalJoystick( 0 ),
	_axisCount( 0 ),
	_trackballCount( 0 ),
	_hatCount( 0 ),
	_buttonCount( 0 )
{

#if OSDL_USES_SDL

	if ( _name.empty() )
		_name = "(unknown joystick)" ;

#else // OSDL_USES_SDL

	throw JoystickException( "Joystick constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
	
}



Joystick::~Joystick() throw()
{

	if ( isOpen() )
		close() ;
		
	/*
	 * Does not seem to be owned:
	 *
	if ( _internalJoystick != 0 )	
		::free( _internalJoystick ) ;
	 *
	 */
	 	
}



const string & Joystick::getName() const
{

	return _name ;
	
}



bool Joystick::isOpen() const
{

#if OSDL_USES_SDL
	
	bool isOpened = ( SDL_JoystickOpened( _index ) == 1 ) ;
	
#if OSDL_DEBUG
	if ( isOpened != ( _internalJoystick != 0 ) ) 
		LogPlug::error( "Joystick::isOpen: conflicting status: "
			"back-end (makes authority) says " + Ceylan::toString( isOpened ) 
			+ " whereas internal status says " 
			+ Ceylan::toString( ( _internalJoystick != 0 ) )
			+ "." ) ;			
#endif // OSDL_DEBUG 
	
	return isOpened ;

#else // OSDL_USES_SDL

	return false ;

#endif // OSDL_USES_SDL
	
}



void Joystick::open()
{

#if OSDL_USES_SDL

	if ( isOpen() )
		throw JoystickException( 
			"Joystick::open requested whereas was already open." ) ;
		
	_internalJoystick = SDL_JoystickOpen( _index ) ;
	
	update() ;
	
#endif // OSDL_USES_SDL
	
}

 
 
void Joystick::close()
{

#if OSDL_USES_SDL

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::close requested whereas was not open." ) ;
		
	SDL_JoystickClose( _internalJoystick ) ;
	
	// Should not be a memory leak, according to SDL doc:
	_internalJoystick = 0 ;

#endif // OSDL_USES_SDL
	
}



void Joystick::axisChanged( const JoystickAxisEvent & joystickEvent )
{
	
	if ( isLinkedToController() )
		getActualController().joystickAxisChanged( joystickEvent ) ;			
	else
	{
		OSDL_JOYSTICK_LOG( "Axis changed for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}
			
}



void Joystick::trackballChanged( const JoystickTrackballEvent & joystickEvent )
{
	
	if ( isLinkedToController() )
		getActualController().joystickTrackballChanged( joystickEvent ) ;		
	else
	{
		OSDL_JOYSTICK_LOG( "Trackball changed for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}
			
}



void Joystick::hatChanged( const JoystickHatEvent & joystickEvent )
{
	
	if ( isLinkedToController() )
		getActualController().joystickHatChanged( joystickEvent ) ;			
	else
	{
		OSDL_JOYSTICK_LOG( "Hat changed for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}		
			
}



void Joystick::buttonPressed( const JoystickButtonEvent & joystickEvent )
{
	
	if ( isLinkedToController() )
		getActualController().joystickButtonPressed( joystickEvent ) ;			
	else
	{
		OSDL_JOYSTICK_LOG( "Button pressed for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}
		
}



void Joystick::buttonReleased( const JoystickButtonEvent & joystickEvent )
{
	
	if ( isLinkedToController() )
		getActualController().joystickButtonReleased( joystickEvent ) ;			
	else
	{
		OSDL_JOYSTICK_LOG( "Button released for joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}		
			
}



JoystickAxesCount Joystick::getNumberOfAxes() const
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getNumberOfAxes: joystick not open." ) ;	
#endif // OSDL_DEBUG

	return _axisCount ;
	
}



JoystickTrackballsCount Joystick::getNumberOfTrackballs() const
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getNumberOfTrackballs: joystick not open." ) ;	
#endif // OSDL_DEBUG

	return _trackballCount ;
	
}



JoystickHatsCount Joystick::getNumberOfHats() const
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getNumberOfHats: joystick not open." ) ;	
#endif // OSDL_DEBUG
	
	return _hatCount ;
	
}



JoystickButtonsCount Joystick::getNumberOfButtons() const
{

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getNumberOfButtons: joystick not open." ) ;	
#endif // OSDL_DEBUG
	
	return _buttonCount ;
	
}



AxisPosition Joystick::getAbscissaPosition() const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getAbscissaPosition: joystick not open." ) ;
		
	if ( _axisCount == 0 )
		Ceylan::emergencyShutdown( 
			"Joystick::getAbscissaPosition: no X axe available." ) ;
	
#endif // OSDL_DEBUG
	
	return SDL_JoystickGetAxis( _internalJoystick, 0 ) ;

#else // OSDL_USES_SDL

	return 0 ;

#endif // OSDL_USES_SDL
	
}



AxisPosition Joystick::getOrdinatePosition() const 
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	if ( ! isOpen() )
		Ceylan::emergencyShutdown( 
			"Joystick::getOrdinatePosition: joystick not open." ) ;
		
	if ( _axisCount <= 1 )
		Ceylan::emergencyShutdown( 
			"Joystick::getOrdinatePosition: no Y axe available." ) ;
	
#endif // OSDL_DEBUG

	return SDL_JoystickGetAxis( _internalJoystick, 1 ) ;
	
#else // OSDL_USES_SDL

	return 0 ;

#endif // OSDL_USES_SDL

}



AxisPosition Joystick::getPositionOfAxis( JoystickAxesCount index ) const
{

#if OSDL_USES_SDL

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::getPositionOfAxis: joystick not open." ) ;
	
	if ( index >= _axisCount )
		throw JoystickException( "Joystick::getPositionOfAxis: axe index ( " 
			+ Ceylan::toString( index ) + ") out of bounds." ) ;
		
	return SDL_JoystickGetAxis( _internalJoystick, index ) ;

#else // OSDL_USES_SDL

	return 0 ;

#endif // OSDL_USES_SDL
	
}



HatPosition Joystick::getPositionOfHat( JoystickHatsCount index ) const
{

#if OSDL_USES_SDL

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::getPositionOfHat: joystick not open." ) ;
	
	if ( index >= _hatCount )
		throw JoystickException( "Joystick::getPositionOfHat: hat index ( " 
			+ Ceylan::toString( index ) + ") out of bounds." ) ;
		
	return SDL_JoystickGetHat( _internalJoystick, index ) ;

#else // OSDL_USES_SDL

	return 0 ;

#endif // OSDL_USES_SDL
	
}



bool Joystick::isButtonPressed( JoystickButtonsCount buttonNumber ) const
{

#if OSDL_USES_SDL

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::isButtonPressed: joystick not open." ) ;
	
	if ( buttonNumber >= _buttonCount )
		throw JoystickException( "Joystick::isButtonPressed: button number ( " 
			+ Ceylan::toString( buttonNumber ) + ") out of bounds." ) ;

	return ( SDL_JoystickGetButton( _internalJoystick, buttonNumber ) == 1 ) ;

#else // OSDL_USES_SDL

	return 0 ;

#endif // OSDL_USES_SDL
	
}



bool Joystick::getPositionOfTrackball( JoystickTrackballsCount ball, 
	BallMotion & deltaX, BallMotion & deltaY ) const				
{

#if OSDL_USES_SDL

	if ( ! isOpen() )
		throw JoystickException( 
			"Joystick::getPositionOfTrackball: joystick not open." ) ;
	
	if ( ball >= _trackballCount )
		throw JoystickException( 
			"Joystick::getPositionOfTrackball: trackball number ( " 
			+ Ceylan::toString( _trackballCount ) + ") out of bounds." ) ;
			
 	return ( SDL_JoystickGetBall( 
		_internalJoystick, ball, & deltaX, & deltaY ) == 0 ) ;

#else // OSDL_USES_SDL

	return 0 ;

#endif // OSDL_USES_SDL

}


	
JoystickNumber Joystick::getIndex() const
{

#if OSDL_USES_SDL

	if ( _internalJoystick == 0 )
		Ceylan::emergencyShutdown( 
			"Joystick::getIndex: no internal joystick registered." ) ;
	
	JoystickNumber index = SDL_JoystickIndex( _internalJoystick ) ;
	
#if OSDL_DEBUG

	if ( index != _index ) 
		LogPlug::error( "Joystick::getIndex: conflicting status: "
			"back-end (makes authority) says index is " 
			+ Ceylan::toString( index ) 
			+ " whereas internal index says " 
			+ Ceylan::toString( _index ) + "." ) ;	
					
#endif // OSDL_DEBUG

	return index ;

#else // OSDL_USES_SDL

	return 0 ;

#endif // OSDL_USES_SDL
	
}



void Joystick::update()
{

#if OSDL_USES_SDL

	_axisCount      = SDL_JoystickNumAxes(    _internalJoystick ) ;
	_trackballCount = SDL_JoystickNumBalls(   _internalJoystick ) ;
	_hatCount       = SDL_JoystickNumHats(    _internalJoystick ) ;
	_buttonCount    = SDL_JoystickNumButtons( _internalJoystick ) ;

#endif // OSDL_USES_SDL
	
}



const string Joystick::toString( Ceylan::VerbosityLevels level ) const
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


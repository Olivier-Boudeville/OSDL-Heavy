/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#include "OSDLJoystickHandler.h"

#include "OSDLJoystick.h"           // for Joystick
#include "OSDLClassicalJoystick.h"  // for ClassicalJoystick
#include "OSDLController.h"         // for axisChanged

#include "OSDLUtils.h"              // for getBackendLastError
#include "OSDLBasic.h"              // for CommonModule


#include "Ceylan.h"                 // for Ceylan logs



using std::string ;
using std::list ;

using namespace Ceylan::Log ;

using namespace OSDL::Events ;



const string DebugPrefix = " " ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>             // for OSDL_VERBOSE_JOYSTICK_HANDLER and al
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS



#if OSDL_USES_SDL

#include "SDL.h"                // for SDL_JoystickEventState, etc.

#endif // OSDL_USES_SDL



#if OSDL_VERBOSE_JOYSTICK_HANDLER

#include <iostream>
#define OSDL_JOYSTICK_HANDLER_LOG( message ) std::cout << "[OSDL Joystick Handler] " << message << std::endl ;

#else // OSDL_VERBOSE_JOYSTICK_HANDLER

#define OSDL_JOYSTICK_HANDLER_LOG( message )

#endif // OSDL_VERBOSE_JOYSTICK_HANDLER





JoystickHandler::JoystickHandler( bool useClassicalJoysticks ) :
	InputDeviceHandler(),
	_joystickCount( 0 ),
	_joysticks( 0 ),
	_useClassicalJoysticks( useClassicalJoysticks )
{

#if OSDL_USES_SDL

	send( "Initializing joystick subsystem." ) ;

	if ( SDL_InitSubSystem( CommonModule::UseVideo ) 
			!= CommonModule::BackendSuccess )
		throw JoystickException( "JoystickHandler constructor: "
			"unable to initialize joystick subsystem: " 
			+ Utils::getBackendLastError() ) ;

	// We want joystick changes to be translated into events:
	SDL_JoystickEventState( SDL_ENABLE ) ;
	
	update() ;

	send( "Joystick subsystem initialized." ) ;

	dropIdentifier() ;
	
#else // OSDL_USES_SDL

	throw InputDeviceHandlerException( "JoystickHandler constructor failed:"
		"no SDL support available" ) ;
	
#endif // OSDL_USES_SDL
	
}



JoystickHandler::~JoystickHandler() throw()
{

#if OSDL_USES_SDL

	send( "Stopping joystick subsystem." ) ;

	// Joysticks instances are owned by the handler:
	blank() ;
	 
	SDL_QuitSubSystem( CommonModule::UseJoystick ) ;
	
	send( "Joystick subsystem stopped." ) ;

#endif // OSDL_USES_SDL

}



void JoystickHandler::update() 
{

	/*
	 * Joystick instances have to be deleted so that the 
	 * _useClassicalJoysticks value is taken into account 
	 * (it might have changed since last call).
	 *
	 */ 
	blank() ;

	
	// Here the joystick array must be created.
	_joystickCount = GetAvailableJoystickCount() ;

	send( Ceylan::toString( _joystickCount ) + " joystick(s) auto-detected." ) ;
	
	/*
	 * This array, faster than a list, will contain _joystickCount pointers 
	 * to Joystick instances:
	 *
	 */
	_joysticks = new Joystick *[ _joystickCount ] ;
	
	for ( JoystickNumber i = 0; i < _joystickCount; i++ )
		if ( _useClassicalJoysticks )
			_joysticks[i] = new ClassicalJoystick( i ) ;
		else
			_joysticks[i] = new Joystick( i ) ;
	
}



void JoystickHandler::openJoystick( JoystickNumber index ) 
{

	/*
	 * Beware, joystick list might be out of synch, test is not exactly
	 * equivalent to _joystickCount.
	 *
	 */	
	if ( index >= GetAvailableJoystickCount() )
		throw JoystickException( "JoystickHandler::openJoystick: index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( GetAvailableJoystickCount() )
			+ " joystick(s) attached)." ) ;
			
	if ( index >= _joystickCount )
		throw JoystickException( "JoystickHandler::openJoystick: index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( _joystickCount )
			+ " joystick(s) attached according to internal joystick list)." ) ;
	
#if OSDL_DEBUG
	if ( _joysticks[ index ] == 0 )
		throw JoystickException( "JoystickHandler::openJoystick: "
			"no known joystick for index " + Ceylan::toString( index ) + "." ) ;
#endif // OSDL_DEBUG
	
	if ( ! _joysticks[ index ]->isOpen() )
		_joysticks[ index ]->open() ;
						
}



void JoystickHandler::linkToController( JoystickNumber index, 
	OSDL::MVC::Controller & controller )
{

	/*
	 * Beware, joystick list might be out of synch, test is not exactly
	 * equivalent to _joystickCount.
	 *
	 */	
	if ( index >= static_cast<JoystickNumber>( GetAvailableJoystickCount() ) )
		throw JoystickException( "JoystickHandler::linkToController: index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( GetAvailableJoystickCount() )
			+ " joystick(s) attached)." ) ;
			
	if ( index >= _joystickCount )
		throw JoystickException( "JoystickHandler::linkToController: index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( _joystickCount )
			+ " joystick(s) attached according to internal joystick list)." ) ;
	
#if OSDL_DEBUG
	if ( _joysticks[ index ] == 0 )
		throw JoystickException( "JoystickHandler::linkToController: "
			"no known joystick for index " + Ceylan::toString( index ) + "." ) ;
#endif // OSDL_DEBUG

	_joysticks[ index ]->setController( controller ) ;
	
}
	


const string JoystickHandler::toString( Ceylan::VerbosityLevels level ) const
{

	string res ;
	
	if ( _joystickCount > 0 )
		res = "Joystick handler managing " 
		+ Ceylan::toString( _joystickCount ) + " joystick(s)" ;
	else
		return "Joystick handler does not manage any joystick" ;

	if ( level == Ceylan::low ) 	
		return res ;
	
	res += ". Listing detected joystick(s): " ;
		
	list<string> joysticks ;
		
	for ( JoystickNumber i = 0; i < _joystickCount; i++ )
		if ( _joysticks[i] != 0 )
			joysticks.push_back( _joysticks[i]->toString( level ) ) ;
		else
			joysticks.push_back( "no joystick at index " 
				+ Ceylan::toString( i ) + " (abnormal)" ) ;
		
	return res + Ceylan::formatStringList( joysticks ) ;
		
}



JoystickNumber JoystickHandler::GetAvailableJoystickCount()
{

#if OSDL_USES_SDL

	return static_cast<JoystickNumber>( SDL_NumJoysticks() ) ;

#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL
	
}




// Protected section.



void JoystickHandler::axisChanged( const JoystickAxisEvent & joystickEvent )
	const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->axisChanged( joystickEvent ) ;

#endif // OSDL_USES_SDL
	
}



void JoystickHandler::trackballChanged( 
	const JoystickTrackballEvent & joystickEvent ) const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->trackballChanged( joystickEvent ) ;

#endif // OSDL_USES_SDL	
	
}



void JoystickHandler::hatChanged( const JoystickHatEvent & joystickEvent ) const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->hatChanged( joystickEvent ) ;

#endif // OSDL_USES_SDL	
	
}



void JoystickHandler::buttonPressed( const JoystickButtonEvent & joystickEvent )
	const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->buttonPressed( joystickEvent ) ;
	
#endif // OSDL_USES_SDL	
	
}



void JoystickHandler::buttonReleased( 
	const JoystickButtonEvent & joystickEvent ) const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->buttonReleased( joystickEvent ) ;
	
#endif // OSDL_USES_SDL	
	
}



void JoystickHandler::blank()
{

	if ( _joysticks != 0 )
	{

		for ( JoystickNumber c = 0; c < _joystickCount; c++ )
		{
			delete _joysticks[c] ;
		}

		delete [] _joysticks ;
		_joysticks = 0 ;
	}
	
	_joystickCount = 0 ;
	
}



void JoystickHandler::checkJoystickAt( JoystickNumber index ) const
{

	if ( index >= _joystickCount )
		Ceylan::emergencyShutdown( 
			"JoystickHandler::checkJoystickAt: index "
			+ Ceylan::toNumericalString( index ) 
			+ " out of bounds (maximum value is "
			+ Ceylan::toNumericalString( _joystickCount - 1 ) + ")." ) ; 
		
	if ( _joysticks[ index ] == 0 )
		Ceylan::emergencyShutdown( "JoystickHandler::checkJoystickAt: "
			"no joystick intance at index "
			+ Ceylan::toNumericalString( index ) + "." ) ;

	if ( ! _joysticks[ index ]->isOpen() )
		Ceylan::emergencyShutdown( 
			"JoystickHandler::checkJoystickAt: joystick at index "
			+ Ceylan::toNumericalString( index ) + " was not open." ) ; 
	
}


/* 
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#include "OSDLClassicalJoystick.h"       

#include "OSDLController.h"    // for joystickAxisChanged, etc.


using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL::Events ;



const AxisPosition ClassicalJoystick::DefaultDeadZoneExtent = 100 ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>        // for OSDL_VERBOSE_JOYSTICK and al 
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS



#if OSDL_USES_SDL

#include "SDL.h"         // for CD-ROM primitives

#endif // OSDL_USES_SDL



#if OSDL_VERBOSE_JOYSTICK

#include <iostream>
#define OSDL_JOYSTICK_LOG( message ) std::cout << "[OSDL Joystick] " << message << std::endl ;

#else // OSDL_VERBOSE_JOYSTICK

#define OSDL_JOYSTICK_LOG( message )

#endif // OSDL_VERBOSE_JOYSTICK




/*
 * Not used currently since event loop is prefered to polling:
 *   - SDL_JoystickUpdate
 *	 - SDL_JoystickEventState (used in OSDLJoysticksHandler)
 *
 */
 


ClassicalJoystick::ClassicalJoystick( 
		JoystickNumber index, AxisPosition deadZoneExtent ) :
	Joystick( index ),
	_deadZoneExtentFirstAxis( deadZoneExtent ),
	_deadZoneExtentSecondAxis( deadZoneExtent )
{

#if ! OSDL_USES_SDL

	throw JoystickException( "ClassicalJoystick constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



ClassicalJoystick::~ClassicalJoystick() throw()
{
		 	
}



void ClassicalJoystick::getDeadZoneValues( AxisPosition & firstAxisExtent, 
	AxisPosition & secondAxisExtent ) const
{

	firstAxisExtent  = _deadZoneExtentFirstAxis ;
	secondAxisExtent = _deadZoneExtentSecondAxis ;
	
}
	
	
					
void ClassicalJoystick::setDeadZoneValues( AxisPosition firstAxisExtent, 
	AxisPosition secondAxisExtent )
{

	_deadZoneExtentFirstAxis  = firstAxisExtent ;
	_deadZoneExtentSecondAxis = secondAxisExtent ;
	
}

	

const string ClassicalJoystick::toString( Ceylan::VerbosityLevels level ) const
{

	return "Classical joystick: " + Joystick::toString( level )
		+ ". The first axis deadzone extent is "   
		+ Ceylan::toString( _deadZoneExtentFirstAxis ) 
		+ ", the second axis deadzone extent is " 
		+ Ceylan::toString( _deadZoneExtentSecondAxis ) ;
				
}





// Protected section.



void ClassicalJoystick::axisChanged( const JoystickAxisEvent & joystickEvent )
{
	
#if OSDL_USES_SDL

	if ( isLinkedToController() )
	{
	
		// If in deadzone, do not notify the controller.
		
		if ( joystickEvent.axis == 0 )
		{
			if ( joystickEvent.value > _deadZoneExtentFirstAxis )
				getActualController().joystickRight( joystickEvent.value ) ;
			else
			{
				// -1 offset to avoid overflow (range: -32768 to 32767)
				AxisPosition pos = -1 - joystickEvent.value ;
				if ( pos > _deadZoneExtentFirstAxis )
					getActualController().joystickLeft( pos ) ;
			}	
			
			return ;		
		}
		
		
		if ( joystickEvent.axis == 1 )
		{
			if ( joystickEvent.value > _deadZoneExtentSecondAxis )
				getActualController().joystickDown( joystickEvent.value ) ;
			else
			{
				// -1 offset to avoid overflow (range: -32768 to 32767)
				AxisPosition pos = -1 - joystickEvent.value ;
				if ( pos > _deadZoneExtentSecondAxis )
					getActualController().joystickUp( pos ) ;
			}	
			
			return ;		
		}
			
	}		
	else
	{
		OSDL_JOYSTICK_LOG( 
			"Axis changed for controller-less classical joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}
		
#endif // OSDL_USES_SDL

}



void ClassicalJoystick::buttonPressed( 
	const JoystickButtonEvent & joystickEvent )
{
	
#if OSDL_USES_SDL

	if ( isLinkedToController() )
	{
	
		switch( joystickEvent.button )
		{
		
			case 0:
				getActualController().joystickFirstButtonPressed() ;
				break ;
				
			case 1:
				getActualController().joystickSecondButtonPressed() ;
				break ;
			
			default:
				OSDL_JOYSTICK_LOG( 
					"Button (neither first or second, hence ignored) "
					"pressed for classical joystick #" 
					+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
					+ EventsModule::DescribeEvent( joystickEvent ) ) ;
				break ;
		}
		
		
	}	
	else
	{
		OSDL_JOYSTICK_LOG( 
			"Button pressed for controller-less classical joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}

#endif // OSDL_USES_SDL
			
}



void ClassicalJoystick::buttonReleased( 
	const JoystickButtonEvent & joystickEvent )
{
	
#if OSDL_USES_SDL

	if ( isLinkedToController() )
	{
	
		switch( joystickEvent.button )
		{
		
			case 0:
				getActualController().joystickFirstButtonReleased() ;
				break ;
				
			case 1:
				getActualController().joystickSecondButtonReleased() ;
				break ;
			
			default:
				OSDL_JOYSTICK_LOG( 
					"Button (neither first or second, hence ignored) "
					"released for classical joystick #" 
					+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
					+ EventsModule::DescribeEvent( joystickEvent ) ) ;
				break ;
		}
		
	}	
	else
	{
	
		OSDL_JOYSTICK_LOG( 
			"Button released for controller-less classical joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + ": "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
	}

#endif // OSDL_USES_SDL
			
}


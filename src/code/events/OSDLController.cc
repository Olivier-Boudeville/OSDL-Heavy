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


#include "OSDLController.h"       

#include "OSDLEvents.h"       // for KeyPressed


using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL::Events ;  // for EventsModule, etc.
using namespace OSDL::MVC ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>       // for OSDL_DEBUG and al (private header)
#endif // OSDL_USES_CONFIG_H



#if OSDL_VERBOSE_CONTROLLER

#include <iostream>
#define OSDL_CONTROLLER_LOG( message ) std::cout << "[OSDL controller] " << message << std::endl ;

#else // OSDL_VERBOSE_CONTROLLER

#define OSDL_CONTROLLER_LOG( message )

#endif // OSDL_VERBOSE_CONTROLLER




Controller::Controller() :
	Ceylan::Controller()
{

}


Controller::Controller( Ceylan::Model & model ) :
	Ceylan::Controller( model )
{

}


Controller::~Controller() throw()
{
	 	
}




// Keyboard section.



void Controller::keyboardFocusGained( const FocusEvent & keyboardFocusEvent )
{
	
#if OSDL_DEBUG

	if ( keyboardFocusEvent.type != EventsModule::ApplicationFocusChanged )
		Ceylan::emergencyShutdown( "Controller::keyboardFocusGained: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: keyboard focus gained: " 
		+ EventsModule::DescribeEvent( keyboardFocusEvent ) ) ;
		
}



void Controller::keyboardFocusLost( const FocusEvent & keyboardFocusEvent )
{
	
#if OSDL_DEBUG

	if ( keyboardFocusEvent.type != EventsModule::ApplicationFocusChanged )
		Ceylan::emergencyShutdown( "Controller::keyboardFocusLost: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: keyboard focus lost: " 
		+ EventsModule::DescribeEvent( keyboardFocusEvent ) ) ;
		
}



void Controller::rawKeyPressed( const KeyboardEvent & keyboardPressedEvent )
{

#if OSDL_DEBUG

	if ( keyboardPressedEvent.type != EventsModule::KeyPressed )
		Ceylan::emergencyShutdown( "Controller::rawKeyPressed: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: raw key pressed: " 
		+ EventsModule::DescribeEvent( keyboardPressedEvent ) ) ;
		
}



void Controller::rawKeyReleased( const KeyboardEvent & keyboardReleasedEvent )
{

#if OSDL_DEBUG

	if ( keyboardReleasedEvent.type != EventsModule::KeyReleased )
		Ceylan::emergencyShutdown( "Controller::rawKeyReleased: "
			"unexpected event received instead." ) ;		
			
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: raw key released: " 
		+ EventsModule::DescribeEvent( keyboardReleasedEvent ) ) ;
		
}



void Controller::unicodeSelected( const KeyboardEvent & keyboardPressedEvent )
{

#if OSDL_DEBUG

	if ( keyboardPressedEvent.type != EventsModule::KeyPressed )
		Ceylan::emergencyShutdown( "Controller::unicodeSelected: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: unicode selected: " 
		+ EventsModule::DescribeEvent( keyboardPressedEvent ) ) ;
		
}




// Mouse section.



void Controller::mouseFocusGained( const FocusEvent & mouseFocusEvent )
{
	
#if OSDL_DEBUG

	if ( mouseFocusEvent.type != EventsModule::ApplicationFocusChanged )
		Ceylan::emergencyShutdown( "Controller::mouseFocusGained: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: mouse focus gained: " 
		+ EventsModule::DescribeEvent( mouseFocusEvent ) ) ;
		
}
		


void Controller::mouseFocusLost( const FocusEvent & mouseFocusEvent )
{
	
#if OSDL_DEBUG

	if ( mouseFocusEvent.type != EventsModule::ApplicationFocusChanged )
		Ceylan::emergencyShutdown( "Controller::mouseFocusLost: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: mouse focus lost: " 
		+ EventsModule::DescribeEvent( mouseFocusEvent ) ) ;
		
}



void Controller::mouseMoved( const MouseMotionEvent & mouseMotionEvent ) 
{

#if OSDL_DEBUG

	if ( mouseMotionEvent.type != EventsModule::MouseMoved )
		Ceylan::emergencyShutdown( "Controller::mouseMoved: "
			"unexpected event received instead." ) ;	
				
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: mouseMoved: " 
		+ EventsModule::DescribeEvent( mouseMotionEvent ) ) ;
		
}



void Controller::mouseButtonPressed( 
	const MouseButtonEvent & mouseButtonPressedEvent )
{

#if OSDL_DEBUG

	if ( mouseButtonPressedEvent.type != EventsModule::MouseButtonPressed )
		Ceylan::emergencyShutdown( "Controller::mouseButtonPressed: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: mouseButtonPressed: " 
		+ EventsModule::DescribeEvent( mouseButtonPressedEvent ) ) ;
		
}

					
					
void Controller::mouseButtonReleased( 
	const MouseButtonEvent & mouseButtonReleasedEvent )					
{

#if OSDL_DEBUG

	if ( mouseButtonReleasedEvent.type != EventsModule::MouseButtonReleased )
		Ceylan::emergencyShutdown( "Controller::mouseButtonReleased: "
			"unexpected event received instead." ) ;		
			
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: mouseButtonReleased: " 
		+ EventsModule::DescribeEvent( mouseButtonReleasedEvent ) ) ;
		
}
	
	
	
void Controller::joystickAxisChanged( 
	const JoystickAxisEvent & joystickAxisEvent )
{

#if OSDL_DEBUG

	if ( joystickAxisEvent.type != EventsModule::JoystickAxisChanged )
		Ceylan::emergencyShutdown( "Controller::joystickAxisChanged: "
			"unexpected event received instead." ) ;	
				
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: joystickAxisChanged: " 
		+ EventsModule::DescribeEvent( joystickAxisEvent ) ) ;
		
}
					
	
					
void Controller::joystickTrackballChanged( 
	const JoystickTrackballEvent & joystickTrackballEvent )
{

#if OSDL_DEBUG

	if ( joystickTrackballEvent.type != EventsModule::JoystickTrackballChanged )
		Ceylan::emergencyShutdown( "Controller::joystickTrackballChanged: "
			"unexpected event received instead." ) ;	
				
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: joystickTrackballChanged: " 
		+ EventsModule::DescribeEvent( joystickTrackballEvent ) ) ;
		
}
		
	
					
void Controller::joystickHatChanged( 
	const JoystickHatEvent & joystickHatChangedEvent )
{

#if OSDL_DEBUG

	if ( joystickHatChangedEvent.type !=
			EventsModule::JoystickHatPositionChanged )
		Ceylan::emergencyShutdown( "Controller::joystickHatPositionChanged: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: joystickHatPositionChanged: " 
		+ EventsModule::DescribeEvent( joystickHatChangedEvent ) ) ;
		
}
					
	
					
void Controller::joystickButtonPressed( 
	const JoystickButtonEvent & joystickButtonPressedEvent )
{

#if OSDL_DEBUG

	if ( joystickButtonPressedEvent.type != 
			EventsModule::JoystickButtonPressed )
		Ceylan::emergencyShutdown( "Controller::joystickButtonPressed: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: joystickButtonPressed: " 
		+ EventsModule::DescribeEvent( joystickButtonPressedEvent ) ) ;
		
}
					
	
					
void Controller::joystickButtonReleased( 
	const JoystickButtonEvent & joystickButtonReleasedEvent )
{

#if OSDL_DEBUG

	if ( joystickButtonReleasedEvent.type !=
			EventsModule::JoystickButtonReleased )
		Ceylan::emergencyShutdown( "Controller::joystickButtonReleased: "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller: joystickButtonReleased: " 
		+ EventsModule::DescribeEvent( joystickButtonReleasedEvent ) ) ;
		
}
	
	
																				
void Controller::joystickLeft( AxisPosition leftExtent )
{

	OSDL_CONTROLLER_LOG( "Controller::joystickLeft: extent is " 
		<< leftExtent ) ; 

}



void Controller::joystickRight( AxisPosition rightExtent )
{

	OSDL_CONTROLLER_LOG( "Controller::joystickRight: extent is " 
		<< rightExtent ) ; 

}



void Controller::joystickUp( AxisPosition upExtent )
{

	OSDL_CONTROLLER_LOG( "Controller::joystickUp: extent is " 
		<< upExtent ) ; 

}



void Controller::joystickDown( AxisPosition downExtent )
{

	OSDL_CONTROLLER_LOG( "Controller::joystickDown: extent is " 
		<< downExtent ) ; 

}



void Controller::joystickFirstButtonPressed()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickFirstButtonPressed"  ) ; 

}



void Controller::joystickFirstButtonReleased()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickFirstButtonReleased" ) ; 

}



void Controller::joystickSecondButtonPressed() 
{

	OSDL_CONTROLLER_LOG( "Controller::joystickSecondButtonPressed" ) ; 

}



void Controller::joystickSecondButtonReleased()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickSecondButtonReleased" ) ; 

}


const Ceylan::Event & Controller::getEventFor( 
	const Ceylan::CallerEventListener & listener )
{

	throw EventsException( "Controller::getEventFor is not expected "
		"to be called." ) ;
	
}

	
const string Controller::toString( Ceylan::VerbosityLevels level ) const
{

	return "OSDL controller: " + Ceylan::Controller::toString( level ) ;		
	
}


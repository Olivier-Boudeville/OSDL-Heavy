#include "OSDLController.h"       

#include "OSDLEvents.h"       // for KeyPressed

#include "SDL.h"


#include "OSDLConfig.h"       // for OSDL_DEBUG and al (private header)


using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL::Events ;  // for EventsModule, etc.
using namespace OSDL::MVC ;



#ifdef OSDL_VERBOSE_CONTROLLER

#include <iostream>
#define OSDL_CONTROLLER_LOG( message ) std::cout << "[OSDL controller] " << message << std::endl ;

#else // OSDL_VERBOSE_CONTROLLER

#define OSDL_CONTROLLER_LOG( message )

#endif // OSDL_VERBOSE_CONTROLLER



Controller::Controller() throw() :
	Ceylan::Controller()
{

}


Controller::Controller( Ceylan::Model & model ) throw() :
	Ceylan::Controller( model )
{

}


Controller::~Controller() throw()
{
	 	
}


void Controller::rawKeyPressed( const KeyboardEvent & keyboardPressedEvent )
	throw()
{

#if OSDL_DEBUG

	if ( keyboardPressedEvent.type != EventsModule::KeyPressed )
		Ceylan::emergencyShutdown( "Controller::rawKeyPressed : "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : raw key pressed : " 
		+ EventsModule::DescribeEvent( keyboardPressedEvent ) ) ;
		
}


void Controller::rawKeyReleased( const KeyboardEvent & keyboardReleasedEvent )
	throw()
{

#if OSDL_DEBUG

	if ( keyboardReleasedEvent.type != EventsModule::KeyReleased )
		Ceylan::emergencyShutdown( "Controller::rawKeyReleased : "
			"unexpected event received instead." ) ;		
			
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : raw key released : " 
		+ EventsModule::DescribeEvent( keyboardReleasedEvent ) ) ;
		
}


void Controller::unicodeSelected( const KeyboardEvent & keyboardPressedEvent )
	throw()
{

#if OSDL_DEBUG

	if ( keyboardPressedEvent.type != EventsModule::KeyPressed )
		Ceylan::emergencyShutdown( "Controller::unicodeSelected : "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : unicode selected : " 
		+ EventsModule::DescribeEvent( keyboardPressedEvent ) ) ;
		
}



void Controller::mouseMoved( const MouseMotionEvent & mouseMotionEvent ) 
	throw()
{

#if OSDL_DEBUG

	if ( mouseMotionEvent.type != EventsModule::MouseMoved )
		Ceylan::emergencyShutdown( "Controller::mouseMoved : "
			"unexpected event received instead." ) ;	
				
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : mouseMoved : " 
		+ EventsModule::DescribeEvent( mouseMotionEvent ) ) ;
		
}


void Controller::mouseButtonPressed( 
	const MouseButtonEvent & mouseButtonPressedEvent ) throw()
{

#if OSDL_DEBUG

	if ( mouseButtonPressedEvent.type != EventsModule::MouseButtonPressed )
		Ceylan::emergencyShutdown( "Controller::mouseButtonPressed : "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : mouseButtonPressed : " 
		+ EventsModule::DescribeEvent( mouseButtonPressedEvent ) ) ;
		
}
					
					
void Controller::mouseButtonReleased( 
	const MouseButtonEvent & mouseButtonReleasedEvent ) throw()					
{

#if OSDL_DEBUG

	if ( mouseButtonReleasedEvent.type != EventsModule::MouseButtonReleased )
		Ceylan::emergencyShutdown( "Controller::mouseButtonReleased : "
			"unexpected event received instead." ) ;		
			
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : mouseButtonReleased : " 
		+ EventsModule::DescribeEvent( mouseButtonReleasedEvent ) ) ;
		
}
	
	
	
	
void Controller::joystickAxisChanged( 
	const JoystickAxisEvent & joystickAxisEvent ) throw()
{

#if OSDL_DEBUG

	if ( joystickAxisEvent.type != EventsModule::JoystickAxisChanged )
		Ceylan::emergencyShutdown( "Controller::joystickAxisChanged : "
			"unexpected event received instead." ) ;	
				
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : joystickAxisChanged : " 
		+ EventsModule::DescribeEvent( joystickAxisEvent ) ) ;
		
}
					
					
void Controller::joystickTrackballChanged( 
	const JoystickTrackballEvent & joystickTrackballEvent ) throw()
{

#if OSDL_DEBUG

	if ( joystickTrackballEvent.type != EventsModule::JoystickTrackballChanged )
		Ceylan::emergencyShutdown( "Controller::joystickTrackballChanged : "
			"unexpected event received instead." ) ;	
				
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : joystickTrackballChanged : " 
		+ EventsModule::DescribeEvent( joystickTrackballEvent ) ) ;
		
}
		
					
void Controller::joystickHatChanged( 
	const JoystickHatEvent & joystickHatChangedEvent ) throw()
{

#if OSDL_DEBUG

	if ( joystickHatChangedEvent.type !=
			EventsModule::JoystickHatPositionChanged )
		Ceylan::emergencyShutdown( "Controller::joystickHatPositionChanged : "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : joystickHatPositionChanged : " 
		+ EventsModule::DescribeEvent( joystickHatChangedEvent ) ) ;
		
}
					
					
void Controller::joystickButtonPressed( 
	const JoystickButtonEvent & joystickButtonPressedEvent ) throw()
{

#if OSDL_DEBUG

	if ( joystickButtonPressedEvent.type != 
			EventsModule::JoystickButtonPressed )
		Ceylan::emergencyShutdown( "Controller::joystickButtonPressed : "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : joystickButtonPressed : " 
		+ EventsModule::DescribeEvent( joystickButtonPressedEvent ) ) ;
		
}
					
					
void Controller::joystickButtonReleased( 
	const JoystickButtonEvent & joystickButtonReleasedEvent ) throw()										
{

#if OSDL_DEBUG

	if ( joystickButtonReleasedEvent.type !=
			EventsModule::JoystickButtonReleased )
		Ceylan::emergencyShutdown( "Controller::joystickButtonReleased : "
			"unexpected event received instead." ) ;
					
#endif // OSDL_DEBUG
	
	OSDL_CONTROLLER_LOG( "OSDL controller : joystickButtonReleased : " 
		+ EventsModule::DescribeEvent( joystickButtonReleasedEvent ) ) ;
		
}
	
																				
void Controller::joystickLeft( AxisPosition leftExtent ) throw()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickLeft : extent is " 
		<< leftExtent ) ; 

}


void Controller::joystickRight( AxisPosition rightExtent ) throw()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickRight : extent is " 
		<< rightExtent ) ; 

}


void Controller::joystickUp( AxisPosition upExtent ) throw()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickUp : extent is " 
		<< upExtent ) ; 

}


void Controller::joystickDown( AxisPosition downExtent ) throw()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickDown : extent is " 
		<< downExtent ) ; 

}


void Controller::joystickFirstButtonPressed() throw()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickFirstButtonPressed"  ) ; 

}


void Controller::joystickFirstButtonReleased() throw()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickFirstButtonReleased" ) ; 

}


void Controller::joystickSecondButtonPressed() throw() 
{

	OSDL_CONTROLLER_LOG( "Controller::joystickSecondButtonPressed" ) ; 

}


void Controller::joystickSecondButtonReleased() throw()
{

	OSDL_CONTROLLER_LOG( "Controller::joystickSecondButtonReleased" ) ; 

}



const string Controller::toString( Ceylan::VerbosityLevels level ) const throw()
{

	return "OSDL controller : " + Ceylan::Controller::toString( level ) ;		
	
}


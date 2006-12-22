#include "OSDLClassicalJoystick.h"       

#include "OSDLController.h"    // for joystickAxisChanged, etc.


using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL::Events ;


const AxisPosition ClassicalJoystick::DefaultDeadZoneExtent = 100 ;


#ifdef OSDL_VERBOSE_JOYSTICK

#include <iostream>
#define OSDL_JOYSTICK_LOG( message ) std::cout << "[OSDL Joystick] " << message << std::endl ;

#else

#define OSDL_JOYSTICK_LOG( message )

#endif



/*
 * Not used currently since event loop is prefered to polling :
 *   - SDL_JoystickUpdate
 *	 - SDL_JoystickEventState (used in OSDLJoysticksHandler)
 *
 *
 */

ClassicalJoystick::ClassicalJoystick( JoystickNumber index, AxisPosition deadZoneExtent ) throw() :
	Joystick( index ),
	_deadZoneExtentFirstAxis( deadZoneExtent ),
	_deadZoneExtentSecondAxis( deadZoneExtent )
{

}


ClassicalJoystick::~ClassicalJoystick() throw()
{
		 	
}


void ClassicalJoystick::axisChanged( const JoystickAxisEvent & joystickEvent ) throw()
{
	
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
		OSDL_JOYSTICK_LOG( "Axis changed for classical joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + " : "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
			
}


void ClassicalJoystick::buttonPressed( const JoystickButtonEvent & joystickEvent ) throw()
{
	
	if ( isLinkedToController() )
	{
		if ( joystickEvent.button == 0 )
			getActualController().joystickFirstButtonPressed() ;
		else
			getActualController().joystickSecondButtonPressed() ;					
	}	
	else
		OSDL_JOYSTICK_LOG( "Button pressed for classical joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + " : "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
}


void ClassicalJoystick::buttonReleased( const JoystickButtonEvent & joystickEvent ) throw()
{
	
	if ( isLinkedToController() )
	{
		if ( joystickEvent.button == 0 )
			getActualController().joystickFirstButtonReleased() ;
		else
			getActualController().joystickSecondButtonReleased() ;					
	}	
	else
		OSDL_JOYSTICK_LOG( "Button released for classical joystick #" 
			+ Ceylan::toNumericalString( joystickEvent.which ) + " : "
			+ EventsModule::DescribeEvent( joystickEvent ) ) ;
}


void ClassicalJoystick::getDeadZoneValues( AxisPosition & firstAxisExtent, 
	AxisPosition & secondAxisExtent ) const throw()
{
	firstAxisExtent  = _deadZoneExtentFirstAxis ;
	secondAxisExtent = _deadZoneExtentSecondAxis ;
}
	
					
void ClassicalJoystick::setDeadZoneValues( AxisPosition firstAxisExtent, 
	AxisPosition secondAxisExtent )	throw()
{
	_deadZoneExtentFirstAxis  = firstAxisExtent ;
	_deadZoneExtentSecondAxis = secondAxisExtent ;
}
	

const string ClassicalJoystick::toString( Ceylan::VerbosityLevels level ) const throw()
{

	return "Classical joystick : " + Joystick::toString( level )
		+ ". The first axis deadzone extent is "   + Ceylan::toString( _deadZoneExtentFirstAxis ) 
		+ ", the second axis deadzone extent is " 
		+ Ceylan::toString( _deadZoneExtentSecondAxis ) ;
				
}

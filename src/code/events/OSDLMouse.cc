#include "OSDLMouse.h"       

#include "OSDLController.h"    // for mouseMoved, etc.
#include "OSDLMouseCommon.h"   // for MouseButtonNumber, etc.

#include "OSDLPoint2D.h"       // for Point2D

#include "SDL.h"               // for SDL_GetMouseState, etc.



using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL ;
using namespace OSDL::Events ;
using namespace OSDL::MVC ;     
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>       // for OSDL_DEBUG and al (private header)
#endif // OSDL_USES_CONFIG_H



#if OSDL_VERBOSE_MOUSE

#include <iostream>
#define OSDL_MOUSE_LOG( message ) std::cout << "[OSDL Mouse] " << message << std::endl ;

#else // OSDL_VERBOSE_MOUSE

#define OSDL_MOUSE_LOG( message )

#endif // OSDL_VERBOSE_MOUSE


// Default : 3 buttons, 1 wheel.
const MouseButtonNumber Mouse::DefaultButtonTotalNumber = 5 ;

// Default : 3 buttons.
const MouseButtonNumber Mouse::DefaultButtonActualNumber = 3 ;

// Default : 1 wheel.
const MouseWheelNumber Mouse::DefaultWheelNumber = 1 ;




Mouse::Mouse( MouseNumber index, bool classicalMouseMode ) throw() :
	OSDL::Events::InputDevice(),
	_index( index ),
	_inClassicalMode( classicalMouseMode ),
	_buttonTotalCount( DefaultButtonTotalNumber ),
	_lastRelativeAbscissa( 0 ),
	_lastRelativeOrdinate( 0 )
{

}


Mouse::~Mouse() throw()
{
	 	
}



// Cursor position.


Video::Coordinate Mouse::getCursorAbscissa() const throw()
{

	int x ;
	
	SDL_GetMouseState( &x, /* y */ 0 ) ;
	
	return static_cast<Video::Coordinate>( x ) ;
	
}


Video::Coordinate Mouse::getCursorOrdinate() const throw()
{

	int y ;
	
	SDL_GetMouseState( /* x */ 0, &y ) ;
	
	return static_cast<Video::Coordinate>( y ) ;
	 	
}


void Mouse::setCursorPosition( const Point2D & newPosition ) const throw()
{

	setCursorPosition( newPosition.getX(), newPosition.getY() ) ;
		
}


void Mouse::setCursorPosition( Coordinate x, Coordinate y ) const throw()
{

	SDL_WarpMouse( static_cast<Ceylan::Uint16>( x ), 
		static_cast<Ceylan::Uint16>( y ) ) ;
		
}



MouseButtonNumber Mouse::getNumberOfButtons() const throw()
{

	return DefaultButtonActualNumber ;
	
}


MouseWheelNumber Mouse::getNumberOfWheels() const throw()
{

	return DefaultWheelNumber ;
	
}




bool Mouse::isLeftButtonPressed() const throw()
{

	MouseButtonMask buttons = SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ;
	
	return static_cast<bool>( buttons & SDL_BUTTON( 1 ) ) ;
	
}


bool Mouse::isMiddleButtonPressed() const throw()
{

	MouseButtonMask buttons = SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ;
	
	return static_cast<bool>( buttons & SDL_BUTTON( 2 ) ) ;
	
}


bool Mouse::isRightButtonPressed() const throw()
{

	MouseButtonMask buttons = SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ;
	
	return static_cast<bool>( buttons & SDL_BUTTON( 3 ) ) ;
	
}


bool Mouse::isButtonPressed( MouseButtonNumber buttonNumber ) 
					const throw( MouseException )
{

	MouseButtonMask buttons = SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ;
	
	return static_cast<bool>( buttons & SDL_BUTTON( buttonNumber ) ) ;
	
}


MouseButtonMask Mouse::getButtonStates() const throw()
{

	return static_cast<MouseButtonMask>( 
		SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ) ;
	
}


void Mouse::update() throw()
{

	int abscissa, ordinate ;
	
	// Buttons state not stored :
	SDL_GetRelativeMouseState( &abscissa, &ordinate ) ;
	
	_lastRelativeAbscissa = static_cast<Video::Coordinate>( abscissa ) ;
	_lastRelativeOrdinate = static_cast<Video::Coordinate>( ordinate ) ;
	
}


const string Mouse::toString( Ceylan::VerbosityLevels level ) const throw()
{

	string res = "Mouse " ;
	
	if ( ! _inClassicalMode )
		res += "not in classical mode" ;
	else	
		res += "in classical mode" ;
		
	res += ", with a total of " 
		+ Ceylan::toNumericalString( _buttonTotalCount ) 
		+ " buttons (to each wheel correspond two buttons)" ;
		
		
	return res ;
				
}


bool Mouse::IsPressed( MouseButtonMask mask, MouseButtonNumber buttonToInspect )
	throw()
{

	return static_cast<bool>( mask && SDL_BUTTON( buttonToInspect ) ) ;
	
}




// Protected section.


void Mouse::focusGained( const FocusEvent & mouseFocusEvent ) throw()
{

	if ( isLinkedToController() )
		getActualController().mouseFocusGained( mouseFocusEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Focus gained for mouse #" 
			+ Ceylan::toNumericalString( DefaultMouse ) + " : "
			+ EventsModule::DescribeEvent( mouseFocusEvent ) ) ;
	}

}


void Mouse::focusLost( const FocusEvent & mouseFocusEvent ) throw()
{

	if ( isLinkedToController() )
		getActualController().mouseFocusLost( mouseFocusEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Focus lost for mouse #" 
			+ Ceylan::toNumericalString( DefaultMouse ) + " : "
			+ EventsModule::DescribeEvent( mouseFocusEvent ) ) ;
	}

}


void Mouse::mouseMoved( const MouseMotionEvent & mouseEvent ) throw()
{

	if ( isLinkedToController() )
		getActualController().mouseMoved( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Motion for mouse #" 
			+ Ceylan::toNumericalString( DefaultMouse ) + " : "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}

}

				
void Mouse::buttonPressed( const MouseButtonEvent & mouseEvent ) throw()
{

	if ( isLinkedToController() )
		getActualController().mouseButtonPressed( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Mouse button #" 
			+ Ceylan::toNumericalString( mouseEvent.button ) + " pressed : "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}

}

					
void Mouse::buttonReleased( const MouseButtonEvent & mouseEvent ) throw()
{

	if ( isLinkedToController() )
		getActualController().mouseButtonReleased( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Mouse button #" 
			+ Ceylan::toNumericalString( mouseEvent.button ) + " released : "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}

}
																			
					
MouseNumber Mouse::getIndex() const throw()
{

	return _index ;
	
}


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


#include "OSDLMouse.h"       

#include "OSDLController.h"    // for mouseMoved, etc.
#include "OSDLMouseCommon.h"   // for MouseButtonNumber, etc.

#include "OSDLPoint2D.h"       // for Point2D



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

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS



#if OSDL_USES_SDL

#include "SDL.h"                // for SDL_GetMouseState, etc.

#endif // OSDL_USES_SDL



#if OSDL_VERBOSE_MOUSE

#include <iostream>
#define OSDL_MOUSE_LOG( message ) std::cout << "[OSDL Mouse] " << message << std::endl ;

#else // OSDL_VERBOSE_MOUSE

#define OSDL_MOUSE_LOG( message )

#endif // OSDL_VERBOSE_MOUSE



// Default: 3 buttons, 1 wheel.
const MouseButtonNumber Mouse::DefaultButtonTotalNumber = 5 ;

// Default: 3 buttons.
const MouseButtonNumber Mouse::DefaultButtonActualNumber = 3 ;

// Default: 1 wheel.
const MouseWheelNumber Mouse::DefaultWheelNumber = 1 ;




Mouse::Mouse( MouseNumber index, bool classicalMouseMode ) :
	OSDL::Events::InputDevice(),
	_index( index ),
	_inClassicalMode( classicalMouseMode ),
	_buttonTotalCount( DefaultButtonTotalNumber ),
	_lastRelativeAbscissa( 0 ),
	_lastRelativeOrdinate( 0 )
{

#if ! OSDL_USES_SDL

	throw MouseException( "Mouse constructor failed:"
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

}



Mouse::~Mouse() throw()
{
	 	
}




// Cursor position.



Video::Coordinate Mouse::getCursorAbscissa() const
{

#if OSDL_USES_SDL

	int x ;
	
	SDL_GetMouseState( &x, /* y */ 0 ) ;
	
	return static_cast<Video::Coordinate>( x ) ;

#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL
	
}



Video::Coordinate Mouse::getCursorOrdinate() const
{

#if OSDL_USES_SDL

	int y ;
	
	SDL_GetMouseState( /* x */ 0, &y ) ;
	
	return static_cast<Video::Coordinate>( y ) ;

#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL
	 	
}




void Mouse::setCursorPosition( const Point2D & newPosition ) const
{

	setCursorPosition( newPosition.getX(), newPosition.getY() ) ;
		
}



void Mouse::setCursorPosition( Coordinate x, Coordinate y ) const
{

#if OSDL_USES_SDL

	SDL_WarpMouse( static_cast<Ceylan::Uint16>( x ), 
		static_cast<Ceylan::Uint16>( y ) ) ;

#endif // OSDL_USES_SDL
		
}



bool Mouse::getCursorVisibility() const
{

#if OSDL_USES_SDL

	return ( SDL_ShowCursor( SDL_QUERY ) == SDL_ENABLE ) ;

#endif // OSDL_USES_SDL
		
}



void Mouse::setCursorVisibility( bool on )
{

#if OSDL_USES_SDL

	if ( on )
		SDL_ShowCursor( SDL_ENABLE ) ;
	else
		SDL_ShowCursor( SDL_DISABLE ) ;

#endif // OSDL_USES_SDL
		
}



MouseButtonNumber Mouse::getNumberOfButtons() const
{

	return DefaultButtonActualNumber ;
	
}



MouseWheelNumber Mouse::getNumberOfWheels() const
{

	return DefaultWheelNumber ;
	
}




bool Mouse::isLeftButtonPressed() const
{

#if OSDL_USES_SDL

	MouseButtonMask buttons = SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ;
	
	return static_cast<bool>( buttons & SDL_BUTTON( 1 ) ) ;

#else // OSDL_USES_SDL

	return false ;
	
#endif // OSDL_USES_SDL
	
}



bool Mouse::isMiddleButtonPressed() const
{

#if OSDL_USES_SDL

	MouseButtonMask buttons = SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ;
	
	return ( ( buttons & SDL_BUTTON( 2 ) ) != 0 ) ;
	
#else // OSDL_USES_SDL

	return false ;
	
#endif // OSDL_USES_SDL
	
}



bool Mouse::isRightButtonPressed() const
{

#if OSDL_USES_SDL

	MouseButtonMask buttons = SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ;
	
	return ( ( buttons & SDL_BUTTON( 3 ) ) != 0 ) ;
	
#else // OSDL_USES_SDL

	return false ;
	
#endif // OSDL_USES_SDL
	
}



bool Mouse::isButtonPressed( MouseButtonNumber buttonNumber ) const 
{

#if OSDL_USES_SDL

	MouseButtonMask buttons = SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ;
	
	return ( ( buttons & SDL_BUTTON( buttonNumber ) ) != 0 ) ;
	
#else // OSDL_USES_SDL

	return false ;
	
#endif // OSDL_USES_SDL
	
}



MouseButtonMask Mouse::getButtonStates() const
{

#if OSDL_USES_SDL

	return static_cast<MouseButtonMask>( 
		SDL_GetMouseState( /* x */ 0, /* y */ 0 ) ) ;
	
#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL
	
}



void Mouse::update()
{

#if OSDL_USES_SDL

	int abscissa, ordinate ;
	
	// Buttons state not stored:
	SDL_GetRelativeMouseState( &abscissa, &ordinate ) ;
	
	_lastRelativeAbscissa = static_cast<Video::Coordinate>( abscissa ) ;
	_lastRelativeOrdinate = static_cast<Video::Coordinate>( ordinate ) ;
	
#endif // OSDL_USES_SDL
		
}



const string Mouse::toString( Ceylan::VerbosityLevels level ) const
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
{

#if OSDL_USES_SDL

	return static_cast<bool>( mask && SDL_BUTTON( buttonToInspect ) ) ;

#else // OSDL_USES_SDL

	return false ;
	
#endif // OSDL_USES_SDL
	
}




// Protected section.



void Mouse::focusGained( const FocusEvent & mouseFocusEvent )
{

	if ( isLinkedToController() )
		getActualController().mouseFocusGained( mouseFocusEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Focus gained for mouse #" 
			+ Ceylan::toNumericalString( DefaultMouse ) + ": "
			+ EventsModule::DescribeEvent( mouseFocusEvent ) ) ;
	}

}



void Mouse::focusLost( const FocusEvent & mouseFocusEvent )
{

	if ( isLinkedToController() )
		getActualController().mouseFocusLost( mouseFocusEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Focus lost for mouse #" 
			+ Ceylan::toNumericalString( DefaultMouse ) + ": "
			+ EventsModule::DescribeEvent( mouseFocusEvent ) ) ;
	}

}



void Mouse::mouseMoved( const MouseMotionEvent & mouseEvent )
{

	if ( isLinkedToController() )
		getActualController().mouseMoved( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Motion for mouse #" 
			+ Ceylan::toNumericalString( DefaultMouse ) + ": "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}

}

				
				
void Mouse::buttonPressed( const MouseButtonEvent & mouseEvent )
{

	if ( isLinkedToController() )
		getActualController().mouseButtonPressed( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Mouse button #" 
			+ Ceylan::toNumericalString( mouseEvent.button ) + " pressed: "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}

}

			
					
void Mouse::buttonReleased( const MouseButtonEvent & mouseEvent )
{

	if ( isLinkedToController() )
		getActualController().mouseButtonReleased( mouseEvent ) ;			
	else
	{
		OSDL_MOUSE_LOG( "Mouse button #" 
			+ Ceylan::toNumericalString( mouseEvent.button ) + " released: "
			+ EventsModule::DescribeEvent( mouseEvent ) ) ;
	}

}
																			
		
					
MouseNumber Mouse::getIndex() const
{

	return _index ;
	
}


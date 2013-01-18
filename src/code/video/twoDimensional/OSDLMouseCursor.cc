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


#include "OSDLMouseCursor.h"

#include "OSDLSurface.h"       // for Surface
#include "OSDLGLTexture.h"     // for GLTexture
#include "OSDLUtils.h"         // for getBackendLastError

#include "Ceylan.h"            // for LogPlug



using std::string ;

using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL 
#include "SDL.h"					 // for SDL_Cursor, etc.
#endif // OSDL_USES_SDL




MouseCursorException::MouseCursorException( const std::string & reason ) :
	VideoException( reason )
{

}

					
MouseCursorException::~MouseCursorException() throw()
{

}





MouseCursor::MouseCursor( Length width,	Length height,
		const Ceylan::Uint8 & data,	const Ceylan::Uint8 & mask,
		Coordinate hotSpotAbscissa,	Coordinate hotSpotOrdinate ) :
	_type( SystemCursor ),
	_systemCursor(),	
	_width( width ),
	_height( height ),
	_surface( 0 ),
	_texture( 0 ),
	_hotSpotAbscissa( hotSpotAbscissa ),
	_hotSpotOrdinate( hotSpotOrdinate )
{

#if OSDL_USES_SDL

	_systemCursor = SDL_CreateCursor( 
		const_cast<Ceylan::Uint8 *>( & data ), 
		const_cast<Ceylan::Uint8 *>( & mask ), 
		static_cast<int>( _width ), 
		static_cast<int>( _height ), 
		static_cast<int>( _hotSpotAbscissa ), 
		static_cast<int>( _hotSpotOrdinate ) ) ;
		
	if ( _systemCursor == 0 )
		throw MouseCursorException( 
			"MouseCursor constructor failed for system cursor: "
			+ Utils::getBackendLastError() ) ;

#else // OSDL_USES_SDL

	throw MouseCursorException( "MouseCursor constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
			
}



MouseCursor::MouseCursor( const Surface & cursorSurface,
		Coordinate hotSpotAbscissa,	Coordinate hotSpotOrdinate ) :
	_type( BlittedCursor ),
	_systemCursor( 0 ),	
	_width(),
	_height(),
	_surface( 0 ),
	_texture( 0 ),
	_hotSpotAbscissa( hotSpotAbscissa ),
	_hotSpotOrdinate( hotSpotOrdinate )
{

#if OSDL_USES_SDL

	// @fixme develop it for 2D and OpenGL case:
#if 0

	if ( Video::IsUsingOpenGL() )
	{
	
		/*
		 For keep, there should be an overall setting (as GetEndpointDrawState)
		 that tells whether precautions should be taken so that the OpenGL
		 context can be reloaded in case it is lost
		 
		 */
		 
		_texture = new GLTexture( sourceSurface, /* flavour */ None ) ;
	
	}

#endif // 0

#else // OSDL_USES_SDL

	throw MouseCursorException( "MouseCursor constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
	
}

	
	
MouseCursor::~MouseCursor() throw()
{

#if OSDL_USES_SDL

	if ( _systemCursor != 0 )
		SDL_FreeCursor( _systemCursor ) ;

#endif // OSDL_USES_SDL
	
	
	if ( _surface != 0 )
		delete _surface ;
		
	
	if ( _texture != 0 )	
		delete _texture ;
		
}



MouseCursor::CursorType MouseCursor::getType() const
{

	return _type ;
	
}



const string MouseCursor::toString( Ceylan::VerbosityLevels level) const
{

	string res = "MouseCursor corresponding to a " ;
	
	switch( _type )
	{
	
		case SystemCursor:
			res += "system cursor" ;
			break ;
	
		case BlittedCursor:
			res += "OSDL-blitted cursor" ;
			break ;
	
		case TexturedCursor:
			res += "OpenGL-rendered cursor" ;
			break ;
	
		default:
			res += "unknown cursor (abnormal)" ;
			break ;
	}
	
	
	return res ;
	
}


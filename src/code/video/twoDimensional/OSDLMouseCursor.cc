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



MouseCursorException::MouseCursorException( const std::string & reason ) 
		throw():
	VideoException( reason )
{

}

					
MouseCursorException::~MouseCursorException() throw()
{

}




MouseCursor::MouseCursor( Length width,	Length height,
	const Ceylan::Uint8 & data,	const Ceylan::Uint8 & mask,
	Coordinate hotSpotAbscissa,	Coordinate hotSpotOrdinate ) 
		throw( MouseCursorException ):
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
	Coordinate hotSpotAbscissa,	Coordinate hotSpotOrdinate )
		throw( MouseCursorException ):
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



MouseCursor::CursorType MouseCursor::getType() const throw()
{

	return _type ;
	
}


const string MouseCursor::toString( Ceylan::VerbosityLevels level) 
	const throw()
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


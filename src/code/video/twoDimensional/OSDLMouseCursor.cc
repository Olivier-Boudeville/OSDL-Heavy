#include "OSDLMouseCursor.h"

#include "OSDLSurface.h"       // for Surface
#include "OSDLGLTexture.h"     // for GLTexture
#include "OSDLUtils.h"         // for getBackendLastError

#include "Ceylan.h"            // for LogPlug

#include "SDL.h"               // for SDL_*Cursor



using std::string ;

using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;



MouseCursorException::MouseCursorException( const std::string & reason ) 
		throw() :
	VideoException( reason )
{

}

					
MouseCursorException::~MouseCursorException() throw()
{

}




MouseCursor::MouseCursor( Length width,	Length height,
	const Ceylan::Uint8 & data,	const Ceylan::Uint8 & mask,
	Coordinate hotSpotAbscissa,	Coordinate hotSpotOrdinate ) 
		throw( MouseCursorException ) :
	_type( SystemCursor ),
	_systemCursor(),	
	_width( width ),
	_height( height ),
	_surface( 0 ),
	_texture( 0 ),
	_hotSpotAbscissa( hotSpotAbscissa ),
	_hotSpotOrdinate( hotSpotOrdinate )
{

	_systemCursor = SDL_CreateCursor( 
		const_cast<Ceylan::Uint8 *>( & data ), 
		const_cast<Ceylan::Uint8 *>( & mask ), 
		static_cast<int>( _width ), 
		static_cast<int>( _height ), 
		static_cast<int>( _hotSpotAbscissa ), 
		static_cast<int>( _hotSpotOrdinate ) ) ;
		
	if ( _systemCursor == 0 )
		throw MouseCursorException( 
			"MouseCursor constructor failed for system cursor : "
			+ Utils::getBackendLastError() ) ;
			
}


MouseCursor::MouseCursor( const Surface & cursorSurface,
	Coordinate hotSpotAbscissa,	Coordinate hotSpotOrdinate )
		throw( MouseCursorException ) :
	_type( BlittedCursor ),
	_systemCursor( 0 ),	
	_width(),
	_height(),
	_surface( 0 ),
	_texture( 0 ),
	_hotSpotAbscissa( hotSpotAbscissa ),
	_hotSpotOrdinate( hotSpotOrdinate )
{

	// @fixme develop it for 2D and OpenGL case :
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

#endif	
	
}

	
MouseCursor::~MouseCursor() throw()
{

	if ( _systemCursor != 0 )
		SDL_FreeCursor( _systemCursor ) ;
	
	
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


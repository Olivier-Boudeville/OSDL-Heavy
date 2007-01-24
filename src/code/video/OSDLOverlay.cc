#include "OSDLOverlay.h"


#include "OSDLVideo.h"    // for VideoModule
#include "OSDLBasic.h"    // for CommonModule


#include "SDL.h"          // for SDL_Overlay



using namespace OSDL::Video ;

using std::string ;



OverlayException::OverlayException( const std::string & message ) throw() :
	VideoException( message )
{

}

	
OverlayException::~OverlayException() throw()
{

}

	
	
// Overlay section.

	
Overlay::Overlay( Length width, Length height, EncodingFormat format ) 
		throw( OverlayException ) :
	_overlay( 0 ),
	_width  ( 0 ),
	_height ( 0 )
{

	if ( ! OSDL::hasExistingCommonModule() )
		throw OverlayException( "Overlay constructor : "
			"no OSDL common module available." ) ;
		
	CommonModule & common = OSDL::getExistingCommonModule() ;

	if ( ! common.hasVideoModule() )
		throw OverlayException( "Overlay constructor : "
			"no OSDL video module available." ) ;
			
	_overlay = SDL_CreateYUVOverlay( width, height, format,
		& common.getVideoModule().getScreenSurface().getSDLSurface() ) ;

	if ( _overlay == 0 )
		throw OverlayException( "Overlay constructor : "
			"overlay instanciation failed." ) ;
		
}


Overlay::~Overlay() throw()
{

	if ( _overlay != 0 )
		SDL_FreeYUVOverlay( _overlay ) ;
		
}


bool Overlay::blit( Coordinate x, Coordinate y ) const throw( OverlayException )
{

	SDL_Rect destinationRect ;
	
	destinationRect.x = x ;
	destinationRect.y = y ;
	
	destinationRect.w = _width ;
	destinationRect.h = _height ;
	
	SDL_DisplayYUVOverlay( _overlay, & destinationRect ) ;
	
}


bool Overlay::blit() const throw( OverlayException )
{

}


bool Overlay::mustBeLocked() const throw()
{
	
	// @fixme
	return true ; 
	
}


void Overlay::preUnlock() throw()
{

	/*
	 * Lockable framework ensures it is called only if necessary 
	 * (i.e. only if 'must be locked') :
	 *
	 */
	SDL_UnlockYUVOverlay( _overlay ) ;
	
}


void Overlay::postLock() throw()
{

	/*
	 * Lockable framework ensures it is called only if necessary 
	 * (i.e. only if 'must be locked') :
	 *
	 */
	SDL_LockYUVOverlay( _overlay ) ;
	
}


const string Overlay::toString( Ceylan::VerbosityLevels level ) const throw()
{

	return "Overlay whose original size is width = " 
		+ Ceylan::toString( _width )
		+ ", height = " + Ceylan::toString( _height ) ;
		
}


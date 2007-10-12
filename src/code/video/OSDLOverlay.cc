#include "OSDLOverlay.h"


#include "OSDLVideo.h"    // for VideoModule
#include "OSDLBasic.h"    // for CommonModule
#include "OSDLUtils.h"    // for getBackendLastError



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>              // for OSDL_DEBUG and al (private header)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS



#if OSDL_USES_SDL

#include "SDL.h"                     // for SDL_Overlay, etc.

typedef SDL_Overlay LowLevelOverlay ;

#else // OSDL_USES_SDL

typedef struct LowLevelOverlay {} ;

#endif // OSDL_USES_SDL



// Note: overlay support is currently almost completely not tested.


using namespace OSDL::Video ;

using std::string ;



OverlayException::OverlayException( const std::string & message ) throw():
	VideoException( message )
{

}

	
OverlayException::~OverlayException() throw()
{

}

	
	
// Overlay section.

	
Overlay::Overlay( Length width, Length height, EncodingFormat format ) 
		throw( OverlayException ):
	_overlay( 0 ),
	_width  ( 0 ),
	_height ( 0 )
{

#if OSDL_USES_SDL

	if ( ! OSDL::hasExistingCommonModule() )
		throw OverlayException( "Overlay constructor: "
			"no OSDL common module available." ) ;
		
	CommonModule & common = OSDL::getExistingCommonModule() ;

	if ( ! common.hasVideoModule() )
		throw OverlayException( "Overlay constructor: "
			"no OSDL video module available." ) ;
			
	_overlay = SDL_CreateYUVOverlay( width, height, format,
		& common.getVideoModule().getScreenSurface().getSDLSurface() ) ;

	if ( _overlay == 0 )
		throw OverlayException( "Overlay constructor: "
			"overlay instanciation failed." ) ;
			
#else // OSDL_USES_SDL

	throw OverlayException( "Overlay constructor failed: "
		"no SDL support available." ) ;
		
#endif // OSDL_USES_SDL

}


Overlay::~Overlay() throw()
{

#if OSDL_USES_SDL

	if ( _overlay != 0 )
		SDL_FreeYUVOverlay( _overlay ) ;
		
#endif // OSDL_USES_SDL
		
}


void Overlay::blit( Coordinate x, Coordinate y ) const throw( OverlayException )
{

#if OSDL_USES_SDL

	SDL_Rect destinationRect ;
	
	destinationRect.x = x ;
	destinationRect.y = y ;
	
	destinationRect.w = _width ;
	destinationRect.h = _height ;
	
	if ( SDL_DisplayYUVOverlay( _overlay, & destinationRect ) != 0 )
		throw OverlayException( "Overlay::blit failed: "
			+ Utils::getBackendLastError() ) ;

#endif // OSDL_USES_SDL
	
}


void Overlay::blit() const throw( OverlayException )
{

	blit( 0, 0 ) ;

}


bool Overlay::mustBeLocked() const throw()
{
	
	// @fixme
	return true ; 
	
}


void Overlay::preUnlock() throw()
{

#if OSDL_USES_SDL

	/*
	 * Lockable framework ensures it is called only if necessary 
	 * (i.e. only if 'must be locked'):
	 *
	 */
	SDL_UnlockYUVOverlay( _overlay ) ;

#endif // OSDL_USES_SDL
	
}


void Overlay::postLock() throw()
{

#if OSDL_USES_SDL

	/*
	 * Lockable framework ensures it is called only if necessary 
	 * (i.e. only if 'must be locked'):
	 *
	 */
	SDL_LockYUVOverlay( _overlay ) ;

#endif // OSDL_USES_SDL
	
}


const string Overlay::toString( Ceylan::VerbosityLevels level ) const throw()
{

	return "Overlay whose original size is width = " 
		+ Ceylan::toString( _width )
		+ ", height = " + Ceylan::toString( _height ) ;
		
}


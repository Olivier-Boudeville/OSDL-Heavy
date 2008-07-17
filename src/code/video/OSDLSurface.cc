#include "OSDLSurface.h"

#include "OSDLTypes.h"				 // for Flags
#include "OSDLVideo.h"
#include "OSDLVideoTypes.h"          // for BitsPerPixel

#include "OSDLVideoTypes.h"          // for Length, etc.
#include "OSDLUprightRectangle.h"    // for UprightRectangle
#include "OSDLPoint2D.h"             // for Point2D
#include "OSDLLine.h"                // for Line: drawHorizontal, etc.
#include "OSDLFixedFont.h"           // for printBasic
#include "OSDLConic.h"               // for drawCircle, drawEllipse
#include "OSDLPixel.h"               // for getColorMasks, ColorMask, etc.
#include "OSDLPolygon.h"             // for drawPie
#include "OSDLWidget.h"              // for Widget
#include "OSDLUtils.h"               // for getBackendLastError

#include "Ceylan.h"                  // for Ceylan::Uint8, etc.


#include <cassert>                   // for assert


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>              // for OSDL_DEBUG_* and al 
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL_GFX

#include "SDL_rotozoom.h"            // for zoom, rotozoom

#endif // OSDL_USES_SDL_GFX


// To protect LoadImage:
#include "OSDLIncludeCorrecter.h"


/*
 * Always remember to access the width and the height of a Surface thanks 
 * to their dedicated methods (getWidth/getHeight) since the '_width' and
 * '_height' attributes, inherited from  UprightRectangle, are never 
 * updated: the values are to be read directly from the internal 
 * back-end surface.
 *
 */

using std::string ;
using std::list ;

using namespace Ceylan ;
using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;


using TwoDimensional::UprightRectangle ;



SurfaceEvent::SurfaceEvent( Ceylan::EventSource & source ) throw(): 
	Ceylan::Event( source )
{

}



SurfaceEvent::~SurfaceEvent() throw()
{

}




VideoMemoryLostException::VideoMemoryLostException( 
		const std::string & message ) throw():
	VideoException( message ) 
{

}



VideoMemoryLostException::VideoMemoryLostException() throw():
	VideoException( "Video memory lost, content needs to be reblitted" ) 
{

}
	
	
	
VideoMemoryLostException::~VideoMemoryLostException() throw()
{

}




/*
 * These flags can be used for all Surfaces.
 *
 * @note They are defined relatively as SDL back-end.
 *
 */
#if OSDL_USES_SDL_GFX


const Ceylan::Flags Surface::Software                = SDL_SWSURFACE   ;
const Ceylan::Flags Surface::Hardware                = SDL_HWSURFACE   ;
const Ceylan::Flags Surface::AsynchronousBlit        = SDL_ASYNCBLIT   ;
const Ceylan::Flags Surface::ExclusivePalette        = SDL_HWPALETTE   ;
const Ceylan::Flags Surface::HardwareAcceleratedBlit = SDL_HWACCEL     ;
const Ceylan::Flags Surface::ColorkeyBlit            = SDL_SRCCOLORKEY ;
const Ceylan::Flags Surface::RLEColorkeyBlit         = SDL_RLEACCEL    ;
const Ceylan::Flags Surface::AlphaBlendingBlit       = SDL_SRCALPHA    ;
const Ceylan::Flags Surface::Preallocated            = SDL_PREALLOC    ;

const Ceylan::Flags OpenGLBlit = SDL_OPENGLBLIT ;


// Private flag.
const Ceylan::Flags Surface::RLEColorkeyBlitAvailable = SDL_RLEACCELOK  ;


/*
 * These flags can only be used for display Surfaces.
 *
 * @note SDL_OPENGLBLIT has been deprecated.
 *
 */

const Ceylan::Flags Surface::AnyPixelFormat = SDL_ANYFORMAT  ;
const Ceylan::Flags Surface::DoubleBuffered = SDL_DOUBLEBUF  ;
const Ceylan::Flags Surface::FullScreen     = SDL_FULLSCREEN ;
const Ceylan::Flags Surface::OpenGL         = SDL_OPENGL     ;
const Ceylan::Flags Surface::Resizable      = SDL_RESIZABLE  ;


#else // OSDL_USES_SDL_GFX


// Same constants as SDL:

const Ceylan::Flags Surface::Software                = 0x00000000 ;
const Ceylan::Flags Surface::Hardware                = 0x00000001 ;
const Ceylan::Flags Surface::AsynchronousBlit        = 0x00000004 ;
const Ceylan::Flags Surface::ExclusivePalette        = 0x20000000 ;
const Ceylan::Flags Surface::HardwareAcceleratedBlit = 0x00000100 ;
const Ceylan::Flags Surface::ColorkeyBlit            = 0x00001000 ;
const Ceylan::Flags Surface::RLEColorkeyBlit         = 0x00004000 ;
const Ceylan::Flags Surface::AlphaBlendingBlit       = 0x00010000 ;
const Ceylan::Flags Surface::Preallocated            = 0x01000000 ;

const Ceylan::Flags OpenGLBlit = 0x0000000A ;


// Private flag.
const Ceylan::Flags Surface::RLEColorkeyBlitAvailable = 0x00002000 ;


/*
 * These flags can only be used for display Surfaces.
 *
 * @note SDL_OPENGLBLIT has been deprecated.
 *
 */

const Ceylan::Flags Surface::AnyPixelFormat = 0x10000000 ;
const Ceylan::Flags Surface::DoubleBuffered = 0x40000000 ;
const Ceylan::Flags Surface::FullScreen     = 0x80000000 ;
const Ceylan::Flags Surface::OpenGL         = 0x00000002 ;
const Ceylan::Flags Surface::Resizable      = 0x00000010 ;


#endif // OSDL_USES_SDL_GFX

	
	

const Length Surface::graphAbscissaOffset   = 10 ;
const Length Surface::graphOrdinateOffset   = 15 ;

const Length Surface::captionAbscissaOffset = 5  ;
const Length Surface::captionOrdinateOffset	= 10 ;
	
	
	
	
#ifdef OSDL_COUNT_INSTANCES

#define CHECKPOINT(message) Ceylan::checkpoint( message )

#else // OSDL_COUNT_INSTANCES

#define CHECKPOINT(message) 

#endif // OSDL_COUNT_INSTANCES
 

 
 
Surface::Surface( LowLevelSurface & surface, DisplayType displayType )
		throw( VideoException ): 
	UprightRectangle( 0, 0, 0, 0 ),
	EventSource(),
	Lockable(),
	_surface( & surface ), 
	_displayType( displayType ),
	_mustBeLocked( false ),
	_needsRedraw( true )  
{

#if OSDL_USES_SDL

#if OSDL_DEBUG_WIDGET

	CHECKPOINT( "Surface constructor from SDL surface." ) ; 
	
	LogPlug::trace( "Surface constructor from SDL surface." ) ;
	 
#endif // OSDL_DEBUG_WIDGET

#else // OSDL_USES_SDL

	throw VideoException( "Surface constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



Surface::Surface( Flags flags, Length width, Length height, BitsPerPixel depth,
	Pixels::ColorMask redMask, Pixels::ColorMask greenMask,
	Pixels::ColorMask blueMask, Pixels::ColorMask alphaMask ) 
			throw( VideoException ):
		UprightRectangle( 0, 0, 
			/* width and height not stored, computed from surface: */ 0, 0 ),
		EventSource(),
		Lockable(),
		_surface( 0 ),
		_displayType( BackBuffer ),
		_mustBeLocked( false ),
 		_needsRedraw( true )  
{


#if OSDL_USES_SDL

#if OSDL_DEBUG_WIDGET

	CHECKPOINT( "Surface constructor with full flags." ) ;
	
	LogPlug::trace( "Surface constructor with flags = "	
		+ Ceylan::toString( flags, /* bit field */ true ) 
		+ ", with width = "	      + Ceylan::toString( width ) 
		+ ", with height = "      + Ceylan::toString( height )
		+ ", with color depth = " + Ceylan::toNumericalString( depth ) 
		+ ", with specified red mask = "    + Ceylan::toHexString( redMask ) 
		+ ", with specified green mask = "  + Ceylan::toHexString( greenMask ) 
		+ ", with specified blue mask = "   + Ceylan::toHexString( blueMask ) 
		+ ", with specified alpha mask = "  + Ceylan::toHexString( alphaMask )	
		+ "." ) ; 
		
#endif // OSDL_DEBUG_WIDGET

	
	if ( alphaMask != 0 )
	{
		flags |= AlphaBlendingBlit ;
		// All other color masks have thus been specified.
	}	
	else 
	{
	
		// Here alpha mask is zero.
		
		/*
		 * All other masks are zero too ? 
		 * Choose them according to endianess, then:
		 *
		 */
		if ( redMask == 0 && greenMask == 0 && blueMask == 0 )
			Pixels::getRecommendedColorMasks( redMask, greenMask, 
				blueMask, alphaMask ) ;
		
		// No alpha-aware blitter for 8 bit per pixel blits:
		if ( depth == 8 )
			alphaMask = 0 ;
			
	}
		
#if OSDL_DEBUG_WIDGET

	LogPlug::trace( "Surface constructor: actual surface flags are " 
		+ Ceylan::toString( flags, /* bit field */ true ) 
		+ ", red mask is "   + Ceylan::toHexString( redMask )  
		+ ", green mask is " + Ceylan::toHexString( greenMask )  
		+ ", blue mask is "  + Ceylan::toHexString( blueMask ) 
		+ ", alpha mask is " + Ceylan::toHexString( alphaMask ) ) ;
		
#endif // OSDL_DEBUG_WIDGET
	
	_surface = SDL_CreateRGBSurface( flags, width, height, depth, 
		redMask, greenMask, blueMask, alphaMask ) ;
	
	if ( _surface == 0 )
		throw VideoException( "Blank surface constructor failed (width = " 
			+ Ceylan::toString( width ) 
			+ ", height = " + Ceylan::toString( height ) + "): " 
			+ Utils::getBackendLastError() ) ; 
		
			
#else // OSDL_USES_SDL

	throw VideoException( "Surface constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



/*
 * Protected Surface constructor only supposed to be called on very 
 * special cases.	
 *
 */			
Surface::Surface() throw( VideoException ):
	UprightRectangle( 0, 0, 0, 0 ),
	EventSource(),
	_surface( 0 ),
	_displayType( BackBuffer ),
	_mustBeLocked( false ),
	_needsRedraw( true )  
{

#if OSDL_USES_SDL
	
	// Empty surface.

#if OSDL_DEBUG_WIDGET
	CHECKPOINT( "Empty Surface constructor." ) ;
	LogPlug::trace( "Empty Surface constructor." ) ;
#endif // OSDL_DEBUG_WIDGET

#else // OSDL_USES_SDL

	throw VideoException( "Surface empty constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}			
	
	
						
Surface::~Surface() throw()
{

#if OSDL_DEBUG_WIDGET

	CHECKPOINT( "Surface destructor." ) ;
	LogPlug::trace( "Surface destructor, " 
		+ Ceylan::toString( _listeners.size() )	+ " widget(s) registered." ) ;
		
#endif // OSDL_DEBUG_WIDGET
	
	Ceylan::EventListener * widget ;
	
	// Do not use iterators for this delete task !
	
	/*
	 * Listeners will be removed one by one thanks to unsubscribeFrom calls:
	 * this is because it is rather unusual that sources detach themselves 
	 * for listeners, it is usually the opposite.
	 *
	 */
	
	while ( ! _listeners.empty() )
	{
	
		widget = _listeners.back() ;
		widget->unsubscribeFrom( *this ) ;
		
		/*
		 * Do not use: '_listeners.pop_back() ;' since this widget has 
		 * already been removed thanks to the call to unsubscribeFrom: it 
		 * would pop next listener !
		 *
		 */
		delete widget ;
		
	}
	
	// Never let invalid structures in the way:
	_listeners.clear() ;
	
	// Do not deallocate if screen surface:
	if ( _displayType == BackBuffer )
		flush() ;
	
}



Clonable & Surface::clone() const throw( ClonableException )
{
	
#if OSDL_USES_SDL
	
	if ( _surface != 0 )
	{
	
#ifdef OSDL_USE_DEPRECATED_CLONING
		
		// Previous implementation:
		
		// Retrieve the state of this source surface:
		Pixels::ColorMask redMask, greenMask, blueMask, alphaMask ;		
		Pixels::getCurrentColorMasks( getPixelFormat(), 
			redMask, greenMask, blueMask, alphaMask ) ;
		
		// Create a similar but blank surface:
		LowLevelSurface * copied = SDL_CreateRGBSurface( getFlags(), 
			getWidth(), getHeight(), getBitsPerPixel(), 
			redMask, greenMask, blueMask, alphaMask ) ;
		 
		/*
		 * Blit the source content onto it:
		 *
		 * "The blit function should not be called on a locked surface": 
		 * which one ?
		 * Supposing it is the target surface, 'copied', which is not locked.
		 *
		 */
		 
		int returned = SDL_BlitSurface( _surface, 0, copied, 0 ) ;
		if ( returned != 0 )
			throw ClonableException( "Surface::clone: blit failed (returned " 
				+ Ceylan::toString( returned ) + "): " 
				+ Utils::getBackendLastError() + "." ) ;
		
		
		/*
		 * Ensures that the clone has a colorkey if the original has it:
		 *
			if ( copied... )
				copied.setColorKey( [...] ) ;
		 * ... return * new Surface( * copied,...
		 */
		 
#endif // OSDL_USE_DEPRECATED_CLONING
		
		
		// Far better and simpler version:
		LowLevelSurface * copied = SDL_ConvertSurface( _surface, 
			& getPixelFormat(), getFlags() ) ;
		
		
		if ( copied == 0 )
			throw ClonableException( 
				"Surface::clone: creation of clone surface failed: " 
				+ Utils::getBackendLastError() + "." ) ;
				 
		return * new Surface( * copied, 
			/* clones cannot be a screen surface */ BackBuffer ) ;
		
	}
	else
	{
		LogPlug::warning( "Surface::clone: cloning a mostly blank surface." ) ;
		return * new Surface() ; 
	}	

#else // OSDL_USES_SDL

	throw ClonableException( "Surface::clone failed:"
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
		
}



LowLevelSurface & Surface::getSDLSurface() const throw()
{

	// addRef 	
	return * _surface ;
	
}



void Surface::setSDLSurface( LowLevelSurface & newSurface, 
	DisplayType displayType ) throw()
{
			
	// Free any previously held surface, and replace it with the provided one.
	flush() ;
			
	_surface = & newSurface ;
	_displayType = displayType ;
	
}



Surface::DisplayType Surface::getDisplayType() const throw()
{

	return _displayType ;
	
}



void Surface::setDisplayType( DisplayType newDisplayType ) throw()
{

	_displayType = newDisplayType ;
	
}



Ceylan::Flags Surface::getFlags() const throw()
{

#if OSDL_USES_SDL

	return _surface->flags ;
	
#else // OSDL_USES_SDL	

	return 0 ;

#endif // OSDL_USES_SDL

}



void Surface::setFlags( Flags newFlags ) throw()
{

#if OSDL_USES_SDL

	// Use with caution: internal LowLevelSurface not changed.
	_surface->flags = newFlags ;
	
#endif // OSDL_USES_SDL

}



void Surface::convertToDisplay( bool alphaChannelWanted ) 
	throw( VideoException )
{

#if OSDL_USES_SDL

	if ( _surface == 0 )
		throw VideoException( 
			"Surface::convertToDisplay: no available internal surface." ) ;
		
	LowLevelSurface * old = _surface ;
	
	if ( alphaChannelWanted )
		_surface = SDL_DisplayFormatAlpha( old ) ;
	else
		_surface = SDL_DisplayFormat( old ) ;

	SDL_FreeSurface( old ) ;
	
	if ( _surface == 0 )
		throw VideoException( 
			"Surface::convertToDisplay: conversion failed." ) ;		

#else // OSDL_USES_SDL	

	throw VideoException( "Surface::convertToDisplay failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
				
}



void Surface::setAlpha( Flags flags, Pixels::ColorElement newAlpha ) 
	throw( VideoException )
{

#if OSDL_USES_SDL

	if ( SDL_SetAlpha( _surface, flags, newAlpha ) != 0 )
		throw VideoException( "Surface::setAlpha failed: " 
			+ Utils::getBackendLastError() ) ;

#else // OSDL_USES_SDL	

	throw VideoException( "Surface::setAlpha failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
			 
}



ColorDefinition Surface::guessColorKeyDefinition() const throw( VideoException )
{

	return Pixels::convertPixelColorToColorDefinition( getPixelFormat(),
		guessColorKey() ) ;
		
}



PixelColor Surface::guessColorKey() const throw( VideoException )
{
	
	Length width  = getWidth() ;
	Length height = getHeight() ;
	
	if ( width == 0 || height == 0 )
		throw VideoException( "Surface::guessColorKey failed: "
			"surface too small" ) ;
			
	PixelColor firstCandidate = getPixelColorAt( 0, 0 ) ;
		
	PixelColor secondCandidate = getPixelColorAt( width-1, height-1 ) ;
		
	if ( Pixels::areEqual( firstCandidate, secondCandidate ) )
		return firstCandidate ;
	
	// First two different, third corner will discriminate:	
	
	PixelColor thirdCandidate = getPixelColorAt( width-1, 0 ) ;
	
			
	if ( Pixels::areEqual( firstCandidate, thirdCandidate ) )
		return firstCandidate ;

	if ( Pixels::areEqual( secondCandidate, thirdCandidate ) )
		return secondCandidate ;
		
	throw VideoException( "Surface::guessColorKey failed: "
		"three different colorkey candidates found" ) ;
		
	 			 
}



void Surface::setColorKey( Flags flags, Pixels::PixelColor keyPixelColor )
	throw( VideoException )
{

#if OSDL_USES_SDL

	if ( SDL_SetColorKey( _surface, flags, keyPixelColor ) != 0 )
		throw VideoException( "Surface::setColorKey (pixel color) failed: "
			+ Utils::getBackendLastError() ) ; 

#else // OSDL_USES_SDL	

	throw VideoException( "Surface::setColorKey failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
			 
			
}



void Surface::setColorKey( Flags flags, Pixels::ColorDefinition keyColorDef ) 
	throw( VideoException )
{

#if OSDL_USES_SDL

	Pixels::PixelColor keyPixelColor = 
		Pixels::convertColorDefinitionToPixelColor( getPixelFormat(),
			keyColorDef ) ;
		
	if ( SDL_SetColorKey( _surface, flags, keyPixelColor ) != 0 )
		throw VideoException( 
			"Surface::setColorKey (color definition) failed: "
			+ Utils::getBackendLastError() ) ; 

#else // OSDL_USES_SDL	

	throw VideoException( "Surface::setColorKey failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
			 
}



void Surface::convertFromColorKeyToAlphaChannel() throw( VideoException )
{
	
	if ( ( getFlags() & ColorkeyBlit ) == 0 )
		throw VideoException( "Surface::convertFromColorKeyToAlphaChannel: "
			"this surface does not use color key apparently: " 
			+ toString( Ceylan::low ) ) ;
			
	convertToDisplay( /* alphaChannelWanted */ true ) ;
			
}



Palette & Surface::getPalette() const throw( VideoException )
{

#if OSDL_USES_SDL

	if ( _surface->format->palette == 0 )
		throw VideoException( "Surface::getPalette failed: "
			"no available palette" ) ;
			
	LogPlug::debug( "Surface::getPalette: low-level palette found." ) ;
	
	return * new Palette( *_surface->format->palette ) ;		

#else // OSDL_USES_SDL	

	throw VideoException( "Surface::getPalette failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
		
}
					


bool Surface::setPalette( const Palette & newPalette, 
	ColorCount startingColorIndex, ColorCount numberOfColors, 
	Flags targetedPalettes ) throw( VideoException ) 
{

#if OSDL_USES_SDL

	if ( numberOfColors == 0 )
	{
	
		if ( newPalette.getNumberOfColors() > startingColorIndex )
		{
			numberOfColors = 
				newPalette.getNumberOfColors() - startingColorIndex ;
		}		
		else
		{
		
			throw VideoException( "Surface::setPalette failed: "
				"starting index out of bounds." ) ;

		}
			
	}
	else if ( startingColorIndex + numberOfColors >
		newPalette.getNumberOfColors() )
	{

			throw VideoException( "Surface::setPalette failed: "
				"too many color index, out of bounds." ) ;
				
	}	
	
	/*
	 * In video/SDL_video.c, SDL_SetPalette seems to copy palette, not take
	 * ownership of it:
	 *
	 */
	return ( SDL_SetPalette( _surface, targetedPalettes, 
		/* const */ newPalette.getColorDefinitions(), startingColorIndex, 
		numberOfColors ) == 1 ) ;

#else // OSDL_USES_SDL	

	throw VideoException( "Surface::setPalette failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
		
}
			

					
Surface & Surface::createColorReducedSurfaceFor( const Palette & palette, 
	bool manageColorkey ) const throw( VideoException )
{


	PixelColor sourceColorkey ;
	
	if ( manageColorkey )
		sourceColorkey = guessColorKey() ;

	// Creates a 8-bit software surface of the same size as this surface:
	Length targetWidth  = getWidth() ;
	Length targetHeight = getHeight() ;
	
	Surface * res = new Surface( Software, /* width */ targetWidth,
		/* height */ targetHeight, /* BitsPerPixel */ 8 ) ;
	
	res->setPalette( palette ) ;	
	
	PixelColor current ;
	ColorCount index ;
	
	// Scans source surface and chooses best index to put in target surface:
	for ( Length y = 0; y < targetHeight; y++ )
		for ( Length x = 0; x < targetWidth; x++ )
		{
		
			current = getPixelColorAt( x, y ) ;
			
			if ( manageColorkey && Pixels::areEqual( current, sourceColorkey ) )
			{
			
				index = palette.getColorKeyIndex() ;
			
			}
			else
			{	
			
				// First retrieves the color definition from this surface:
				ColorDefinition colorDef = 
							Pixels::convertPixelColorToColorDefinition( 
						getPixelFormat(), current ) ;
			
				// Then finds in palette the closest index:
				index = palette.getClosestColorIndexTo( colorDef ) ;
		
			}
			
			
			// Finally puts the chosen index on the current pixel:
			res->putPixelColorAt( x, y, static_cast<PixelColor>( index ),
				/* alpha */ AlphaOpaque, /* blending */ false, 
				/* clipping */ false, /* locking */ false ) ;
				
		}

	return *res ;	

}
	
					

Pixels::PixelFormat & Surface::getPixelFormat() const throw( VideoException )
{

#if OSDL_USES_SDL

	if ( _surface->format == 0 )
		throw VideoException( "Surface::getPixelFormat failed: "
			"no pixel format available." ) ;
	
	return * _surface->format ;

#else // OSDL_USES_SDL	

	throw VideoException( "Surface::getPixelFormat failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
	
}



void Surface::setPixelFormat( Pixels::PixelFormat & newFormat ) throw() 
{

#if OSDL_USES_SDL

	_surface->format = & newFormat ;

#endif // OSDL_USES_SDL

}



bool Surface::fill( Pixels::ColorDefinition colorDef ) throw()
{

#if OSDL_DEBUG_SURFACE

	LogPlug::trace( "Surface::fill in rectangular area from [" 
		+ Ceylan::toString( _x ) + ";" 
		+ Ceylan::toString( _y ) + "] to [" 
		+ Ceylan::toString( _x + getWidth() ) + ";" 
		+ Ceylan::toString( _y + getHeight() ) + "]" ) ;
		 
#endif // OSDL_DEBUG_SURFACE
	
	// Ignores clipping area:
	return drawBox( /* the surface *is* an UprightRectangle */ *this,
		colorDef, /* filled */ true ) ;
		
}



bool Surface::clear() throw( VideoException )
{

	if ( _displayType == OpenGLScreenSurface )
	{
	
		
		
	}
	
	
	return fill( /* clear color */ Pixels::Black ) ;
	
}



Surface & Surface::flipVertical() const throw( VideoException )
{    

#if OSDL_USES_SDL
	
	LowLevelSurface * result = SDL_CreateRGBSurface( 
		_surface->flags, 
		_surface->w, 
		_surface->h,
		_surface->format->BytesPerPixel * 8, 
		_surface->format->Rmask, 
		_surface->format->Gmask,
		_surface->format->Bmask,
		_surface->format->Amask ) ;
		 
	BytesPerPixel bpp = _surface->format->BytesPerPixel ;
	
	Ceylan::Uint16 scanline = _surface->pitch ;
	
	Ceylan::Uint8 * src  = 
		reinterpret_cast<Ceylan::Uint8 *>( _surface->pixels ) ;
		
	Ceylan::Uint8 * target =
		reinterpret_cast<Ceylan::Uint8 *>( result->pixels ) + scanline - bpp ;
  
  	const Coordinate height = result->h ;
  	const Coordinate width  = result->w ;
	
	/*
	 * Line by line, stores the copied pixel from right to left in target
	 * surface:
	 *
	 */
	
	for ( Coordinate y = 0; y < height; y++ )
    	for ( Coordinate x = 0; x < width; x++ )
			::memcpy( target + ( ( scanline * y ) - ( x * bpp ) ),
				src + ( ( scanline * y ) + ( x * bpp ) ), bpp ) ;
				
	return * new Surface( * result ) ;

#else //  OSDL_USES_SDL

	throw VideoException( "Surface::flipVertical failed: "
		"no SDL support available" ) ;
	
#endif //  OSDL_USES_SDL
		
}



Surface & Surface::flipHorizontal() const throw( VideoException )
{

#if OSDL_USES_SDL

	LowLevelSurface * result = SDL_CreateRGBSurface( 
		_surface->flags, 
		_surface->w, 
		_surface->h,
		_surface->format->BytesPerPixel * 8, 
		_surface->format->Rmask, 
		_surface->format->Gmask,
		_surface->format->Bmask, 
		_surface->format->Amask ) ;

	Ceylan::Uint16 scanline = _surface->pitch ;

	Ceylan::Uint8 * src    =
		reinterpret_cast<Ceylan::Uint8 *>( _surface->pixels ) ;
		
	Ceylan::Uint8 * target = 
		reinterpret_cast<Ceylan::Uint8 *>( result->pixels ) 
			+ result->pitch * ( result->h - 1 ) ;

  	const Coordinate height = result->h ;
  
  	// Changes simply the order of the lines:
	
	for ( Coordinate y = 0; y < height; y++ )
		::memcpy ( target - ( scanline * y ), 
			src + ( scanline * y ), scanline ) ;

	return * new Surface( * result ) ;

#else //  OSDL_USES_SDL

	throw VideoException( "Surface::flipHorizontal failed: "
		"no SDL support available" ) ;
	
#endif //  OSDL_USES_SDL
	
}



string Surface::describePixelAt( Coordinate x, Coordinate y ) throw()
{

	string res = "Pixel at [" + Ceylan::toString( x ) + ";" 
		+ Ceylan::toString( y ) + "]: " ;
	
	lock() ;
	PixelColor p = getPixelColorAt( x, y ) ;
	unlock() ;
	
	res += "pixel color is " + Ceylan::toHexString( p ) + ": " ;
	
	return res + Pixels::toString( p, getPixelFormat() ) ;
	
}



Pitch Surface::getPitch() const throw()
{

#if OSDL_USES_SDL

	return  _surface->pitch ;
	
#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL

}



void Surface::setPitch( Pitch newPitch ) throw() 
{

#if OSDL_USES_SDL

	_surface->pitch = newPitch ;
		
#endif // OSDL_USES_SDL
	
}



Length Surface::getWidth() const throw() 
{

#if OSDL_USES_SDL

	return _surface->w ;

#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL
	
}



void Surface::setWidth( Length newWidth ) throw()
{

	resize( newWidth, getHeight() ) ;
	
}



Length Surface::getHeight() const throw()
{

#if OSDL_USES_SDL

	return _surface->h ;

#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL
	
}



void Surface::setHeight( Length newHeight ) throw()
{

	resize( getWidth(), newHeight ) ;
	
}



void Surface::resize( Length newWidth, Length newHeight, bool scaleContent )
	throw( VideoException )	
{	


#if OSDL_USES_SDL
	
	if ( ( newWidth == getWidth() ) && ( newHeight == getHeight() ) )
		return ;
		
	// Retrieve the state of this current surface:
	Pixels::ColorMask redMask, greenMask, blueMask, alphaMask ;	
		
	Pixels::getCurrentColorMasks( getPixelFormat(), 
		redMask, greenMask, blueMask, alphaMask ) ;
		
	// Create a similar surface whose size is the requested one :
	LowLevelSurface * resized = SDL_CreateRGBSurface( getFlags(), 
		newWidth, newHeight, getBitsPerPixel(), 
			redMask, greenMask, blueMask, alphaMask ) ;
	
	// Cannot throw exception since inherited setWidth/Height methods cannot:
	if ( resized == 0 )
		Ceylan::emergencyShutdown( 
			"Surface::resize: creation of newer internal surface failed: "
			+ Utils::getBackendLastError() ) ; 

	
	// Needed to fix colorkey afterwards (copied exactly as is):
	Flags colorKeyFlags = getFlags() & ( ColorkeyBlit | RLEColorkeyBlit ) ;
	Pixels::PixelColor colorkey = _surface->format->colorkey ;
	
	// Fixing per-alpha settings:
	resized->format->alpha = _surface->format->alpha ;
	
	// @todo: fix other alpha settings (if any) and maybe lock as well ?
	
	Surface * zoomed ;
	
	if ( scaleContent )
	{
	
		try
		{
		
			zoomed = & zoom( 
				static_cast<Ceylan::Maths::Real>( newWidth )  / getWidth(),
				static_cast<Ceylan::Maths::Real>( newHeight ) / getHeight(),
				VideoModule::GetAntiAliasingState() ) ;
				
		}
		catch( const VideoException & e )
		{
			Ceylan::emergencyShutdown( 
				"Surface::resize: creation of scaled surface failed: "
				+ e.toString() ) ; 		
		}	
		
	}
	
	SDL_FreeSurface( _surface ) ;
	
	_surface = resized ;
	
	setColorKey( colorKeyFlags, colorkey ) ;
	
	if ( scaleContent )
	{
	
		try
		{
	
			zoomed->blitTo( * this ) ;
			delete zoomed ;

		}
		catch( const VideoException & e )
		{
			Ceylan::emergencyShutdown( 
				"Surface::resize: blit of scaled surface failed: "
				+ e.toString() ) ; 		
		}	
		
	}
	else
	{
		setRedrawState( true ) ;	
	}
	
#else //  OSDL_USES_SDL

	throw VideoException( "Surface::resize failed: "
		"no SDL support available" ) ;
	
#endif //  OSDL_USES_SDL
	
}



BitsPerPixel Surface::getBitsPerPixel() const throw()
{

#if OSDL_USES_SDL

	return _surface->format->BitsPerPixel ;
	
#else //  OSDL_USES_SDL

	return 0 ;

#endif //  OSDL_USES_SDL

}



void Surface::setBitsPerPixel( BitsPerPixel newBitsPerPixel ) throw() 
{

#if OSDL_USES_SDL

	_surface->format->BitsPerPixel = newBitsPerPixel ;
	
	// SDL surface's BytesPerPixel left untouched.

#endif //  OSDL_USES_SDL
	
}



BytesPerPixel Surface::getBytesPerPixel() const throw()
{

#if OSDL_USES_SDL

	return _surface->format->BytesPerPixel ;
	
#else //  OSDL_USES_SDL

	return 0 ;

#endif //  OSDL_USES_SDL
	
}



void Surface::setBytesPerPixel( BytesPerPixel newBytesPerPixel ) throw() 
{

#if OSDL_USES_SDL

	_surface->format->BytesPerPixel = newBytesPerPixel ;
	
	// SDL surface's BitsPerPixel left untouched.
	
#endif //  OSDL_USES_SDL
	
}



void * Surface::getPixels() const throw() 
{

#if OSDL_USES_SDL

	return _surface->pixels ;

#else //  OSDL_USES_SDL

	return 0 ;

#endif //  OSDL_USES_SDL
		
}



void Surface::setPixels( void * newPixels ) throw()
{

#if OSDL_USES_SDL

	_surface->pixels = newPixels ;	

#endif //  OSDL_USES_SDL

}





// Proxy methods section.


Pixels::PixelColor Surface::getPixelColorAt( Coordinate x, Coordinate y ) const 
	throw( VideoException )
{

	return Pixels::getPixelColor( *this, x, y ) ;
	  
}



Pixels::ColorDefinition Surface::getColorDefinitionAt( 
	Coordinate x, Coordinate y ) const throw( VideoException )
{

	return Pixels::convertPixelColorToColorDefinition( getPixelFormat(),
		Pixels::getPixelColor( *this, x, y ) ) ;  
		
}



void Surface::putRGBAPixelAt( Coordinate x, Coordinate y,
		ColorElement red, ColorElement green, ColorElement blue, 
		ColorElement alpha, 
		bool blending, bool clipping, bool locking ) throw( VideoException ) 
{

	Pixels::putRGBAPixel( *this, x, y, red, green, blue, alpha, 
		blending, clipping, locking ) ;
	
}



void Surface::putColorDefinitionAt( Coordinate x, Coordinate y,
		ColorDefinition colorDef, 
		bool blending, bool clipping, bool locking ) throw( VideoException ) 
{

	Pixels::putRGBAPixel( *this, x, y, colorDef.r, colorDef.g, colorDef.b,
		colorDef.unused, blending, clipping, locking ) ;
		
}



void Surface::putPixelColorAt( Coordinate x, Coordinate y,
		PixelColor convertedColor, ColorElement alpha,
		bool blending, bool clipping, bool locking ) throw( VideoException ) 
{

	Pixels::putPixelColor( *this, x, y, convertedColor, alpha, 
		blending, clipping, locking ) ;
		
}



bool Surface::setAlphaForColor( Pixels::ColorDefinition colorDef,
	Pixels::ColorElement newAlpha ) throw()
{
	
#if OSDL_DEBUG_COLOR

	LogPlug::trace( 
		"Surface::setAlphaForColor: scanning for color definition " 
		+ Pixels::toString( colorDef ) 
		+ ", so that its alpha coordinate gets replaced by "
		+ Ceylan::toNumericalString( newAlpha ) ) ;
		
#endif // OSDL_DEBUG_COLOR
		
	/*
	 * Useless if surface has no alpha coordinate (maybe the alpha mask 
	 * could be checked too):	
	 *
	 */
	if ( ( getFlags() & AlphaBlendingBlit ) == 0 )
		return false ;
	
	ColorDefinition currentDef ;
		
	lock() ;
	
	Length height = getHeight() ;
	Length width  = getWidth() ;
	
	for ( Coordinate y = 0; y < height; y++ )
		for ( Coordinate x = 0; x < width; x++ )
		{
				
			currentDef = getColorDefinitionAt( x, y ) ;
			
			// Replace the pixel with a new alpha only if RGB matches:
			if ( Pixels::areEqual( currentDef, colorDef, 
				/* use alpha */ false ) )
			{	
			
#if OSDL_DEBUG_PIXEL
				if ( x % 20 == 0 && y % 20 == 0 )
					LogPlug::debug( 
						"Surface::setAlphaForColor: replacing alpha of " 
						+ Pixels::toString( currentDef ) + " with new alpha = " 
						+ Ceylan::toNumericalString( newAlpha ) + " at ["
						+ Ceylan::toString( x ) + ";" + Ceylan::toString( y ) 
						+ "]" ) ;
#endif // OSDL_DEBUG_PIXEL
					
				currentDef.unused = newAlpha ;
				
				putColorDefinitionAt( x, y, currentDef, 
					/* no blending since put alpha must not be modified */ 
						false ) ;
				
#if OSDL_DEBUG_PIXEL

				// Re-read to check modification:
				if ( x % 20 == 0 && y % 20 == 0 )
				{
				
					ColorDefinition readDef = getColorDefinitionAt( x, y ) ;
					
					if ( ! Pixels::areEqual( readDef, currentDef, 
							/* use alpha */ true ) )
						LogPlug::error( "Surface::setAlphaForColor: "
							"alpha replacement failed: expecting " 
							+ Pixels::toString( readDef ) 
							+ ", read " + Pixels::toString( currentDef ) ) ;
				}
				
#endif // OSDL_DEBUG_PIXEL
					
			}
			else
			{
			
				/*
				if ( x % 10 == 0 && y % 10 == 0 )
					LogPlug::debug( "Surface::setAlphaForColor: pixel in ["
						+ Ceylan::toString( x ) + ";" + Ceylan::toString( y ) 
						+ "], whose color definition is " 
						+ Pixels::toString( currentDef ) 
						+ ", does not match." ) ;
				*/
			}
		
		}
		
	unlock() ;
	
	return true ;
		
}




bool Surface::drawHorizontalLine( Coordinate xStart, Coordinate xStop,
	Coordinate y, Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha ) throw()
{

	return Line::drawHorizontal( *this,  xStart, xStop, y, 
		red, green, blue, alpha ) ;
		
}



bool Surface::drawHorizontalLine( Coordinate xStart, Coordinate xStop,
	Coordinate y, Pixels::PixelColor actualColor ) throw()
{

	return Line::drawHorizontal( *this,  xStart, xStop, y, actualColor ) ;
	
}



bool Surface::drawHorizontalLine( Coordinate xStart, Coordinate xStop,
	Coordinate y, Pixels::ColorDefinition colorDef ) throw()
{

	return Line::drawHorizontal( *this,  xStart, xStop, y, colorDef ) ;
	
}




bool Surface::drawVerticalLine( Coordinate x, Coordinate yStart, 
	Coordinate yStop, Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha) throw()
{

	return Line::drawVertical( *this,  x, yStart, yStop, 
		red, green, blue, alpha ) ;
		
}
	

	
bool Surface::drawVerticalLine( Coordinate x, Coordinate yStart,
	 Coordinate yStop, Pixels::ColorDefinition colorDef ) throw()
{

	return Line::drawVertical( *this,  x, yStart, yStop, colorDef ) ;
	
}

	
	
	
bool Surface::drawLine( Coordinate xStart, Coordinate yStart,
	Coordinate xStop, Coordinate yStop, 
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha ) throw()
{

	return Line::draw( *this, xStart, yStart, xStop, yStop, 
		red, green, blue, alpha ) ;
		
}



bool Surface::drawLine( Coordinate xStart, Coordinate yStart,
		Coordinate xStop, Coordinate yStop, Pixels::ColorDefinition colorDef )
	throw()
{

	return Line::draw( *this, xStart, yStart, xStop, yStop, colorDef ) ;
	
}




bool Surface::drawCross( const Point2D & center, 
	Pixels::ColorDefinition colorDef, Length squareEdge ) throw() 
{

	return Line::drawCross( *this, center, colorDef, squareEdge ) ;
	
}



bool Surface::drawCross( Coordinate xCenter, Coordinate yCenter,
	Pixels::ColorDefinition colorDef, Length squareEdge ) throw() 
{

	return Line::drawCross( *this, xCenter, yCenter, colorDef, squareEdge ) ;
	
}
	


bool Surface::drawEdges( Pixels::ColorDefinition edgeColor, Length edgeWidth )
	throw()	
{
	
	Coordinate xmin = 0 ;
	Coordinate xmax = getWidth() - 1 ;
	
	Coordinate ymin = 0 ;
	Coordinate ymax = getHeight() - 1 ;
	
	while ( edgeWidth != 0 )
	{
	
		if ( ! drawVerticalLine( xmin, ymin, ymax, edgeColor ) )
			return false ;
			
		if ( ! drawVerticalLine( xmax, ymin, ymax, edgeColor ) )
			return false ;
			
		if ( ! drawHorizontalLine( xmin, xmax, ymin, edgeColor ) )
			return false ;
			
		if ( ! drawHorizontalLine( xmin, xmax, ymax, edgeColor ) )
			return false ;
				
		edgeWidth-- ;
		
		xmin++ ;
		xmax -- ;
		
		ymin++ ;
		ymax-- ;
	}
	
	return true ;
	
}



bool Surface::drawBox( const UprightRectangle & rectangle, 
		Pixels::ColorElement red, Pixels::ColorElement green, 
		Pixels::ColorElement blue, Pixels::ColorElement alpha, bool filled ) 
	throw()
{

	return rectangle.draw( *this, red, green, blue, alpha, filled ) ;
	
}
	
	
	
bool Surface::drawBox( const UprightRectangle & rectangle, 
	Pixels::ColorDefinition colorDef, bool filled ) throw()
{

	return rectangle.draw( *this, colorDef, filled ) ;
	
}
	



bool Surface::drawCircle( Coordinate xCenter, Coordinate yCenter, 
	Length radius, Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha,
	bool filled, bool blended ) throw()
{

	return TwoDimensional::drawCircle( *this, xCenter, yCenter, radius, 
		red, green, blue, alpha, filled, blended ) ;
				
}



bool Surface::drawCircle( Coordinate xCenter, Coordinate yCenter, 
	Length radius, Pixels::ColorDefinition colorDef, 
	bool filled, bool blended ) throw()
{

	return TwoDimensional::drawCircle( *this, xCenter, yCenter, radius, 
		colorDef, filled, blended ) ;
						
}



bool Surface::drawDiscWithEdge( Coordinate xCenter, Coordinate yCenter, 
	Length outerRadius, Length innerRadius, 
	Pixels::ColorDefinition ringColorDef, 
	Pixels::ColorDefinition discColorDef, bool blended ) throw()
{

	return TwoDimensional::drawDiscWithEdge( *this, xCenter, yCenter,
		outerRadius, innerRadius, ringColorDef, discColorDef, blended ) ;		
		
}	
					


bool Surface::drawEllipse( Coordinate xCenter, Coordinate yCenter, 
	Length horizontalRadius, Length verticalRadius,
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha, bool filled ) throw()
{

	return TwoDimensional::drawEllipse( *this, xCenter, yCenter,
		horizontalRadius, verticalRadius, red, green, blue, alpha, filled ) ;	
			
}



bool Surface::drawEllipse( Coordinate xCenter, Coordinate yCenter, 
	Length horizontalRadius, Length verticalRadius,
	Pixels::ColorDefinition colorDef, bool filled ) throw()
{

	return TwoDimensional::drawEllipse( *this, xCenter, yCenter,
		horizontalRadius, verticalRadius, colorDef, filled ) ;		
		
}



bool Surface::drawPie( Coordinate xCenter, Coordinate yCenter, Length radius, 
	Ceylan::Maths::AngleInDegrees angleStart, 
	Ceylan::Maths::AngleInDegrees angleStop,
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha ) throw() 
{

	return TwoDimensional::drawPie( *this, xCenter, yCenter, radius, 
		angleStart, angleStop, red, green, blue, alpha ) ;
		
}



bool Surface::drawPie( Coordinate xCenter, Coordinate yCenter, Length radius, 
	Ceylan::Maths::AngleInDegrees angleStart, 
	Ceylan::Maths::AngleInDegrees angleStop, 
	Pixels::ColorDefinition colorDef ) throw()		
{

	return TwoDimensional::drawPie( *this, xCenter, yCenter, 
		radius, angleStart, angleStop, colorDef ) ;
		
}




bool Surface::drawTriangle( Coordinate x1, Coordinate y1, 
		Coordinate x2, Coordinate y2, Coordinate x3, Coordinate y3,
		Pixels::ColorElement red, Pixels::ColorElement green, 
		Pixels::ColorElement blue, Pixels::ColorElement alpha, bool filled ) 
	throw()
{

	return TwoDimensional::drawTriangle( *this, x1, y1, x2, y2, x3, y3, 
		red, green, blue, alpha, filled ) ;
		
}		



bool Surface::drawTriangle( Coordinate x1, Coordinate y1, 
	Coordinate x2, Coordinate y2, Coordinate x3, Coordinate y3,
	Pixels::ColorDefinition colorDef, bool filled ) throw()
{

	return TwoDimensional::drawTriangle( *this, x1, y1, x2, y2, x3, y3,
		colorDef, filled ) ;
		
}	


	
bool Surface::drawTriangle( const Point2D & p1, const Point2D & p2, 
		const Point2D & p3, 
		Pixels::ColorElement red, Pixels::ColorElement green, 
		Pixels::ColorElement blue, Pixels::ColorElement alpha, bool filled ) 
	throw()
{

	return TwoDimensional::drawTriangle( *this, p1, p2, p3, 
		red, green, blue, alpha, filled ) ;
		
}	
	
	

bool Surface::drawTriangle( const Point2D & p1, const Point2D & p2, 
	const Point2D & p3, Pixels::ColorDefinition colorDef, bool filled ) throw()
{

	return TwoDimensional::drawTriangle( *this, p1, p2, p3, 
		colorDef, filled ) ;
		
}	
	
	
	
bool Surface::drawPolygon( const list<Point2D *> summits, 
	Coordinate x, Coordinate y,
	Pixels::ColorDefinition colorDef, bool filled ) throw()
{

	return TwoDimensional::drawPolygon( *this, summits, x, y, 
		colorDef, filled ) ;
			
}



bool Surface::drawPolygon( const list<Point2D *> summits, 
	Coordinate x, Coordinate y,
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha, bool filled ) throw()
{

	return TwoDimensional::drawPolygon( *this, summits, x, y, 
		red, green, blue, alpha, filled ) ;
		
}



bool Surface::drawGrid( Length columnStride, Length rowStride,
	Pixels::ColorDefinition lineColor, 
	bool fillBackground, Pixels::ColorDefinition backColor ) throw()
{

	
	if ( fillBackground )
	{
		if ( fill( backColor ) == false )
			return false ;
	}
	
	Length xBorder = getWidth() ;
	Length yBorder = getHeight() ;

	Coordinate x = 0 ;
	
	// Draw columns:
	while ( x < xBorder )
	{
		if ( ! drawVerticalLine( x, 0, yBorder, lineColor ) )
			return false ;
		x += columnStride ;
	}

	Coordinate y = 0 ;
	
	// Draw rows:
	while ( y < yBorder )
	{
		if ( ! drawHorizontalLine( 0, xBorder, y, lineColor ) )
			return false ;
		y += rowStride ;
	}
	
	return true ;
		
}
	
	
					
									
bool Surface::printText( const std::string & text, Coordinate x, Coordinate y, 
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha ) throw()	
{

	return Text::printBasic( text, *this, x, y, red, green, blue, alpha ) ;
	
}



bool Surface::printText( const std::string & text, Coordinate x, Coordinate y, 
	ColorDefinition colorDef ) throw()	
{

	return Text::printBasic( text, *this, x, y, colorDef ) ;
	
}



bool Surface::blitTo( Surface & targetSurface ) const throw( VideoException )
{

	return blitTo( targetSurface, 0, 0 ) ;
	
}



bool Surface::blitTo( Surface & targetSurface, Coordinate x, Coordinate y ) 
	const throw( VideoException )
{

#if OSDL_USES_SDL

#if OSDL_DEBUG_WIDGET

	LogPlug::trace( "Surface::blitTo: blitting to [" + Ceylan::toString( x )
		+ ";" + Ceylan::toString( y ) + "]."  ) ; 
		
#endif // OSDL_DEBUG_WIDGET


#if OSDL_DEBUG

	if ( isLocked() )
		throw VideoException( "Surface::blitTo with no source rectangle called "
			"whereas surface is locked." ) ;
			
#endif // OSDL_DEBUG

	SDL_Rect destinationRect ;
	
	destinationRect.x = x ;
	destinationRect.y = y ;
	// destinationRect width and height do not matter for SDL_BlitSurface.
	
	switch( SDL_BlitSurface( & getSDLSurface(), /* sourceRectangle */ 0, 
		& targetSurface.getSDLSurface(), & destinationRect ) )
	{
	
		case 0:
			// Success.
			return true ;
			break ;
				
		case -2:
			/*
			 * VideoMemoryLostException is a child class of VideoException, 
			 * no special message to deliver:
			 *
			 */
			throw VideoMemoryLostException() ;
			break ;
	
		case -1:
			throw VideoException( 
				"Surface::blitTo with no source rectangle: error in blit, " 
				+ Utils::getBackendLastError() ) ;
			break ;
	
		default:
			LogPlug::error( "Unexpected returned value in Surface::blitTo "
				"with no source rectangle." ) ;
			return false ;	
			break ;
	}
					
	return false ;	
	
#else // OSDL_USES_SDL

	return false ;
	
#endif // OSDL_USES_SDL
		
}



bool Surface::blitTo( Surface & targetSurface, 
	const TwoDimensional::Point2D & location ) const throw( VideoException )
{

	return blitTo( targetSurface, location.getX(), location.getY() ) ;
	
}



bool Surface::blitTo( Surface & targetSurface, 
		const TwoDimensional::UprightRectangle & sourceRectangle,
		const TwoDimensional::Point2D & destinationLocation ) 
	const throw( VideoException )
{

#if OSDL_USES_SDL

#if OSDL_DEBUG

	if ( isLocked() )
		throw VideoException( "Surface::blitTo with source rectangle called "
			"whereas surface is locked." ) ;
			
#endif // OSDL_DEBUG


#if OSDL_DEBUG_BLIT

	LogPlug::debug( "Surface::blitTo with source rectangle = " 
		+ sourceRectangle.toString() + " and destination location = " 
		+ destinationLocation.toString() ) ;
	
#endif // OSDL_DEBUG_BLIT
	
	SDL_Rect destinationRect ;
	
	destinationRect.x = destinationLocation.getX() ;
	destinationRect.y = destinationLocation.getY() ;
	// destinationRect width and height do not matter for SDL_BlitSurface.
	
	switch( SDL_BlitSurface( & getSDLSurface(),
		sourceRectangle.toLowLevelRect(), & targetSurface.getSDLSurface(), 
		& destinationRect ) )
	{
	
		case 0:
			// Success.
			return true ;
			break ;
				
		case -2:
			// VideoMemoryLostException is a child class of VideoException:
			throw VideoMemoryLostException() ;
			break ;
	
		case -1:
			throw VideoException( 
				"Surface::blitTo with source rectangle: error in blit: " 
				+ Utils::getBackendLastError() ) ;
			break ;
	
		default:
			LogPlug::error( "Unexpected returned value in Surface::blitTo "
				"with source rectangle." ) ;
			return false ;	
			break ;
	}
	
	return false ;

#else // OSDL_USES_SDL

	throw VideoException( "Surface::blitTo failed :"
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
		
}
	
	
	
Surface & Surface::zoom( Ceylan::Maths::Real abscissaZoomFactor, 
		Ceylan::Maths::Real ordinateZoomFactor,	bool antialiasing ) 
	const throw( VideoException )
{

#if OSDL_USES_SDL_GFX

	// Antialiasing not supported with flipping:
	if ( abscissaZoomFactor < 0 || ordinateZoomFactor < 0 )
		antialiasing = false ;
	
	LowLevelSurface * res =::zoomSurface( 
		const_cast<LowLevelSurface *>( _surface ), 
		abscissaZoomFactor,
		 ordinateZoomFactor,
		antialiasing ? SMOOTHING_ON: SMOOTHING_OFF ) ;
	
	if ( res == 0 )
		throw VideoException( "Surface::zoom: unable to zoom surface." ) ;
	
	// Creates a new back-buffer surface:
	return * new Surface( *res ) ;

#else // OSDL_USES_SDL_GFX

	throw VideoException( "Surface::zoom failed :"
		"no SDL_gfx support available" ) ;
		
#endif // OSDL_USES_SDL_GFX
				
}

	
					
Surface & Surface::rotoZoom( Ceylan::Maths::AngleInDegrees angle, 
		Ceylan::Maths::Real zoomFactor, bool antialiasing ) 
	const throw( VideoException )
{

	return rotoZoom( angle, zoomFactor, zoomFactor, antialiasing ) ;
	
}
						
										
Surface & Surface::rotoZoom( 
		Ceylan::Maths::AngleInDegrees angle, 
		Ceylan::Maths::Real abscissaZoomFactor,
		Ceylan::Maths::Real ordinateZoomFactor,
		bool antialiasing ) 
	const throw( VideoException )
{

#if OSDL_USES_SDL_GFX

	// Antialiasing not supported with flipping:
	if ( abscissaZoomFactor < 0 || ordinateZoomFactor < 0 )
		antialiasing = false ;
	
	LowLevelSurface * res =::rotozoomSurfaceXY( 
		const_cast<LowLevelSurface *>( _surface ), angle, abscissaZoomFactor,
		ordinateZoomFactor,	antialiasing ? SMOOTHING_ON: SMOOTHING_OFF ) ;
	
	if ( res == 0 )
		throw VideoException( 
			"Surface::rotoZoom: unable to rotozoom surface." ) ;
	
	return * new Surface( *res ) ;

#else // OSDL_USES_SDL_GFX

	throw VideoException( "Surface::rotoZoom failed :"
		"no SDL_gfx support available" ) ;
		
#endif // OSDL_USES_SDL_GFX

}

	
	
						
UprightRectangle & Surface::getClippingArea() const throw( VideoException ) 
{

#if OSDL_USES_SDL

	return * new UprightRectangle( _surface->clip_rect ) ;
	
#else // OSDL_USES_SDL
	
	throw VideoException( "Surface::getClippingArea failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

}
						
		
					
void Surface::setClippingArea( UprightRectangle & newClippingArea ) throw() 
{	

#if OSDL_USES_SDL

	TwoDimensional::Point2D corner = newClippingArea.getUpperLeftCorner() ;
	
	_surface->clip_rect.x = corner.getX() ; 
	_surface->clip_rect.y = corner.getY() ;
	 
	_surface->clip_rect.w = newClippingArea.getWidth() ; 
	_surface->clip_rect.h = newClippingArea.getHeight() ; 

#endif // OSDL_USES_SDL
	
}



UprightRectangle & Surface::getContentArea() const throw( VideoException ) 
{

	
	Length width = getWidth() ;
	Length minWidth = width ;
	Length maxWidth = 0 ;
	
	Length height = getHeight() ;
	Length minHeight = height ;
	Length maxHeight = 0  ;

	PixelColor colorkey = guessColorKey() ;
	PixelColor current ;
	
	for ( Length y = 0; y < height; y++ )
		for ( Length x = 0; x < width; x++ )
		{
		
			current = getPixelColorAt( x, y ) ;
			
			if ( ! areEqual( current, colorkey ) )
			{
			
				// We have a non-colorkey pixel here:
				
				if ( x < minWidth )
					minWidth = x ;
					
				if ( x > maxWidth )
					maxWidth = x ;
			
			
				if ( y < minHeight )
					minHeight = y ;
					
				if ( y > maxHeight )
					maxHeight = y ;
			
			}
		
		}
	
	return * new UprightRectangle( minWidth, minHeight, 
		/* width */ maxWidth - minWidth + 1,
		/* height */ maxHeight - minHeight + 1 ) ;
		
}



// Another proxy-method section.


void Surface::loadImage( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::Load( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ; 
		
}



void Surface::loadJPG( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::LoadJPG( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ; 
		
}



void Surface::loadPNG( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::LoadPNG( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ; 
		
}



void Surface::loadBMP( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::LoadBMP( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ; 
		
}



void Surface::loadGIF( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::LoadGIF( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ; 
		
}



void Surface::loadLBM( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::LoadLBM( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ; 
		
}



void Surface::loadPCX( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::LoadPCX( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ; 
		
}



void Surface::loadPNM( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::LoadPNM( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ; 
		
}



void Surface::loadTGA( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::LoadTGA( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ; 
		
}



void Surface::loadXPM( const string & filename, bool blitOnly,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::LoadXPM( *this, filename, blitOnly, 
		convertToDisplayFormat, convertWithAlpha  ) ;
		 
}



void Surface::savePNG( const std::string & filename, bool overwrite ) 
	throw( TwoDimensional::ImageException )
{

	TwoDimensional::Image::SavePNG( *this, filename, overwrite ) ;
	
}



void Surface::saveBMP( const std::string & filename, bool overwrite ) 
	throw( TwoDimensional::ImageException )
{

#if OSDL_USES_SDL

	if ( ( overwrite == false ) && Ceylan::System::File::Exists( filename ) )
		throw TwoDimensional::ImageException( 
			"Surface::saveBMP: target file " + filename 
			+ " already exists, and overwrite mode is off." ) ;
			
	if ( SDL_SaveBMP( _surface, filename.c_str() ) == -1 ) 
		throw TwoDimensional::ImageException( 
			"Surface::saveBMP: unable to save image: " 
			+ Utils::getBackendLastError() ) ;
			
#else // OSDL_USES_SDL

	throw TwoDimensional::ImageException( "Surface::saveBMP failed :"
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

}


					
void Surface::update() throw( VideoException )
{


#if OSDL_USES_SDL


#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "Surface::update" ) ; 
#endif // OSDL_DEBUG_WIDGET


#if OSDL_DEBUG
	if ( isLocked() )
		throw VideoException( 
			"Surface::update called whereas surface is locked." ) ;
#endif // OSDL_DEBUG

	// Updates the internal video buffer before display:
	redraw() ;

	switch( _displayType )
	{
	
	
		case BackBuffer:
			LogPlug::warning( "Surface::update requested on a "
				"non-screen surface ! (nothing done)" ) ;			
			break ;
	
	
		case ClassicalScreenSurface:
#if OSDL_DEBUG_WIDGET
			LogPlug::trace( 
				"Surface::update: flipping classical screen buffer" ) ; 
#endif // OSDL_DEBUG_WIDGET
		
			// Double-buffered: flip, others: update the whole rectangle.
			if ( SDL_Flip( _surface ) != 0 )
				throw VideoException( 
					"Surface::update: unable to flip classical screen: "
					+ Utils::getBackendLastError() ) ;
		
			/*
			 * If a rectangle area was known to include all changes in the
			 * screen surface, we could use here:
			 *
		 		
			if ( _surface->flags & SDL_DOUBLEBUF ) 
			{
				SDL_Flip(_surface) ;
			} 
			else 
			{
				SDL_UpdateRects( _surface, 1, & area ) ;
			}
		
			 *
			 */
			break ;
		
		
		case OpenGLScreenSurface:
#if OSDL_DEBUG_WIDGET
			LogPlug::trace( 
				"Surface::update: flipping OpenGL screen buffer" ) ; 
#endif // OSDL_DEBUG_WIDGET
			
			/*
			 * Should a different call be done if OpenGL without double
			 * buffering is used ? Probably no.
			 *
			 * SDL_GL_SwapBuffers should imply a glFlush(), but some reported
			 * adding one glFlush causes a miraculous recovery:
			 *
			 */
			glFlush() ;
			SDL_GL_SwapBuffers() ;
			break ;
		
		
		default:
			LogPlug::error( 
				"Surface::update: unknown display type, nothing done." ) ;	
			break ;
			
	}		

#else // OSDL_USES_SDL

	throw VideoException( "Surface::update failed :"
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
	
		
}



void Surface::updateRectangles( const list<UprightRectangle *> & listRects )
	throw( VideoException )
{

	/*
	 * Does not use SDL_UpdateRects for efficiency reasons 
	 * (avoid too many structure conversions).
	 *
	 */
	
	for ( list<UprightRectangle *>::const_iterator it = listRects.begin() ; 
		it != listRects.end() ;	it++ )
	{
		
#if OSDL_DEBUG		
		if ( *it == 0 )
		{
			LogPlug::error( "Surface::updateRectangles: "
				"null pointer in rectangle list." ) ;
			break ;
		}	
#endif // OSDL_DEBUG
		
	 	updateRectangle( * (*it) ) ;
	 
	}

}
	
			
					
void Surface::updateRectangle( const UprightRectangle & rect ) 
	throw( VideoException ) 
{	

	updateRectangle( rect.getUpperLeftAbscissa(), 
		rect.getUpperLeftOrdinate(), rect.getWidth(), rect.getHeight() ) ;
			
}



void Surface::updateRectangle( Coordinate x, Coordinate y, 
	Length width, Length height ) throw( VideoException ) 
{	

#if OSDL_USES_SDL

#if OSDL_DEBUG
	if ( isLocked() )
		LogPlug::error( 
			"Surface::updateRectangle() called whereas surface is locked." ) ;
#endif // OSDL_DEBUG
	
	if ( _displayType != BackBuffer )
	{
		SDL_UpdateRect( _surface, x, y, width, height ) ;
	}	
	else
	{
		throw VideoException( 
			"Surface::updateRectangle requested on a non-screen surface" ) ;
	}

#else // OSDL_USES_SDL

	throw VideoException( "Surface::updateRectangle failed :"
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
		
}
	
	
	
void Surface::setRedrawState( bool needsToBeRedrawn ) throw()
{

	_needsRedraw = needsToBeRedrawn ;
	
}


	
bool Surface::getRedrawState() const throw()
{

	return _needsRedraw ;
	
}

	
	
void Surface::redraw() throw()
{


	/*
	 * Far too verbose:
	 
#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "Surface::redraw: needs redraw attribute is " 
		+ Ceylan::toString(_needsRedraw ) + "." ) ; 
#endif // OSDL_DEBUG_WIDGET

	*
	*/
	
	if ( getRedrawState() )
	{
		
		// First redraw thyself:	
		redrawInternal() ;
	
		// Then request the sub-components to do so recursively:
		RedrawRequestEvent redrawEvent( *this ) ;
	
		/*
		 * This code relies on notifyAllListeners order, which uses a 
		 * classic iterator, from front to back of listeners list, 
		 * therefore from bottom to top-level widgets.
		 *
		 */
		notifyAllListeners( redrawEvent ) ;
	
		setRedrawState( false ) ;
		
	}
	
	// Do nothing if _needsRedraw is false.	
	
}



void Surface::redrawInternal() throw()
{

#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "Surface::redrawInternal (non overriden version)" ) ; 
#endif // OSDL_DEBUG_WIDGET

	/*
	 * Meant to be overriden if needed (filled with background color, 
	 * background image blitted, etc.)
	 *
	 */
	 	 
}
	

					
bool Surface::isInternalSurfaceAvailable() const throw()
{

	return ( _surface != 0 ) ;
	
}



void Surface::addWidget( TwoDimensional::Widget & widget ) 
	throw( VideoException )
{

#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "Surface::addWidget: adding " + widget.toString() ) ; 
#endif // OSDL_DEBUG_WIDGET

	/*
	 * Relies on event source implementation, which add new listeners to 
	 * back of list (push_back), which means for widgets, the top level.
	 *
	 */
	 
	try
	{
		add( /* a listener */ widget ) ;
	}
	catch( const Ceylan::EventException & e )
	{
		throw VideoException( 
			"Surface::addWidget: unable to add new listener widget: "
			+ e.toString() ) ;
	}
	
}



Surface & Surface::getWidgetRenderTarget() throw()	
{

	/*
	 * For simple surfaces (and widgets), subwidgets should target 
	 * this surface:
	 *
	 */
	return *this ;
	
}	


	
void Surface::putWidgetToFront( TwoDimensional::Widget & widget ) 
	throw( VideoException )
{

	/*
	 * Reorders _listeners list, inherited from EventSource, so that 
	 * the widget is at top level, i.e. in last position of the list (back).
	 *
	 */
	_listeners.remove( & widget ) ;
	
	// Put this widget in last slot (top-level):
	_listeners.push_back( & widget ) ;	
	 
}


					
void Surface::putWidgetToBack( TwoDimensional::Widget & widget ) 
	throw( VideoException )
{

	/*
	 * Reorders _listeners list, inherited from EventSource, so that 
	 * the widget is at bottom level, i.e. in first position of the list 
	 * (front).
	 *
	 */
	_listeners.remove( & widget ) ;
	
	// Put this widget in last (bottom-level) slot:
	_listeners.push_front( & widget ) ;	

}



void Surface::centerMousePosition() throw()
{

	setMousePosition( getWidth() / 2, getHeight() / 2 ) ;
	
}



void Surface::setMousePosition( Coordinate newX, Coordinate newY ) 
	throw( VideoException ) 
{

#if OSDL_USES_SDL

#if OSDL_DEBUG

	if ( _displayType == BackBuffer )
		throw VideoException( "Surface::setMousePosition failed: "
			"called on a non-screen surface." ) ;
	
#endif // OSDL_DEBUG
	
	SDL_WarpMouse( newX, newY ) ;
	
#else // OSDL_USES_SDL

	throw VideoException( "Surface::setMousePosition failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
	
}

	
										
bool Surface::mustBeLocked() const throw() 
{

#if OSDL_USES_SDL

	// Not stored once for all, since may change during the surface life ?
	return SDL_MUSTLOCK( _surface ) ;
	
#else // OSDL_USES_SDL

	return true ;
		
#endif // OSDL_USES_SDL
	 
}



void Surface::preUnlock() throw()
{

#if OSDL_USES_SDL

	/*
	 * Lockable framework ensures it is called only if necessary 
	 * (i.e. only if 'must be locked'):
	 *
	 */
	SDL_UnlockSurface( _surface ) ;

#endif // OSDL_USES_SDL
	
}



void Surface::postLock() throw()
{
	
#if OSDL_USES_SDL

	/*
	 * Lockable framework ensures it is called only if necessary 
	 * (i.e. only if 'must be locked'):
	 *
	 */
	SDL_LockSurface( _surface ) ;

#endif // OSDL_USES_SDL

}



Ceylan::System::Size Surface::getSizeInMemory() const throw()
{

#if OSDL_USES_SDL

	Ceylan::System::Size currentSize = sizeof( Surface ) ;
	
	if ( _surface != 0 )
	{
	
		currentSize += sizeof( LowLevelSurface ) ;

		// PixelFormat are supposed never shared:
		if ( _surface->format != 0 )
		{
		
			currentSize += sizeof( SDL_PixelFormat ) ;
			
			if ( _surface->format->palette != 0 )
			{
				currentSize += sizeof( SDL_Palette ) ;
				currentSize += _surface->format->palette->ncolors 
					* sizeof( SDL_Color ) ;
			}	
			
			if ( _surface->pixels != 0 )
				currentSize +=  _surface->w * _surface->h *
					_surface->format->BytesPerPixel ;
						
		}
		// else: count might be wrong.
	}

	// Do not count same pointed block more than once !
		 
	return currentSize ;

#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL
	
}



bool Surface::displayData( const Ceylan::Maths::IntegerData * dataArray,
	Ceylan::Uint32 dataCount,
	Pixels::ColorDefinition pencilColor, Pixels::ColorDefinition captionColor,
	Pixels::ColorDefinition backgroundColor,
	const string & caption, const UprightRectangle * inBox ) throw()
{
	
	// No debug requested by default:
#define	OSDL_DEBUG_DISPLAY_DATA 0

	Coordinate x, y ;
	Length width, height ;
		
	if ( inBox == 0 )
	{
		x = 0 ;
		y = 0 ;
		width  = getWidth() ;
		height = getHeight() ;
	}
	else
	{
	
		/*
		 * This absurd copy is a bit clumsy, but it allows to avoid making
		 * a new rectangle in the other branch.
		 *
		 */
		 		
		x      = inBox->getUpperLeftAbscissa() ;
		y      = inBox->getUpperLeftOrdinate() ;
		width  = inBox->getWidth() ;
		height = inBox->getHeight() ;
	}
	
	UprightRectangle drawingArea( x, y, width, height ) ;
	
#if OSDL_DEBUG_DISPLAY_DATA
	LogPlug::debug( "Drawing area will be: " + drawingArea.toString() ) ;
#endif // OSDL_DEBUG_DISPLAY_DATA
			
	// Draw first the background:	
	drawBox( drawingArea, backgroundColor, /* filled */ true ) ;
	
	// Then draw its border with pencil color:	
	drawBox( drawingArea, pencilColor, /* filled */ false ) ;
	
	
	// Now draw the curve:	
	
	// Abscissa: one pixel corresponds to one value.
	Coordinate xstart = drawingArea.getUpperLeftAbscissa() 
		+ graphAbscissaOffset ;	
	
	Coordinate xmax = drawingArea.getUpperLeftAbscissa() 
		+ drawingArea.getWidth() - graphAbscissaOffset ;
	Coordinate xstop ;
	 
	
	/*
	 * Number of pixel in abscissa for each sample 
	 * (auto-adjust to largest possible):
	 *
	 */
	Coordinate stride = static_cast<Coordinate>( 
		Ceylan::Maths::Floor( static_cast<Ceylan::Float32>( 
			( drawingArea.getWidth() - 2 * graphAbscissaOffset ) 
				/ static_cast<Ceylan::Float32>( dataCount ) ) ) ) ;
		
#if OSDL_DEBUG_DISPLAY_DATA
	LogPlug::debug( "Abscissa stride is " 
		+ Ceylan::toString( stride ) + "." ) ;	
#endif // OSDL_DEBUG_DISPLAY_DATA
	
	if ( stride < 1 )
	{
		LogPlug::error( 
			"Surface::displayData: available width too small to draw." ) ;
		return false ;
	}	
	
	
	if ( xstart + static_cast<Coordinate>( dataCount ) > xmax )
	{
		LogPlug::warning( 
			"Surface::displayData: graph will be truncated to x = "
			+ Ceylan::toString( xmax ) + " (insufficient width)." ) ;
		xstop = xmax ;	
	}
	else
	{
		xstop = xstart + stride * dataCount ;		
	}
	
	
	if ( xstart > xstop )
	{
	
		LogPlug::error( 
			"Surface::displayData: available width too small to draw." ) ;
		return false ;
	}	

	
	// Searching for extremas:
	
	Ceylan::Maths::IntegerData maxData = dataArray[ 0 ] ;
	Ceylan::Maths::IntegerData minData = dataArray[ 0 ] ;
	
	for ( Ceylan::Uint32 i = 1; i < dataCount; i++ )
	{
	
		if ( dataArray[i] > maxData )
			maxData = dataArray[i] ;
			
		if ( dataArray[i] < minData )
			minData = dataArray[i] ;
			
	}
	
	
#if OSDL_DEBUG_DISPLAY_DATA
	LogPlug::debug( "Dynamic range from " + Ceylan::toString( minData ) 
		+ " to " + Ceylan::toString( maxData ) + "." ) ;
#endif // OSDL_DEBUG_DISPLAY_DATA
	
	Ceylan::Maths::IntegerData ordinateDrawRange = 
		drawingArea.getHeight() - 2 * graphOrdinateOffset ;
	
	if ( ordinateDrawRange < 0 )
	{
		LogPlug::error( "Surface::displayData: height too small to draw." ) ;
		return false ;
	}	
	
	Ceylan::Float32 yScaleFactor ;
	
	if ( maxData != minData )
		yScaleFactor = static_cast<Ceylan::Float32>( ordinateDrawRange ) 
			/ ( maxData - minData ) ;
	else
		yScaleFactor = 1 ;
	
		
#if OSDL_DEBUG_DISPLAY_DATA

	LogPlug::debug( "Ordinate scale factor is " 
		+ Ceylan::toString( yScaleFactor ) 
		+ " (" + Ceylan::toString( ordinateDrawRange ) 
		+ " / " + Ceylan::toString( maxData - minData ) + ")." ) ;
			
#endif // OSDL_DEBUG_DISPLAY_DATA
	
	Coordinate yplotBase = drawingArea.getUpperLeftOrdinate() 
		+ drawingArea.getHeight() - graphOrdinateOffset ;
	
	//LogPlug::debug( "yplotBase is " + Ceylan::toString( yplotBase ) ) ;
	
	Ceylan::Uint32 dataCurrent = 0 ;

	Coordinate xplot = xstart ;
	
	Coordinate yplotPrevious = yplotBase ;
	Coordinate yplot ;
	
	while ( xplot < xstop )
	{
		yplot = static_cast<Coordinate>( 
			( dataArray[ dataCurrent ] - minData ) * yScaleFactor ) ;
		
		drawLine( xplot, yplotPrevious, xplot+1, yplotBase - yplot, 
			pencilColor ) ;
		yplotPrevious = yplotBase - yplot ;
	
		/*
		 * LogPlug::debug( "yplotPrevious is " 
		  	+ Ceylan::toString( yplotPrevious ) ) ;
		 *
		 */
		
		dataCurrent++ ;
		xplot += stride ;
		
	}
	
	/*
	 * These are not real axes 
	 * (they are not set so that they are y=0 for example):
	 *
	 */
	
	ColorDefinition axisColor = Pixels::Ivory ;
	
	drawHorizontalLine( xstart, xstop, yplotBase, axisColor ) ;
	drawVerticalLine( xstart, yplotBase, 
		drawingArea.getUpperLeftOrdinate() + graphOrdinateOffset, axisColor ) ;
	
	/*
	 * LogPlug::debug( "Last y plotted was " 
	 	+ Ceylan::toString( yplotPrevious ) + "." ) ;
	 *
	 */
	
	if( caption.size() > 0 )
		printText( caption, 
			drawingArea.getUpperLeftAbscissa() + captionAbscissaOffset, 
			drawingArea.getUpperLeftOrdinate() + drawingArea.getHeight() 
				- captionOrdinateOffset,
			captionColor ) ;

	return true ;
	
}
	
	
		 
const string Surface::toString( Ceylan::VerbosityLevels level ) const throw()
{

#if OSDL_USES_SDL

	std::list<string> surfaceList ;

	switch( _displayType )
	{
	
		case BackBuffer:
			surfaceList.push_back( "This is a back buffer surface." ) ;
			break ;
	
		case ClassicalScreenSurface:
			surfaceList.push_back( "This is a classical screen surface." ) ;
			break ;
		
		case OpenGLScreenSurface:	
			surfaceList.push_back( "This is an OpenGL screen surface." ) ;
			break ;
		
		default:
			surfaceList.push_back( 
				"This is a surface of unknown type (abnormal)." ) ;
			break ;
		
	}
			
	surfaceList.push_back( "Dimensions: ( width = " 
		+ Ceylan::toString( getWidth() ) + " ; height = "
		+ getHeight() + " )" ) ; 
	
	if ( level == Ceylan::low )
		return "Surface description: " 
			+ Ceylan::formatStringList( surfaceList ) ;
		
	surfaceList.push_back( "Pitch: " + Ceylan::toString( getPitch() ) ) ;
	
	surfaceList.push_back( "Pixel format: bits per pixel = " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint16>( getBitsPerPixel() ) ) ) ;
	
	surfaceList.push_back( "Video flags: " + InterpretFlags( getFlags() ) ) ;
	
	if ( _surface->format->palette == 0 )
		surfaceList.push_back( 
			"Surface uses direct color definitions (no palette used)" ) ;
	else
		surfaceList.push_back( "Surface uses a palette (indexed colors)" ) ;
	
	surfaceList.push_back( "Pitch: " + Ceylan::toString( getPitch() ) ) ;
		
	surfaceList.push_back( "Clipping area: " 
		+ UprightRectangle( _surface->clip_rect ).toString( level ) ) ;

	surfaceList.push_back( "Size in memory: " 
		+ Ceylan::toString( 
			static_cast<Ceylan::Uint32>( getSizeInMemory() ) ) + " bytes" ) ;
		
	/*
	 * surfaceList.push_back( "Reference count: " 
	 	+ Ceylan::toString( _refcount ) ) ;
	 *
	 */
	
	surfaceList.push_back( "Widget relationship: " 
		+ EventSource::toString( level ) ) ;
	
	
	return "Surface description: " + Ceylan::formatStringList( surfaceList ) ;

#else // OSDL_USES_SDL

	return "" ;
	
#endif // OSDL_USES_SDL
	
}



Surface & Surface::LoadImage( const std::string & filename,
		bool convertToDisplayFormat, bool convertWithAlpha ) 
	throw( TwoDimensional::ImageException )
{

	Surface * toLoad = new Surface() ;
	
	try 
	{
		toLoad->loadImage( filename, /* blitOnly */ false, 
			convertToDisplayFormat, convertWithAlpha ) ;
	} 
	catch( const TwoDimensional::ImageException & e )
	{
		delete toLoad ;
		throw ;
	}
		
	return * toLoad ;
	
}

	
	
string Surface::InterpretFlags( Flags flags ) throw() 
{


	/*
	 * Indicates surely that these flags corresponds to a screen surface:
	 * (sufficient but not necessary) 
	 *
	 */
	 	
	std::list<string> res ;
	
	if ( flags & ( AnyPixelFormat | DoubleBuffered 
		| FullScreen | OpenGL | OpenGLBlit ) )
	{
	
		res.push_back( "This surface should be a display surface." ) ;
		
		if ( flags & AnyPixelFormat )
			res.push_back( 
				"Allows any pixel format for the display surface." ) ;
		else
			res.push_back( "Only specified pixel format "
				"is accepted for the display surface." ) ;

		if ( flags & DoubleBuffered )
			res.push_back( "Display surface is double buffered." ) ;
		else
			res.push_back( "Display surface is not double buffered." ) ;
		
		if ( flags & FullScreen )
			res.push_back( "Display surface is fullscreen." ) ;
		else
			res.push_back( "Display surface is in windowed mode "
				"(not full screen)." ) ;
		
		if ( flags & OpenGL )
			res.push_back( "Display surface has an OpenGL context." ) ;
		else
			res.push_back( "Display surface has no OpenGL context." ) ;
		
		if ( flags & OpenGLBlit )
			res.push_back( 
				"Display surface supports OpenGL blitting (deprecated)." ) ;
			
		/*
		 * Ignore if OpenGLBlit is not used: avoid useless publicity 
		 * for bad habits.
		 * 
	 	
		else
			res.push_back( 
				"Display surface does not support OpenGL blitting." ) ;
		 *
		 */
		 
		if ( flags & Resizable )
			res.push_back( "Display surface is resizable." ) ;
		else
			res.push_back( "Display surface is not resizable." ) ;
	
	}
	else
	{
		res.push_back( "No flag dedicated to display surfaces "
			"is used for this surface." ) ;
	}		
	
	
	if ( flags & Software )
		res.push_back( "Surface is to be stored in system memory." ) ;
	else
		res.push_back( 
			"Surface is not required to be stored in system memory." ) ;
		
		
	if ( flags & Hardware )
		res.push_back( "Surface is to be stored in video memory." ) ;
	else
		res.push_back( 
			"Surface is not required to be stored in video memory." ) ;
		
		
	if ( flags & AsynchronousBlit )
		res.push_back( "Surface should use asynchronous blits if possible." ) ;
	else
		res.push_back( 
			"Surface is not required to use asynchronous blits if possible." ) ;
			
					
	if ( flags & ExclusivePalette )
		res.push_back( "Surface should have an exclusive palette." ) ;
	else
		res.push_back( "Surface has not an exclusive palette." ) ;
		
		
	if ( flags & HardwareAcceleratedBlit )
		res.push_back( "Surface is to use hardware-accelerated blits." ) ;
	else
		res.push_back( 
			"Surface is not required to use hardware-accelerated blits." ) ;
		
		
	if ( flags & ColorkeyBlit )
		res.push_back( "Surface is to use colorkey blitting." ) ;
	else
		res.push_back( "Surface is not required to use colorkey blitting." ) ;
		
		
	if ( flags & RLEColorkeyBlit )
		res.push_back( "Surface is to use RLE-accelerated colorkey blits." ) ;
	else
		res.push_back( 
			"Surface is not required to use RLE-accelerated colorkey blits." ) ;
		
		
	if ( flags & AlphaBlendingBlit )
		res.push_back( "Surface is to use alpha blending blits." ) ;
	else
		res.push_back( 
			"Surface is not required to use alpha blending blits." ) ;
		
		
	if ( flags & Preallocated )
		res.push_back( "Surface is to use preallocated memory." ) ;
	else
		res.push_back( "Surface is not required to use preallocated memory." ) ;
		
		
	return "The specified surface flags, whose value is " 
		+ Ceylan::toString( flags, /* bit field */ true ) 
		+ ", means: " + Ceylan::formatStringList( res ) ;
		
}




Offset Surface::getOffset() const throw()
{

#if OSDL_USES_SDL

	return _surface->offset ;
	
#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL
	
}



void Surface::setOffset( Offset offset ) throw()
{

#if OSDL_USES_SDL

	 _surface->offset = offset ;
	 
#endif // OSDL_USES_SDL
	 
}
	


void Surface::flush() throw() 
{
	
#if OSDL_USES_SDL

	if ( _surface != 0 )
	{

#if OSDL_DEBUG_SURFACE
		LogPlug::trace( "Surface::flush: flushing surface." ) ;
#endif // OSDL_DEBUG_SURFACE

		if ( _displayType != BackBuffer )
			inconsistencyDetected( 
				"Surface::flush: trying to delete a screen surface." ) ;
 		
		SDL_FreeSurface( _surface ) ;
		_surface = 0 ;
		
	}	

#endif // OSDL_USES_SDL

}



void Surface::inconsistencyDetected( const string & message ) const throw()	
{

	Ceylan::emergencyShutdown( "Inconsistency detected in OSDL Surface: "
		+ message ) ;
		
}



Surface::Surface( const Surface & source ) throw():
	TwoDimensional::UprightRectangle( 0, 0, 0, 0 ),
	Ceylan::EventSource(),
	Ceylan::Lockable(),
	Ceylan::SmartResource(),
	_surface( 0 ), 
	_displayType( BackBuffer ), 
	_mustBeLocked( false ),
	_needsRedraw( true ) 
{

	// Not implemented on purpose, use clone() instead !
	Ceylan::emergencyShutdown( 
		"Surface copy constructor called, whereas should never be used." ) ;
		
}



std::ostream & operator << ( std::ostream & os, Surface & s ) throw()
{
    return os << s.toString() ;
}


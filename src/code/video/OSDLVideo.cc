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


#include "OSDLVideo.h"

#include "OSDLImage.h"          // for ImageException and BMP headers
#include "OSDLSurface.h"        // for Surface
#include "OSDLVideoRenderer.h"  // for VideoRenderer
#include "OSDLUtils.h"          // for getBackendLastError
#include "OSDLBasic.h"          // for getExistingCommonModule, GetVersion, etc.
#include "OSDLGUI.h"            // for GUIModule::GetBackendLastError, etc.


#include <list>
using std::list ;

using std::string ;
using std::pair ;





#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>              // for OSDL_CACHE_OVERALL_SETTINGS and al
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL_IMAGE
#include "SDL_image.h"               // for IMG_Init and IMG_Quit
#endif // OSDL_USES_SDL_IMAGE


#if OSDL_USES_AGAR

#include <agar/config/have_opengl.h>

#include <agar/core.h>
#include <agar/gui.h>
#include <agar/gui/opengl.h>

#endif // OSDL_USES_AGAR


using namespace Ceylan ;
using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Rendering ;
using namespace OSDL::Video::OpenGL ;




// Replicating these defines allows to enable them on a per-class basis:
#if OSDL_VERBOSE_VIDEO_MODULE

#define LOG_DEBUG_VIDEO(message)   LogPlug::debug(message)
#define LOG_TRACE_VIDEO(message)   LogPlug::trace(message)
#define LOG_WARNING_VIDEO(message) LogPlug::warning(message)

#else // OSDL_VERBOSE_VIDEO_MODULE

#define LOG_DEBUG_VIDEO(message)
#define LOG_TRACE_VIDEO(message)
#define LOG_WARNING_VIDEO(message)

#endif // OSDL_VERBOSE_VIDEO_MODULE




/*
 * These flags can be used for all Surfaces <b>created by setMode</b>.
 *
 * @note They are defined relatively to SDL back-end, see SDL_video.h
 *
 * May overlap a bit with Surface constants.
 *
 * @see Surface
 *
 */



/*
 * Checking binary value to avoid collisions of mask:
 *
 * (use http://www.easycalculation.com/hex-converter.php if really unsure)
 *

SDL_SWSURFACE	0x00000000 = 0b00000000000000000000000000000000
SDL_HWSURFACE	0x00000001 = 0b00000000000000000000000000000001
SDL_OPENGL      0x00000002 = 0b00000000000000000000000000000010
SDL_ASYNCBLIT	0x00000004 = 0b00000000000000000000000000000100
SDL_OPENGLBLIT	0x0000000A = 0b00000000000000000000000000001010
SDL_RESIZABLE	0x00000010 = 0b00000000000000000000000000010000
SDL_NOFRAME		0x00000020 = 0b00000000000000000000000000100000
SDL_HWACCEL		0x00000100 = 0b00000000000000000000000100000000
SDL_SRCCOLORKEY	0x00001000 = 0b00000000000000000001000000000000
SDL_RLEACCELOK	0x00002000 = 0b00000000000000000010000000000000
SDL_RLEACCEL	0x00004000 = 0b00000000000000000100000000000000
SDL_SRCALPHA	0x00010000 = 0b00000000000000010000000000000000
SDL_PREALLOC	0x01000000 = 0b00000001000000000000000000000000
SDL_ANYFORMAT	0x10000000 = 0b00010000000000000000000000000000
SDL_HWPALETTE	0x20000000 = 0b00100000000000000000000000000000
SDL_DOUBLEBUF	0x40000000 = 0b01000000000000000000000000000000
SDL_FULLSCREEN	0x80000000 = 0b10000000000000000000000000000000

*/



#if OSDL_USES_SDL

#include "SDL.h"


const Ceylan::Flags VideoModule::SoftwareSurface  = SDL_SWSURFACE ;
const Ceylan::Flags VideoModule::HardwareSurface  = SDL_HWSURFACE ;
const Ceylan::Flags VideoModule::AsynchronousBlit = SDL_ASYNCBLIT ;
const Ceylan::Flags VideoModule::AnyPixelFormat   = SDL_ANYFORMAT ;
const Ceylan::Flags VideoModule::ExclusivePalette = SDL_HWPALETTE ;
const Ceylan::Flags VideoModule::DoubleBuffered   = SDL_DOUBLEBUF ;
const Ceylan::Flags VideoModule::FullScreen       = SDL_FULLSCREEN ;
const Ceylan::Flags VideoModule::OpenGL           = SDL_OPENGL ;
const Ceylan::Flags VideoModule::Resizable        = SDL_RESIZABLE ;
const Ceylan::Flags VideoModule::NoFrame          = SDL_NOFRAME ;



/// See http://sdldoc.csn.ul.ie/sdlenvvars.php
const std::string VideoModule::_SDLEnvironmentVariables[] =
{

		"SDL_FBACCEL",
		"SDL_FBDEV",
		"SDL_FULLSCREEN_UPDATE",
		"SDL_VIDEODRIVER",
		"SDL_VIDEO_CENTERED",
		"SDL_VIDEO_GL_DRIVER",
		"SDL_VIDEO_X11_DGAMOUSE",
		"SDL_VIDEO_X11_MOUSEACCEL",
		"SDL_VIDEO_X11_NODIRECTCOLOR",
		"SDL_VIDEO_X11_VISUALID",
		"SDL_VIDEO_YUV_DIRECT",
		"SDL_VIDEO_YUV_HWACCEL",
		"SDL_WINDOWID"

} ;



#else // OSDL_USES_SDL


// Same as SDL:

const Ceylan::Flags VideoModule::SoftwareSurface  = 0x00000000 ;
const Ceylan::Flags VideoModule::HardwareSurface  = 0x00000001 ;
const Ceylan::Flags VideoModule::AsynchronousBlit = 0x00000004 ;
const Ceylan::Flags VideoModule::AnyPixelFormat   = 0x10000000 ;
const Ceylan::Flags VideoModule::ExclusivePalette = 0x20000000 ;
const Ceylan::Flags VideoModule::DoubleBuffered   = 0x40000000 ;
const Ceylan::Flags VideoModule::FullScreen       = 0x80000000 ;
const Ceylan::Flags VideoModule::OpenGL           = 0x00000002 ;
const Ceylan::Flags VideoModule::Resizable        = 0x00000010 ;
const Ceylan::Flags VideoModule::NoFrame          = 0x00000020 ;

/// See http://sdldoc.csn.ul.ie/sdlenvvars.php
const std::string VideoModule::_SDLEnvironmentVariables[] = {} ;

#endif // OSDL_USES_SDL




bool VideoModule::_IsUsingOpenGL = false ;
bool VideoModule::_DrawEndPoint  = false ;
bool VideoModule::_AntiAliasing  = true  ;


const Ceylan::Uint16 VideoModule::DriverNameMaximumLength = 50 ;

const BitsPerPixel VideoModule::UseCurrentColorDepth = 0 ;




// To protect LoadIcon:
#include "OSDLIncludeCorrecter.h"



VideoModule::VideoModule() :
	Ceylan::Module(
		"OSDL video module",
		"This is the root video module of OSDL",
		"http://osdl.sourceforge.net",
		"Olivier Boudeville",
		"olivier.boudeville@online.fr",
		OSDL::GetVersion(),
		"disjunctive LGPL/GPL" ),
	_screen( 0 ),
	_displayInitialized( false ),
	_gui( 0 ),
	_renderer( 0 ),
	_openGLcontext( 0 ),
	_drawEndPoint( false ),
	_antiAliasing( true ),
	_screenStartingSecond( 0 ),
	_screenStartingMicrosecond( 0 )
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM7

	throw OSDL::Exception( "VideoModule constructor failed: "
		"not supported on the ARM7." ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	send( "Initializing video subsystem." ) ;

	send( "Video subsystem initialized." ) ;

	dropIdentifier() ;

#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL

	send( "Initializing video subsystem." ) ;

	if ( SDL_InitSubSystem( CommonModule::UseVideo )
			!= CommonModule::BackendSuccess )
		throw VideoException( "VideoModule constructor: "
			"unable to initialize video subsystem: "
			+ Utils::getBackendLastError() ) ;

#if OSDL_USES_SDL_IMAGE

	/*
	 * Initializing image loader, for JPEG and PNG support:
	 *
	 * (avoids later hiccups, thanks to initial library loading)
	 *
	 */
	Ceylan::Flags requestedFormatFlags = IMG_INIT_JPG | IMG_INIT_PNG ;

	Ceylan::Flags actualFormatFlags = ::IMG_Init( requestedFormatFlags ) ;

	if ( actualFormatFlags != requestedFormatFlags )
		throw VideoException( "VideoModule constructor: "
		  "unable to initialize the image subsystem (SDL_image): "
		  "requested flags "
		  + Ceylan::toString( requestedFormatFlags, /* bitField */ true )
		  + " differ from obtained flags "
		  + Ceylan::toString( actualFormatFlags, /* bitField */ true ) ) ;

#endif // OSDL_USES_SDL_IMAGE

	send( "Video subsystem initialized." ) ;

	dropIdentifier() ;

#else // OSDL_USES_SDL

	throw VideoException( "VideoModule constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#endif // OSDL_ARCH_NINTENDO_DS

}



VideoModule::~VideoModule() throw()
{

#if OSDL_USES_SDL

	send( "Stopping video subsystem." ) ;

	// _gui not owned.


#if OSDL_USES_SDL_IMAGE

	send( "Stopping SDL_image subsystem." ) ;
	::IMG_Quit() ;

#endif // OSDL_USES_SDL_IMAGE


	if ( _openGLcontext != 0 )
		delete _openGLcontext ;

	if ( _renderer != 0 )
		delete _renderer ;

	// Back-end screen surface will not be deallocated here:
	if ( _screen != 0 )
	{

		send( "Average frame-rate has been "
			+ Ceylan::toString( getAverageRefreshRate(), /* precision */ 2 )
			+ " frames per second." ) ;

		delete _screen ;

	}


	SDL_QuitSubSystem( CommonModule::UseVideo ) ;

	send( "Video subsystem stopped." ) ;

#endif // OSDL_USES_SDL

}



bool VideoModule::hasScreenSurface() const
{

	return ( _screen != 0 ) ;

}



Surface & VideoModule::getScreenSurface() const
{

	if ( _screen == 0 )
		throw VideoException( "VideoModule::getScreenSurface: "
			"no available screen surface." ) ;

	return * _screen ;

}



void VideoModule::setScreenSurface( Surface & newScreenSurface )
{

	send( "Setting screen surface to " + newScreenSurface.toString(), 8 ) ;

	if ( _screen->getDisplayType() == Surface::BackBuffer )
		throw VideoException(
			"VideoModule::setScreenSurface: from its display type, "
			"specified surface is not a screen surface" ) ;

	_screen = & newScreenSurface ;

}



bool VideoModule::hasRenderer() const
{

	return ( _renderer != 0 ) ;

}



OSDL::Rendering::VideoRenderer & VideoModule::getRenderer() const
{

	if ( _renderer == 0 )
		throw VideoException(
			"VideoModule::getRenderer: no video renderer available." ) ;

	return * _renderer ;

}



void VideoModule::setRenderer( VideoRenderer & newRenderer )
{

	if (  _renderer != 0 )
		delete _renderer ;

	_renderer = & newRenderer ;

}



bool VideoModule::hasOpenGLContext() const
{

	return ( _openGLcontext != 0 ) ;

}



OpenGL::OpenGLContext & VideoModule::getOpenGLContext() const
{

	if ( _openGLcontext == 0 )
		throw VideoException(
			"VideoModule::getOpenGLContext: no OpenGL context available." ) ;

	return * _openGLcontext ;

}



void VideoModule::setOpenGLContext( OpenGL::OpenGLContext & newOpenGLContext )
{

	if ( _openGLcontext != 0 )
		delete _openGLcontext ;

	_openGLcontext = & newOpenGLContext ;

	_IsUsingOpenGL = true ;

}




BitsPerPixel VideoModule::getBestColorDepthForMode(
	Length width, Length height, BitsPerPixel askedBpp, Ceylan::Flags flags )
{

#if OSDL_USES_SDL

	return SDL_VideoModeOK( width, height, askedBpp, flags ) ;

#else // OSDL_USES_SDL

	return 0 ;

#endif // OSDL_USES_SDL

}



bool VideoModule::isDisplayInitialized() const
{

	return _displayInitialized ;

}


bool VideoModule::isGUIEnabled() const
{

  return ( _gui != 0) ;

}



void VideoModule::setGUIModule( GUIModule & guiModule )
{

  // Preferably to be called before setMode, to be taken into account.

  if ( _gui != 0 )
	throw VideoException( "VideoModule::setGUIModule failed: "
	  "a GUI module was already registered." ) ;

  _gui = & guiModule ;

}



void VideoModule::forgetGUIModule()
{

  _gui = 0 ;

}



Ceylan::Flags VideoModule::setMode( Length width, Length height,
	BitsPerPixel askedBpp, Ceylan::Flags flags, OpenGL::Flavour flavour )
{

#if OSDL_USES_SDL

	_displayInitialized = false ;

	_screenStartingSecond = 0 ;
	_screenStartingMicrosecond = 0 ;

	Ceylan::Flags userFlags = flags ;

	send( "Trying to set " + Ceylan::toString( width ) + 'x'
	  + Ceylan::toString( height ) + " video mode, with "
	  + ( ( askedBpp == 0 ) ? string( "current screen color depth" ) :
		Ceylan::toString( static_cast<Ceylan::Uint16>( askedBpp ) ) )
	  + " bits per pixel), with user-defined flags "
	  + InterpretFlags( flags )
	  + "The " + OpenGLContext::ToString( flavour )
	  + " flavour is selected" ) ;

	if ( ( ( flags & OpenGL ) == 0 ) && ( flavour != OpenGL::None ) )
	{

		LOG_WARNING_VIDEO( "VideoModule::setMode: OpenGL flavour selected ("
			+ OpenGLContext::ToString( flavour )
			+ ") whereas OpenGL flag not set, adding it." ) ;

		flags |= OpenGL ;

	}

	bool useOpenGLRequested = ( ( flags & OpenGL ) != 0 ) ;

	if ( useOpenGLRequested && ( flags & DoubleBuffered ) )
	{

		/*
		 * Double buffering with OpenGL is not to be selected with the
		 * DoubleBuffered flag for setMode, we therefore ensure it is
		 * deactivated now (it will be activated appropriately later on the
		 * OpenGL context):
		 *
		 */
		flags &= ~DoubleBuffered ;

	}

	if ( useOpenGLRequested )
	{

		// First tries the optimal ("best") settings for that flavour:
		OpenGLContext::SetUpForFlavour( flavour, /* safest */ false ) ;

		if ( askedBpp != 0 )
			OpenGLContext::SetColorDepth( askedBpp ) ;

	}


	if ( userFlags != flags )
		send( "Initializing the display with following modified flags: "
			+ InterpretFlags( flags ) ) ;
	else
		send( "Initializing the display with unchanged user flags") ;

	SDL_Surface * screen = SDL_SetVideoMode( width, height, askedBpp, flags ) ;

	if ( screen == 0 )
	{

		if ( useOpenGLRequested )
		{

			LogPlug::warning( "VideoModule::setMode: "
				"failed to initialize OpenGL video mode (\""
				+ Utils::getBackendLastError()
				+ "\"), using safer settings as a fall-back." ) ;

			/*
			 * We might have triggered an error such as:
			 * "Couldn't find matching GLX visual", trying safer yet less
			 * accelerated/fancy settings:
			 *
			 */
			OpenGLContext::SetUpForFlavour( flavour, /* safest */ true ) ;

			screen = SDL_SetVideoMode( width, height, askedBpp, flags ) ;

		}

		/*
		 * No fall-back available yet for non-OpenGL modes, next exception will
		 * be thrown.
		 *
		 */

	}


	if ( screen == 0 )
		throw VideoException( "Could not set "
			+ Ceylan::toString( width )    + 'x'
			+ Ceylan::toString( height )   + 'x'
			+ Ceylan::toNumericalString( askedBpp )
			+ " with flags " + Ceylan::toString( flags, /* bit field */ true )
			+ " video mode: " + Utils::getBackendLastError() ) ;


	if ( useOpenGLRequested )
	{

		_screen = new Video::Surface( * screen,
			/* display type */ Surface::OpenGLScreenSurface ) ;

		send( OpenGLContext::InterpretFeatureAvailability() ) ;

	}
	else
	{

		_screen = new Video::Surface( * screen,
			/* display type */ Surface::ClassicalScreenSurface ) ;

	}


	int bpp = _screen->getBitsPerPixel() ;

	send( "Actual color depth is " + Ceylan::toString( bpp )
		+ " bits per pixel." ) ;

	// A zero bit per pixel request means any depth, no warning in this case:
	if ( ( askedBpp != bpp ) && ( askedBpp != 0 ) )
		LOG_WARNING_VIDEO( "Color depth is " + Ceylan::toString( bpp )
			+ " bits per pixel (instead of the asked "
			+ Ceylan::toString( static_cast<Ceylan::Uint16>( askedBpp ) )
			+ " bits per pixel)." ) ;


	if ( useOpenGLRequested )
	{

		/*
		 * Initializes the flavours and the context, since setMode has just been
		 * called:
		 *
		 */
		switch ( flavour )
		{

			case OpenGL::None:
				if ( useOpenGLRequested )
				{
					if ( ! hasOpenGLContext() )
						setOpenGLContext( * new OpenGLContext( flavour, bpp,
							_screen->getWidth(), _screen->getHeight() ) ) ;
					else
						_openGLcontext->selectFlavour( flavour ) ;
				}
				break ;

			case OpenGL::OpenGLFor2D:
				if ( ! hasOpenGLContext() )
					setOpenGLContext( * new OpenGLContext( flavour, bpp,
						_screen->getWidth(), _screen->getHeight() ) ) ;
				else
					_openGLcontext->selectFlavour( flavour ) ;
				break ;

			case OpenGL::OpenGLFor3D:
				if ( ! hasOpenGLContext() )
					setOpenGLContext( * new OpenGLContext( flavour, bpp,
						_screen->getWidth(), _screen->getHeight() ) ) ;
				else
					_openGLcontext->selectFlavour( flavour ) ;
				break ;

			case OpenGL::Reload:
				if ( hasOpenGLContext() )
					_openGLcontext->reload() ;
				else
					throw VideoException( "VideoModule::setMode: "
						"reload flavour selected whereas "
						"no OpenGL context was available, nothing done." ) ;
				break ;

			default:
				throw VideoException( "VideoModule::setMode: unknown "
					+ OpenGLContext::ToString( flavour )
					+ " flavour selected." ) ;
				break ;

		}

	}

	/*
	 * In all cases, if OpenGL is to be used, an OpenGL context is available
	 * here.
	 *
	 */

	/*
	 * Defines the viewport independently of flavours (lower-left corner of the
	 * OpenGL viewport is the origin of the setMode window).
	 *
	 * By default, the viewport is chosen so that it takes all the display
	 * window.
	 *
	 */
	if ( useOpenGLRequested )
		_openGLcontext->setViewPort( _screen->getWidth(),
			_screen->getHeight() ) ;


	send( "After display creation, interpreting actually obtained surface: "
		+ _screen->toString()
		+ "The corresponding pixel format for this screen surface is: "
		+ Pixels::toString( _screen->getPixelFormat() ) ) ;

	if ( _openGLcontext != 0 )
		send( "Current OpenGL context: " + _openGLcontext->toString() ) ;


	// Done only once OpenGL has been set:
	if ( _gui != 0 )
	  _gui->postSetModeInit( *screen ) ;



	// Start counting time:
	Ceylan::System::getPreciseTime( _screenStartingSecond,
		_screenStartingMicrosecond ) ;

	_displayInitialized = true ;

	return _screen->getFlags() ;

#else // OSDL_USES_SDL

	throw VideoException( "VideoModule::setMode failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



void VideoModule::resize( Length newWidth, Length newHeight )
{

	send( "Resizing window to (" + Ceylan::toString( newWidth ) + ", "
		+ Ceylan::toString( newHeight ) + ")." ) ;


	/* @fixme OpenGL: restore context (destroyed textures, glViewport etc.)
	if ( _renderer )
	{
		_renderer->unloadTextures() ;
	}
	*/

	if ( _screen == 0 )
		throw VideoException(
			"VideoModule::resize: video mode was not set." ) ;

	if ( ( _openGLcontext != 0 ) && OpenGLContext::ContextIsLostOnResize )
		setMode( newWidth, newHeight, _screen->getBitsPerPixel(),
			_screen->getFlags(), OpenGL::Reload ) ;
	else
		setMode( newWidth, newHeight, _screen->getBitsPerPixel(),
			_screen->getFlags(), OpenGL::None ) ;


	/* @fixme Change viewport:
	if ( _renderer )
	{
		_renderer->viewPortResized( newWidth, newHeight ) ;
	}
	*/

}



void VideoModule::redraw()
{

	if ( _screen == 0 )
		throw VideoException(
			"VideoModule::redraw: video mode was not set." ) ;

	/* @fixme Change
	if ( _renderer )
	{
		_renderer->redraw() ; or wait until next frame?
	}
	else
	*/

	if ( _gui != 0 )
	  _gui->redraw() ;
	else
	  _screen->redraw() ;

}



void VideoModule::toggleFullscreen()
{

#if OSDL_USES_SDL

	if ( _screen == 0 )
		throw VideoException(
			"VideoModule::toggleFullscreen: video mode was not set." ) ;

	if ( SDL_WM_ToggleFullScreen( & _screen->getSDLSurface() ) == 0 )
		throw VideoException( "VideoModule::toggleFullscreen failed: "
			+ Utils::getBackendLastError() ) ;

#endif // OSDL_USES_SDL

}




#if OSDL_ARCH_NINTENDO_DS

#if defined(OSDL_RUNS_ON_ARM9)


/**
 * Writes specified 16-bit value to specified address, with specific byte order.
 *
 */
void write16( Ceylan::Uint16* address, Ceylan::Uint16 value)
{

	Ceylan::Uint8* first  = reinterpret_cast<Ceylan::Uint8*>( address ) ;
	Ceylan::Uint8* second = first + 1 ;

	*first  = value & 0xff ;
	*second = value >>8 ;

}



/**
 * Writes specified 32-bit value to specified address, with specific byte order.
 *
 */
void write32( Ceylan::Uint32* address, Ceylan::Uint32 value)
{

	Ceylan::Uint8* first  = reinterpret_cast<Ceylan::Uint8*>( address ) ;
	Ceylan::Uint8* second = first + 1 ;
	Ceylan::Uint8* third  = first + 2 ;
	Ceylan::Uint8* fourth = first + 3 ;

	*first  =  value      & 0xff ;
	*second = (value>>8)  & 0xff ;
	*third  = (value>>16) & 0xff ;
	*fourth = (value>>24) & 0xff ;

}


#endif //  defined(OSDL_RUNS_ON_ARM9)

#endif // OSDL_ARCH_NINTENDO_DS



void VideoModule::makeBMPScreenshot( const string & screenshotFilename )
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM7

	throw OSDL::Exception( "VideoModule::makeBMPScreenshot failed: "
		"not supported on the ARM7." ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	/*
	 * Generating a BMP file for the content of the main core.
	 *
	 * Inspired from Graphics/capture/ScreenShot/source/screenshot.cpp, from
	 * libnds examples.
	 *
	 * @note VRAM D must be free, as the capture uses it.
	 *
	 */

	/*
	 * @note: pure black screenshot always returned, at least on framebuffer
	 * mode.
	 *
	 */

	try
	{

		vramSetBankD( VRAM_D_LCD ) ;

		/*
		 * Enables capture device and triggers it asynchronously:
		 * (no 'reinterpret_cast<Ceylan::Uint32>(..' allowed)
		 *
		 * DCAP_ENABLE: enables display capturing
		 * DCAP_BANK(3): which VRAM bank to use (0=A, 1=B, 2=C, 3=D)
		 * DCAP_SIZE(3): 3 is full size (256 x 192)
		 * DCAP_SRC(1): capture source [not specified]
		 *
		 */
		REG_DISPCAPCNT = (unsigned int)
			( DCAP_ENABLE | DCAP_BANK(3) | DCAP_SIZE(3) ) ;


		const System::Size sizeOfHeaders = sizeof(TwoDimensional::BMPInfoHeader)
			+ sizeof(TwoDimensional::BMPHeader) ;

		// 3 is for RGB bytes (24-bit):
		const System::Size fileSize =
			SCREEN_WIDTH * SCREEN_HEIGHT * 3 + sizeOfHeaders ;

		// The BMP file will be created in memory first:
		Ceylan::Byte * temp = new Ceylan::Byte[ fileSize ] ;

		TwoDimensional::BMPHeader* header =
			reinterpret_cast<TwoDimensional::BMPHeader*>( temp ) ;

		TwoDimensional::BMPInfoHeader* infoHeader =
			reinterpret_cast<TwoDimensional::BMPInfoHeader*>(
				temp + sizeof(TwoDimensional::BMPHeader) ) ;

		// Fill the headers first:
		write16( &header->type, /* magic value */ 0x4D42 ) ;
		write32( &header->size, fileSize ) ;

		write32( &header->offset, sizeOfHeaders ) ;

		write16( &header->firstReserved, 0 ) ;
		write16( &header->secondReserved, 0 ) ;

		write16( &infoHeader->bits, 24 ) ;
		write32( &infoHeader->size, sizeof(TwoDimensional::BMPInfoHeader) ) ;
		write32( &infoHeader->compression, 0 ) ;
		write32( &infoHeader->width, SCREEN_WIDTH ) ;
		write32( &infoHeader->height, SCREEN_HEIGHT ) ;
		write16( &infoHeader->planes, 1);
		write32( &infoHeader->imagesize, SCREEN_WIDTH*SCREEN_HEIGHT*3 ) ;
		write32( &infoHeader->xresolution, 0 ) ;
		write32( &infoHeader->yresolution, 0 ) ;
		write32( &infoHeader->importantcolours, 0 ) ;
		write32( &infoHeader->ncolours, 0 ) ;

		// Wait until ready (deferred waiting, thus reduced):
		while( REG_DISPCAPCNT & DCAP_ENABLE )
			;

		/*
		dmaCopy( VRAM_D, temp + sizeOfHeaders,
			SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(Ceylan::Uint16) ) ;
		 */

		// Then write the image pixels:
		for( int y=0; y<SCREEN_HEIGHT; y++ )
			for( int x=0; x<SCREEN_WIDTH; x++ )
			{

				// Stores the image upside-down (BMP convention):
				Ceylan::Uint16 color = VRAM_D[
					SCREEN_WIDTH*SCREEN_HEIGHT - (y+1)*SCREEN_WIDTH + x ] ;

				Ceylan::Uint8 b = (color      &31) << 3 ;
				Ceylan::Uint8 g = ((color>>5) &31) << 3 ;
				Ceylan::Uint8 r = ((color>>10)&31) << 3 ;

				temp[ ( (y*SCREEN_WIDTH)+x)*3 + sizeOfHeaders + 0 ] = r ;
				temp[ ( (y*SCREEN_WIDTH)+x)*3 + sizeOfHeaders + 1 ] = g ;
				temp[ ( (y*SCREEN_WIDTH)+x)*3 + sizeOfHeaders + 2 ] = b ;
			}

		// Flushes the DTCM before writing the RAM to file:
		DC_FlushAll() ;

		Ceylan::System::File & outputFile =
			Ceylan::System::File::Create( screenshotFilename ) ;

		outputFile.write( temp, fileSize ) ;

		delete [] temp ;

		delete & outputFile ;

	}
	catch( Ceylan::Exception & e )
	{

		throw VideoException( "VideoModule::makeBMPScreenshot failed: "
			+ e.toString() ) ;
	}


#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

	getScreenSurface().saveBMP( screenshotFilename ) ;

#endif // OSDL_ARCH_NINTENDO_DS

}



bool VideoModule::getEndPointDrawState() const
{

	return _drawEndPoint ;

}



void VideoModule::setEndPointDrawState( bool newState )
{

	_drawEndPoint = newState ;

	// Cached value:
	_DrawEndPoint = newState ;

}



bool VideoModule::getAntiAliasingState() const
{

	return _antiAliasing ;

}



void VideoModule::setAntiAliasingState( bool newState )
{

	_antiAliasing = newState ;

	// Cached value:
	_AntiAliasing = newState ;

}



std::string VideoModule::getDriverName() const
{

	return VideoModule::GetDriverName() ;

}



void VideoModule::setWindowCaption( const string & newTitle,
	 const string & newIconName )
{

#if OSDL_USES_SDL

	SDL_WM_SetCaption( newTitle.c_str(), newIconName.c_str() ) ;

#endif // OSDL_USES_SDL

}



void VideoModule::getWindowCaption( string & title, string & iconName )
{

#if OSDL_USES_SDL

	char newTitle[ 50 ] ;
	char newIconName[ 50 ] ;

	SDL_WM_GetCaption(
		reinterpret_cast<char **>( & newTitle ),
		reinterpret_cast<char **>( & newIconName ) ) ;

	title = newTitle  ;
	iconName = newIconName ;

#endif // OSDL_USES_SDL

}



void VideoModule::setWindowIcon( const std::string & filename )
{

#if OSDL_USES_SDL


	if ( isDisplayInitialized() )
		throw VideoException( "VideoModule::setWindowIcon called whereas "
			"display was already initialized "
			"(VideoModule::setMode was already called)." ) ;

	/*
	 * The mask is a bitmask that describes the shape of the icon:
	 *
	 * - if mask is null (0), then the shape is determined by the colorkey of
	 * icon, if any, or makes the icon rectangular (no transparency) otherwise
	 *
	 * - if mask is non-null (non 0), it has to point to a bitmap with bits set
	 * where the corresponding pixel should be visible
	 *
	 * The format of the bitmap is as follows.
	 * Scanlines come in the usual top-down order.
	 * Each scanline consists of (width / 8) bytes, rounded up.
	 *
	 * The most significant bit of each byte represents the leftmost pixel.
	 *
	 */

	Pixels::ColorElement * mask ;

	Surface * iconSurface ;

	try
	{
		iconSurface = & TwoDimensional::Image::LoadIcon( filename, &mask ) ;
	}
	catch( const TwoDimensional::ImageException & e )
	{
		throw VideoException( "VideoModule::setWindowIcon: "
			+ e.toString() ) ;
	}

	LOG_DEBUG_VIDEO( "Setting icon now" ) ;

	SDL_WM_SetIcon( & iconSurface->getSDLSurface(), mask ) ;
	// No error status available.

	/*
	 * Do not know whether they can/should be freed:
	 * (Valgrind apparently prefers that version)
	 *
	 */
	delete [] mask ;

	delete iconSurface ;

#endif // OSDL_USES_SDL

}



bool VideoModule::iconifyWindow()
{

#if OSDL_USES_SDL

	return ( SDL_WM_IconifyWindow() != 0 ) ;

#else // OSDL_USES_SDL

	return false ;

#endif // OSDL_USES_SDL


}



Ceylan::Float32 VideoModule::getAverageRefreshRate()
{

	if ( _screen == 0 )
		throw VideoException( "VideoModule::getAverageRefreshRate failed: "
			"display is not initialized." ) ;

	System::Second currentSecond ;
	System::Microsecond currentMicrosecond ;

	System::getPreciseTime( currentSecond, currentMicrosecond ) ;

	System::Microsecond duration = System::getDurationBetween(
		_screenStartingSecond, _screenStartingMicrosecond,
		currentSecond, currentMicrosecond ) ;

	return static_cast<Ceylan::Float32>(
		( 1000000.0f / duration ) * _screen->getUpdateCount() ) ;

}



bool VideoModule::isUsingOpenGL() const
{

	return hasOpenGLContext() ;

}


const string VideoModule::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "Video module, " ;

	if ( _screen == 0 )
		res += "no video mode set, " ;
	else
		res += "video mode set, " ;

	if ( _renderer == 0 )
		res += "no video renderer set, " ;
	else
		res += "a video renderer is set, " ;

	if ( _openGLcontext == 0 )
		res += "no available OpenGL context, " ;
	else
		res += "an OpenGL context is available, " ;

	if ( _gui != 0 )
		res += "with GUI support enabled" ;
	else
		res += "with no GUI support enabled" ;

	if ( level == Ceylan::low )
		return res ;

	res += ". " + Ceylan::Module::toString() ;

	if ( _screen != 0 )
		res += " Screen surface information: "
			+ _screen->toString( level ) ;

	if ( _renderer != 0 )
		res += ". Internal renderer: "
			+ _renderer->toString( level ) ;

	if ( _openGLcontext != 0 )
		res += ". Current OpenGL context: "
			+ _openGLcontext->toString( level ) ;

	return res ;

}





// Static section.



bool VideoModule::IsDisplayInitialized()
{

	/*
	 * Display is initialized iff the video module says so, therefore video
	 * module and common module must (necessarily) already exist.
	 *
	 */

	if ( ! OSDL::hasExistingCommonModule() )
		return false ;

	// No exception should ever occur since already tested:
	CommonModule & common = OSDL::getExistingCommonModule() ;

	if ( ! common.hasVideoModule() )
		return false ;

	VideoModule & video = common.getVideoModule() ;

	return video.isDisplayInitialized() ;

}



bool VideoModule::GetEndPointDrawState()
{

#if OSDL_CACHE_OVERALL_SETTINGS

	return _DrawEndPoint ;

#else // OSDL_CACHE_OVERALL_SETTINGS

	// Retrieves the video module to read the actual official value:

	if ( ! OSDL::hasExistingCommonModule() )
		Ceylan::emergencyShutdown(
			"VideoModule::GetEndPointDrawState() called "
			"whereas no common module available." ) ;

	// No exception should ever occur since already tested:
	CommonModule & common = OSDL::getExistingCommonModule() ;

	if ( ! common.hasVideoModule() )
		Ceylan::emergencyShutdown(
			"VideoModule::GetEndPointDrawState() called "
			"whereas no video module available." ) ;

	VideoModule & video = common.getVideoModule() ;

#if OSDL_DEBUG_CACHED_STATES

	bool realState = video.getEndPointDrawState() ;

	if ( realState != _DrawEndPoint )
		Ceylan::emergencyShutdown(
			"VideoModule::GetEndPointDrawState() inconsistency detected: "
			"cached value should have been "
			+ Ceylan::toString( realState ) + "." ) ;

	return realState ;

#else // OSDL_DEBUG_CACHED_STATES

	return video.getEndPointDrawState() ;

#endif // OSDL_DEBUG_CACHED_STATES

#endif // OSDL_CACHE_OVERALL_SETTINGS

}



bool VideoModule::GetAntiAliasingState()
{

#if OSDL_CACHE_OVERALL_SETTINGS

	return _AntiAliasing ;

#else // OSDL_CACHE_OVERALL_SETTINGS

	if ( ! OSDL::hasExistingCommonModule() )
		Ceylan::emergencyShutdown(
			"VideoModule::GetAntiAliasingState() called "
			"whereas no common module available." ) ;

	// No exception should ever occur since already tested:
	CommonModule & common = OSDL::getExistingCommonModule() ;

	if ( ! common.hasVideoModule() )
		Ceylan::emergencyShutdown(
			"VideoModule::GetAntiAliasingState() called "
			"whereas no video module available." ) ;

	VideoModule & video = common.getVideoModule() ;

#if OSDL_DEBUG_CACHED_STATES

	bool realState = video.getAntiAliasingState() ;

	if ( realState != _AntiAliasing )
		Ceylan::emergencyShutdown(
			"VideoModule::GetAntiAliasingState() inconsistency detected: "
			"cached value should have been "
			+ Ceylan::toString( realState ) + "." ) ;

	return realState ;

#else // OSDL_DEBUG_CACHED_STATES

	return video.getAntiAliasingState() ;

#endif // OSDL_DEBUG_CACHED_STATES

#endif // OSDL_CACHE_OVERALL_SETTINGS

}



string VideoModule::GetDriverName()
{

#if OSDL_USES_SDL

	char driverName[ VideoModule::DriverNameMaximumLength + 1 ]  ;

	if ( SDL_VideoDriverName( driverName, DriverNameMaximumLength ) == 0 )
		throw VideoException( "VideoModule::GetDriverName failed: "
			"the video driver is probably not initialized." ) ;

	return std::string( driverName ) ;

#else // OSDL_USES_SDL

	return "unknown (not using SDL)" ;

#endif // OSDL_USES_SDL

}



string VideoModule::InterpretFlags( Ceylan::Flags flags )
{

	std::list<string> res ;

	/*
	 * As the Software (SDL_SWSURFACE) flag is null (0), the test had no real
	 * meaning:
	 *
	 */

	/*
	if ( flags & SoftwareSurface )
		res.push_back(
			"Video surface requested to be created in system memory "
			"(SoftwareSurface is set)." ) ;
	else
		res.push_back(
			"Video surface not requested to be created in system memory "
			"(SoftwareSurface not set)." ) ;
	*/

	if ( flags & HardwareSurface )
		res.push_back(
			"Display surface requested to be created in video memory "
			"(HardwareSurface is set)." ) ;
	else
		res.push_back(
			"Display surface not requested to be created in video memory "
			"(HardwareSurface not set)." ) ;

	if ( flags & AsynchronousBlit )
		res.push_back(
			"Use, if possible, asynchronous updates of the display surface "
			"(AsynchronousBlit is set)." ) ;
	else
		res.push_back(
			"Do not use asynchronous blits (AsynchronousBlit not set)." ) ;

	if ( flags & AnyPixelFormat )
		res.push_back(
			"Display surface should use the available video surface, "
			"regardless of its pixel depth (AnyPixelFormat is set)." ) ;
	else
		res.push_back(
			"Display surface will emulate, thanks to a shadow surface, "
			"the specified bit-per-pixel mode if not available "
			"(AnyPixelFormat not set)." ) ;

	if ( flags & ExclusivePalette )
		res.push_back(
			"Display surface is given exclusive palette access, "
			"in order to always have the requested colors "
			"(ExclusivePalette is set)." ) ;
	else
		res.push_back(
			"Display surface is not required to have exclusive palette access, "
			"some colors might be different from the requested ones "
			"(ExclusivePalette not set)." ) ;

	if ( flags & DoubleBuffered )
		res.push_back(
			"Enable hardware double buffering, "
			"only valid with display surface in video memory "
			"and without OpenGL (DoubleBuffered is set)." ) ;
	else
		res.push_back( "No hardware double buffering requested, or OpenGL used "
			"(DoubleBuffered not set)." ) ;

	if ( flags & FullScreen )
		res.push_back(
			"Display surface should attempt to use full screen mode. "
			"If a hardware resolution change is not possible "
			"(for whatever reason), the next higher resolution "
			"will be used and the display window centered "
			"on a black background (FullScreen is set)." ) ;
	else
		res.push_back(
			"No full screen mode requested (FullScreen not set)." ) ;

	if ( flags & OpenGL )
		res.push_back(
			"Display surface should have an OpenGL rendering context, "
			"whose video attributes should already have been set "
			"(OpenGL is set)." ) ;
	else
		res.push_back(
			"Display surface is not requested to have an OpenGL context "
			"(OpenGL not set)." ) ;

	// Deprecated flag:

	if ( flags & 0x0000000A /* SDL_OPENGLBLIT */ )
			res.push_back(
				"Display surface should have an OpenGL rendering context, "
				"and will allow normal blitting operations. "
				"This SDL_OPENGLBLIT flag is deprecated and "
				"should not be used for new applications." ) ;

	/*
	 * Not even try to advertise deprecated SDL_OPENGLBLIT:

	else
			res.push_back( "The deprecated SDL_OPENGLBLIT flag is not set "
				"and this is how it should be." ) ;
	 */

	if ( flags & Resizable )
		res.push_back( "Requests a resizable window (Resizable is set)." ) ;
	else
		res.push_back( "No resizable window requested (Resizable not set)." ) ;


	if ( flags & NoFrame )
		res.push_back(
			"Requests creation of a window with no title bar or "
			"frame decoration (NoFrame is set)" ) ;
	else
		res.push_back(
			"Title bar and frame decoration allowed for display window "
			"(NoFrame not set)." ) ;

	return "The specified display surface mode flags, whose value is "
		+ Ceylan::toString( flags, /* bit field */ true )
		+ ", mean: " + Ceylan::formatStringList( res ) ;

}



bool VideoModule::HardwareSurfacesCanBeCreated()
{

#if OSDL_USES_SDL

	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::HardwareSurfacesCanBeCreated: "
			"unable to retrieve video informations." ) ;

	return static_cast<bool>( videoInfo->hw_available ) ;

#else // OSDL_USES_SDL

	throw VideoException( "VideoModule::HardwareSurfacesCanBeCreated failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



bool VideoModule::WindowManagerAvailable()
{

#if OSDL_USES_SDL

	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::WindowManagerAvailable: "
			"unable to retrieve video informations." ) ;

	return static_cast<bool>( videoInfo->wm_available ) ;

#else // OSDL_USES_SDL

	throw VideoException( "VideoModule::WindowManagerAvailable failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



bool VideoModule::HardwareToHardwareBlitsAccelerated()
{

#if OSDL_USES_SDL


	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::HardwareToHardwareBlitsAccelerated: "
			"unable to retrieve video informations." ) ;

	return static_cast<bool>( videoInfo->blit_hw ) ;

#else // OSDL_USES_SDL

	throw VideoException(
		"VideoModule::HardwareToHardwareBlitsAccelerated failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



bool VideoModule::HardwareToHardwareColorkeyBlitsAccelerated()
{

#if OSDL_USES_SDL


	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::HardwareToHardwareColorkeyBlitsAccelerated: "
			"unable to retrieve video informations." ) ;

	return static_cast<bool>( videoInfo->blit_hw_CC ) ;

#else // OSDL_USES_SDL

	throw VideoException(
		"VideoModule::HardwareToHardwareColorkeyBlitsAccelerated failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



bool VideoModule::HardwareToHardwareAlphaBlitsAccelerated()
{

#if OSDL_USES_SDL

	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::HardwareToHardwareAlphaBlitsAccelerated: "
			"unable to retrieve video informations." ) ;

	return static_cast<bool>( videoInfo->blit_hw_A ) ;

#else // OSDL_USES_SDL

	throw VideoException(
		"VideoModule::HardwareToHardwareAlphaBlitsAccelerated failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



bool VideoModule::SoftwareToHardwareBlitsAccelerated()
{

#if OSDL_USES_SDL

	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::SoftwareToHardwareBlitsAccelerated: "
			"unable to retrieve video informations." ) ;

	return static_cast<bool>( videoInfo->blit_sw ) ;

#else // OSDL_USES_SDL

	throw VideoException(
		"VideoModule::SoftwareToHardwareBlitsAccelerated failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



bool VideoModule::SoftwareToHardwareColorkeyBlitsAccelerated()
{

#if OSDL_USES_SDL

	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::SoftwareToHardwareColorkeyBlitsAccelerated: "
			"unable to retrieve video informations." ) ;

	return static_cast<bool>( videoInfo->blit_sw_CC ) ;

#else // OSDL_USES_SDL

	throw VideoException(
		"VideoModule::SoftwareToHardwareColorkeyBlitsAccelerated failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



bool VideoModule::SoftwareToHardwareAlphaBlitsAccelerated()
{

#if OSDL_USES_SDL


	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::SoftwareToHardwareAlphaBlitsAccelerated: "
			"unable to retrieve video informations." ) ;

	return static_cast<bool>( videoInfo->blit_sw_A ) ;

#else // OSDL_USES_SDL

	throw VideoException(
		"VideoModule::SoftwareToHardwareAlphaBlitsAccelerated failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



bool VideoModule::ColorFillsAccelerated()
{

#if OSDL_USES_SDL

	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::ColorFillsAccelerated: "
			"unable to retrieve video informations." ) ;

	return static_cast<bool>( videoInfo->blit_fill ) ;

#else // OSDL_USES_SDL

	throw VideoException(
		"VideoModule::ColorFillsAccelerated failed: no SDL support available"
	) ;

#endif // OSDL_USES_SDL

}



Ceylan::Uint32 VideoModule::GetVideoMemorySize()
{

#if OSDL_USES_SDL

	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::GetVideoMemorySize: "
			"unable to retrieve video informations." ) ;

	return static_cast<Ceylan::Uint32>( videoInfo->video_mem ) ;

#else // OSDL_USES_SDL

	throw VideoException( "VideoModule::GetVideoMemorySize failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



Pixels::PixelFormat VideoModule::GetVideoDevicePixelFormat()
{

#if OSDL_USES_SDL

	const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo() ;

	if ( videoInfo == 0 )
		throw( "VideoModule::GetVideoDevicePixelFormat: "
			"unable to retrieve video informations." ) ;

	return * static_cast<Pixels::PixelFormat *>( videoInfo->vfmt ) ;

#else // OSDL_USES_SDL

	throw VideoException( "VideoModule::GetVideoDevicePixelFormat failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



string VideoModule::DescribeVideoCapabilities()
{

	string result ;

	try
	{
		result = "Video device capabilities for "
		+ Ceylan::Network::getMostPreciseLocalHostName()
		+ " are:" ;
	}
	catch ( const Ceylan::Network::NetworkException & e )
	{
		LogPlug::error( "VideoModule::getVideoCapabilities: "
			"error when retrieving host name: "
			+ e.toString() ) ;
		result = "Video device capabilities are:" ;

	}

	std::list<string> l ;

	if ( HardwareSurfacesCanBeCreated() )
		l.push_back( "Hardware surfaces can be created." ) ;
	else
		l.push_back( "Only software surfaces can be created." ) ;

	if ( WindowManagerAvailable() )
		l.push_back( "A window manager is available." ) ;
	else
		l.push_back( "No window manager available." ) ;



	if ( HardwareToHardwareBlitsAccelerated() )
		l.push_back( "Hardware to hardware blits are accelerated." ) ;
	else
		l.push_back( "No hardware to hardware blit acceleration available." ) ;


	if ( HardwareToHardwareColorkeyBlitsAccelerated() )
		l.push_back( "Hardware to hardware colorkey blits are accelerated." ) ;
	else
		l.push_back(
			"No hardware to hardware colorkey blit acceleration available." ) ;


	if ( HardwareToHardwareAlphaBlitsAccelerated() )
		l.push_back( "Hardware to hardware alpha blits are accelerated." ) ;
	else
		l.push_back(
			"No hardware to hardware alpha blit acceleration available." ) ;



	if ( SoftwareToHardwareBlitsAccelerated() )
		l.push_back( "Software to hardware blits are accelerated." ) ;
	else
		l.push_back(
			"No software to hardware blit acceleration available." ) ;


	if ( SoftwareToHardwareColorkeyBlitsAccelerated() )
		l.push_back( "Software to hardware colorkey blits are accelerated." ) ;
	else
		l.push_back(
			"No software to hardware colorkey blit acceleration available." ) ;


	if ( SoftwareToHardwareAlphaBlitsAccelerated() )
		l.push_back( "Software to hardware alpha blits are accelerated." ) ;
	else
		l.push_back(
			"No software to hardware alpha blit acceleration available." ) ;



	if ( ColorFillsAccelerated() )
		l.push_back( "Color fills are accelerated." ) ;
	else
		l.push_back( "No color fill acceleration available." ) ;


	if ( GetVideoMemorySize() )
		l.push_back( "Total amount of video memory is "
			+ Ceylan::toString( GetVideoMemorySize() ) + " kilobytes." ) ;
	else
		l.push_back( "Unable to get total amount of video memory." ) ;

	l.push_back( Pixels::toString( GetVideoDevicePixelFormat() ) ) ;


	return result + Ceylan::formatStringList( l ) ;

}



bool VideoModule::AreDefinitionsRestricted( list<Definition> & definitions,
	Ceylan::Flags flags, Pixels::PixelFormat * pixelFormat )
{

#if OSDL_USES_SDL

	SDL_Rect ** modes ;

#if OSDL_DEBUG_VIDEO
	LOG_TRACE_VIDEO( "VideoModule::AreDefinitionsRestricted: "
		"getting available modes.", 8 ) ;
#endif // OSDL_DEBUG_VIDEO

	modes = SDL_ListModes( pixelFormat, flags ) ;


	/*
	 * Beware to 64-bit machines.
	 *
	 * Basically we are trying to convert a pointer to an int, gcc on Linux will
	 * not accept Ceylan::Uint32 to become Ceylan::Uint64, whereas Visual C++ on
	 * 32 bit will find a pointer truncation from 'SDL_Rect **' to
	 * Ceylan::SignedLongInteger:
	 *
	 */
#ifdef OSDL_RUNS_ON_WINDOWS

	Ceylan::Uint64 numericModes =
		reinterpret_cast<Ceylan::Uint64>( modes ) ;

#else // OSDL_RUNS_ON_WINDOWS

	/*
	 * long and void * have the same size in 32 bit-platforms (4 bytes) and on
	 * 64 bit ones (8 bytes).
	 *
	 */
	Ceylan::SignedLongInteger numericModes =
		reinterpret_cast<Ceylan::SignedLongInteger>( modes ) ;

#endif // OSDL_RUNS_ON_WINDOWS


	if ( numericModes == 0 )
	{
		LOG_DEBUG_VIDEO( "No screen dimensions available for format "
			+ Pixels::toString( * pixelFormat ) + " !" ) ;

		// List stays empty, and restricted result is true: nothing available.
		return true ;

	}

	// From that point, at least one dimension will be available.

	// Checking whether our resolution is restricted.
	if ( numericModes == -1 )
	{

		// Any dimension is okay for the given format, not restricted:
		return false ;

	}

	// In this last case, only a set of definitions is available:

	for ( Ceylan::Uint16 i = 0 ; modes[ i ] ; i++)
	{
		definitions.push_back( pair<Length, Length>(
			modes[ i ]->w, modes[ i ]->h ) ) ;
	}

	return true ;

#else // OSDL_USES_SDL

	throw VideoException( "VideoModule::AreDefinitionsRestricted failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



string VideoModule::DescribeAvailableDefinitions( Ceylan::Flags flags,
	Pixels::PixelFormat * pixelFormat )
{

#if OSDL_USES_SDL

	list<Definition> defList ;

#if OSDL_DEBUG_VIDEO
	LOG_TRACE_VIDEO( "VideoModule::DescribeAvailableDefinitions: "
		"getting available modes.", 8 ) ;
#endif // OSDL_DEBUG_VIDEO

	// Checking whether there is any mode available.

	if ( ! AreDefinitionsRestricted( defList, flags, pixelFormat ) )
	{

		if ( pixelFormat == 0 )
			return "All screen dimensions are supported for "
				"the best possible video mode" ;
		else
			return "All screen dimensions are supported for "
				"the given pixel format" ;

	}
	else // definitions are restricted:
	{

		if ( defList.empty() )
		{
			return "No screen dimension available for "
				"specified format and flags" ;
		}
		else
		{

			string result ;

			list<string> l ;

			// Print valid modes.

			if ( pixelFormat == 0 )
				result = "Screen dimensions supported for "
					"the best possible video mode are:" ;
			else
				result = "Screen dimensions supported for "
					"the given pixel format are:" ;

			for ( list<Definition>::const_iterator it = defList.begin();
				it != defList.end(); it++ )
			{
				l.push_back( Ceylan::toString( (*it).first )
					+ 'x' +  Ceylan::toString( (*it).second ) ) ;
			}

			return result + Ceylan::formatStringList( l ) ;

		}

	}

#else // OSDL_USES_SDL

	throw VideoException( "VideoModule::DescribeAvailableDefinitions: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



StandardScreenSize VideoModule::ToStandardScreenSize(
	const std::string & standardScreenSizeInText )
{

	StandardScreenSize screenSize ;

	try
	{

		screenSize = static_cast<StandardScreenSize>(
			Ceylan::stringToUnsignedLong( standardScreenSizeInText ) ) ;

	}
	catch( const Ceylan::Exception & e )
	{

		throw VideoException( "VideoModule::ToStandardScreenSize: "
			"could not convert '" + standardScreenSizeInText
			+ "' into a number designating a screen size." ) ;
	}

	switch( screenSize )
	{

		case Size_640x480:
		case Size_800x600:
		case Size_1024x780:
		case Size_1280x1024:
		case Size_1680x1050:
		case Size_FullScreen:
			return screenSize ;
			break ;

		default:
			throw VideoException( "VideoModule::ToStandardScreenSize: "
				"'" + Ceylan::toString(screenSize)
				+ "' is not a known standard screen size." ) ;

	}

}



void VideoModule::ToScreenDimensions( StandardScreenSize screenSize,
	Length & screenWidth, Length & screenHeight )
{

	switch( screenSize )
	{

		case Size_640x480:
			screenWidth  = 640 ;
			screenHeight = 480 ;
			break ;

		case Size_800x600:
			screenWidth  = 800 ;
			screenHeight = 600 ;
			break ;

		case Size_1024x780:
			screenWidth  = 1024 ;
			screenHeight = 780 ;
			break ;

		case Size_1280x1024:
			screenWidth  = 1280 ;
			screenHeight = 1024;
			break ;

		case Size_1680x1050:
			screenWidth  = 1680 ;
			screenHeight = 1050 ;
			break ;

		case Size_FullScreen:
			throw VideoException( "VideoModule::ToScreenDimensions: "
				"'" + Ceylan::toString(screenSize)
				+ "' is a logical size which cannot be converted statically "
				" into a physical one." ) ;
			break ;

		default:
			throw VideoException( "VideoModule::ToScreenDimensions: "
				"'" + Ceylan::toString(screenSize)
				+ "' is not a known standard screen size." ) ;

	}

}



string VideoModule::DescribeEnvironmentVariables()
{

	Ceylan::Uint16 varCount =
		sizeof( _SDLEnvironmentVariables ) / sizeof (string) ;

	string result = "Examining the " + Ceylan::toString( varCount )
		+ " video-related environment variables for SDL backend:" ;

	list<string> variables ;

	string var, value ;

	TextOutputFormat htmlFormat = Ceylan::TextDisplayable::GetOutputFormat() ;

	for ( Ceylan::Uint16 i = 0; i < varCount; i++ )
	{

		var = _SDLEnvironmentVariables[ i ] ;
		value = Ceylan::System::getEnvironmentVariable( var ) ;

		if ( value.empty() )
		{
			if ( htmlFormat == TextDisplayable::html )
			{
				variables.push_back( "<em>" + var + "</em> is not set." ) ;
			}
			else
			{
				variables.push_back( var + " is not set." ) ;
			}
		}
		else
		{
			if ( htmlFormat == TextDisplayable::html )
			{
				variables.push_back( "<b>" + var + "</b> set to ["
					+ value + "]." ) ;
			}
			else
			{
				variables.push_back( var + " set to [" + value + "]." ) ;
			}
		}

	}

	return result + Ceylan::formatStringList( variables ) ;

}



bool VideoModule::IsUsingOpenGL()
{

  return _IsUsingOpenGL ;

}


bool VideoModule::IsUsingDrawingPrimitives()
{

#if OSDL_USES_SDL_GFX

  return true ;

#else // OSDL_USES_SDL_GFX

  return false ;

#endif // OSDL_USES_SDL_GFX

}

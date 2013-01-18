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


#include "OSDLOpenGL.h"

#include "OSDLVideo.h"        // for VideoModule
#include "OSDLGLTexture.h"    // for SetTextureFlavour
#include "OSDLUtils.h"        // for getBackendLastError



// for CEYLAN_DETECTED_LITTLE_ENDIAN, openGLContextsCanBeLost, etc.:
#include "Ceylan.h"


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>   // for OSDL_USES_OPENGL and al
#endif // OSDL_USES_CONFIG_H

#ifdef OSDL_HAVE_OPENGL
#include "SDL_opengl.h"   // for GL, GLU
#endif // OSDL_HAVE_OPENGL


#if OSDL_USES_SDL
#include "SDL.h"          // for SDL_Cursor, etc.
#endif // OSDL_USES_SDL



#ifdef OSDL_RUNS_ON_WINDOWS

// Microsoft stupidly managed to redefine symbols in an header (windef.h):

#ifdef near
#undef near
#endif // near

#ifdef far
#undef far
#endif // far

#endif // OSDL_RUNS_ON_WINDOWS



using std::string ;

#include <list>
using std::list ;

using namespace OSDL::Video ;
using namespace OSDL::Video::OpenGL ;
using namespace OSDL::Video::OpenGL::GLU ;

using namespace Ceylan::Log ;



/*
 * All known SDL+OpenGL attributes are:

	SDL_GL_RED_SIZE,
	SDL_GL_GREEN_SIZE,
	SDL_GL_BLUE_SIZE,
	SDL_GL_ALPHA_SIZE,
	SDL_GL_BUFFER_SIZE,
	SDL_GL_DOUBLEBUFFER,
	SDL_GL_DEPTH_SIZE,
	SDL_GL_STENCIL_SIZE,
	SDL_GL_ACCUM_RED_SIZE,
	SDL_GL_ACCUM_GREEN_SIZE,
	SDL_GL_ACCUM_BLUE_SIZE,
	SDL_GL_ACCUM_ALPHA_SIZE,
	SDL_GL_STEREO,
	SDL_GL_MULTISAMPLEBUFFERS,
	SDL_GL_MULTISAMPLESAMPLES,
	SDL_GL_ACCELERATED_VISUAL,
	SDL_GL_SWAP_CONTROL

 *
 */




/*
 * Implementation notes:
 *
 * OSDL_USES_OPENGL being not defined not always triggers an exception in all
 * non-static methods, as the constructor would have done that already.
 *
 * Dynamic loading of OpenGL functions could be performed.
 *
 * It would involve a global shared pointer to a structure containing all needed
 * function pointers, loaded at start-up.
 *
 * See also: /SDL-1.2.x/test/testdyngl.c
 * Agar relies on static loading too.
 *
 */


// OpenGL RGBA masks, since it always assumes RGBA order:

#if CEYLAN_DETECTED_LITTLE_ENDIAN

OSDL::Video::Pixels::ColorMask OSDL::Video::OpenGL::RedMask   = 0x000000ff ;
OSDL::Video::Pixels::ColorMask OSDL::Video::OpenGL::GreenMask = 0x0000ff00 ;
OSDL::Video::Pixels::ColorMask OSDL::Video::OpenGL::BlueMask  = 0x00ff0000 ;
OSDL::Video::Pixels::ColorMask OSDL::Video::OpenGL::AlphaMask = 0xff000000 ;


#else // CEYLAN_DETECTED_LITTLE_ENDIAN

OSDL::Video::Pixels::ColorMask OSDL::Video::OpenGL::RedMask   = 0xff000000 ;
OSDL::Video::Pixels::ColorMask OSDL::Video::OpenGL::GreenMask = 0x00ff0000 ;
OSDL::Video::Pixels::ColorMask OSDL::Video::OpenGL::BlueMask  = 0x0000ff00 ;
OSDL::Video::Pixels::ColorMask OSDL::Video::OpenGL::AlphaMask = 0x000000ff ;

#endif // CEYLAN_DETECTED_LITTLE_ENDIAN





OpenGLException::OpenGLException( const std::string & reason ):
	VideoException( reason )
{

}


OpenGLException::~OpenGLException() throw()
{

}



const bool OpenGLContext::ContextCanBeLost =
	Ceylan::System::openGLContextsCanBeLost() ;


const bool OpenGLContext::ContextIsLostOnResize =
	Ceylan::System::openGLContextsLostOnResize() ;


const bool OpenGLContext::ContextIsLostOnApplicationSwitch =
	Ceylan::System::openGLContextsLostOnApplicationSwitch() ;


const bool OpenGLContext::ContextIsLostOnColorDepthChange =
	Ceylan::System::openGLContextsLostOnColorDepthChange() ;



const GLLength OpenGLContext::DefaultOrthographicWidth = 1000.0f ;

const GLCoordinate OpenGLContext::DefaultNearClippingPlaneFor2D =     -1.0f ;
const GLCoordinate OpenGLContext::DefaultFarClippingPlaneFor2D  =      1.0f ;

const GLCoordinate OpenGLContext::DefaultNearClippingPlaneFor3D =      1.0f	;
const GLCoordinate OpenGLContext::DefaultFarClippingPlaneFor3D  = 100000.0f ;




OpenGLContext::OpenGLContext( OpenGL::Flavour flavour, BitsPerPixel plannedBpp,
		Length viewportWidth, Length viewportHeight ) :
	_flavour( OpenGL::None ),
	_redSize( 0 ),
	_greenSize( 0 ),
	_blueSize( 0 ),
	_alphaSize( 0 ),
	_viewportWidth( viewportWidth ),
	_viewportHeight( viewportHeight ),
	_projectionMode( Orthographic ),
	_projectionWidth( DefaultOrthographicWidth ),
	_nearClippingPlane( DefaultNearClippingPlaneFor2D ),
	_farClippingPlane( DefaultFarClippingPlaneFor2D ),
	_clearColor()
{

#if OSDL_USES_OPENGL

	// Note: plannedBpp and _*Size not used currently.

	LogPlug::trace( "OpenGLContext constructor" ) ;

	selectFlavour( flavour ) ;

	setClearColor( Pixels::Black ) ;
	clearViewport() ;

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext constructor failed: "
		"OpenGL support not available." ) ;

#endif // OSDL_USES_OPENGL

}



OpenGLContext::~OpenGLContext() throw()
{

	LogPlug::trace( "OpenGLContext destructor" ) ;

}




void OpenGLContext::selectFlavour( Flavour flavour )
{

	LogPlug::trace( "OpenGLContext::selectFlavour" ) ;

	/*
	 * During execution an application may perfectly go for example from 2D to
	 * 3D while the display is already initialized:

	if ( VideoModule::IsDisplayInitialized() )
		LogPlug::warning(
			"OpenGLContext::selectFlavour: display is already initialized." ) ;
	 *
	 */

	switch( flavour )
	{

		case None:
			_flavour = None ;
			// Nothing to do.
			return ;
			break ;

		case OpenGLFor2D:
			// Sets _flavour as well:
			set2DFlavour() ;
			break ;

		case OpenGLFor3D:
			// Sets _flavour as well:
			set3DFlavour() ;
			return ;
			break ;

		case Reload:
			// _flavour left as is:
			reload() ;
			return ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext:selectFlavour: "
				"unknown flavour selected." ) ;
			break ;

	}

	/*
	 * Needed, as will update the projection which depends on the flavour:
	 * (viewport size not changing here)
	 *
	 */
	setViewPort( _viewportWidth, _viewportHeight ) ;

}



void OpenGLContext::set2DFlavour()
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext::set2DFlavour" ) ;

	_flavour = OpenGLFor2D ;

	/*
	 * Saves a lot of the OpenGL state machine:
	 *
	 * Actually setting explicitly the OpenGL state is preferred here to
	 * pushing/popping it (more reliable).
	 *
	 */
	//pushAttribute( GL_ENABLE_BIT ) ;

	// No depth sorting used:
	setDepthBufferStatus( false ) ;


	/*
	 * setOrthographicProjectionFor2D not called here, as it is automatically
	 * triggered whenever the screen is resized: the screen resize triggers a
	 * viewport resize (setViewPort) which triggers the projection update
	 * (updateProjection).
	 *
	 * VideoModule::setMode will call setViewPort after having managed the
	 * OpenGL flavour, OpenGLContext::selectFlavour will do the same.
	 *
	 */


	// No culling of faces used:
	OpenGLContext::DisableFeature( CullPolygons ) ;

	setShadingModel( Flat ) ;


	/*
	 * Blends the incoming RGBA color values with the values in the color
	 * buffers:
	 *
	 */
	OpenGLContext::EnableFeature( Alphablending ) ;

	/*
	 * Ponders the source color components by its alpha coordinate Ac, and the
	 * destination by (1-Ac):
	 *
	 */
	setBlendingFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) ;

	GLTexture::SetTextureFlavour( GLTexture::For2D ) ;


	// Model-view section:
	glMatrixMode( GL_MODELVIEW ) ;

	// No 'glPushMatrix()', just erase the modelview matrix content:
	glLoadIdentity() ;

	/*
	 * See in http://www.opengl.org/resources/faq/technical/
	 * transformations.htm#tran0030 for explanation about exact pixelization:
	 *
	 */
	glTranslatef( 0.375, 0.375, 0.0 ) ;

	// From here all primitives can be rendered at integer positions.


#endif // OSDL_USES_OPENGL

}



void OpenGLContext::set3DFlavour()
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext::set3DFlavour" ) ;

	_flavour = OpenGLFor3D ;

	/*
	 * Saves a lot of the OpenGL state machine:
	 *
	 * Actually setting explicitly the OpenGL state is preferred here to
	 * pushing/popping it (more reliable).
	 *
	 */
	//pushAttribute( GL_ENABLE_BIT ) ;


	// Depth tests wanted:
	EnableFeature( DepthTests ) ;

	setShadingModel( Smooth ) ;

	/*
	 * Blends the incoming RGBA color values with the values in the color
	 * buffers:
	 *
	 */
	EnableFeature( Alphablending ) ;

	/*
	 * Ponders the source color components by its alpha coordinate Ac, and the
	 * destination by (1-Ac):
	 *
	 */
	setBlendingFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) ;

	GLTexture::SetTextureFlavour( GLTexture::For3D ) ;

	glMatrixMode( GL_PROJECTION ) ;
	//glPushMatrix() ;
	glLoadIdentity() ;

	gluPerspective( /* angle of view */ 45.0f,
		/* aspect */ static_cast<GLfloat>( _viewportWidth ) /
			static_cast<GLfloat>( _viewportHeight ),
		/* near z plane */ 1.0f,
		/* far z plane */ 100 ) ;

	// Model-view section:
	glMatrixMode( GL_MODELVIEW ) ;

	// No 'glPushMatrix()', just erase the modelview matrix content:
	glLoadIdentity() ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::blank()
{

	LogPlug::warning( "OpenGLContext::blank not implemented yet." ) ;

}



void OpenGLContext::reload()
{

	setClearColor( _clearColor ) ;
	clearViewport() ;

	updateProjection() ;

	LogPlug::warning( "OpenGLContext::reload not fully implemented yet." ) ;

}



void OpenGLContext::setBlendingFunction( GLEnumeration sourceFactor,
	GLEnumeration destinationFactor )
{

#if OSDL_USES_OPENGL

	glBlendFunc( sourceFactor, destinationFactor ) ;

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_ENUM:
			throw OpenGLException( "OpenGLContext::setBlendingFunction: "
				"a factor is not an accepted value." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setBlendingFunction: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::setBlendingFunction: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setBlendingFunction failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setShadingModel( ShadingModel newShadingModel )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	switch( newShadingModel )
	{

		case Flat:
			glShadeModel( GL_FLAT ) ;
			break ;

		case Smooth:
			glShadeModel( GL_SMOOTH ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::setShadingModel: "
				"unknown shading model specified." ) ;
			break ;

	}

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_ENUM:
			throw OpenGLException( "OpenGLContext::setShadingModel: "
				"unexpected value for shading model." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setShadingModel: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::setShadingModel: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::setShadingModel failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setShadingModel failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setCullingStatus( bool newStatus )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	if ( newStatus )
		EnableFeature( CullPolygons ) ;
	else
		DisableFeature( CullPolygons ) ;

#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::setCullingStatus failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setCullingStatus failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setCulling( CulledFacet culledFacet,
	FrontOrientation frontOrientation, bool autoEnable )
{

#if OSDL_USES_OPENGL

	switch( culledFacet )
	{

		case Front:
			glCullFace( GL_FRONT ) ;
			break ;

		case Back:
			glCullFace( GL_BACK ) ;
			break ;

		case FrontAndBack:
			glCullFace( GL_FRONT_AND_BACK ) ;
			break ;
	}


#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_ENUM:
			throw OpenGLException( "OpenGLContext::setCulling (facet): "
				"unexpected culled facet selection." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setCulling (facet): "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::setCulling (facet): "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


	switch( frontOrientation )
	{

		case Clockwise:
			glFrontFace( GL_CW ) ;
			break ;

		case CounterClockwise:
			glFrontFace( GL_CCW ) ;
			break ;

	}


#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_ENUM:
			throw OpenGLException( "OpenGLContext::setCulling: (orientation)"
				"unexpected front orientation selection." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setCulling: (orientation)"
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::setCulling (orientation): "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


	if ( autoEnable )
		setCullingStatus( true ) ;

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setCulling failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setDepthBufferStatus( bool newStatus )
{

	// Later: add glDepthFunc support.

#if OSDL_USES_OPENGL

	if ( newStatus )
	{
		EnableFeature( DepthTests ) ;
		clearDepthBuffer() ;
	}
	else
	{
		DisableFeature( DepthTests ) ;
	}

#else // OSDL_USES_OPENGL

	throw OpenGLException(
		"OpenGLContext::setDepthBufferStatus failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setViewPort( Length width, Length height,
	const TwoDimensional::Point2D & lowerLeftCorner )
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext::setViewPort called for "
		+ Ceylan::toString( width ) + "x" + Ceylan::toString( height )
		+ " from lower left corner" + lowerLeftCorner.toString() ) ;

	_viewportWidth  = width  ;
	_viewportHeight = height ;

	glViewport( lowerLeftCorner.getX(), lowerLeftCorner.getY(),
		width, height ) ;


#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_VALUE:
			throw OpenGLException( "OpenGLContext::setViewPort: "
				"either width or height is negative." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setViewPort: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::setViewPort: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


	// Restores the aspect ratio of the projection to match the new viewport:
	updateProjection() ;


#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setViewPort failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setOrthographicProjection( GLLength width,
	GLCoordinate near, GLCoordinate far )
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext::setOrthographicProjection" ) ;

	_projectionMode  = Orthographic ;
	_projectionWidth = width ;

	glMatrixMode( GL_PROJECTION ) ;
	glPushMatrix() ;
	glLoadIdentity() ;

	// Centers projection box in (x,y) planes:

	GLCoordinate right = width / 2 ;

	// Enforces the viewport aspect ratio:
	GLCoordinate top = ( width * _viewportHeight ) / ( 2 * _viewportWidth ) ;

	glOrtho( /* left */ -right, /* right */ right,
		/* bottom */ -top, /* top */ top,
		/* near */ near, /* far */ far ) ;


#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setOrthographicProjection: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			LogPlug::warning( "OpenGLContext::setOrthographicProjection: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setOrthographicProjection failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setOrthographicProjectionFor2D(
	GLLength width, GLLength height )
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext::setOrthographicProjectionFor2D" ) ;

	_projectionMode  = Orthographic ;
	_projectionWidth = width ;

	glMatrixMode( GL_PROJECTION ) ;

	// No 'glPushMatrix()', just erase the projection matrix content:
	glLoadIdentity() ;

	/*
	 * Note the reversed top/bottom settings to avoid an upside-down
	 * referential:
	 *
	 * @note: 'gluOrtho2D(0, width, 0, height);' could have been used instead.
	 *
	 */
	glOrtho( /* left */ 0, /* right */ width,
	  /* bottom */ height, /* top */ 0,
	  /* near */ DefaultNearClippingPlaneFor2D,
	  /* far */ DefaultFarClippingPlaneFor2D ) ;


#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException(
				"OpenGLContext::setOrthographicProjectionFor2D: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException(
				"OpenGLContext::setOrthographicProjectionFor2D: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


#else // OSDL_USES_OPENGL

	throw OpenGLException(
		"OpenGLContext::setOrthographicProjectionFor2D failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setClearColor( const Pixels::ColorDefinition & color )
{

#if OSDL_USES_OPENGL

	_clearColor = color ;

	glClearColor( static_cast<GLclampf>( color.r/255.0 ),
			static_cast<GLclampf>( color.g/255.0 ),
			static_cast<GLclampf>( color.b/255.0 ),
			static_cast<GLclampf>( color.unused/255.0 ) ) ;

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setClearColor: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			LogPlug::warning( "OpenGLContext::setClearColor: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setClearColor failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::clearViewport()
{

#if OSDL_USES_OPENGL

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_VALUE:
			throw OpenGLException( "OpenGLContext::clearViewport: "
				"invalid bit in specified clear mask." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::clearViewport: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			LogPlug::warning( "OpenGLContext::clearViewport: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::clearViewport failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::clearDepthBuffer()
{

#if OSDL_USES_OPENGL

	glClear( GL_DEPTH_BUFFER_BIT ) ;


#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{
		case GL_NO_ERROR:
			break ;

		case GL_INVALID_VALUE:
			throw OpenGLException( "OpenGLContext::clearDepthBuffer: "
				"invalid bit in specified clear mask." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::clearDepthBuffer: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			LogPlug::warning( "OpenGLContext::clearDepthBuffer: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::clearDepthBuffer failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::pushAttribute( GLBitField attributeField )
{

#if OSDL_USES_OPENGL

	glPushAttrib( attributeField ) ;

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_STACK_OVERFLOW:
			throw OpenGLException( "OpenGLContext::pushAttribute: "
				"attribute stack is full." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::pushAttribute: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::pushAttribute: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::pushAttribute failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::popAttribute()
{

#if OSDL_USES_OPENGL

	glPopAttrib() ;

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_STACK_UNDERFLOW:
			throw OpenGLException( "OpenGLContext::popAttribute: "
				"attribute stack is empty." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::popAttribute: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::popAttribute: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::popAttribute failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}





const string OpenGLContext::toString( Ceylan::VerbosityLevels level ) const
{

	std::list<string> res ;

	res.push_back( "OpenGL context whose current selected flavour is "
		+ ToString( _flavour ) + "." ) ;

	OSDL::Video::BitsPerPixel redSize, greenSize, blueSize, alphaSize ;

	BitsPerPixel bpp = GetColorDepth( redSize, greenSize, blueSize,
		alphaSize ) ;

	res.push_back( "Overall bit per pixel is "
		+ Ceylan::toNumericalString( bpp ) + "." ) ;

	res.push_back( "Red component size: "
		+ Ceylan::toNumericalString( redSize ) + " bits." ) ;

	res.push_back( "Green component size: "
		+ Ceylan::toNumericalString( greenSize ) + " bits." ) ;

	res.push_back( "Blue component size: "
		+ Ceylan::toNumericalString( blueSize ) + " bits." ) ;

	res.push_back( "Alpha component size: "
		+ Ceylan::toNumericalString( alphaSize ) + " bits." ) ;

	return "Current OpenGL state is:" + Ceylan::formatStringList( res ) ;

}





// Static section.



void OpenGLContext::SetUpForFlavour( OpenGL::Flavour flavour, bool safest )
{

#if OSDL_USES_OPENGL

	// Performs operations before setting the video mode.

	switch( flavour )
	{

		case None:
			// Do nothing.
			break ;

		case OpenGLFor2D:
			SetDoubleBufferStatus( true ) ;
			// No SetDepthBufferSize, as depth sorting will not be used.
			// No SetFullScreenAntialiasingStatus, as 2D would look fuzzy.
			SetHardwareAccelerationStatus( true ) ;
			TrySettingVerticalBlankSynchronizationStatus( true ) ;
			// No SetColorDepth, left as is.
			break ;

		case OpenGLFor3D:
			SetDoubleBufferStatus( true ) ;
			SetDepthBufferSize( 16 ) ;
			// May trigger "Couldn't find matching GLX visual":
			if ( ! safest )
			{

				SetFullScreenAntialiasingStatus( true,
					/* samplesPerPixelNumber */ 4 ) ;

			}
			else
			{

				SetFullScreenAntialiasingStatus( false ) ;

			}
			SetHardwareAccelerationStatus( true ) ;
			TrySettingVerticalBlankSynchronizationStatus( true ) ;
			// No SetColorDepth, left as is.
			break ;

		case Reload:
			// TO-DO: re-set previous settings?
			break ;

		default:
			throw OpenGLException( "OpenGLContext::SetUpForFlavour failed: "
				"unknown flavour (" + Ceylan::toString( flavour )
				+ "), which is abnormal." ) ;
			break ;
	}


#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::SetUpForFlavour failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL


}



void OpenGLContext::EnableFeature( OpenGL::Feature feature )
{

#if OSDL_USES_OPENGL

#if OSDL_CHECK_OPENGL_CALLS

	switch( feature )
	{

		case Alphablending:
		case TwoDimensionalTexturing:
		case CullPolygons:
		case DepthTests:
		case Lighting:
		case Light_1:
		case Light_2:
		case Light_3:
		case Light_4:
		case Light_5:
		case Light_6:
		case Light_7:
		case Light_8:
			break ;

		default:
			throw OpenGLException( "OpenGLContext::EnableFeature: "
				"unexpected feature requested ("
				+ Ceylan::toString( feature ) + ")." ) ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS

	glEnable( feature ) ;

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_ENUM:
			throw OpenGLException( "OpenGLContext::EnableFeature: "
				"invalid enumeration." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::EnableFeature: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::EnableFeature: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::EnableFeature failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::DisableFeature( Feature feature )
{

#if OSDL_CHECK_OPENGL_CALLS

	switch( feature )
	{

		case Alphablending:
		case TwoDimensionalTexturing:
		case CullPolygons:
		case DepthTests:
		case Lighting:
		case Light_1:
		case Light_2:
		case Light_3:
		case Light_4:
		case Light_5:
		case Light_6:
		case Light_7:
		case Light_8:
			break ;

		default:
			throw OpenGLException( "OpenGLContext::DisableFeature: "
				"unexpected feature specified ("
				+ Ceylan::toString( feature ) + ")." ) ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS

#if OSDL_USES_OPENGL

	glDisable( feature ) ;

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{

		case GL_NO_ERROR:
			break ;

		case GL_INVALID_ENUM:
			throw OpenGLException( "OpenGLContext::DisableFeature: "
				"invalid enumeration." ) ;
			break ;

		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::DisableFeature: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::DisableFeature: "
				"unexpected error reported." ) ;
			break ;

	}

#endif // OSDL_CHECK_OPENGL_CALLS


#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::DisableFeature failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



bool OpenGLContext::GetDoubleBufferStatus()
{

	return ( GetGLAttribute( SDL_GL_DOUBLEBUFFER ) == 1 ) ;

}



void OpenGLContext::SetDoubleBufferStatus( bool newStatus )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */
	SetGLAttribute( SDL_GL_DOUBLEBUFFER, newStatus ? 1 : 0 ) ;

#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::SetDoubleBufferStatus failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::SetDoubleBufferStatus failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



Ceylan::Uint8 OpenGLContext::GetDepthBufferSize()
{

	return static_cast<Ceylan::Uint8>( GetGLAttribute( SDL_GL_DEPTH_SIZE ) ) ;

}



void OpenGLContext::SetDepthBufferSize( Ceylan::Uint8 bitsNumber )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */
	SetGLAttribute( SDL_GL_DEPTH_SIZE, bitsNumber ) ;

#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::SetDepthBufferSize failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::SetDepthBufferSize failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



Ceylan::Uint8 OpenGLContext::GetFullScreenAntialiasingStatus()
{

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called, so this method should be called after VideoModule::setMode.
	 *
	 */

	if ( GetGLAttribute( SDL_GL_MULTISAMPLEBUFFERS ) == 0 )
		return 0 ;

	return static_cast<Ceylan::Uint8>(
		GetGLAttribute( SDL_GL_MULTISAMPLESAMPLES ) ) ;

}



void OpenGLContext::SetFullScreenAntialiasingStatus( bool newStatus,
	Ceylan::Uint8 samplesPerPixelNumber )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */

	if ( newStatus )
	{

		SetGLAttribute( SDL_GL_MULTISAMPLEBUFFERS,
			/* Number of multisample buffers (0 or 1) */ 1 ) ;

		SetGLAttribute( SDL_GL_MULTISAMPLESAMPLES, samplesPerPixelNumber ) ;

	}
	else
	{

		SetGLAttribute( SDL_GL_MULTISAMPLEBUFFERS,
			/* Number of multisample buffers (0 or 1) */ 0 ) ;

		SetGLAttribute( SDL_GL_MULTISAMPLESAMPLES, 0 ) ;

	}

#else // OSDL_USES_SDL

	throw OpenGLException(
		"OpenGLContext::SetFullScreenAntialiasingStatus failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException(
		"OpenGLContext::SetFullScreenAntialiasingStatus failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



bool OpenGLContext::GetHardwareAccelerationStatus()
{

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called, so this method should be called after VideoModule::setMode.
	 *
	 */

	return ( GetGLAttribute( SDL_GL_ACCELERATED_VISUAL ) == 1 ) ;

}



void OpenGLContext::SetHardwareAccelerationStatus( bool newStatus )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */
	SetGLAttribute( SDL_GL_ACCELERATED_VISUAL, newStatus ? 1 : 0 ) ;

#else // OSDL_USES_SDL

	throw OpenGLException(
		"OpenGLContext::SetHardwareAccelerationStatus failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException(
		"OpenGLContext::SetHardwareAccelerationStatus failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



bool OpenGLContext::GetVerticalBlankSynchronizationStatus()
{

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called, so this method should be called after VideoModule::setMode.
	 *
	 */

	return ( GetGLAttribute( SDL_GL_SWAP_CONTROL ) == 1 ) ;

}



void OpenGLContext::SetVerticalBlankSynchronizationStatus( bool newStatus )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */
	SetGLAttribute( SDL_GL_SWAP_CONTROL, newStatus ? 1 : 0 ) ;

#else // OSDL_USES_SDL

	throw OpenGLException(
		"OpenGLContext::SetVerticalBlankSynchronizationStatus failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException(
		"OpenGLContext::SetVerticalBlankSynchronizationStatus failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



bool OpenGLContext::TrySettingVerticalBlankSynchronizationStatus(
	bool newStatus )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */
	return TrySettingGLAttribute( SDL_GL_SWAP_CONTROL, newStatus ? 1 : 0 ) ;

#else // OSDL_USES_SDL

	throw OpenGLException(
		"OpenGLContext::TrySettingVerticalBlankSynchronizationStatus failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException(
		"OpenGLContext::TrySettingVerticalBlankSynchronizationStatus failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



Ceylan::Uint8 OpenGLContext::GetColorDepth(
	OSDL::Video::BitsPerPixel & redSize,
	OSDL::Video::BitsPerPixel & greenSize,
	OSDL::Video::BitsPerPixel & blueSize,
	OSDL::Video::BitsPerPixel & alphaSize )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL


	redSize = static_cast<OSDL::Video::BitsPerPixel>(
		GetGLAttribute( SDL_GL_RED_SIZE ) ) ;

	greenSize = static_cast<OSDL::Video::BitsPerPixel>(
		GetGLAttribute( SDL_GL_GREEN_SIZE ) ) ;

	blueSize = static_cast<OSDL::Video::BitsPerPixel>(
		GetGLAttribute( SDL_GL_BLUE_SIZE ) ) ;

	alphaSize = static_cast<OSDL::Video::BitsPerPixel>(
		GetGLAttribute( SDL_GL_ALPHA_SIZE ) ) ;

	// Alpha not counted here:
	return redSize + greenSize + blueSize ;


#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::GetColorDepth failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::GetColorDepth failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::SetColorDepth( BitsPerPixel plannedBpp )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	// Setting the relevant bits per color component for OpenGL framebuffer:

	int rgbSize[ 3 ] ;

	switch( plannedBpp )
	{

		case 8:
			rgbSize[0] = 3 ;
			rgbSize[1] = 3 ;
			rgbSize[2] = 2 ;
			break ;

		case 15:
		case 16:
			rgbSize[0] = 5 ;
			rgbSize[1] = 5 ;
			rgbSize[2] = 5 ;
			break ;

		case 32:
			rgbSize[0] = 8 ;
			rgbSize[1] = 8 ;
			rgbSize[2] = 8 ;
			break ;

		default:
			throw OpenGLException( "OpenGLContext::SetColorDepth failed: "
				"unsupported color depth ("
				+ Ceylan::toNumericalString(plannedBpp)
				+ " bits per pixel)" ) ;
			break ;
	}


	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */
	SetGLAttribute( SDL_GL_RED_SIZE,   rgbSize[0] ) ;
	SetGLAttribute( SDL_GL_GREEN_SIZE, rgbSize[1] ) ;
	SetGLAttribute( SDL_GL_BLUE_SIZE,  rgbSize[2] ) ;

	//SetGLAttribute( SDL_GL_ALPHA_SIZE,  rgbSize[3] ) ;


#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::SetColorDepth failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::SetColorDepth failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::SetColorDepth(
	OSDL::Video::BitsPerPixel redSize,
	OSDL::Video::BitsPerPixel greenSize,
	OSDL::Video::BitsPerPixel blueSize )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */
	SetGLAttribute( SDL_GL_RED_SIZE,   redSize ) ;
	SetGLAttribute( SDL_GL_GREEN_SIZE, greenSize ) ;
	SetGLAttribute( SDL_GL_BLUE_SIZE,  blueSize ) ;

	//SetGLAttribute( SDL_GL_ALPHA_SIZE, alphaSize ) ;

#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::SetColorDepth failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::SetColorDepth failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



std::string OpenGLContext::InterpretFeatureAvailability()
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * A feature might be potentially available but here be actually not
	 * available if it was not requested before the last VideoModule::setMode
	 * call.
	 *
	 * Testing HasGLAttribute beforehand, as we do not want this method to throw
	 * any exception.
	 *
	 */

	std::list<string> res ;

	if ( HasGLAttribute( SDL_GL_DOUBLEBUFFER ) )
	{

		if ( GetDoubleBufferStatus() )
			res.push_back( "OpenGL double-buffering available." ) ;
		else
			res.push_back( "OpenGL double-buffering not available." ) ;

	}
	else
		res.push_back( "No OpenGL double-buffering attribute defined." ) ;


	if ( HasGLAttribute( SDL_GL_DEPTH_SIZE ) )
		res.push_back( "OpenGL depth buffer size: "
			+ Ceylan::toNumericalString( GetDepthBufferSize() ) + " bits." ) ;
	else
		res.push_back( "No OpenGL depth buffer size attribute defined." ) ;


	if ( HasGLAttribute( SDL_GL_MULTISAMPLEBUFFERS ) )
	{

		Ceylan::Uint8 samples = GetFullScreenAntialiasingStatus() ;

		if ( samples == 0 )
			res.push_back( "OpenGL Fullscreen antialiasing not available." ) ;
		else
			res.push_back( Ceylan::toNumericalString( samples )
				+ "x fullscreen antialiasing available." ) ;
	}
	else
	{

		res.push_back( "No OpenGL multisample buffer attribute defined." ) ;

	}


	if ( HasGLAttribute( SDL_GL_ACCELERATED_VISUAL ) )
	{

		if ( GetHardwareAccelerationStatus() )
			res.push_back( "OpenGL hardware acceleration available." ) ;
		else
			res.push_back( "OpenGL hardware acceleration not available." ) ;

	}
	else
	{

		res.push_back( "No OpenGL hardware acceleration attribute defined." ) ;

	}


	if ( HasGLAttribute( SDL_GL_SWAP_CONTROL ) )
	{

		if ( GetVerticalBlankSynchronizationStatus() )
			res.push_back(
				"OpenGL vertical blank synchronization (VSYNC) available." ) ;
		else
			res.push_back(
				"OpenGL vertical blank synchronization (VSYNC) not available."
			) ;
	}
	else
	{

		res.push_back( "No OpenGL vertical blank synchronization (VSYNC) "
			"attribute defined." ) ;

	}


	if ( HasGLAttribute( SDL_GL_RED_SIZE )
			&& HasGLAttribute( SDL_GL_GREEN_SIZE )
			&& HasGLAttribute( SDL_GL_BLUE_SIZE )
			&& HasGLAttribute( SDL_GL_ALPHA_SIZE ) )
	{

		OSDL::Video::BitsPerPixel redSize, greenSize, blueSize, alphaSize,
			total ;

		total = GetColorDepth( redSize, greenSize, blueSize, alphaSize ) ;

		res.push_back( "OpenGL Color depths: "
			+ Ceylan::toNumericalString( total )
			+ " color bits per pixel, with "
			+ Ceylan::toNumericalString( redSize )   + " bits for red, "
			+ Ceylan::toNumericalString( greenSize ) + " bits for green, "
			+ Ceylan::toNumericalString( blueSize )  + " bits for blue, and "
			+ Ceylan::toNumericalString( alphaSize ) + " bits for alpha" ) ;

	}
	else
	{

		res.push_back( "OpenGL color depth component attributes "
			"are not all defined." ) ;

	}

	return "Summary of the currently available OpenGL features: "
		+ Ceylan::formatStringList( res ) ;


#else // OSDL_USES_SDL

	throw OpenGLException(
		"OpenGLContext::InterpretFeatureAvailability failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException(
		"OpenGLContext::InterpretFeatureAvailability failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



string OpenGLContext::ToString( OpenGL::Flavour flavour )
{

	switch( flavour )
	{

		case OpenGL::None:
			return "no OpenGL" ;
			break ;

		case OpenGL::OpenGLFor2D:
			return "OpenGL for 2D" ;
			break ;

		case OpenGL::OpenGLFor3D:
			return "OpenGL for 3D" ;
			break ;

		case OpenGL::Reload:
			return "reload OpenGL context" ;
			break ;

		default:
			return "unknown flavour (" + Ceylan::toString( flavour )
				+ "), which is abnormal" ;
			break ;

	}

}





// Protected section.


void OpenGLContext::updateProjection()
{

	LogPlug::trace( "OpenGLContext::updateProjection" ) ;

	switch( _flavour )
	{

		case OpenGLFor2D:
			// _projectionMode bypassed (ignored) here:
			setOrthographicProjectionFor2D( _viewportWidth, _viewportHeight ) ;
			return ;
			break ;

		case OpenGLFor3D:
			// _projectionMode bypassed (ignored) here:
			// FIXME 3D counterpart not written yet.
			return ;
			break ;

		default:
			LogPlug::warning( "OpenGLContext::updateProjection: "
				"not managed currently for that flavour." ) ;
			return ;
			break ;

	}


}



bool OpenGLContext::HasGLAttribute( GLAttribute attribute )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: any set attribute do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */

	int value ;

	return ( SDL_GL_GetAttribute( attribute, & value ) != -1 ) ;

#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::HasGLAttribute failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::HasGLAttribute failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



int OpenGLContext::GetGLAttribute( GLAttribute attribute )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: a set attribute will not take effect until VideoModule::setMode is
	 * called.
	 *
	 */

	int value ;

	if ( SDL_GL_GetAttribute( attribute, & value ) == -1 )
		throw OpenGLException(
			"OpenGLContext::GetGLAttribute of attribute corresponding to "
			+ GLAttributeToString( attribute ) + " failed: "
			+ Utils::getBackendLastError() ) ;

	return value ;

#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::GetGLAttribute failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::GetGLAttribute failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::SetGLAttribute( GLAttribute attribute, int value )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */

	if ( SDL_GL_SetAttribute( attribute, value ) == -1 )
		throw OpenGLException(
			"OpenGLContext::SetGLAttribute of attribute corresponding to "
			+ GLAttributeToString( attribute ) + " to value "
			+ Ceylan::toString( value ) + " failed: "
			+ Utils::getBackendLastError() ) ;

#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::SetGLAttribute failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::SetGLAttribute failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



bool OpenGLContext::TrySettingGLAttribute( GLAttribute attribute, int value )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode is
	 * called.
	 *
	 */

	if ( SDL_GL_SetAttribute( attribute, value ) == -1 )
	{

		LogPlug::warning( "OpenGLContext::TrySettingGLAttribute "
			"of attribute corresponding to "
			+ GLAttributeToString( attribute ) + " to value "
			+ Ceylan::toString( value ) + " failed: "
			+ Utils::getBackendLastError() ) ;

		return false ;

	}
	else
	{

		int newValue ;

		try
		{

			newValue = GetGLAttribute( attribute ) ;

			if ( newValue != value )
			{

				LogPlug::warning( "OpenGLContext::TrySettingGLAttribute "
					"of attribute corresponding to "
					+ GLAttributeToString( attribute ) + " to value "
					+ Ceylan::toString( value ) + " did not fail, "
					"but re-reading the set attribute results in the value "
					+ Ceylan::toString( newValue ) + " instead." ) ;

				return false ;

			}

		}
		catch( const OpenGLException & e )
		{

			LogPlug::warning( "OpenGLContext::TrySettingGLAttribute "
				"of attribute corresponding to "
				+ GLAttributeToString( attribute ) + " to value "
				+ Ceylan::toString( value )
				+ " failed: when trying to read back that value, got: "
				+ e.toString() ) ;

			return false ;

		}


		return true ;

	}


#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::TrySettingGLAttribute failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::TrySettingGLAttribute failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



std::string OpenGLContext::GLAttributeToString( GLAttribute attribute )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	string res ;


	/*
	 * These attributes are defined in SDL_video.h:
	 *
	 */
	switch( attribute )
	{

		case SDL_GL_RED_SIZE:
			res = "minimum depth, in bits, of OpenGL red component" ;
			break ;

		case SDL_GL_GREEN_SIZE:
			res = "minimum depth, in bits, of OpenGL green component" ;
			break ;

		case SDL_GL_BLUE_SIZE:
			res = "minimum depth, in bits, of OpenGL blue component" ;
			break ;

		case SDL_GL_ALPHA_SIZE:
			res = "minimum depth, in bits, of alpha component" ;
			break ;

		case SDL_GL_BUFFER_SIZE:
			res = "minimum depth, in bits, of the OpenGL framebuffer" ;
			break ;

		case SDL_GL_DOUBLEBUFFER:
			res = "OpenGL double-buffering" ;
			break ;

		case SDL_GL_DEPTH_SIZE:
			res = "minimum depth, in bits, of the OpenGL depth buffer" ;
			break ;

		case SDL_GL_STENCIL_SIZE:
			res = "minimum depth, in bits, of the OpenGL stencil buffer" ;
			break ;

		case SDL_GL_ACCUM_RED_SIZE:
			res = "minimum depth, in bits, of the red component "
				"in OpenGL accumulation buffer" ;
			break ;

		case SDL_GL_ACCUM_GREEN_SIZE:
			res = "minimum depth, in bits, of the green component "
				"in OpenGL accumulation buffer" ;
			break ;

		case SDL_GL_ACCUM_BLUE_SIZE:
			res = "size of OpenGL accumulation blue component" ;
			break ;

		case SDL_GL_ACCUM_ALPHA_SIZE:
			res = "minimum depth, in bits, of the alpha component "
				"in OpenGL accumulation buffer" ;
			break ;

		case SDL_GL_STEREO:
			res = "OpenGL stereo (left and right) rendering" ;
			break ;

		case SDL_GL_MULTISAMPLEBUFFERS:
			res = "OpenGL full screen anti-aliasing (FSAA)" ;
			break ;

		case SDL_GL_MULTISAMPLESAMPLES:
			res = "number of samples per pixel "
				"in the OpenGL multisample buffer" ;
			break ;

		case SDL_GL_ACCELERATED_VISUAL:
			res = "OpenGL support for accelerated visuals" ;
			break ;

		case SDL_GL_SWAP_CONTROL:
			res = "OpenGL vertical synchronization (v-sync)" ;
			break ;

		default:
			res = "unknown OpenGL attribute (abnormal)" ;
			break ;

	}

	return res ;

#else // OSDL_USES_SDL

	throw OpenGLException( "OpenGLContext::GLAttributeToString failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::GLAttributeToString failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}

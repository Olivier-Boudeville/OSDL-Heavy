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
#include "SDL.h"		  // for SDL_Cursor, etc.
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
 *
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
 * OSDL_USES_OPENGL being not defined not always triggers an exception in
 * all non-static methods, as the constructor would have done that already.
 *
 * Dynamic loading of OpenGL functions could be performed.
 * It would involve a global shared pointer to a structure containing all
 * needed function pointers, loaded at start-up.
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




OpenGLException::OpenGLException( const std::string & reason ) throw(): 
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
	Length viewportWidth, Length viewportHeight )
		throw( OpenGLException ):
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




void OpenGLContext::selectFlavour( Flavour flavour ) throw( OpenGLException )
{

	LogPlug::trace( "OpenGLContext::selectFlavour" ) ;
		
	if ( VideoModule::IsDisplayInitialized() )
		LogPlug::warning( 
			"OpenGLContext::selectFlavour: display is already initialized." ) ;
	
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



void OpenGLContext::set2DFlavour() throw( OpenGLException )
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext::set2DFlavour" ) ;
	
	_flavour = OpenGLFor2D ;

	// Saves a lot of the OpenGL state machine:
	pushAttribute( GL_ENABLE_BIT ) ;
	
	// No depth sorting used:
	setDepthBufferStatus( false ) ;


	/*
	 * setOrthographicProjectionFor2D not called here, as it is 
	 * automatically triggered whenever the screen is resized: the 
	 * screen resize triggers a viewport resize (setViewPort) which
	 * triggers the projection update (updateProjection). 
	 *
	 * VideoModule::setMode will call setViewPort after having managed the
	 * OpenGL flavour, OpenGLContext::selectFlavour will do the same.
	 *
	 */

	
	// No culling of faces used:
	OpenGLContext::DisableFeature( GL_CULL_FACE ) ;
	
	setShadingModel( Flat ) ;

	
	/*
	 * Blends the incoming RGBA color values with the values in the color
	 * buffers: 
	 *
	 */
	OpenGLContext::EnableFeature( GL_BLEND ) ;
	
	/*
	 * Ponders the source color components by its alpha coordinate Ac,
	 * and the destination by (1-Ac):
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



void OpenGLContext::set3DFlavour() throw( OpenGLException )
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext::set3DFlavour" ) ;

	_flavour = OpenGLFor3D ;

	// Saves a lot of the OpenGL state machine:
	pushAttribute( GL_ENABLE_BIT ) ;

	// Depth tests wanted:
	EnableFeature( GL_DEPTH_TEST ) ;

	setShadingModel( Smooth ) ;
	
	/*
	 * Blends the incoming RGBA color values with the values in the color
	 * buffers: 
	 *
	 */
	EnableFeature( GL_BLEND ) ;
	
	/*
	 * Ponders the source color components by its alpha coordinate Ac,
	 * and the destination by (1-Ac):
	 *
	 */
	setBlendingFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) ;
	
	GLTexture::SetTextureFlavour( GLTexture::For3D ) ;

	glMatrixMode( GL_PROJECTION ) ;
	glPushMatrix() ;
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



void OpenGLContext::blank() throw( OpenGLException )
{

	LogPlug::warning( "OpenGLContext::blank not implemented yet." ) ;
		
}



void OpenGLContext::reload() throw( OpenGLException ) 
{

	setClearColor( _clearColor ) ;
	clearViewport() ;
	
	updateProjection() ;
	
	LogPlug::warning( "OpenGLContext::reload not fully implemented yet." ) ;
	
}



void OpenGLContext::setBlendingFunction( GLEnumeration sourceFactor,
	GLEnumeration destinationFactor ) throw( OpenGLException )
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
	throw( OpenGLException )
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



void OpenGLContext::setCullingStatus( bool newStatus ) throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	if ( newStatus )
		EnableFeature( GL_CULL_FACE ) ;
	else
		DisableFeature( GL_CULL_FACE ) ;
	
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
	throw( OpenGLException ) 
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
	throw( OpenGLException )
{

	// Later: add glDepthFunc support.

#if OSDL_USES_OPENGL

	if ( newStatus )
	{
		EnableFeature( GL_DEPTH_TEST ) ;
		clearDepthBuffer() ;
	}	
	else
	{
		DisableFeature( GL_DEPTH_TEST ) ;
	}
		
#else // OSDL_USES_OPENGL

	throw OpenGLException( 
		"OpenGLContext::setDepthBufferStatus failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL
			
}



void OpenGLContext::setViewPort( Length width, Length height, 
	const TwoDimensional::Point2D & lowerLeftCorner ) throw( OpenGLException )
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
	
	
	// Restore the aspect ratio of the projection to match the new viewport:
	updateProjection() ; 	


#else // OSDL_USES_OPENGL
	
	throw OpenGLException( "OpenGLContext::setViewPort failed: "
		"no OpenGL support available" ) ;
		
#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setOrthographicProjection( GLLength width, 
	GLCoordinate near, GLCoordinate far ) throw ( OpenGLException )
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
	GLLength width, GLLength height ) throw ( OpenGLException )
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
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

	_clearColor = color ;
	
	glClearColor( color.r/255.0, color.g/255.0, color.b/255.0, 
		color.unused/255.0 ) ;

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



void OpenGLContext::clearViewport() throw( OpenGLException )
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



void OpenGLContext::clearDepthBuffer() throw( OpenGLException )
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
	throw( OpenGLException )
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



void OpenGLContext::popAttribute() throw( OpenGLException )
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





const string OpenGLContext::toString( Ceylan::VerbosityLevels level ) 
	const throw() 
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



void OpenGLContext::SetUpForFlavour( OpenGL::Flavour flavour )
	throw( OpenGLException )
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
			SetVerticalBlankSynchronizationStatus( true ) ;
			// No SetColorDepth, left as is.
			break ;
			
		case OpenGLFor3D:
			SetDoubleBufferStatus( true ) ;
			SetDepthBufferSize( 16 ) ;
			SetFullScreenAntialiasingStatus( true,
				/* samplesPerPixelNumber */ 4 ) ;
			SetHardwareAccelerationStatus( true ) ;
			SetVerticalBlankSynchronizationStatus( true ) ;
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


	
void OpenGLContext::EnableFeature( GLEnumeration feature ) 
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL
	
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



void OpenGLContext::DisableFeature( GLEnumeration feature ) 
	throw( OpenGLException )
{

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



bool OpenGLContext::GetDoubleBufferStatus() throw( OpenGLException )
{

	return static_cast<bool>( GetGLAttribute( SDL_GL_DOUBLEBUFFER ) ) ;

}



void OpenGLContext::SetDoubleBufferStatus( bool newStatus ) 
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called.
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



Ceylan::Uint8 OpenGLContext::GetDepthBufferSize() throw( OpenGLException )
{

	return static_cast<Ceylan::Uint8>( GetGLAttribute( SDL_GL_DEPTH_SIZE ) ) ;

}



void OpenGLContext::SetDepthBufferSize( Ceylan::Uint8 bitsNumber ) 
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called.
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
	throw( OpenGLException )
{

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called, so this method should be called after VideoModule::setMode.
	 *
	 */
	
	if ( GetGLAttribute( SDL_GL_MULTISAMPLEBUFFERS ) == 0 ) 
		return 0 ;
		
	return static_cast<Ceylan::Uint8>( 
		GetGLAttribute( SDL_GL_MULTISAMPLESAMPLES ) ) ;
			
}



void OpenGLContext::SetFullScreenAntialiasingStatus( bool newStatus, 
	Ceylan::Uint8 samplesPerPixelNumber ) throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called.
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



bool OpenGLContext::GetHardwareAccelerationStatus() throw( OpenGLException )	
{

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called, so this method should be called after VideoModule::setMode.
	 *
	 */

	return static_cast<bool>( GetGLAttribute( SDL_GL_ACCELERATED_VISUAL ) ) ;
	
}



void OpenGLContext::SetHardwareAccelerationStatus( bool newStatus ) 
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called.
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
	throw( OpenGLException )	
{

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called, so this method should be called after VideoModule::setMode.
	 *
	 */

	return static_cast<bool>( GetGLAttribute( SDL_GL_SWAP_CONTROL ) ) ;
	
}



void OpenGLContext::SetVerticalBlankSynchronizationStatus( bool newStatus ) 
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called.
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



Ceylan::Uint8 OpenGLContext::GetColorDepth( 
		OSDL::Video::BitsPerPixel & redSize, 
		OSDL::Video::BitsPerPixel & greenSize, 
		OSDL::Video::BitsPerPixel & blueSize,
		OSDL::Video::BitsPerPixel & alphaSize ) throw( OpenGLException )
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
	throw( OpenGLException )
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
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called.
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
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called.
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
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * A feature might be potentially available but here be actually not
	 * available if it was not requested before the last VideoModule::setMode
	 * call.
	 *
	 */
	 
	std::list<string> res ;
	
	if ( GetDoubleBufferStatus() )
		res.push_back( "Double-buffering available." ) ;
	else
		res.push_back( "Double-buffering not available." ) ;


	res.push_back( "Depth buffer size: " 
		+ Ceylan::toString( GetDepthBufferSize() ) + " bits." ) ;


	Ceylan::Uint8 samples = GetFullScreenAntialiasingStatus() ;
	if ( samples == 0 )
		res.push_back( "Fullscreen antialiasing not available." ) ;
	else
		res.push_back( Ceylan::toString( samples) 
			+ "x fullscreen antialiasing available." ) ;


	if ( GetHardwareAccelerationStatus() )
		res.push_back( "OpenGL hardware acceleration available." ) ;
	else
		res.push_back( "OpenGL hardware acceleration not available." ) ;


	if ( GetVerticalBlankSynchronizationStatus() )
		res.push_back( "Vertical blank synchronization (VSYNC) available." ) ;
	else
		res.push_back( 
			"Vertical blank synchronization (VSYNC) not available." ) ;

	OSDL::Video::BitsPerPixel redSize, greenSize, blueSize, alphaSize, total ;
	total = GetColorDepth( redSize, greenSize, blueSize, alphaSize ) ;
	
	res.push_back( "Color depths: " 
		+ Ceylan::toNumericalString( total ) + " color bits per pixel, with "
		+ Ceylan::toNumericalString( redSize )   + " bits for red, "   
		+ Ceylan::toNumericalString( greenSize ) + " bits for green, " 
		+ Ceylan::toNumericalString( blueSize )  + " bits for blue, and "  
		+ Ceylan::toNumericalString( alphaSize ) + " bits for alpha" ) ;


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



string OpenGLContext::ToString( OpenGL::Flavour flavour ) throw() 
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


void OpenGLContext::updateProjection() throw( OpenGLException )
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



int OpenGLContext::GetGLAttribute( GLAttribute attribute ) 
	throw( OpenGLException )
{
	
#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: any set attribute do not take effect until VideoModule::setMode
	 * is called.
	 *
	 */
	
	int value ;
	 
	if ( SDL_GL_GetAttribute( attribute, & value ) == -1 )
		throw OpenGLException( "OpenGLContext::GetGLAttribute of attribute "
			+ Ceylan::toString( attribute ) + " failed: "
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
	throw( OpenGLException )
{


#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	/*
	 * Note: the set attributes do not take effect until VideoModule::setMode
	 * is called.
	 *
	 */
	 
	if ( SDL_GL_SetAttribute( attribute, value ) == -1 )
		throw OpenGLException( "OpenGLContext::SetGLAttribute of attribute "
			+ Ceylan::toString( attribute ) + " to value "
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


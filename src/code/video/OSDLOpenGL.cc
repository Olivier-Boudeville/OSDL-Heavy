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
#include "SDL.h"					 // for SDL_Cursor, etc.
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
 * Implementation notes:
 * 
 * OSDL_USES_OPENGL being not defined not always triggers an exception in
 * all non-static methods, as the constructor would have done that already.
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
	_viewportWidth( viewportWidth ),
	_viewportHeight( viewportHeight ),
	_projectionMode( Orthographic ),
	_projectionWidth( DefaultOrthographicWidth ),
	_nearClippingPlane( DefaultNearClippingPlaneFor2D ),
	_farClippingPlane( DefaultFarClippingPlaneFor2D ),
	_clearColor()
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext constructor" ) ;

	selectFlavour( flavour, plannedBpp ) ;
	
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




void OpenGLContext::selectFlavour( Flavour flavour, BitsPerPixel plannedBpp )
	throw( OpenGLException )
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
			set2DFlavour( plannedBpp ) ;
			break ;
			
		case OpenGLFor3D:
			// Sets _flavour as well:
			set3DFlavour( plannedBpp ) ;
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
	
}



void OpenGLContext::set2DFlavour( BitsPerPixel plannedBpp ) 
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext::set2DFlavour" ) ;
	
	_flavour = OpenGLFor2D ;

	// Saves a lot of the OpenGL state machine:
	pushAttribute( GL_ENABLE_BIT ) ;
	
	// No depth sorting used:
	setDepthBufferStatus( false ) ;

	// To avoid fuzzy graphics:
	setFullScreenAntialiasingStatus( false ) ;

	/*
	 * setOrthographicProjectionFor2D not called here, as it is 
	 * automatically triggered whenever the screen is resized: the 
	 * screen resize triggers a viewport resize (setViewPort) which
	 * triggers the projection update (updateProjection). 
	 *
	 * VideoModule::setMode will call setViewPort after having managed the
	 * OpenGL flavour.
	 *
	 */
	setDoubleBufferStatus( true ) ;
	
	setColorDepth( plannedBpp ) ;

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

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::set3DFlavour( BitsPerPixel plannedBpp ) 
	throw( OpenGLException )
{

	LogPlug::trace( "OpenGLContext::set3DFlavour" ) ;

	_flavour = OpenGLFor3D ;

	// Saves a lot of the OpenGL state machine:
	pushAttribute( GL_ENABLE_BIT ) ;

	// Depth tests wanted:
	setDepthBufferSize( 16 ) ;
	EnableFeature( GL_DEPTH_TEST ) ;

	setFullScreenAntialiasingStatus( true ) ;
	
	setDoubleBufferStatus( true ) ;

	setColorDepth( plannedBpp ) ;

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
	

	glMatrixMode( GL_PROJECTION ) ;
	glPushMatrix() ;
	glLoadIdentity() ;

	gluPerspective( /* angle of view */ 45.0f, 
		/* aspect */ (GLfloat) _viewportWidth / (GLfloat) _viewportHeight, 
		/* near z plane */ 1.0f, 
		/* far z plane */ 100 ) ;

	GLTexture::SetTextureFlavour( GLTexture::For3D ) ;
		
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



Ceylan::Uint8 OpenGLContext::getColorDepth( 
		OSDL::Video::BitsPerPixel & redSize, 
		OSDL::Video::BitsPerPixel & greenSize, 
		OSDL::Video::BitsPerPixel & blueSize )
	const throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	// @fixme: Alpha currently not managed here.
	
	int value ;
	
	SDL_GL_GetAttribute( SDL_GL_RED_SIZE, & value ) ;		
	redSize = static_cast<OSDL::Video::BitsPerPixel>( value ) ;
	
	SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, & value ) ;	
	greenSize = static_cast<OSDL::Video::BitsPerPixel>( value ) ;
	 	
	SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, & value  ) ;		
	blueSize = static_cast<OSDL::Video::BitsPerPixel>( value ) ;
	
	return redSize + greenSize + blueSize ;

#else // OSDL_USES_SDL
	
	throw OpenGLException( "OpenGLContext::getColorDepth failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::getColorDepth failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}

	
	
void OpenGLContext::setColorDepth( BitsPerPixel plannedBpp ) 
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
			throw OpenGLException( "OpenGLContext::setColorDepth failed: "
				"unsupported color depth (" + Ceylan::toString(plannedBpp) 
				+ " bits per pixel)" ) ;
			break ;
	}
	
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE,   rgbSize[0] ) ;
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, rgbSize[1] ) ;
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,  rgbSize[2] ) ;

#else // OSDL_USES_SDL
	
	throw OpenGLException( "OpenGLContext::setColorDepth failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setColorDepth failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setColorDepth( 
		OSDL::Video::BitsPerPixel redSize, 
		OSDL::Video::BitsPerPixel greenSize,
		OSDL::Video::BitsPerPixel blueSize ) 
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE,   redSize   ) ;
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, greenSize ) ;
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,  blueSize  ) ;

#else // OSDL_USES_SDL
	
	throw OpenGLException( "OpenGLContext::setColorDepth failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setColorDepth failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

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


		
bool OpenGLContext::getDoubleBufferStatus() throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	int value ;
	
	if ( SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, & value ) != 0 ) 
		throw OpenGLException( 
			"OpenGLContext::getDoubleBufferStatus: error occurred, "
			+ Utils::getBackendLastError() ) ;

	return ( value != 0 ) ; 

#else // OSDL_USES_SDL
	
	throw OpenGLException( "OpenGLContext::getDoubleBufferStatus failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::getDoubleBufferStatus failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



bool OpenGLContext::setDoubleBufferStatus( bool newStatus ) 
	throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, newStatus ) ;
	
	return getDoubleBufferStatus() ;

#else // OSDL_USES_SDL
	
	throw OpenGLException( "OpenGLContext::setDoubleBufferStatus failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
	
#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setDoubleBufferStatus failed: "
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
	
	

void OpenGLContext::setFullScreenAntialiasingStatus( bool newStatus, 
	Ceylan::Uint8 samplesPerPixelNumber ) throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	if ( newStatus ) 
	{
	
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 
			/* Number of multisample buffers (0 or 1) */ 1 ) ;
			
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 
			samplesPerPixelNumber ) ;
			
	}
	else
	{
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0 ) ;	
	}

#else // OSDL_USES_SDL
	
	throw OpenGLException( 
		"OpenGLContext::setFullScreenAntialiasingStatus failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
	
#else // OSDL_USES_OPENGL

	throw OpenGLException( 
		"OpenGLContext::setFullScreenAntialiasingStatus failed: "
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



void OpenGLContext::setDepthBufferSize( Ceylan::Uint8 bitsNumber, 
	bool autoEnable ) throw( OpenGLException )
{

#if OSDL_USES_OPENGL

#if OSDL_USES_SDL

	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, bitsNumber ) ;
	
	if ( autoEnable )	
		setDepthBufferStatus( true ) ;

#else // OSDL_USES_SDL
	
	throw OpenGLException( "OpenGLContext::setDepthBufferSize failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

#else // OSDL_USES_OPENGL

	throw OpenGLException( "OpenGLContext::setDepthBufferSize failed: "
		"no OpenGL support available" ) ;

#endif // OSDL_USES_OPENGL

}



void OpenGLContext::setViewPort( Length width, Length height, 
	const TwoDimensional::Point2D & lowerLeftCorner ) throw( OpenGLException )
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "OpenGLContext::setViewPort" ) ;
	
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
	
	OSDL::Video::BitsPerPixel redSize, greenSize, blueSize ;
	
	BitsPerPixel bpp = getColorDepth( redSize, greenSize, blueSize ) ;
	res.push_back( "Overall bit per pixel is " 
		+ Ceylan::toNumericalString( bpp ) + "." ) ;
	
	res.push_back( "Red component size: "   
		+ Ceylan::toNumericalString( redSize ) + " bits." ) ;
		
	res.push_back( "Green component size: " 
		+ Ceylan::toNumericalString( greenSize ) + " bits." ) ;
		
	res.push_back( "Blue component size: "  
		+ Ceylan::toNumericalString( blueSize ) + " bits." ) ;
	
	// Alpha ?
			
	return "Current OpenGL state is:" + Ceylan::formatStringList( res ) ;
	
}




// Static section.


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
				+ "), which is abnormal)" ;
			break ;	
			
	}
	
}



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


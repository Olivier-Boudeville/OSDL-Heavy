#include "OSDLOpenGL.h"

#include "OSDLVideo.h"    // for VideoModule
#include "OSDLUtils.h"    // for getBackendLastError

// for CEYLAN_DETECTED_LITTLE_ENDIAN, openGLContextsCanBeLost, etc. :
#include "Ceylan.h"       


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>   // for OSDL_USES_OPENGL and al 
#endif // OSDL_USES_CONFIG_H

#ifdef OSDL_HAVE_OPENGL
#include "SDL_opengl.h"   // for GL, GLU
#endif // OSDL_HAVE_OPENGL



using std::string ;

#include <list>
using std::list ;


using namespace OSDL::Video::OpenGL ;
using namespace OSDL::Video::OpenGL::GLU ;

using namespace Ceylan::Log ;




// OpenGL RGBA masks, since it always assumes RGBA order : 
	
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




OpenGLException::OpenGLException( const std::string & reason ) throw() : 
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


 				
OpenGLContext::OpenGLContext( OpenGL::Flavour flavour ) 
		throw( OpenGLException ) :
	_flavour( OpenGL::None ),
	_redSize( 0 ),
	_greenSize( 0 ),
	_blueSize( 0 ),
	_viewportWidth( 0 ),
	_viewportHeight( 0 ),
	_projectionMode( Orthographic ),
	_projectionWidth( DefaultOrthographicWidth ),
	_nearClippingPlane( DefaultNearClippingPlaneFor2D ),
	_farClippingPlane( DefaultFarClippingPlaneFor2D )
{

	selectFlavour( flavour ) ;
	
}



OpenGLContext::~OpenGLContext() throw()
{

}




void OpenGLContext::selectFlavour( Flavour flavour 
	/*, BitsPerPixel plannedBpp */) throw( OpenGLException )
{
	
	_flavour = flavour ;
	
	if ( VideoModule::IsDisplayInitialized() )
		LogPlug::warning( 
			"OpenGLContext::selectFlavour : display is already initialized." ) ;
	
	switch( flavour )
	{
	
		case None:
			// Nothing to do.
			return ;
			break ;
		
		case OpenGLFor2D:
			set2DFlavour( /*plannedBpp*/ ) ;
			break ;
			
		case OpenGLFor3D:
			set3DFlavour( /*plannedBpp*/ ) ;
			break ;
		
		default:
			LogPlug::error( 
				"OpenGLContext:selectFlavour : unknown flavour selected, "
				"defaulting to None." ) ;
			return ;
			break ;		
	
	}
	
}



void OpenGLContext::set2DFlavour() throw( OpenGLException )
{

	//glShadeModel(GL_FLAT);
	setFullScreenAntialiasingStatus( true ) ;
	//glDisable( GL_DEPTH_TEST ) ;
	
	// Defines camera :
	setOrthographicProjection( DefaultOrthographicWidth, 
		DefaultNearClippingPlaneFor2D, DefaultFarClippingPlaneFor2D ) ;
	
	setDoubleBufferStatus( true ) ;
	
}



void OpenGLContext::set3DFlavour() throw( OpenGLException )
{

	setFullScreenAntialiasingStatus( true ) ;

	setDoubleBufferStatus( true ) ;
	
	
	/*
	setDepthBufferSize( 16 ) ;
	glEnable( GL_DEPTH_TEST ) ;
	glMatrixMode( GL_PROJECTION ) ;
	glLoadIdentity() ;

	gluPerspective( 45.0f, (GLfloat) ScreenWidth/ (GLfloat) ScreenHeight, 3.0f, ZDepth ) ;
	*/
	
	LogPlug::warning( "OpenGLContext::set3DFlavour : not implemented yet." ) ;
	
}



void OpenGLContext::blank() throw( OpenGLException )
{

	LogPlug::warning( "OpenGLContext::blank not implemented yet." ) ;
		
}



void OpenGLContext::reload() throw( OpenGLException ) 
{

	LogPlug::warning( "OpenGLContext::blank not implemented yet." ) ;
	
}



Ceylan::Uint8 OpenGLContext::getColorDepth( 
		OSDL::Video::BitsPerPixel & redSize, 
		OSDL::Video::BitsPerPixel & greenSize, 
		OSDL::Video::BitsPerPixel & blueSize )
	const throw( OpenGLException )
{

	// @fixme : Alpha currently not managed here.
	
	int value ;
	
	SDL_GL_GetAttribute( SDL_GL_RED_SIZE, & value ) ;		
	redSize = static_cast<OSDL::Video::BitsPerPixel>( value ) ;
	
	SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, & value ) ;	
	greenSize = static_cast<OSDL::Video::BitsPerPixel>( value ) ;
	 	
	SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, & value  ) ;		
	blueSize = static_cast<OSDL::Video::BitsPerPixel>( value ) ;
	
	return redSize + greenSize + blueSize ;
	
}

	
void OpenGLContext::setColorDepth( BitsPerPixel plannedBpp ) 
	throw( OpenGLException )
{

	// Setting the relevant bits per color component for OpenGL framebuffer :

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

		default:
			rgbSize[0] = 8 ;
			rgbSize[1] = 8 ;
			rgbSize[2] = 8 ;
			break ;
	}
	
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE,   rgbSize[0] ) ;
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, rgbSize[1] ) ;
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,  rgbSize[2] ) ;

}


void OpenGLContext::setColorDepth( 
		OSDL::Video::BitsPerPixel redSize, 
		OSDL::Video::BitsPerPixel greenSize,
		OSDL::Video::BitsPerPixel blueSize ) 
	throw( OpenGLException )
{

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE,   redSize   ) ;
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, greenSize ) ;
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,  blueSize  ) ;

}
	

	
bool OpenGLContext::getDoubleBufferStatus() throw( OpenGLException )
{

	int value ;
	
	if ( SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, & value ) != 0 ) 
		throw OpenGLException( 
			"OpenGLContext::getDoubleBufferStatus : error occurred, "
			+ Utils::getBackendLastError() ) ;

	return ( value != 0 ) ; 

}


bool OpenGLContext::setDoubleBufferStatus( bool newStatus ) 
	throw( OpenGLException )
{

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, newStatus ) ;
	
	return getDoubleBufferStatus() ;
	
}


void OpenGLContext::setShadingModel( ShadingModel newShadingModel ) 
	throw( OpenGLException )
{

	switch( newShadingModel )
	{
	
		case Flat:
			glShadeModel( GL_FLAT ) ; 
			break ;
			
		case Smooth:
			glShadeModel( GL_SMOOTH ) ;
			break ;
			
		default:
			throw OpenGLException( "OpenGLContext::setShadingModel : "
				"unknown shading model specified." ) ;
			break ;		
			
	}
	
	switch ( glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_ENUM:
			throw OpenGLException( "OpenGLContext::setShadingModel : "
				"unexpected value for shading model." ) ;
			break ;
				
		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setShadingModel : "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			throw OpenGLException( "OpenGLContext::setShadingModel : "
				"unexpected error reported." ) ;
			break ;	
				
	}
	
}


void OpenGLContext::setCullingStatus( bool newStatus ) throw()
{

	if ( newStatus )
		glEnable( GL_CULL_FACE ) ;
	else
		glDisable( GL_CULL_FACE ) ;
		
}


void OpenGLContext::setCulling( CulledFacet culledFacet, 
		FrontOrientation frontOrientation, bool autoEnable ) 
	throw( OpenGLException ) 
{

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


	switch ( glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_ENUM:
			throw OpenGLException( "OpenGLContext::setCulling (facet) : "
				"unexpected culled facet selection." ) ;
			break ;
				
		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setCulling (facet) : "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			throw OpenGLException( "OpenGLContext::setCulling (facet) : "
				"unexpected error reported." ) ;
			break ;	
				
	}
		
			
	switch( frontOrientation )
	{
	
		case Clockwise:
			glFrontFace( GL_CW ) ;
			break ;
			
		case CounterClockwise:
			glFrontFace( GL_CCW ) ;
			break ;
	
	}
	
	
	switch ( glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_ENUM:
			throw OpenGLException( "OpenGLContext::setCulling : (orientation)"
				"unexpected front orientation selection." ) ;
			break ;
				
		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setCulling : (orientation)"
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			throw OpenGLException( "OpenGLContext::setCulling (orientation) : "
				"unexpected error reported." ) ;
			break ;	
				
	}
			

	if ( autoEnable )
		setCullingStatus( true ) ;

}
	

void OpenGLContext::setFullScreenAntialiasingStatus( bool newStatus, 
	Ceylan::Uint8 samplesPerPixelNumber ) throw( OpenGLException )
{

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
	
}


void OpenGLContext::setDepthBufferStatus( bool newStatus ) throw()
{

	if ( newStatus )
		glEnable( GL_DEPTH_TEST ) ;
	else
		glDisable( GL_DEPTH_TEST ) ;
		
}


void OpenGLContext::setDepthBufferSize( Ceylan::Uint8 bitsNumber, 
	bool autoEnable ) throw( OpenGLException )
{

	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, bitsNumber ) ;
	
	if ( autoEnable )	
		setDepthBufferStatus( true ) ;

}


void OpenGLContext::setViewPort( Length width, Length height, 
	const TwoDimensional::Point2D & lowerLeftCorner ) throw( OpenGLException )
{

	_viewportWidth  = width  ;
	_viewportHeight = height ;
	
	glViewport( lowerLeftCorner.getX(), lowerLeftCorner.getY(), 
		width, height ) ;

	switch ( glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_VALUE:
			throw OpenGLException( "OpenGLContext::setViewPort : "
				"either width or height is negative." ) ;
			break ;
				
		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setViewPort : "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			throw OpenGLException( "OpenGLContext::setViewPort : "
				"unexpected error reported." ) ;
			break ;	
				
	}
	
	// Restore the aspect ratio of the projection :
	updateProjection() ; 
	
}


void OpenGLContext::setOrthographicProjection( GLLength width, 
	GLCoordinate near, GLCoordinate far ) throw ( OpenGLException )
{

	_projectionMode  = Orthographic ;
	_projectionWidth = width ;

	glMatrixMode( GL_PROJECTION ) ;
	glLoadIdentity() ;
	
	// Centers projection box in (x,y) planes:
	
	GLCoordinate right = width / 2 ;
	
	// Enforce the viewport aspect ratio :
	GLCoordinate top = ( width * _viewportHeight ) / ( 2 * _viewportWidth ) ;
	
	glOrtho( /* left */ -right, /* right */ right, 
		/* bottom */ -top, /* top */ top,
		/* near */ near, /* far */ far ) ;

	switch ( glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::setOrthographicProjection : "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			LogPlug::warning( "OpenGLContext::setOrthographicProjection : "
				"unexpected error reported." ) ;
			break ;	
				
	}

}


void OpenGLContext::clearViewport() throw( OpenGLException )
{

	glClear( GL_COLOR_BUFFER_BIT ) ;

	switch ( glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_VALUE:
			throw OpenGLException( "OpenGLContext::clearViewport : "
				"invalid bit in specified clear mask." ) ;
			break ;
		
		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::clearViewport : "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			LogPlug::warning( "OpenGLContext::clearViewport : "
				"unexpected error reported." ) ;
			break ;	
				
	}
	
}	


void OpenGLContext::clearDepthBuffer() throw( OpenGLException )
{

	glClear( GL_DEPTH_BUFFER_BIT ) ;

	switch ( glGetError() )
	{
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_VALUE:
			throw OpenGLException( "OpenGLContext::clearDepthBuffer : "
				"invalid bit in specified clear mask." ) ;
			break ;
		
		case GL_INVALID_OPERATION:
			throw OpenGLException( "OpenGLContext::clearDepthBuffer : "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			LogPlug::warning( "OpenGLContext::clearDepthBuffer : "
				"unexpected error reported." ) ;
			break ;	
				
	}
	
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
	
	res.push_back( "Red component size : "   
		+ Ceylan::toNumericalString( redSize ) + " bits." ) ;
		
	res.push_back( "Green component size : " 
		+ Ceylan::toNumericalString( greenSize ) + " bits." ) ;
		
	res.push_back( "Blue component size : "  
		+ Ceylan::toNumericalString( blueSize ) + " bits." ) ;
	
	// Alpha ?
			
	return "Current OpenGL state is :" + Ceylan::formatStringList( res ) ;
	
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

	switch ( _projectionMode )
	{
	
		case Orthographic:
			// Forces recomputation of projection height :
			setOrthographicProjection( _projectionWidth,
				 _nearClippingPlane, _farClippingPlane ) ; 
			break ;
			
		default:
			throw OpenGLException( "OpenGLContext::updateProjection	: "
				"not implemented for current projection." ) ;
			break ;
				
	}	
			
}



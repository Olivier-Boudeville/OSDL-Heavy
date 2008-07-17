#include "OSDLGLTexture.h"

#include "OSDLOpenGL.h"              // for EnableFeature and al
#include "OSDLVideo.h"               // for VideoModule::SoftwareSurface
#include "OSDLSurface.h"             // for Surface


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>              // for OSDL_USES_OPENGL and al 
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_OPENGL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL 
#include "SDL.h"					 // for SDL_Surface, etc.
#endif // OSDL_USES_SDL


#ifdef OSDL_HAVE_OPENGL
#include "SDL_opengl.h"              // for GL functions
#endif // OSDL_HAVE_OPENGL



// To protect LoadImage:
#include "OSDLIncludeCorrecter.h"

using std::string ;

using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::Maths ; // for IsAPowerOfTwo 

using namespace OSDL::Video ;
using namespace OSDL::Video::OpenGL ;


GLTexture::TextureDimensionality GLTexture::CurrentTextureDimensionality = 
	TwoDim ;


/*
 * Implementation notes:
 * 
 * OSDL_USES_OPENGL being not defined not always triggers an exception in
 * all non-static methods, as the constructor would have done that already.
 *
 */


// Replicating these defines allows to enable them on a per-class basis:
#if OSDL_VERBOSE_VIDEO_MODULE

#define LOG_DEBUG_TEXTURE(message)   LogPlug::debug(message)
#define LOG_TRACE_TEXTURE(message)   LogPlug::trace(message)
#define LOG_WARNING_TEXTURE(message) LogPlug::warning(message)

#else // OSDL_VERBOSE_VIDEO_MODULE

#define LOG_DEBUG_TEXTURE(message)
#define LOG_TRACE_TEXTURE(message)
#define LOG_WARNING_TEXTURE(message)

#endif // OSDL_VERBOSE_VIDEO_MODULE



GLTextureException::GLTextureException( const std::string & reason ) throw():
	OpenGLException( reason )
{

}

					
GLTextureException::~GLTextureException() throw()
{

}




GLTexture::GLTexture( const std::string imageFilename, TextureFlavour flavour ) 
		throw( GLTextureException ):
	_source( 0 ),
	_id( 0 ),
	_flavour( flavour ),
	_width( 0 ),
	_height( 0 )
{

#if OSDL_USES_OPENGL
	
	LOG_DEBUG_TEXTURE( "Constructing a GLTexture from file " + imageFilename ) ;
	
	Surface * loaded ;
	
	try
	{
	
		loaded = & Surface::LoadImage( imageFilename, 
			/* convertToDisplay */ false ) ;
			
	}
	catch( const TwoDimensional::ImageException & e )
	{
	
		throw GLTextureException( 
			"GLTexture constructor: unable to load source image from file '"
			+ imageFilename + "': " + e.toString() ) ;
	}
	
	upload( *loaded ) ;
	
	// Texture not wanted any more, in all cases (it has been copied):
	delete loaded ;

#else // OSDL_USES_OPENGL

	throw GLTextureException( "GLTexture constructor: "
		"OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL
		
}



GLTexture::GLTexture( Surface & sourceSurface, TextureFlavour flavour )
		throw( GLTextureException ):
	_source( 0 ),
	_id( 0 ),
	_flavour( flavour ),
	_width( 0 ),
	_height( 0 )
{

	LOG_DEBUG_TEXTURE( "Constructing a GLTexture from a surface" ) ;

	upload( sourceSurface ) ;
	
}



GLTexture::~GLTexture() throw()
{

#if OSDL_USES_OPENGL

	LOG_DEBUG_TEXTURE( "Deleting a GLTexture" ) ;

	if ( _id != 0 )
		glDeleteTextures( 1, & _id ) ;
		
	if ( _source != 0 )
		delete _source ;

#endif // OSDL_USES_OPENGL
		
}



bool GLTexture::canBeUploaded() const throw()
{

	return ( _source != 0 ) ;
	
}



void GLTexture::upload() throw( GLTextureException )
{

#if OSDL_USES_OPENGL

	LOG_DEBUG_TEXTURE( "GLTexture::upload" ) ;

	if ( ! canBeUploaded() )
		throw GLTextureException( "GLTexture::upload: "
			"texture cannot be uploaded into OpenGL context." ) ;
			
	// @todo	
	// if ( OpenGLContext::ContextCanBeLost	) ... else..

#else // OSDL_USES_OPENGL

	throw GLTextureException( "GLTexture::upload failed: "
		"OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL
	
}



const string GLTexture::toString( Ceylan::VerbosityLevels level ) const throw()
{

	string res = "OpenGL texture, " ;
	
	if ( _id == 0 )
		res += "which has no OpenGL identifier" ;
	else
		res += "whose OpenGL identifier is " + Ceylan::toString( _id ) ;
	
	if ( _source == 0 )
		res += ", and which has no available internal surface "
			"kept for reload." ;
	else
		res += ", which owns an internal surface, "
			"kept for reloading purposes." ;

	res += " Its current size (width x height) is " 
		+ Ceylan::toString( _width ) + "x" 
		+ Ceylan::toString( _height ) + " pixels" ;
	
	return res ;	 	
		
}
	


// Static section.


GLTexture::TextureDimensionality GLTexture::GetTextureDimensionality() throw()
{

	return CurrentTextureDimensionality ;
	
}



void GLTexture::SetTextureDimensionality( 
	TextureDimensionality newDimensionality ) throw()
{

	CurrentTextureDimensionality = newDimensionality ;
	
}



void GLTexture::SetTextureFlavour( TextureFlavour textureFlavour ) 
	throw( GLTextureException )
{

#if OSDL_USES_OPENGL

	LogPlug::trace( "GLTexture::SetTextureFlavour" ) ;
	
	switch( textureFlavour )
	{
	
	
		case None:
			return ;
			break ;
		
		
		case Basic:

			LogPlug::trace( "GLTexture::SetTextureFlavour: Basic" ) ;
		
		 	/*
			 * Minifying function: weighted average of the four closest 
			 * texture elements.
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) ;
			
		 	/*
			 * Magnifying function: weighted average of the four closest
			 * texture elements.
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) ;
	
			/*
			 * Wrap parameter for texture coordinate s: clamped to the range
			 * [0,1].
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP ) ;
			
			/*
			 * Wrap parameter for texture coordinate t: clamped to the 
			 * range [0,1].
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP ) ;
			
			break ;		
	
	
		case For2D:
		
			LogPlug::trace( "GLTexture::SetTextureFlavour: For2D" ) ;

			// Two-dimensional texturing will be performed:
			OpenGLContext::EnableFeature( GL_TEXTURE_2D ) ;
	
			GLTexture::SetTextureDimensionality( GLTexture::TwoDim ) ;

			/*
			 * To perform correct alphablending:
			 *
			 */
			SetTextureEnvironmentParameter( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
				GL_MODULATE ) ;

		 	/*
			 * Filters uses GL_NEAREST instead of GL_LINEAR as:
			 *  1. sprites will be rendered exactly as defined (pixel-perfect,
			 * no zoom/filtering should be performed)
			 *  2. otherwise, when the sprite dimensions will be both powers of
			 * two (which should be always the case), glTexImage2D will be used
			 * instead of gluBuild2DMipmaps, and the former defines only the 
			 * base mipmap, which therefore must be the only one to be used.
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) ;
			
		 	/*
			 * Magnifying function: weighted average of the four closest
			 * texture elements.
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ) ;
	
			/*
			 * Wrap parameter for texture coordinate s: clamped to the range
			 * [0,1].
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP ) ;
			
			/*
			 * Wrap parameter for texture coordinate t: clamped to the 
			 * range [0,1].
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP ) ;
			
			break ;		
			
	
		default:
			throw GLTextureException( 
				"GLTexture::SetTextureFlavour: unknown texture flavour: "
				+ Ceylan::toString( textureFlavour ) + "." ) ;
			break ;	
			
	}

#else // OSDL_USES_OPENGL

	throw GLTextureException( "GLTexture::SetTextureFlavour failed: "
		"OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL
	
}



void GLTexture::SetTextureEnvironmentParameter( 
		GLEnumeration targetEnvironment,
		GLEnumeration environmentParameter,
		GLfloat parameterValue ) 
	throw( GLTextureException )
{

#if OSDL_USES_OPENGL

	glTexEnvf( targetEnvironment, environmentParameter, parameterValue ) ;
	

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_ENUM:
			throw GLTextureException(
				"GLTexture::SetTextureEnvironmentParameter: "
				"invalid enumeration." ) ;
			break ;
				
		case GL_INVALID_VALUE:
			throw GLTextureException(
				"GLTexture::SetTextureEnvironmentParameter: "
				"invalid enumeration." ) ;
			break ;
				
		case GL_INVALID_OPERATION:
			throw GLTextureException(
				"GLTexture::SetTextureEnvironmentParameter: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			throw GLTextureException(
				"GLTexture::SetTextureEnvironmentParameter: "
				"unexpected error reported." ) ;
			break ;	
				
	}

#endif // OSDL_CHECK_OPENGL_CALLS
	

#else // OSDL_USES_OPENGL
	
	throw GLTextureException( 
		"GLTexture::SetTextureEnvironmentParameter failed: "
		"no OpenGL support available" ) ;
		
#endif // OSDL_USES_OPENGL

}



void GLTexture::SetTextureEnvironmentParameter( 
		GLEnumeration targetEnvironment,
		GLEnumeration environmentParameter,
		const GLfloat * parameterValues ) 
	throw( GLTextureException )
{

#if OSDL_USES_OPENGL

	glTexEnvfv( targetEnvironment, environmentParameter, parameterValues ) ;
	

#if OSDL_CHECK_OPENGL_CALLS

	switch ( glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_ENUM:
			throw GLTextureException(
				"GLTexture::SetTextureEnvironmentParameter: "
				"invalid enumeration." ) ;
			break ;
				
		case GL_INVALID_VALUE:
			throw GLTextureException(
				"GLTexture::SetTextureEnvironmentParameter: "
				"invalid enumeration." ) ;
			break ;
				
		case GL_INVALID_OPERATION:
			throw GLTextureException(
				"GLTexture::SetTextureEnvironmentParameter: "
				"incorrectly executed between the execution of glBegin and "
				"the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			throw GLTextureException(
				"GLTexture::SetTextureEnvironmentParameter: "
				"unexpected error reported." ) ;
			break ;	
				
	}

#endif // OSDL_CHECK_OPENGL_CALLS
	

#else // OSDL_USES_OPENGL
	
	throw GLTextureException( 
		"GLTexture::SetTextureEnvironmentParameter failed: "
		"no OpenGL support available" ) ;
		
#endif // OSDL_USES_OPENGL

}




// Protected section.


void GLTexture::upload( Surface & sourceSurface ) throw( GLTextureException ) 
{

#if OSDL_USES_OPENGL


	LogPlug::trace( "GLTexture::upload" ) ;

	_width  = sourceSurface.getWidth() ;
	_height = sourceSurface.getHeight() ;
	
	
	// Inspired from Stephane Marchesin's routine.
	
	try
	{
	
		/*
		 * No alpha blending, no RLE acceleration should be used, and 
		 * overall alpha is set to full transparency.
		 *
		 */

		// Colorkey not managed.
	
		// Saves the alpha blending attributes:
		Flags savedFlags = sourceSurface.getFlags() & ( 
			Surface::AlphaBlendingBlit | Surface::RLEColorkeyBlitAvailable ) ;
	
		// Saves the overall surface alpha value:
		Pixels::ColorElement savedAlpha = sourceSurface.getPixelFormat().alpha ;
	
		bool mustModifyOverallAlpha =
			( ( savedFlags & Surface::AlphaBlendingBlit ) != 0 ) ;
	
		if ( mustModifyOverallAlpha ) 
		  	sourceSurface.setAlpha( /* disable alpha blending */ 0, 
				/* new per-surface alpha value */ AlphaTransparent ) ;
	
		Length width  = sourceSurface.getWidth() ;
		Length height = sourceSurface.getHeight() ;
	
		// To avoid having transparent surfaces, use 0 for AlphaMask ?
		
		// Temporary surface for the uploading:
		Surface & convertedSurface =  * new Surface(
			VideoModule::SoftwareSurface, width, height, 32 /* bits per pixel */,
			RedMask, GreenMask, BlueMask, AlphaMask ) ;
			
		sourceSurface.blitTo( convertedSurface ) ;
		 

		// Restores the alpha blending attributes of the source surface:
		if ( mustModifyOverallAlpha ) 
		  	sourceSurface.setAlpha( /* alpha flags */ savedFlags, 
				/* new per-surface alpha value */ savedAlpha ) ;
	
	
		// Generates a new name for this texture in its identifier member:
		glGenTextures( /* one name requested */ 1, & _id ) ;

#if OSDL_CHECK_OPENGL_CALLS

		switch ( glGetError() )
		{
	
			case GL_NO_ERROR:
				break ;
		
			case GL_INVALID_VALUE:
				throw OpenGLException( 
					"GLTexture::upload failed (glGenTextures): "
					"invalid number of requested names." ) ;
				break ;
		
			case GL_INVALID_OPERATION:
				throw OpenGLException( 
					"GLTexture::upload failed (glGenTextures): "
					"incorrectly executed between the execution of glBegin and "
					"the corresponding execution of glEnd." ) ;
				break ;
		
			default:
				LogPlug::warning( "GLTexture::upload failed (glGenTextures): "
					"unexpected error reported." ) ;
				break ;	
				
		}

#endif // OSDL_CHECK_OPENGL_CALLS

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST ) ;
	
	
		/*
		 * 2-dimensional textures here:
		 *
		 * @note "The state of a n-dimensional texture immediately after it 
		 * is first bound is equivalent to the state of the default 
		 * GL_TEXTURE_nD at GL initialization": it implies this call will reset
		 * the texture settings, notably GL_TEXTURE_MIN_FILTER, which would 
		 * result in glTexImage2D selecting a blank texture (filter being 
		 * GL_LINEAR instead of GL_NEAREST).
		 *
		 */
		glBindTexture( /* texturing target */ GL_TEXTURE_2D, 
			/* name to bind */ _id ) ;
		
		// We have therefore to re-set the texture settings:
		SetTextureFlavour( _flavour ) ;
		
		
#if OSDL_CHECK_OPENGL_CALLS

		switch ( glGetError() )
		{
	
			case GL_NO_ERROR:
				break ;
		
			case GL_INVALID_ENUM:
				throw OpenGLException( 
					"GLTexture::upload failed (glBindTexture): "
					"target is not an allowed value." ) ;
				break ;
		
			case GL_INVALID_OPERATION:
				throw OpenGLException( 
					"GLTexture::upload failed (glBindTexture): "
					"wrong dimensionality or incorrectly executed between "
					"the execution of glBegin and the corresponding execution "
					"of glEnd." ) ;
				break ;
		
			default:
				LogPlug::warning( "GLTexture::upload failed (glBindTexture): "
					"unexpected error reported." ) ;
				break ;	
				
		}

#endif // OSDL_CHECK_OPENGL_CALLS
	
		if ( IsAPowerOfTwo( width ) && IsAPowerOfTwo( height ) )
		{


			LogPlug::trace( "GLTexture::upload: glTexImage2D with "
				+ Ceylan::toString( width ) + "x" 
				+ Ceylan::toString( height ) ) ;
		
			
        	glTexImage2D( 
				/* target texture */ GL_TEXTURE_2D, 
				/* level-of-detail number: base image */ 0, 
				/* number of color components */ GL_RGBA, 
				/* already a power of two */ width, 
				/* already a power of two */ height, 
				/* no border */ 0, 
				/* pixel format */ GL_RGBA, 
				/* pixel data type */ GL_UNSIGNED_BYTE, 
				convertedSurface.getPixels() ) ;

#if OSDL_CHECK_OPENGL_CALLS
			
			switch ( glGetError() )
			{
	
				case GL_NO_ERROR:
					break ;
		
				case GL_INVALID_ENUM:
					throw OpenGLException( 
						"GLTexture::upload failed (glTexImage2D): "
						"invalid enum." ) ;
					break ;
		
				case GL_INVALID_VALUE:
					throw OpenGLException( 
						"GLTexture::upload failed (glTexImage2D): "
						"invalid value." ) ;
					break ;
		
				case GL_INVALID_OPERATION:
					throw OpenGLException( 
						"GLTexture::upload failed (glTexImage2D): "
						"incorrectly executed between the execution of "
						"glBegin and the corresponding execution of glEnd." ) ;
					break ;
		
				default:
					throw OpenGLException( 
						"GLTexture::upload failed (glTexImage2D): "
						"unexpected error reported." ) ;
					break ;	
				
			}

#endif // OSDL_CHECK_OPENGL_CALLS

		}		
		else
		{
        	
			LogPlug::trace( "GLTexture::upload: gluBuild2DMipmaps" ) ;

			// At least one dimension is not a power of two:
			GLU::Int res = gluBuild2DMipmaps( 
				/* target texture */ GL_TEXTURE_2D, 
				/* number of color components */ GL_RGBA, 
				/* may be a non-power of two */ width, 
				/* may be a non-power of two */ height, 
				/* pixel format */ GL_RGBA, 
				/* pixel data type */ GL_UNSIGNED_BYTE, 
				convertedSurface.getPixels() ) ;
				
#if OSDL_CHECK_OPENGL_CALLS

			switch ( res )
			{
	
				case 0:
					// Success.
					break ;
					
				case GLU_INVALID_ENUM:
					throw OpenGLException( 
						"GLTexture::upload failed (gluBuild2DMipmaps): "
						"invalid enum." ) ;
					break ;
		
				case GLU_INVALID_VALUE:
					throw OpenGLException( 
						"GLTexture::upload failed (gluBuild2DMipmaps): "
						"invalid value." ) ;
					break ;
				
				default:
					throw OpenGLException( 
						"GLTexture::upload failed (gluBuild2DMipmaps): "
						"unexpected error reported." ) ;
					break ;	
				
			}

#endif // OSDL_CHECK_OPENGL_CALLS
				
		}
		
		delete & convertedSurface ;
		
	}
	catch( const VideoException & e )
	{
	
		throw GLTextureException( "GLTexture::upload failed: "
			+ e.toString() ) ;
			
	}
	
	
#else // OSDL_USES_OPENGL

	throw GLTextureException( "GLTexture::upload failed: "
		", altough OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL

}	


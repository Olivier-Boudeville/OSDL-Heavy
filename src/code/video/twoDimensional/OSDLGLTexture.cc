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


#include "OSDLGLTexture.h"

#include "OSDLOpenGL.h"              // for EnableFeature and al
#include "OSDLBasic.h"               // for getExistingCommonModule and al
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
using namespace OSDL::Video::Pixels ;
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



GLTextureException::GLTextureException( const std::string & reason ) :
	OpenGLException( reason )
{

}

			
					
GLTextureException::~GLTextureException() throw()
{

}





GLTexture::GLTexture( const std::string textureFilename,
		TextureFlavour flavour, bool preload ):
	Ceylan::LoadableWithContent<Surface>( textureFilename ),
	_id( 0 ),
	_flavour( flavour ),
	_unloadable( false )
{

#if OSDL_USES_OPENGL

	LOG_DEBUG_TEXTURE( "Constructing a GLTexture from a file" ) ;

	if ( preload )
	{
	
		try
		{
		
			load() ;
			
		}
		catch( const Ceylan::LoadableException & e )
		{
		
			throw GLTextureException( 
				"GLTexture constructor failed while preloading: " 
				+ e.toString() ) ;
				
		}
			
	}

#else // OSDL_USES_OPENGL

	/*
	 * Prevents from creating a texture instance if no OpenGL support is
	 * available (then methods have not to be protected specifically)
	 *
	 */
	throw GLTextureException( "GLTexture constructor failed: "
		"no OpenGL support available." ) ;
	 
#endif // OSDL_USES_OPENGL
		
}



GLTexture::GLTexture( Surface & sourceSurface, TextureFlavour flavour ) :
	Ceylan::LoadableWithContent<Surface>( "(non applicable)" ),
	_id( 0 ),
	_flavour( flavour ),
	_unloadable( true )
{

	LOG_DEBUG_TEXTURE( "Constructing a GLTexture from a surface" ) ;

	setUpInternalSurfaceFrom( sourceSurface ) ;
			
}




GLTexture::~GLTexture() throw()
{
	
	LOG_DEBUG_TEXTURE( "Deleting a GLTexture" ) ;

	try
	{
	
		/* 
		 * Tries to remove unconditionally the texture from video card, as
		 * we are not able to know whether it is still there:
		 * (OpenGL call ignores names (i.e. id) that do not correspond to
		 * existing textures; isResident may not give this information).
		 *
		 */
		remove() ;
	
	}
	catch( const GLTextureException & e )
	{
		
		LogPlug::error( "GLTexture destructor failed "
			"while removing it from video card: " + e.toString() ) ;
		
	}

	
	try
	{
	
		if ( hasContent() )
		{
		
			if ( _unloadable )
			{
			
				delete _content ;
				_content = 0 ;
				
			}
			else
			{
			
				unload() ;
				
			}	
			
		}		
	
	}
	catch( const Ceylan::LoadableException & e )
	{
		
		LogPlug::error( "GLTexture destructor failed while unloading texture: " 
			+ e.toString() ) ;
		
	}
	
		
	//LogPlug::trace( "GLTexture deallocated." ) ;

}



Length GLTexture::getWidth() const
{

	if ( _content != 0 )
		return _content->getWidth() ;
	else
		throw GLTextureException( "GLTexture::getWidth called "
			"whereas texture not loaded." ) ;

}



Length GLTexture::getHeight() const
{

	if ( _content != 0 )
		return _content->getHeight() ;
	else
		throw GLTextureException( "GLTexture::getHeight called "
			"whereas texture not loaded." ) ;
	
}





bool GLTexture::wasUploaded() const
{

	return ( _id != 0 ) ;
	
}




/*
 * Note: an upload method from any existing surface could be added back
 * (available in previous versions, see source control).
 *
 */


void GLTexture::upload()
{

#if OSDL_USES_OPENGL

	//LogPlug::trace( "GLTexture::upload" ) ;

	/*
	 * This checking is probably a bit too expensive/useless for releases
	 * to be done each time, however it will save some absent-minded developers:
	 *
	 */
#if OSDL_DEBUG

	// Here we have to have a proper OpenGL context already available:

	if ( ! OSDL::getExistingCommonModule().getVideoModule().hasOpenGLContext() )
		throw GLTextureException( "GLTexture::upload called "
			"whereas OpenGL has not been initialized (no OpenGL context)." ) ;

#endif // OSDL_DEBUG	
	

	if ( wasUploaded() )
		throw GLTextureException( "GLTexture::upload called "
			"whereas a texture identifier is already available." ) ;
		
	if ( ! hasContent() )
		load() ;

	
	// Generates a new name for this texture in its identifier member:
	glGenTextures( /* one name requested */ 1, & _id ) ;


#if OSDL_CHECK_OPENGL_CALLS

	switch ( ::glGetError() )
	{
	
		case GL_NO_ERROR:
				break ;
		
		case GL_INVALID_VALUE:
			throw GLTextureException( 
				"GLTexture::upload failed (glGenTextures): "
				"invalid number of requested names." ) ;
			break ;
		
		case GL_INVALID_OPERATION:
			throw GLTextureException( 
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
	
		
#if OSDL_CHECK_OPENGL_CALLS

	switch ( ::glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
	
		case GL_INVALID_ENUM:
			throw GLTextureException( 
				"GLTexture::upload failed (glBindTexture): "
				"target is not an allowed value." ) ;
			break ;
	
		case GL_INVALID_OPERATION:
			throw GLTextureException( 
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
	
	/*
	 * We have therefore to re-set the texture settings:
	 * (beware to this side-effect!)
	 *
	 */
	SetTextureFlavour( _flavour ) ;
	

	Length width  = getWidth() ;
	Length height = getHeight() ;
	
	
	if ( IsAPowerOfTwo( width ) && IsAPowerOfTwo( height ) )
	{

		LogPlug::trace( "GLTexture::upload: glTexImage2D with "
			+ Ceylan::toString( width ) + "x" 
			+ Ceylan::toString( height ) + ", for: " + _content->toString() ) ;
	
		
    	glTexImage2D( 
			/* target texture */ GL_TEXTURE_2D, 
			/* level-of-detail number: base image */ 0, 
			/* number of color components */ GL_RGBA, 
			/* already a power of two */ width, 
			/* already a power of two */ height, 
			/* no border */ 0, 
			/* pixel format */ GL_RGBA, 
			/* pixel data type */ GL_UNSIGNED_BYTE, 
			_content->getPixels() ) ;

#if OSDL_CHECK_OPENGL_CALLS
			
		switch ( ::glGetError() )
		{
	
			case GL_NO_ERROR:
				break ;
		
			case GL_INVALID_ENUM:
				throw GLTextureException( 
					"GLTexture::upload failed (glTexImage2D): "
					"invalid enum." ) ;
				break ;
		
			case GL_INVALID_VALUE:
				throw GLTextureException( 
					"GLTexture::upload failed (glTexImage2D): "
					"invalid value." ) ;
				break ;
		
			case GL_INVALID_OPERATION:
				throw GLTextureException( 
					"GLTexture::upload failed (glTexImage2D): "
					"incorrectly executed between the execution of "
					"glBegin and the corresponding execution of glEnd." ) ;
				break ;
		
			default:
				throw GLTextureException( 
					"GLTexture::upload failed (glTexImage2D): "
					"unexpected error reported." ) ;
				break ;	
			
		}

#endif // OSDL_CHECK_OPENGL_CALLS

	}		
	else
	{
        	
		LogPlug::trace( "GLTexture::upload: gluBuild2DMipmaps with "
			+ Ceylan::toString( width ) + "x" 
			+ Ceylan::toString( height ) + ", for: " + _content->toString() ) ;

			
		// At least one dimension is not a power of two:
		GLU::Int res = gluBuild2DMipmaps( 
			/* target texture */ GL_TEXTURE_2D, 
			/* number of color components */ GL_RGBA, 
			/* may be a non-power of two */ width, 
			/* may be a non-power of two */ height, 
			/* pixel format */ GL_RGBA, 
			/* pixel data type */ GL_UNSIGNED_BYTE, 
			_content->getPixels() ) ;
				
#if OSDL_CHECK_OPENGL_CALLS

		switch ( res )
		{
	
			case 0:
				// Success.
				break ;
				
			case GLU_INVALID_ENUM:
				throw GLTextureException( 
					"GLTexture::upload failed (gluBuild2DMipmaps): "
					"invalid enum." ) ;
				break ;
		
			case GLU_INVALID_VALUE:
				throw GLTextureException( 
					"GLTexture::upload failed (gluBuild2DMipmaps): "
					"invalid value." ) ;
				break ;
			
			default:
				throw GLTextureException( 
					"GLTexture::upload failed (gluBuild2DMipmaps): "
					"unexpected error reported." ) ;
				break ;	
			
		}

#endif // OSDL_CHECK_OPENGL_CALLS
				
	}
	
	// Still loaded in all cases.
				
	
#else // OSDL_USES_OPENGL

	throw GLTextureException( "GLTexture::upload failed: "
		"OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL

}	

	


void GLTexture::remove()
{

#if OSDL_USES_OPENGL

	if ( _id != 0 )
	{
		
		::glDeleteTextures( 1, & _id ) ;

		_id = 0 ;

#if OSDL_CHECK_OPENGL_CALLS

		switch ( ::glGetError() )
		{
	
			case GL_NO_ERROR:
				break ;
		
			case GL_INVALID_VALUE:
				throw GLTextureException( "GLTexture::remove failed: "
					"incorrect negative ID specified." ) ;
				break ;
		
			case GL_INVALID_OPERATION:
				throw GLTextureException( "GLTexture::remove failed: "
					"incorrectly executed between the execution of glBegin "
					"and the corresponding execution of glEnd." ) ;
				break ;
		
			default:
				LogPlug::warning( "GLTexture::remove failed: "
					"unexpected error reported." ) ;
				break ;	
			
		}

#endif // OSDL_CHECK_OPENGL_CALLS

	}

	
#endif // OSDL_USES_OPENGL
		
}




void GLTexture::setAsCurrent() const
{
	
#if OSDL_USES_OPENGL

	//LOG_DEBUG_TEXTURE( "GLTexture::setAsCurrent" ) ;

	if ( _id == 0 )
		throw GLTextureException( "GLTexture::setAsCurrent: texture not loaded "
			"(no available texture identifier)" ) ;
			
	// We consider here that this is a const operation:
	::glBindTexture( /* target */ GL_TEXTURE_2D, /* texture */ _id ) ;
	
		
#if OSDL_CHECK_OPENGL_CALLS

	switch ( ::glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_VALUE:
			throw GLTextureException( "GLTexture::setAsCurrent failed: "
				"incorrect target specified." ) ;
			break ;
		
		case GL_INVALID_OPERATION:
			throw GLTextureException( "GLTexture::setAsCurrent failed: "
				"invalid operation, texture has a dimensionality that "
				"does not match that of target, "
				"or incorrectly executed between the execution of glBegin "
				"and the corresponding execution of glEnd." ) ;
			break ;
		
		default:
			LogPlug::warning( "GLTexture::setAsCurrent failed: "
				"unexpected error reported." ) ;
			break ;	
	}

#endif // OSDL_CHECK_OPENGL_CALLS
			
#else // OSDL_USES_OPENGL

	throw GLTextureException( "GLTexture::setAsCurrent failed: "
		"OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL
	
}



bool GLTexture::isResident()
{
	
#if OSDL_USES_OPENGL

	LOG_DEBUG_TEXTURE( "GLTexture::isResident" ) ;
	
	if ( _id == 0 )
		return false ;
		
		
	// Dummy:
	GLboolean residences ;
	
	
	/*
	 * glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_RESIDENT, etc.)
	 * would only query the texture currently bound.
	 *
	 */
	bool res = ( ::glAreTexturesResident( 
		/* number of textures to be queried */ 1,
		/* array containing the names of the textures to be queried */ & _id,
		/* dummy returned array */ &residences ) == GL_TRUE ) ;
		
#if OSDL_CHECK_OPENGL_CALLS

	switch ( ::glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
		
		case GL_INVALID_VALUE:
			throw GLTextureException( "GLTexture::isResident failed: "
				"invalid (negative?) texture number." ) ;
			break ;
		
		case GL_INVALID_OPERATION:
			throw GLTextureException( "GLTexture::isResident failed: "
				"invalid operation, incorrectly executed between "
				"the execution of glBegin and the corresponding execution "
				"of glEnd." ) ;
			break ;
		
		default:
			LogPlug::warning( "GLTexture::isResident failed: "
				"unexpected error reported." ) ;
			break ;	
				
	}

#endif // OSDL_CHECK_OPENGL_CALLS

	return res ;
			
#else // OSDL_USES_OPENGL

	throw GLTextureException( "GLTexture::isResident failed: "
		"OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL
	
}




// LoadableWithContent template instanciation.



bool GLTexture::load()
{

	if ( hasContent() )
		return false ;

	LOG_DEBUG_TEXTURE( "Constructing a GLTexture from file " + _contentPath ) ;

	try
	{

		// Inspired from Stephane Marchesin's routine. Thanks Stephane!

		/*
		 * No alpha blending, no RLE acceleration should be used, and 
		 * overall alpha is set to full transparency.
		 *
		 * Colorkey not managed.
		 *
		 */

		// Temporary surface:
		Surface * originalSurface = & Surface::LoadImage( _contentPath, 
			/* convertToDisplayFormat */ false, /* convertWithAlpha */ false ) ;
		
		setUpInternalSurfaceFrom( * originalSurface ) ;
		
	}
	catch( const Ceylan::Exception & e )
	{
	
		throw Ceylan::LoadableException( "GLTexture::load failed: "
			"unable to load from '" + _contentPath + "': " + e.toString() ) ;
			
	}	

	return true ;
	
	// _content loaded.
	
}



bool GLTexture::unload()
{

	if ( ! hasContent() )
		return false ;

	// There is content to unload here:

	if ( _unloadable )
		return false ;
		
	delete _content ;
	_content = 0 ;

	
	return true ;
	
}



const string GLTexture::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "OpenGL texture, " ;
	
	if ( _id == 0 )
		res += "which has no OpenGL identifier" ;
	else
		res += "whose OpenGL identifier is " + Ceylan::toString( _id ) ;
	
	res += ", whose content path is '" + _contentPath ;
	
	if ( _content == 0 )
		res += "', and which content is not loaded" ;
	else
		res += "', which content is loaded. "
			"Its current size (width x height) is " 
			+ Ceylan::toString( getWidth() ) + "x" 
			+ Ceylan::toString( getHeight() ) + " pixels" ;
	
	return res ;	 	
		
}
	



// Static section.



GLTexture::TextureDimensionality GLTexture::GetTextureDimensionality()
{

	return CurrentTextureDimensionality ;
	
}



void GLTexture::SetTextureDimensionality( 
	TextureDimensionality newDimensionality )
{

	CurrentTextureDimensionality = newDimensionality ;
	
}



void GLTexture::SetTextureFlavour( TextureFlavour textureFlavour ) 
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
			OpenGLContext::EnableFeature( OpenGL::TwoDimensionalTexturing ) ;
	
			GLTexture::SetTextureDimensionality( GLTexture::TwoDim ) ;

			/*
			 * To perform correct alphablending:
			 *
			 */
			SetTextureEnvironmentParameter( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
				GL_MODULATE ) ;

		 	/*
			 * Filters are supposed to use GL_NEAREST instead of GL_LINEAR, as:
			 *  1. sprites will be rendered exactly as defined (pixel-perfect,
			 * no zoom/filtering should be performed)
			 *  2. otherwise, when the sprite dimensions will be both powers of
			 * two (which should be always the case), glTexImage2D will be used
			 * instead of gluBuild2DMipmaps, and the former defines only the 
			 * base mipmap, which therefore must be the only one to be used.
			 *
			 * However after some testing, even for textures for 2D rendering,
			 * the final result is very poor with GL_NEAREST, whereas it seems
			 * perfect with GL_LINEAR, so we preferred GL_LINEAR:
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
				/* GL_NEAREST */ GL_LINEAR ) ;
			
		 	/*
			 * Magnifying function: weighted average of the four closest
			 * texture elements.
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
				/* GL_NEAREST */ GL_LINEAR ) ;
	
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

			
		case For3D:
		
			LogPlug::trace( "GLTexture::SetTextureFlavour: For3D" ) ;

			// Two-dimensional texturing will be performed:
			OpenGLContext::EnableFeature( OpenGL::TwoDimensionalTexturing ) ;
	
			GLTexture::SetTextureDimensionality( GLTexture::TwoDim ) ;

			/*
			 * To perform correct alphablending:
			 *
			 */
			SetTextureEnvironmentParameter( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
				GL_MODULATE ) ;

		 	/*
			 * GL_LINEAR preferred to GL_NEAREST for 3D, as it is smoother.
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
				GL_LINEAR ) ;
			
		 	/*
			 * Magnifying function: weighted average of the four closest
			 * texture elements.
			 *
			 */
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
				GL_LINEAR ) ;
	
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
{

#if OSDL_USES_OPENGL

	glTexEnvf( targetEnvironment, environmentParameter, parameterValue ) ;
	

#if OSDL_CHECK_OPENGL_CALLS

	switch ( ::glGetError() )
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



void GLTexture::UnbindCurrentTexture()
{

#if OSDL_USES_OPENGL

	glBindTexture( /* texturing target */ GL_TEXTURE_2D, 
		/* name to bind */ 0 ) ;
	
		
#if OSDL_CHECK_OPENGL_CALLS

	switch ( ::glGetError() )
	{
	
		case GL_NO_ERROR:
			break ;
	
		case GL_INVALID_ENUM:
			throw GLTextureException( 
				"GLTexture::UnbindCurrentTexture failed: "
				"target is not an allowed value." ) ;
			break ;
	
		case GL_INVALID_OPERATION:
			throw GLTextureException( 
				"GLTexture::UnbindCurrentTexture failed: "
				"wrong dimensionality or incorrectly executed between "
				"the execution of glBegin and the corresponding execution "
				"of glEnd." ) ;
			break ;
	
		default:
			LogPlug::warning( "GLTexture::UnbindCurrentTexture failed: "
				"unexpected error reported." ) ;
			break ;	
			
	}

#endif // OSDL_CHECK_OPENGL_CALLS

#else // OSDL_USES_OPENGL
	
	throw GLTextureException( 
		"GLTexture::UnbindCurrentTexture failed: "
		"no OpenGL support available" ) ;
		
#endif // OSDL_USES_OPENGL

}



std::string GLTexture::GetExtensionForFlavour( TextureFlavour flavour )
{

	switch( flavour )
	{
	
		case None:
			return ".tex" ;

		case Basic:
			return ".tex" ;
			
		case For2D:
			return ".tex2D" ;
			
		case For3D:
			return ".tex3D" ;
		
		default:
			throw GLTextureException( "GLTexture::GetExtensionForFlavour: "
				"unknown flavour (" + Ceylan::toString(flavour) + ")" ) ;
			
	}
	
}



void GLTexture::SetTextureEnvironmentParameter( 
	GLEnumeration targetEnvironment,
	GLEnumeration environmentParameter,
	const GLfloat * parameterValues ) 
{

#if OSDL_USES_OPENGL

	glTexEnvfv( targetEnvironment, environmentParameter, parameterValues ) ;
	

#if OSDL_CHECK_OPENGL_CALLS

	switch ( ::glGetError() )
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


// Helper used by multiple code paths.
void GLTexture::setUpInternalSurfaceFrom( Surface & sourceSurface )
{

	if ( ( sourceSurface.getFlags() & Surface::AlphaBlendingBlit ) != 0 )
		sourceSurface.setAlpha( /* disable alpha blending */ 0, 
			/* new per-surface alpha value */ AlphaTransparent ) ;

	// To avoid having transparent surfaces, use 0 for AlphaMask?
		
	_content = new Surface( VideoModule::SoftwareSurface,
		sourceSurface.getWidth(), sourceSurface.getHeight(), 
		/* bits per pixel */ 32, RedMask, GreenMask, BlueMask, AlphaMask ) ;

	sourceSurface.blitTo( *_content ) ;

	delete & sourceSurface ;
	
}


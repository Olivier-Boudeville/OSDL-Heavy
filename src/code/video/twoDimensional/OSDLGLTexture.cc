#include "OSDLGLTexture.h"

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
using namespace Ceylan::Maths ;

using namespace OSDL::Video ;
using namespace OSDL::Video::OpenGL ;


GLTexture::TextureMode GLTexture::CurrentTextureMode = TwoDim ;



GLTextureException::GLTextureException( const std::string & reason ) throw():
	OpenGLException( reason )
{

}

					
GLTextureException::~GLTextureException() throw()
{

}




GLTexture::GLTexture( const std::string imageFilename, 
	Textureflavour flavour ) 
		throw( GLTextureException ):
	_source( 0 ),
	_id( 0 )
{

#if OSDL_USES_OPENGL
	
	Surface * loaded ;
	
	try
	{
	
		loaded = & Surface::LoadImage( imageFilename, 
			/* convertToDisplay */ false ) ;
			
	}
	catch( const TwoDimensional::ImageException & e )
	{
	
		throw GLTextureException( 
			"GLTexture constructor: unable to load source image from file "
			+ imageFilename + ": " + e.toString() ) ;
	}
	
	upload( *loaded, flavour ) ;
	
	// Texture not wanted any more, in all cases (it has been copied):
	delete loaded ;

#else // OSDL_USES_OPENGL

	throw GLTextureException( "OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL
		
}



GLTexture::GLTexture( Surface & sourceSurface, Textureflavour flavour ) 
		throw( GLTextureException ):
	_source( 0 )
{

	upload( sourceSurface, flavour ) ;
	
}



GLTexture::~GLTexture() throw()
{

#if OSDL_USES_OPENGL

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

	if ( ! canBeUploaded() )
		throw GLTextureException( "GLTexture::upload: "
			"texture cannot be uploaded into OpenGL context." ) ;
			
	// @todo	
	// if ( OpenGLContext::ContextCanBeLost	) ... else..

#else // OSDL_USES_OPENGL

	throw GLTextureException( "OpenGL support not available." ) ;
	
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
	
	return res ;	 	
		
}
	


// Static section.


GLTexture::TextureMode GLTexture::GetTextureMode() throw()
{

	return CurrentTextureMode ;
	
}


void GLTexture::SetTextureMode( TextureMode newMode ) throw()
{

	CurrentTextureMode = newMode ;
	
}



void GLTexture::SetTextureFlavour( Textureflavour flavour ) 
	throw( GLTextureException )
{

#if OSDL_USES_OPENGL

	switch( flavour )
	{
	
		case None:
			return ;
			break ;
		
		
		case Basic:
		
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
	
	
		default:
			LogPlug::warning( 
				"GLTexture::SetTextureFlavour: unknown flavour: "
				+ Ceylan::toString( flavour ) + "." ) ;
			break ;	
			
	}

#else // OSDL_USES_OPENGL

	throw GLTextureException( "OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL
	
}



void GLTexture::upload( Surface & sourceSurface, 
	Textureflavour flavour ) throw( GLTextureException ) 
{

#if OSDL_USES_OPENGL

	/*
inspiration: 	
GLuint loadTextureCK(char *filepath, int ckr, int ckg, int ckb){
  GLuint texture;
  SDL_Surface *imagesurface;
  SDL_Surface *tmpsurface;
  Uint32 colorkey;
  int w, h;

  imagesurface = SDL_LoadBMP(filepath);
  if (!imagesurface)
    return 0;

  w = imagesurface->w;
  h = imagesurface->h;

  // create temporary surface with the correct OpenGL format
  tmpsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
(use Pixels::getRecommendedColorMask)                                    );
  if (!tmpsurface)
    return 0;

  // set colour key
  colorkey = SDL_MapRGBA(tmpsurface->format, ckr, ckg, ckb, 0);
  SDL_FillRect(tmpsurface, NULL, colorkey);

  colorkey = SDL_MapRGBA(imagesurface->format, ckr, ckg, ckb, 0);
  SDL_SetColorKey(imagesurface, SDL_SRCCOLORKEY, colorkey);

  SDL_BlitSurface(imagesurface, NULL, tmpsurface, NULL);

  // create OpenGL texture
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 
               0, GL_RGBA, GL_UNSIGNED_BYTE, tmpsurface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  SDL_FreeSurface(imagesurface);
  SDL_FreeSurface(tmpsurface);

  return texture;
		
	*/


/*
	// Inspired from Gabriel Gambetta's routine.

void OGLVideoDriver::initPixelFormats (void)
{
	#ifdef __APPLE__
		m_pPixelFormat.nBPP = 24;
		m_pPixelFormat.nAMask = 0x00000000;
		m_pPixelFormat.nRMask = 0x00FF0000;
		m_pPixelFormat.nGMask = 0x0000FF00;
		m_pPixelFormat.nBMask = 0x000000FF;

		m_pPixelFormatAlpha.nBPP = 32;
		m_pPixelFormatAlpha.nAMask = 0x000000FF;
		m_pPixelFormatAlpha.nRMask = 0xFF000000;
		m_pPixelFormatAlpha.nGMask = 0x00FF0000;
		m_pPixelFormatAlpha.nBMask = 0x0000FF00;
	#else
		m_pPixelFormat.nBPP = 24;
		m_pPixelFormat.nAMask = 0x00000000;
		m_pPixelFormat.nRMask = 0x000000FF;
		m_pPixelFormat.nGMask = 0x0000FF00;
		m_pPixelFormat.nBMask = 0x00FF0000;

		m_pPixelFormatAlpha.nBPP = 32;
		m_pPixelFormatAlpha.nAMask = 0xFF000000;
		m_pPixelFormatAlpha.nRMask = 0x000000FF;
		m_pPixelFormatAlpha.nGMask = 0x0000FF00;
		m_pPixelFormatAlpha.nBMask = 0x00FF0000;
	#endif
}

I convert all my surfaces to that format, and later...


	byte* pData;
	int nPitch;

	pSurf->lock(pRect, &pData, &nPitch);

	int nPitchBytes = nPitch;
	int nPitchMod = nPitch % nChannels;
	nPitch /= nChannels;

	static const int lModToAlign[4] = { 1, 2, 4, 8 };
	glPixelStorei(GL_UNPACK_ALIGNMENT, lModToAlign[nPitchMod]);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, nPitch);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nW, nH, nFormat,
GL_UNSIGNED_BYTE, pData);

	pSurf->unlock();

*/


/*




#include "SDL.h"
#include "SDL_endian.h"
#include "SDL_opengl.h"

int main(int, char**)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetVideoMode(400, 400, 0, SDL_OPENGL);

    // some.bmp is a red ship with a black backround
    SDL_Surface* original = SDL_LoadBMP("some.bmp");

    // We set a black colorkey for the ship
    SDL_SetColorKey(original, SDL_SRCCOLORKEY, 0);

    // We create a surface with known format to pass to opengl
    SDL_Surface* withalpha = SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        original->w,
        original->h,
        32,
        SDL_SwapBE32(0xff000000),
        SDL_SwapBE32(0x00ff0000),
        SDL_SwapBE32(0x0000ff00),
        SDL_SwapBE32(0x000000ff));

    // We blit one on top of the other, but withalpha keeps
    // a transparent black in the backround
    // We should flip the texture so it has the origin in the bottom,
    // as opengl expects, but we flip coordinates instead
    SDL_BlitSurface(original, NULL, withalpha, NULL);
    SDL_FreeSurface(original);

    // The sky is blue
    glClearColor(0, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // We enable skipping transparent texels
    glAlphaFunc(GL_GREATER, 0);
    glEnable(GL_ALPHA_TEST);

    GLuint texName = 0;
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        withalpha->w,
        withalpha->h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        withalpha->pixels);
    SDL_FreeSurface(withalpha);

    glBegin(GL_QUADS); {
        glTexCoord2d(0, 0);
        glVertex2d(-0.8, +0.8);
        glTexCoord2d(0, 1);
        glVertex2d(-0.8, -0.8);
        glTexCoord2d(1, 1);
        glVertex2d(+0.8, -0.8);
        glTexCoord2d(1, 0);
        glVertex2d(+0.8, +0.8);
    }glEnd();
    SDL_GL_SwapBuffers();

    // You should see a red ship over a blue backround
    SDL_Delay(1000);
    SDL_Quit();
    return 0;
}


*/


	// Inspired from Stephane Marchesin's routine.
	
	SDL_Surface * sourceInternal = & sourceSurface.getSDLSurface() ;
	
	/*
	 * No alpha blending, no RLE acceleration should be used, and 
	 * overall alpha is set to full transparency.
	 *
	 * 
	 */

	// Colorkey not managed.
	
	// Saves the alpha blending attributes:
	Flags savedFlags = sourceInternal->flags & ( 
		Surface::AlphaBlendingBlit | Surface::RLEColorkeyBlitAvailable ) ;
		
	Pixels::ColorElement savedAlpha = sourceInternal->format->alpha ;
	
	bool mustModifyOverallAlpha =
		( ( savedFlags & Surface::AlphaBlendingBlit ) != 0 ) ;
	
	if ( mustModifyOverallAlpha ) 
	  	SDL_SetAlpha( sourceInternal, 0, 0 ) ;
	
	Length width  = sourceInternal->w ;
	Length height = sourceInternal->h ;
	
	// To avoid having transparent surfaces, use 0 for AlphaMask ?
	SDL_Surface * convertedSurface = SDL_CreateRGBSurface(
		VideoModule::SoftwareSurface, 
		width, height, 32 /* bits per pixel */,
		RedMask, GreenMask, BlueMask, AlphaMask ) ;
	
	if ( convertedSurface == 0 )
		throw GLTextureException( 
			"GLTexture::upload: RGB surface creation failed." ) ;
		
		 
	int res = SDL_BlitSurface( sourceInternal, 0, convertedSurface, 0 ) ;

	// Restores the alpha blending attributes:
	if ( mustModifyOverallAlpha ) 
		SDL_SetAlpha( sourceInternal, savedFlags, savedAlpha ) ;
	
	if ( res != 0 )
	{
		SDL_FreeSurface( convertedSurface ) ;
		throw GLTextureException( "GLTexture::upload: blit failed (returned "
			+ Ceylan::toString( res ) + ")." ) ;
	}

	
	// Generates a new name for this texture:
	glGenTextures( 1, & _id ) ;
	
	glBindTexture( GL_TEXTURE_2D, _id ) ;
	
	// Apply the wanted settings:
	SetTextureFlavour( flavour ) ;
	
	if ( IsAPowerOfTwo( width ) && IsAPowerOfTwo( height ) )
        glTexImage2D( 
			/* mandatory */ GL_TEXTURE_2D, 
			/* level: base image */  0, 
			/* number of color components */ GL_RGBA, 
			width, 
			height, 
			/* border */ 0, 
			/* pixel format */ GL_RGBA, 
			/* pixel data type */ GL_UNSIGNED_BYTE, 
			convertedSurface->pixels ) ;
	else
        gluBuild2DMipmaps( 
			/* mandatory */ GL_TEXTURE_2D, 
			/* number of color components */ GL_RGBA, 
			width, 
			height,
			/* pixel format */ GL_RGBA, 
			/* pixel data type */ GL_UNSIGNED_BYTE, 
			convertedSurface->pixels ) ;
			
	SDL_FreeSurface( convertedSurface ) ;

#else // OSDL_USES_OPENGL

	throw GLTextureException( "OpenGL support not available." ) ;
	
#endif // OSDL_USES_OPENGL

}	


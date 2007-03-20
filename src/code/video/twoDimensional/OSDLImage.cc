#include "OSDLImage.h"

#include "OSDLVideo.h"              // for VideoModule
#include "OSDLUtils.h"              // for getBackendLastError
#include "OSDLSurface.h"
#include "OSDLPixel.h"


#include "SDL_image.h"              // for IMG_GetError and al

#include "png.h"                    // for png_structp and many others


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>             // for OSDL_DEBUG and al 
#endif // OSDL_USES_CONFIG_H


using std::string ;

using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;


/// JPG and PNG are the two recommended formats :

const string Image::JPGTag  = "JPG" ;
const string Image::PNGTag  = "PNG" ;


/// Other supported formats are :

const string Image::BMPTag  = "BMP" ;
const string Image::GIFTag  = "GIF" ;
const string Image::LBMTag  = "LBM" ;
const string Image::PCXTag  = "PCX" ;
const string Image::PNMTag  = "PNM" ;
const string Image::TGATag  = "TGA" ;
const string Image::XPMTag  = "XPM" ;



ImageException::ImageException( const std::string & reason ) throw() :
	VideoException( reason )
{

}


ImageException::~ImageException() throw()
{

}
	


/**
 * @see http://jcatki.no-ip.org/SDL_image/SDL_image.html
 *
 */

Image::Image( const string & filename ) throw( ImageException ) 
{

}


Image::~Image() throw() 
{

}	


Surface & Image::LoadIcon( const string & filename, 
	Pixels::ColorElement ** mask ) throw( ImageException )
{

	// Adapted from SDL example (testwm.c).
	
#if OSDL_DEBUG_IMAGE
	LogPlug::debug( "Loading icon." ) ;
#endif // OSDL_DEBUG_IMAGE

	// Do not convert to display, setMode should not have been called already !
	
	// Image format should be auto-detected :
	Surface & iconSurface = Surface::LoadImage( filename, 
		/* blitOnly */ false, /* convertToDisplay */ false ) ;

	
#if OSDL_DEBUG_IMAGE
	LogPlug::debug( "Icon loaded." ) ;
	
	LogPlug::debug( "Displaying icon informations : " 
		+ iconSurface.toString() ) ;
#endif // OSDL_DEBUG_IMAGE
	
	if ( iconSurface.getPixelFormat().palette == 0 ) 
	{
	
		delete & iconSurface ;
		throw ImageException( "Image::LoadIcon : image " + filename
			+ " does not seem to have a palette as it should." ) ;
						
	}
	
	// Use upper left pixel index as color key :
	iconSurface.setColorKey( Surface::ColorkeyBlit, 
			* ( (Pixels::ColorElement *) iconSurface.getPixels() ) ) ;

	ColorElement * pixels = (ColorElement *) iconSurface.getPixels() ;
	
#if OSDL_DEBUG_IMAGE

	LogPlug::debug( "Image::LoadIcon : index of upper left pixel is "
		+ Ceylan::toString( static_cast<unsigned short>( *pixels ) ) ) ;
	
	LogPlug::debug( "Image::LoadIcon : transparent pixel is : "
		+ Pixels::toString(
			iconSurface.getPixelFormat().palette->colors[*pixels] ) ) ;


	LogPlug::debug( 
		"Creating image mask (one bit per image pixel, rounded up)" ) ;
		
#endif // OSDL_DEBUG_IMAGE
		
	int maskLen = ( iconSurface.getWidth() * iconSurface.getHeight() + 7 ) / 8 ;
	ColorElement * constructedMask = new ColorElement[ maskLen ] ;
	
	if ( constructedMask == 0 ) 
	{
		delete & iconSurface ;
		throw ImageException( "Image::LoadIcon : not enough memory." ) ;
	}
	
	::memset( constructedMask, 0, maskLen ) ;
	
	int paletteIndex ;
	int maskIndex ;
	
	
	for ( Coordinate i = 0; i < iconSurface.getHeight(); i++ )
	{
	
		//LogPlug::debug( "Scanning line [ " + Ceylan::toString( i ) + "]" ) ;
		 
        for ( Coordinate j = 0; j < iconSurface.getWidth(); j++ ) 
		{
		
			/*
			 *
			 
			LogPlug::debug( "Scanning pixel [ " 
				+ Ceylan::toString( j ) + " ]" ) ;
				
			 *
			 */
				
            paletteIndex = i * iconSurface.getPitch() + j ;
            maskIndex    = i * iconSurface.getWidth() + j ;
			
			/*
			 * Write the corresponding bit to 1 when current pixel is 
			 * not the color key :
			 *
			 */
            if ( pixels[ paletteIndex ] != *pixels )
                constructedMask[ maskIndex >>3 ] |= 
					1 << ( 7 - ( maskIndex & 7) ) ;
        }
		
	}	

#if OSDL_DEBUG_IMAGE	
	LogPlug::debug( "Icon scanned." ) ;
#endif // OSDL_DEBUG_IMAGE
	
	*mask = constructedMask ;
	
#if OSDL_DEBUG_IMAGE
	LogPlug::debug( "Icon fully loaded." ) ;
#endif // OSDL_DEBUG_IMAGE
	
	return iconSurface ;
	
}



void Image::Load( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

#if OSDL_DEBUG_IMAGE	
	LogPlug::debug( "Image::Load : loading image from " + filename ) ;
#endif // OSDL_DEBUG_IMAGE
	
	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load JPG file " + filename 
			+ " : file not found." ) ;
			
	SDL_Surface * image = IMG_Load( filename.c_str() ) ; 	
		
	if ( image == 0 ) 
		throw ImageException( "Unable to load image stored in " + filename 
			+ " thanks to IMG_Load : "
			+ string( IMG_GetError() ) ) ;   
			
#if OSDL_DEBUG_IMAGE	
	LogPlug::debug( "Image loaded." ) ;
#endif // OSDL_DEBUG_IMAGE
	
	/*
	 * If conversion to display format is needed, substitute the 
	 * converted image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{
	
		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::Load called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;
				
		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is 
		 * to be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
#if OSDL_DEBUG_IMAGE	
			LogPlug::debug( 
				"Converting image to display format, with alpha." ) ;
#endif // OSDL_DEBUG_IMAGE

			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
#if OSDL_DEBUG_IMAGE	
			LogPlug::debug( "Converting image to display format, no alpha." ) ;
#endif // OSDL_DEBUG_IMAGE

			formattedImage = SDL_DisplayFormat( image ) ;	

		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
		if ( image == 0 )
			throw ImageException( 
				"Unable to convert to display the image loaded from " 
				+ filename + "." ) ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
	
		// We should blit the loaded image in already existing internal surface.
#if OSDL_DEBUG_IMAGE	
		LogPlug::debug( "Blitting loaded image to already existing surface." ) ;
#endif // OSDL_DEBUG_IMAGE
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::Load : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::Load : trying to blit an image "
				"whereas target surface is locked" ) ;
				
#endif // OSDL_DEBUG
		
		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( "Image::Load : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::Load : video memory was lost during blit." ) ;
	
		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{
	
#if OSDL_DEBUG_IMAGE	
		LogPlug::debug( "Replacing already existing image by loaded one." ) ; 
#endif // OSDL_DEBUG
		
		/*
		 * Simply replace the former internal surface by this new one.
		 * The former surface gets automatically deallocated by setSDLSurface.
		 *
		 * The target surface is, from now on, the new owner of the buffer
		 * pointed to by the image pointer.
		 *
		 */

		targetSurface.setSDLSurface( * image ) ;
		
	}
	
	
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change anything for that, and replacement triggers
	 * deallocation
	 *  - any used temporary surface got deallocated (it is the case 
	 * for display format conversion and for blitted image)
	 *
	 */	 
	
#if OSDL_DEBUG_IMAGE	
	LogPlug::debug( "Image loaded." ) ;
#endif // OSDL_DEBUG
	
}
	


void Image::LoadJPG( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load JPG file " 
			+ filename + " : file not found." ) ;
			
	SDL_Surface * image ;
	
	image = IMG_LoadTyped_RW( SDL_RWFromFile( filename.c_str(), "rb" ), 
		1, const_cast<char *>( JPGTag.c_str() ) ) ;
					
	if ( image == 0 ) 
		throw ImageException( "Unable to load JPG image stored in " + filename 
			+ " thanks to IMG_Load_RW : "
			+ string( IMG_GetError() ) ) ;   
	
	
	/*
	 * If conversion to display format is needed, substitute the 
	 * converted image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{

		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::LoadJPG called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;
	
		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is to 
		 * be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
			formattedImage = SDL_DisplayFormat( image ) ;	
		
		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
	
		// We should blit the loaded image in already existing internal surface.
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::LoadJPG : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::LoadJPG : trying to blit an image "
				"whereas target surface is locked" ) ;
				
#endif // OSDL_DEBUG
		

		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( "Image::LoadJPG : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::LoadJPG : video memory was lost during blit." ) ;
	

		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{
	
		/*
		 * Simply replace the former internal surface by this new one.
		 * The former surface gets automatically deallocated by setSDLSurface.
		 * The target surface is, from now on, the new owner of the 
		 * buffer pointed to by the image pointer.
		 *
		 */

		targetSurface.setSDLSurface( * image ) ;
	}
	
	
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change anything for that, and replacement triggers
	 * deallocation
	 *  - any used temporary surface got deallocated (it is the case 
	 * for display format conversion and for blitted image)
	 *
	 */	 
	
}
	
	
	
void Image::LoadPNG( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load PNG file " 
			+ filename + " : file not found." ) ;
			
	SDL_Surface * image ;
		
	image = IMG_LoadTyped_RW( SDL_RWFromFile( filename.c_str(), "rb" ), 
		1, const_cast<char *>( PNGTag.c_str() ) ) ;
	
	if ( image == 0 ) 
		throw ImageException( "Unable to load PNG image stored in " 
			+ filename + " thanks to IMG_Load_RW : "
			+ string( IMG_GetError() ) ) ;   
	
	
	/*
	 * If conversion to display format is needed, substitute the 
	 * converted image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{

		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::LoadPNG called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;
	
		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is 
		 * to be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
			formattedImage = SDL_DisplayFormat( image ) ;	
		
		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
	
		// We should blit the loaded image in already existing internal surface.
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::LoadPNG : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::LoadPNG : trying to blit an image "
				"whereas target surface is locked" ) ;
				
#endif // OSDL_DEBUG
		
		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( "Image::LoadPNG : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::LoadPNG : video memory was lost during blit." ) ;

		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{
	
		/*
		 * Simply replace the former internal surface by this new one.
		 * The former surface gets automatically deallocated by setSDLSurface.
		 * The target surface is, from now on, the new owner of the 
		 * buffer pointed to by the image pointer.
		 *
		 */
		 
		targetSurface.setSDLSurface( * image ) ;
		
	}
	
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change
	 * anything for that, and replacement triggers deallocation
	 *  - any used temporary surface got deallocated (it is the case 
	 * for display format conversion and for blitted image)
	 *
	 */	
	
}
	
	
void Image::LoadBMP( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load BMP file " 
			+ filename + " : file not found." ) ;
			
	SDL_Surface * image ;
	
	image = IMG_LoadTyped_RW( SDL_RWFromFile( filename.c_str(), "rb" ), 
		1, const_cast<char *>( BMPTag.c_str() ) ) ;
			
	if ( image == 0 ) 
		throw ImageException( "Unable to load BMP image stored in " 
			+ filename 
			+ " thanks to IMG_Load_RW : "
			+ string( IMG_GetError() ) ) ;   
	
	
	/*
	 * If conversion to display format is needed, substitute the 
	 * converted image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{

		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::LoadBMP called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;
	
		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is 
		 * to be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
			formattedImage = SDL_DisplayFormat( image ) ;	
		
		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
	
		// We should blit the loaded image in already existing internal surface.
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::LoadBMP : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::LoadBMP : trying to blit an image "
				"whereas target surface is locked" ) ;
				
#endif // OSDL_DEBUG
		
		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( "Image::LoadBMP : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::LoadBMP : video memory was lost during blit." ) ;

		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{

		/*
		 * Simply replace the former internal surface by this new one.
		 * The former surface gets automatically deallocated by setSDLSurface.
		 *
		 * The target surface is, from now on, the new owner of the 
		 * buffer pointed to by the image pointer.
		 *
		 */
		 
		targetSurface.setSDLSurface( * image ) ;
	}
	
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change
	 * anything for that, and replacement triggers deallocation
	 *  - any used temporary surface got deallocated (it is the case 
	 * for display format conversion and for blitted image)
	 *
	 */		 
	
}
		
	
	
void Image::LoadGIF( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load GIF file " 
			+ filename + " : file not found." ) ;
			
	SDL_Surface * image ;
	
	image = IMG_LoadTyped_RW( SDL_RWFromFile( filename.c_str(), "rb" ), 
		1, const_cast<char *>( GIFTag.c_str() ) ) ;
	
	if ( image == 0 ) 
		throw ImageException( "Unable to load GIF image stored in " + filename 
			+ " thanks to IMG_Load_RW : "
			+ string( IMG_GetError() ) ) ;   
	
	
	/*
	 * If conversion to display format is needed, substitute the 
	 * converted image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{
	
		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::LoadGIF called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;

		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is to 
		 * be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
			formattedImage = SDL_DisplayFormat( image ) ;	
		
		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
		// We should blit the loaded image in already existing internal surface.
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::LoadGIF : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::LoadGIF : trying to blit an image "
				"whereas target surface is locked" ) ;
				
#endif // OSDL_DEBUG
		
		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( "Image::LoadGIF : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::LoadGIF : video memory was lost during blit." ) ;

		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{

		/*
		 * Simply replace the former internal surface by this new one.
		 * The former surface gets automatically deallocated by setSDLSurface.
		 *
		 * The target surface is, from now on, the new owner of the buffer
		 * pointed to by the image pointer.
		 *
		 */
		 
		targetSurface.setSDLSurface( * image ) ;
	}
	
	
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change anything for that, and replacement triggers
	 * deallocation
	 *  - any used temporary surface got deallocated (it is the case for 
	 * display format conversion and for blitted image)
	 *
	 */	 
	
}
	
	
void Image::LoadLBM( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load LBM file " 
			+ filename + " : file not found." ) ;
			
	SDL_Surface * image ;
		
	image = IMG_LoadTyped_RW( SDL_RWFromFile( filename.c_str(), "rb" ), 
		1, const_cast<char *>( LBMTag.c_str() ) ) ;
			
	if ( image == 0 ) 
		throw ImageException( "Unable to load LBM image stored in " + filename 
			+ " thanks to IMG_Load_RW : "
			+ string( IMG_GetError() ) ) ;   
	
	
	/*
	 * If conversion to display format is needed, substitute the converted 
	 * image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{
	
		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::LoadLBM called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;

		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is 
		 * to be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
			formattedImage = SDL_DisplayFormat( image ) ;	
		
		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
		// We should blit the loaded image in already existing internal surface.
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::LoadLBM : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::LoadLBM : trying to blit an image "
				"whereas target surface is locked" ) ;
				
#endif // OSDL_DEBUG
		
		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( "Image::LoadLBM : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::LoadLBM : video memory was lost during blit." ) ;

		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{
	
		/*
		 * Simply replace the former internal surface by this new one.
		 *
		 * The former surface gets automatically deallocated by setSDLSurface.
		 *
		 * The target surface is, from now on, the new owner of the buffer
		 * pointed to by the image pointer.
		 *
		 */
		 
		targetSurface.setSDLSurface( * image ) ;
	}
	
		 
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change anything for that, and replacement triggers
	 * deallocation
	 *  - any used temporary surface got deallocated (it is the case for 
	 * display format conversion and for blitted image)
	 *
	 */	 
	 
}
	
	
	
void Image::LoadPCX( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load PCX file " 
			+ filename + " : file not found." ) ;
			
	SDL_Surface * image ;
	  
	image = IMG_LoadTyped_RW( SDL_RWFromFile( filename.c_str(), "rb" ), 
		1, const_cast<char *>( PCXTag.c_str() ) ) ;
			
	if ( image == 0 ) 
		throw ImageException( "Unable to load PCX image stored in " + filename 
			+ " thanks to IMG_Load_RW : "
			+ string( IMG_GetError() ) ) ;   
	
	
	/*
	 * If conversion to display format is needed, substitute the converted 
	 * image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{
	

		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::LoadPCX called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;

		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is 
		 * to be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
			formattedImage = SDL_DisplayFormat( image ) ;	
		
		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
		// We should blit the loaded image in already existing internal surface.
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::LoadPCX : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::LoadPCX : trying to blit an image "
				"whereas target surface is locked" ) ;
				
#endif // OSDL_DEBUG
		
		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( "Image::LoadPCX : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::LoadPCX : video memory was lost during blit." ) ;

		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{
	
		/*
		 * Simply replace the former internal surface by this new one.
		 *
		 * The former surface gets automatically deallocated by setSDLSurface.
		 *
		 * The target surface is, from now on, the new owner of the buffer
		 * pointed to by the image pointer.
		 *
		 */
		 
		targetSurface.setSDLSurface( * image ) ;
	}
	
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change anything for that, and replacement triggers
	 * deallocation
	 *  - any used temporary surface got deallocated (it is the case for 
	 * display format conversion and for blitted image)
	 *
	 */	 
	 
	
}
	
	
void Image::LoadPNM( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load PNM file " 
			+ filename + " : file not found." ) ;
			
	SDL_Surface * image ;
		
	image = IMG_LoadTyped_RW( SDL_RWFromFile( filename.c_str(), "rb" ), 
		1, const_cast<char *>( PNMTag.c_str() ) ) ;
	
	if ( image == 0 ) 
		throw ImageException( "Unable to load PNM image stored in " + filename 
			+ " thanks to IMG_Load_RW : "
			+ string( IMG_GetError() ) ) ;   
	
	
	/*
	 * If conversion to display format is needed, substitute the converted 
	 * image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{

		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::LoadPNM called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;
	
		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is 
		 * to be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
			formattedImage = SDL_DisplayFormat( image ) ;	
		
		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
		// We should blit the loaded image in already existing internal surface.
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::LoadPNM : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::LoadPNM : trying to blit an image "
				"whereas target surface is locked" ) ;
#endif // OSDL_DEBUG
		
		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( "Image::LoadPNM : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::LoadPNM : video memory was lost during blit." ) ;

		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{

		/*
		 * Simply replace the former internal surface by this new one.
		 *
		 * The former surface gets automatically deallocated by setSDLSurface.
		 *
		 * The target surface is, from now on, the new owner of the buffer
		 * pointed to by the image pointer.
		 *
		 */
		 
		targetSurface.setSDLSurface( * image ) ;
	}
	
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change anything for that, and replacement triggers
	 * deallocation
	 *  - any used temporary surface got deallocated (it is the case for 
	 * display format conversion and for blitted image)
	 *
	 */	 
 	
}
	
	
void Image::LoadTGA( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load TGA file " 
			+ filename + " : file not found." ) ;
			
	SDL_Surface * image ;
		
	image = IMG_LoadTyped_RW( SDL_RWFromFile( filename.c_str(), "rb" ), 
		1, const_cast<char *>( TGATag.c_str() ) ) ;
		
	if ( image == 0 ) 
		throw ImageException( "Unable to load TGA image stored in " + filename 
			+ " thanks to IMG_Load_RW : "
			+ string( IMG_GetError() ) ) ;   
	
	
	/*
	 * If conversion to display format is needed, substitute the converted 
	 * image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{

		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::LoadTGA called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;
	
		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is 
		 * to be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
			formattedImage = SDL_DisplayFormat( image ) ;	
		
		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
		// We should blit the loaded image in already existing internal surface.
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::LoadTGA : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::LoadTGA : trying to blit an image "
				"whereas target surface is locked" ) ;
				
#endif // OSDL_DEBUG
		
		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( "Image::LoadTGA : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::LoadTGA : video memory was lost during blit." ) ;

		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{

		/*
		 * Simply replace the former internal surface by this new one.
		 *
		 * The former surface gets automatically deallocated by setSDLSurface.
		 *
		 * The target surface is, from now on, the new owner of the buffer
		 * pointed to by the image pointer.
		 *
		 */

		targetSurface.setSDLSurface( * image ) ;
	}
	
	
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change anything for that, and replacement triggers
	 * deallocation
	 *  - any used temporary surface got deallocated (it is the case for 
	 * display format conversion and for blitted image)
	 *
	 */	 
}
	


void Image::LoadXPM( Surface & targetSurface, const std::string & filename, 
		bool blitOnly, bool convertToDisplay, bool convertWithAlpha ) 
	throw( ImageException )
{

	if ( ! File::Exists( filename ) )
		throw ImageException( "Unable to load XPM file " 
			+ filename + " : file not found." ) ;
			
	SDL_Surface * image ;
		
	image = IMG_LoadTyped_RW( SDL_RWFromFile( filename.c_str(), "rb" ), 
		1, const_cast<char *>( XPMTag.c_str() ) ) ;
		
	if ( image == 0 ) 
		throw ImageException( "Unable to load XPM image stored in " + filename 
			+ " thanks to IMG_Load_RW : "
			+ string( IMG_GetError() ) ) ;   
	
	
	/*
	 * If conversion to display format is needed, substitute the converted 
	 * image to the loaded one :
	 *
	 */
	
	if ( convertToDisplay )
	{
	
		if ( ! VideoModule::IsDisplayInitialized() )
			throw ImageException( 
				"Image::LoadXPM called with request to convert surface "
				"to display, whereas display not initialized "
				"(VideoModule::setMode never called)." ) ;

		SDL_Surface * formattedImage ;
		
		/*
		 * SDL_DisplayFormat* copies the surface, which therefore is 
		 * to be deallocated :
		 *
		 */
		
		if ( convertWithAlpha )
		{
		
			formattedImage = SDL_DisplayFormatAlpha( image ) ;	
			
		}
		else
		{
		
			formattedImage = SDL_DisplayFormat( image ) ;	
		
		}	
		
		SDL_FreeSurface( image ) ;
		
		image = formattedImage ;
		
	}
		
	/*
	 * Now we have our image surface, we can either blit it or have 
	 * it replace the former one :
	 *
	 */
			
	if ( blitOnly )
	{
		// We should blit the loaded image in already existing internal surface.
		
#if OSDL_DEBUG

		if ( ! targetSurface.isInternalSurfaceAvailable() )
			throw ImageException( "Image::LoadXPM : trying to blit image "
				"whereas target SDL_surface pointer is null" ) ;
				
#endif // OSDL_DEBUG
		
			
#if OSDL_DEBUG
		
		// The target surface should not be locked :	

		if ( targetSurface.isLocked() )
			throw ImageException( "Image::LoadXPM : trying to blit an image "
				"whereas target surface is locked" ) ;
				
#endif // OSDL_DEBUG
		
		int result = SDL_BlitSurface( image, 0, 
			& targetSurface.getSDLSurface(), 0 ) ;
		
		if ( result == -1)
			throw ImageException( 
				"Image::LoadXPM : error in blit : " 
				+ Utils::getBackendLastError() ) ;
	
		if ( result == -2 )
			throw ImageException( 
				"Image::LoadXPM : video memory was lost during blit." ) ;

		/*
		 * In this case, the loaded image has been used and therefore 
		 * is not needed any more :
		 *
		 */
	 
	 	SDL_FreeSurface( image ) ;		
		
	}
	else
	{

		/*
		 * Simply replace the former internal surface by this new one.
		 *
		 * The former surface gets automatically deallocated by setSDLSurface.
		 *
		 * The target surface is, from now on, the new owner of the buffer
		 * pointed to by the image pointer.
		 *
		 */
		 
		targetSurface.setSDLSurface( * image ) ;
	}
	
	
	/*
	 * In all cases :
	 *	- no memory leak should occur with the internal surface, since 
	 * blit does not change anything for that, and replacement triggers
	 * deallocation
	 *  - any used temporary surface got deallocated (it is the case for 
	 * display format conversion and for blitted image)
	 *
	 */	 
	 
}	



void Image::SavePNG( Surface & targetSurface, const std::string & filename, 
	bool overwrite ) throw( ImageException )
{

	// See png.h and http://www.libpng.org/pub/png/libpng.html for more details.
	
	if ( ! overwrite && Ceylan::System::File::Exists( filename ) )
		throw TwoDimensional::ImageException( 
			"Surface::savePNG : target file '" + filename 
			+ "' already exists, and overwrite mode is off." ) ;

	png_structp png_ptr ;
	png_infop info_ptr ;

	png_ptr = ::png_create_write_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 ) ;

	if ( png_ptr == 0 )
	{  
		::png_destroy_write_struct( & png_ptr, (png_infopp) 0 ) ;
		throw TwoDimensional::ImageException( 
			"Surface::savePNG : unable to save image '"
			+ filename + "' (step 1)" ) ;
	}
	
	info_ptr = ::png_create_info_struct( png_ptr ) ;
	
	if ( info_ptr == 0 )
	{  
		::png_destroy_write_struct( & png_ptr, (png_infopp) 0 ) ;
		throw TwoDimensional::ImageException( 
			"Surface::savePNG : unable to save image '"
				+ filename + "' (step 2)" ) ;
	}


    if ( ::setjmp( png_jmpbuf( png_ptr ) ) )
	{  
		::png_destroy_write_struct( & png_ptr, (png_infopp) 0 ) ;
		throw TwoDimensional::ImageException( 
			"Surface::savePNG : unable to save image '"
			+ filename + "' (step 3)" ) ;
	}
	
	// Let's create a file with C-style :
	
	/*
	 * Something as :
	 
	std::ifstream outputFile = open( filename.c_str(), ifstream::out ) ;

	 * could not work since functions like png_init_io wants a FILE *, 
	 * except maybe with a clumsy ifstream::rdbuf.
	 *
	 * ifstream destructor would automatically close the file on any 
	 * exit scheme.
	 *
	 */

	FILE * outputFile = ::fopen( filename.c_str(), "wb" ) ;
	
	if ( outputFile == 0 )
	{  
		::png_destroy_write_struct( & png_ptr, (png_infopp) 0 ) ;
		throw TwoDimensional::ImageException( 
			"Surface::savePNG : unable to save image '"
			+ filename + "' (step 4) : " + Ceylan::System::explainError() ) ;
	}
	
   	::png_init_io( png_ptr, outputFile ) ;

    info_ptr->width  = targetSurface.getWidth() ;
    info_ptr->height = targetSurface.getHeight() ;
	
	// Superfluity :
	//info_ptr->rowbytes = _width ;
	
	
	// The choice might be not as generic as wished :
    info_ptr->bit_depth = 8 ;
	
	/* 
	 * At least two other color type should be managed :
	 *	- PNG_COLOR_TYPE_GRAY
	 *  - PNG_COLOR_TYPE_PALETTE
	 *
	 * No alpha coordinate seems to be managed.
	 *
	 */	   
    info_ptr->color_type = PNG_COLOR_TYPE_RGB ;


	/* Thanks to Murlock, this method could be a little more generic :
	
    switch( type_img )
    {
	
     	case TYPE_RGB:
     		info_ptr->rowbytes     *= 3 ;
		    info_ptr->sig_bit.red   = 8 ;
		    info_ptr->sig_bit.green = 8 ;
		    info_ptr->sig_bit.blue  = 8 ;
     		info_ptr->color_type    = PNG_COLOR_TYPE_RGB ;
     		break ;
			
     	case TYPE__GRAY:
			info_ptr->sig_bit.gray = 8 ;
     		info_ptr->color_type   = PNG_COLOR_TYPE_GRAY ;
     		break ;
			
     	case TYPE_PALETTE:
			info_ptr->valid       = PNG_INFO_PLTE ;    	
			info_ptr->num_palette = nb_col;
     		info_ptr->color_type  = PNG_COLOR_TYPE_PALETTE;
     		info_ptr->palette     =(png_color*)
				malloc(nb_col*sizeof(png_color));
			
			// Use OSDLPalette instead :
			
     		for ( unsigned int i=0; i < nb_col; i++ )
     		{
     			info_ptr->palette[i].red   = palette[i].r ;
     			info_ptr->palette[i].green = palette[i].g ;
     			info_ptr->palette[i].blue  = palette[i].b ;
     		}
     		break ;
		
		default:
			throw TwoDimensional::ImageException( 
				"Surface::savePNG : unexpected image type." ) ;
			break ;	
     }
	 
	*/
	 	
	// Could be PNG_INTERLACE_NONE or PNG_INTERLACE_ADAM7 :
    info_ptr->interlace_type = PNG_INTERLACE_ADAM7 ;
	
	// No palette used :
	info_ptr->num_palette = 0 ;
	info_ptr->palette     = 0 ;
	
	info_ptr->valid = 0 ;


	::png_write_info( png_ptr, info_ptr ) ;


	// Now write the pixels, one by one :
	targetSurface.lock() ;
	
	unsigned char ** png_rows = new unsigned char * 
		[ targetSurface.getHeight() ] ;

    for ( Coordinate y = 0; y < targetSurface.getHeight(); y++ )
	{
	
    	png_rows[ y ] = new unsigned char[ 3 * targetSurface.getWidth() ] ;

		ColorDefinition readDef ;
		
    	for ( Coordinate x = 0; x < targetSurface.getWidth(); x++ )
		{
		
			readDef = targetSurface.getColorDefinitionAt( x, y ) ;
			
			/*
    		SDL_GetRGB( targetSurface.getPixel( x, y ), 
				& targetSurface.getPixelFormat(), & r, & g, & b ) ;
			 */
    		png_rows[ y ][ x * 3 + 0 ] = readDef.r ;
    		png_rows[ y ][ x * 3 + 1 ] = readDef.g ;
    		png_rows[ y ][ x * 3 + 2 ] = readDef.b ;
		}
	}

	targetSurface.unlock() ;
	  
	png_write_image( png_ptr, png_rows ) ;

	for ( Coordinate y = 0; y < targetSurface.getHeight(); y++ )
		delete [] png_rows[ y ] ;
	
	delete [] png_rows ;
	  
	::png_write_end( png_ptr, 0 ) ;

	::png_destroy_write_struct( & png_ptr, & info_ptr ) ;
	
	::fclose( outputFile ) ;


}



void Image::SaveBMP( Surface & targetSurface, const std::string & filename, 
	bool overwrite ) throw( ImageException )
{

	if ( ! overwrite && Ceylan::System::File::Exists( filename ) )
		throw TwoDimensional::ImageException( 
			"Surface::saveBMP : target file '" + filename 
			+ "' already exists, and overwrite mode is off." ) ;
			
	if ( SDL_SaveBMP( & targetSurface.getSDLSurface(), filename.c_str() ) != 0 )
		throw TwoDimensional::ImageException( 
			"Surface::saveBMP : error while saving BMP file '" 
			+ filename + "' : "	+ OSDL::Utils::getBackendLastError() ) ;

}



const string Image::toString( Ceylan::VerbosityLevels level ) const throw()
{
	
	if ( _filename.empty() )
		return "Image object not linked to a file" ;
		
	return "Image object linked with file " + _filename ;
	
}


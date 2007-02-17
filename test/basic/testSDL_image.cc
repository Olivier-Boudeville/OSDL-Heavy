#include "OSDL.h"     // just for basic primitives such as getBackendLastError
using namespace OSDL ;

using namespace Ceylan::Log ;

#include "SDL_image.h"


#include <string>
using std::string ;


#define SDL_SUCCESS  0
#define SDL_ERROR   -1


/*
 * This module is made to test just plain SDL_image : nothing here should 
 * depend on OSDL !
 *
 */


/**
 * Displays an image (type auto-detected) stored in file fileName into
 * SDL_Surface targetSurface.
 * targetSurface should be screen surface (obtained with SDL_SetVideoMode)
 * otherwise blitting is useless.
 *
 * @see adapted from http://sdldoc.csn.ul.ie/guidevideo.php
 * @see adapted from http://jcatki.no-ip.org/SDL_image/SDL_image.html#SEC6
 *
 */
void displayImage( const string & fileName, SDL_Surface * targetSurface )
{

    // Loads the JPEG file into a surface.	

	SDL_Surface * mySurface ;
	
	mySurface = IMG_Load( fileName.c_str() ) ;
	
	if ( mySurface == 0 )
	{
		LogPlug::fatal( "Unable to load image from file " + fileName
			+ " : " + Ceylan::toString( IMG_GetError() ) ) ;
		throw Ceylan::TestException( "Unable to load image from file "
			+ fileName + " : " + Ceylan::toString( IMG_GetError() ) ) ;	 
	}

    /*
     * Palettized targetSurface modes will have a default palette (a standard
     * 8*8*4 colour cube), but if the image is palettized as well we can
     * use that palette for a nicer colour matching.
	 *
	 
    if ( mySurface->format->palette && targetSurface->format->palette ) 
    	SDL_SetColors( targetSurface, mySurface->format->palette->colors, 0,
			mySurface->format->palette->ncolors ) ;
			
     */
  
    // Blit onto the targetSurface surface;
	
    if ( SDL_BlitSurface( mySurface, 0, targetSurface, 0 ) < 0 ) 
	{
		LogPlug::fatal( "BlitSurface error: " 
			+ Utils::getBackendLastError() ) ;
		throw Ceylan::TestException( "BlitSurface error: " 
			+ Utils::getBackendLastError() ) ;
    }
	
	LogPlug::info( "Image width is " + Ceylan::toString( mySurface->w )
		+ ", height is " + Ceylan::toString( mySurface->h ) );
	
    SDL_UpdateRect( targetSurface, 0, 0, mySurface->w, mySurface->h ) ;

    // Free the allocated BMP surface.
    SDL_FreeSurface( mySurface ) ;
	
}



int main( int argc, char * argv[] )
{


	LogHolder myLog( argc, argv ) ;
	
	try 
	{
	
		LogPlug::info( "Testing basic SDL_image" ) ;
		
		LogPlug::info( "Starting SDL (base, audio and video)" ) ;

    	if ( SDL_Init( SDL_INIT_VIDEO ) != SDL_SUCCESS ) 
		{
			LogPlug::fatal( "Unable to initialize SDL : " 
				+ Utils::getBackendLastError() ) ;
 			return Ceylan::ExitFailure ;
		}
		
		LogPlug::info( "SDL successfully initialized" ) ;
	
		int xrange   = 640 ;
		int yrange   = 480 ;
		
		// 0 means current bpp :
		int askedBpp = 0 ;
	
		LogPlug::info( "Setting " 
			+ Ceylan::toString( xrange ) + "x"
			+ Ceylan::toString( yrange ) + " with " 
			+ Ceylan::toString( askedBpp ) + " bits per pixel video mode." ) ;

    	SDL_Surface * screen = SDL_SetVideoMode( xrange, yrange, askedBpp,
			SDL_SWSURFACE ) ;

    	if ( screen == 0 ) 
		{
        	LogPlug::fatal( "Couldn't set " 
				+ Ceylan::toString( xrange ) + "x"
				+ Ceylan::toString( yrange ) + " with " 
				+ Ceylan::toString( askedBpp ) + " bits per pixel video mode : "
				+ Utils::getBackendLastError() ) ;
 			return Ceylan::ExitFailure ;
    	}
	
		int bpp = screen->format->BitsPerPixel ;
	
		LogPlug::info( "Color depth is " + Ceylan::toString( bpp ) 
			+ " bits per pixel" ) ;
		
		if ( askedBpp != bpp )
		{
			 LogPlug::info( "Color depth is " + Ceylan::toString( bpp ) 
			 	+ " bits per pixel instead of the asked " 
				+ Ceylan::toString( askedBpp ) + " bits per pixel." ) ;
		}


		Ceylan::System::FileLocator imageFinder ;
		
		// When run from executable directory :
		imageFinder.addPath( "../../src/doc/web/images" ) ;
	
		// When run from tests-results directory :
		imageFinder.addPath( "../src/doc/web/images" ) ;
	
		const string firstImageFile = imageFinder.find(
			"Soldier-heavy-purple-small.png" ) ;
		
		LogPlug::info( "Displaying a JPEG image from file " + firstImageFile ) ;
		
		displayImage( firstImageFile, screen ) ;

		Ceylan::display( "< Press any key on SDL window to stop >" ) ;
	
		SDL_Event event ; 
	
		do 
		{
	  		SDL_PollEvent( & event ) ;
		} 
		while ( event.type != SDL_KEYDOWN ) ;

	
		const string secondImageFile = imageFinder.find(
			"osdl-zoom-inverted.png" ) ;
		
		LogPlug::info( "Displaying a PNG image from file " + secondImageFile ) ;

		displayImage( secondImageFile, screen ) ;
			
		Ceylan::display( "< Press any key on SDL window to stop >" ) ;
	
		do 
		{
		
	  		SDL_PollEvent( & event ) ;
			
		} while ( event.type != SDL_KEYDOWN ) ;

    	LogPlug::info( "Stopping SDL" ) ;
    	SDL_Quit() ;
    	LogPlug::info( "SDL successfully stopped." ) ;

		LogPlug::info( "End of basic SDL_image test." ) ;
	
 
    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught : " 
			 + std::string( e.what() ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
	
        LogPlug::error( "Unknown exception caught" ) ;
       	return Ceylan::ExitFailure ;

    }

    return Ceylan::ExitSuccess ;

}


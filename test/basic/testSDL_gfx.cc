#include "OSDL.h"     // just for basic primitives such as getBackendLastError
using namespace OSDL ;


#include "Ceylan.h"
using namespace Ceylan::Log ;

#include "SDL_gfxPrimitives.h"


#include <string>
using std::string ;




#define SDL_SUCCESS  0
#define SDL_ERROR   -1


// This module is made to test just plain SDL_gfx : nothing here should depend on OSDL !


void displayString( const string & message, SDL_Surface * targetSurface, 
	Sint16 x, Sint16 y, 
	Uint8 r, Uint8 g, Uint8 b, Uint8 a ) 
{
	
	if ( stringRGBA( targetSurface, x, y, 
		const_cast<char *>( message.c_str() ), r, g, b, a ) != 0 )
	{
		LogPlug::error( "Error in displayString : stringRGBA." ) ;	
	}
	
}



int main( int argc, char * argv[] )
{

	LogHolder myLog( argc, argv ) ;
	
	try {
	
		LogPlug::info( "Testing basic SDL_gfx" ) ;
		
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
		
		// A null value signifies the current screen bpp shoud be used :
		int askedBpp = 0 ;
	
		LogPlug::info( "Setting " 
			+ Ceylan::toString( xrange ) + "x"
			+ Ceylan::toString( yrange ) + " with " 
			+ Ceylan::toString( askedBpp ) + " bits per pixel video mode." ) ;


    	SDL_Surface * screen = SDL_SetVideoMode( xrange, yrange, askedBpp, SDL_SWSURFACE ) ;

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
	
		LogPlug::info( "Color depth is " + Ceylan::toString( bpp ) + " bits per pixel" ) ;
		
		if ( askedBpp != bpp )
		{
			 LogPlug::info( "Color depth is " + Ceylan::toString( bpp ) + " bits per pixel"
			 	" instead of the asked " + Ceylan::toString( askedBpp ) + " bits per pixel." ) ;
		}
		
		LogPlug::info( "Drawing various graphical primitives." ) ;
		
		displayString( "OSDL rocks !", screen, 50, 50, 0, 255, 0, 127 ) ;
		displayString( "Ceylan rocks !", screen, 5, 5, 255, 255, 0, 255 ) ;
		trigonRGBA( screen, 100, 100, 310, 130, 200, 250, 23, 78, 33, 255 );
		circleRGBA( screen, 100, 100, 80, 23, 78, 33, 230 );
		boxRGBA( screen, 300, 300, 380, 400, 78, 33, 230, 230 ) ;
		ellipseRGBA( screen, 200, 200, 100, 50, 30, 30, 98, 255 ) ;
			
    	SDL_UpdateRect( screen, 0, 0, screen->w, screen->h ) ;

	
		Ceylan::display( "< Press any key on SDL window to stop >" ) ;
		
		SDL_Event event ; 
	
		do 
		{
	  		SDL_PollEvent( & event ) ;
		} while ( event.type != SDL_KEYDOWN ) ;

    	LogPlug::info( "Stopping SDL" ) ;
    	SDL_Quit() ;
    	LogPlug::info( "SDL successfully stopped." ) ;

		LogPlug::info( "End of basic SDL_gfx test." ) ;
	
 
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


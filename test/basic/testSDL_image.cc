/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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

    // Loads the image file into a surface.	

	SDL_Surface * mySurface ;
	
	mySurface = IMG_Load( fileName.c_str() ) ;
	
	if ( mySurface == 0 )
	{
		LogPlug::fatal( "Unable to load image from file '" + fileName
			+ "' : " + Ceylan::toString( IMG_GetError() ) ) ;
		throw Ceylan::TestException( "Unable to load image from file '"
			+ fileName + "' : " + Ceylan::toString( IMG_GetError() ) ) ;	 
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
	
    SDL_UpdateRect( targetSurface, 0, 0, targetSurface->w, targetSurface->h ) ;

    // Free the allocated BMP surface.
    SDL_FreeSurface( mySurface ) ;
	
}


void waiter( bool isBatch )
{

	SDL_Event event ; 
	
	if ( ! isBatch )
	{

		Ceylan::display( "< Press any key on SDL window to stop >" ) ;
	
		do 
		{
	
			// Avoid busy waits :
			SDL_WaitEvent( & event ) ;
		
		} while ( event.type != SDL_KEYDOWN ) ;
	}
	else
	{
		SDL_Delay( 1000 /* milliseconds */ ) ;
	}

}


int main( int argc, char * argv[] )
{


	LogHolder myLog( argc, argv ) ;
	
	try 
	{
	
		LogPlug::info( "Testing basic SDL_image" ) ;
		
		bool isBatch = false ;
		
		std::string executableName ;
		std::list<std::string> options ;
		
		Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;
		
		std::string token ;
		bool tokenEaten ;
		
		
		while ( ! options.empty() )
		{
		
			token = options.front() ;
			options.pop_front() ;

			tokenEaten = false ;
						
			if ( token == "--batch" )
			{
				LogPlug::info( "Batch mode selected" ) ;
				isBatch = true ;
				tokenEaten = true ;
			}
			
			if ( token == "--interactive" )
			{
				LogPlug::info( "Interactive mode selected" ) ;
				isBatch = false ;
				tokenEaten = true ;
			}
			
			
			if ( LogHolder::IsAKnownPlugOption( token ) )
			{
				// Ignores log-related (argument-less) options.
				tokenEaten = true ;
			}
			
			
			if ( ! tokenEaten )
			{
				throw Ceylan::CommandLineParseException( 
					"Unexpected command line argument : " + token ) ;
			}
		
		}


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
			"VikingWalkingInSnow.jpg" ) ;
		
		LogPlug::info( "Displaying a JPEG image from file " + firstImageFile ) ;
		
		displayImage( firstImageFile, screen ) ;
		waiter( isBatch ) ;
	
		const string secondImageFile = imageFinder.find(
			"osdl-zoom-inverted.png" ) ;
		
		LogPlug::info( "Displaying a PNG image from file " + secondImageFile ) ;

		displayImage( secondImageFile, screen ) ;
		waiter( isBatch ) ;
	
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


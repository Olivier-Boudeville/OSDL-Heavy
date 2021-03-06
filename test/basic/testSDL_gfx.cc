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


#include "OSDL.h"     // just for basic primitives such as getBackendLastError
using namespace OSDL ;


using namespace Ceylan::Log ;


// Disabled, as long as SDL_gfx build fails on 64-bit:
#define OSDL_BYPASS_SDL_GFX_TESTING 1

#ifndef OSDL_BYPASS_SDL_GFX_TESTING

// Not included by OSDL headers (only in implementations files):
#include "SDL_gfxPrimitives.h"

#endif // OSDL_BYPASS_SDL_GFX_TESTING



#include <string>
using std::string ;




#define SDL_SUCCESS  0
#define SDL_ERROR   -1


/*
 * This module is made to test just plain SDL_gfx: nothing here should depend on
 * OSDL, except basic helpers.
 *
 */



void displayString( const string & message, SDL_Surface * targetSurface,
  Sint16 x, Sint16 y,
  Uint8 r, Uint8 g, Uint8 b, Uint8 a )
{

#ifndef OSDL_BYPASS_SDL_GFX_TESTING
  if ( stringRGBA( targetSurface, x, y,
	  const_cast<char *>( message.c_str() ), r, g, b, a ) != 0 )
  {
	LogPlug::error( "Error in displayString: stringRGBA." ) ;
  }
#endif // OSDL_BYPASS_SDL_GFX_TESTING
}



int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;

	try
	{

	  LogPlug::info( "Testing basic SDL_gfx" ) ;

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
			"Unexpected command line argument: " + token ) ;
		}

	  }

	  LogPlug::info( "Starting SDL (base, audio and video)" ) ;

	  if ( SDL_Init( SDL_INIT_VIDEO ) != SDL_SUCCESS )
	  {

		LogPlug::fatal( "Unable to initialize SDL: "
		  + Utils::getBackendLastError() ) ;

		return Ceylan::ExitFailure ;
	  }

	  LogPlug::info( "SDL successfully initialized" ) ;

	  int xrange = 640 ;
	  int yrange = 480 ;

	  // A null value signifies the current screen bpp shoud be used:
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
		  + Ceylan::toString( askedBpp ) + " bits per pixel video mode: "
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

	  LogPlug::info( "Drawing various graphical primitives." ) ;

	  displayString( "OSDL rocks!", screen, 50, 50, 0, 255, 0, 127 ) ;
	  displayString( "Ceylan rocks!", screen, 5, 5, 255, 255, 0, 255 ) ;

#ifndef OSDL_BYPASS_SDL_GFX_TESTING

	  trigonRGBA( screen, 100, 100, 310, 130, 200, 250, 23, 78, 33, 255 );
	  circleRGBA( screen, 100, 100, 80, 23, 78, 33, 230 );
	  boxRGBA( screen, 300, 300, 380, 400, 78, 33, 230, 230 ) ;
	  ellipseRGBA( screen, 200, 200, 100, 50, 30, 30, 98, 255 ) ;

#endif // OSDL_BYPASS_SDL_GFX_TESTING

	  SDL_UpdateRect( screen, 0, 0, screen->w, screen->h ) ;


	  Ceylan::display( "< Press any key on SDL window to stop >" ) ;

	  SDL_Event event ;

	  if ( ! isBatch )
	  {

		do
		{

		  // Avoid busy waits:
		  SDL_WaitEvent( & event ) ;

		} while ( event.type != SDL_KEYDOWN ) ;
	  }
	  else
	  {
		SDL_Delay( 1000 /* milliseconds */ ) ;
	  }

	  LogPlug::info( "Stopping SDL" ) ;
	  SDL_Quit() ;
	  LogPlug::info( "SDL successfully stopped." ) ;

	  LogPlug::info( "End of basic SDL_gfx test." ) ;


	}

	catch ( const Ceylan::Exception & e )
	{

	  LogPlug::error( "Ceylan exception caught: "
		+ e.toString( Ceylan::high ) ) ;
	  return Ceylan::ExitFailure ;

	}

	catch ( const std::exception & e )
	{

	  LogPlug::error( "Standard exception caught: "
		+ std::string( e.what() ) ) ;
	  return Ceylan::ExitFailure ;

	}

	catch ( ... )
	{

	  LogPlug::error( "Unknown exception caught" ) ;
	  return Ceylan::ExitFailure ;

	}

  }

  // To deallocate helpers like Ceylan's filesystem manager for logs:
  OSDL::shutdown() ;

  return Ceylan::ExitSuccess ;

}

/*
 * Copyright (C) 2003-2011 Olivier Boudeville
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

#include <string>
using std::string ;




#define SDL_SUCCESS  0
#define SDL_ERROR   -1


/*
 * This module is made to test just plain SDL: nothing here should depend on
 * OSDL, apart basic helpers.
 *
 */


/**
 * Displays BMP image stored in file fileName into SDL_Surface targetSurface.
 *
 * targetSurface should be screen surface (obtained with SDL_SetVideoMode),
 * otherwise blitting is useless.
 *
 * @see adapted from http://sdldoc.csn.ul.ie/guidevideo.php
 *
 */
void display_bmp( const string & fileName, SDL_Surface * targetSurface )
{

  SDL_Surface * image ;

  // Loads the BMP file into a surface.
  image = SDL_LoadBMP( fileName.c_str() ) ;

  if ( image == 0 )
  {

	LogPlug::fatal( "Unable to load '" + fileName + "':"
	  + Utils::getBackendLastError() ) ;

	throw Ceylan::TestException( "Unable to load '" + fileName + "': "
	  + Utils::getBackendLastError() ) ;
  }


  /*
   * Palettized targetSurface modes will have a default palette (a standard
   * 8*8*4 colour cube), but if the image is palettized as well we can use
   * that palette for a nicer colour matching.
   */
  if ( image->format->palette && targetSurface->format->palette )
	SDL_SetColors( targetSurface, image->format->palette->colors, 0,
	  image->format->palette->ncolors ) ;

  // Blit onto the targetSurface surface:
  if( SDL_BlitSurface( image, NULL, targetSurface, NULL ) < 0 )
  {

	LogPlug::fatal( "BlitSurface error: "
	  + Utils::getBackendLastError() ) ;

	throw Ceylan::TestException( "BlitSurface error: "
	  + Utils::getBackendLastError() ) ;
  }

  SDL_UpdateRect( targetSurface, 0, 0, image->w, image->h ) ;

  // Free the allocated BMP surface.
  SDL_FreeSurface( image ) ;

}



/**
 * Sets the pixel at (x,y) to the given value. Generic (not optimized).
 *
 * @note the surface must be locked before calling this!
 *
 * @see adapted from http://sdldoc.csn.ul.ie/guidevideo.php
 * @see getPixel
 *
 */
void putPixel( SDL_Surface * targetSurface, int x, int y, Uint32 pixel )
{
  int bpp = targetSurface->format->BytesPerPixel ;

  // Here p is the address to the pixel we want to set.
  Uint8 * p = ( Uint8 * ) targetSurface->pixels + y * targetSurface->pitch
	+ x * bpp ;

  switch( bpp )
  {

  case 1:

	*p = pixel ;
	break ;

  case 2:

	*( Uint16 * ) p = pixel ;
	break ;

  case 3:

	if( SDL_BYTEORDER == SDL_BIG_ENDIAN )
	{
	  p[0] = ( pixel >> 16 ) & 0xff ;
	  p[1] = ( pixel >>  8 ) & 0xff ;
	  p[2] = pixel & 0xff ;
	}
	else
	{
	  p[0] = pixel & 0xff ;
	  p[1] = ( pixel >> 8  ) & 0xff ;
	  p[2] = ( pixel >> 16 ) & 0xff ;
	}
	break ;

  case 4:

	*( Uint32 * ) p = pixel ;
	break ;
  }
}


/**
 * Returns the pixel value at (x,y). Generic (not optimized)
 *
 * @note the surface must be locked before calling this!
 *
 * @see adapted from http://sdldoc.csn.ul.ie/guidevideo.php
 *
 */
Uint32 getPixel( SDL_Surface * fromSurface, int x, int y )
{
  int bpp = fromSurface->format->BytesPerPixel ;

  //Here p is the address to the pixel we want to retrieve.
  Uint8 * p = (Uint8 *) fromSurface->pixels + y * fromSurface->pitch
	+ x * bpp ;

  switch( bpp )
  {

  case 1:
	return * p ;

  case 2:
	return *( Uint16 * ) p ;

  case 3:
	if( SDL_BYTEORDER == SDL_BIG_ENDIAN )
	  return p[0] << 16 | p[1] << 8 | p[2] ;
	else
	  return p[0] | p[1] << 8 | p[2] << 16 ;

  case 4:
	return *( Uint32 * )p;

  default:
	LogPlug::fatal( "Abnormal bit per pixel detected in getPixel." ) ;
	throw Ceylan::TestException(
	  "Abnormal bit per pixel detected in getPixel." ) ;
	break ;
  }

}




/**
 * Extracting color components from a 32-bit color value.
 *
 * Format is [ Red; Green; Blue; Alpha ].
 *
 * Color components are stored in pixel, according to fromSurface's format.
 *
 * @see adapted from http://sdldoc.csn.ul.ie/guidevideo.php
 *
 */
const string RGBtoString( Uint32 pixel, const SDL_Surface * fromSurface )
{

  LogPlug::info( "Decoding pixel value " + Ceylan::toString( pixel ) + "." ) ;

  SDL_PixelFormat * fmt ;
  Uint32 temp ;
  Uint8 red, green, blue, alpha ;

  fmt = fromSurface->format ;

  // Get Red component.
  temp = pixel & fmt->Rmask ;  /* Isolate red component */
  temp = temp >> fmt->Rshift ; /* Shift it down to 8-bit */
  temp = temp << fmt->Rloss ;  /* Expand to a full 8-bit number */
  red  = (Uint8) temp ;

  // Get Green component.
  temp = pixel & fmt->Gmask ;  /* Isolate green component */
  temp = temp >> fmt->Gshift ; /* Shift it down to 8-bit */
  temp = temp << fmt->Gloss ;  /* Expand to a full 8-bit number */
  green = (Uint8) temp ;

  // Get Blue component.
  temp = pixel & fmt->Bmask ;  /* Isolate blue component */
  temp = temp >> fmt->Bshift ; /* Shift it down to 8-bit */
  temp = temp << fmt->Bloss ;  /* Expand to a full 8-bit number */
  blue = (Uint8) temp ;

  // Get Alpha component.
  temp = pixel & fmt->Amask ;  /* Isolate alpha component */
  temp = temp >> fmt->Ashift ; /* Shift it down to 8-bit */
  temp = temp << fmt->Aloss ;  /* Expand to a full 8-bit number */
  alpha = (Uint8) temp ;

  string result = string( "[ " ) + red + " ; " + green + " ; " + blue
	+ " ; " + alpha + " ]" ;

  LogPlug::info( "[ Red; Green; Blue; Alpha ] = " + result ) ;

  return result ;

}




int main( int argc, char * argv[] )
{

  {


	LogHolder myLog( argc, argv ) ;

	try
	{

	  LogPlug::info( "Testing basic SDL." ) ;

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

	  LogPlug::info( "Starting SDL (base and video)" ) ;

	  if ( SDL_Init( SDL_INIT_VIDEO ) != SDL_SUCCESS )
	  {
		LogPlug::fatal( "Unable to initialize SDL: "
		  + Utils::getBackendLastError() ) ;
		return Ceylan::ExitFailure ;
	  }

	  LogPlug::info( "SDL successfully initialized" ) ;

	  int xrange   = 640 ;
	  int yrange   = 480 ;
	  int askedBpp = 32 ;

	  LogPlug::info( "Setting "
		+ Ceylan::toString( xrange ) + "x"
		+ Ceylan::toString( yrange ) + " with "
		+ Ceylan::toString( askedBpp ) + " bits per pixel video mode." ) ;


	  SDL_Surface * screen = SDL_SetVideoMode( xrange, yrange, askedBpp,
		SDL_SWSURFACE ) ;

	  if ( screen == 0 )
	  {

		LogPlug::fatal( "Could not set "
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

	  Ceylan::System::FileLocator imageFinder ;

	  // When run from executable directory:
	  imageFinder.addPath( "../../src/doc/web/images" ) ;

	  // When run from tests-results directory:
	  imageFinder.addPath( "../src/doc/web/images" ) ;

	  const string imageFile = imageFinder.find( "OSDL.bmp" ) ;

	  LogPlug::info( "Displaying a BMP image from file " + imageFile ) ;

	  display_bmp( imageFile, screen ) ;

	  int x, y = 240 ;

	  SDL_LockSurface( screen ) ;


	  for ( x=1 ; x<6 ; x++ )
	  {
		LogPlug::info( "Testing getPixel at ( "
		  + Ceylan::toString( x ) + " ; "
		  + Ceylan::toString( y ) + "): "
		  + RGBtoString( getPixel( screen, x, y ), screen ) ) ;
	  }


	  x = 15 ;


	  LogPlug::info( "Testing getPixel at ( " + Ceylan::toString( x )
		+  " ; " + Ceylan::toString( y ) + "): "
		+ RGBtoString( getPixel( screen, x, y ), screen ) ) ;


	  // Update just the part of the display that we've changed.
	  SDL_UpdateRect( screen, x, y, 1, 1 ) ;


	  LogPlug::info( "Testing putPixel at ( " + Ceylan::toString( x )
		+  " ; " + Ceylan::toString( y ) + ":  putting a yellow pixel"
		+ RGBtoString( getPixel( screen, x, y ), screen ) ) ;

	  Uint32 yellow = SDL_MapRGB( screen->format, 0xff, 0xff, 0x00 ) ;

	  // Lock the screen for direct access to the pixels.

	  if ( SDL_MUSTLOCK( screen ) )
		if ( SDL_LockSurface( screen ) < 0 )
		{

		  LogPlug::fatal( "Can't lock screen: "
			+ Utils::getBackendLastError() ) ;

		  throw Ceylan::TestException(
			"Can't lock screen: "
			+ Utils::getBackendLastError() ) ;

		}

	  putPixel( screen, x, y, yellow ) ;

	  if ( SDL_MUSTLOCK( screen ) )
		SDL_UnlockSurface( screen ) ;

	  // Update just the part of the display that we've changed:
	  SDL_UpdateRect( screen, x, y, 1, 1 ) ;

	  LogPlug::info( "Checking pixel at ( " + Ceylan::toString( x )
		+  " ; " + Ceylan::toString( y )
		+ " ): should be yellow. getPixel read: "
		+ RGBtoString( getPixel( screen, x, y ), screen ) ) ;

	  int x1 = 200 ;
	  int y1 = 250 ;
	  int dx = 200 ;

	  LogPlug::info( "Drawing a yellow line from ( "
		+ Ceylan::toString( x1 )
		+  " ; " + Ceylan::toString( y1 )
		+ " ) to ( "
		+ Ceylan::toString( x1 + dx )
		+  " ; " + Ceylan::toString( y1 )
		+ " ) " ) ;

	  // Lock the screen for direct access to the pixels.

	  if ( SDL_MUSTLOCK( screen ) )
	  {

		LogPlug::fatal( "Can't lock screen: "
		  + Utils::getBackendLastError() ) ;

		throw Ceylan::TestException(
		  "Can't lock screen: "
		  + Utils::getBackendLastError() ) ;
	  }


	  for ( int d = 0; d < dx ; d++ )
		putPixel( screen, x1 + d, y1, yellow ) ;

	  if ( SDL_MUSTLOCK( screen ) )
		SDL_UnlockSurface( screen ) ;

	  // Update just the part of the display that we've changed.
	  SDL_UpdateRect( screen, x1, y1, dx, 1 ) ;

	  Ceylan::display( "< Press any key on SDL window to stop >" ) ;

	  SDL_Event event ;

	  if ( ! isBatch )
	  {

		do
		{

		  // Avoid busy waiting:
		  SDL_WaitEvent( & event ) ;

		} while ( event.type != SDL_KEYDOWN ) ;
	  }
	  else
	  {
		SDL_Delay( 1000 /* milliseconds */ ) ;
	  }

	  LogPlug::info( "Stopping SDL" ) ;
	  SDL_Quit() ;
	  LogPlug::info( "SDL successfully stopped" ) ;

	  LogPlug::info( "End of basic SDL test" ) ;

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

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


#include "OSDL.h"

#include <iostream>

#include <string>
using std::string ;

#include <list>
using std::list ;

#include <exception>


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL ;
using namespace OSDL::Video ;


/// Clears the main screen with specified color (framebuffer mode assumed).
void clearScreen( Ceylan::Uint16 color )
{

	uint16 * buffer = VRAM_A ;
	
	for( int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++ ) 
    	*buffer++ = color ;
		
}


/// Puts specified color at specified location on the framebuffer.
void putPixel( Ceylan::Uint8 x, Ceylan::Uint8 y, Ceylan::Uint16 color )
{

	uint16 * buffer = VRAM_A ;
	
	buffer[x+y*SCREEN_WIDTH] = color ;
	
}



/**
 * Draws a rectangle at specified location (upper-left corner), with specified 
 * width and height and color.
 *
 */
void drawRectangle( Ceylan::Uint8 xStart, Ceylan::Uint8 yStart, 
	Ceylan::Uint8 width, Ceylan::Uint8 height, Ceylan::Uint16 color )
{	

	for( int y = 0; y < height; y++ ) 
		for( int x = 0; x < width; x++ ) 
			putPixel( xStart + x, yStart + y, color ) ;
}



/**
 * Draws a square at specified location (upper-left corner), with specified 
 * edge length and color.
 *
 */
void drawSquare( Ceylan::Uint8 xStart, Ceylan::Uint8 yStart, 
	Ceylan::Uint8 edgeLength, Ceylan::Uint16 color )
{	

	drawRectangle( xStart, yStart, edgeLength, edgeLength, color ) ;
	
}



/// Draws a series of squares: 8x8, 16x16, 32x32, 64x64 to the main screen.
void drawShapes()
{

	Ceylan::Uint8 xOffset = 5 ;
	Ceylan::Uint8 yOffset = 5 ;

	// Height of 8, yellow:
	Uint16 color = RGB15(31,31,0) ;
	
	drawSquare(     0 +xOffset, yOffset,     8, color ) ;
	drawRectangle( 50 +xOffset, yOffset, 16, 8, color ) ;
	drawRectangle( 100+xOffset, yOffset, 32, 8, color ) ;
	
	// Height of 16, white:
	yOffset += 10 ;
	color = RGB15(31,31,31) ;

	drawSquare(     0 +xOffset, yOffset,     16, color ) ;
	drawRectangle( 50 +xOffset, yOffset, 16, 16, color ) ;
	drawRectangle( 100+xOffset, yOffset, 32, 16, color ) ;

	// Height of 32, blue:
	yOffset += 20 ;
	color = RGB15(0,0,31) ;

	drawSquare(     0 +xOffset, yOffset,     32, color ) ;
	drawRectangle( 50 +xOffset, yOffset, 16, 32, color ) ;
	drawRectangle( 100+xOffset, yOffset, 32, 32, color ) ;
	drawRectangle( 150+xOffset, yOffset, 64, 32, color ) ;

	// Height of 64, cyan:
	yOffset += 40 ;
	color = RGB15(0,31,31) ;
	
	drawRectangle( 0 +xOffset, yOffset, 32, 64, color ) ;
	drawSquare(    50+xOffset, yOffset, 64, color ) ;
	
}



/**
 * Test for the support of framebuffer provided by the OSDL library on the
 * Nintendo DS.
 *
 */
int main( int argc, char * argv[] )
{

	// To avoid using a screen for the logs:
	LogHolder myLog( argc, argv, /* immediateWrite */ false ) ;

	
    try
    {
				
		
		// For the test:
		bool interactive = true ;
		//bool interactive = false ;
		
		bool testFailed = false ;		
		
		LogPlug::info( "Test of OSDL support for framebuffer-based video." ) ;
	
		
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo ) ;	

		//VideoModule & myVideo = myOSDL.getVideoModule() ;
		
		LogPlug::debug( "Powering on LCD and both 2D cores" ) ;
		powerON( POWER_ALL_2D ) ;
		
		LogPlug::debug( "Setting main display to render directly "
			"from the framebuffer." ) ;
		videoSetMode( MODE_FB0 ) ;
		
		// Sub core not used here, not able to use framebuffer.
		
		// Using bank A for the framebuffer:
		vramSetBankA( VRAM_A_LCD ) ;
		
		clearScreen( RGB15(31,0,0) ) ;
		
		drawShapes() ;
		
		LogPlug::info( "OSDL state: " + myOSDL.toString() ) ;

		//myVideo.makeBMPScreenshot( "testOSDLVideoFramebuffer.bmp" ) ;
							
		if ( interactive )
		{
		
			LogPlug::info( "Press any key to stop waiting" ) ;
			waitForKey() ;
		
		}
				
		
		if ( testFailed )
			throw OSDL::TestException( "Test failed because of error(s) "
				"previously displayed." ) ;
								
		if ( interactive )
		{
		
			LogPlug::info( "Press any key to end the test" ) ;
			waitForKey() ;
		
		}
		
		OSDL::stop() ;
			
		// LogHolder out of scope: log browser triggered.
		
    }
   
    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught: " 
			+ e.toString( Ceylan::high ) ) ;
			
		return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( string( "Standard exception caught: " ) + e.what() ) ;
		return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
	
        LogPlug::error( "Unknown exception caught" ) ;
		return Ceylan::ExitFailure ;

    }

	LogPlug::info( "Exit on success (no error)" ) ;
	
    return Ceylan::ExitSuccess ;

}


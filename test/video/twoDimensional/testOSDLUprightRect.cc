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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;


using namespace Ceylan::Log ;




/**
 * Small unit tests for UprightRectangle primitive, rendered both as classical
 * rectangles or as rectangles with round corners.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	try
	{


	  bool testClassicalRectangles = true ;
	  bool testRoundedRectangles   = true ;
	  bool drawPlentyOfRandomRects = true ;

	  if ( ! VideoModule::IsUsingDrawingPrimitives() )
	  {

		testRoundedRectangles   = false ;

	  }

	  bool screenshotWanted = true ;

	  bool gridWanted = true ;

	  LogPlug::info( "Testing OSDL UprightRectangle" ) ;


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

		if ( token == "--online" )
		{
		  // Ignored:
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


	  Point2D p1( 14, 34 ) ;
	  Point2D p2( 50, 45 ) ;

	  UprightRectangle r1( p1, p2 ) ;

	  LogPlug::info( "UprightRectangle made from points "
		+ p1.toString() + " and " + p2.toString()
		+ " displays as " + r1.toString() ) ;

	  LogPlug::info(
		"Trying to make an incorrect UprightRectangle from points "
		+ p2.toString() + " and " + p1.toString() ) ;


	  bool caught = false ;

	  try
	  {

		UprightRectangle r2( p2, p1 ) ;

	  }
	  catch( const OSDL::Video::VideoException & e )
	  {
		LogPlug::info( "Correct, VideoException caught: "
		  + e.toString() ) ;
		caught = true ;
	  }

	  if ( ! caught )
		throw OSDL::TestException(
		  "Creating an abnormal UprightRectangle has not been detected !"
								  ) ;


	  LogPlug::info( "Initializing display to draw rectangles." ) ;

	  CommonModule & myOSDL = OSDL::getCommonModule(
		CommonModule::UseVideo | CommonModule::UseKeyboard ) ;

	  VideoModule & myVideo = myOSDL.getVideoModule() ;

	  Length screenWidth  = 640 ;
	  Length screenHeight = 480 ;

	  myVideo.setMode( screenWidth, screenHeight,
		VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

	  Surface & screen = myVideo.getScreenSurface() ;

	  screen.lock() ;


	  LogPlug::info( "Prerequisite: having four random generators" ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0,
		screenWidth ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0,
		screenHeight ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;


	  // Used for width and height, clipping is thus tested too:
	  Ceylan::Maths::Random::WhiteNoiseGenerator sizeRand( 1, 200 ) ;


	  Coordinate x, y ;

	  ColorElement red ;
	  ColorElement green ;
	  ColorElement blue ;
	  ColorElement alpha ;

	  Length width, height ;

	  UprightRectangle * rect ;


	  if ( testClassicalRectangles )
	  {


		// First, filled rectangles:

		for ( Ceylan::Uint8 i = 0; i < 50; i++ )
		{

		  x = abscissaRand.getNewValue() ;
		  y = ordinateRand.getNewValue() ;

		  width  = sizeRand.getNewValue() ;
		  height = sizeRand.getNewValue() ;

		  red   = colorRand.getNewValue() ;
		  green = colorRand.getNewValue() ;
		  blue  = colorRand.getNewValue() ;
		  alpha = colorRand.getNewValue() ;

		  rect = new UprightRectangle( x, y, width, height ) ;

		  LogPlug::info( rect->toString() ) ;

		  rect->draw( screen, red, green, blue, alpha ) ;

		  delete rect ;
		}


		// Second, non filled rectangles:

		for ( Ceylan::Uint8 i = 0; i < 50; i++ )
		{

		  x = abscissaRand.getNewValue() ;
		  y = ordinateRand.getNewValue() ;

		  width  = sizeRand.getNewValue() ;
		  height = sizeRand.getNewValue() ;

		  red   = colorRand.getNewValue() ;
		  green = colorRand.getNewValue() ;
		  blue  = colorRand.getNewValue() ;
		  alpha = colorRand.getNewValue() ;

		  rect = new UprightRectangle( x, y, width, height ) ;

		  LogPlug::info( rect->toString() ) ;

		  rect->draw( screen, red, green, blue, alpha, false ) ;

		  delete rect ;
		}


		// Finally, add borders to the screen:
		screen.drawBox( screen, Pixels::Bisque, /* filled */ false ) ;

		screen.unlock() ;

		screen.update() ;

		if ( screenshotWanted )
		  screen.savePNG( std::string( argv[0] )
			+ "-classical-examples.png" ) ;

		if ( ! isBatch )
		  myOSDL.getEventsModule().waitForAnyKey() ;

	  }


	  if ( testRoundedRectangles )
	  {

		// Draw now rectangles with rounded corners:


		// Only one example:

		screen.clear() ;

		if ( gridWanted )
		  screen.drawGrid() ;

		screen.lock() ;

		UprightRectangle * aRect ;


		// Horizontal one:
		aRect = new UprightRectangle( /* x */ 5, /* y */ 5,
		  /* width */ 300, /* height */ 50 ) ;

		aRect->drawWithRoundedCorners( screen, /* edgeWith */ 13,
		  /* edgeColorDef */ Pixels::White,
		  /* backgroundColorDef */ Pixels::DarkSlateBlue ) ;

		delete aRect ;

		// Vertical one:
		aRect = new UprightRectangle( /* x */ 5, /* y */ 100,
		  /* width */ 50, /* height */ 350 ) ;

		aRect->drawWithRoundedCorners( screen, /* edgeWith */ 13,
		  /* edgeColorDef */ Pixels::White,
		  /* backgroundColorDef */ Pixels::DarkSlateBlue ) ;

		delete aRect ;

		// Square one:
		aRect = new UprightRectangle( /* x */ 100, /* y */ 60,
		  /* width */ 100, /* height */ 100 ) ;

		aRect->drawWithRoundedCorners( screen, /* edgeWith */ 13,
		  /* edgeColorDef */ Pixels::White,
		  /* backgroundColorDef */ Pixels::DarkSlateBlue ) ;

		delete aRect ;

		// Thin edged:
		aRect = new UprightRectangle( /* x */ 100, /* y */ 200,
		  /* width */ 150, /* height */ 250 ) ;

		aRect->drawWithRoundedCorners( screen, /* edgeWith */ 1,
		  /* edgeColorDef */ Pixels::White,
		  /* backgroundColorDef */ Pixels::DarkSlateBlue ) ;

		delete aRect ;


		// Thick edged:
		aRect = new UprightRectangle( /* x */ 300, /* y */ 50,
		  /* width */ 300, /* height */ 350 ) ;


		// Now test a partially opaque rounded rectangle:
		ColorDefinition semiAlphaEdgeColor =
		  Pixels::convertRGBAToColorDefinition( 255, 0, 0, 128 ) ;

		ColorDefinition semiAlphaBackColor =
		  Pixels::convertRGBAToColorDefinition( 0, 255, 0, 128 ) ;

		// Used to compare colors:
		screen.drawCircle( /* x */ 320, /* y */ 450,
		  /* radius */ 30, semiAlphaEdgeColor ) ;

		screen.drawCircle( /* x */ 610, /* y */ 450, /* radius */ 30,
		  semiAlphaBackColor ) ;

		if ( ! aRect->drawWithRoundedCorners( screen, /* edgeWith */ 25,
			/* edgeColorDef */ semiAlphaEdgeColor,
			/* backgroundColorDef */ semiAlphaBackColor ) )
		  LogPlug::error(
			"Drawing a rectangle with round corner failed." ) ;


		delete aRect ;


		screen.unlock() ;

		screen.update() ;

		if ( screenshotWanted )
		  screen.savePNG( std::string( argv[0] )
			+ "-rounded-examples.png" ) ;

		if ( ! isBatch )
		  myOSDL.getEventsModule().waitForAnyKey() ;


		if ( drawPlentyOfRandomRects )
		{

		  // Plenty of random rounded rectangles:

		  screen.clear() ;
		  screen.lock() ;

		  Ceylan::Maths::Random::WhiteNoiseGenerator edgeRand( 0, 10 ) ;
		  Length edgeWith ;

		  for ( Ceylan::Uint32 i = 0; i < 100; i++ )
		  {

			x = abscissaRand.getNewValue() ;
			y = ordinateRand.getNewValue() ;

			width  = sizeRand.getNewValue() ;
			height = sizeRand.getNewValue() ;

			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;

			Pixels::ColorDefinition edgeColorDef =
			  Pixels::convertRGBAToColorDefinition(
				red, green, blue, alpha ) ;

			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;

			Pixels::ColorDefinition backgroundColorDef =
			  Pixels::convertRGBAToColorDefinition(
				red, green, blue, alpha ) ;

			edgeWith = edgeRand.getNewValue() ;

			/*
			 * Ensure that rectangle dimensions are enough, regarding edge width:
			 *
			 */
			rect = new UprightRectangle( x, y,
			  width + 3 * edgeWith, height + 3 * edgeWith ) ;

			LogPlug::info( rect->toString() ) ;

			rect->drawWithRoundedCorners( screen, edgeWith,
			  edgeColorDef, backgroundColorDef ) ;

			delete rect ;
		  }


		  screen.update() ;

		  if ( screenshotWanted )
			screen.savePNG( std::string( argv[0] )
			  + "-rounded-plenty.png" ) ;

		  if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;


		}

	  }


	  LogPlug::info( "Stopping OSDL." ) ;
	  OSDL::stop() ;

	  LogPlug::info( "End of OSDL UprightRectangle test" ) ;

	}

	catch ( const OSDL::Exception & e )
	{

	  LogPlug::error( "OSDL exception caught: "
		+ e.toString( Ceylan::high ) ) ;
	  return Ceylan::ExitFailure ;

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

  OSDL::shutdown() ;

  return Ceylan::ExitSuccess ;

}

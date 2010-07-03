/*
 * Copyright (C) 2003-2010 Olivier Boudeville
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
using namespace Ceylan::Maths::Random ;


#include <string>



/**
 * Small usage tests for Surfaces.
 *
 */
int main( int argc, char * argv[] )
{


	bool screenshotWanted = true ;


	LogHolder myLog( argc, argv ) ;

	try
	{


		LogPlug::info( "Testing OSDL surface" ) ;


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

				// Ignored for this test.
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


		LogPlug::info( "Prerequisite: initializing the display" ) ;


		CommonModule & myOSDL = OSDL::getCommonModule(
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;

		VideoModule & myVideo = myOSDL.getVideoModule() ;

		Length screenWidth  = 640 ;
		Length screenHeight = 480 ;

		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

		Surface & screen = myVideo.getScreenSurface() ;

		LogPlug::info( "Testing first the starting colors of pixels, "
			"both for screen surface and a newly created RGBA test surface." ) ;

		screen.lock() ;

		ColorDefinition last = Pixels::Black ;

		bool allEqual = true ;

		for ( Coordinate y = 0; y < screenHeight ; y++ )
		{
			for ( Coordinate x = 0; x < screenWidth ; x++ )
			{

				if ( x % 40 == 0 && y % 40 == 0 )
					LogPlug::trace( "Screen pixel at "
						+ Point2D( x, y ).toString() + ": "
						+ screen.describePixelAt( x, y ) ) ;

		 		ColorDefinition current = screen.getColorDefinitionAt( x, y ) ;

				// Screen is RGB, not RGBA:
				if ( ! Pixels::areEqual( last, current,
					/* use alpha */ false ) )
				{
					LogPlug::error( "Current screen color at "
						+ Point2D( x, y ).toString() + ", "
						+ Pixels::toString( current )
						+ ", is not the same as the one of previous pixel ("
						+ Pixels::toString( last ) + ")" ) ;
					last = current ;
					allEqual = false ;
				}
			}
		}

		if ( allEqual )
			LogPlug::info(
				"All pixels of the screen start at color definition "
				+ Pixels::toString(	Pixels::Black ) ) ;
		else
			throw Ceylan::TestException(
				"Screen pixels do not all start at the same color" ) ;

		screen.unlock() ;

		Length testSurfaceWidth  = 50 ;
		Length testSurfaceHeight = 100 ;


		// RGBA test surface:
		Surface & testSurface = * new Surface (
			Surface::Software | Surface::AlphaBlendingBlit,
			/* width */ testSurfaceWidth, /* height */ testSurfaceHeight ) ;


		testSurface.lock() ;


		last = Pixels::Transparent ;

		for ( Coordinate y = 0; y < testSurfaceHeight ; y++ )
		{
			for ( Coordinate x = 0; x < testSurfaceWidth ; x++ )
			{

				if ( x % 40 == 0 && y % 40 == 0 )
					LogPlug::trace( "Test surface pixel at "
						+ Point2D( x, y ).toString() + ": "
						+ testSurface.describePixelAt( x, y ) ) ;

		 		ColorDefinition current = testSurface.getColorDefinitionAt(
					x, y ) ;

				// Test surface is RGBA:
				if ( ! Pixels::areEqual( last, current, /* use alpha */ true ) )
				{
					LogPlug::error( "Current RGBA test surface color at "
						+ Point2D( x, y ).toString() + ", "
						+ Pixels::toString( current )
						+ ", is not the same as the one of previous pixel ("
						+ Pixels::toString( last ) + ")" ) ;
					last = current ;
					allEqual = false ;
				}
			}
		}

		if ( allEqual )
			LogPlug::info( "All pixels of the RGBA test surface "
				"start at color definition "
				+ Pixels::toString(	Pixels::Black ) ) ;
		else
			throw Ceylan::TestException( "RGBA test surface pixels "
				"do not all start at the same color" ) ;

		testSurface.unlock() ;

		delete & testSurface ;

		// Selected set of colors:
		ColorDefinition pencilColor  = Pixels::Orange ;
		ColorDefinition captionColor = Pixels::Yellow ;
		ColorDefinition backColor    = Pixels::DarkSlateBlue ;


		LogPlug::info( "Drawing normal PDF-based distribution graph." ) ;

		Seed seed = 145 ;

		Sample mean     = 77 ;
		Deviation sigma = 4 ;

		LogPlug::info( "Creating a probability density function (PDF) "
			" corresponding to a normal distribution whose sample mean is "
			+ Ceylan::toString( mean )
			+ " and whose standard deviation is "
			+ Ceylan::toString( sigma ) + "." ) ;

		NormalProbabilityFunction myGaussian( mean, sigma ) ;

		RandomValue sampleStart = 50 ;
		RandomValue sampleStop  = 120 ;
		RandomValue sampleCount = sampleStop - sampleStart ;

		LogPlug::info( "Creating a random generator whose PDF is: "
			+ myGaussian.toString() + " Samples will range from "
			+ Ceylan::toString( sampleStart ) + " (included) to "
			+ Ceylan::toString( sampleStop )  + " (excluded)." ) ;

		RandomGeneratorFromPDF myGaussianGenerator( myGaussian,
			sampleStart, sampleStop, seed ) ;

		LogPlug::info( "Displaying normal law. "
			+ myGaussianGenerator.displayProbabilities() ) ;


		Ceylan::Uint32 firstDrawCount  = 100 ;
		Ceylan::Uint32 secondDrawCount = 1000 ;
		Ceylan::Uint32 thirdDrawCount  = 10000 ;
		Ceylan::Uint32 fourthDrawCount = 100000 ;

		Ceylan::Uint32 totalDrawCount = 0 ;

		Length graphWidth  = 3 * sampleCount
			+ 2 * Surface::graphAbscissaOffset + 20 ;

		Length graphHeight = 80 ;


		LogPlug::info( "Generating series of random samples." ) ;

		RandomValue newValue ;

		/*
		 * Constructs a table recording how many samples are drawn for
		 * each possible value.
		 *
		 */

		Ceylan::Maths::IntegerData * distributionTable =
			new Ceylan::Maths::IntegerData[ sampleCount ] ;

		for ( Ceylan::Uint32 i = 0 ; i < sampleCount; i++)
			distributionTable[ i ] = 0 ;

		//LogPlug::info( "Throwing dice (one out of ten displayed)..." ) ;

		/// After 100 samples:
		for ( Ceylan::Uint32 currentDrawCount = totalDrawCount;
			currentDrawCount < firstDrawCount; currentDrawCount++ )
		{
			newValue = myGaussianGenerator.getNewValue() ;

			// Avoid too many useless logs:
			/*
			 if ( currentDrawCount % 10 == 0 )

			 	LogPlug::info( "Drawing value "
					+ Ceylan::toString( newValue ) + "." ) ;
			*/

			distributionTable[ newValue - sampleStart ] += 1 ;
		}

		totalDrawCount = firstDrawCount ;

		screen.lock() ;


		// SDL_gfx might be disabled:
		if ( VideoModule::IsUsingDrawingPrimitives() )
		{

		  screen.printText( "Graphs of a normal distribution", 100, 30,
			Pixels::Yellow ) ;

		  screen.printText( "(sample mean is "
			+ Ceylan::toString( mean ) + ", standard deviation is "
			+ Ceylan::toString( sigma, 2 ) + ")", 130, 50, Pixels::Yellow ) ;

		}

		UprightRectangle firstDrawingArea( 100, 80, graphWidth, graphHeight ) ;

		if ( VideoModule::IsUsingDrawingPrimitives() )
		{

		  screen.displayData( distributionTable, sampleCount, pencilColor,
			captionColor, backColor,
			"Distribution of " + Ceylan::toString( totalDrawCount )
			+ " samples", & firstDrawingArea ) ;
		}

		/// After 1 000 samples:
		for ( Ceylan::Uint32 currentDrawCount = totalDrawCount;
			currentDrawCount < secondDrawCount; currentDrawCount++ )
		{
			newValue = myGaussianGenerator.getNewValue() ;

			// Avoid too many useless logs:
			/*
			 if ( currentDrawCount % 10 == 0 )

			 	LogPlug::info( "Drawing value "
					+ Ceylan::toString( newValue ) + "." ) ;
			*/

			distributionTable[ newValue - sampleStart ] += 1 ;
		}

		totalDrawCount = secondDrawCount ;


		UprightRectangle secondDrawingArea( 100, 180,
			graphWidth, graphHeight ) ;

		if ( VideoModule::IsUsingDrawingPrimitives() )
		{

		  screen.displayData( distributionTable, sampleCount, pencilColor,
			captionColor, backColor,
			"Distribution of " + Ceylan::toString( totalDrawCount )
			+ " samples", & secondDrawingArea ) ;

		}

		/// After 10 000 samples:
		for ( Ceylan::Uint32 currentDrawCount = totalDrawCount;
			currentDrawCount < thirdDrawCount; currentDrawCount++ )
		{
			newValue = myGaussianGenerator.getNewValue() ;

			// Avoid too many useless logs:
			/*
			 if ( currentDrawCount % 10 == 0 )

			 	LogPlug::info( "Drawing value "
					+ Ceylan::toString( newValue ) + "." ) ;
			*/

			distributionTable[ newValue - sampleStart ] += 1 ;
		}

		totalDrawCount = thirdDrawCount ;

		UprightRectangle thirdDrawingArea( 100, 280, graphWidth, graphHeight ) ;

		if ( VideoModule::IsUsingDrawingPrimitives() )
		{

		  screen.displayData( distributionTable, sampleCount, pencilColor,
			captionColor, backColor,
			"Distribution of " + Ceylan::toString( totalDrawCount )
			+ " samples", & thirdDrawingArea ) ;

		}

		// After 100 000 samples:
		for ( Ceylan::Uint32 currentDrawCount = totalDrawCount;
			currentDrawCount < fourthDrawCount; currentDrawCount++ )
		{
			newValue = myGaussianGenerator.getNewValue() ;

			// Avoid too many useless logs:
			/*
			 if ( currentDrawCount % 10 == 0 )

			 	LogPlug::info( "Drawing value "
					+ Ceylan::toString( newValue ) + "." ) ;
			*/

			distributionTable[ newValue - sampleStart ] += 1 ;
		}

		totalDrawCount = fourthDrawCount ;

		UprightRectangle fourthDrawingArea( 100, 380,
			graphWidth, graphHeight ) ;

		if ( VideoModule::IsUsingDrawingPrimitives() )
		{

		  screen.displayData( distributionTable, sampleCount, pencilColor,
			captionColor, backColor,
			"Distribution of " + Ceylan::toString( totalDrawCount )
			+ " samples", & fourthDrawingArea ) ;

		}

		/*

		LogPlug::info( "Displaying final distribution table: " ) ;

		for ( Ceylan::Uint32 i = 0 ; i < sampleStop - sampleStart; i++)
			LogPlug::info( Ceylan::toString( i + sampleStart ) + " occured "
				+ Ceylan::toString( distributionTable[ i ] ) + " time(s)." ) ;
		*/

		screen.unlock() ;

		screen.update() ;

		if ( screenshotWanted )
			screen.savePNG( argv[0] + std::string( ".png" ) ) ;

		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;

		LogPlug::info( "Stopping OSDL." ) ;
		OSDL::stop() ;

		LogPlug::info( "End of OSDL Surface test." ) ;

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

	return Ceylan::ExitSuccess ;

}

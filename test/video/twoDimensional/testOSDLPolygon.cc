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
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;

using namespace Ceylan::Log ;
using namespace Ceylan::Maths ;


#include <string>
using std::string ;

#include <list>
using std::list ;




/**
 * Testing polygon sets thanks to snow flakes.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	bool plentyOfFlakes     = true ;
	bool screenshotDemanded = true ;


	try
	{


	  LogPlug::info( "Testing OSDL Polygon services." ) ;

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

	  LogPlug::info( "Drawing flake-shaped polygon sets." ) ;


	  Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0,
		screenWidth ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0,
		screenHeight ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator branchCountRand( 3, 9 ) ;
	  Ceylan::Maths::Random::WhiteNoiseGenerator lengthRand( 20, 30 ) ;
	  Ceylan::Maths::Random::WhiteNoiseGenerator thicknessRand( 2, 8 ) ;
	  Ceylan::Maths::Random::WhiteNoiseGenerator childAngleRand( 30, 70 ) ;
	  Ceylan::Maths::Random::WhiteNoiseGenerator branchingHeightRatioRand( 2,
		8 ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator scaleRand( 2, 9 ) ;


	  PolygonSet * currentFlake ;

	  screen.lock() ;

	  if ( ! plentyOfFlakes )
	  {

		LogPlug::info( "Only one flake will be drawn." ) ;

		PolygonSet & flake = PolygonSet::CreateFlake( 5,
		  /* length */ 50, /* thickness */ 10,
		  /* childAngle */ 50, /* branchingHeightRatio */ 0.6f,
		  /* scale */ 0.4f ) ;

		LogPlug::info( "Flake polygon set displays as: "
		  + flake.toString() ) ;

		flake.draw( screen, 320, 200, Snow, true ) ;
		delete & flake ;

		screen.unlock() ;
		screen.update() ;

		if ( screenshotDemanded )
		  screen.savePNG( string( argv[0] ) + ".png" ) ;

		if ( ! isBatch )
		  myOSDL.getEventsModule().waitForAnyKey() ;


		LogPlug::info( "Stopping OSDL." ) ;
		OSDL::stop() ;

		LogPlug::info( "End of OSDL Polygon test." ) ;

		return Ceylan::ExitSuccess ;


	  }


	  for ( Coordinate ordinate = 0; ordinate < screenHeight; ordinate += 50 )
		for ( Coordinate abscissa = 0; abscissa < screenWidth;
			  abscissa += 50 )
		{


		  LogPlug::info( "Drawin a flake at "
			+ Point2D( abscissa, ordinate ).toString() + "." ) ;

		  currentFlake = & PolygonSet::CreateFlake(
			branchCountRand.getNewValue(),
			lengthRand.getNewValue(),
			thicknessRand.getNewValue(),
			static_cast<AngleInDegrees>( childAngleRand.getNewValue() ),
			branchingHeightRatioRand.getNewValue() / 10.0f,
			scaleRand.getNewValue() / 10.0f
												   ) ;

		  currentFlake->draw( screen, abscissa, ordinate,
			/* color name */ Snow, /* filled */ true ) ;

		  delete currentFlake ;

		}

	  screen.unlock() ;

	  if ( screenshotDemanded )
		screen.savePNG( argv[0] + string( ".png" ) ) ;

	  screen.update() ;

	  if ( ! isBatch )
		myOSDL.getEventsModule().waitForAnyKey() ;

	  LogPlug::info( "Stopping OSDL." ) ;
	  OSDL::stop() ;


	  LogPlug::info( "End of OSDL Polygon test." ) ;


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

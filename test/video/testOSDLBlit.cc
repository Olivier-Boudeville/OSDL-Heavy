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
using namespace Ceylan::Maths::Random ;


#include <string>



/**
 * Small usage tests for blittings.
 *
 */
int main( int argc, char * argv[] )
{


  bool screenshotWanted = true ;

  {

	LogHolder myLog( argc, argv ) ;


	try
	{


	  LogPlug::info( "Testing OSDL blit" ) ;


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



	  LogPlug::info( "Prerequisite: initializing the display" ) ;

	  CommonModule & myOSDL = OSDL::getCommonModule(
		CommonModule::UseVideo | CommonModule::UseKeyboard ) ;

	  VideoModule & myVideo = myOSDL.getVideoModule() ;

	  Length screenWidth  = 640 ;
	  Length screenHeight = 480 ;

	  myVideo.setMode( screenWidth, screenHeight,
		VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

	  Surface & screen = myVideo.getScreenSurface() ;


	  screen.lock() ;
	  screen.redraw() ;

	  /*
	   * Should SDL_gfx be unavailable, we do not want exceptions to be
	   * triggered:
	   */
	  if ( VideoModule::IsUsingDrawingPrimitives() )
	  {

		screen.drawCircle( 250, 250, 50, Green, /* filled */ false ) ;

	  }

	  Surface & offscreen = * new Surface( Surface::Software,
		/* width */ 100, /* height */ 100, screen.getBitsPerPixel() ) ;

	  offscreen.lock() ;

	  if ( VideoModule::IsUsingDrawingPrimitives() )
	  {

		offscreen.drawCircle( 25, 25, /* radius */ 25,
		  /* Pixels::ColorDefinition */ Pixels::Red, /* filled */ true ) ;

		offscreen.drawCircle( 50, 50, /* radius */ 25,
		  /* Pixels::ColorDefinition */ Pixels::Green, /* filled */ true ) ;

		offscreen.drawCircle( 75, 75, /* radius */ 25,
		  /* Pixels::ColorDefinition */ Pixels::Blue, /* filled */ true ) ;

	  }

	  offscreen.unlock() ;

	  if ( screenshotWanted )
		offscreen.savePNG( argv[0] + std::string( "-offscreen.png" ) ) ;

	  offscreen.blitTo( screen,
		TwoDimensional::Point2D(
		  static_cast<Coordinate>( 400 ), 250 ) ) ;

	  delete &offscreen ;

	  screen.unlock() ;

	  screen.update() ;

	  if ( ! isBatch )
		myOSDL.getEventsModule().waitForAnyKey() ;


	  LogPlug::info( "Stopping OSDL." ) ;
	  OSDL::stop() ;

	  LogPlug::info( "End of OSDL blit test" ) ;

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

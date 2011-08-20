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


#include <string>



/**
 * Small usage tests for widgets.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	try
	{


	  LogPlug::info( "Testing OSDL Widget" ) ;



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
		CommonModule::UseVideo | CommonModule::UseEvents ) ;

	  VideoModule & myVideo = myOSDL.getVideoModule() ;

	  Length screenWidth  = 640 ;
	  Length screenHeight = 480 ;

	  myVideo.setMode( screenWidth, screenHeight,
		VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

	  Surface & screen = myVideo.getScreenSurface() ;

	  LogPlug::info( "Drawing widget." ) ;


	  LogPlug::info( "Before adding widget, screen surface is: "
		+ screen.toString() ) ;

	  screen.lock() ;

	  if ( VideoModule::IsUsingDrawingPrimitives() )
	  {

		/*
		 * Widgets will be owned (and deallocated) by their container, here the
		 * screen surface.
		 *
		 * Widget width is only 100, so 'Heimdal' will be truncated to 'Heimd':
		 *
		 */
		new Widget( /* container */ screen,
		  /* location */ Point2D( 50, 50 ),
		  /* width */ 100,
		  /* height */ 100,
		  /* baseColorMode */ Widget::BackgroundColor,
		  /* backgroundColor */ Brown,
		  "Eye of Heimdal" ) ;


		// Made to overlap:

		new Widget( /* container */ screen,
		  /* location */ Point2D( 100, 100 ),
		  /* width */ 100,
		  /* height */ 100,
		  /* baseColorMode */ Widget::BackgroundColor,
		  /* backgroundColor */ Turquoise,
		  "Hello world" ) ;


		LogPlug::info( "After adding widget, screen surface is: "
		  + screen.toString() ) ;

		screen.drawCircle( 150, 150, 150, Red, /* filled */ false ) ;


		screen.redraw() ;

		screen.drawCircle( 300, 250, 100, Green, /* filled */ true ) ;

	  }

	  screen.unlock() ;

	  screen.update() ;

	  screen.savePNG( argv[0] + std::string( ".png" ) ) ;

	  if ( ! isBatch )
		myOSDL.getEventsModule().waitForAnyKey() ;

	  LogPlug::info( "End of OSDL Widget test." ) ;

	  LogPlug::info( "Stopping OSDL." ) ;
	  OSDL::stop() ;

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

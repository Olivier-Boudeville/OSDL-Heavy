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

using namespace Ceylan::Log ;


#include <string>
using std::string ;


const std::string iconFile = "OSDL-icon.png" ;



/**
 * Test for window manager operations.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	try
	{


	  LogPlug::info( "Testing OSDL window manager services." ) ;


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


	  LogPlug::info( "Prerequisite: starting OSDL with video enabled." ) ;

	  CommonModule & myOSDL = getCommonModule(
		CommonModule::UseVideo | CommonModule::UseEvents ) ;

	  VideoModule & myVideo = myOSDL.getVideoModule() ;

	  string title ;
	  string iconName ;

	  myVideo.getWindowCaption( title, iconName ) ;

	  LogPlug::info( "Reading window caption: title is '" + title
		+ "', icon name is '" + iconName + "'." ) ;

	  title    = "OSDL is great" ;
	  iconName = "OSDL window manager test" ;

	  LogPlug::info( "Writing window caption: title will be '" + title
		+ "', icon name will be '" + iconName + "'." ) ;

	  myVideo.setWindowCaption( title, iconName ) ;

	  LogPlug::info( "Reading window caption: title is '" + title
		+ "', icon name is '" + iconName + "'." ) ;

	  LogPlug::info( "Setting icon image." ) ;

	  Ceylan::System::FileLocator imageFinder ;


	  // When run from playTests.sh build directory:
	  imageFinder.addPath( "../src/doc/web/images" ) ;

	  // When run from executable build directory:
	  imageFinder.addPath( "../../src/doc/web/images" ) ;

	  // When run from executable install directory:
	  imageFinder.addPath( "../OSDL/doc/web/images" ) ;

	  myVideo.setWindowIcon( imageFinder.find( iconFile ) ) ;

	  Length screenWidth  = 640 ;
	  Length screenHeight = 480 ;

	  myVideo.setMode( screenWidth, screenHeight,
		VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

	  Surface & screen = myVideo.getScreenSurface() ;

	  screen.update() ;

	  if ( ! isBatch )
		myOSDL.getEventsModule().waitForAnyKey() ;


	  LogPlug::info( "Stopping OSDL." ) ;
	  OSDL::stop() ;

	  LogPlug::info( "End of OSDL window manager test." ) ;

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

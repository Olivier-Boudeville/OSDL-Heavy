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
using namespace OSDL::Video::OpenGL ;

using namespace Ceylan::Log ;



const std::string firstTextureFilename  = "Soldier-heavy-purple-small.png" ;
const std::string secondTextureFilename = "Rune-stone-small.jpg" ;



const std::string imageWebPath = "web/images" ;




/**
 * Testing the basic services of the OSDL video using OpenGL.
 *
 * @note Only used here for 2D.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	try
	{


	  LogPlug::info( "Testing OSDL video OpenGL basic services." ) ;


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


	  LogPlug::info( "Starting OSDL with OpenGL enabled." ) ;

	  CommonModule & myOSDL = getCommonModule(
		CommonModule::UseVideo | CommonModule::UseEvents ) ;

	  LogPlug::info( "Testing real video (displayable)." ) ;

	  LogPlug::info( "Getting video module." ) ;
	  VideoModule & myVideo = myOSDL.getVideoModule() ;


	  LogPlug::info( "Displaying available video definitions: "
		+ VideoModule::DescribeAvailableDefinitions(
		  Surface::FullScreen | Surface::Hardware ) ) ;

	  LogPlug::info( "Displaying configuration informations, "
		"including best available pixel format: "
		+ VideoModule::DescribeVideoCapabilities() ) ;

	  LogPlug::info( "Displaying video driver name: "
		+ VideoModule::GetDriverName() + "." ) ;


	  // Going from potential to real:

	  LogPlug::info( "Entering visual tests: "
		"initializing the screen with OpenGL." ) ;


	  Length screenWidth  = 640 ;
	  Length screenHeight = 480 ;

	  myVideo.setMode( screenWidth, screenHeight,
		VideoModule::UseCurrentColorDepth,
		VideoModule::OpenGL /* | VideoModule::Fullscreen */, OpenGLFor2D ) ;

	  LogPlug::info( "Displaying now new current video informations. "
		+ VideoModule::DescribeVideoCapabilities() ) ;

	  Surface & screen = myVideo.getScreenSurface() ;

	  screen.clear() ;

	  Ceylan::System::FileLocator textureFinder ;


	  // When run from playTests.sh build directory:
	  textureFinder.addPath( "../src/doc/" + imageWebPath ) ;

	  // When run from executable build directory:
	  textureFinder.addPath( "../../src/doc/" + imageWebPath ) ;

	  // When run from executable install directory:
	  textureFinder.addPath( "../OSDL/doc/" + imageWebPath ) ;


	  GLTexture & firstTexture = * new GLTexture(
		textureFinder.find( firstTextureFilename ), GLTexture::For2D ) ;

	  LogPlug::info( "Is the first texture already resident? "
		+ Ceylan::toString( firstTexture.isResident() ) ) ;

	  // Texture was (pre)-loaded at construction, now uploading it:
	  firstTexture.upload() ;

	  glBegin(GL_QUADS);
	  {

		glTexCoord2d( 0, 0 ) ;
		glVertex2d( 100, 100 ) ;

		glTexCoord2d (0, 1 ) ;
		glVertex2d( 100, 196 ) ;

		glTexCoord2d( 1, 1 ) ;
		glVertex2d( 196,196 ) ;

		glTexCoord2d( 1, 0 ) ;
		glVertex2d( 196, 100 ) ;

	  }
	  glEnd();

	  LogPlug::info( "Is the first texture resident now? "
		+ Ceylan::toString( firstTexture.isResident() ) ) ;


	  GLTexture & secondTexture = * new GLTexture(
		textureFinder.find( secondTextureFilename ), GLTexture::For2D ) ;

	  LogPlug::info( "Is the second texture already resident? "
		+ Ceylan::toString( secondTexture.isResident() ) ) ;

	  // Texture was (pre)-loaded at construction, now uploading it:
	  secondTexture.upload() ;

	  glBegin(GL_QUADS);
	  {

		glTexCoord2d( 0, 0 ) ;
		glVertex2d( 120, 120 ) ;

		glTexCoord2d (0, 1 ) ;
		glVertex2d( 120, 216 ) ;

		glTexCoord2d( 1, 1 ) ;
		glVertex2d( 216,216 ) ;

		glTexCoord2d( 1, 0 ) ;
		glVertex2d( 216, 120 ) ;

	  }
	  glEnd();

	  LogPlug::info( "Is the second texture resident now? "
		+ Ceylan::toString( secondTexture.isResident() ) ) ;

	  LogPlug::info( VideoModule::DescribeEnvironmentVariables() ) ;

	  OSDL::Events::EventsModule & myEvents = myOSDL.getEventsModule() ;


	  screen.update() ;

	  if ( ! isBatch )
		myEvents.waitForAnyKey() ;

	  delete & firstTexture ;
	  delete & secondTexture ;

	  LogPlug::info( "Stopping OSDL." ) ;
	  OSDL::stop() ;

	  LogPlug::info( "End of OSDL OpenGL test." ) ;

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

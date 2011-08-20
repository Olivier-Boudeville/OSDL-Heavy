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
using namespace Ceylan::System ;


#include <string>
using std::string ;


const std::string trueTypeFirstFontName  = "stilltim.ttf" ;
const std::string trueTypeSecondFontName = "iomanoid.ttf" ;
const std::string trueTypeThirdFontName  = "minynbi.ttf" ;
const std::string trueTypeFourthFontName = "cretino.ttf" ;



/*
 * Fixed font directory is defined relatively to OSDL documentation tree, from
 * playTests.sh location in build tree:
 *
 */
const std::string fixedFontDirFromExec = "../../../src/doc/web/common/fonts" ;


/**
 * This font directory is defined relatively to the build tree for this test:
 *
 */
const std::string fixedFontDirForBuildPlayTests =
  "../src/doc/web/common/fonts" ;


/*
 * Fixed font directory is defined relatively to OSDL documentation tree, from
 * playTests.sh location in installed tree:
 *
 */
const std::string fixedFontDirForInstalledPlayTests
= "../OSDL/doc/web/common/fonts" ;



/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * from executable build directory:
 *
 */
const std::string trueTypeFontDirFromExec =
  "../../../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in build tree:
 *
 */
const std::string trueTypeFontDirForBuildPlayTests
= "../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in installed tree:
 *
 */
const std::string trueTypeFontDirForInstalledPlayTests
= "../OSDL/doc/web/common/fonts" ;





/**
 * Small global tests for all graphical text output: useful to check at a glance
 * that everything is still working.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;

	// SDL_gfx might be disabled, in that case no putPixel will be available:
	if ( ! VideoModule::IsUsingDrawingPrimitives() )
	{

	  LogPlug::warning( "SDL_gfx is disabled, thus no fixed font will be "
		"available, stopping test now." ) ;

	  OSDL::stop() ;

	  OSDL::shutdown() ;

	  return Ceylan::ExitSuccess ;

	}

	try
	{


	  LogPlug::info( "Testing all OSDL primitives for text output." ) ;


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

	  LogPlug::info( "Font path is managed by "
		+ Text::Font::FontFileLocator.toString() ) ;


	  screen.lock() ;

	  bool gridWanted = true ;

	  if ( gridWanted )
	  {
		LogPlug::info( "Drawing a grid to check transparency of text." ) ;
		if ( ! screen.drawGrid() )
		  LogPlug::error( "Grid rendering failed." ) ;
	  }


	  LogPlug::info( "Testing basic text output." ) ;

	  if ( ! Text::printBasic( "This is a test for 'printBasic' primitive, "
		  "which uses the built-in font.", screen, 10, 40, Pixels::Tomato ) )
		throw Ceylan::TestException(
		  "Test for 'printBasic' primitive failed" ) ;



	  LogPlug::info( "Testing fixed font text output." ) ;


	  Text::FixedFont::FixedFontFileLocator.addPath(
		fixedFontDirFromExec ) ;

	  Text::FixedFont::FixedFontFileLocator.addPath(
		fixedFontDirForBuildPlayTests ) ;

	  Text::FixedFont::FixedFontFileLocator.addPath(
		fixedFontDirForInstalledPlayTests ) ;


	  Text::FixedFont myFixedFont( 9, 18,
		/* renderingStyle */ Text::Font::Bold, /* convertToDisplay */ true,
		/* render cache */ Text::Font::None ) ;

	  LogPlug::info( "Successfully loaded following font: "
		+ myFixedFont.toString() ) ;

	  myFixedFont.blitLatin1Text( screen, 10, 50,
		"This is a test for 'FixedFont' rendering,",
		Text::Font::Solid, Pixels::Chartreuse ) ;

	  myFixedFont.blitLatin1Text( screen, 10, 70,
		"which can use various fonts and attributes.",
		Text::Font::Solid, Pixels::Chartreuse ) ;


	  LogPlug::info( "Testing TrueType font text output." ) ;

	  const string firstTestSentence =
		"This is a test for OSDL's TrueType Font rendering. It is ok?" ;

	  const string secondTestSentence = "I hope so, coz it was quite "
		"a nightmare to have it all workin'" ;

	  Text::Font::RenderQuality quality =
		/* Text::Font::Solid */ Text::Font::Blended ;

	  Text::Font::RenderCache cache =
		/* Text::Font::None */ Text::Font::WordCached ;


	  Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
		trueTypeFontDirFromExec ) ;

	  Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
		trueTypeFontDirForBuildPlayTests ) ;

	  Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
		trueTypeFontDirForInstalledPlayTests ) ;


	  std::string trueTypeFirstFontPath = Text::TrueTypeFont::FindPathFor(
		trueTypeFirstFontName ) ;

	  Text::TrueTypeFont myFirstTrueTypeFont( trueTypeFirstFontPath,
		/* renderingStyle */ Text::Font::Normal,
		/* convertToDisplay */ true,
		/* render cache */ cache ) ;

	  myFirstTrueTypeFont.load( /* point size */ 30 ),

		LogPlug::info( "Successfully loaded following font: "
		  + myFirstTrueTypeFont.toString() ) ;

	  myFirstTrueTypeFont.blitLatin1Text( screen, 10, 100,
		firstTestSentence, quality, Pixels::Orchid ) ;

	  myFirstTrueTypeFont.blitLatin1Text( screen, 10, 140,
		secondTestSentence, quality, Pixels::Orchid ) ;



	  std::string trueTypeSecondFontPath = Text::TrueTypeFont::FindPathFor(
		trueTypeSecondFontName ) ;

	  Text::TrueTypeFont mySecondTrueTypeFont( trueTypeSecondFontPath,
		/* renderingStyle */ Text::Font::Normal,
		/* convertToDisplay */ true,
		/* render cache */ cache ) ;

	  mySecondTrueTypeFont.load( /* point size */ 30 ),

		LogPlug::info( "Successfully loaded following font: "
		  + mySecondTrueTypeFont.toString() ) ;

	  mySecondTrueTypeFont.blitLatin1Text( screen, 10, 200,
		firstTestSentence, quality, Pixels::HotPink ) ;

	  mySecondTrueTypeFont.blitLatin1Text( screen, 10, 240,
		secondTestSentence, quality, Pixels::HotPink ) ;


	  std::string trueTypeThirdFontPath = Text::TrueTypeFont::FindPathFor(
		trueTypeThirdFontName ) ;

	  Text::TrueTypeFont myThirdTrueTypeFont( trueTypeThirdFontPath,
		/* renderingStyle */ Text::Font::Normal,
		/* convertToDisplay */ true,
		/* render cache */ cache ) ;

	  myThirdTrueTypeFont.load( /* point size */ 30 ),

		LogPlug::info( "Successfully loaded following font: "
		  + myThirdTrueTypeFont.toString() ) ;

	  myThirdTrueTypeFont.blitLatin1Text( screen, 10, 300, firstTestSentence,
		quality, Pixels::PeachPuff ) ;

	  myThirdTrueTypeFont.blitLatin1Text( screen, 10, 340, secondTestSentence,
		quality, Pixels::PeachPuff ) ;


	  std::string trueTypeFourthFontPath = Text::TrueTypeFont::FindPathFor(
		trueTypeFourthFontName ) ;

	  Text::TrueTypeFont myFourthTrueTypeFont( trueTypeFourthFontPath,
		/* renderingStyle */ Text::Font::Normal,
		/* convertToDisplay */ true,
		/* render cache */ cache ) ;

	  myFourthTrueTypeFont.load( /* point size */ 30 ) ;

	  LogPlug::info( "Successfully loaded following font: "
		+ myFourthTrueTypeFont.toString() ) ;

	  myFourthTrueTypeFont.blitLatin1Text( screen, 10, 400,
		firstTestSentence, quality, Pixels::LawnGreen ) ;

	  myFourthTrueTypeFont.blitLatin1Text( screen, 10, 440,
		secondTestSentence, quality, Pixels::LawnGreen ) ;


	  screen.unlock() ;

	  screen.update() ;
	  screen.savePNG( std::string( argv[0] ) + "-allfonts.png" ) ;

	  if ( ! isBatch )
		myOSDL.getEventsModule().waitForAnyKey() ;


	  LogPlug::info( "Stopping OSDL." ) ;
	  OSDL::stop() ;


	  LogPlug::info( "End of OSDL test of primitives for text output." ) ;


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

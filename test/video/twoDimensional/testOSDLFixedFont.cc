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
using namespace OSDL::Video::TwoDimensional::Text ;
using namespace OSDL::Video::Pixels ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;


#include <string>



/**
 * This font directory is defined relatively to the place where the script
 * automating the test suite will be run (trunk/test/playTests.sh, which changes
 * its current directory to trunk/test in all cases):
 *
 */
const std::string fixedFontDirForPlayTests = "../src/doc/web/common/fonts" ;


/**
 * This font directory is defined relatively to the build tree for this test:
 *
 */
const std::string fixedFontDirFromTestBuildLocation =
  "../../../src/doc/web/common/fonts" ;


/**
 * This font directory is defined relatively to the install tree for this test:
 *
 */
const std::string fixedFontDirFromTestInstallLocation =
  "../OSDL/doc/web/common/fonts" ;


/**
 * Small usage tests for fixed font text output.
 *
 */
int main( int argc, char * argv[] )
{

  bool randomTestWanted = true ;
  bool fontTestWanted   = true ;

  bool gridWanted = true ;

  LogHolder myLog( argc, argv ) ;

  /* SDL_gfx might be disabled, in that case no fixed font (nor grid) will
   * be available:
   */
  if ( ! VideoModule::IsUsingDrawingPrimitives() )
  {

	LogPlug::warning( "SDL_gfx is disabled, thus no fixed font will be "
	  "available, stopping test now." ) ;

	return Ceylan::ExitSuccess ;

  }

  try
  {


	LogPlug::info( "Testing OSDL Fixed font service" ) ;


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

	LogPlug::info( "Font path is managed by "
	  + Text::Font::FontFileLocator.toString() ) ;

	if ( randomTestWanted )
	{


	  LogPlug::info(
		"Writing at random places text with random color." ) ;

	  LogPlug::info( "Prerequisite: having four random generators" ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0,
		screenWidth ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0,
		screenHeight ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
	  Ceylan::Maths::Random::WhiteNoiseGenerator charRand( 0, 256 ) ;

	  Coordinate x, y ;

	  bool correct = false ;
	  try
	  {
		FixedFont nonExistingFont( 3, 17,
		  /* renderingStyle */ Font::Normal,
		  /* convertToDisplay */ true,
		  /* render cache */ Font::None ) ;
	  }
	  catch( const FontException & e )
	  {

		LogPlug::info(
		  "Asking for a non-existing font failed as expected: "
		  + e.toString() ) ;
		correct = true ;
	  }

	  if ( ! correct )
		throw Ceylan::TestException( "Requesting a non-existing font "
		  "did not fail as expected." ) ;

	  /*
	   * Wanting to use at first '6x12.fnt', and to find it from
	   * various locations.
	   *
	   */

	  FixedFont::FixedFontFileLocator.addPath(
		fixedFontDirForPlayTests ) ;

	  FixedFont::FixedFontFileLocator.addPath(
		fixedFontDirFromTestBuildLocation ) ;

	  FixedFont::FixedFontFileLocator.addPath(
		fixedFontDirFromTestInstallLocation	) ;


	  FixedFont existingFont( 6, 12, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;

	  LogPlug::info( "Successfully loaded following font: "
		+ existingFont.toString() ) ;

	  ColorElement red ;
	  ColorElement green ;
	  ColorElement blue ;

	  ColorDefinition textColor ;

	  Surface * glyphSurface ;

	  /*
	   * No screen.lock() / screen.unlock() needed thanks to primitive
	   * auto-lock.
	   *
	   */

	  if ( gridWanted )
	  {

		LogPlug::info(
		  "Drawing a grid to check transparency of text." ) ;

		if ( ! screen.drawGrid() )
		  LogPlug::error( "Grid rendering failed." ) ;
	  }

	  Ceylan::Latin1Char toRender ;


	  // Generates an intermediate texture and blit it, or blit directly:
	  bool useBlit = false ;

	  for ( Ceylan::Uint32 i = 0; i < 600; i++ )
	  {

		x = abscissaRand.getNewValue() ;
		y = ordinateRand.getNewValue() ;

		red   = colorRand.getNewValue() ;
		green = colorRand.getNewValue() ;
		blue  = colorRand.getNewValue() ;

		toRender = static_cast<Ceylan::Latin1Char>(
		  charRand.getNewValue() ) ;

		textColor = convertRGBAToColorDefinition( red, green, blue,
		  Pixels::AlphaOpaque ) ;

		// Avoid too much logs:
		if ( i % 10 == 0 )
		  LogPlug::debug( "Requesting rendering for character '"
			+ Ceylan::toString( toRender )
			+ "' at (" + Ceylan::toString( x ) + ";"
			+ Ceylan::toString( y )
			+ ") with color " + Pixels::toString( textColor )
			+ "." ) ;

		// Try renderLatin1GlyphAlpha instead to see it is not correct.

		if ( useBlit )
		{
		  existingFont.blitLatin1Glyph( screen, x, y, toRender,
			Font::Solid, textColor ) ;
		}
		else
		{
		  glyphSurface = & existingFont.renderLatin1Glyph( toRender,
			Font::Solid, textColor ) ;

		  glyphSurface->blitTo( screen, x, y ) ;
		  delete glyphSurface ;
		}

	  }

	  Surface * textSurface = & existingFont.renderLatin1Text(
		"You want to read a game-book",
		Font::Solid, Pixels::DarkOrange ) ;

	  textSurface->blitTo( screen, 100, 100 ) ;

	  delete textSurface ;


	  screen.update() ;
	  screen.savePNG( std::string( argv[0] ) + "-random.png" ) ;

	  if ( ! isBatch )
		myOSDL.getEventsModule().waitForAnyKey() ;

	}

	if ( fontTestWanted )
	{

	  screen.clear() ;


	  if ( gridWanted )
	  {

		LogPlug::info(
		  "Drawing a grid to check transparency of text." ) ;

		if ( ! screen.drawGrid() )
		  LogPlug::error( "Grid rendering failed." ) ;
	  }

	  std::string toPrint = "1234567890&'(-_"
		"aàâbcçdeéèêfghiîjklmnoôpqrstuûvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" ;

	  ColorDefinition textColor = Pixels::PaleGreen ;

	  FixedFont::FixedFontFileLocator.addPath(
		fixedFontDirForPlayTests ) ;

	  FixedFont::FixedFontFileLocator.addPath(
		fixedFontDirFromTestBuildLocation ) ;

	  // Test in turn all known font files:

	  // Width: 5
	  FixedFont one( 5, 7, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;

	  one.blitLatin1Text( screen, 10, 10, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont two( 5, 8, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;

	  two.blitLatin1Text( screen, 10, 20, toPrint, Font::Solid,
		textColor ) ;


	  // Width: 6
	  FixedFont three( 6, 9, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;

	  three.blitLatin1Text( screen, 10, 30, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont four( 6, 10, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;

	  four.blitLatin1Text( screen, 10, 40, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont five( 6, 12, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;

	  five.blitLatin1Text( screen, 10, 55, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont six( 6, 13, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;

	  six.blitLatin1Text( screen, 10, 70, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont seven( 6, 13, /* renderingStyle */ Font::Bold,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  seven.blitLatin1Text( screen, 10, 85, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont height( 6, 13, /* renderingStyle */ Font::Italic,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  height.blitLatin1Text( screen, 10, 100, toPrint, Font::Solid,
		textColor ) ;


	  // Width: 7
	  FixedFont nine( 7, 13, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  nine.blitLatin1Text( screen, 10, 115, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont ten( 7, 13, /* renderingStyle */ Font::Bold,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  ten.blitLatin1Text( screen, 10, 130, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont eleven( 7, 13, /* renderingStyle */ Font::Italic,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  eleven.blitLatin1Text( screen, 10, 145, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont twelve( 7, 14, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  twelve.blitLatin1Text( screen, 10, 160, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont thirteen( 7, 14, /* renderingStyle */ Font::Bold,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  thirteen.blitLatin1Text( screen, 10, 175, toPrint, Font::Solid,
		textColor ) ;


	  // Width: 8

	  FixedFont fourteen( 8, 13, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  fourteen.blitLatin1Text( screen, 10, 190, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont fifteen( 8, 13, /* renderingStyle */ Font::Bold,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  fifteen.blitLatin1Text( screen, 10, 205, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont sixteen( 8, 13, /* renderingStyle */ Font::Italic,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  sixteen.blitLatin1Text( screen, 10, 220, toPrint, Font::Solid,
		textColor ) ;


	  // Width: 9

	  FixedFont seventeen( 9, 15, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  seventeen.blitLatin1Text( screen, 10, 235, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont heighteen( 9, 15, /* renderingStyle */ Font::Bold,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  heighteen.blitLatin1Text( screen, 10, 255, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont nineteen( 9, 18, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  nineteen.blitLatin1Text( screen, 10, 275, toPrint, Font::Solid,
		textColor ) ;

	  FixedFont twenty( 9, 18, /* renderingStyle */ Font::Bold,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  twenty.blitLatin1Text( screen, 10, 305, toPrint, Font::Solid,
		textColor ) ;


	  // Width: 10

	  FixedFont twentyone( 9, 18, /* renderingStyle */ Font::Normal,
		/* convertToDisplay */ true, /* render cache */ Font::None ) ;
	  twentyone.blitLatin1Text( screen, 10, 325, toPrint, Font::Solid,
		textColor ) ;

	  // Also the built-in font:
	  printBasic( toPrint, screen, 10, 470, textColor ) ;

	  screen.update() ;

	  screen.savePNG( std::string( argv[0] ) + "-allfonts.png" ) ;

	  if ( ! isBatch )
		myOSDL.getEventsModule().waitForAnyKey() ;

	}


	LogPlug::info( "Stopping OSDL." ) ;
	OSDL::stop() ;

	LogPlug::info( "End of OSDL FixedFont test" ) ;

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

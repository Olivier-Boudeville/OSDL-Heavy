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
const std::string trueTypeFontDirForBuildPlayTests =
  "../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in installed tree:
 *
 */
const std::string trueTypeFontDirForInstalledPlayTests =
  "../OSDL/doc/web/common/fonts" ;



/*
 * Those are Larabie splendid fonts, see read_me.html under
 * trueTypeFontDirFromExec:
 *
 */
const std::string firstTrueTypeFontFile  = "cretino.ttf" ;
const std::string secondTrueTypeFontFile = "iomanoid.ttf" ;
const std::string thirdTrueTypeFontFile  = "stilltim.ttf" ;

const std::string firstTestSentence  = "Ceylan and OSDL rock!" ;
const std::string secondTestSentence = "Yeah man, they both rock." ;




/**
 * Small usage tests for caches of TrueType font cache.
 *
 */
int main( int argc, char * argv[] )
{


  LogHolder myLog( argc, argv ) ;


  bool testGlyphCache = true ;
  bool testWordCache  = true ;
  bool testTextCache  = true ;


  try
  {


	LogPlug::info( "Testing OSDL TrueType font cache: "
	  "inspect 'Debug' channel to check cache hits and misses" ) ;


	LogPlug::info( "At the end of this test, one should see red letters "
	  " ('a', 'b'), and two groups of two sentences, "
	  "first green, then blue." ) ;


	LogPlug::info( "Note that having compiled OSDL "
	  "with the OSDL_DEBUG_FONT flag set "
	  "allows for far more debug informations." ) ;


	LogPlug::info( "Testing OSDL video basic services." ) ;



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


	CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo
	  | CommonModule::UseKeyboard ) ;

	VideoModule & myVideo = myOSDL.getVideoModule() ;

	Length screenWidth  = 640 ;
	Length screenHeight = 480 ;

	myVideo.setMode( screenWidth, screenHeight,
	  VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

	Surface & screen = myVideo.getScreenSurface() ;

	Pixels::ColorDefinition textColor = Pixels::Red  ;


	TrueTypeFont::TrueTypeFontFileLocator.addPath(
	  trueTypeFontDirFromExec ) ;

	TrueTypeFont::TrueTypeFontFileLocator.addPath(
	  trueTypeFontDirForBuildPlayTests ) ;

	TrueTypeFont::TrueTypeFontFileLocator.addPath(
	  trueTypeFontDirForInstalledPlayTests ) ;


	Surface * mySurface ;

	bool gridWanted = true ;

	if ( gridWanted )
	{

	  if ( VideoModule::IsUsingDrawingPrimitives() )
	  {

		LogPlug::info( "Drawing a grid to check transparency of text." ) ;

		if ( ! screen.drawGrid() )
		  LogPlug::error( "Grid rendering failed." ) ;
	  }

	}

	// Glyph cached section.


	if ( testGlyphCache )
	{


	  LogPlug::info( "First: testing cache for glyph rendering." ) ;


	  std::string firstTrueTypeFontPath = Text::TrueTypeFont::FindPathFor(
		firstTrueTypeFontFile ) ;

	  TrueTypeFont myGlyphCachedTrueTypeFont( firstTrueTypeFontPath,
		/* font index */ 0, /* convertToDisplay */ true,
		/* render cache */ Font::GlyphCached ) ;

	  myGlyphCachedTrueTypeFont.load( /* point size */ 15 ),

		LogPlug::info(
		  "Successfully loaded following font in initial state: "
		  + myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  mySurface = & myGlyphCachedTrueTypeFont.renderLatin1Glyph(
		'a', Font::Solid, textColor ) ;

	  Coordinate ordinate = 60 ;

	  mySurface->blitTo( screen, 20, ordinate ) ;
	  delete mySurface ;

	  LogPlug::info( "Font after first render of 'a': "
		+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  mySurface = & myGlyphCachedTrueTypeFont.renderLatin1Glyph(
		'a', Font::Solid, textColor ) ;

	  mySurface->blitTo( screen, 40, ordinate ) ;
	  delete mySurface ;

	  LogPlug::info( "Font after second render of 'a': "
		+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  mySurface = & myGlyphCachedTrueTypeFont.renderLatin1Glyph(
		'a', Font::Solid, textColor ) ;

	  mySurface->blitTo( screen, 60, ordinate ) ;
	  delete mySurface ;

	  LogPlug::info( "Font after third render of 'a': "
		+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  myGlyphCachedTrueTypeFont.blitLatin1Glyph( screen, 30, ordinate,
		'b', Font::Solid, textColor ) ;

	  LogPlug::info( "Font after first blit of 'b': "
		+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  myGlyphCachedTrueTypeFont.blitLatin1Glyph( screen, 50, ordinate,
		'b', Font::Solid, textColor ) ;

	  LogPlug::info( "Font after second blit of 'b': "
		+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  myGlyphCachedTrueTypeFont.blitLatin1Glyph( screen, 70, ordinate,
		'b', Font::Solid, textColor ) ;

	  LogPlug::info( "Font after third blit of 'b': "
		+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;


	}



	// Word cached section.

	if ( testWordCache )
	{

	  LogPlug::info( "Second: testing cache for word rendering, "
		"with first test sentence: '" + firstTestSentence
		+ "', with second test sentence '"
		+ secondTestSentence + "'." ) ;

	  textColor = Pixels::Green ;

	  std::string secondTrueTypeFontPath =
		Text::TrueTypeFont::FindPathFor( secondTrueTypeFontFile ) ;

	  TrueTypeFont myWordCachedTrueTypeFont( secondTrueTypeFontPath,
		/* font index */ 0, /* convertToDisplay */ true,
		/* render cache */ Font::WordCached ) ;

	  myWordCachedTrueTypeFont.load( /* point size */ 15 ) ;

	  LogPlug::info(
		"Successfully loaded following font in initial state: "
		+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  mySurface = & myWordCachedTrueTypeFont.renderLatin1Text(
		firstTestSentence, Font::Solid, textColor ) ;

	  Coordinate ordinate = 100 ;

	  mySurface->blitTo( screen, 20, ordinate ) ;
	  delete mySurface ;

	  LogPlug::info( "Font after first render of first test sentence: "
		+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  mySurface = & myWordCachedTrueTypeFont.renderLatin1Text(
		firstTestSentence, Font::Solid, textColor ) ;

	  mySurface->blitTo( screen, 220, ordinate ) ;
	  delete mySurface ;

	  LogPlug::info( "Font after second render of first test sentence: "
		+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  mySurface = & myWordCachedTrueTypeFont.renderLatin1Text(
		firstTestSentence, Font::Solid, textColor ) ;

	  mySurface->blitTo( screen, 420, ordinate ) ;
	  delete mySurface ;

	  LogPlug::info( "Font after third render of first test sentence: "
		+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;


	  myWordCachedTrueTypeFont.blitLatin1Text( screen, 20, 150,
		secondTestSentence, Font::Solid, textColor ) ;

	  LogPlug::info( "Font after first blit of second test sentence: "
		+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  myWordCachedTrueTypeFont.blitLatin1Text( screen, 220, 150,
		secondTestSentence, Font::Solid, textColor ) ;

	  LogPlug::info( "Font after first blit of second test sentence: "
		+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  myWordCachedTrueTypeFont.blitLatin1Text( screen, 420, 150,
		secondTestSentence, Font::Solid, textColor ) ;

	  LogPlug::info( "Font after first blit of second test sentence: "
		+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	}



	// Text cached section.


	if ( testTextCache )
	{


	  LogPlug::info( "Third: testing cache for text rendering, "
		"with first test sentence: '" + firstTestSentence
		+ "', with second test sentence '"
		+ secondTestSentence + "'." ) ;

	  textColor = Pixels::Blue ;


	  std::string thirdTrueTypeFontPath =
		Text::TrueTypeFont::FindPathFor( thirdTrueTypeFontFile ) ;

	  TrueTypeFont myTextCachedTrueTypeFont( thirdTrueTypeFontPath,
		/* font index */ 0, /* convertToDisplay */ true,
		/* render cache */ Font::TextCached ) ;

	  myTextCachedTrueTypeFont.load( /* point size */ 28 ),

		LogPlug::info(
		  "Successfully loaded following font in initial state: "
		  + myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  Coordinate ordinate = 200 ;

	  mySurface = & myTextCachedTrueTypeFont.renderLatin1Text(
		firstTestSentence, Font::Solid, textColor ) ;

	  mySurface->blitTo( screen, 20, ordinate ) ;
	  delete mySurface ;

	  LogPlug::info( "Font after first render of first test sentence: "
		+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  mySurface = & myTextCachedTrueTypeFont.renderLatin1Text(
		firstTestSentence, Font::Solid, textColor ) ;

	  mySurface->blitTo( screen, 220, ordinate ) ;
	  delete mySurface ;

	  LogPlug::info( "Font after second render of first test sentence: "
		+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  mySurface = & myTextCachedTrueTypeFont.renderLatin1Text(
		firstTestSentence, Font::Solid, textColor ) ;

	  mySurface->blitTo( screen, 420, ordinate ) ;
	  delete mySurface ;

	  LogPlug::info(
		"Font after third render of first test sentence being "
		+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;


	  ordinate = 250 ;

	  myTextCachedTrueTypeFont.blitLatin1Text( screen, 20, ordinate,
		secondTestSentence, Font::Solid, textColor ) ;

	  LogPlug::info( "Font after first blit of second test sentence: "
		+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  myTextCachedTrueTypeFont.blitLatin1Text( screen, 220, ordinate,
		secondTestSentence, Font::Solid, textColor ) ;

	  LogPlug::info( "Font after first blit of second test sentence: "
		+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;

	  myTextCachedTrueTypeFont.blitLatin1Text( screen, 420, ordinate,
		secondTestSentence, Font::Solid, textColor ) ;

	  LogPlug::info( "Font after first blit of second test sentence: "
		+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;


	}


	screen.update() ;

	if ( ! isBatch )
	  myOSDL.getEventsModule().waitForAnyKey() ;


	LogPlug::info( "Stopping OSDL." ) ;
	OSDL::stop() ;


	LogPlug::info( "End of OSDL TrueType font cache test." ) ;

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

  OSDL::shutdown() ;

  return Ceylan::ExitSuccess ;

}

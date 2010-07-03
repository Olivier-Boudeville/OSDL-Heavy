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
using namespace OSDL::Video::Pixels ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::TwoDimensional::Text ;


using namespace Ceylan::Log ;


#include <string>
using std::string ;


#define OSDL_TEST_WIDGET_DEBUG 0

#if OSDL_TEST_WIDGET_DEBUG

#include <iostream>
#define CHECKPOINT(message) std::cout << "[testOSDLTextWidget] " << message << std::endl ;

#else // OSDL_TEST_WIDGET_DEBUG

#define CHECKPOINT(message)

#endif // OSDL_TEST_WIDGET_DEBUG



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



const std::string firstTrueTypeFontName  = "stilltim.ttf" ;
const std::string secondTrueTypeFontName = "cretino.ttf" ;
const std::string thirdTrueTypeFontName  = "earwigfa.ttf" ;
//const std::string thirdTrueTypeFontName  = "neurochr.ttf" ;



/*
 * Fixed font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in build tree:
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
 * Fixed font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in installed tree:
 *
 */
const std::string fixedFontDirForInstalledPlayTests
= "../OSDL/doc/web/common/fonts" ;





/**
 * Small usage tests for text widgets.
 *
 */
int main( int argc, char * argv[] )
{

  LogHolder myLog( argc, argv ) ;

  // SDL_gfx might be disabled, in that case no putPixel will be available:
  if ( ! VideoModule::IsUsingDrawingPrimitives() )
  {

	LogPlug::warning( "SDL_gfx is disabled, thus no fixed font will be "
	  "available, stopping test now." ) ;

	return Ceylan::ExitSuccess ;

  }

 Font::RenderQuality chosenQuality = Font::Blended ;


  try
  {


	LogPlug::info( "Testing OSDL Text widget" ) ;


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

	CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo
	  | CommonModule::UseKeyboard ) ;

	VideoModule & myVideo = myOSDL.getVideoModule() ;

	Length screenWidth  = 640 ;
	Length screenHeight = 480 ;

	CHECKPOINT( "Before setMode." ) ;
	myVideo.setMode( screenWidth, screenHeight,
	  VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

	CHECKPOINT( "Retrieving screen surface." ) ;
	Surface & screen = myVideo.getScreenSurface() ;

	LogPlug::info( "Drawing text widget." ) ;


	LogPlug::info( "Preparing TrueType font." ) ;

	Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
	  trueTypeFontDirFromExec ) ;

	Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
	  trueTypeFontDirForBuildPlayTests ) ;

	Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
	  trueTypeFontDirForInstalledPlayTests ) ;


	std::string firstTrueTypeFontPath = Text::TrueTypeFont::FindPathFor(
	  firstTrueTypeFontName ) ;

	Text::TrueTypeFont myFirstTrueTypeFont( firstTrueTypeFontPath,
	  /* index */ 0,
	  /* convertToDisplay */ true,
	  /* render cache */ Text::Font::WordCached ) ;

	myFirstTrueTypeFont.load( /* point size */ 26 ) ;


	std::string secondTrueTypeFontPath = Text::TrueTypeFont::FindPathFor(
	  secondTrueTypeFontName ) ;

	Text::TrueTypeFont mySecondTrueTypeFont( secondTrueTypeFontPath,
	  /* index */ 0,
	  /* convertToDisplay */ true,
	  /* render cache */ Text::Font::WordCached ) ;

	mySecondTrueTypeFont.load( /* point size */ 30 ) ;


	std::string thirdTrueTypeFontPath = Text::TrueTypeFont::FindPathFor(
	  thirdTrueTypeFontName ) ;

	Text::TrueTypeFont myThirdTrueTypeFont( thirdTrueTypeFontPath,
	  /* index */ 0,
	  /* convertToDisplay */ true,
	  /* render cache */ Text::Font::WordCached ) ;

	myThirdTrueTypeFont.load( /* point size */ 30 ) ;

	LogPlug::info( "Preparing Fixed font." ) ;


	Text::FixedFont::FixedFontFileLocator.addPath(
	  fixedFontDirFromExec ) ;

	Text::FixedFont::FixedFontFileLocator.addPath(
	  fixedFontDirForBuildPlayTests ) ;

	Text::FixedFont::FixedFontFileLocator.addPath(
	  fixedFontDirForInstalledPlayTests ) ;


	Text::FixedFont myFixedFont( 9, 18,
	  /* renderingStyle */ Text::Font::Bold,
	  /* convertToDisplay */ true, /* render cache */
	  Text::Font::WordCached ) ;


	const string myFirstText =
	  "This is an example of a text widget with square corners. "
	  "Dare to be naive. Zombie processes detected, machine is haunted. "
	  "End of paragraph.\n"
	  "Happiness is twin floppies. Intel CPUs are not defective, "
	  "they just act that way. "
	  "Have you reconsidered a computer career ? "
	  "People who take cold baths never have rheumatism, "
	  "but they have cold baths. Another paragraph hits the dust.\n"
	  "I recognize terror as the finest emotion and so I will try to "
	  "terrorize the reader. "
	  "But if I find that I cannot terrify, I will try to horrify, "
	  "and if I find that I cannot horrify, I'll go for the gross-out "
	  "(Stephen King)." ;

	/*
	 * Very long words could be tested as well, but it would lead to partial
	 * renderings (as expected by the API).
	 *
	 "Testing long words: "
	 "Happinessistwinfloppies.IntelCPUsarenotdefective,"
	 "theyjustactthatway. "
	 "YouwillvisittheDungPitsofGlivesoon. "
	 *
	 */

	const string mySecondText =
	  "This is an example of a text widget with round corners. "
	  "Never trust an operating system. "
	  "You know you're using the computer too much when "
	  "you emerge packages just to watch the compile messages fly "
	  "by on the screen...\n"
	  "You will visit the Dung Pits of Glive soon. "
	  "If people could put rainbows in zoos, they'd do it." ;

	LogPlug::info( "Before adding text widget, screen surface is: "
	  + screen.toString() ) ;

	CHECKPOINT( "Creating first widget." ) ;


	// Will be deallocated by container (screen) on explicit removal:
	new TextWidget( screen,
	  Point2D( (Coordinate) 10, 20 ),
	  /* width */ 400,
	  /* height */ 450,
	  /* shape */ TextWidget::SquareCorners,
	  /* text color */ Pixels::Green,
	  /* edge color */ Pixels::CadetBlue,
	  /* back color */ Pixels::Purple,
	  myFirstText,
	  myFirstTrueTypeFont,
	  /* minimum height */ true,
	  /* vertically aligned */ true,
	  /* justified */ true,
	  /* quality */ chosenQuality,
	  /* title */ "TrueType font test" ) ;


	CHECKPOINT( "Creating second widget." ) ;


	// Made to overlap:

	/*
	 * Automatic registering to container (screen), which would remove
	 * it automatically, but, for example purpose here, the pointer is
	 * kept so that the deallocation is triggered explicitly in user code.
	 * As this instance count should show, both types of deletion should
	 * work.
	 *
	 */
	new TextWidget( screen,
	  Point2D( (Coordinate) 330, 220 ),
	  /* width */ 300,
	  /* height  */ 240,
	  /* shape */ TextWidget::SquareCorners,
	  /* text color */ Pixels::Brown,
	  /* edge color */ Pixels::Sienna,
	  /* back color */ Pixels::ForestGreen,
	  mySecondText,
	  myFixedFont,
	  /* minimum height */ true,
	  /* vertically aligned */ true,
	  /* justified */ true,
	  /* quality */ chosenQuality,
	  /* title */ "Fixed font test" ) ;

	CHECKPOINT( "Creating third widget (non text)." ) ;


	// Test a basic non-decorated widget:
	Widget * theWidgetIChoseToDeleteMyself = new Widget( screen,
	  Point2D( (Coordinate) 220, 250 ),
	  /* width */ 50,
	  /* height */ 100,
	  /* baseColorMode */ Widget::BackgroundColor,
	  /* background color */ Pixels::Blue,
	  /* title */ "",
	  /* minMaximizable */ false, /* draggable */ false,
	  /* wrappable */ false, 	/* closable */ false ) ;

	LogPlug::info( "After adding text widget, screen surface is: "
	  + screen.toString() ) ;

	screen.lock() ;
	screen.redraw() ;
	screen.unlock() ;

	screen.update() ;

	screen.savePNG( argv[0] + std::string( "-simple.png" ) ) ;

	if ( ! isBatch )
	  myOSDL.getEventsModule().waitForAnyKey() ;

	/*
	 * If you do not want this widget to show up in second screen just
	 * delete it:
	 *
	 */
	delete theWidgetIChoseToDeleteMyself ;

	screen.clear() ;

	std::string currentText = "A short sentence. " ;

	// Uncomment this to check that too long word are correctly managed:
	/*
	 * currentText +=
	 " Atoolongwordtofitinthetextcontaineryesmylorditisverylong" ;
	 *
	 */

	CHECKPOINT( "Creating fourth widget." ) ;


	new TextWidget( screen,
	  Point2D( (Coordinate) 50, 100 ),
	  /* width */ 400,
	  /* height */ 300,
	  /* shape */ TextWidget::SquareCorners,
	  /* text color */ Pixels::Yellow,
	  /* edge color */ Pixels::Pink,
	  /* back color */ Pixels::Brown,
	  currentText,
	  mySecondTrueTypeFont,
	  /* minimum height */ true,
	  /* vertically aligned */ true,
	  /* justified */ true,
	  /* quality */ chosenQuality,
	  /* title */ "I want a caption" ) ;

	currentText += "Longer one. More, so that there are two lines." ;

	CHECKPOINT( "Creating fifth widget." ) ;


	new TextWidget( screen,
	  Point2D( (Coordinate) 50, 170 ),
	  /* width */ 400,
	  /* height */ 300,
	  /* shape */ TextWidget::SquareCorners,
	  /* text color */ Pixels::Yellow,
	  /* edge color */ Pixels::Pink,
	  /* back color */ Pixels::Brown,
	  currentText,
	  mySecondTrueTypeFont,
	  /* minimum height */ true,
	  /* vertically aligned */ true,
	  /* justified */ true,
	  /* quality */ chosenQuality,
	  /* title */ "" ) ;

	currentText +=
	  " Still a longer one, hoping a total of at least three lines." ;

	CHECKPOINT( "Creating sixth widget." ) ;


	new TextWidget( screen,
	  Point2D( (Coordinate) 50, 280 ),
	  /* width */ 400,
	  /* height */ 300,
	  /* shape */ TextWidget::SquareCorners,
	  /* text color */ Pixels::Yellow,
	  /* edge color */ Pixels::Pink,
	  /* back color */ Pixels::Brown,
	  currentText,
	  mySecondTrueTypeFont,
	  /* minimum height */ true,
	  /* vertically aligned */ true,
	  /* justified */ true,
	  /* quality */ chosenQuality,
	  /* title */ "" ) ;


	new TextWidget( screen,
	  Point2D( (Coordinate) 300, 30 ),
	  /* width */ 335,
	  /* height */ 300,
	  /* shape */ TextWidget::RoundCorners,
	  /* text color */ Pixels::MintCream,
	  /* edge color */ Pixels::MintCream,
	  /* back color */ Pixels::DarkSlateBlue,
	  "This is a hello world message, from a beautiful text widget "
	  "with splendid round corners, as anyone can see !",
	  myThirdTrueTypeFont,
	  /* minimum height */ true,
	  /* vertically aligned */ true,
	  /* justified */ true,
	  /* quality */ chosenQuality,
	  /* title */ "" ) ;


	screen.lock() ;
	screen.redraw() ;
	screen.unlock() ;

	screen.update() ;

	screen.savePNG( argv[0] + std::string( ".png" ) ) ;

	if ( ! isBatch )
	  myOSDL.getEventsModule().waitForAnyKey() ;


	CHECKPOINT( "Before stopping OSDL " + Ceylan::Countable::ToString() ) ;

	LogPlug::info( "Stopping OSDL." ) ;
	OSDL::stop() ;

	CHECKPOINT( "After having stopped OSDL "
	  + Ceylan::Countable::ToString() ) ;

	LogPlug::info( "End of OSDL Text widget test." ) ;


	// Fonts are deallocated automatically.

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

  CHECKPOINT( "Just before exiting: " + Ceylan::Countable::ToString() ) ;

  return Ceylan::ExitSuccess ;

}

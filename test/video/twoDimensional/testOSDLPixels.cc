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
 * Small usage tests for low level pixel management.
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

  bool randomTest   = true ;
  bool fewPixelTest = true ;


  try
  {


	LogPlug::info( "Testing OSDL pixel management." ) ;


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
	  CommonModule::UseVideo | CommonModule::UseEvents ) ;

	VideoModule & myVideo = myOSDL.getVideoModule() ;

	Length screenWidth  = 640 ;
	Length screenHeight = 480 ;

	myVideo.setMode( screenWidth, screenHeight,
	  VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

	Surface & screen = myVideo.getScreenSurface() ;


	if ( randomTest	)
	{

	  LogPlug::info( "Prerequisite: having three random generators" ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand(
		0, screenWidth ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand(
		0, screenHeight ) ;

	  Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;


	  LogPlug::info( "Drawing random pixels with random colors" ) ;

	  screen.lock() ;

	  Coordinate abscissa ;
	  Coordinate ordinate ;

	  ColorElement red ;
	  ColorElement green ;
	  ColorElement blue ;
	  ColorElement alpha ;


	  LogPlug::info( "Displaying pixel format: "
		+ Pixels::toString( screen.getPixelFormat() ) ) ;


	  for ( Ceylan::Uint32 i = 0; i < 50; i++ )
	  {

		for ( Ceylan::Uint32 j = 0; j < 5000; j++ )
		{

		  abscissa = abscissaRand.getNewValue() ;
		  ordinate = ordinateRand.getNewValue() ;

		  red   = colorRand.getNewValue() ;
		  green = colorRand.getNewValue() ;
		  blue  = colorRand.getNewValue() ;
		  alpha = colorRand.getNewValue() ;

		  screen.putRGBAPixelAt( abscissa, ordinate,
			red, green, blue, alpha ) ;

		}

		// Avoid having too many logs (300 000 would be too much !)
		LogPlug::info( "Putting at [ "
		  + Ceylan::toString( abscissa ) + " ; "
		  + Ceylan::toString( ordinate )
		  + " ] pixel [R;G;B;A] = [ "
		  + Ceylan::toNumericalString( red )	+ " ; "
		  + Ceylan::toNumericalString( green ) + " ; "
		  + Ceylan::toNumericalString( blue )  + " ; "
		  + Ceylan::toNumericalString( alpha ) + " ]" ) ;

		LogPlug::info( "Reading afterwards: actual pixel color is "
		  + Pixels::toString(
			screen.getColorDefinitionAt( abscissa, ordinate ) ) ) ;

	  }

	  screen.unlock() ;

	  screen.update() ;

	  if ( ! isBatch )
		myOSDL.getEventsModule().waitForAnyKey() ;

	}


	if ( fewPixelTest )
	{


	  LogPlug::info(
		"Drawing a few colored pixels to allow fine log checking" ) ;

	  screen.lock() ;

	  // DodgerBlue = {  30, 144, 255, 255 }
	  ColorDefinition firstColorDef = Pixels::DodgerBlue ;

	  PixelColor firstPixelColor =
		Pixels::convertColorDefinitionToPixelColor(
		  screen.getPixelFormat(), firstColorDef ) ;

	  LogPlug::info( "First color, " + Pixels::toString( firstColorDef )
		+ ", converted to screen pixel format "
		"and then de-converted, displays as: "
		+ Pixels::toString( Pixels::convertPixelColorToColorDefinition(
			screen.getPixelFormat(), firstPixelColor ) )
		+ "." ) ;

	  // Reddish:
	  ColorElement red   = 249 ;
	  ColorElement green =   2 ;
	  ColorElement blue  =  14 ;
	  ColorElement alpha = 130 ;

	  ColorDefinition secondColorDef =
		Pixels::convertRGBAToColorDefinition(
		  red, green, blue, alpha ) ;

	  if ( Pixels::areEqual( secondColorDef,
		  Pixels::convertRGBAToColorDefinition(
			red, green, blue, alpha ) ) )
	  {

		LogPlug::info( "Small test for basic two-way conversion "
		  "between RGBA and color definition succeeded." ) ;
	  }
	  else
	  {
		throw Ceylan::TestException( "This color definition "
		  + Pixels::toString( secondColorDef )
		  + " should be strictly equal to this RGBA coordinates: [ "
		  + Ceylan::toNumericalString( red   ) + " ; "
		  + Ceylan::toNumericalString( green ) + " ; "
		  + Ceylan::toNumericalString( blue  ) + " ; "
		  + Ceylan::toNumericalString( alpha ) + " ]." ) ;
	  }


	  PixelColor secondPixelColor =
		Pixels::convertColorDefinitionToPixelColor(
		  screen.getPixelFormat(), secondColorDef ) ;

	  LogPlug::info( "Second color, "
		+ Pixels::toString( secondColorDef )
		+ ", converted to pixel format and then de-converted, "
		"displays as: "
		+ Pixels::toString( Pixels::convertPixelColorToColorDefinition(
			screen.getPixelFormat(), secondPixelColor ) ) + "." ) ;

	  LogPlug::info( "Putting onscreen both pixels." ) ;

	  screen.putPixelColorAt( 10, 10, firstPixelColor,
		firstColorDef.unused ) ;

	  screen.putPixelColorAt( 20, 20, secondPixelColor,
		secondColorDef.unused ) ;

	  LogPlug::info( "Reading both pixels from screen." ) ;

	  LogPlug::info( "First pixel displays as "
		+ screen.describePixelAt( 10, 10 ) ) ;

	  LogPlug::info( "Second pixel displays as "
		+ screen.describePixelAt( 20, 20 )
		+ " Alpha cannot match since the screen surface "
		"has no alpha coordinate, in this case AlphaOpaque "
		"should be returned instead. Moreover, this second color "
		"is not totally opaque, hence is alphablended by "
		"the pure black background, leading to a darker color." ) ;


	  screen.unlock() ;

	  screen.update() ;

	  if ( ! isBatch )
		myOSDL.getEventsModule().waitForAnyKey() ;

	}


	LogPlug::info( "Stopping OSDL." ) ;
	OSDL::stop() ;


	LogPlug::info( "End of OSDL pixel management test." ) ;

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

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
using namespace OSDL::Audio ;


using namespace Ceylan::Log ;



/**
 * Testing the basic services of the OSDL audio module.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	try
	{


	  LogPlug::info( "Testing OSDL audio basic services." ) ;


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


	  LogPlug::info( "Starting OSDL with audio enabled." ) ;

	  CommonModule & myOSDL = getCommonModule( CommonModule::UseAudio ) ;

	  myOSDL.logState() ;


	  LogPlug::info( "Getting audio module." ) ;
	  AudioModule & myAudio = myOSDL.getAudioModule() ;

	  myAudio.logState() ;


	  LogPlug::info( "Displaying audio driver name: "
		+ myAudio.getDriverName() + "." ) ;

	  Ceylan::Maths::Hertz outputFrequency = 22050 ;

	  SampleFormat outputSampleFormat =
		AudioModule::NativeSint16SampleFormat ;

	  ChannelFormat outputChannel = AudioModule::Stereo ;

	  ChunkSize chunkSize = 4096 ;

	  ChannelNumber mixingChannelNumber = 16 ;

	  LogPlug::info( "Requesting a mixing mode at "
		+ Ceylan::toString( outputFrequency ) + " Hz, with sample format "
		+ sampleFormatToString( outputSampleFormat )
		+ ", channel format "
		+ channelFormatToString( outputChannel )
		+ ", a chunk size of " + Ceylan::toString( chunkSize )
		+ " bytes and " + Ceylan::toString( mixingChannelNumber )
		+ " input (logical) mixing channels." ) ;


	  myAudio.setMode( outputFrequency, outputSampleFormat, outputChannel,
		chunkSize, mixingChannelNumber ) ;

	  myAudio.logState() ;

	  ChannelNumber count ;

	  Ceylan::System::Millisecond latency = myAudio.getObtainedMode(
		outputFrequency, outputSampleFormat, count ) ;

	  /*
	   * An exception would have been thrown otherwise, and specified variables
	   * will have their values updated by the call:
	   *
	   */
	  LogPlug::info( "Obtained a mixing mode at "
		+ Ceylan::toString( outputFrequency ) + " Hz, with sample format "
		+ sampleFormatToString( outputSampleFormat ) + " and "
		+ Ceylan::toString( count ) + " output channel(s), "
		"which results in a mean theoritical latency of "
		+ Ceylan::toString( latency ) + " milliseconds." ) ;


	  /*
		if ( ! isBatch )
		myEvents.waitForAnyKey() ;
	  */

	  LogPlug::info( "Stopping OSDL." ) ;
	  OSDL::stop() ;

	  LogPlug::info( "End of OSDL audio test." ) ;

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

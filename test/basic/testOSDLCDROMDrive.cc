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

using namespace Ceylan::Log ;

using namespace std ;



/**
 * Test of CD-ROM drive management.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	try
	{


	  LogPlug::info( "Testing OSDL CD-ROM services." ) ;


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

	  OSDL::CommonModule & myOSDL = OSDL::getCommonModule(
		CommonModule::UseCDROM ) ;

	  LogPlug::info( "Informations about common module: "
		+ myOSDL.toString() ) ;

	  CDROMDriveHandler & driveHandler = myOSDL.getCDROMDriveHandler() ;

	  LogPlug::info( "CD-ROM drive count: "
		+ Ceylan::toString(
		  CDROMDriveHandler::GetAvailableCDROMDrivesCount() ) ) ;

	  LogPlug::info( "Before requesting any drive: "
		+ driveHandler.toString() ) ;

	  list<string> insertedMedia = CDROMDrive::GetListOfInsertedMedia() ;

	  LogPlug::info ( "Detected inserted media: "
		+ Ceylan::formatStringList( insertedMedia ) ) ;


	  LogPlug::info( "Controlling all available CD-ROM drives." ) ;

	  CDROMDriveNumber driveNumber =
		CDROMDriveHandler::GetAvailableCDROMDrivesCount() ;


	  // Nothing more to test?
	  if ( driveNumber == 0 )
	  {

		LogPlug::info( "No drive available, stopping test." ) ;

		OSDL::stop() ;

		OSDL::shutdown() ;

		return Ceylan::ExitSuccess ;

	  }

	  // Open all available drives:
	  for ( CDROMDriveNumber i = 0; i < driveNumber; i++ )
		driveHandler.getCDROMDrive( i ) ;

	  LogPlug::info( "After having used all drives: "
		+ driveHandler.toString() ) ;

	  CDROMDrive & defaultDrive = driveHandler.getCDROMDrive( 0 ) ;

	  defaultDrive.open() ;

	  if ( ! defaultDrive.isCDInDrive() )
	  {

		LogPlug::info(
		  "Drive available but no CD in tray, stopping test." ) ;

		OSDL::stop() ;

		OSDL::shutdown() ;

		return Ceylan::ExitSuccess ;

	  }

	  /*
	   * A drive, a CD, let's attempt to play audio tracks (if any and if not in
	   * batch mode):
	   *
	   */

	  if ( ! isBatch )
	  {

		// Plays 'testDuration' seconds of each track:
		Ceylan::System::Second testDuration = 10 ;

		TrackNumber trackCount = defaultDrive.getTracksCount() ;

		for ( TrackNumber i = 0; i < trackCount; i++ )
		{

		  CDTrack * currentTrack = & defaultDrive.getTrack( i ) ;
		  LogPlug::info( "Current track is: "
			+ currentTrack->toString() ) ;

		  /*
		   * Not all configurations are able to play music directly from the
		   * CD-ROM drive to the soundcard (if any): usually there must be a
		   * special cable between them.
		   *
		   */
		  defaultDrive.playTracks( i, /* number of tracks */ 0,
			/* starting offset */ 0, /* ending frame */
			CDROMDrive::ConvertTimeToFrameCount(
			  testDuration /* seconds */ ) ) ;

		  Ceylan::System::sleepForSeconds( 10 ) ;

		  delete currentTrack ;

		}

		// Too annoying for the user:
		// defaultDrive.eject() ;

	  }


	  OSDL::stop() ;

	  LogPlug::info( "End of OSDL CD-ROM test." ) ;

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

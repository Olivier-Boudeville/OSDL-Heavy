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

#include <iostream>

#include <string>
using std::string ;

#include <list>
using std::list ;

#include <exception>


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL ;
using namespace OSDL::Audio ;




/**
 * Test for the support of music (MP3-encoded) offered by the OSDL library on
 * the Nintendo DS.
 *
 */
int main( int argc, char * argv[] )
{


	LogHolder myLog( argc, argv ) ;


	try
	{


		// For the test:
		bool interactive = true ;
		//bool interactive = false ;

		bool testFailed = false ;

		LogPlug::info( "Test of OSDL support for MP3 music output." ) ;


		CommonModule & myOSDL = OSDL::getCommonModule(
			CommonModule::UseAudio ) ;

		LogPlug::info( "OSDL state: " + myOSDL.toString() ) ;


		// Always created automatically on the DS:
		CommandManager & myCommandManager =
			CommandManager::GetExistingCommandManager() ;


		LogPlug::info( "Current ARM7 status just after OSDL activation is: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		const string musicFilename = "test.osdl.music" ;

		LogPlug::info( "Creating test music instance from the '"
			+ musicFilename + "' file." ) ;

		Music testMusic( musicFilename ) ;

		LogPlug::info( "Sending to the ARM7 a command request to play it." ) ;

		// Default: play once.
		testMusic.play() ;

		/*
		 * The main loop has to refill buffers when appropriate, as it should
		 * not be done in IRQ handlers.
		 *
		 */


		LogPlug::info( "Refilling now buffers while playing." ) ;


		while ( testMusic.isPlaying() )
		{

			Music::ManageCurrentMusic() ;
			atomicSleep() ;

		}

		LogPlug::info( "Current ARM7 status after first playback: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		if ( interactive )
		{

			LogPlug::info( "First playback ended, "
				"press any key to play it again with a fade-in." ) ;
			waitForKey() ;

		}

		LogPlug::info( "Playing music again, once, with a 2-second fade-in." ) ;
		testMusic.playWithFadeIn( 2000 /* milliseconds */ ) ;

		while ( testMusic.isPlaying() )
		{

			Music::ManageCurrentMusic() ;
			atomicSleep() ;

		}

		LogPlug::info( "Current ARM7 status after second playback: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		if ( interactive )
		{

			LogPlug::info( "Second playback ended, "
				"press any key to play it twice in a row." ) ;
			waitForKey() ;

		}


		LogPlug::info( "Now playing that music twice." ) ;
		testMusic.play( 2 ) ;

		while ( testMusic.isPlaying() )
		{

			Music::ManageCurrentMusic() ;
			atomicSleep() ;

		}

		LogPlug::info( "Current ARM7 status after third (double) playback: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		if ( interactive )
		{


			LogPlug::info( "Third playback ended, press any key to continue "
				"with pause/unpause" ) ;
			waitForKey() ;

		}


		LogPlug::info( "Now playing once, pausing after one second, "
			"waiting five seconds, then unpausing." ) ;
		testMusic.play( 1 ) ;


		Ceylan::Uint16 VBICount = 60 ;

		while ( testMusic.isPlaying() && VBICount-- > 0 )
		{

			Music::ManageCurrentMusic() ;
			atomicSleep() ;

		}

		testMusic.pause() ;
		LogPlug::info( "Paused ! Sleeping 4 seconds..." ) ;
		Ceylan::System::sleepForSeconds( 4 ) ;
		testMusic.unpause() ;
		LogPlug::info( "...unpaused !" ) ;

		while ( testMusic.isPlaying() )
		{

			Music::ManageCurrentMusic() ;
			atomicSleep() ;

		}

		LogPlug::info(
			"Current ARM7 status after fourth (paused/unpaused) playback: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		if ( interactive )
		{

			LogPlug::info(
				"Fourth playback ended, press any key to play and stop." ) ;
			waitForKey() ;

		}


		LogPlug::info( "Now playing once, stopping music after one second." ) ;
		testMusic.play( 1 ) ;
		VBICount = 60 ;

		while ( testMusic.isPlaying() && VBICount-- > 0 )
		{

			Music::ManageCurrentMusic() ;
			atomicSleep() ;

		}

		LogPlug::info( "Stopping music." ) ;
		testMusic.stop() ;

		LogPlug::info(
			"Current ARM7 status after fifth (stopped) playback: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		if ( interactive )
		{

			LogPlug::info(
				"Fifth playback ended, press any key to test fade-out." ) ;
			waitForKey() ;

		}


		LogPlug::info( "Now playing once, fading-out music after a half second "
			"and for 2 seconds." ) ;
		testMusic.play( 1 ) ;
		VBICount = 30 ;

		while ( testMusic.isPlaying() && VBICount-- > 0 )
		{

			Music::ManageCurrentMusic() ;
			atomicSleep() ;

		}

		LogPlug::info( "Requesting the fade-out." ) ;
		testMusic.fadeOut( 2000 /* milliseconds */ ) ;

		while ( testMusic.isPlaying() )
		{

			Music::ManageCurrentMusic() ;
			atomicSleep() ;

		}

		LogPlug::info(
			"Current ARM7 status after sixth (faded-out) playback: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		LogPlug::info(
			"Setting volume to maximum, now that fade-out is over "
			"(new volume is "
			+ Ceylan::toNumericalString( Audio::MaxVolume ) + ")." ) ;
		testMusic.setVolume( Audio::MaxVolume ) ;

		if ( interactive )
		{

			LogPlug::info(
				"Sixth playback ended, press any key to play in a loop." ) ;
			waitForKey() ;

		}

		LogPlug::info( "Now playing in a loop, for ever "
			"(clamped to 30s)." ) ;

		testMusic.play( Loop ) ;

		VBICount = 60 * 30 ;

		while ( testMusic.isPlaying() && VBICount-- > 0 )
		{

			Music::ManageCurrentMusic() ;
			atomicSleep() ;

		}

		LogPlug::info( "Stopping now infinite playback." ) ;
		testMusic.stop() ;

		if ( interactive )
		{

			LogPlug::info( "Press any key to stop waiting" ) ;
			waitForKey() ;

		}


		if ( testFailed )
			throw OSDL::TestException( "Test failed because of error(s) "
				"previously displayed." ) ;

		if ( interactive )
		{

			LogPlug::info( "Press any key to end the test" ) ;
			waitForKey() ;

		}

		// LogHolder out of scope: log browser triggered.

	}

	catch ( const Ceylan::Exception & e )
	{

		LogPlug::error( "Ceylan exception caught: "
			+ e.toString( Ceylan::high ) ) ;

		return Ceylan::ExitFailure ;

	}

	catch ( const std::exception & e )
	{

		LogPlug::error( string( "Standard exception caught: " ) + e.what() ) ;
		return Ceylan::ExitFailure ;

	}

	catch ( ... )
	{

		LogPlug::error( "Unknown exception caught" ) ;
		return Ceylan::ExitFailure ;

	}

	LogPlug::info( "Exit on success (no error)" ) ;

	OSDL::shutdown() ;

	return Ceylan::ExitSuccess ;

}

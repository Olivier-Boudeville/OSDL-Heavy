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
 * Test for the support of music (MP3-encoded) offered by the OSDL library
 * on the Nintendo DS.
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
		
			LogPlug::info( "First playback ended, press any key to continue" ) ;
			waitForKey() ;
		
		}
		
		LogPlug::info( "Playing music again, once." ) ;
		testMusic.play() ;

		while ( testMusic.isPlaying() )
		{
		
			Music::ManageCurrentMusic() ;
			atomicSleep() ;
		
		}
		
		LogPlug::info( "Current ARM7 status after second playback: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		if ( interactive )
		{
		
			LogPlug::info( 
				"Second playback ended, press any key to continue" ) ;
			waitForKey() ;
		
		}

		return 0 ;
		
		
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
		

			LogPlug::info( "Third playback ended, press any key to continue" ) ;
			waitForKey() ;
		
		}
		
		
		LogPlug::info( "Now playing once, pausing after one second, "
			"waiting two seconds, then unpausing." ) ;
		testMusic.play( 1 ) ;
		
		
		Ceylan::Uint16 VBICount = 60 ;
		
		while ( testMusic.isPlaying() && VBICount-- > 0 )
		{
		
			Music::ManageCurrentMusic() ;
			atomicSleep() ;
		
		}
		
		testMusic.pause() ;
		LogPlug::info( "Paused ! Sleeping 2s..." ) ;
		Ceylan::System::sleepForSeconds( 2 ) ;
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
				"Fourth playback ended, press any key to continue" ) ;
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
				"Fifth playback ended, press any key to continue" ) ;
			waitForKey() ;
		
		}
		
		LogPlug::info( "Now playing in a loop, for ever "
			"(clamped to 1 minute)." ) ;
		testMusic.play( Loop ) ;

		VBICount = 60 ;
		
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
	
    return Ceylan::ExitSuccess ;

}


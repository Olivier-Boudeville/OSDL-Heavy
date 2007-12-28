#include "OSDL.h"

#include "Ceylan.h"

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
 * Test for the support of raw sound (non-encoded) offered by the OSDL library
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
		
		
		LogPlug::info( "Test of OSDL support for raw sound output" ) ;		
	
		
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseAudio ) ;	

		LogPlug::info( "OSDL state: " + myOSDL.toString() ) ;


		CommandManager & myCommandManager = 
			CommandManager::GetExistingCommandManager() ;
			
		LogPlug::info( "Current ARM7 status just after OSDL activation is: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		const string soundFilename = "OSDL.osdl.sound" ;
		
		LogPlug::info( "Creating test sound instance from the '"
			+ soundFilename + "' file." ) ;
			
		Sound testSound( soundFilename, /* preload */ false ) ;
		
		LogPlug::info( "Loading its data." ) ;
		testSound.load() ;
		
		LogPlug::info( "Sending to the ARM7 a command request to play it." ) ;
		myCommandManager.playSound( testSound ) ;
			
		
		if ( interactive )
		{
		
			LogPlug::info( "Press any key to stop waiting" ) ;
			waitForKey() ;
		
		}
		
		LogPlug::info( "Current ARM7 status just after OSDL activation is: "
			 + myCommandManager.interpretLastARM7StatusWord() ) ;

		bool testFailed = false ;		
		
							
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

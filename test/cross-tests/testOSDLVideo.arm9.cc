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
 * Test for the support of video offered by the OSDL library on the Nintendo DS.
 *
 */
int main( int argc, char * argv[] )
{

	// To avoid using a screen for the logs:
	LogHolder myLog( argc, argv, /* immediateWrite */ false ) ;

	
    try
    {
				
		
		// For the test:
		bool interactive = true ;
		//bool interactive = false ;
		
		bool testFailed = false ;		
		
		LogPlug::info( "Test of OSDL support for video." ) ;		
	
		
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo ) ;	

		LogPlug::info( "OSDL state: " + myOSDL.toString() ) ;

		//LogPlug::info( "Loading 
		

										
		
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


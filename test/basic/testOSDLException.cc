#include "OSDL.h"
using namespace OSDL ;

using namespace Ceylan::Log ;




/**
 * Test of TestException throwing and catching.
 *
 */


int main( int argc, char * argv[] ) 
{    

	LogHolder myLog( argc, argv ) ;
	
	try 
	{

		LogPlug::info( "Testing OSDL exception handling" ) ;

		throw OSDL::TestException( 
			"This exception has been explicitly raised." ) ;
		
		
		// Never reached :
		
		LogPlug::fatal( "Failed raising exception." ) ;
		return Ceylan::ExitFailure ;
			
	}
	
    catch ( const OSDL::Exception & e )
    {
	
        LogPlug::info( "End of OSDL exception handling test." ) ;	
					
		// This is a success, from this test point of view.	 
        return Ceylan::ExitSuccess ;

    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught : " 
			 + std::string( e.what() ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
        LogPlug::error( "Unknown exception caught" ) ;
       	return Ceylan::ExitFailure ;

    }

	// Exception missed :
    return Ceylan::ExitFailure ;

}


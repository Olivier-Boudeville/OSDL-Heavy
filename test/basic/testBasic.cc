#include "Ceylan.h"
using namespace Ceylan::Log ;


#include "OSDL.h"
using namespace OSDL ;



/**
 * Test for the basic module.
 *
 * Basically useful to check library linking works.
 *
 */


int main( int argc, const char * const argv[] )
{

    try
    {
	
		LogPlugClassical::startService( argv[ 0 ] ) ;
		
        LogPlug::info( "Testing some of the basic services." ) ;

		CommonModule & myModule = getCommonModule( static_cast<Flags>( 1 ) ) ; 
		
		LogPlug::info( myModule.toString() ) ;
		
        LogPlug::info( "End of basic test." ) ;


    }
	
    catch ( const OSDL::Exception & e )
    {
        LogPlug::error( "OSDL exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

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

    return Ceylan::ExitSuccess ;

}


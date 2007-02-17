#include "OSDL.h"
using namespace OSDL ;

using namespace Ceylan::Log ;



/**
 * Test for the basic module.
 *
 * Basically useful to check whether library linking works.
 *
 */
int main( int argc, char * argv[] )
{


	LogHolder myLog( argc, argv ) ;

    try
    {
			
        LogPlug::info( "Testing some of the basic services." ) ;

		CommonModule & myModule = getCommonModule( CommonModule::UseVideo ) ; 
		
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


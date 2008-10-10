#include "OSDL.h"
using namespace OSDL ;

using namespace Ceylan::Log ;



/**
 * Test for some utilities provided by the OSDL basic module.
 *
 * @note Corresponds to the test of OSDLUtils.h API.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	

    try 
	{

    	LogPlug::info( "Testing OSDL utility services." ) ;
	
		// No need to start OSDL services for them:
		LogPlug::info( "Native directory separator is: "
        	+ Ceylan::toString( Utils::getNativeDirectorySeparator() ) ) ;
        
		LogPlug::info( "Base program path is: "	+ Utils::getProgramPath() ) ;

		LogPlug::info( "User directory is: " + Utils::getUserDirectory() ) ;
        
  
		LogPlug::info( "End of OSDL utils test." ) ;
 
 
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


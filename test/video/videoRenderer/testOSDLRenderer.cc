#include <string>

#include "Ceylan.h"
using namespace Ceylan::Log ;



#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;


/**
 * Small usage tests for basic renderer.
 *
 *
 */
 
 
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;

    try {
			

    	LogPlug::info( "Testing OSDL basic renderer" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		myVideo.setMode( 640, 480, 16, VideoModule::SoftwareSurface ) ;
			
		//Surface & screen = myVideo.getScreenSurface() ;
				
				
  
				
		LogPlug::info( "End of OSDL renderer test." ) ;
		
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


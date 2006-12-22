#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;


#include "Ceylan.h"
using namespace Ceylan::Log ;


#include <string>
using std::string ;


const std::string iconFile = "OSDL-icon.png" ;


/**
 * Test for window manager operations.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;

	
    try 
	{
	
	
		LogPlug::info( "Testing OSDL window manager services." ) ;
			
		LogPlug::info( "Prerequesite : starting OSDL with video enabled." )	;
			
        CommonModule & myOSDL = getCommonModule( 
			CommonModule::UseVideo | CommonModule::NoParachute  ) ;		
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		string title ;
		string iconName ;
				
		myVideo.getWindowCaption( title, iconName ) ;
					
		LogPlug::info( "Reading window caption : title is '" + title 
			+ "', icon name is '" + iconName + "'." )	;
					
		title    = "OSDL is great" ;
		iconName = "OSDL window manager test" ;
		
		LogPlug::info( "Writing window caption : title will be '" + title 
			+ "', icon name will be '" + iconName + "'." ) ;
			
		myVideo.setWindowCaption( title, iconName ) ;
									
		LogPlug::info( "Reading window caption : title is '" + title 
			+ "', icon name is '" + iconName + "'." )	;
			
		LogPlug::info( "Setting icon image." ) ;
		
		Ceylan::System::FileLocator imageFinder ;
				
		// When run from executable directory :
		imageFinder.addPath( "../../src/doc/web/images" ) ;
	
		// When run from tests-results directory :
		imageFinder.addPath( "../src/doc/web/images" ) ;

		myVideo.setWindowIcon( imageFinder.find( iconFile ) ) ;
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth, 
			VideoModule::SoftwareSurface ) ;

		Surface & screen = myVideo.getScreenSurface() ;
					
		screen.update() ;
		
		myOSDL.getEventsModule().waitForAnyKey() ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		LogPlug::info( "End of OSDL window manager test." ) ;
		
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


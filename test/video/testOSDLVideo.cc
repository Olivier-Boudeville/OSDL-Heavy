#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;


#include "Ceylan.h"
using namespace Ceylan::Log ;



/**
 * Testing the basic services of the OSDL video.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;

	
    try 
	{
	
	
		LogPlug::info( "Testing OSDL video basic services." ) ;
			
		LogPlug::info( "Starting OSDL with video enabled." )	;
			
        CommonModule & myOSDL = getCommonModule( CommonModule::UseVideo ) ;		
			
		myOSDL.logState() ;
			
		LogPlug::info( "Testing basic 2D primitives." ) ;
		
		Coordinate x = 52 ;
		Coordinate y = 27 ;
		
		LogPlug::info( "Creation of a 2D point at ( " 
			+ Ceylan::toString( x )  
			+ " ; " 
			+ Ceylan::toString( y )
			+ ")" ) ;
			
		TwoDimensional::Point2D p( x, y ) ;
		LogPlug::info( p.toString() ) ;
		
		
		LogPlug::info( "Testing real video (displayable)." ) ;
		
		LogPlug::info( "Getting video module." ) ;
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		myVideo.logState() ;
		
		LogPlug::debug( "myVideo class name is : " + myVideo.getClassName() ) ;
		
		LogPlug::info( "Displaying available video definitions : " 
			+ VideoModule::DescribeAvailableDefinitions( 
					Surface::FullScreen | Surface::Hardware ) ) ;
		
		LogPlug::info( "Displaying configuration informations, "
			"including best available pixel format : "
			+ VideoModule::DescribeVideoCapabilities() ) ;
						
		LogPlug::info( "Displaying video driver name : "
			+ myVideo.getDriverName() + "." ) ;
		

		// Going from potential to real :
					
		LogPlug::info( "Entering visual tests : initializing the screen." ) ;
							
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;

		LogPlug::info( "Displaying now new current video informations. "
			+ VideoModule::DescribeVideoCapabilities() ) ;
		
	
		Surface & screen = myVideo.getScreenSurface() ;
			
		screen.lock() ;	
		
		for ( unsigned int n = 0 ; n < 1000 ; n++) 
		{
			// Here should be put all drawings.
		}		
		
		screen.unlock() ;
		
		LogPlug::info( VideoModule::DescribeEnvironmentVariables() ) ;
		
		OSDL::Events::EventsModule & myEvents = myOSDL.getEventsModule() ;
		
		myEvents.logState() ;
		
		screen.update() ;
		
		myEvents.waitForAnyKey() ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		LogPlug::info( "End of OSDL video test." ) ;
		
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


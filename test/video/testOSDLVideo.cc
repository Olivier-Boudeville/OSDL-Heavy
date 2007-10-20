#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;


using namespace Ceylan::Log ;



/**
 * Testing the basic services of the OSDL video module.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;

	
    try 
	{
	
	
		LogPlug::info( "Testing OSDL video basic services." ) ;


		bool isBatch = false ;
		
		std::string executableName ;
		std::list<std::string> options ;
		
		Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;
		
		std::string token ;
		bool tokenEaten ;
		
		
		while ( ! options.empty() )
		{
		
			token = options.front() ;
			options.pop_front() ;

			tokenEaten = false ;
						
			if ( token == "--batch" )
			{
			
				LogPlug::info( "Batch mode selected" ) ;
				isBatch = true ;
				tokenEaten = true ;
			}
			
			if ( token == "--interactive" )
			{
				LogPlug::info( "Interactive mode selected" ) ;
				isBatch = false ;
				tokenEaten = true ;
			}
			
			if ( token == "--online" )
			{
				// Ignored:
				tokenEaten = true ;
			}
			
			if ( LogHolder::IsAKnownPlugOption( token ) )
			{
				// Ignores log-related (argument-less) options.
				tokenEaten = true ;
			}
			
			
			if ( ! tokenEaten )
			{
				throw Ceylan::CommandLineParseException( 
					"Unexpected command line argument: " + token ) ;
			}
		
		}

			
		LogPlug::info( "Starting OSDL with video enabled." )	;
			
        CommonModule & myOSDL = getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseEvents ) ;		
			
		myOSDL.logState() ;
			
		LogPlug::info( "Testing basic 2D primitives." ) ;
		
		Coordinate x = 52 ;
		Coordinate y = 27 ;
		
		LogPlug::info( "Creation of a 2D point at ( " 
			+ Ceylan::toString( x ) + " ; " + Ceylan::toString( y )	+ ")" ) ;
			
		TwoDimensional::Point2D p( x, y ) ;
		LogPlug::info( p.toString() ) ;
		
		
		LogPlug::info( "Testing real video (displayable)." ) ;
		
		LogPlug::info( "Getting video module." ) ;
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		myVideo.logState() ;
			
		LogPlug::info( "Displaying available video definitions: " 
			+ VideoModule::DescribeAvailableDefinitions( 
					Surface::FullScreen | Surface::Hardware ) ) ;
		
		LogPlug::info( "Displaying configuration informations, "
			"including best available pixel format: "
			+ VideoModule::DescribeVideoCapabilities() ) ;
						
		LogPlug::info( "Displaying video driver name: "
			+ myVideo.getDriverName() + "." ) ;
		

		// Going from potential to real:
					
		LogPlug::info( "Entering visual tests: initializing the screen." ) ;
							
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

		LogPlug::info( "Displaying now new current video informations. "
			+ VideoModule::DescribeVideoCapabilities() ) ;
		
	
		Surface & screen = myVideo.getScreenSurface() ;
			
		screen.lock() ;	
		
		for ( Ceylan::Uint32 n = 0; n < 1000; n++) 
		{
			// Here should be put all drawings.
		}		
		
		screen.unlock() ;
		
		LogPlug::info( VideoModule::DescribeEnvironmentVariables() ) ;
		screen.update() ;

		OSDL::Events::EventsModule & myEvents = myOSDL.getEventsModule() ;
		
		myEvents.logState() ;
		
		
		if ( ! isBatch )
			myEvents.waitForAnyKey() ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		LogPlug::info( "End of OSDL video test." ) ;
		
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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;

using namespace Ceylan::Log ;


#include <string>



/// Images location :
const std::string firstImageFile  = "Soldier-heavy-purple-small.png" ;
const std::string secondImageFile = "Battle-three-actors-small.jpg"  ;


/*
 * Image directory is defined relatively to OSDL documentation tree, usually
 * this pathname relative to the install directory where this test 
 * executable should lie is :
 * (to be reached from executable directory)
 *
 */
const std::string imageDirFromExec = "../../../src/doc/web/images" ;


/*
 * Image directory is defined relatively to OSDL documentation tree, 
 * usually this pathname relative to the install directory where this 
 * test executable should lie is :
 * (to be reached from OSDL/OSDL-${OSDL_VERSION}/src/code)
 *
 */
const std::string imageDirForPlayTests = "../src/doc/web/images" ;



/**
 * Small usage tests for Image class.
 *
 */ 
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;


	bool screenshotWanted = true ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL Image" ) ;	
         
		
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
				// Ignored :
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
					"Unexpected command line argument : " + token ) ;
			}
		
		}
		
		 
		OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;		
		
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;


		LogPlug::info( "Loading image located in " + firstImageFile ) ;
				
		screen.lock() ;

		Ceylan::System::FileLocator imageFinder ;
		
		// When run from executable directory :
		imageFinder.addPath( imageDirFromExec ) ;
	
		// When run from tests-results directory :
		imageFinder.addPath( imageDirForPlayTests ) ;
		
		screen.drawGrid() ;

 		screen.loadImage( imageFinder.find( firstImageFile ), 
			/* blit only */ true ) ;
				
		screen.unlock() ;

		screen.update() ;
		
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
			
			
		LogPlug::info( "Loading image located in "	+ secondImageFile 
			+ " with request of image format auto-detection." ) ;
				
		screen.lock() ;


 		Surface & other = Surface::LoadImage( 
			imageFinder.find( secondImageFile ) ) ;
			
		other.blitTo( screen, 270, 155 ) ;
				
		screen.unlock() ;

		screen.update() ;
		
		if ( screenshotWanted )
			screen.savePNG( std::string( argv[0] ) + ".png" ) ;
							
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
			
		delete & other ;
					
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;
		
		LogPlug::info( "End of OSDL Image test." ) ;
	
		
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


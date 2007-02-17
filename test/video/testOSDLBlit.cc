#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;

using namespace Ceylan::Log ;
using namespace Ceylan::Maths::Random ;


#include <string>



/**
 * Small usage tests for blittings.
 *
 */
int main( int argc, char * argv[] ) 
{


	bool screenshotWanted = true ;


	LogHolder myLog( argc, argv ) ;


    try 
	{
			

    	LogPlug::info( "Testing OSDL blit" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				

		screen.lock() ;							
		screen.redraw() ;	
		screen.drawCircle( 250, 250, 50, Green, /* filled */ false ) ;	

		Surface & offscreen = * new Surface( Surface::Software, 
			/* width */ 100, /* height */ 100, screen.getBitsPerPixel() ) ;
		
		offscreen.lock() ;	
		
		offscreen.drawCircle( 25, 25, /* radius */ 25, 
			/* Pixels::ColorDefinition */ Pixels::Red, /* filled */ true ) ;
			
		offscreen.drawCircle( 50, 50, /* radius */ 25, 
			/* Pixels::ColorDefinition */ Pixels::Green, /* filled */ true ) ;
			
		offscreen.drawCircle( 75, 75, /* radius */ 25, 
			/* Pixels::ColorDefinition */ Pixels::Blue, /* filled */ true ) ;
			
		offscreen.unlock() ;	
		
		if ( screenshotWanted )
			offscreen.savePNG( argv[0] + std::string( "-offscreen.png" ) ) ;
		
		offscreen.blitTo( screen, 
			TwoDimensional::Point2D( 
				static_cast<Coordinate>( 400 ), 250 ) ) ;
				
		screen.unlock() ;
		 			
		screen.update() ;	
					
		myOSDL.getEventsModule().waitForAnyKey() ;
				
		LogPlug::info( "End of OSDL Lines test" ) ;
		
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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;

using namespace Ceylan::Log ;


#include <string>



/**
 * Small usage tests for widgets.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;


    try 
	{
			

    	LogPlug::info( "Testing OSDL Widget" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
    	LogPlug::info( "Drawing widget." ) ;		
 		
		
		LogPlug::info( "Before adding widget, screen surface is : "
			 + screen.toString() ) ;

		screen.lock() ;		
	
		/*
		 * Widgets will be owned (and deallocated) by their container, here 
		 * the screen surface.
		 *
		 * Widget width is only 100, so 'Heimdal' will be truncated to 'Heimd' :
		 *
		 */
		new Widget( /* container */ screen,
			/* location */ Point2D( static_cast<Coordinate>( 50 ), 50 ), 
			/* width */ 100, 
			/* height */ 100, 
			/* baseColorMode */ Widget::BackgroundColor,
			/* backgroundColor */ Brown,
			"Eye of Heimdal" ) ;


		// Made to overlap :
		
		new Widget( /* container */ screen,
			/* location */ Point2D( static_cast<Coordinate>( 100 ), 100 ), 
			/* width */ 100, 
			/* height */ 100, 
			/* baseColorMode */ Widget::BackgroundColor,
			/* backgroundColor */ Turquoise,
			"Hello world" ) ;
		
		
		LogPlug::info( "After adding widget, screen surface is : " 
			+ screen.toString() ) ;

		
		screen.drawCircle( 150, 150, 150, Red, /* filled */ false ) ;

		screen.redraw() ;
		
		screen.drawCircle( 300, 250, 100, Green, /* filled */ true ) ;
					
		screen.unlock() ;
				
		screen.update() ;	
		
		screen.savePNG( argv[0] + std::string( ".png" ) ) ;
		
		myOSDL.getEventsModule().waitForAnyKey() ;
						
		LogPlug::info( "End of OSDL Widget test" ) ;
		
		OSDL::stop() ;
		
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


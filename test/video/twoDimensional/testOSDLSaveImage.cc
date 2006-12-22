#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;

#include "Ceylan.h"
using namespace Ceylan::Log ;


#include <string>




/**
 * Small usage tests for Image class.
 *
 */
 int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;


    try 
	{
			

    	LogPlug::info( "Testing OSDL save Image" ) ;	
         
		 
		OSDL::CommonModule & myOSDL = 
			OSDL::getCommonModule( CommonModule::UseVideo | CommonModule::NoParachute ) ;		
				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length windowWidth  = 640 ;
		Length windowHeight = 480 ;
		
		myVideo.setMode( windowWidth, windowHeight, VideoModule::UseCurrentColorDepth, 
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;

		
    	LogPlug::info( "Creating a silly image" ) ;	


    	LogPlug::info( "Prerequesite : having three random generators" ) ;	
		
		Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0, windowWidth ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0, windowHeight ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
		
				
    	LogPlug::info( "Drawing random pixels with random colors" ) ;	
		
		screen.lock() ;
		
		Coordinate abscissa ;
		Coordinate ordinate ;
		
		ColorElement red ;
		ColorElement green ;
		ColorElement blue ;
		ColorElement alpha ;
		
						
		for ( int j = 0; j < 50000; j++ )
		{
		
			abscissa = abscissaRand.getNewValue() ;
			ordinate = ordinateRand.getNewValue() ;
			
			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;
								
			putRGBAPixel( screen, abscissa, ordinate, red, green, blue, alpha ) ;
		}

			
		screen.unlock() ;

		screen.update() ;
						
		myOSDL.getEventsModule().waitForAnyKey() ;
		
		
		std::string bmpFile = "screenshot.bmp" ;
		std::string pngFile = "screenshot.png" ;
		
    	LogPlug::info( "Saving screen surface in " + bmpFile ) ;	
		screen.saveBMP( bmpFile ) ;
		
    	LogPlug::info( "Saving screen surface in " + pngFile ) ;	
		screen.savePNG( pngFile ) ;
					
		LogPlug::info( "End of OSDL save Image test." ) ;
	
		
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


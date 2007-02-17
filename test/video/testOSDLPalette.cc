#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;


using namespace Ceylan::Log ;


#include <string>
using std::string ;



/**
 * Testing the OSDL palette-oriented services.
 *
 */
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;
	
    try 
	{
	
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ;
		
		LogPlug::info( "Testing OSDL palette services." ) ;
		
		LogPlug::info( "Creating greyscale palette." ) ;
			
		Palette & greyPal = Palette::CreateGreyScalePalette( screenHeight ) ;
			
		LogPlug::info( "Displaying greyscale palette : " 
			+ greyPal.toString() ) ;		

		OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
				CommonModule::UseVideo | CommonModule::NoParachute ) ;	
				
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
		
		Surface & screen = myVideo.getScreenSurface() ;

		greyPal.draw( screen ) ;
	
		screen.update() ;
						
		myOSDL.getEventsModule().waitForAnyKey() ;
		
		delete & greyPal ;
		
		Palette & colorPal = Palette::CreateGradationPalette(
			Pixels::MidnightBlue, Pixels::DeepPink, screenHeight ) ;

		LogPlug::info( "Displaying colored palette : " + colorPal.toString() ) ;		
		
		colorPal.draw( screen ) ;
		
		screen.update() ;
						
		myOSDL.getEventsModule().waitForAnyKey() ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
		
        OSDL::stop() ;

		delete & colorPal ;
		
		LogPlug::info( "End of OSDL palette test." ) ;
		
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


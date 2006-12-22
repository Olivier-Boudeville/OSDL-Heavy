#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;

#include "Ceylan.h"
using namespace Ceylan::Log ;


#include <string>



/**
 * Small usage tests for circles.
 *
 * @see testOSDLConic.cc
 *
 */
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL Circle" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         		 
		CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
    	LogPlug::info( "Drawing random coloured circles, discs, ellipses, filled or not." ) ;		
 		
		screen.lock() ;		
	
    	LogPlug::info( "Prerequesite : having three random generators" ) ;	
		
		Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0, screenWidth ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0, screenHeight ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator radiusRand( 0, 100 ) ;
		
		// Returns 0 or 1, as a coin :
		Ceylan::Maths::Random::WhiteNoiseGenerator coinRand( 0, 2 ) ;
		
		Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
	
		Coordinate x, y ;
			
		ColorElement red ;
		ColorElement green ;
		ColorElement blue ;
		ColorElement alpha ;
	
	
		for ( int i = 0; i < 200; i++ )
		{
								
			x      = abscissaRand.getNewValue() ;			
			y      = ordinateRand.getNewValue() ;
			
			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;
			
			if ( coinRand.getNewValue() == 0 )					
				screen.drawCircle( x, y, radiusRand.getNewValue(), red, green, blue, alpha, 
					/* filled */ static_cast<bool>( coinRand.getNewValue() ) ) ;
			else
				screen.drawDiscWithEdge( x, y, radiusRand.getNewValue(), radiusRand.getNewValue(), 
					Pixels::convertRGBAToColorDefinition(
						colorRand.getNewValue(), colorRand.getNewValue(), 
						colorRand.getNewValue(), colorRand.getNewValue() ), 
					Pixels::convertRGBAToColorDefinition(
						colorRand.getNewValue(), colorRand.getNewValue(), 
						colorRand.getNewValue(), colorRand.getNewValue() )
				 ) ;
					
		}
	
		screen.unlock() ;
				
		screen.update() ;	
		
		myOSDL.getEventsModule().waitForAnyKey() ;
				
		LogPlug::info( "End of OSDL Circle test" ) ;
		
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


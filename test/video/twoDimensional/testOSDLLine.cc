#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;


using namespace Ceylan::Log ;


#include <string>



/**
 * Small usage tests for Lines.
 *
 */
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL Lines" ) ;	
		
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
				
				
    	LogPlug::info( "Drawing vertical lines" ) ;	
		
		screen.lock() ;

		for ( Ceylan::Uint32 x = 0; x < 250; x++ )
		{
					
			LogPlug::info( "drawing from abscissa " + Ceylan::toString( x ) 
				+ " a vertical line whose color is [R,G,B] = [" 
				+ Ceylan::toString( x ) + ", " + Ceylan::toString( x ) + ", "
				+ Ceylan::toString( x ) + "] " ) ;
				
			screen.drawVerticalLine( x, 100, 300, x, x, x ) ;
		}
		
		
    	LogPlug::info( "Drawing horizontal lines" ) ;	
		
		for ( Ceylan::Uint32 y = 100; y < 300; y++ )
		{
					
			LogPlug::info( "drawing from ordinate " + Ceylan::toString( y ) 
				+ " an horizontal line whose color is [R,G,B] = [" 
				+ Ceylan::toString( y - 50 ) + ", " 
				+ Ceylan::toString( y - 50 ) + ", "
				+ Ceylan::toString( y - 50 ) + "] " ) ;

			screen.drawHorizontalLine( 300, 550, y, y-50, y-50, y-50 ) ;
				
		}
	
    	LogPlug::info( "Drawing random coloured lines" ) ;	

    	LogPlug::info( "Prerequesite : having three random generators" ) ;	
		
		Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 
			0, screenWidth ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 
			0, screenHeight ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
	
		Coordinate x1, y1, x2, y2 ;
		
		ColorElement red ;
		ColorElement green ;
		ColorElement blue ;
		ColorElement alpha ;
	
	
		for ( Ceylan::Uint32 i = 0; i < 10000; i++ )
		{
								
			x1 = abscissaRand.getNewValue() ;
			x2 = abscissaRand.getNewValue() ;
			
			y1 = ordinateRand.getNewValue() ;
			y2 = ordinateRand.getNewValue() ;
			
			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;
								
			screen.drawLine( x1, y1, x2, y2, red, green, blue, alpha ) ;
		}
	
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


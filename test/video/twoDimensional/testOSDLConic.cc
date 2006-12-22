#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;

#include "Ceylan.h"
using namespace Ceylan::Log ;


#include <string>



/**
 * Small usage tests for conic curves, including circles.
 *
 * @see testOSDLCircle.cc
 *
 */
int main( int argc, char * argv[] ) 
{

	bool screenshotWanted = true ;

	LogHolder myLog( argc, argv ) ;

	bool testSingleDiscWithEdge    = true ;
	bool testRandomEllipsesAndDisc = true ;
	
	
    try 
	{
			

    	LogPlug::info( "Testing OSDL Conic" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         		 
		CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 

		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
    	LogPlug::info( "Drawing random coloured circles, discs, ellipses, filled or not." ) ;		
 		
	
		if ( testSingleDiscWithEdge )
		{
		
			LogPlug::info( "Testing a disc with edges, blitted onto a grid" ) ;
			
			screen.lock() ;	
				
			screen.drawGrid() ;
			
			ColorDefinition red   = Pixels::convertRGBAToColorDefinition( 255, 0, 0, 128 ) ;
			ColorDefinition green = Pixels::convertRGBAToColorDefinition( 0, 255, 0, 128 ) ;
			ColorDefinition blue  = Pixels::convertRGBAToColorDefinition( 0, 0, 255, 128 ) ;
			
			// Both green must look the same :
			screen.drawCircle( 550, 400, 80, green, /* filled */ true ) ;

			// Both red must look the same :
			screen.drawCircle( 550, 80, 80, red, /* filled */ true ) ;

			if ( ! screen.drawDiscWithEdge( /* x */ 240, /* y */ 240, 
				/* outer radius */ 240, /* inner radius */ 150, 
				/* ring color */ red, /* inner disc color */ green ) )
					throw Ceylan::TestException( 
						"Drawing of a disc with edges (inner not opaque) failed." ) ;
			
			if ( ! screen.drawDiscWithEdge( /* x */ 550, /* y */ 240, 
				/* outer radius */ 40, /* inner radius */ 20, 
				/* ring color */ Pixels::Tan, /* inner disc color */ Pixels::Honeydew ) )
					throw Ceylan::TestException( 
						"Drawing of a disc with edges (inner opaque) failed." ) ;
			
				 
			screen.unlock() ;
	
			screen.update() ;
	
			myOSDL.getEventsModule().waitForAnyKey() ;
			
		}
		
		
		if ( testRandomEllipsesAndDisc ) 
		{
		
			LogPlug::info( "Testing random ellipses, circles with our without edges" ) ;
			
			
    		LogPlug::info( "Prerequesite : having three random generators" ) ;	
		
			Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0, screenWidth ) ;
			Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0, screenHeight ) ;
			Ceylan::Maths::Random::WhiteNoiseGenerator radiusRand( 0, 200 ) ;
	
			// Returns 0 or 1, as a coin :
			Ceylan::Maths::Random::WhiteNoiseGenerator coinRand( 0, 2 ) ;
			
			Ceylan::Maths::Random::WhiteNoiseGenerator typeRand( 0, 3 ) ;
	
			Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
	
			Coordinate x, y ;
	
			ColorElement red ;
			ColorElement green ;
			ColorElement blue ;
			ColorElement alpha ;
	
			screen.lock() ;		
	
			for ( int i = 0; i < 200; i++ )
			{
	
				x     = abscissaRand.getNewValue() ;
				y     = ordinateRand.getNewValue() ;
	
				red   = colorRand.getNewValue() ;
				green = colorRand.getNewValue() ;
				blue  = colorRand.getNewValue() ;
				alpha = colorRand.getNewValue() ;
	
				switch( typeRand.getNewValue() )
				{
				
					case 0:
						screen.drawCircle( x, y, radiusRand.getNewValue(), red, green, blue, alpha,
							/* filled */ static_cast<bool>( coinRand.getNewValue() ) ) ;
						break ;
					
					case 1:
						screen.drawEllipse( x, y, radiusRand.getNewValue(),
							radiusRand.getNewValue(),
							red, green, blue, alpha,
							/* filled */ static_cast<bool>( coinRand.getNewValue() ) ) ;
						break ;
						
					case 2:
						screen.drawDiscWithEdge( x, y, radiusRand.getNewValue(), 
							radiusRand.getNewValue(), 
							Pixels::convertRGBAToColorDefinition( red, green, blue, alpha ),
							Pixels::convertRGBAToColorDefinition( colorRand.getNewValue(),
								colorRand.getNewValue(), colorRand.getNewValue(),
								colorRand.getNewValue() ) ) ;
						break ;
						
					default:
						throw Ceylan::TestException( "Unexpected type drawn !" ) ;
						break ;		
						
				}		
	
			}
	
			screen.unlock() ;
	
			screen.update() ;

			if ( screenshotWanted )
				screen.savePNG( argv[0] + std::string( ".png" ) ) ;
	
			myOSDL.getEventsModule().waitForAnyKey() ;
	
		}
		
				
		LogPlug::info( "End of OSDL Conic test" ) ;
		
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


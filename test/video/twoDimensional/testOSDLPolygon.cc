#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;

#include "Ceylan.h"
using namespace Ceylan::Log ;
using namespace Ceylan::Maths ;


#include <string>
using std::string ;

#include <list>
using std::list ;




/**
 * Testing polygon sets thanks to snow flakes.
 *
 */
int main( int argc, char * argv[] ) 
{

	bool plentyOfFlakes     = true ;
	bool screenshotDemanded = true ;
	
	LogHolder myLog( argc, argv ) ;

    try 
	{
			
			
    	LogPlug::info( "Testing OSDL Polygon" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
    	LogPlug::info( "Drawing flake-shaped polygon sets." ) ;					
 		
				
		Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0, screenWidth ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0, screenHeight ) ;
		
		Ceylan::Maths::Random::WhiteNoiseGenerator branchCountRand( 3, 9 ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator lengthRand( 20, 30 ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator thicknessRand( 2, 8 ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator childAngleRand( 30, 70 ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator branchingHeightRatioRand( 2, 8 ) ;
		Ceylan::Maths::Random::WhiteNoiseGenerator scaleRand( 2, 9 ) ;
		
			
		PolygonSet * currentFlake ;
		
		screen.lock() ;		

		if ( ! plentyOfFlakes )
		{		
		
			LogPlug::info( "Only one flake will be drawn." ) ;
			
			PolygonSet & flake = PolygonSet::CreateFlake( 5, /* length */ 50, /* thickness */ 10, 
				/* childAngle */ 50, /* branchingHeightRatio */ 0.6, /* scale */ 0.4 ) ; 
			LogPlug::info( "Flake polygon set displays as : " + flake.toString() ) ;
			flake.draw( screen, 320, 200, Snow, true ) ;
			delete & flake ;
			screen.unlock() ;				
			screen.update() ;			
			
			if ( screenshotDemanded )
				screen.savePNG( string( argv[0] ) + ".png" ) ;
				
			myOSDL.getEventsModule().waitForAnyKey() ;
			
			LogPlug::info( "End of OSDL Polygon test " ) ;
			
			return 0 ;
			
		}
				
				
		for ( Coordinate ordinate = 0; ordinate < screenHeight; ordinate += 50 )
			for ( Coordinate abscissa = 0; abscissa < screenWidth; abscissa += 50 )
		{
		
			currentFlake = & PolygonSet::CreateFlake( 
				branchCountRand.getNewValue(),
				lengthRand.getNewValue(), 
				thicknessRand.getNewValue(), 
				childAngleRand.getNewValue(),
				( float ) branchingHeightRatioRand.getNewValue() / 10.0f, 
				( float ) scaleRand.getNewValue() / 10.0f
			) ; 
			
			currentFlake->draw( screen, abscissa, ordinate, 
				/* color name */ Snow, /* filled */ true ) ;
				
			delete currentFlake ;
			
		}
		
		screen.unlock() ;
		
		if ( screenshotDemanded )
			screen.savePNG( argv[0] + string( ".png" ) ) ;
				
		screen.update() ;	
		
		myOSDL.getEventsModule().waitForAnyKey() ;

		LogPlug::info( "End of OSDL Polygon test" ) ;
		
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


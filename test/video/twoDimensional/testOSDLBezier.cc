#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;


#include "Ceylan.h"
using namespace Ceylan::Log ;


#include <string>



/**
 * Small usage tests for Bezier curves.
 *
 */
int main( int argc, char * argv[] ) 
{

	bool screenshotWanted = true ;


	LogHolder myLog( argc, argv ) ;


    try 
	{
			

    	LogPlug::info( "Testing OSDL Bezier curves" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
				
    	LogPlug::info( "Drawing Bezier curves" ) ;	
		
		listPoint2D controlPoints ;
		
		screen.lock() ;
		
		// Draws 'O' :
		Point2D oFirst( 100, 80 ) ;
		controlPoints.push_back( & oFirst ) ;		
		screen.drawCross( oFirst ) ;
		
		Point2D oSecond( 5, 180 ) ;
		controlPoints.push_back( & oSecond ) ;
		screen.drawCross( oSecond ) ;
		
		Point2D oThird( 170, 220 ) ;
		controlPoints.push_back( & oThird ) ;
		screen.drawCross( oThird ) ;
				
		Point2D oFourth( 180, 120 ) ;
		controlPoints.push_back( & oFourth ) ;
		screen.drawCross( oFourth ) ;
				
		Point2D oFifth( 100, 90 ) ;
		controlPoints.push_back( & oFifth ) ;
		screen.drawCross( oFifth ) ;
				
		if ( ! TwoDimensional::drawBezierCurve( screen, controlPoints, 
				/* numberOfSteps */ 50, Pixels::Bisque ) ) 
			throw Ceylan::TestException( "Drawing of Bezier curve failed (letter 'O')." ) ;
		
		controlPoints.clear() ;
		
		
		// Draws 'S' :
		
		Point2D sFirst( 200, 200 ) ;
		controlPoints.push_back( & sFirst ) ;		
		screen.drawCross( sFirst ) ;
		
		Point2D sSecond( 350, 180 ) ;
		controlPoints.push_back( & sSecond ) ;
		screen.drawCross( sSecond ) ;
		
		Point2D sThird( 170, 100 ) ;
		controlPoints.push_back( & sThird ) ;
		screen.drawCross( sThird ) ;
				
		Point2D sFourth( 300, 80 ) ;
		controlPoints.push_back( & sFourth ) ;
		screen.drawCross( sFourth ) ;
				
		if ( ! TwoDimensional::drawBezierCurve( screen, controlPoints, 
				/* numberOfSteps */ 50, Pixels::Bisque ) ) 
			throw Ceylan::TestException( "Drawing of Bezier curve failed (letter 'S')." ) ;
		
		controlPoints.clear() ;



		// Draws 'D' :
		
		Point2D dFirst( 360, 100 ) ;
		controlPoints.push_back( & dFirst ) ;		
		screen.drawCross( dFirst ) ;
		
		Point2D dSecond( 300, 300 ) ;
		controlPoints.push_back( & dSecond ) ;
		screen.drawCross( dSecond ) ;
		
		Point2D dThird( 480, 180 ) ;
		controlPoints.push_back( & dThird ) ;
		screen.drawCross( dThird ) ;
				
		Point2D dFourth( 480, 140 ) ;
		controlPoints.push_back( & dFourth ) ;
		screen.drawCross( dFourth ) ;

		Point2D dFifth( 350, 90 ) ;
		controlPoints.push_back( & dFifth ) ;
		screen.drawCross( dFifth ) ;
				
		if ( ! TwoDimensional::drawBezierCurve( screen, controlPoints, 
				/* numberOfSteps */ 50, Pixels::Bisque ) ) 
			throw Ceylan::TestException( "Drawing of Bezier curve failed (letter 'D')." ) ;
		
		controlPoints.clear() ;
			

		// Draws 'L' :
		
		Point2D lFirst( 520, 100 ) ;
		controlPoints.push_back( & lFirst ) ;		
		screen.drawCross( lFirst ) ;
		
		Point2D lSecond( 480, 220 ) ;
		controlPoints.push_back( & lSecond ) ;
		screen.drawCross( lSecond ) ;
		
		Point2D lThird( 620, 180 ) ;
		controlPoints.push_back( & lThird ) ;
		screen.drawCross( lThird ) ;
				
					
		if ( ! TwoDimensional::drawBezierCurve( screen, controlPoints, 
				/* numberOfSteps */ 50, Pixels::Bisque ) ) 
			throw Ceylan::TestException( "Drawing of Bezier curve failed (letter 'L')." ) ;
		
		controlPoints.clear() ;
			
	
		screen.unlock() ;
				
		screen.update() ;	

		if ( screenshotWanted )
			screen.savePNG( argv[0] + std::string( ".png" ) ) ;
		
		myOSDL.getEventsModule().waitForAnyKey() ;
				
		LogPlug::info( "End of OSDL Bezier curves test" ) ;
		
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


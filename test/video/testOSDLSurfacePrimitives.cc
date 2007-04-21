#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;


using namespace Ceylan::Log ;
using namespace Ceylan::Maths::Random ;


#include <string>



/**
 * Small usage tests for Surfaces drawing primitives.
 *
 */
int main( int argc, char * argv[] ) 
{


	bool screenshotWanted = true ;
	
	
	LogHolder myLog( argc, argv ) ;

    try 
	{


    	LogPlug::info( "Testing OSDL surface drawing primitives" ) ;	

			
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
			
				// Ignored for this test.
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

		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
		
		LogPlug::info( "Testing first the starting colors of pixels, "
			"both for screen surface and a newly created RGBA test surface." ) ;
		
		screen.lock() ;
		
		screen.fill( Pixels::Black ) ;	
		
		screen.drawGrid( 20, 20, Pixels::IndianRed ) ;
		 
		// putPixels methods showned elsewhere.
		screen.drawHorizontalLine( 10, 150, 80, Pixels::Khaki ) ;		
		screen.drawVerticalLine( 100, 50, 280, Pixels::SlateGrey ) ;
		screen.drawLine( 170, 80, 400, 160, Pixels::RosyBrown ) ;
		
		screen.drawCross( 250, 250, Pixels::Orange, /* edge */ 30 ) ;
		screen.drawEdges( Honeydew, /* width */ 7 ) ;
		
		UprightRectangle firstRect( 170, 130, 80, 50 ) ;
		
		screen.drawBox( firstRect, Pixels::DodgerBlue, /* filled */ true ) ;

		UprightRectangle secondRect( 570, 130, 30, 250 ) ;
		
		screen.drawBox( secondRect, Pixels::Silver, /* filled */ false ) ;


		UprightRectangle toBeRounded( /* x */ 150, /* y */ 290, 
				/* width */ 200, /* height */ 150 ) ;
		
		toBeRounded.drawWithRoundedCorners( screen, /* edgeWith */ 13,
				/* edgeColorDef */ Pixels::White,
				/* backgroundColorDef */ Pixels::DarkSlateBlue ) ;


		UprightRectangle roundTransparent( /* x */ 30, /* y */ 40, 
				/* width */ 600, /* height */ 190 ) ;

		// Now test a partially opaque rounded rectangle :			
		ColorDefinition alphaEdgeColor = 
			Pixels::convertRGBAToColorDefinition( 255, 0, 0, 100 ) ;
			
		ColorDefinition alphaBackColor = 
			Pixels::convertRGBAToColorDefinition( 0, 255, 0, 100 ) ;
		
		roundTransparent.drawWithRoundedCorners( screen, /* edgeWith */ 25,
			/* edgeColorDef */ alphaEdgeColor,
			/* backgroundColorDef */ alphaBackColor ) ;

			
		screen.drawCircle( 500, 250, 50, Pixels::DarkGreen, 
			/* filled */ true, /* blended */ true ) ;
		
		screen.drawCircle( 300, 350, 50, Pixels::Orange, 
			/* filled */ false, /* blended */ true ) ;
		
		screen.drawDiscWithEdge( 570, 350, 80, 50, Pixels::SteelBlue,
			Pixels::Silver, /* blended */ true ) ;
		
		screen.drawEllipse( 100, 400, 80, 50, Pixels::Red, 
			/* filled */ false ) ;
		
		screen.drawEllipse( 100, 300, 80, 50, Pixels::PaleGreen, 
			/* filled */ true ) ;
		
		screen.drawPie( 500, 100, 80, /* angle start */ 50, 
			/* angle stop */ 160, Pixels::YellowGreen ) ;
		
		screen.drawTriangle( 300, 300, 380, 250, 350, 100, Pixels::Purple ) ;
		
		// Polygons tested elsewhere.
		PolygonSet * flake = & PolygonSet::CreateFlake( 
			/* branch count */ 7,
			/* length */ 90,
			/* thickness */ 10,
			/* child angle */ 35,
			/* height ratio */ 0.5,
			/* scale */ 0.4f ) ; 
			
		flake->draw( screen, 450, 350, /* color name */ Snow, 
			/* filled */ true ) ;
				
		delete flake ;
		
		screen.printText( "Non-solid colors "
			"(with intermediate alpha coordinates) could be used as well.", 
			15, 460, Pixels::LavenderBlush ) ;


		// Let's add some Bezier curves :
		
		listPoint2D controlPoints ;
		
		Pixels::ColorDefinition letterColor = Pixels::Yellow ;
		
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
				/* numberOfSteps */ 50, letterColor ) ) 
			throw Ceylan::TestException( 
				"Drawing of Bezier curve failed (letter 'O')." ) ;
		
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
				/* numberOfSteps */ 50, letterColor ) ) 
			throw Ceylan::TestException( 
				"Drawing of Bezier curve failed (letter 'S')." ) ;
		
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
				/* numberOfSteps */ 50, letterColor ) ) 
			throw Ceylan::TestException( 
				"Drawing of Bezier curve failed (letter 'D')." ) ;
		
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
				/* numberOfSteps */ 50, letterColor ) ) 
			throw Ceylan::TestException( 
				"Drawing of Bezier curve failed (letter 'L')." ) ;
		
		controlPoints.clear() ;

			
		screen.unlock() ;
				
		screen.update() ;	
		
		if ( screenshotWanted )
			screen.savePNG( argv[0] + std::string( ".png" ) ) ;
		
		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;

		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;
				
		LogPlug::info( "End of OSDL Surface drawing primitives test." ) ;
		
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


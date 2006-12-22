#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;


#include "Ceylan.h"
using namespace Ceylan::Log ;
using namespace Ceylan::Maths::Random ;


#include <string>



/**
 * Small usage tests for Surfaces.
 *
 */
int main( int argc, char * argv[] ) 
{


	bool screenshotWanted = true ;
	
	
	LogHolder myLog( argc, argv ) ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL surface" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
		
		LogPlug::info( "Testing first the starting colors of pixels, "
			"both for screen surface and a newly created RGBA test surface." ) ;
		
		screen.lock() ;
		
		ColorDefinition last = Pixels::Black ;
		
		bool allEqual = true ;
		
		for ( Coordinate y = 0; y < screenHeight ; y++ )
		{
			for ( Coordinate x = 0; x < screenWidth ; x++ )
			{
				
				if ( x % 40 == 0 && y % 40 == 0 )
					LogPlug::trace( "Screen pixel at " + Point2D( x, y ).toString() + " : " 
						+ screen.describePixelAt( x, y ) ) ;
						
		 		ColorDefinition current = screen.getColorDefinitionAt( x, y ) ;
				
				// Screen is RGB, not RGBA :
				if ( ! Pixels::areEqual( last, current, /* use alpha */ false ) )
				{
					LogPlug::error( "Current screen color at " + Point2D( x, y ).toString() + ", "
						+ Pixels::toString( current ) 
						+ ", is not the same as the one of previous pixel ("
						+ Pixels::toString( last ) + ")" ) ; 
					last = current ;	
					allEqual = false ;
				}	
			}
		}
				
		if ( allEqual )
			LogPlug::info( "All pixels of the screen start at color definition " 
				+ Pixels::toString(	Pixels::Black ) ) ;
		else
			throw Ceylan::TestException( "Screen pixels do not all start at the same color" ) ;
					
		screen.unlock() ;
		
		Length testSurfaceWidth  = 50 ;
		Length testSurfaceHeight = 100 ; 
		
		
		// RGBA test surface :
		Surface & testSurface = * new Surface ( Surface::Software | Surface::AlphaBlendingBlit,
			/* width */ testSurfaceWidth, /* height */ testSurfaceHeight ) ;
		

		testSurface.lock() ;
		
				
		last = Pixels::Transparent ;
		
		for ( Coordinate y = 0; y < testSurfaceHeight ; y++ )
		{
			for ( Coordinate x = 0; x < testSurfaceWidth ; x++ )
			{
				

				if ( x % 40 == 0 && y % 40 == 0 )
					LogPlug::trace( "Test surface pixel at " + Point2D( x, y ).toString() + " : " 
						+ testSurface.describePixelAt( x, y ) ) ;

		 		ColorDefinition current = testSurface.getColorDefinitionAt( x, y ) ;
				
				// Test surface is RGBA :
				if ( ! Pixels::areEqual( last, current, /* use alpha */ true ) )
				{
					LogPlug::error( "Current RGBA test surface color at " 
						+ Point2D( x, y ).toString() + ", "
						+ Pixels::toString( current ) 
						+ ", is not the same as the one of previous pixel ("
						+ Pixels::toString( last ) + ")" ) ; 
					last = current ;	
					allEqual = false ;
				}	
			}
		}
		
		if ( allEqual )
			LogPlug::info( "All pixels of the RGBA test surface start at color definition " 
				+ Pixels::toString(	Pixels::Black ) ) ;
		else
			throw Ceylan::TestException( 
				"RGBA test surface pixels do not all start at the same color" ) ;
									
		testSurface.unlock() ;

		delete & testSurface ;

		// Selected set of colors :		
		ColorDefinition pencilColor  = Pixels::Orange ;
		ColorDefinition captionColor = Pixels::Yellow ;
		ColorDefinition backColor    = Pixels::DarkSlateBlue ;
				
		
        LogPlug::info( "Drawing normal PDF-based distribution graph." ) ;

		Seed seed = 145 ;
		
		Sample mean     = 77 ;
		Deviation sigma = 4 ;
		
		LogPlug::info( "Creating a probability density function (PDF) corresponding to "
			"a normal distribution whose sample mean is " + Ceylan::toString( mean )
			+ " and whose standard deviation is " + Ceylan::toString( sigma ) + "." ) ;
			
		NormalProbabilityFunction myGaussian( mean, sigma ) ; 	
		
		RandomValue sampleStart = 50 ;
		RandomValue sampleStop  = 120 ;
		RandomValue sampleCount = sampleStop - sampleStart ;
				 		
		LogPlug::info( "Creating a random generator whose PDF is : " 
			+ myGaussian.toString() + " Samples will range from "
			+ Ceylan::toString( sampleStart ) + " (included) to " 
			+ Ceylan::toString( sampleStop )  + " (excluded)." ) ;
		
		RandomGeneratorFromPDF myGaussianGenerator( myGaussian, 
			sampleStart, sampleStop, seed ) ;
		
		LogPlug::info( "Displaying normal law. " 
			+ myGaussianGenerator.displayProbabilities() ) ;
		 	
		
		unsigned int firstDrawCount  = 100 ;		
		unsigned int secondDrawCount = 1000 ;		
		unsigned int thirdDrawCount  = 10000 ;		
		unsigned int fourthDrawCount = 100000 ;		
		
		unsigned int totalDrawCount = 0 ;

		Length graphWidth  = 3 * sampleCount + 2 * Surface::graphAbscissaOffset + 20 ;
		Length graphHeight = 80 ;
		
		
		LogPlug::info( "Generating series of random samples." ) ;
		
		RandomValue newValue ;
		
		// Constructs a table recording how many samples are drawn for each possible value.
		
		Ceylan::Maths::IntegerData * distributionTable = 
			new Ceylan::Maths::IntegerData[ sampleCount ] ;
		
		for ( unsigned int i = 0 ; i < sampleCount; i++)
			distributionTable[ i ] = 0 ;
			
		//LogPlug::info( "Throwing dice (one out of ten displayed)..." ) ;
		
		/// After 100 samples :
		for ( unsigned int currentDrawCount = totalDrawCount; 
			currentDrawCount < firstDrawCount; currentDrawCount++ )
		{
			newValue = myGaussianGenerator.getNewValue() ;
			
			// Avoid too many useless logs :			
			/*
			 if ( currentDrawCount % 10 == 0 )
			
			 	LogPlug::info( "Drawing value " + Ceylan::toString( newValue ) + "." ) ;
			*/
			
			distributionTable[ newValue - sampleStart ] += 1 ;
		}
		
		totalDrawCount = firstDrawCount ;

		screen.lock() ;

		screen.printText( "Graphs of a normal distribution", 100, 30, Pixels::Yellow ) ;
			
		screen.printText( "(sample mean is "
			+ Ceylan::toString( mean ) + ", standard deviation is " 
			+ Ceylan::toString( sigma, 2 ) + ")", 130, 50, Pixels::Yellow ) ;
			
		UprightRectangle firstDrawingArea( 100, 80, graphWidth, graphHeight ) ;

			
		screen.displayData( distributionTable, sampleCount, pencilColor, captionColor, backColor, 
			"Distribution of " + Ceylan::toString( totalDrawCount )
			+ " samples", & firstDrawingArea ) ;


		/// After 1 000 samples :
		for ( unsigned int currentDrawCount = totalDrawCount; 
			currentDrawCount < secondDrawCount; currentDrawCount++ )
		{
			newValue = myGaussianGenerator.getNewValue() ;
			
			// Avoid too many useless logs :			
			/*
			 if ( currentDrawCount % 10 == 0 )
			
			 	LogPlug::info( "Drawing value " + Ceylan::toString( newValue ) + "." ) ;
			*/
			
			distributionTable[ newValue - sampleStart ] += 1 ;
		}
		
		totalDrawCount = secondDrawCount ;
	
	
		UprightRectangle secondDrawingArea( 100, 180, graphWidth, graphHeight ) ;
			
		screen.displayData( distributionTable, sampleCount, pencilColor, captionColor, backColor, 
			"Distribution of " + Ceylan::toString( totalDrawCount )
			+ " samples", & secondDrawingArea ) ;


		/// After 10 000 samples :
		for ( unsigned int currentDrawCount = totalDrawCount; 
			currentDrawCount < thirdDrawCount; currentDrawCount++ )
		{
			newValue = myGaussianGenerator.getNewValue() ;
			
			// Avoid too many useless logs :			
			/*
			 if ( currentDrawCount % 10 == 0 )
			
			 	LogPlug::info( "Drawing value " + Ceylan::toString( newValue ) + "." ) ;
			*/
			
			distributionTable[ newValue - sampleStart ] += 1 ;
		}
		
		totalDrawCount = thirdDrawCount ;

		UprightRectangle thirdDrawingArea( 100, 280, graphWidth, graphHeight ) ;
			
		screen.displayData( distributionTable, sampleCount, pencilColor, captionColor, backColor, 
			"Distribution of " + Ceylan::toString( totalDrawCount )
			+ " samples", & thirdDrawingArea ) ;


		// After 100 000 samples :
		for ( unsigned int currentDrawCount = totalDrawCount; 
			currentDrawCount < fourthDrawCount; currentDrawCount++ )
		{
			newValue = myGaussianGenerator.getNewValue() ;
			
			// Avoid too many useless logs :			
			/*
			 if ( currentDrawCount % 10 == 0 )
			
			 	LogPlug::info( "Drawing value " + Ceylan::toString( newValue ) + "." ) ;
			*/
			
			distributionTable[ newValue - sampleStart ] += 1 ;
		}
		
		totalDrawCount = fourthDrawCount ;
	
		UprightRectangle fourthDrawingArea( 100, 380, graphWidth, graphHeight ) ;
			
		screen.displayData( distributionTable, sampleCount, pencilColor, captionColor, backColor, 
			"Distribution of " + Ceylan::toString( totalDrawCount )
			+ " samples", & fourthDrawingArea ) ;

		/*

		LogPlug::info( "Displaying final distribution table : " ) ;
	
		for ( unsigned int i = 0 ; i < sampleStop - sampleStart; i++)
			LogPlug::info( Ceylan::toString( i + sampleStart ) + " occured " 
				+ Ceylan::toString( distributionTable[ i ] ) + " time(s)." ) ;
		*/	
				
		screen.unlock() ;
				
		screen.update() ;	
		
		if ( screenshotWanted )
			screen.savePNG( argv[0] + std::string( ".png" ) ) ;
		
		myOSDL.getEventsModule().waitForAnyKey() ;
				
		LogPlug::info( "End of OSDL Surface test." ) ;
		
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


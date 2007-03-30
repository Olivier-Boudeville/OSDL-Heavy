#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;


using namespace Ceylan::Log ;
using namespace Ceylan::Maths::Random ;


#include <string>



/// Images names :
const std::string firstImageFile  = "Soldier-heavy-purple-small.png" ;



/**
 * Small usage tests for Surface transformations.
 *
 */ 
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;


    try 
	{
			

    	LogPlug::info( "Testing OSDL surface transformations" ) ;	


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
				// Ignored :
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
		
		LogPlug::info( "Informations about this screen surface : " 
			+ screen.toString() ) ;
				

		screen.lock() ;

		screen.drawGrid() ;
		
		Ceylan::System::FileLocator imageFinder ;
				
		// When run from playTests.sh build directory :
		imageFinder.addPath( "../../src/doc/web/images" ) ;

		// When run from executable build directory :
		imageFinder.addPath( "../src/doc/web/images" ) ;
	
		// When run from executable install directory :
		imageFinder.addPath( "../OSDL/doc/web/images" ) ;
		
 		screen.loadImage( imageFinder.find( firstImageFile ), 
			/* blit only */ true ) ;
				
		screen.unlock() ;
		
		screen.update() ;

		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;
		
					
		Surface & flipV = screen.flipVertical() ;
		flipV.blitTo( screen, TwoDimensional::Point2D::Origin ) ;
		Surface & clonedSurface( * dynamic_cast<Surface * >( 
			& flipV.clone() ) ) ;
			
		delete & flipV ;
		screen.update() ;

		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;


		Surface & flipH = screen.flipHorizontal() ;
		flipH.blitTo( screen, TwoDimensional::Point2D::Origin ) ;
		delete & flipH ;
		screen.update() ;
						
		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;


		// Relies on clipping :
		
		clonedSurface.blitTo( screen, -30, 30 ) ;
		screen.update() ;
				
		clonedSurface.blitTo( screen, -130, 130 ) ;
		screen.update() ;
		
		Surface * stretched = & clonedSurface.zoom( 1, 0.3 ) ;
		stretched->blitTo( screen, -30, 30 ) ;		
		delete stretched ;
		
		clonedSurface.resize( 150, 400, /* scale content */ true ) ;
		clonedSurface.blitTo( screen, 30, 10 ) ;		
		
		screen.update() ;
		
		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;
		
		
		// Finish with a correct image :
		clonedSurface.blitTo( screen, TwoDimensional::Point2D::Origin ) ;
		delete & clonedSurface ;
		
		
		// Add some random bordered discs :

		screen.lock() ;		
	
    	LogPlug::info( "Prerequesite : having three random generators" ) ;	
		
		Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0, 
			screenWidth ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0, 
			screenHeight ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator radiusRand( 0, 100 ) ;
		
		
		Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
	
		Coordinate x, y ;
			
		ColorElement red ;
		ColorElement green ;
		ColorElement blue ;
		ColorElement alpha ;
		
		for ( Ceylan::Uint32 i = 0; i < 100; i++ )
		{
								
			x = abscissaRand.getNewValue() ;			
			y = ordinateRand.getNewValue() ;
			
			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;
			
			Pixels::ColorDefinition discColorDef =
				Pixels::convertRGBAToColorDefinition( 
				red, green, blue, alpha ) ;
				
			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;
			
			Pixels::ColorDefinition ringColorDef =
				Pixels::convertRGBAToColorDefinition( 
				red, green, blue, alpha ) ;
			
			/*
			 * Do not order radiuses, hence one disc out of two should not 
			 * be drawn.
			 *
			 */
			screen.drawDiscWithEdge( x, y, 
				radiusRand.getNewValue(), radiusRand.getNewValue(),
				discColorDef, ringColorDef ) ;
					
		}
	
		// Draws a single blue disc with white edges on top of all :
		screen.drawDiscWithEdge( /* x */ 50, /* y */ 50, /* outer radius */ 50, 
			/* inner radius */ 45 ) ;
		
		screen.unlock() ;

		screen.update() ;

		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;
	
				
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;
		
		LogPlug::info( "End of OSDL Surface transformation test." ) ;
		
		
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


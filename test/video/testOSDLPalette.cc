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


		LogPlug::info( "Testing OSDL palette services." ) ;
		
		
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
				// Ignored:
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
					"Unexpected command line argument: " + token ) ;
			}
		
		}
		
					

		OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
				CommonModule::UseVideo | CommonModule::UseEvents ) ;	
				
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 

		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
		
		Surface & screen = myVideo.getScreenSurface() ;


		LogPlug::info( "Creating greyscale palette." ) ;
		Palette & greyPal = Palette::CreateGreyScalePalette( screenHeight ) ;


		LogPlug::info( "Displaying greyscale palette: " 
			+ greyPal.toString() ) ;		

		greyPal.draw( screen ) ;
	
	
		screen.update() ;
						
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
		
		delete & greyPal ;
		
		
		Palette & colorPal = Palette::CreateGradationPalette(
			Pixels::MidnightBlue, Pixels::DeepPink, screenHeight ) ;

		LogPlug::info( "Displaying colored palette: " + colorPal.toString() ) ;
		
		colorPal.draw( screen ) ;
		
		screen.update() ;
		
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
		
		delete & colorPal ;
		
		Palette & masterPal = Palette::CreateMasterPalette() ;

		LogPlug::info( "Displaying master palette: " + masterPal.toString() ) ;
		
		const string unencodedMasterPalFilename = "testOSDLPalette.rgb" ;
		LogPlug::info( "Saving it in unencoded format in '" 
			+ unencodedMasterPalFilename + "'." ) ;
		masterPal.save( unencodedMasterPalFilename, /* encoded */ false ) ;
		
		const string encodedMasterPalFilename = "testOSDLPalette.pal" ;
		LogPlug::info( "Saving it in encoded format in '" 
			+ encodedMasterPalFilename + "'." ) ;
		masterPal.save( encodedMasterPalFilename, /* encoded */ true ) ;
		
		/*
		 * Checking correctness: color #6 is [ 0 ; 36 ; 63] (in 8-bit).
		 * It should be in bytes 12 and 13 of the .pal, which are:
		 * 0x9C80 = 40064 = 1001110010000000 = 1 00111 00100 00000 in BGR order.
		 * so in [0;31] R = 00000 = 0, G = 00100 = 4, B = 00111 = 7 which
		 * in [0;255] is R = 0, G = 33, B = 58. The small differences compared
		 * to the color #6 are quantization errors.
		 *
		 */
		 
		masterPal.draw( screen ) ;
		
		screen.update() ;
		
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
		
		delete & masterPal ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		LogPlug::info( "End of OSDL palette test." ) ;
		
	}
		
    catch ( const OSDL::Exception & e )
    {
	
        LogPlug::error( "OSDL exception caught: "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught: "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught: " 
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


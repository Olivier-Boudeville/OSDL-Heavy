#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;
using namespace OSDL::Rendering ;

using namespace Ceylan::Log ;


#include <string>


		
void informShape( Length width, Length height )
{

	LogPlug::info( "A sprite whose dimensions would be width = "
		+ Ceylan::toString( width ) + " and height = " 
		+ Ceylan::toString( height ) + " would be placed in a "
		+ Sprite::DescribeShape( 
			Sprite::GetSmallestEnclosingShape( width, height ) )
		+ " shape." ) ;

}




/**
 * Test for the Sprite class.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL Sprite class." ) ;	
		

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
			
				
		LogPlug::info( "Testing assigned sprite shapes for various dimensions."
			) ;		
		
		// 8x8:	
 		informShape(  1,  2 ) ;			
 		informShape(  1,  8 ) ;			
		
		// 16x8:
 		informShape(  9,  7 ) ;			

		// 32x8:
 		informShape(  19,  7 ) ;			


		// 8x16:
 		informShape(  1,  9 ) ;			
		
		// 16x16:
 		informShape( 16,  9 ) ;			

		// 32x16:
 		informShape( 32,  9 ) ;			


		// 8x32:
 		informShape( 5, 32 ) ;	
		
		// 16x32:
 		informShape( 16, 32 ) ;	
		
		// 32x32:		
 		informShape( 31, 32 ) ;			
 		informShape( 32, 32 ) ;	
		
		// 64x32:		
		informShape( 33, 32 ) ;
		
		// 32x64:
		informShape( 32, 33 ) ;
		
		// 64x64:
		informShape( 45, 64 ) ;
		
		// Exception correctly raised: informShape( 65, 33 ) ;
			
		
		/*
		
    	LogPlug::info( "Pre requesite: initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
  
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		*/		
				
		LogPlug::info( "End of OSDL sprite test." ) ;
		
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

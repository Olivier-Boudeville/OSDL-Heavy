#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;


#include "Ceylan.h"
using namespace Ceylan::Log ;



/**
 * Test for pixel-level operations.
 *
 */
int main( int argc, char * argv[] ) 
{


    LogHolder myLog( argc, argv ) ;
	
		
    try 
	{
	
	
		LogPlug::info( "Testing OSDL video's pixel services" ) ;
			
		LogPlug::info( "Starting OSDL with video enabled" )	;
			
		OSDL::CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo ) ;		

		LogPlug::info( "Displaying pixel format of the current video mode : "
			+ Pixels::toString( VideoModule::GetVideoDevicePixelFormat() ) + "." ) ;	
		
		LogPlug::info( "Initializing video") ;
		VideoModule & myVideo = myOSDL.getVideoModule() ; 

		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;

		LogPlug::info( myVideo.toString() ) ;
				
		LogPlug::info( "Displaying pixel format of the current video mode : "
			+ Pixels::toString( VideoModule::GetVideoDevicePixelFormat() ) + "." ) ;	
			
		Surface & screen = myVideo.getScreenSurface() ;
		
		Pixels::ColorDefinition pureBlackDef = Pixels::convertRGBAToColorDefinition( 0, 0, 0, 0 ) ;
	 
	 	// Should be RGBA = [0;0;0;255] :
		Pixels::ColorDefinition readDef = screen.getColorDefinitionAt( 10, 10 ) ;
		
		if ( ! Pixels::areEqual( pureBlackDef, readDef, /* use alpha */ false ) )
			throw Ceylan::TestException( "Color definitions should have been deemed equal "
				"since alpha coordinate was to be ignored" ) ;
				
		if ( Pixels::areEqual( pureBlackDef, readDef, /* use alpha */ true ) )
			throw Ceylan::TestException( "Color definitions should have been deemed equal "
				"since alpha coordinate was to be ignored" ) ;
		
		LogPlug::info( "Stopping OSDL" ) ;		
		OSDL::stop() ;

		LogPlug::info( "End of OSDL pixel test." ) ;
		
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


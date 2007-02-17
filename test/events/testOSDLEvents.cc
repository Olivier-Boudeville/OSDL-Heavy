#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Events ;


using namespace Ceylan::Log ;



#include <iostream>  // for cout



void exampleKeyHandler( const KeyboardEvent & keyboardEvent )
{

	// If 'q' is hit, quit :
	
	if ( keyboardEvent.type == EventsModule::KeyPressed )
	{
	
		if ( static_cast<KeyboardHandler::KeyIdentifier>(
			keyboardEvent.keysym.sym ) == KeyboardHandler::qKey )
			OSDL::getExistingCommonModule().getEventsModule().requestQuit() ;
	}

}


int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Testing OSDL events basic services." ) ;

		LogPlug::info( "Starting OSDL with video and, "
			"therefore, events enabled." ) ;	
				
        OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;		
			
		LogPlug::info( "Testing basic event handling." ) ;
		
		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ; 

		LogPlug::info( EventsModule::DescribeEnvironmentVariables() ) ;
				
		LogPlug::info( "Displaying a dummy window "
			"to have access to an event queue." ) ;
			
		LogPlug::info( "Getting video." ) ;
		OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		// A window is needed to have the SDL event system working :
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
		
		LogPlug::info( "Entering the event loop for key press waiting." ) ;
		myEvents.waitForAnyKey() ;
		
		LogPlug::info( 
			"Registering a new keyboard key handler converter, for 'q' key." ) ;
			
		myEvents.getKeyboardHandler().setDefaultRawKeyHandler( 
			exampleKeyHandler ) ;

		LogPlug::info( "Entering main loop, press 'q' to quit." ) ;		
		myEvents.enterMainLoop() ;
		LogPlug::info( "Exiting main loop." ) ;		
		
		LogPlug::info( "stopping OSDL." ) ;		
        OSDL::stop() ;

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


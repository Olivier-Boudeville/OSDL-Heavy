#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Events ;


#include "Ceylan.h"
using namespace Ceylan::Log ;



#include <iostream>  // for cout


/* 

	Key converters have been deprecated, therefore there is not much to test here.
	
	
	

/// User-defined AbstractingEventConverter :
UserEventType myKeyboardEventConverter( const BasicEvent & aBasicEvent )
{

	// Returns a 'quit' user event iff 'q' key is pressed :
	
	 * The first test should be useless since the event mapping should trigger only KeyPressed
	 * events.
	 *
	
	if ( aBasicEvent.type == EventsModule::KeyPressed )
	{
	
		std::cout << "Key " << SDL_GetKeyName( aBasicEvent.key.keysym.sym ) 
			<< " has been pressed" ; 
			
		if ( aBasicEvent.key.keysym.mod & LeftShiftModifier )
			std::cout << " with left shift modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & RightShiftModifier )
			std::cout << " with right shift modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & LeftControlModifier )
			std::cout << " with left control modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & RightControlModifier )
			std::cout << " with right control modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & LeftAltModifier )
			std::cout << " with left alt modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & RightAltModifier )
			std::cout << " with right alt modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & LeftMetaModifier )
			std::cout << " with left meta modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & RightMetaModifier )
			std::cout << " with right meta modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & NumModifier )
			std::cout << " with num lock modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & CapsModifier )
			std::cout << " with caps lock modifier." << std::endl ; 
		
		if ( aBasicEvent.key.keysym.mod & ModeModifier )
			std::cout << " with mode modifier." << std::endl ; 
						
				
		if ( aBasicEvent.key.keysym.sym == qKey )
		{
			LogPlug::debug( "Q key spotted by user-defined event converter !" ) ;			
			return EventsModule::QuitRequested ;
		}	
		
		if ( aBasicEvent.key.keysym.sym == EnterKey )
		{
			LogPlug::debug( "Enter key spotted by user-defined event converter !" ) ;			
			return EventsModule::QuitRequested ;
		}
			
		LogPlug::debug( "No specifically redirected key spotted "
			"by user-defined event converter !" ) ;			
		return EventsModule::NoEvent ;
	}
	else
	{
		LogPlug::error( "testOSDLEvents : myKeyboardEventConverter received wrong event type." ) ;
		return EventsModule::NoEvent ;
	}	
}

*/

void exampleKeyHandler( const KeyboardEvent & keyboardEvent )
{

	// If 'q' is hit, quit :
	
	if ( keyboardEvent.type == EventsModule::KeyPressed )
	{
	
		if ( static_cast<KeyboardHandler::KeyIdentifier>( keyboardEvent.keysym.sym ) 
				== KeyboardHandler::qKey )
			OSDL::getExistingCommonModule().getEventsModule().requestQuit() ;
	}

}


int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Testing OSDL events basic services." ) ;

		LogPlug::info( "Starting OSDL with video and, therefore, events enabled." ) ;		
        OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;		
			
		LogPlug::info( "Testing basic event handling." ) ;
		
		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ; 

		LogPlug::info( EventsModule::DescribeEnvironmentVariables() ) ;
				
		LogPlug::info( "Displaying a dummy window to have access to an event queue." ) ;
			
		LogPlug::info( "Getting video." ) ;
		OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		// A window is needed to have the SDL event system working :
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
		
		LogPlug::info( "Entering the event loop for key press waiting." ) ;
		myEvents.waitForAnyKey() ;
		
		LogPlug::info( "Registering a new keyboard key handler converter, for 'q' key." ) ;
		myEvents.getKeyboardHandler().setDefaultRawKeyHandler( exampleKeyHandler ) ;

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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Events ;
using namespace OSDL::Video ;

#include "Ceylan.h"
using namespace Ceylan::Log ;



#include <iostream>  // for cout, endl



class MyController : public OSDL::MVC::Controller
{

	public:
	
	
		MyController( EventsModule & eventsModule ) throw() :
			_eventsModule( & eventsModule )
		{
		
		
		}
		
		
		void rawKeyPressed( const KeyboardEvent & keyboardPressedEvent ) throw()
		{
			LogPlug::info( "A key was pressed, exiting." ) ;
			_eventsModule->requestQuit() ;
		}
		
		
		void joystickAxisChanged( const JoystickAxisEvent & joystickAxisEvent )	throw()
		{
			std::cout << "Joystick is moving..." << std::endl ;
		}
		
		
		const Ceylan::Event & getEventFor( const Ceylan::CallerEventListener & listener ) 
			throw( Ceylan::EventException )
		{
			throw Ceylan::EventException( "MyController::getEventFor : "
				"not expected to be called." ) ;
		}
		
		
	protected:	
		
		EventsModule * _eventsModule ;
		
		
} ;




/**
 * Testing OSDL joystick handling.
 *
 */
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Testing OSDL joystick services." ) ;

		LogPlug::info( "Starting OSDL with joystick support." ) ;		
        OSDL::CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseJoystick ) ;		
		
		LogPlug::info( "Testing basic event handling." ) ;
		
		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ; 
		
		LogPlug::info( "Events module : " + myEvents.toString() ) ;
		
		JoystickHandler & myJoystickHandler = myEvents.getJoystickHandler() ;
		LogPlug::info( "Current joystick handler is : " 
			+ myJoystickHandler.toString( Ceylan::high ) ) ;

		unsigned int joyCount = myJoystickHandler.GetAvailableJoystickCount()  ;
		LogPlug::info( "There are " + Ceylan::toString( joyCount )
			+ " attached joystick(s), opening them all." ) ;
			
		for ( unsigned int i = 0 ; i < joyCount; i++ )
			myJoystickHandler.openJoystick( i ) ;
		
		LogPlug::info( "New joystick handler state is : " 
			+ myJoystickHandler.toString( Ceylan::high ) ) ;
		
		if ( joyCount == 0 )
		{
			LogPlug::info( "No joystick to test, stopping test." ) ;
			OSDL::stop() ;
			return Ceylan::ExitSuccess ;		
		}
		else
		{
			LogPlug::info( "Using the first joystick" ) ;
		}
		
		MyController myController( myEvents ) ;
		
		myJoystickHandler.linkToController( /* first joystick */ 0,  myController ) ;
			
		LogPlug::info( "Displaying a dummy window to have access to an event queue." ) ;
			
		LogPlug::info( "Getting video." ) ;
		OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ; 

		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		// A SDL window is needed to have the SDL event system working :
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
		
		LogPlug::info( "Entering the event loop for key press waiting." ) ;
		//myEvents.waitForAnyKey() ;
		
		std::cout << "Push the first button of joystick or hit any key to stop this test" 
			<< std::endl ;
		
		LogPlug::info( "Entering main loop." ) ;		
		myEvents.enterMainLoop() ;
		LogPlug::info( "Exiting main loop." ) ;		
				
		LogPlug::info( "End of joystick test." ) ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Events ;


using namespace Ceylan::Log ;


#include <iostream>  // for cout
using std::cout ;
using std::endl ;

#include <string>
using std::string ;




class MyController : public OSDL::MVC::Controller
{

	public:
	
	
		MyController( EventsModule & events ) throw() :
			_events( & events )
		{
		
		
		}
		
		
		void joystickButtonPressed( 
			const JoystickButtonEvent & joystickButtonPressedEvent ) throw()
		{
		
			_events->requestQuit() ;
			
		}
		
		
		void rawKeyPressed( const KeyboardEvent & keyboardPressedEvent ) throw()
		{
			
			switch( keyboardPressedEvent.keysym.sym )
			{
			
				case KeyboardHandler::UpArrowKey:
					_direction = 1 ;	
					break ;
					
				case KeyboardHandler::DownArrowKey:
					_direction = 2 ;	
					break ;
					
				case KeyboardHandler::LeftArrowKey:
					_direction = 3 ;	
					break ;
					
				case KeyboardHandler::RightArrowKey:
					_direction = 4 ;	
					break ;
				
				case KeyboardHandler::EnterKey:
					_events->requestQuit() ;
					break ;
				
				default:
					// Do nothing.
					break ;	
			}	
			
			cout << "                  " << toString() ;	
	
				
		}
		


		void joystickUp( AxisPosition leftExtent ) throw()
		{
		
			_direction = 1 ;
			cout << "                  " << toString() ;
				
		}
	
		
		void joystickDown( AxisPosition leftExtent ) throw()
		{
		
			_direction = 2 ;
			cout << "                  " << toString() ;
				
		}
		
		
		void joystickLeft( AxisPosition leftExtent ) throw()
		{
		
			_direction = 3 ;
			cout << "                  " << toString() ;
				
		}
		
		
		void joystickRight( AxisPosition leftExtent ) throw()
		{
		
			_direction = 4 ;
			cout << "                  " << toString() ;	
			
		}
		
		
		const Ceylan::Event & getEventFor( 
				const Ceylan::CallerEventListener & listener ) 
			throw( Ceylan::EventException )
		{
		
			throw Ceylan::EventException( "MyController::getEventFor : "
				"not expected to be called." ) ;
				
		}
		
		
		/* 
		
		 If no classical joystick is to be used :
		 
		void joystickAxisChanged( const JoystickAxisEvent & joystickAxisEvent )
			throw()
		{
		
			// Dead zones : [-1000; 1000] 
		
			if ( joystickAxisEvent.axis == 0 )
			{
				if ( joystickAxisEvent.value < -1000 )
					_direction = 3 ;
				else if ( joystickAxisEvent.value > -1000 )
					_direction = 4 ;
			}		
			else if ( joystickAxisEvent.axis == 1 )
			{
				if ( joystickAxisEvent.value < -1000 )
					_direction = 1 ;
				else if ( joystickAxisEvent.value > -1000 )
					_direction = 2;
			}
					
			cout << "                  " << toString() ;	
					
		}
		
		*/
		
		
		const string toString( Ceylan::VerbosityLevels level = Ceylan::high )
			const throw()	
		{
		
			switch( _direction )
			{
				case 1:
					return "^" ;
				case 2 : 
					return "v" ;
				case 3 :
					return "<" ;
				case 4 :
					return ">" ;
			}
			
			return "(unexpected direction selected)" ;
						
		}
		
		
	protected:
	
		/// 1 : up, 2 : down, 3 : left, 4 : right.
		Ceylan::Uint8 _direction ;
		
		EventsModule * _events ;
			
} ;



/**
 * Testing OSDL Controller integration with input layer.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{ 

		
		LogPlug::info( "Testing OSDL controller to input device bridge." ) ;

		
		LogPlug::info( "Starting OSDL with keyboard joystick support." ) ;	
		
		// Will trigger the video module as well for events :	
        OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseJoystick | CommonModule::UseKeyboard ) ;		
		
		LogPlug::info( "Testing basic event handling." ) ;
		
		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ; 
		
		LogPlug::info( "Events module : " + myEvents.toString() ) ;
		
		myEvents.getKeyboardHandler().setSmarterDefaultKeyHandlers() ;
		
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
		
		LogPlug::info( "Displaying a dummy window "
			"to have access to an event queue." ) ;
			
		LogPlug::info( "Getting video." ) ;
		OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		// A window is needed to have the event system working :
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
		
		MyController aController( myEvents ) ;
		
					
		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::DownArrowKey, aController ) ;
			
		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::LeftArrowKey, aController ) ;
			
		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::RightArrowKey, aController ) ;
		
		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::EnterKey, aController ) ;

		
		if ( joyCount == 0 )
		{
			LogPlug::info( "No joystick detected, no test performed." ) ;
			return 0 ;
		}	

		std::cout << "(when the joystick is pushed, a character "
			"('<' or '>' or '^' or 'v', for left, right, up and down) "
			"describes the direction it is aimed at)" << std::endl
			<< "(press the enter key or a joystick button to exit)" ;

		
		myJoystickHandler.linkToController( /* JoystickNumber */ 0, 
			aController ) ;
		
		LogPlug::info( "Entering the event loop "
			"for event waiting so that Controller can act." ) ;
		
		LogPlug::info( "Entering main loop." ) ;		
		myEvents.enterMainLoop() ;
		LogPlug::info( "Exiting main loop." ) ;		
				
		LogPlug::info( "End of OSDL controller test." ) ;
		
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


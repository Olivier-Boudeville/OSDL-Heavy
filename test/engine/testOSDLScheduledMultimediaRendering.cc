#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Events ;
using namespace OSDL::Video ;

using namespace Ceylan::Log ;



#include <iostream>     // for cout
using std::cout ;
using std::endl ;

#include <string>
using std::string ;

#include <list>
using std::list ;


#if OSDL_TEST_VERBOSE

#define OSDL_DISPLAY_DEBUG( message ) std::cout << "[testOSDLScheduledMultimediaRendering] " << message << endl ;

#else // OSDL_TEST_VERBOSE

#define OSDL_DISPLAY_DEBUG( message )

#endif // OSDL_TEST_VERBOSE




/**
 * Tests the whole scheduling and rendering framework : the scheduler 
 * manages a root renderer which is a multimedia renderer. 
 *
 * This one uses an internal video renderer, and an internal audio 
 * renderer.
 *
 */


/// Directions are : 1 : up, 2 : down, 3 : left, 4 : right.
typedef Ceylan::Uint8 Direction ;



/**
 * Very basic multimedia renderer which just uses a list to store its 
 * registered views.
 *
 */
class MyBasicMultimediaRenderer : public OSDL::Rendering::MultimediaRenderer
{


	public:

	
		MyBasicMultimediaRenderer() 
				throw( OSDL::Rendering::RenderingException ) :
			MultimediaRenderer( /* registerToScheduler */ true ),
			_views()
		{
		
		}
		
		
		~MyBasicMultimediaRenderer() throw()
		{
		
		}
		
		
		void registerView( Ceylan::View & newView ) throw()
		{
		
			OSDL_DISPLAY_DEBUG( "View registered to multimedia renderer !" ) ;
			_views.push_back( & newView	) ;	
			
		}
		
		
		/**
		 * Called by the scheduler when a new rendering tick occurs, makes
		 * the views display themselves.
		 *
		 */
		void renderVideo( Events::RenderingTick currentRenderingTick = 0 )
			throw()
		{
		
			OSDL_DISPLAY_DEBUG( "Multimedia renderer is rendering video, with " 
				<< _views.size() << " view(s) being registered !" ) ;

			OSDL::Rendering::Renderer::render( currentRenderingTick ) ;
			for ( std::list<Ceylan::View *>::iterator it = _views.begin() ;
					it != _views.end(); it++ )
				(*it)->renderModel() ;
				
		}
		
		
		/// Mute test case.
		void renderAudio( Events::RenderingTick currentRenderingTick = 0 )
			throw()
		{
		
			OSDL_DISPLAY_DEBUG( "Multimedia renderer is rendering audio, with " 
				<< _views.size() << " view(s) being registered !" ) ;
				
		}
		
		
		
		
	protected:	

		std::list<Ceylan::View *> _views ;

} ;




class MyMVCEvent : public Ceylan::MVCEvent
{

	public:


		MyMVCEvent( Ceylan::EventSource & source ) throw() : 
			 MVCEvent( source ),
			 _direction( /* up */ 1  )
		{
		
		}
		
		
		void setDirection( Direction newDirection ) throw()
		{
			_direction = newDirection ;
		}
		
		
		Direction getDirection() const throw() 
		{
			return _direction ;
		}
			
		
	protected:
	
		Direction _direction ;
					 
} ;




class MyController : public OSDL::MVC::Controller
{


	public:
	
	
		MyController() throw() :
			_eventForModel( * this )
		{	
		
		}
		
				
		void rawKeyPressed( const KeyboardEvent & keyboardPressedEvent ) throw()
		{

			
			switch( keyboardPressedEvent.keysym.sym )
			{
			
				case KeyboardHandler::UpArrowKey:
					OSDL_DISPLAY_DEBUG( "Controller updated with key up !" ) ;
					_eventForModel.setDirection( 1 ) ;	
					break ;
					
				case KeyboardHandler::DownArrowKey:
					OSDL_DISPLAY_DEBUG( "Controller updated with key down !" ) ;
					_eventForModel.setDirection( 2 ) ;	
					break ;
					
				case KeyboardHandler::LeftArrowKey:
					OSDL_DISPLAY_DEBUG( "Controller updated with key left !" ) ;
					_eventForModel.setDirection( 3 ) ;	
					break ;
					
				case KeyboardHandler::RightArrowKey:
					OSDL_DISPLAY_DEBUG( 
						"Controller updated with key right !" ) ;
					_eventForModel.setDirection( 4 ) ;	
					break ;
				
				case KeyboardHandler::EnterKey:
					OSDL_DISPLAY_DEBUG( 
						"Controller updated with enter key !" ) ;
					_eventForModel.setDirection( 5 ) ;	
					break ;
				
				default:
					// Do nothing.
					OSDL_DISPLAY_DEBUG( 
						"Controller updated with key not registered !" ) ;
					break ;	
			}	
			
			
			// We are schedule-driven here, nothing to propagate.
						
		}
		

		void joystickFirstButtonPressed() throw()	
		{
		
			OSDL_DISPLAY_DEBUG( "Controller updated !" ) ;
			_eventForModel.setDirection( 5 ) ;	
			
		}
		
		
		void joystickUp( AxisPosition leftExtent ) throw()
		{
		
			OSDL_DISPLAY_DEBUG( "Controller updated !" ) ;
			_eventForModel.setDirection( 1 ) ;
			
		}
	
		
		void joystickDown( AxisPosition leftExtent ) throw()
		{
		
			OSDL_DISPLAY_DEBUG( "Controller updated !" ) ;
			_eventForModel.setDirection( 2 ) ;
			
		}
		
		
		void joystickLeft( AxisPosition leftExtent ) throw()
		{
		
			OSDL_DISPLAY_DEBUG( "Controller updated !" ) ;
			_eventForModel.setDirection( 3 ) ;
			
		}
		
		
		void joystickRight( AxisPosition leftExtent ) throw()
		{
		
			OSDL_DISPLAY_DEBUG( "Controller updated !" ) ;
			_eventForModel.setDirection( 4 ) ;
			
		}
		
		
		const Ceylan::Event & getEventFor( 
				const Ceylan::CallerEventListener & listener )	
			throw( Ceylan::EventException )
		{
		
			OSDL_DISPLAY_DEBUG( "Controller interrogated, returning state " 
				<< Ceylan::toNumericalString( _eventForModel.getDirection() )
				<< " !" ) ;
			return _eventForModel ;
			
		}
				
		
		/// Not used here.
		const string toString( Ceylan::VerbosityLevels level = Ceylan::high )
			const throw()	
		{
		
			switch( _eventForModel.getDirection() )
			{
				case 1:
					return "^" ;
					
				case 2 : 
					return "v" ;
					
				case 3 :
					return "<" ;
					
				case 4 :
					return ">" ;
					
				case 5 :
					return "quit ! (seen by controller)" ;
					
			}
			
			return "(unexpected direction selected) : " 
				+ Ceylan::toString( _eventForModel.getDirection() ) ;
						
		}
		
		
		
	protected:
	

		/// This event stores the current direction.
		MyMVCEvent _eventForModel ;
					
} ;




class MyModel : public OSDL::MVC::Model
{

	public:
	
	
		MyModel() :
			Model( /* autoRegister */ true, /* period */ 1, 
				/* policy */ Engine::relaxed ),
			_eventForView( * this )			
		{
		
		}
		
		
		// Not used there : not event-driven.
		virtual void beNotifiedOf( const Ceylan::Event & newEvent ) throw()
		{
		
			const MyMVCEvent * event = dynamic_cast<const MyMVCEvent *>( 
				& newEvent ) ;
			
			if ( event != 0 )
			{
				_eventForView.setDirection( event->getDirection() ) ;
			}
			
			// We are event-driven here :
			notifyAllViews( _eventForView ) ;
			
		}
	
	
		const Ceylan::Event & getEventFor( 
				const Ceylan::CallerEventListener & listener )	
			throw( Ceylan::EventException )
		{
		
			OSDL_DISPLAY_DEBUG( "Model interrogated, returning state " 
				<< Ceylan::toNumericalString( _eventForView.getDirection() )
				<< " !" ) ;
				
			return _eventForView ;
		}
		
	
	 	virtual void onActivation( Events::SimulationTick newTick ) throw()
		{
		
			OSDL_DISPLAY_DEBUG( "Model activated !" ) ;
			
			OSDL_DISPLAY_DEBUG( "Model state : " + toString() ) ;
			
			// Only one event source, must be the controller :	
			MyController * myController = dynamic_cast<MyController *>(
				_sources.back() ) ;
			
			const MyMVCEvent * myEvent = dynamic_cast<const MyMVCEvent *>( 
				& myController->getEventFor( * this ) ) ;
				
			_eventForView.setDirection( myEvent->getDirection() ) ;
			
		}
	
	
	protected:
			
		MyMVCEvent _eventForView ;
					
} ;




class MyView : public Ceylan::View
{


	public:
	
	
		MyView( Ceylan::Model & model, EventsModule & events ) throw() :
			View( model ),
			_actualDirection( 1 ),
			_events( & events )
		{
		
		}
				
		
		virtual void renderModel() throw()
		{
				
			OSDL_DISPLAY_DEBUG( "View displaying !" ) ;


			// First retrieves from the model the necessary informations :
			Ceylan::Model * myModel ;
			
			// Actually only one source : 
			for ( std::list<Ceylan::EventSource *>::iterator 
				it = _sources.begin() ;	it != _sources.end(); it++ )
			{
			
				myModel = dynamic_cast<Ceylan::Model *>( *it ) ;
				const MyMVCEvent * myModelEvent = 
					dynamic_cast<const MyMVCEvent *>( 
						& myModel->getEventFor( * this ) ) ;
				
				// Directions could be blended :	
				_actualDirection = myModelEvent->getDirection() ;
				
			}

			OSDL_DISPLAY_DEBUG( "View state : " + toString() ) ;
								
			switch( _actualDirection )
			{
				case 1:
					cout << "^" << endl ;
					break ;
					
				case 2 : 
					cout << "v" << endl ;
					break ;
					
				case 3 :
					cout << "<" << endl ;
					break ;
					
				case 4 :
					cout << ">" << endl ;
					break ;
					
				case 5 :
					cout << "quit ! "
						"(seen by the view, actual end of event loop)" << endl ;
					_events->requestQuit() ;	
					break ;
					
				default:
					cout << "Unexpected direction selected : " 
						 << _actualDirection << endl ;	
					break ;
			}
						
		}
	
	
		/// Not used here since not event-driven.
		virtual void beNotifiedOf( const Ceylan::Event & newEvent ) throw()
		{
		
			const MyMVCEvent * event = dynamic_cast<const MyMVCEvent *>( 
				& newEvent ) ;
			
			if ( event != 0 )
			{
				_actualDirection = event->getDirection() ;
			}
			
			// Then render !
			renderModel() ;
			
		}


	private:
	
		Direction _actualDirection ;
		EventsModule * _events ;
		
} ;
		


/**
 * Testing scheduled OSDL MVC integration with input layer and multimedia
 * renderer.
 *
 * This test creates MVC instances as automatic variables, to test the
 * framework robustness : it is a typical example of a misuse that should 
 * have led to a crash, since the deallocation order of aController, aModel 
 * and aModel is not mastered.
 *
 * Nevertheless counter-measures are applied, and traced through the log 
 * system, so that the user is notified that the life cycle of his objects
 * is wrong.
 *
 * The right solution would have been to use block to control the variable
 * scopes, or to create instances thanks to new/delete pairs.
 * 
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

					
		LogPlug::info( "Testing OSDL scheduled multimedia integration." ) ;

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

		LogPlug::info( "Starting OSDL with joystick support." ) ;		
        OSDL::CommonModule & myOSDL = OSDL::getCommonModule(
			CommonModule::UseJoystick | CommonModule::UseKeyboard ) ;		
		
		LogPlug::info( "Testing event handling." ) ;
		
		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ; 
		
		LogPlug::info( "Scheduler support requested." ) ;
		myEvents.useScheduler() ;
		
		LogPlug::info( "Events module : " + myEvents.toString() ) ;
		
		myEvents.getKeyboardHandler().setSmarterDefaultKeyHandlers() ;
		
		JoystickHandler & myJoystickHandler = myEvents.getJoystickHandler() ;
		LogPlug::info( "Current joystick handler is : " 
			+ myJoystickHandler.toString( Ceylan::high ) ) ;

		JoystickNumber joyCount = 
			myJoystickHandler.GetAvailableJoystickCount() ;
			
		if ( joyCount > 0 )	
		{
		
			LogPlug::info( "There are " + Ceylan::toString( joyCount )
				+ " attached joystick(s), opening them all." ) ;
			
			for ( JoystickNumber i = 0 ; i < joyCount; i++ )
				myJoystickHandler.openJoystick( i ) ;
		
			LogPlug::info( "New joystick handler state is : " 
				+ myJoystickHandler.toString( Ceylan::high ) ) ;
		
		}
		else
		{
		
			LogPlug::info( "There is no joystick attached." ) ;
		
		}

		LogPlug::info( 
			"Displaying a dummy window to have access to an event queue." ) ;
			
		LogPlug::info( "Getting video." ) ;
		OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		// A SDL window is needed to have the SDL event system working :
		myVideo.setMode( 640, 480, VideoModule::UseCurrentColorDepth, 
			VideoModule::SoftwareSurface ) ;
		
		
		/*
		 * Create and link MVC instances (new and pointers should be used 
		 * to control deallocation order). 
		 *
		 * This test relies on OSDL's MVC ability to overcome faulty life 
		 * cycle, see the Error log channel to understand why (therefore it 
		 * is also a test for robustness to misuse).
		 *
		 */
		 
		MyModel aModel ;
		
		MyView aView( aModel, myEvents ) ;
		
		MyController aController ;
		
		aModel.subscribeToController( aController ) ;
				
		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::UpArrowKey, aController ) ;
			
		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::DownArrowKey, aController ) ;
			
		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::LeftArrowKey, aController ) ;
			
		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::RightArrowKey,	aController ) ;
			
		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::EnterKey, aController ) ;
		
		if ( joyCount > 0 )
			myJoystickHandler.linkToController( /* JoystickNumber */ 0, 
				aController ) ;
		
		
		LogPlug::info( "Creating renderer." ) ;
		
		/*
		 * Auto-registers itself to the scheduler.
		 *
		 * Warning : this renderer must be specially allocated thanks to a
		 * new(), and not simply as an automatic variable, since the 
		 * renderer will automatically register itself to the scheduler,
		 * which will take ownership of it, and will deallocate it upon its 
		 * own destruction. 
		 *
		 * Therefore it must not be an automatic variable whose life cycle 
		 * would be managed independently.
		 *
		 */
		MyBasicMultimediaRenderer * aRenderer = 
			new MyBasicMultimediaRenderer() ;
		
		aRenderer->registerView( aView ) ;

		LogPlug::debug( "Showing scheduler state before activation : " 
			+ OSDL::Engine::Scheduler::GetExistingScheduler().toString() ) ;


		if ( isBatch )
		{
			LogPlug::warning( "Main loop not launched, as in batch mode." ) ;
		}
		else
		{

			LogPlug::info( "Entering the event loop for event waiting "
				"so that Controller can act." ) ;

			std::cout << "< Hit Enter or push the first button "
				"of the first joystick (if any) "
				"to end scheduler-based MVC test >" << std::endl ;
		
			myEvents.enterMainLoop() ;
			LogPlug::info( "Exiting main loop." ) ;	
				
		}


		LogPlug::debug( "Showing scheduler state after activation : " 
			+ OSDL::Engine::Scheduler::GetExistingScheduler().toString() ) ;


		LogPlug::info( "End of OSDL scheduler-based multimedia MVC test." ) ;

		LogPlug::warning( "The MVC were allocated on the stack, hence their "
			"deallocation order is not mastered, messages in error log plug "
			"may appear." ) ;
		
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


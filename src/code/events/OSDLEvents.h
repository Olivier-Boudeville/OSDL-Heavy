#ifndef OSDL_EVENTS_H_
#define OSDL_EVENTS_H_


#include "OSDLEventsCommon.h"    // for typedefs

#include "OSDLTypes.h"           // for Flag
#include "OSDLException.h"       // for OSDL::Exception

#include "Ceylan.h"              // for Uint32

#include "SDL.h"                 // for SDL_Event

#include <string>
#include <map>


namespace OSDL
{


	// The event module is managed by the common module;
	class CommonModule ;
	
	
	namespace Engine
	{
	
		// The event module can use the scheduler.
		class Scheduler ;
	
	}
	
	
	
	/// Deals with all events of interest for a multimedia engine.
	namespace Events
	{
	
	
		// The event module can manage the keyboard.
		class KeyboardHandler ;
		
			
		// The event module can manage the joysticks.
		class JoystickHandler ;
		
		
		
		/**
		 * A tick is the most precise time unit that can be measured by 
		 * the system.
		 *
		 * @note Ticks are usually one millisecond long.
		 *
		 */
		typedef Ceylan::Uint32 Tick ;
		
		
		/**
		 * Engine tick is the most fine unit of time managed by the scheduler.
		 *
		 * @note Their duration is typically one or more microseconds.
		 *
		 */
		typedef Tick EngineTick ;
		
		
		/**
		 * Simulation tick is the most fine unit of time between two 
		 * simulation steps.
		 *
		 * @note Their duration is an integer multiple of the duration of 
		 * the engine ticks.
		 *
		 */
		typedef Tick SimulationTick ;
		
		
		/**
		 * Rendering tick is the most fine unit of time between two 
		 * rendering steps.
		 *
		 * @note Their duration is an integer multiple of the duration of 
		 * the engine ticks.
		 *
		 */
		typedef Tick RenderingTick ;
	
	
		/**
		 * Rendering ticks is the most fine unit of time between two 
		 * input pollings.
		 *
		 * @note Their duration is an integer multiple of the duration of 
		 * the engine ticks.
		 *
		 */
		typedef Tick InputTick ;
		

		/**
		 * Unit for frequencies (Hz). 
		 *
		 * For example, a periodical event whose frequency is 100 Hz would
		 * happen once on each period P = 1/100 s = 10 ms.
		 *
		 * @note For our needs, it is an integer type.
		 *
		 */
		typedef Ceylan::Uint32 Hertz ;


		/**
		 * Allows to count how many engine ticks should be waited by the
		 * scheduler until a new periodic event is triggered.
		 *
		 * Period is a specialization of engine ticks in the case of 
		 * periodic events.
		 *
		 */
		typedef EngineTick Period ;

		
		/**
		 * Basic event type, describing what technically happened.
		 *
		 * @example : 'Escape key was pressed'.
		 *
		 */
		typedef Ceylan::Uint8 BasicEventType  ;
		
		
		/**
		 * Higher level event type, describing what happened with the user.
		 *
		 * @example : 'The user requested to exit'.
		 *
		 */
		typedef Ceylan::Uint8 UserEventType ;
						

			
		/**
		 * Handles the asynchronous event-based interaction model, general 
		 * input handling, and basic time management, including the logic 
		 * and rendering frequencies.
		 *
		 * Our optional scheduler allows to uncouple all these times.
		 *
		 * The user is given the choice between a basic event loop, for which
		 * input management, rendering and logic have to happen at the same
		 * frequency, and a scheduler, that allows him to set a different
		 * frequency for each of these concerns, and permits fine-grained
		 * activation of the scheduled objects, be it periodic or 
		 * specifically programmed.
		 *
		 * Both scheduling choices allow idle calls to be performed if,
		 * CPU-wise, the process can afford it. 
		 *
		 * The default idle call is the smallest possible OS sleep (one time
		 * slice), to avoid the operating system to take counter-measures
		 * against this process, which otherwise would be deemed too 
		 * CPU-hungry.
		 * The user can supply a specific idle callback, if wanted. 
		 *
		 * Under all circumstances, the callback function should last for
		 * very small durations, compared to the frequencies requested 
		 * (better multiple idle calls than too long too few), otherwise 
		 * the scheduling may constantly fail. 
		 *
		 * Event module notes :
		 *
		 * The event modules comes with no joystick handler registered, but 
		 * with a keyboard handler.
		 *
		 * Scheduler notes :
		 *
		 * @note Even though logic and rendering are not as tightly linked 
		 * as they often are, if the sum of their processing needs exceeds 
		 * what the hosting computer can give, they will have to overcome 
		 * this resource limitation. 
		 * The logic is deemed more important than rendering, therefore 
		 * jerked graphics will be preferred by the scheduler to
		 * frame-rate dependent logic.
		 *
		 * Basic settings are as follow :
		 *   - an engine tick lasts for 1 millisecond (engine frequency : 1 kHz)
		 *   - a simulation tick lasts for 10 engine ticks (logic frequency :
		 * 100 Hz)
		 *   - a rendering tick lasts for 25 engine ticks (rendering frequency,
		 * also known as framerate : 40 Hz, i.e. 40 frames per second), if 
		 * there is enough processing power
		 *   - an input tick lasts for 20 ticks (input frequency : 50 Hz)
		 *
		 * @note With the SDL back-end, a created window (see
		 * VideoModule::setMode) is needed to have the event system working.
		 *
		 *
		 */
		class EventsModule : public Ceylan::Module
		{
		
		
			// The common module has to create the event module.
			friend class CommonModule ;
		
			
			
			public:

							
				/**
				 * Waits for any key to be hit.
			 	 * 
			 	 * Will not work with the SDL back-end unless a window is 
				 * opened thanks to VideoModule::setMode : otherwise, no 
				 * event would be generated.
				 *
			 	 * @note This waiting method does not <b>poll</b> indefinitively
			 	 * for events, which would use 100% of the CPU all the time. 
				 * It just <b>waits</b> for an event, consuming almost no
				 * ressource.
			 	 *
				 */ 						
				virtual void waitForAnyKey( bool displayWaitingMessage = true ) 
					const throw() ; 


				/**
				 * Sleeps for the specified duration.
				 *
				 * @param seconds the number of seconds to wait.
				 *
				 * @param micros the remaining part of the time to wait,
				 * expressed as a number of microseconds. 
				 *
				 * As full seconds should be taken into account with the
				 * parameter <b>seconds</b>, <b>micros</b> should be less
				 * than one second, i.e. should be in the range 0 to 10E6 - 1.
				 *
				 * @throw EventsException if a system call failed.
		 		 *
				 * @return whether the deadline was successfully met, i.e. 
				 * if the waiting was on schedule. 
				 *
				 * @note This method must cope with the operating system
				 * scheduling, its precision should be of about a few
				 * microseconds.
				 *
				 * @see Ceylan::System::smartSleep
				 *
				 */ 						
				virtual bool sleepFor( Ceylan::System::Second seconds, 
						Ceylan::System::Microsecond micros ) 
					const throw( EventsException ) ; 

				
				/**
				 * Defines whether a scheduler should be used by the event 
				 * loop. 
				 * If a scheduler is wanted but does not already exist, 
				 * it will be created with default settings.
				 *
				 * This scheduler will be in charge of all the active 
				 * objects that will be registered subsequently.  
				 *
				 * @param on tells whether a scheduler is wanted.
				 *
				 * @see Scheduler::GetScheduler method to further 
				 * customize the scheduler.
				 *
				 */
				 virtual void useScheduler( bool on = true ) throw() ;
				 				  


				/**
				 * Tells whether a keyboard handler is available.
				 *
				 */
				virtual bool hasKeyboardHandler() const throw() ;
				 
				
				/**
				 * Returns the keyboard handler currently used.
				 *
				 * @throw EventsException if no keyboard handler is available.
				 *
				 */
				virtual KeyboardHandler & getKeyboardHandler() const 
					throw( EventsException ) ;
				   				   							

				/**
				 * Sets a new keyboard handler.
				 *
				 * @param newHandler the new joystcik handler.
				 *
				 * @note If a keyboard handler was already registered, 
				 * it is unregistered and deallocated first.
				 *
				 */
				virtual void setKeyboardHandler( 
					KeyboardHandler & newHandler ) throw() ;
					
									
				 
				 
				/**
				 * Tells whether a joystick handler is available.
				 *
				 */
				virtual bool hasJoystickHandler() const throw() ;
				 
				
				/**
				 * Returns the joystick handler currently used.
				 *
				 * @throw EventsException if no joystick handler is available.
				 *
				 */
				virtual JoystickHandler & getJoystickHandler() 
					const throw( EventsException ) ;
				   				   							

				/**
				 * Sets a new joystick handler.
				 *
				 * @param newHandler the new joystcik handler.
				 *
				 * @note If a joystick handler was already registered, 
				 * it is unregistered and deallocated first.
				 *
				 */
				virtual void setJoystickHandler( 
					JoystickHandler & newHandler ) throw() ;
					
					
				
				/**
				 * Sets the idle function, which is called whenever the 
				 * current scheduling system, either the main event loop 
				 * or the scheduler, detects a period of idle activity. 
				 *
				 * The callback can be useful when dealing with animations
				 * for example, or to execute background activities (such 
				 * as counting the frame rate), or to let the process yield 
				 * some CPU time so that the OS does not consider it as too
				 * CPU-hungry, and does not take counter-measures against it.
				 *
				 * @note Ensure that the maximum time spent in the callback
				 * is negligible compared to the period corresponding to 
				 * the maximum frequency being set, so that it does
				 * not result in a too heavy load which would make the 
				 * scheduler fail constantly. 
				 *
				 * @param idleCallback the idle callback, which can be null 
				 * (0) to specify no idle callback is wanted.
				 *
				 * @param callbackData the user-supplied data pointer that
				 * the idle callback will be given, if not null.
				 *
				 * Any prior callback or callback data will be replaced 
				 * by the one specified.
				 *
				 */	
				virtual void setIdleCallback( 
					Ceylan::System::Callback idleCallback, 
					void * callbackData = 0 ) throw() ;
							
				
				/**
				 * Sets the actual target frequency that the basic event 
				 * loop will try to run at.
				 * 
				 * @note Will not take effect until the next call to 
				 * the basic event loop.
				 *
				 */
				virtual void setEventLoopTargetFrequency( 
					Hertz targetFrequency ) throw() ;
				
				 										
				/**
				 * Enters the main event loop.
				 * 
				 * If a scheduler is being used, will start it, and 
				 * stop it on exit.
				 * Otherwise the basic event loop will be used.
				 *
				 * @note If an idle callback is set, it will be executed
				 * appropriately.
				 *
				 * @throw EventsException if a serious scheduling or
				 * event-related problem occured.
				 *
				 */
				virtual void enterMainLoop() throw( EventsException ) ;


				/**
				 * Requests the application to quit by exiting the main 
				 * loop.
				 *
				 * @note This method is useful for input handlers to send 
				 * too quit notifications, otherwise the only way of 
				 * quitting would be provided by the window manager, if
				 * any.
				 *
				 */
				virtual	void requestQuit() throw() ;
				
									
				/**
				 * Updates the state of the various inputs of interest
				 * (keyboard, mouse, joystick).
				 *
				 * Reads all pending low-level events and dispatch them to
				 * the appropriate manager.
				 *
				 * @note Useful for event loops, public for allowing the
				 * scheduler to call it.
				 *
				 */
				virtual void updateInputState() throw() ;
				
					
	            /**
	             * Returns an user-friendly description of the state of
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;


				
				// Static section.
				
					
				/**
				 * Returns a summary about the possible use of events-related
				 * environment variables, for the selected back-end.
				 *
				 * @note The SDL back-end can be partly driven by a set of
				 * environment variables.
				 *
				 */	
				static std::string DescribeEnvironmentVariables() throw() ;	



				/**
				 * Tells whether event system has already been initialized.
				 *
				 * @note This method is static so that calling it is 
				 * convenient : no need to explicitly retrieve the common
				 * module, then events module before knowing the result. 
				 *
				 * The need to retrieve the right module from scratch at 
				 * each call is rather inefficient though.
				 *
				 */
				static bool IsEventsInitialized() throw() ;
		
	
				/**
				 * Gets the number of milliseconds since the back-end
				 * initialization.
				 *
				 * @note This value wraps (overflows) if the program runs for
				 * more than about 49 days.
				 *
				 * @throw EventsException if the back-end is not already
				 * initialized.
				 *
				 */ 						
				static Ceylan::System::Millisecond GetMillisecondsSinceStartup()
					throw( EventsException ) ; 


				/// Describes a focus event.
				static std::string DescribeEvent( 
					const FocusEvent & focusEvent ) ;
				
				
				/// Describes a keyboard event.
				static std::string DescribeEvent( 
					const KeyboardEvent & keyboardEvent ) ;
								
				
				/// Describes a mouse motion event.
				static std::string DescribeEvent( 
					const MouseMotionEvent & mouseMotionEvent ) ;
				
				
				/// Describes a mouse button event.
				static std::string DescribeEvent( 
					const MouseButtonEvent & mouseButtonEvent ) ;
				
				
				/// Describes a joystick axis event.
				static std::string DescribeEvent( 
					const JoystickAxisEvent & axisEvent ) ;
				
				
				/// Describes a joystick trackball event.
				static std::string DescribeEvent( 
					const JoystickTrackballEvent & ballEvent ) ;
				
				
				/// Describes a joystick hat event.
				static std::string DescribeEvent( 
					const JoystickHatEvent & hatEvent ) ;
				
				
				/// Describes a joystick button event.
				static std::string DescribeEvent( 
					const JoystickButtonEvent & buttonEvent ) ;
				
				
				/// Describes a quit event.
				static std::string DescribeEvent( 
					const UserRequestedQuitEvent & quitEvent ) ;
				
				
				/// Describes a window manager event (system specific).
				static std::string DescribeEvent( 
					const SystemSpecificWindowManagerEvent & 
						windowManagerEvent ) ;
				
				
				/// Describes a window resized event.
				static std::string DescribeEvent( 
					const WindowResizedEvent & resizeEvent ) ;
				
				/// Describes a screen exposed event (needs redraw).
				static std::string DescribeEvent( 
					const ScreenExposedEvent & redrawEvent ) ;
				
				/// Describes an user event.
				static std::string DescribeEvent( 
					const UserEvent & userEvent ) ;
				
	
				/*
				 * Basic event types, used to discriminate in the generic
				 * event structure which is the actual type.
				 *
				 */
				 								
				
				/// Returns a string describing the specified basic event.
				static std::string DescribeEvent( BasicEvent anEvent ) throw() ;
				  
	
				/**
				 * Identifies the event corresponding to application 
				 * gaining or loosing focus(es).
				 *
				 */
				static const BasicEventType ApplicationFocusChanged ;
								
				/// Identifies the event corresponding to a key press.
				static const BasicEventType KeyPressed ;
				
				/// Identifies the event corresponding to a key release.
				static const BasicEventType KeyReleased ;
				
				/// Identifies the event corresponding to a mouse motion.
				static const BasicEventType MouseMoved ;
				
				/// Identifies the event corresponding to a mouse button press.
				static const BasicEventType MouseButtonPressed ;
				
				/**
				 * Identifies the event corresponding to a mouse button 
				 * release.
				 *
				 */
				static const BasicEventType MouseButtonReleased ;
				
				/// Identifies the event corresponding to a joystick axis move.
				static const BasicEventType JoystickAxisChanged ;
				
				/**
				 * Identifies the event corresponding to a joystick 
				 * trackball motion.
				 *
				 */
				static const BasicEventType JoystickTrackballChanged ;
				
				/**
				 * Identifies the event corresponding to a change of 
				 * joystick hat position.
				 *
				 */
				static const BasicEventType JoystickHatPositionChanged ;
				
				/**
				 * Identifies the event corresponding to a joystick button
				 * being pressed.
				 *
				 */
				static const BasicEventType JoystickButtonPressed ;
				
				/**
				 * Identifies the event corresponding to a joystick button
				 * being released.
				 *
				 */
				static const BasicEventType JoystickButtonReleased ;
				
				/**
				 * Identifies the event corresponding to the user requesting
				 * to quit.
				 *
				 */
				static const BasicEventType UserRequestedQuit ;
				
				/**
				 * Identifies the event corresponding to system specific 
				 * event.
				 *
				 */
				static const BasicEventType SystemSpecificTriggered ;
				
				/**
				 * Identifies the event corresponding to a resize of user's
				 * video mode.
				 *
				 */
				static const BasicEventType UserResizedVideoMode ;
				
				/**
				 * Identifies the event corresponding to a need to redraw
				 * screen.
				 *
				 */
				static const BasicEventType ScreenNeedsRedraw ;
				
				/// Identifies the first event that can be user-defined.
				static const BasicEventType FirstUserEventTriggered ;
				
				/// Identifies the last event that can be user-defined.
				static const BasicEventType LastUserEventTriggered ;
				
								
				// User event types :
				
				/**
				 * Void event, useful to return when no event should be
				 * generated.
				 *
				 */
				static const UserEventType NoEvent ;
				
				
				/// Identifies the user event corresponding to a quit request.
				static const UserEventType QuitRequested ;
				
											
				///	The default frequency targeted by the basic event loop.		
				static const Hertz DefaultEventLoopTargetedFrequency = 100 ;
				
				
				
				
		protected :
		
		

				/**
				 * Enters the basic event loop, not using any scheduler.
				 *
				 * The loop will aim to enforce the specified frequency, 
				 * i.e. the current frame per second target. 
				 *
				 * @note This is basically a never-return method, if no 
				 * exit handler is registered.
				 *
				 * @throw EventsException if a time-related problem, 
				 * more serious than deadline misses, occurs.
				 *
				 */
				virtual void enterBasicMainLoop() throw( EventsException ) ;
				 
				 
				/**
				 * Called whenever the application is deemed idle.
				 * Applies the idle behaviour, which can be user-defined
				 * (setIdleCallback), or otherwise which will default to
				 * micro-sleeps.
				 *
				 * @see setIdleCallback
				 *
				 */
				virtual void onIdle() throw() ;
				 
		
				/**
				 * Called whenever at least one of the three focuses 
				 * (mouse, keyboard or application visibility) changed.
				 *
				 * @note Call specialized methods such as onMouseFocusGained
				 * to handle each case.
				 *
				 */
				virtual void onApplicationFocusChanged( 
					const FocusEvent & focusEvent ) throw() ;
		
				
				
				/**
				 * Called whenever a key is pressed.
				 *
				 * @param keyboardEvent the keyboard event.
				 *
				 */
				virtual void onKeyPressed( 
					const KeyboardEvent & keyboardEvent ) throw() ;
		
		
				/**
				 * Called whenever a key is released.
				 *
				 * @param keyboardEvent the keyboard event.
				 *
				 */
				virtual void onKeyReleased( 
					const KeyboardEvent & keyboardEvent ) throw() ;
		
		
		
				/**
				 * Called whenever the mouse moved.
				 *
				 * @param mouseEvent the mouse motion event.
				 *
				 */
				virtual void onMouseMotion( 
					const MouseMotionEvent & mouseEvent ) throw() ; 
				
		
				/**
				 * Called whenever a mouse button is pressed.
				 *
				 * @param mouseEvent the mouse button event.
				 *
				 */
				virtual void onMouseButtonPressed( 
					const MouseButtonEvent & mouseEvent ) throw() ; 
				
				
				/**
				 * Called whenever a mouse button is released.
				 *
				 * @param mouseEvent the mouse button event.
				 *
				 */
				virtual void onMouseButtonReleased( 
					const MouseButtonEvent & mouseEvent ) throw() ; 
				
				
				
				/**
				 * Called whenever the joystick axis changed.
				 *
				 * @param joystickEvent the joystick axis event.
				 *
				 * @note All opened joysticks can be updated.
				 *
				 */
				virtual void onJoystickAxisChanged( 
					const JoystickAxisEvent & joystickEvent ) throw() ; 
				
				
				/**
				 * Called whenever the joystick trackball changed.
				 *
				 * @param joystickEvent the joystick trackball event.
				 *
				 * @note All opened joysticks can be updated.
				 *
				 */
				virtual void onJoystickTrackballChanged( 
					const JoystickTrackballEvent & joystickEvent ) throw() ; 
				
				
				/**
				 * Called whenever the joystick hat changed.
				 *
				 * @param joystickEvent the joystick hat event.
				 *
				 * @note All opened joysticks can be updated.
				 *
				 */
				virtual void onJoystickHatChanged( 
					const JoystickHatEvent & joystickEvent ) throw() ; 
				
				
				/**
				 * Called whenever the joystick button is pressed.
				 *
				 * @param joystickEvent the joystick button event.
				 *
				 * @note All opened joysticks can be updated.
				 *
				 */
				virtual void onJoystickButtonPressed( 
					const JoystickButtonEvent & joystickEvent )	throw() ; 
				
				
				/**
				 * Called whenever the joystick button is released.
				 *
				 * @param joystickEvent the joystick button event.
				 *
				 * @note All opened joysticks can be updated.
				 *
				 */
				virtual void onJoystickButtonReleased( 
						const JoystickButtonEvent & joystickEvent ) 
					throw() ; 
				
				
				
				/**
				 * Called whenever the application gained mouse focus.
				 *
				 * @note Made to be overriden, basically just writes the 
				 $ event in the standard output.
				 *
				 */
				virtual void onMouseFocusGained() throw() ; 	

				
				/**
				 * Called whenever the application lost mouse focus.
				 *
				 * @note Made to be overriden, basically just writes 
				 * the event in the standard output.
				 *
				 */
				virtual void onMouseFocusLost() throw() ; 	
				
			
			
				/**
				 * Called whenever the application gained keyboard focus.
				 *
				 * @note Made to be overriden, basically just writes the
				 * event in the standard output.
				 *
				 */
				virtual void onKeyboardFocusGained() throw() ; 	

				
				/**
				 * Called whenever the application lost keyboard focus.
				 *
				 * @note Made to be overriden, basically just writes 
				 * the event in the standard output.
				 *
				 */
				virtual void onKeyboardFocusLost() throw() ; 	
				
				
				
				/**
				 * Called whenever the application is iconified (minimized ).
				 *
				 * @note Made to be overriden, basically just writes the
				 * event in the standard output.
				 *
				 */
				virtual void onApplicationIconified() throw() ; 	

				
				/**
				 * Called whenever the application is restored.
				 *
				 * @note Made to be overriden, basically just writes
				 * the event in the standard output.
				 *
				 */
				virtual void onApplicationRestored() throw() ; 	


				/**
				 * Called whenever the application is requested to quit.
				 *
				 */
				virtual void onQuitRequested() throw() ;
				
								
				/**
				 * Called whenever a system-specific window manager event 
				 * is received.
				 * These unhandled events are received from the window 
				 * manager, and can be used, for example, to implement
				 * cut-and-paste in your application.
				 *
				 */
				virtual void onSystemSpecificWindowManagerEvent( 
					const SystemSpecificWindowManagerEvent & wmEvent ) throw() ;
					
				
				/**
				 * Called whenever the window is resized.
				 *
				 * Only applications that have been specified as resizable 
				 * in the setMode call can receive that event.
				 *
				 * @note The window should be resized to the new dimensions
				 * using setMode.
				 *
				 */	
				virtual void onResizedWindow( 
					const WindowResizedEvent & resizeEvent ) throw() ;
				
				
				/**
				 * Called whenever the screen has been modified outside 
				 * of the application, usually by the window manager, 
				 * and needs to be redrawn.
				 *
				 */
				virtual void onScreenNeedsRedraw() throw() ;
				
				
				/**
				 * Called whenever an user-defined event occurs.
				 *
				 * @note Data is transmitted alongside this event.
				 *
				 */
				virtual void onUserEvent( const UserEvent & userEvent ) 
					throw() ;
	

				/**
				 * Called whenever an unknown event type occurs during 
				 * the input update phase.
				 *
				 */
				virtual void onUnknownEventType( 
					const BasicEvent & unknownEvent ) throw() ;
				
				 
				 
				 
				// Section for protected attributes.
								
										
				/**
				 * Tells whether a scheduler should be used.
				 *
				 * @note By default, a basic event loop is used, instead 
				 * of a full-blown scheduler.
				 *
				 */				 
				bool _useScheduler ;				


				/**
				 * The internal keyboard handler.
				 *
				 */
				KeyboardHandler * _keyboardHandler ;

				 
				/**
				 * The internal joystick handler, if any.
				 *
				 */
				JoystickHandler * _joystickHandler ;	
								
		
		
				/**
				 * Array of all known environment variables related to events,
				 * for the SDL back-end.
				 *
				 */
				static std::string _SDLEnvironmentVariables[] ;


				/// Tells whether the user asked to quit.				
				bool _quitRequested ;
				
				
				/**
				 * The targeted frequency of the basic event loop.
				 *
				 * The loop will try to enforce it. 
				 * Should there be time left for an iteration, then the 
				 * event loop will call the idle callback multiple times, 
				 * until the next iteration is to occur.
				 *
				 * If no idle callback is registered, then for each 
				 * iteration with time left, the event loop will call the 
				 * basic sleep mecanism, as provided by the operating
				 * system. 
				 * This will be the smallest possible actual sleep duration,
				 * which is at least 1 millisecond with Linux 2.6 kernels.
				 *
				 */
				Hertz _loopTargetedFrequency ;				
				
				
				/**
				 * Count the number of idle calls made during the current
				 * event loop.
				 *
				 */
				Ceylan::Uint32 _idleCallsCount ;


				/**
				 * The idle callback, if any, to be called by the basic 
				 * event loop.
				 *
				 */
				Ceylan::System::Callback _loopIdleCallback ;
						
								
				/**
				 * The data, if any, to provide to the idle callback, 
				 * should the basic event loop be used.
				 *
				 */
				void * _loopIdleCallbackData ;				
				
				
				
				// Static section.
				
				
				/// Tells whether event system has already been initialized.
				static bool _EventsInitialized ;
			

				/// Number of available slots used to keep track of frame rate.
				static const unsigned int _FrameTimingSlotsCount ;
				
				
				/// Designates the mouse focus.
				static const short _MouseFocus ;
				
				/// Designates the keyboard focus.
				static const short _KeyboardFocus ;
				
				/// Designates the application focus (iconified or restored).
				static const short _ApplicationFocus ;
				
				/**
				 * Header of messages sent by default implementations 
				 * (ex : onMouseFocusGained).
				 *
				 */
				static const std::string _MessageHeader ;
				
				
				/**
				 * Records the number of buttons available on the current mouse.
				 *
				 * @fixme Put in MouseHandler
				 *
				 */
				static const unsigned int _MouseButtonCount ;
			
				
				
				
		private:
		
		
				/**
				 * Initializes the events module.
				 *
				 * @param eventsFlag flag describing which input device 
				 * should be enabled (keyboard, mouse, joystick, etc.)
				 *
				 * @throw EventsException if the initialization of an input
				 * subsystem failed.
				 *
				 */
				explicit EventsModule( Flags eventsFlag ) 
					throw( EventsException ) ;
			
			
				/// Deletes the events module.
				virtual ~EventsModule() throw() ;
					

				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit EventsModule( const EventsModule & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				EventsModule & operator = ( const EventsModule & source )
					throw() ;
				
		} ;
	
	}	
	
}



#endif // OSDL_EVENTS_H_

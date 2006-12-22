#ifndef OSDL_JOYSTICK_HANDLER_H_
#define OSDL_JOYSTICK_HANDLER_H_



#include "OSDLInputDeviceHandler.h"   // for inheritance
#include "OSDLJoystickCommon.h"       // for JoystickNumber

//#include "Ceylan.h"                   // for 


#include <string>
#include <list>


namespace OSDL
{


	namespace MVC
	{

		// The joystick handler can propagate events to the relevant controllers.
		class Controller ;
	
	}

		
	namespace Events
	{
	
	
		// Manages joysticks.
		class Joystick ;
		

		
		/**
		 * Handler for all detected joysticks.
		 *
		 * Joystick indexes range from 0 (included) to GetAvailableJoystickCount() (exluded).
		 *
		 * @note The UseJoystick flag must be specified  in the call to 
		 * CommonModule::getCommonModule so that joystick support is enabled.
		 *
		 */
		class JoystickHandler : public InputDeviceHandler
		{
		
			
			public:



				/**
				 * Constructs a new joystick handler.
				 *
				 * @param useClassicalJoysticks tells whether detected joysticks should be 
				 * managed as complex joysticks (powerful but require the controller to take in
				 * charge this complexity) or classical joysticks (for usual needs, comes with
				 * preset behaviour which ease controller work).
				 *
				 * @throw InputDeviceHandlerException if the initialization of the joystick
				 * subsystem failed.
				 *
				 */
				explicit JoystickHandler( bool useClassicalJoysticks = true )
					throw( InputDeviceHandlerException ) ;
				
				
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~JoystickHandler() throw() ;


				/**
				 * Auto-detects and registers any attached joystick.
				 *
				 * @note Recreates from scratch all joystick informations.
				 *
				 */
				 virtual void update() throw() ;


				/**
				 * Opens joystick of specified index. If joystick is already open, does nothing.
				 *
				 * @param index the index of the joystick to open.
				 *
				 * @throw JoystickException if index is out of bounds.
				 *
				 * @see GetAvailableJoystickCount
				 *
				 */
				virtual void openJoystick( JoystickNumber index ) throw( JoystickException ) ;
	
	
				/**
				 * Links specified joystick to specified controller, so that any further 
				 * joystick-related event will be sent to the controller. 
				 *
				 * Removes automatically any link previously defined between this joystick and
				 * any other controller.
				 *
				 * @param index the index of the joystick that shall be linked to the controller.
				 *
				 * @param controller the OSDL controller which will be notified of joystick changes.
				 *
				 * @throw JoystickException if index is out of bounds.
				 *
				 */
				virtual void linkToController( JoystickNumber index, 
					OSDL::MVC::Controller & controller ) throw( JoystickException ) ;
				 
				 
				/**
				 * Called whenever a joystick axis changed, so that its controller, if any, is
				 * notified.
				 *
				 *
				 */
				virtual void axisChanged( const JoystickAxisEvent & joystickEvent )	const throw() ;
							
							
				/**
				 * Called whenever a joystick trackball changed, so that its controller, if any, 
				 * is notified.
				 *
				 *
				 */
				virtual void trackballChanged( const JoystickTrackballEvent & joystickEvent ) 
					const throw() ;
					
							
				/**
				 * Called whenever a joystick hat changed, so that its controller, if any, is
				 * notified.
				 *
				 *
				 */
				virtual void hatChanged( const JoystickHatEvent & joystickEvent ) const throw() ;
				
				
							
				/**
				 * Called whenever a joystick button was pressed, so that its controller, if any, 
				 * is notified.
				 *
				 *
				 */
				virtual void buttonPressed( const JoystickButtonEvent & joystickEvent ) 
					const throw() ;
						
							
				/**
				 * Called whenever a joystick button was released, so that its controller, if any,
				 * is notified.
				 *
				 *
				 */
				virtual void buttonReleased( const JoystickButtonEvent & joystickEvent ) 
					const throw() ;
						
			
	            /**
	             * Returns an user-friendly description of the state of this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;
				
				
				
				// Static section.
				
				 
				/**
				 * Returns the number of available joysticks, i.e. the joysticks currently 
				 * attached to the system.
				 *
				 *
				 */
				static JoystickNumber GetAvailableJoystickCount() throw() ;
				
				
				
				
		protected :
						 	
				
				/**
				 * Blanks this joystick handler, i.e. forget all previously known joysticks.
				 *
				 */
				virtual void blank() throw() ;
				 			
				
				/**
				 * Checks that the joystick at the specified index exists and is opened.
				 *
				 * @param index the index of the joystick to check.
				 *
				 * @note Stop in emergency the application if the check failed.
				 *
				 */						
				virtual void checkJoystickAt( JoystickNumber index ) const throw() ;
				
								
				/**
				 * Records how many joysticks are known by this handler.
				 *
				 */
				JoystickNumber _joystickCount ;
				
				 
				/**
				 * Records all known joysticks.
				 * A dynamic array is preferred to a list for performance reasons.
				 * (read it as Joystick * _joysticks[])
				 *
				 */
				Joystick ** _joysticks ;
				
				
				/**
				 * Tells whether complex joysticks (if false) or classical joysticks (if true) 
				 * should be created.
				 *
				 *
				 */
				bool _useClassicalJoysticks ;



			private:
		
		
				/**
				 * Copy constructor made private to ensure that it will be never called.
				 * The compiler should complain whenever this undefined constructor is called, 
				 * implicitly or not.
				 * 
				 *
				 */			 
				explicit JoystickHandler( const JoystickHandler & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it will be never called.
				 * The compiler should complain whenever this undefined operator is called, 
				 * implicitly or not.
				 * 
				 *
				 */			 
				JoystickHandler & operator = ( const JoystickHandler & source ) throw() ;
										
				
		} ;
	
	}	
	
}



#endif // OSDL_JOYSTICK_HANDLER_H_

#ifndef OSDL_CLASSICAL_JOYSTICK_H_
#define OSDL_CLASSICAL_JOYSTICK_H_


#include "OSDLJoystickCommon.h"    // for JoystickNumber, AxisPosition, etc.
#include "OSDLJoystick.h"          // for inheritance, etc.

#include <string>
#include <list>



namespace OSDL
{


	namespace Events
	{
	
			
		
		/**
		 * Models a classical joystick, with simplified access to the two 
		 * main axes, commonly used to indicate the joystick current 
		 * direction, with customizable deadzone support and two buttons
		 * directly managed.
		 *
		 * This simplifies the task of the any controller being linked 
		 * to this joystick.
		 *
		 * @note Only the two first axes have a deadzone managed, since they
		 * usually correspond to the actual stick. 
		 * Further axes are often dedicated to throttle, which do not need
		 * any deadzone at all for most applications.
		 *
		 * @note Controllers should be linked to at most one Classical 
		 * joystick, since the joystick index is not propagated.
		 *
		 */
		class ClassicalJoystick : public Joystick
		{
		
			
			public:


				/**
				 * Constructs a new joystick handler.
				 *
				 * @param index the index of this joystick in platform list.
				 *
				 */
				explicit ClassicalJoystick( JoystickNumber index, 
						AxisPosition deadZoneExtent = DefaultDeadZoneExtent )
					throw() ;
				
				
				/**
				 * Virtual destructor, closing corresponding joystick device.
				 *
				 */
				virtual ~ClassicalJoystick() throw() ;
				
				
				/**
				 * Called whenever an axis of this joystick changed, and 
				 * notify the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 */
				virtual void axisChanged( 
					const JoystickAxisEvent & joystickEvent ) throw() ;
		
		
				/**
				 * Called whenever a button of this joystick was pressed, 
				 * and notify the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 */
				virtual void buttonPressed( 
					const JoystickButtonEvent & joystickEvent ) throw() ;
	
	
				/**
				 * Called whenever a button of this joystick was released, 
				 * and notify the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 */
				virtual void buttonReleased( 
					const JoystickButtonEvent & joystickEvent ) throw() ;
				


				/**
				 * Returns the deadzone extent for the two first axes.
				 *
				 * @param firstAxisExtent will be set to the extent for the
				 * first axis.
				 *
				 * @param secondAxisExtent will be set to the extent for the
				 * second axis.
				 *
				 */
				virtual void getDeadZoneValues( AxisPosition & firstAxisExtent, 
					AxisPosition & secondAxisExtent ) const throw() ;
					
				
				/**
				 * Sets the deadzone values for the two first axes.
				 *
				 * @param firstAxisExtent the extent of the deadzone for the
				 * first axis.
				 *
				 * @param secondAxisExtent the extent of the deadzone for the
				 * second axis.
				 *
				 */
				virtual void setDeadZoneValues( 
						AxisPosition firstAxisExtent  = DefaultDeadZoneExtent,
						AxisPosition secondAxisExtent = DefaultDeadZoneExtent )
					throw() ;
				 	
					
								
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
				
				
				static const AxisPosition DefaultDeadZoneExtent ;
				
			
			
		protected :
		

				/**
				 * Stores the dead zone extent for first axis. 
				 * If d is the dead zone value, then a reported joystick 
				 * axis value in :
				 *  - ]32768; -d[ will notify the controller to the joystick 
				 * is on the left
				 *  - [-d;d] will not notify the controller of anything
				 *  - ]d; 32767[ will notify the controller to the joystick 
				 * is on the right
				 *
				 */
				AxisPosition _deadZoneExtentFirstAxis ;


				/**
				 * Stores the dead zone extent for first axis. 
				 * If d is the dead zone value, then a reported joystick 
				 * axis value in :
				 *  - ]32768; -d[ will notify the controller to the joystick 
				 * is up
				 *  - [-d;d] will not notify the controller of anything
				 *  - ]d; 32767[ will notify the controller to the joystick 
				 * is down
				 *
				 */
				AxisPosition _deadZoneExtentSecondAxis ;



		private:
		
		
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit ClassicalJoystick( const Joystick & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				ClassicalJoystick & operator = ( const Joystick & source )
					throw() ;
										
				
		} ;
	
	}	
	
}



#endif // OSDL_CLASSICAL_JOYSTICK_H_

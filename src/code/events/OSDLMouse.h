#ifndef OSDL_MOUSE_H_
#define OSDL_MOUSE_H_


#include "OSDLMouseCommon.h"       // for MouseNumber, etc.
#include "OSDLInputDevice.h"       // for inheritance
#include "OSDLVideoTypes.h"        // for Coordinate


#include <string>
#include <list>




namespace OSDL
{


	namespace Events
	{
	
					
		
		/**
		 * Models a basic mouse, including relevant buttons and wheels.
		 *
		 * All events for this mouse are propagated to the associated
		 * controller, if any, which allows to finely master all available
		 * informations, at the expense of a higher complexity on the 
		 * controller part.
		 *
		 * For usual needs, ClassicalMouse child class should be more
		 * convenient, since it is a higher-level (but a little less
		 * general-purpose) model.
		 *
		 * @see ClassicalMouse
		 *
		 */
		class OSDL_DLL Mouse : public OSDL::Events::InputDevice
		{
		
		
			/*
			 * The mouse handler has to trigger the mouse event callbacks
			 * for this mouse.
			 *
			 */
			friend class OSDL::Events::MouseHandler ;
			
			
			
			public:


				/**
				 * Constructs a new mouse manager.
				 *
				 * @param index the index of this mouse in platform list.
				 *
				 */
				explicit Mouse( MouseNumber index ) throw() ;
				
				
				/**
				 * Virtual destructor, closing corresponding mouse device.
				 *
				 */
				virtual ~Mouse() throw() ;
				
				


				/**
				 * Returns the absolute abscissa of this mouse cursor.
				 *
				 */ 
				virtual Video::Coordinate getCursorAbscissa() const throw() ;
				 
				 
				/**
				 * Returns the absolute ordinate of this mouse cursor.
				 *
				 */ 
				virtual Video::Coordinate getCursorOrdinate() const throw() ;
				 
				 
				 
				
				// Buttons & wheels section.
				
				
				/**
				 * Returns the number of mouse buttons, wheels not included.
				 *
				 * @note Information not available yet, returns always three.
				 *
				 */
				virtual MouseButtonNumber getNumberOfButtons() 
					const throw() ;
				 
				 
				/**
				 * Returns the number of mouse wheels.
				 *
				 * @note Information not available yet, returns always one.
				 *
				 */
				virtual MouseWheelNumber getNumberOfWheels() 
					const throw() ;
				
				
				
				/**
				 * Tells whether the left button of this mouse is currently
				 * pressed. 
				 *
				 * @return true iff this button is pressed.
				 *
				 */ 
				virtual bool isLeftButtonPressed() const throw() ;
				 

				/**
				 * Tells whether the middle button of this mouse is currently
				 * pressed. 
				 *
				 * @return true iff this button is pressed.
				 *
				 */ 
				virtual bool isMiddleButtonPressed() const throw() ;
				 

				/**
				 * Tells whether the right button of this mouse is currently
				 * pressed. 
				 *
				 * @return true iff this button is pressed.
				 *
				 */ 
				virtual bool isRightButtonPressed() const throw() ;
				
				 
				/**
				 * Tells whether specified mouse button is pressed.
				 *
				 * @param buttonNumber the number of the button for this
				 * mouse, for example 'LeftButton'.
				 *
				 * @return true iff the button is currently pressed.
				 *
				 * @throw MouseException if the mouse is not 
				 * already open or if the button number is out of bounds.
				 *
				 */ 
				virtual bool isButtonPressed( MouseButtonsCount buttonNumber ) 
					const throw( MouseException ) ;				 
				 
				 
				/**
				 * Returns the state of all the buttons of this mouse. 
				 * This includes the wheels.
				 *
				 * @return a button mask, each bit of which tells whether 
				 * the associated button is pressed.
				 *
				 * @see IsPressed to interpret the button mask.
				 *
				 */ 
				virtual MouseButtonMask getButtonStates() const throw() ;   
				 				 			
				
									
				
				 				 
							
								 					
				/**
				 * Updates the description of this mouse, for relative 
				 * movements of the cursor.
				 *
				 */
				virtual void update() throw() ;
				
								
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
				
				
				/**
				 * Reads specified button mask, and tells whether specified
				 * mouse button is currently pressed.
				 *
				 * @note Wheel directions are managed like buttons, if
				 * IsPressed( aMask, WheelUp ) is true, then the wheel is up.
				 *
				 * @return true iff the button is pressed, according to 
				 * the mask.
				 *
				 * @see getButtonStates.
				 *
				 */
				static bool IsPressed( MouseButtonMask mask, 
					MouseButtonNumber buttonToInspect ) throw() ;
					
					
					
		protected :
		
		
				/**
				 * Returns the index of this mouse in the platform list.
				 *
				 * @note Should be useless.
				 *
				 */
				virtual MouseNumber getIndex() const throw() ;
		
		
		
				/*
				 * Event-driven callbacks for input propagation, from
				 * the joystick handler to this mouse.
				 *
				 */

				
				// Focus section.


				/**
				 * Called whenever this mouse gained focus, and 
				 * notifies the linked controller, if any.
				 *
				 */
				virtual void focusGained() throw() ;
	
	
				/**
				 * Called whenever this mouse lost focus, and 
				 * notifies the linked controller, if any.
				 *
				 */
				virtual void focusLost() throw() ;
	
	
	
	
				// Cursor position section.
	
	
				/**
				 * Called whenever this mouse moved, and 
				 * notifies the linked controller, if any.
				 *
				 * @param mouseEvent the corresponding mouse event.
				 *
				 */
				virtual void mouseMoved( 
					const MouseMotionEvent & mouseEvent ) throw() ;
	
							
				/**
				 * Called whenever this mouse moved, and 
				 * notifies the linked controller, if any.
				 *
				 * @param mouseEvent the corresponding mouse event.
				 *
				 */
				virtual void mouseMoved( 
					const MouseMotionEvent & mouseEvent ) throw() ;
	

				/**
				 * Called whenever a button of this mouse was pressed, 
				 * and notifies the linked controller, if any.
				 *
				 * @param mouseEvent the corresponding mouse event.
				 *
				 */
				virtual void buttonPressed( 
					const MouseButtonEvent & mouseEvent ) throw() ;
	
	
				/**
				 * Called whenever a button of this mouse was released, 
				 * and notifies the linked controller, if any.
				 *
				 * @param mouseEvent the corresponding mouse event.
				 *
				 */
				virtual void buttonReleased( 
					const MouseButtonEvent & mouseEvent ) throw() ;
				
				/**
				 * Returns the index of this mouse in the platform list.
				 *
				 * @note Should be useless.
				 *
				 */
				virtual MouseNumber getIndex() const throw() ;
	
				
				/**
				 * The implementation dependent name of this mouse, if any.
				 *
				 */
				std::string _name ;
				
				
				/// The number of this mouse in platform list.				
				MouseNumber _index ;
				
								
				/// The internal raw mouse being used.
				RawMouse * _internalMouse ;
	
				
				/// The number of axes for this mouse.
				MouseAxesCount _axisCount ;


				/// The number of trackballs for this mouse.
				MouseTrackballsCount _trackballCount ;


				/// The number of hats for this mouse.
				MouseHatsCount _hatCount ;


				/// The number of buttons for this mouse.
				MouseButtonsCount _buttonCount ;



		private:
		
		
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Mouse( const Mouse & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Mouse & operator = ( const Mouse & source ) throw() ;
										
				
		} ;
	
	}	
	
}



#endif // OSDL_MOUSE_H_

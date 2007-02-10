#ifndef OSDL_JOYSTICK_H_
#define OSDL_JOYSTICK_H_


#include "OSDLJoystickCommon.h"    // for JoystickNumber, etc.
#include "OSDLInputDevice.h"       // for inheritance

#include "SDL.h"                   // for SDL_Joystick

#include <string>
#include <list>




namespace OSDL
{


	namespace Events
	{
	
	
		/// Low level joystick object.
		typedef SDL_Joystick RawJoystick  ;
				
		
		/**
		 * Models a basic joystick, including relevant axes, hats, balls and
		 * buttons.
		 *
		 * Usually, joystick directions should be interpreted this way :
		 *   - axis 0 : left-right direction, negative values are left, 
		 * positive are right
		 *   - axis 1 : up-down direction, negative values are up, positive 
		 * are down
		 *
		 * All events are propagated to the associated controller, if any, 
		 * which allows to finely master all available informations, at the
		 * expense of a higher complexity on the controller part.
		 *
		 * For usual needs, ClassicalJoystick child class should be more
		 * convenient, since it is a higher-level (but a little less
		 * general-purpose) model.
		 *
		 * @see ClassicalJoystick
		 *
		 */
		class Joystick : public OSDL::Events::InputDevice
		{
		
			
			public:


				/**
				 * Constructs a new joystick manager.
				 *
				 * @param index the index of this joystick in platform list.
				 *
				 */
				explicit Joystick( JoystickNumber index ) throw() ;
				
				
				/**
				 * Virtual destructor, closing corresponding joystick device.
				 *
				 */
				virtual ~Joystick() throw() ;
				
				
				/**
				 * Returns the implementation dependent name of this joystick.
				 *
				 */
				virtual const std::string & getName() const throw() ;
							 
				 
				/**
				 * Tells whether this joystick is currently opened.
				 *
				 * @note A joystick must be opened before being used.
				 *
				 */
				virtual bool isOpen() const throw() ;
				
				
				/**
				 * Opens the joystick, so that it can be used.
				 *
				 * @throw JoystickException if the joystick was already open.
				 *
				 */
				virtual void open() throw( JoystickException ) ; 
				 
				 
				/**
				 * Closes the joystick.
				 *
				 * @throw JoystickException if the joystick was not open.
				 *
				 */ 			
				virtual void close() throw( JoystickException ) ;


				/**
				 * Called whenever an axis of this joystick changed, and 
				 * notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 */
				virtual void axisChanged( 
					const JoystickAxisEvent & joystickEvent ) throw() ;
	
							
				/**
				 * Called whenever a trackball of this joystick changed, and 
				 * notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 */
				virtual void trackballChanged( 
					const JoystickTrackballEvent & joystickEvent ) throw() ;
	
							
				/**
				 * Called whenever a hat of this joystick changed, 
				 * and notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 */
				virtual void hatChanged( 
					const JoystickHatEvent & joystickEvent ) throw() ;
	
	
				/**
				 * Called whenever a button of this joystick was pressed, 
				 * and notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 */
				virtual void buttonPressed( 
					const JoystickButtonEvent & joystickEvent ) throw() ;
	
	
				/**
				 * Called whenever a button of this joystick was released, 
				 * and notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 */
				virtual void buttonReleased( 
					const JoystickButtonEvent & joystickEvent ) throw() ;
	
											
				/**
				 * Returns the number of joystick axes.
				 *
				 * @note The joystick must already have been opened.
				 *
				 */
				virtual JoystickAxesCount getNumberOfAxes() const throw() ;
				
				
				/**
				 * Returns the number of joystick trackballs.
				 *
				 * @note The joystick must already have been opened.
				 *
				 */
				virtual JoystickTrackballsCount getNumberOfTrackballs() 
					const throw() ;
				
				
				/**
				 * Returns the number of joystick hats.
				 *
				 * @note The joystick must already have been opened.
				 *
				 */
				virtual JoystickHatsCount getNumberOfHats() const throw() ;
				
				
				/**
				 * Returns the number of joystick buttons.
				 *
				 * @note The joystick must already have been opened.
				 *
				 */
				virtual JoystickButtonsCount getNumberOfButtons() 
					const throw() ;
				 
				
				/**
				 * Returns the absolute position of the X axis.
				 *
				 * @note On most modern joysticks the X axis is represented 
				 * by axis 0, which is the axis that is used by this method.
				 *
				 * @note Joystick must be already open.
				 *
				 */ 
				virtual AxisPosition getAbscissaPosition() const throw() ;
				 
				 
				/**
				 * Returns the absolute position of the Y axis.
				 *
				 * @note On most modern joysticks the Y axis is represented 
				 * by axis 1, which is the axis that is used by this method.
				 *
				 * @note Joystick must be already open.
				 *
				 */ 
				virtual AxisPosition getOrdinatePosition() const throw() ;
				 
				 
				/**
				 * Returns the absolute position of the specified axis.
				 *
				 * @param index the index of the axis for this joystick.
				 *
				 * @return the axis position.
				 *
				 * @throw JoystickException if the joystick is not already 
				 * opened or if index is out of bounds.
				 *
				 */ 
				virtual AxisPosition getPositionOfAxis( 
						JoystickAxesCount index ) 
					const throw( JoystickException ) ;				 
				 	
							
				/**
				 * Returns the absolute position of the specified hat.
				 *
				 * @param index the index of the hat for this joystick.
				 *
				 * @return the hat position.
				 *
				 * @throw JoystickException if the joystick is not already 
				 * open or if index is out of bounds.
				 *
				 */ 
				virtual HatPosition getPositionOfHat( 
						JoystickHatsCount index ) 
					const throw( JoystickException ) ;				 
				
				 			
				/**
				 * Tells whether specified joystick button is pressed.
				 *
				 * @param buttonNumber the number of the button for this
				 * joystick.
				 *
				 * @return true iff the button is currently pressed.
				 *
				 * @throw JoystickException if the joystick is not 
				 * already open or if the button number is out of bounds.
				 *
				 */ 
				virtual bool isButtonPressed( 
						JoystickButtonsCount buttonNumber ) 
					const throw( JoystickException ) ;				 
			
			
				/**
				 * Returns the relative position of the specified trackball.
				 *
				 * @param ball the number of the ball for this joystick.
				 *
				 * @param deltaX the abscissa motion delta, since last call to 
				 * this method, will be stored here.
				 *
				 * @param deltaY the ordinate motion delta, since last call to
				 * this method, will be stored here.				
				 * 
				 * @return true iff the operation succeeded.
				 *
				 * @throw JoystickException if the joystick is not already 
				 * open or if index is out of bounds.
				 *
				 * @note Trackballs can only return relative motion since 
				 * the last call of this method.
				 *
				 */ 
				virtual bool getPositionOfTrackball( 
						JoystickTrackballsCount ball,
						BallMotion & deltaX, BallMotion & deltaY ) 
					const throw( JoystickException ) ;	
							
								 					
				/**
				 * Updates the description of this joystick (trackballs, hats,
				 * buttons, etc.)
				 *
				 * @note The joystick must be open.
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
				
				
			
		protected :
		
				
				/**
				 * Returns the index of this joystick in the platform list.
				 *
				 * @note Should be useless.
				 *
				 */
				virtual JoystickNumber getIndex() const throw() ;
	
				
				/**
				 * The implementation dependent name of this joystick, if any.
				 *
				 */
				std::string _name ;
				
				
				/// The number of this joystick in platform list.				
				JoystickNumber _index ;
				
								
				/// The internal raw joystick being used.
				RawJoystick * _internalJoystick ;
	
				
				/// The number of axes for this joystick.
				JoystickAxesCount _axisCount ;


				/// The number of trackballs for this joystick.
				JoystickTrackballsCount _trackballCount ;


				/// The number of hats for this joystick.
				JoystickHatsCount _hatCount ;


				/// The number of buttons for this joystick.
				JoystickButtonsCount _buttonCount ;



		private:
		
		
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Joystick( const Joystick & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Joystick & operator = ( const Joystick & source ) throw() ;
										
				
		} ;
	
	}	
	
}



#endif // OSDL_JOYSTICK_H_

/* 
 * Copyright (C) 2003-2013 Olivier Boudeville
 *
 * This file is part of the OSDL library.
 *
 * The OSDL library is free software: you can redistribute it and/or modify
 * it under the terms of either the GNU Lesser General Public License or
 * the GNU General Public License, as they are published by the Free Software
 * Foundation, either version 3 of these Licenses, or (at your option) 
 * any later version.
 *
 * The OSDL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License and the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License and of the GNU General Public License along with the OSDL library.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Olivier Boudeville (olivier.boudeville@esperide.com)
 *
 */


#ifndef OSDL_JOYSTICK_H_
#define OSDL_JOYSTICK_H_


#include "OSDLJoystickCommon.h"    // for JoystickNumber, etc.
#include "OSDLInputDevice.h"       // for inheritance


#include <string>
#include <list>



// 'struct SDL_Joystick ;' already forward-declared.




namespace OSDL
{



	namespace Events
	{

		
		
		#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 
	
		/*
		 * No way of forward declaring LowLevelJoystick apparently:
		 * we would have liked to specify 'struct LowLevelThing ;' here and
		 * in the implementation file (.cc): 'typedef BackendThing
		 * LowLevelThing' but then the compiler finds they are conflicting
		 * declarations.
		 *
		 */
		typedef ::SDL_Joystick LowLevelJoystick ;

		#else // OSDL_USES_SDL

		struct LowLevelJoystick {} ;

		#endif // OSDL_USES_SDL
		
		
		
		/**
		 * Models a basic joystick, including relevant axes, hats, balls and
		 * buttons.
		 *
		 * Usually, joystick directions should be interpreted this way:
		 *   - axis 0: left-right direction, negative values are left, 
		 * positive are right
		 *   - axis 1: up-down direction, negative values are up, positive 
		 * are down
		 *
		 * All events for this joystick are propagated to the associated
		 * controller, if any, which allows to finely master all available
		 * informations, at the expense of a higher complexity on the 
		 * controller part.
		 *
		 * For usual needs, ClassicalJoystick child class should be more
		 * convenient, since it is a higher-level (but a little less
		 * general-purpose) model.
		 *
		 * @see ClassicalJoystick
		 *
		 */
		class OSDL_DLL Joystick: public OSDL::Events::InputDevice
		{
		
		

			/*
			 * The joystick handler has to trigger the joystick event callbacks
			 * for this joystick.
			 *
			 */
			friend class OSDL::Events::JoystickHandler ;

			
			
			public:



				/**
				 * Constructs a new joystick manager.
				 *
				 * @param index the index of this joystick in platform list.
				 *
				 * @throw JoystickException if the operation failed or is
				 * not supported.
				 *
				 */
				explicit Joystick( JoystickNumber index ) ;
				
				
				
				/**
				 * Virtual destructor, closing corresponding joystick device.
				 *
				 */
				virtual ~Joystick() throw() ;
				
				
				
				/**
				 * Returns the implementation dependent name of this joystick.
				 *
				 */
				virtual const std::string & getName() const ;
							 
							 
				 
				/**
				 * Tells whether this joystick is currently opened.
				 *
				 * @note A joystick must be opened before being used.
				 *
				 */
				virtual bool isOpen() const ;
				
				
				
				/**
				 * Opens the joystick, so that it can be used.
				 *
				 * @throw JoystickException if the joystick was already open.
				 *
				 */
				virtual void open() ; 
				 
				 
				 
				/**
				 * Closes the joystick.
				 *
				 * @throw JoystickException if the joystick was not open.
				 *
				 */ 			
				virtual void close() ;


											
				/**
				 * Returns the number of joystick axes.
				 *
				 * @note The joystick must already have been opened.
				 *
				 */
				virtual JoystickAxesCount getNumberOfAxes() const ;
				
				
				
				/**
				 * Returns the number of joystick trackballs.
				 *
				 * @note The joystick must already have been opened.
				 *
				 */
				virtual JoystickTrackballsCount getNumberOfTrackballs() const ;
				
				
				
				/**
				 * Returns the number of joystick hats.
				 *
				 * @note The joystick must already have been opened.
				 *
				 */
				virtual JoystickHatsCount getNumberOfHats() const ;
				
				
				
				/**
				 * Returns the number of joystick buttons.
				 *
				 * @note The joystick must already have been opened.
				 *
				 */
				virtual JoystickButtonsCount getNumberOfButtons() const ;
				 
				
				
				/**
				 * Returns the absolute position of the X axis.
				 *
				 * @note On most modern joysticks the X axis is represented 
				 * by axis 0, which is the axis that is used by this method.
				 *
				 * @note Joystick must be already open.
				 *
				 */ 
				virtual AxisPosition getAbscissaPosition() const ;
				 
				 
				 
				/**
				 * Returns the absolute position of the Y axis.
				 *
				 * @note On most modern joysticks the Y axis is represented 
				 * by axis 1, which is the axis that is used by this method.
				 *
				 * @note Joystick must be already open.
				 *
				 */ 
				virtual AxisPosition getOrdinatePosition() const ;
				 
				 
				 
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
					JoystickAxesCount index ) const ;				 
				 	
					
							
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
					JoystickHatsCount index ) const ;				 
				
				
				 			
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
					JoystickButtonsCount buttonNumber )	const  ;
			
			
			
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
					BallMotion & deltaX, BallMotion & deltaY ) const ;	
					
							
								 					
				/**
				 * Updates the description of this joystick (trackballs, hats,
				 * buttons, etc.)
				 *
				 * @note The joystick must be open.
				 *
				 */
				virtual void update() ;
				
				
								
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
					Ceylan::VerbosityLevels level = Ceylan::high ) const  ;
				
				
				
				
			
		protected:
		
				
				
				/**
				 * Returns the index of this joystick in the platform list.
				 *
				 * @note Should be useless.
				 *
				 */
				virtual JoystickNumber getIndex() const ;
	
	
	
				/*
				 * Event-driven callbacks for input propagation, from
				 * the joystick handler to this joystick.
				 *
				 */
				
	
	
				/**
				 * Called whenever an axis of this joystick changed, and 
				 * notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 * @note Expected to be triggered by the Joystick handler.
				 *
				 */
				virtual void axisChanged( 
					const JoystickAxisEvent & joystickEvent ) ;
	
	
							
				/**
				 * Called whenever a trackball of this joystick changed, and 
				 * notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 * @note Expected to be triggered by the Joystick handler.
				 *
				 */
				virtual void trackballChanged( 
					const JoystickTrackballEvent & joystickEvent ) ;
	
	
							
				/**
				 * Called whenever a hat of this joystick changed, 
				 * and notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 * @note Expected to be triggered by the Joystick handler.
				 *
				 */
				virtual void hatChanged( 
					const JoystickHatEvent & joystickEvent ) ;
	
	
	
				/**
				 * Called whenever a button of this joystick was pressed, 
				 * and notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 * @note Expected to be triggered by the Joystick handler.
				 *
				 */
				virtual void buttonPressed( 
					const JoystickButtonEvent & joystickEvent ) ;
	
	
	
				/**
				 * Called whenever a button of this joystick was released, 
				 * and notifies the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 * @note Expected to be triggered by the Joystick handler.
				 *
				 */
				virtual void buttonReleased( 
					const JoystickButtonEvent & joystickEvent ) ;
	

				
				/**
				 * The implementation dependent name of this joystick, if any.
				 *
				 */
				std::string _name ;
				
				
				
				/// The number of this joystick in platform list.				
				JoystickNumber _index ;
				
						
								
				/// The internal raw joystick being used.
				LowLevelJoystick * _internalJoystick ;
	
	
				
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
				explicit Joystick( const Joystick & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Joystick & operator = ( const Joystick & source ) ;
										
				
		} ;
	
	
	}	
	
}



#endif // OSDL_JOYSTICK_H_


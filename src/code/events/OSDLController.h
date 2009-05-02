/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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


#ifndef OSDL_CONTROLLER_H_
#define OSDL_CONTROLLER_H_


#include "OSDLEventsCommon.h"       // for typedefs
#include "OSDLJoystickCommon.h"     // for AxisPosition


#include "Ceylan.h"                 // for inheritance

#include <string>




namespace OSDL
{



	namespace MVC
	{
			
		
		
		/**
		 * Specialization of generic Ceylan controller, linked with actual 
		 * input devices such as mouse, keyboard or joystick.
		 *
		 * As these controllers have to be able to listen to various input
		 * devices, all specific method handlers (such as axisChanged) have 
		 * to be defined. 
		 * Their default implementation is basically a do-nothing method, 
		 * only adding logs if in debug mode. 
		 * This way, the controllers being instanciated in the application,
		 * which are child classes of these OSDL::Events::Controller, just 
		 * have to override the handlers corresponding to the specific 
		 * events they want to listen to: not defining an handler does not
		 * prevent from instanciating the controller.
		 *
		 * These controllers must have a total control of the input devices 
		 * they are linked to.
		 * This hinders abstracting events to simplify their management: 
		 * for example, one cannot say that an increasing joystick axis 
		 * value means in all cases the same as, say, the mouse being pushed
		 * forward or the right arrow key being pressed.
		 * 
		 * Therefore the controllers are tightly linked to the input 
		 * devices, and they will be in charge of converting these
		 * low level input events into higher level object-specific events.
		 *
		 * One of the consequences is that for event propagation an abstract
		 * Ceylan::Controller is not suitable: one cannot call such specific
		 * methods like axisChanged. 
		 * That is the explanation too for the
		 * OSDL::Events::Controller::getActualController method.
		 *
		 * Some basic abstraction efforts are done, though. 
		 * For example, according to the keyboard mode (raw or text), the
		 * controller is notified differently for more versality.
		 *
		 * Similarly, if a controller is linked with a classical joystick, 
		 * the joystick actual direction will be reported thanks to the
		 * joystickLeft, joystickRight, joystickUp and joystickDown methods
		 * instead of the unspecialized axisChanged method.
		 * Same thing for joystick{First|Second}Button{Pressed|Released}.
		 *
		 * @note Controllers should be linked to at most one Classical 
		 * joystick, since the joystick index is not propagated.
		 *
		 * These controllers are mostly designed to capture flow of 
		 * informations coming from an actual input device. 
		 *
		 * However, most modelings of AI behaviours will have to fit into
		 * the controller interface. 
		 * Basically, the AI of, say, a driver, should be uncoupled from
		 * the physics that apply to his car. 
		 * In this case, the driver is only a particular controller for that
		 * car, and its only options may be for him to steer the wheel or 
		 * press the gas pedal. 
		 * Whatever is the cause of these actions, this is the task of 
		 * the model to take them into account and update the object state
		 * accordingly.
		 *
		 * AI-based controllers may inherit from active objects, so that 
		 * they can be scheduled as well.
		 *
		 */
		class OSDL_DLL Controller : public Ceylan::Controller
		{
		
			
			
			public:



				/**
				 * Constructs a new controller.
				 *
				 */
				Controller() ;
				
				
				
				/**
				 * Constructs a controller that will send events to the
				 * specified model.
				 *
				 */
				explicit Controller( Ceylan::Model & model ) ;
				
				
				
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~Controller() throw() ;
				



				// Keyboard section.
				
				
				
				/**
				 * Called whenever a keyboard linked to this controller has 
				 * gained focus.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void keyboardFocusGained( 
					const FocusEvent & keyboardFocusEvent ) ;



				/**
				 * Called whenever a keyboard linked to this controller has 
				 * lost focus.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void keyboardFocusLost( 
					const FocusEvent & keyboardFocusEvent ) ;


			
				/**
				 * Called whenever a raw key linked to this controller has 
				 * been pressed.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void rawKeyPressed( 
					const KeyboardEvent & keyboardPressedEvent ) ;



				/**
				 * Called whenever a raw key linked to this controller has 
				 * been released.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void rawKeyReleased( 
					const KeyboardEvent & keyboardReleasedEvent ) ;



				/**
				 * Called whenever a Unicode character linked to this 
				 * controller has been selected.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void unicodeSelected( 
					const KeyboardEvent & keyboardPressedEvent ) ;





				// Mouse section.
				

				
				/**
				 * Called whenever a mouse linked to this controller has 
				 * gained focus.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void mouseFocusGained( 
					const FocusEvent & mouseFocusEvent ) ;



				/**
				 * Called whenever a mouse linked to this controller has 
				 * lost focus.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void mouseFocusLost(
					const FocusEvent & mouseFocusEvent ) ;


				
				/**
				 * Called whenever a mouse linked to this controller moved.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void mouseMoved( 
					const MouseMotionEvent & mouseMotionEvent ) ;



				/**
				 * Called whenever a mouse linked to this controller 
				 * had one of its buttons pressed.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void mouseButtonPressed( 
					const MouseButtonEvent & mouseButtonPressedEvent ) ;



				/**
				 * Called whenever a mouse linked to this controller had 
				 * one of its buttons released.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void mouseButtonReleased( 
					const MouseButtonEvent & mouseButtonReleasedEvent ) ;




				// Joystick section.



				/**
				 * Called whenever a joystick linked to this controller 
				 * had one of its axes changed.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void joystickAxisChanged( 
					const JoystickAxisEvent & joystickAxisEvent ) ;



				/**
				 * Called whenever a joystick linked to this controller 
				 * had one of its trackballs changed.
				 *
				 * @note This method is meant to be overriden so that 
				 * this controller can react to this event.
				 *
				 */
				virtual void joystickTrackballChanged( 
					const JoystickTrackballEvent & joystickTrackballEvent ) ;



				/**
				 * Called whenever a joystick linked to this controller 
				 * had one of its hats changed.
				 *
				 * @note This method is meant to be overriden so that 
				 * this controller can react to this event.
				 *
				 */
				virtual void joystickHatChanged( 
					const JoystickHatEvent & joystickHatChangedEvent ) ;



				/**
				 * Called whenever a joystick linked to this controller 
				 * had one of its buttons pressed.
				 *
				 * @note This method is meant to be overriden so that 
				 * this controller can react to this event.
				 *
				 */
				virtual void joystickButtonPressed( 
					const JoystickButtonEvent & joystickButtonPressedEvent ) ;



				/**
				 * Called whenever a joystick linked to this controller 
				 * had one of its buttons changed.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void joystickButtonReleased( 
					const JoystickButtonEvent & joystickButtonReleasedEvent ) ;


				
				
				
				// Classical joystick section.
				
				
				
				/**
				 * Called whenever the classical joystick linked to this
				 * controller is pushed to the left.
				 *
				 * @param leftExtent the extent, ranging for [0,32767], of 
				 * the joystick position to the left.
				 *
				 * @note The deadzone d, if non null, will clamp extent 
				 * values to ]d,32767].
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void joystickLeft( Events::AxisPosition leftExtent ) ;
				
				
				
				/**
				 * Called whenever the classical joystick linked to this
				 * controller is pushed to the right.
				 *
				 * @param rightExtent the extent, ranging for [0,32767], of 
				 * the joystick position to the right.
				 *
				 * @note The deadzone d, if non null, will clamp extent 
				 * values to ]d,32767].
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void joystickRight( Events::AxisPosition rightExtent ) ;
				
				
				
				/**
				 * Called whenever the classical joystick linked to this
				 * controller is pushed up.
				 *
				 * @param upExtent the up extent, ranging for [0,32767], 
				 * of the joystick position.
				 *
				 * @note The deadzone d, if non null, will clamp extent 
				 * values to ]d,32767].
				 *
				 * @note This method is meant to be overriden so that 
				 * this controller can react to this event.
				 *
				 */
				virtual void joystickUp( Events::AxisPosition upExtent )  ;
				
				
				
				/**
				 * Called whenever the classical joystick linked to this
				 * controller is pushed down.
				 *
				 * @param downExtent the down extent, ranging for [0,32767], 
				 * of the joystick position.
				 *
				 * @note The deadzone d, if non null, will clamp extent 
				 * values to ]d,32767].
				 *
				 * @note This method is meant to be overriden so that 
				 * this controller can react to this event.
				 *
				 */
				virtual void joystickDown( Events::AxisPosition downExtent ) ;
				
				
		
				/**
				 * Called whenever the first button of the classical 
				 * joystick linked to this controller, if any, is pressed.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void joystickFirstButtonPressed() ;
				  
				  
				  
				/**
				 * Called whenever the first button of the classical 
				 * joystick linked to this controller, if any, is released.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void joystickFirstButtonReleased() ;
				  
				  
				  
				/**
				 * Called whenever the second button of the classical 
				 * joystick linked to this controller, if any, is pressed.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void joystickSecondButtonPressed() ;
				  
				  
				  
				/**
				 * Called whenever the second button of the classical 
				 * joystick linked to this controller, if any, is released.
				 *
				 * @note This method is meant to be overriden so that this
				 * controller can react to this event.
				 *
				 */
				virtual void joystickSecondButtonReleased() ;
				  
				  

	            /**
	             * Returns an user-friendly description of the state 
				 * of this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall 
				 * settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;
			
				
							

			private:
		
		
		
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Controller( const Controller & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Controller & operator = ( const Controller & source ) ;
										
				
		} ;
		
	
	}	
	
}



#endif // OSDL_CONTROLLER_H_


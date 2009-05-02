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


#ifndef OSDL_MOUSE_H_
#define OSDL_MOUSE_H_


#include "OSDLMouseCommon.h"       // for MouseNumber, etc.
#include "OSDLInputDevice.h"       // for inheritance
#include "OSDLVideoTypes.h"        // for Coordinate


#include <string>
#include <list>





namespace OSDL
{



	namespace Video
	{
	
	
		namespace TwoDimensional
		{
		
			// For cursor position.
			class Point2D ;
		
		}
		
		
	}
			
	
		
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
		 * For usual needs, Mouse instances should be set in classical
		 * mode: it would be more convenient, since it is a higher-level
		 * (but a little less general-purpose) model.
		 *
		 */
		class OSDL_DLL Mouse: public OSDL::Events::InputDevice
		{
		
		
			/*
			 * The mouse handler has to trigger the mouse event callbacks
			 * for this mouse.
			 *
			 */
			friend class OSDL::Events::MouseHandler ;
			
			
			
			public:



				/**
				 * Constructs a new mouse instance.
				 *
				 * @param index the index of this mouse in platform list.
				 *
				 * @param classicalMouseMode tells whether this mouse should
				 * be in classical mode (higher level), or in normal mode
				 * (low-level events only are propagated to controllers).
				 *
				 * @throw MouseException if the operation failed or is not
				 * supported.
				 *
				 */
				explicit Mouse( MouseNumber index, 
					bool classicalMouseMode = true ) ;
				
				
				
				/**
				 * Virtual destructor, closing corresponding mouse device.
				 *
				 */
				virtual ~Mouse() throw() ;
				
				



				// Cursor position.
				
				
				
				/**
				 * Returns the absolute abscissa of this mouse cursor.
				 *
				 */ 
				virtual Video::Coordinate getCursorAbscissa() const ;
				 				
				
				
				/**
				 * Returns the absolute ordinate of this mouse cursor.
				 *
				 */ 
				virtual Video::Coordinate getCursorOrdinate() const ;
				 
				 
				 
				/**
				 * Sets the cursor position for this mouse.
				 *
				 * @param newPosition the new cursor position.
				 *
				 * @note Generation a mouse moved event.
				 *
				 */
				virtual void setCursorPosition( 
					const Video::TwoDimensional::Point2D & newPosition ) const ;
				 
				 
				 
				/**
				 * Sets the cursor position for this mouse.
				 *
				 * @param x the new cursor abscissa.
				 *
				 * @param y the new cursor ordinate.
				 *
				 * @note Generation a mouse moved event.
				 *
				 */
				virtual void setCursorPosition( Video::Coordinate x, 
					Video::Coordinate y ) const ;
				 
				
				
				/**
				 * Returns whether the cursor of this mouse is visible.
				 *
				 * @return true iff the cursor is visible.
				 *
				 */ 
				virtual bool getCursorVisibility() const ;

				 
				 
				/**
				 * Sets the visibility for the cursor of this mouse.
				 *
				 * @param on cursor rendering will be enabled iff true.
				 *
				 * @note The mouse cursor starts as visible.
				 *
				 */ 
				virtual void setCursorVisibility( bool on ) ;
				 
				
				
				
				// Buttons & wheels section.
				
				
				
				/**
				 * Returns the number of actual mouse buttons, wheels not
				 * included.
				 *
				 */
				virtual MouseButtonNumber getNumberOfButtons() const ;
				 
				 
				 
				/**
				 * Returns the number of mouse wheels.
				 *
				 * @note Information not available yet, returns always one.
				 *
				 */
				virtual MouseWheelNumber getNumberOfWheels() const ;
				
				
				
				/**
				 * Tells whether the left button of this mouse is currently
				 * pressed. 
				 *
				 * @return true iff this button is pressed.
				 *
				 */ 
				virtual bool isLeftButtonPressed() const ;
				 
				 

				/**
				 * Tells whether the middle button of this mouse is currently
				 * pressed. 
				 *
				 * @return true iff this button is pressed.
				 *
				 */ 
				virtual bool isMiddleButtonPressed() const ;
				 


				/**
				 * Tells whether the right button of this mouse is currently
				 * pressed. 
				 *
				 * @return true iff this button is pressed.
				 *
				 */ 
				virtual bool isRightButtonPressed() const ;
				
				
				 
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
				virtual bool isButtonPressed( MouseButtonNumber buttonNumber ) 
					const ;				 
				 
				 
				 
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
				virtual MouseButtonMask getButtonStates() const ;   
				 				 			
							
								 					
				/**
				 * Updates the description of this mouse, for relative 
				 * movements of the cursor.
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
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;
				
				
				
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
					MouseButtonNumber buttonToInspect ) ;
					
					
					
				/**
				 * By default, a mouse is supposed to have 5 buttons:
				 * left, middle, right, wheel up and down. 
				 *	
				 */	
				static const MouseButtonNumber DefaultButtonTotalNumber ;
				
				
				
				/**
				 * By default, a mouse is supposed to have 3 actual buttons:
				 * left, middle, right. 
				 *	
				 */	
				static const MouseButtonNumber DefaultButtonActualNumber ;
				
				
				
				/**
				 * By default, a mouse is supposed to have 1 wheel.
				 *	
				 */	
				static const MouseWheelNumber DefaultWheelNumber ;
				
				
				
				
		protected:
		
		
		
				/**
				 * Returns the index of this mouse in the platform list.
				 *
				 * @note Should be useless.
				 *
				 */
				virtual MouseNumber getIndex() const ;
		
		
		
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
				 * @param mouseFocusEvent the event about the change in 
				 * mouse focus.
				 *
				 */
				virtual void focusGained( 
					const FocusEvent & mouseFocusEvent ) ;
	
	
	
				/**
				 * Called whenever this mouse lost focus, and 
				 * notifies the linked controller, if any.
				 *
				 * @param mouseFocusEvent the event about the change in 
				 * mouse focus.
				 *
				 */
				virtual void focusLost(	const FocusEvent & mouseFocusEvent ) ;
	
	
	
	
				// Cursor position section.
	
	
	
				/**
				 * Called whenever this mouse moved, and 
				 * notifies the linked controller, if any.
				 *
				 * @param mouseEvent the corresponding mouse event.
				 *
				 */
				virtual void mouseMoved( 
					const MouseMotionEvent & mouseEvent ) ;
		
		

				/**
				 * Called whenever a button of this mouse was pressed, 
				 * and notifies the linked controller, if any.
				 *
				 * @param mouseEvent the corresponding mouse event.
				 *
				 */
				virtual void buttonPressed( 
					const MouseButtonEvent & mouseEvent ) ;
	
	
	
				/**
				 * Called whenever a button of this mouse was released, 
				 * and notifies the linked controller, if any.
				 *
				 * @param mouseEvent the corresponding mouse event.
				 *
				 */
				virtual void buttonReleased( 
					const MouseButtonEvent & mouseEvent ) ;
				
				
				
				
				/// The number of this mouse in platform list.				
				MouseNumber _index ;
				
				
				
				/// Tells whether this mouse should be in classical mode.
				bool _inClassicalMode ; 



				/// The number of total buttons for this mouse.
				MouseButtonNumber _buttonTotalCount ;


				/// Records the last relative abscissa.
				Video::Coordinate _lastRelativeAbscissa ;
				
				
				/// Records the last relative ordinate.
				Video::Coordinate _lastRelativeOrdinate ;
				



		private:
		
		
		
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Mouse( const Mouse & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Mouse & operator = ( const Mouse & source ) ;
										
				
				
		} ;
		
	
	}	
	
}



#endif // OSDL_MOUSE_H_


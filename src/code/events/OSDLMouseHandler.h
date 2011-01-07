/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#ifndef OSDL_MOUSE_HANDLER_H_
#define OSDL_MOUSE_HANDLER_H_



#include "OSDLInputDeviceHandler.h"   // for inheritance
#include "OSDLMouseCommon.h"          // for MouseNumber, etc.


#include <string>
#include <list>




namespace OSDL
{



	namespace MVC
	{

		/*
		 * The mouse handler can propagate mouse events to the relevant
		 * controllers.
		 *
		 */
		class Controller ;
	
	}

		
		
	namespace Events
	{
	
	
	
		// Manages mice.
		class Mouse ;
		
		
		
		/**
		 * Handler for mice connected to the system, if any.
		 *
		 * Manages the mouse position, its up to three buttons, and a 
		 * possible mouse wheel (up/down).
		 *
		 * @note The UseMouse flag must be specified  in the call to 
		 * CommonModule::getCommonModule so that mouse support is enabled.
		 *
		 * @note No multiple mice are managed for the moment.
		 *
		 */
		class OSDL_DLL MouseHandler: public InputDeviceHandler
		{
		

			/*
			 * The events module has to trigger the mouse event callbacks
			 * of this mouse handler.
			 *
			 */
			friend class OSDL::Events::EventsModule ;

			
			public:




				/**
				 * Constructs a new mouse handler.
				 *
				 * @param useClassicalMice tells whether detected 
				 * mice should be managed as complex mice 
				 * (powerful interface, but it requires the controller to 
				 * take in charge this complexity) or classical mice 
				 * (for usual needs, comes with preset behaviours which ease
				 * controller work).
				 *
				 * @throw InputDeviceHandlerException if the initialization 
				 * of the mouse subsystem failed.
				 *
				 */
				explicit MouseHandler( bool useClassicalMice = true ) ;
				
				
				
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~MouseHandler() throw() ;



				/**
				 * Returns whether a default mouse is available.
				 *
				 * @return true iff a default mouse is available.
				 *
				 */
				bool hasDefaultMouse() const ;
				 


				/**
				 * Returns the default mouse, if any.
				 *
				 * @throw MouseException if no default mouse is available.
				 *
				 */
				Mouse & getDefaultMouse() ;



				/**
				 * Links the default mouse to specified controller, so that 
				 * any further mouse-related event will be sent to this
				 * controller. 
				 *
				 * Removes automatically any link previously defined between
				 * the mouse and any other controller.
				 *
				 * @param controller the OSDL controller which will be 
				 * notified of mouse changes.
				 *
				 * @throw MouseException if the operation failed.
				 *
				 */
				virtual void linkToController( 
					OSDL::MVC::Controller & controller ) ;
				 
				 
	
				/**
				 * Links the specified mouse to specified controller, so that 
				 * any further mouse-related event will be sent to this
				 * controller. 
				 *
				 * Removes automatically any link previously defined between
				 * the mouse and any other controller.
				 *
				 * @param index the index of the mouse to link, in case
				 * there are other mice than the main default one.
				 *
				 * @param controller the OSDL controller which will be 
				 * notified of mouse changes.
				 *
				 * @throw MouseException if the operation failed.
				 *
				 */
				virtual void linkToController( MouseNumber index,
					OSDL::MVC::Controller & controller ) ;
				 
			
			
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
				
				
				
				
				// Static section.
				
				 
				 
				/**
				 * Returns the number of available mice, i.e. the 
				 * mice currently attached to the system.
				 *
				 * @return always one, for the moment.
				 * 
				 */
				static MouseNumber GetAvailableMiceCount() ;
				
				
				
				
		protected:
						
					
					
				/*
				 * Event-driven callbacks for input propagation, from
				 * the Events module to this handler.
				 *
				 */
			
			
					
				/**
				 * Called whenever a mouse focus is gained by the 
				 * application.
				 *
				 * @param mouseFocusEvent the mouse event about gained focus.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */ 
				virtual void focusGained( 
					const FocusEvent & mouseFocusEvent ) const ;
				
				
				
				/**
				 * Called whenever a mouse focus is lost by the 
				 * application.
				 *
				 * @param mouseFocusEvent the mouse event about lost focus.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */ 
				virtual void focusLost( 
					const FocusEvent & mouseFocusEvent ) const ;
				 
						
						 	
				/**
				 * Called whenever a mouse is moved, so that its
				 * controller, if any, is notified.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */
				virtual void mouseMoved( 
					const MouseMotionEvent & mouseMovedEvent ) const ;
				
				
							
				/**
				 * Called whenever a mouse button was pressed, so 
				 * that its controller, if any, is notified.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */
				virtual void buttonPressed( 
					const MouseButtonEvent & mouseButtonPressedEvent ) const ;
						
							
							
				/**
				 * Called whenever a mouse button was released, so 
				 * that its controller, if any, is notified.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */
				virtual void buttonReleased( 
					const MouseButtonEvent & mouseButtonReleasedEvent ) const ;



				/**
				 * Blanks this mouse handler, i.e. makes it forget all
				 * previously known mice.
				 *
				 */
				virtual void blank() ;


				
				/**
				 * Checks that the mouse at the specified index exists 
				 * and is opened.
				 *
				 * @param index the index of the mouse to check.
				 *
				 * @note Stop in emergency the application if the check failed.
				 *
				 */						
				virtual void checkMouseAt( MouseNumber index ) const ;
				
				
				

				/**
				 * Records how many mice are known by this handler.
				 *
				 */
				MouseNumber _miceCount ;
				
				
				 
				/**
				 * Records all known mice.
				 *
				 * A dynamic array is preferred to a list for performance
				 * reasons.
				 *
				 * (read it as Mouse * _mice[])
				 *
				 */
				Mouse ** _mice ;



				/**
				 * Tells whether complex mice (if false) or classical
				 * mice (if true) should be created.
				 *
				 */
				bool _useClassicalMice ;




			private:
		
		
		
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit MouseHandler( const MouseHandler & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				MouseHandler & operator = ( const MouseHandler & source ) ;
										
				
		} ;
	
	
	}	
	
}



#endif // OSDL_MOUSE_HANDLER_H_


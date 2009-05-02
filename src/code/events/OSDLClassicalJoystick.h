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
		class OSDL_DLL ClassicalJoystick: public Joystick
		{
		
			
			/*
			 * The joystick handler has to trigger the joystick event callbacks
			 * for this classical joystick.
			 *
			 */
			friend class OSDL::Events::JoystickHandler ;
			
			
			
			public:



				/**
				 * Constructs a new joystick handler.
				 *
				 * @param index the index of this joystick in platform list.
				 *
				 * @param deadZoneExtent the extent of the joystick dead zone.
				 *
				 * @throw JoystickException if the operation failed or is
				 * not supported.
				 *
				 */
				explicit ClassicalJoystick( JoystickNumber index, 
					AxisPosition deadZoneExtent = DefaultDeadZoneExtent ) ;
				
				
				
				/**
				 * Virtual destructor, closing corresponding joystick device.
				 *
				 */
				virtual ~ClassicalJoystick() throw() ;
				


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
					AxisPosition & secondAxisExtent ) const ;
					
				
				
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
					AxisPosition secondAxisExtent = DefaultDeadZoneExtent ) ;
				 	
					
								
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
				
				
				
				static const AxisPosition DefaultDeadZoneExtent ;
				
			
			
			
			
		protected:
		
		

				/*
				 * Event-driven callbacks for input propagation, from
				 * the joystick handler to this classical joystick.
				 *
				 */

				
				
				/**
				 * Called whenever an axis of this joystick changed, and 
				 * notify the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 * @note Expected to be triggered by the Joystick handler.
				 *
				 */
				virtual void axisChanged( 
					const JoystickAxisEvent & joystickEvent ) ;
		
		
		
				/**
				 * Called whenever a button of this joystick was pressed, 
				 * and notify the linked controller, if any.
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
				 * and notify the linked controller, if any.
				 *
				 * @param joystickEvent the corresponding joystick event.
				 *
				 * @note Expected to be triggered by the Joystick handler.
				 *
				 */
				virtual void buttonReleased( 
					const JoystickButtonEvent & joystickEvent ) ;
				


				/**
				 * Stores the dead zone extent for first axis. 
				 * If d is the dead zone value, then a reported joystick 
				 * axis value in:
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
				 * axis value in:
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
				explicit ClassicalJoystick( const Joystick & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				ClassicalJoystick & operator = ( const Joystick & source ) ;
										
				
		} ;
		
	
	}	
	
}



#endif // OSDL_CLASSICAL_JOYSTICK_H_


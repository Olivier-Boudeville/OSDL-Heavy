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


#ifndef OSDL_JOYSTICK_HANDLER_H_
#define OSDL_JOYSTICK_HANDLER_H_



#include "OSDLInputDeviceHandler.h"   // for inheritance
#include "OSDLJoystickCommon.h"       // for JoystickNumber, etc.


#include <string>
#include <list>




namespace OSDL
{



	namespace MVC
	{

		/*
		 * The joystick handler can propagate joystick events to the relevant
		 * controllers.
		 *
		 */
		class Controller ;

	}



	namespace Events
	{



		// Manages joysticks.
		class Joystick ;




		/**
		 * Handler for all detected joysticks.
		 *
		 * Joystick indexes range from 0 (included) to
		 * GetAvailableJoystickCount() (excluded).
		 *
		 * @note The UseJoystick flag must be specified in the call to
		 * CommonModule::getCommonModule so that joystick support is enabled.
		 *
		 */
		class OSDL_DLL JoystickHandler : public InputDeviceHandler
		{


			/*
			 * The events module has to trigger the joystick event callbacks of
			 * this joystick handler.
			 *
			 */
			friend class OSDL::Events::EventsModule ;



			public:



				/**
				 * Constructs a new joystick handler.
				 *
				 * @param useClassicalJoysticks tells whether detected joysticks
				 * should be managed as complex joysticks (powerful interface,
				 * but it requires the controller to take in charge this
				 * complexity) or classical joysticks (for usual needs, comes
				 * with preset behaviours which ease controller work).
				 *
				 * @throw InputDeviceHandlerException if the initialization of
				 * the joystick subsystem failed.
				 *
				 */
				explicit JoystickHandler( bool useClassicalJoysticks = true ) ;



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
				virtual void update() ;



				/**
				 * Opens joystick of specified index.
				 * If joystick is already opened, does nothing.
				 *
				 * @param index the index of the joystick to open.
				 *
				 * @throw JoystickException if index is out of bounds.
				 *
				 * @see GetAvailableJoystickCount
				 *
				 */
				virtual void openJoystick( JoystickNumber index ) ;



				/**
				 * Links specified joystick to specified controller, so that any
				 * further joystick-related event will be sent to the
				 * controller.
				 *
				 * Removes automatically any link previously defined between
				 * this joystick and any other controller.
				 *
				 * @param index the index of the joystick that shall be linked
				 * to the controller.
				 *
				 * @param controller the OSDL controller which will be notified
				 * of joystick changes.
				 *
				 * @throw JoystickException if index is out of bounds.
				 *
				 */
				virtual void linkToController( JoystickNumber index,
					OSDL::MVC::Controller & controller ) ;



				/**
				 * Returns an user-friendly description of the state of this
				 * object.
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
				 * Returns the number of available joysticks, i.e. the joysticks
				 * currently attached to the system.
				 *
				 */
				static JoystickNumber GetAvailableJoystickCount() ;




		protected:



				/*
				 * Event-driven callbacks for input propagation, from the Events
				 * module to this handler.
				 *
				 */



				/**
				 * Called whenever a joystick axis changed, so that its
				 * controller, if any, is notified.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */
				virtual void axisChanged(
					const JoystickAxisEvent & joystickEvent ) const ;



				/**
				 * Called whenever a joystick trackball changed, so that its
				 * controller, if any, is notified.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */
				virtual void trackballChanged(
					const JoystickTrackballEvent & joystickEvent ) const ;



				/**
				 * Called whenever a joystick hat changed, so that its
				 * controller, if any, is notified.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */
				virtual void hatChanged(
					const JoystickHatEvent & joystickEvent ) const ;



				/**
				 * Called whenever a joystick button was pressed, so that its
				 * controller, if any, is notified.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */
				virtual void buttonPressed(
					const JoystickButtonEvent & joystickEvent )	const ;



				/**
				 * Called whenever a joystick button was released, so that its
				 * controller, if any, is notified.
				 *
				 * @note Expected to be triggered by the EventsModule.
				 *
				 */
				virtual void buttonReleased(
					const JoystickButtonEvent & joystickEvent )	const ;



				/**
				 * Blanks this joystick handler, i.e. makes it forget all
				 * previously known joysticks.
				 *
				 */
				virtual void blank() ;



				/**
				 * Checks that the joystick at the specified index exists and is
				 * opened.
				 *
				 * @param index the index of the joystick to check.
				 *
				 * @note Stop in emergency the application if the check failed.
				 *
				 */
				virtual void checkJoystickAt( JoystickNumber index ) const ;




				/**
				 * Records how many joysticks are known by this handler.
				 *
				 */
				JoystickNumber _joystickCount ;



				/**
				 * Records all known joysticks.
				 *
				 * A dynamic array is preferred to a list for performance
				 * reasons.
				 *
				 * (read it as Joystick * _joysticks[])
				 *
				 */
				Joystick ** _joysticks ;



				/**
				 * Tells whether complex joysticks (if false) or classical
				 * joysticks (if true) should be created.
				 *
				 */
				bool _useClassicalJoysticks ;




			private:



				/**
				 * Copy constructor made private to ensure that it will never be
				 * called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 *
				 */
				explicit JoystickHandler( const JoystickHandler & source ) ;



				/**
				 * Assignment operator made private to ensure that it will never
				 * be called.
				 *
				 * The compiler should complain whenever this undefined operator
				 * is called, implicitly or not.
				 *
				 */
				JoystickHandler & operator = (
					const JoystickHandler & source ) ;



		} ;


	}


}



#endif // OSDL_JOYSTICK_HANDLER_H_

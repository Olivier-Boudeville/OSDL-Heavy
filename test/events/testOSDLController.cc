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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Events ;


using namespace Ceylan::Log ;


#include <iostream>  // for cout
using std::cout ;
using std::endl ;

#include <string>
using std::string ;




class MyController : public OSDL::MVC::Controller
{

	public:


		MyController( EventsModule & events ) throw():
			_direction( 1 ),
			_events( & events )
		{


		}


		void joystickButtonPressed(
			const JoystickButtonEvent & joystickButtonPressedEvent ) throw()
		{

			_events->requestQuit() ;

		}


		void rawKeyPressed( const KeyboardEvent & keyboardPressedEvent ) throw()
		{

			switch( keyboardPressedEvent.keysym.sym )
			{

				case KeyboardHandler::UpArrowKey:
					_direction = 1 ;
					break ;

				case KeyboardHandler::DownArrowKey:
					_direction = 2 ;
					break ;

				case KeyboardHandler::LeftArrowKey:
					_direction = 3 ;
					break ;

				case KeyboardHandler::RightArrowKey:
					_direction = 4 ;
					break ;

				case KeyboardHandler::EnterKey:
					_events->requestQuit() ;
					break ;

				default:
					// Do nothing.
					break ;
			}

			cout << "                  " << toString() << std::endl ;


		}



		void joystickUp( AxisPosition leftExtent ) throw()
		{

			_direction = 1 ;
			cout << "                  " << toString() ;

		}


		void joystickDown( AxisPosition leftExtent ) throw()
		{

			_direction = 2 ;
			cout << "                  " << toString() ;

		}


		void joystickLeft( AxisPosition leftExtent ) throw()
		{

			_direction = 3 ;
			cout << "                  " << toString() ;

		}


		void joystickRight( AxisPosition leftExtent ) throw()
		{

			_direction = 4 ;
			cout << "                  " << toString() ;

		}


		const Ceylan::Event & getEventFor(
				const Ceylan::CallerEventListener & listener )
			throw( Ceylan::EventException )
		{

			throw Ceylan::EventException( "MyController::getEventFor: "
				"not expected to be called." ) ;

		}


		/*

		 If no classical joystick is to be used:

		void joystickAxisChanged( const JoystickAxisEvent & joystickAxisEvent )
			throw()
		{

			// Dead zones: [-1000; 1000]

			if ( joystickAxisEvent.axis == 0 )
			{
				if ( joystickAxisEvent.value < -1000 )
					_direction = 3 ;
				else if ( joystickAxisEvent.value > -1000 )
					_direction = 4 ;
			}
			else if ( joystickAxisEvent.axis == 1 )
			{
				if ( joystickAxisEvent.value < -1000 )
					_direction = 1 ;
				else if ( joystickAxisEvent.value > -1000 )
					_direction = 2;
			}

			cout << "                  " << toString() ;

		}

		*/


		const string toString( Ceylan::VerbosityLevels level = Ceylan::high )
			const throw()
		{

			switch( _direction )
			{
				case 1:
					return "^" ;
				case 2:
					return "v" ;
				case 3:
					return "<" ;
				case 4:
					return ">" ;
			}

			return "(unexpected direction selected)" ;

		}


	protected:

		/// 1: up, 2: down, 3: left, 4: right.
		Ceylan::Uint8 _direction ;

		EventsModule * _events ;

} ;



/**
 * Testing OSDL Controller integration with input layer.
 *
 */
int main( int argc, char * argv[] )
{


	LogHolder myLog( argc, argv ) ;


	try
	{


		LogPlug::info( "Testing OSDL controller to input device bridge." ) ;


		bool isBatch = false ;

		std::string executableName ;
		std::list<std::string> options ;

		Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;

		std::string token ;
		bool tokenEaten ;


		while ( ! options.empty() )
		{

			token = options.front() ;
			options.pop_front() ;

			tokenEaten = false ;

			if ( token == "--batch" )
			{

				LogPlug::info( "Batch mode selected" ) ;
				isBatch = true ;
				tokenEaten = true ;
			}

			if ( token == "--interactive" )
			{
				LogPlug::info( "Interactive mode selected" ) ;
				isBatch = false ;
				tokenEaten = true ;
			}

			if ( token == "--online" )
			{
				// Ignored:
				tokenEaten = true ;
			}

			if ( LogHolder::IsAKnownPlugOption( token ) )
			{
				// Ignores log-related (argument-less) options.
				tokenEaten = true ;
			}


			if ( ! tokenEaten )
			{
				throw Ceylan::CommandLineParseException(
					"Unexpected command line argument: " + token ) ;
			}

		}

		LogPlug::info( "Starting OSDL with keyboard and joystick support." ) ;

		// Will trigger the video module as well for events:
		OSDL::CommonModule & myOSDL = OSDL::getCommonModule(
			CommonModule::UseJoystick | CommonModule::UseKeyboard ) ;

		LogPlug::info( "Testing basic event handling." ) ;

		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ;

		LogPlug::info( "Events module: " + myEvents.toString() ) ;

		myEvents.getKeyboardHandler().setSmarterDefaultKeyHandlers() ;

		JoystickHandler & myJoystickHandler = myEvents.getJoystickHandler() ;
		LogPlug::info( "Current joystick handler is: "
			+ myJoystickHandler.toString( Ceylan::high ) ) ;

		JoystickNumber joyCount =
			myJoystickHandler.GetAvailableJoystickCount() ;

		if ( joyCount > 0 )
		{

			LogPlug::info( "There are " + Ceylan::toString( joyCount )
				+ " attached joystick(s), opening them all." ) ;

			for ( JoystickNumber i = 0 ; i < joyCount; i++ )
				myJoystickHandler.openJoystick( i ) ;

			LogPlug::info( "New joystick handler state is: "
				+ myJoystickHandler.toString( Ceylan::high ) ) ;

		}
		else
		{

			LogPlug::info( "There is no joystick attached." ) ;

		}

		LogPlug::info( "New joystick handler state is: "
			+ myJoystickHandler.toString( Ceylan::high ) ) ;

		LogPlug::info( "Displaying a dummy window "
			"to have access to an event queue." ) ;

		LogPlug::info( "Getting video." ) ;
		OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ;

		// A window is needed to have the event system working:
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ;

		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

		MyController aController( myEvents ) ;


		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::UpArrowKey, aController ) ;

		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::DownArrowKey, aController ) ;

		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::LeftArrowKey, aController ) ;

		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::RightArrowKey, aController ) ;

		myEvents.getKeyboardHandler().linkToController(
			KeyboardHandler::EnterKey, aController ) ;


		if ( joyCount > 0 )
			myJoystickHandler.linkToController( /* first joystick */ 0,
				aController ) ;


		if ( isBatch )
		{

			LogPlug::warning( "Main loop not launched, as in batch mode." ) ;

		}
		else
		{

			LogPlug::info( "Entering the event loop "
				"for event waiting so that Controller can act." ) ;

			std::cout << std::endl
				<< "When the joystick is pushed, a character "
				"('<' or '>' or '^' or 'v', for left, right, up and down) "
				"describes the direction it is aimed at." << std::endl
				<< "< Hit Enter on the OSDL window, or push the first button "
				"of the first joystick (if any) "
				"to end OSDL controller test >" << std::endl ;

			myEvents.enterMainLoop() ;
			LogPlug::info( "Exiting main loop." ) ;

		}

		LogPlug::info( "End of OSDL controller test." ) ;

		LogPlug::info( "stopping OSDL." ) ;
		OSDL::stop() ;

	}

	catch ( const OSDL::Exception & e )
	{

		LogPlug::error( "OSDL exception caught: "
			 + e.toString( Ceylan::high ) ) ;
		return Ceylan::ExitFailure ;

	}

	catch ( const Ceylan::Exception & e )
	{

		LogPlug::error( "Ceylan exception caught: "
			 + e.toString( Ceylan::high ) ) ;
		return Ceylan::ExitFailure ;

	}

	catch ( const std::exception & e )
	{

		LogPlug::error( "Standard exception caught: "
			 + std::string( e.what() ) ) ;
		return Ceylan::ExitFailure ;

	}

	catch ( ... )
	{
		LogPlug::error( "Unknown exception caught" ) ;
		return Ceylan::ExitFailure ;

	}

	OSDL::shutdown() ;

	return Ceylan::ExitSuccess ;

}

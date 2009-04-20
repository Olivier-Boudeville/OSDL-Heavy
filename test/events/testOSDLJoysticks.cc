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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Events ;
using namespace OSDL::Video ;

using namespace Ceylan::Log ;



#include <iostream>  // for cout, endl



class MyController : public OSDL::MVC::Controller
{

	public:
	
	
		MyController( EventsModule & eventsModule ) throw() :
			_eventsModule( & eventsModule )
		{
		
		
		}
		
		
		void rawKeyPressed( const KeyboardEvent & keyboardPressedEvent ) throw()
		{
			LogPlug::info( "A key was pressed, exiting." ) ;
			_eventsModule->requestQuit() ;
		}
		
		
		void joystickAxisChanged( const JoystickAxisEvent & joystickAxisEvent )
			throw()
		{
			std::cout << "Joystick is moving..." << std::endl ;
		}
		
		
		const Ceylan::Event & getEventFor( 
				const Ceylan::CallerEventListener & listener ) 
			throw( Ceylan::EventException )
		{
			throw Ceylan::EventException( "MyController::getEventFor: "
				"not expected to be called." ) ;
		}
		
		
	protected:	
		
		EventsModule * _eventsModule ;
		
		
} ;




/**
 * Testing OSDL joystick handling.
 *
 */
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Testing OSDL joystick services." ) ;


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
		
		
		LogPlug::info( "Starting OSDL with joystick support." ) ;		
        OSDL::CommonModule & myOSDL = OSDL::getCommonModule(
			CommonModule::UseJoystick ) ;		
		
		LogPlug::info( "Testing basic event handling." ) ;
		
		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ; 
		
		LogPlug::info( "Events module: " + myEvents.toString() ) ;
		
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
		
			LogPlug::info( "No joystick to test, stopping test." ) ;
			OSDL::stop() ;
			return Ceylan::ExitSuccess ;		
		
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
		
		LogPlug::info( "Using the first joystick" ) ;
		
		MyController myController( myEvents ) ;
		
		if ( joyCount > 0 )
			myJoystickHandler.linkToController( /* first joystick */ 0,
				myController ) ;

		if ( isBatch )
		{
		
			LogPlug::warning( "Main loop not launched, as in batch mode." ) ;
			
		}
		else
		{
		
			LogPlug::info( "Entering the event loop "
				"for event waiting so that joystick can be tested." ) ;

			std::cout << std::endl << "< Push the first button of joystick "
			"or hit any key to stop this test >" << std::endl ;
		
			myEvents.enterMainLoop() ;
			LogPlug::info( "Exiting main loop." ) ;	
				
		}

		
				
		LogPlug::info( "End of joystick test." ) ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
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

    return Ceylan::ExitSuccess ;

}


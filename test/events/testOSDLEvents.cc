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



void exampleKeyHandler( const KeyboardEvent & keyboardEvent )
{

	// If 'q' or 'Enter' is hit, quit :
	
	if ( keyboardEvent.type == EventsModule::KeyPressed )
	{
	
		KeyboardHandler::KeyIdentifier pressedKey = 
			static_cast<KeyboardHandler::KeyIdentifier>(
			keyboardEvent.keysym.sym ) ;
			
		if ( pressedKey == KeyboardHandler::qKey 
				|| pressedKey == KeyboardHandler::EnterKey
				|| pressedKey == KeyboardHandler::EnterKeypadKey )
			OSDL::getExistingCommonModule().getEventsModule().requestQuit() ;
			
	}

}


int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Testing OSDL events basic services." ) ;

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
				// Ignored :
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
					"Unexpected command line argument : " + token ) ;
			}
		
		}
		

		LogPlug::info( "Starting OSDL with keyboard  support." ) ;	
				
		// Will trigger the video module as well for events :	
        OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseKeyboard ) ;		
			
		LogPlug::info( "Testing basic event handling." ) ;
		
		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ; 

		LogPlug::info( EventsModule::DescribeEnvironmentVariables() ) ;
				
		LogPlug::info( "Displaying a dummy window "
			"to have access to an event queue." ) ;
			
		LogPlug::info( "Getting video." ) ;
		OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		// A window is needed to have the SDL event system working :
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
		
		if ( ! isBatch )
		{
		
			LogPlug::info( "Entering the event loop for key press waiting." ) ;
			
			// Adds the relevant message in standard output :
			myEvents.waitForAnyKey() ;
			
		}
		
		
		LogPlug::info( 
			"Registering a new keyboard key handler converter, for 'q' key." ) ;
			
		myEvents.getKeyboardHandler().setDefaultRawKeyHandler( 
			exampleKeyHandler ) ;
			
			
		if ( isBatch )
		{
			LogPlug::warning( "Main loop not launched, as in batch mode." ) ;
		}
		else
		{

			LogPlug::info( 
				"Entering the event loop for keyboard event waiting " ) ;

			std::cout << "< Press Enter or 'q' key "
				"to end event-driven MVC test >" << std::endl ;
		
			myEvents.enterMainLoop() ;
			LogPlug::info( "Exiting main loop." ) ;	
				
		}

		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

   }
	
    catch ( const OSDL::Exception & e )
    {
        LogPlug::error( "OSDL exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const Ceylan::Exception & e )
    {
        LogPlug::error( "Ceylan exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
        LogPlug::error( "Standard exception caught : " 
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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Events ;
using namespace OSDL::Video ;

using namespace Ceylan::Log ;



#include <iostream>  // for cout, endl





/**
 * Testing OSDL mouse handling.
 *
 * @note Useful also to re-set a visible cursor after a SDL program hid it
 * and then crashed.
 *
 */
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Testing OSDL mouse services." ) ;


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
		
		
		LogPlug::info( "Starting OSDL with mouse support." ) ;		
        OSDL::CommonModule & myOSDL = OSDL::getCommonModule(
			CommonModule::UseMouse ) ;		
		
		LogPlug::info( "Testing basic event handling." ) ;
		
		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ; 
		
		LogPlug::info( "Events module: " + myEvents.toString() ) ;
		
		MouseHandler & myMouseHandler = myEvents.getMouseHandler() ;
		
		LogPlug::info( "Current mouse handler is: " 
			+ myMouseHandler.toString( Ceylan::high ) ) ;

			
		if ( myMouseHandler.hasDefaultMouse() )	
		{
		
			LogPlug::info( "A default mouse is available." ) ;
			
		}
		else
		{
		
			LogPlug::info( "No mouse to test, stopping test." ) ;
			OSDL::stop() ;
			return Ceylan::ExitSuccess ;		
		
		}
		
		LogPlug::info( "New mouse handler state is: " 
			+ myMouseHandler.toString( Ceylan::high ) ) ;
		
			
		LogPlug::info( "Displaying a dummy window "
			"to have access to an event queue." ) ;
			
		LogPlug::info( "Getting video." ) ;
		OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ; 

		// A window is needed to have the event system working:

		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;

			
		/*
		 * Useful to re-set a visible cursor after a SDL program hid it
		 * and then crashed...
		 *
		 */
		Mouse & mouse = myMouseHandler.getDefaultMouse() ;
		
		LogPlug::info( "Using the first mouse." ) ;
		
		// Does its best to untangled the mouse:
		mouse.setCursorVisibility( true ) ;
		
		myEvents.setGrabInputMode( true ) ;
		myEvents.setGrabInputMode( false ) ;
		
		mouse.setCursorVisibility( true ) ;


		LogPlug::info( "End of mouse test." ) ;
		
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


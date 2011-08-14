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


// We must know whether we can use CEGUI to define our actual test CEGUI:
#include "OSDLConfig.h"

#ifdef OSDL_USES_CEGUI

#include "CEGUI.h"
using namespace CEGUI ;

#endif // OSDL_USES_CEGUI



/*
 * Test of OSDL GUI support.
 *
 */

/*
class MyGUIView : public OSDL::Rendering::BaseGUIView
{

	public:


		MyGUIView()
		{

			// Here is the place where we are to define the GUI elements:

			AG_BindGlobalKey(

			  //static_cast<SDLKey>( KeyboardHandler::EscapeKey ),
			  AG_KEY_ESCAPE,

			  // static_cast<SDLMod>( KeyboardHandler::NoneModifier ),
			  AG_KEYMOD_ANY,

			  Engine::Scheduler::StopExistingScheduler ) ;


			AG_BindGlobalKey(

			  //static_cast<SDLKey>( KeyboardHandler::F8Key ),
			  AG_KEY_F8,

			  //static_cast<SDLMod>( KeyboardHandler::NoneModifier ),
			  AG_KEYMOD_ANY,

			  AG_ViewCapture ) ;

			MultiLineExample( "Multiline Example (no word wrapping)", 0 ) ;

			MultiLineExample( "Multiline Example (with word wrapping)", 1 ) ;

			SingleLineExample() ;

		}


		virtual ~MyGUIView() throw()
		{

		}

		// The render() method can be inherited as is.

} ;

*/


void create_gui( Video::Surface & screen )
{

#ifdef OSDL_USES_CEGUI

  LogPlug::info( "Creating the GUI." ) ;

  WindowManager & winManager = WindowManager::getSingleton() ;

  DefaultWindow & rootWin = * static_cast<DefaultWindow*>(
	winManager.createWindow( "DefaultWindow", "Root" ) ) ;

  System::getSingleton().setGUISheet( &rootWin ) ;

  FrameWindow & myWin = * static_cast<FrameWindow*>( winManager.createWindow(
	  "TaharezLook/FrameWindow", "Demo Window" ) ) ;

  rootWin.addChildWindow( &myWin ) ;

  myWin.setPosition( UVector2( cegui_reldim(0.25f), cegui_reldim(0.25f) ) ) ;
  myWin.setSize( UVector2( cegui_reldim(0.5f), cegui_reldim(0.5f) ) ) ;

  myWin.setMaxSize( UVector2( cegui_reldim(1.0f), cegui_reldim(1.0f) ) ) ;
  myWin.setMinSize( UVector2( cegui_reldim(0.1f), cegui_reldim(0.1f) ) ) ;

  myWin.setText( "Hello World! This is a minimal SDL+OpenGL+CEGUI test." ) ;

#else // OSDL_USES_CEGUI

  throw( OSDL::TestException( "No CEGUI support available." ) ) ;

#endif // OSDL_USES_CEGUI

}



/**
 * Testing the services related to GUI (Graphical User Interface) of the OSDL
 * basic module (this is tested in basic, as it is transverse to multiple
 * modules, like video, audio and events).
 *
 */
int main( int argc, char * argv[] )
{


	LogHolder myLog( argc, argv ) ;


	try
	{


		LogPlug::info( "Testing OSDL GUI services." ) ;


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


		LogPlug::info( "Starting OSDL with GUI enabled." ) ;

		// Implies UseVideo, UseAudio and UseEvents:
		CommonModule & myOSDL = getCommonModule( CommonModule::UseGUI ) ;

		myOSDL.logState() ;


		LogPlug::info( "Testing real video (displayable)." ) ;

		LogPlug::info( "Getting video module." ) ;
		VideoModule & myVideo = myOSDL.getVideoModule() ;

		myVideo.logState() ;

		LogPlug::info( "Displaying available video definitions: "
			+ VideoModule::DescribeAvailableDefinitions(
					Surface::FullScreen | Surface::Hardware ) ) ;

		LogPlug::info( "Displaying configuration informations, "
			"including best available pixel format: "
			+ VideoModule::DescribeVideoCapabilities() ) ;

		LogPlug::info( "Displaying video driver name: "
			+ myVideo.getDriverName() + "." ) ;


		// Going from potential to real:

		LogPlug::info( "Entering visual tests: initializing the screen." ) ;

		Length screenWidth  = 640 ;
		Length screenHeight = 480 ;

		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface,
			OpenGL::OpenGLFor2D ) ;

		LogPlug::info( "Displaying now new current video informations. "
			+ VideoModule::DescribeVideoCapabilities() ) ;


		OSDL::Events::EventsModule & myEvents = myOSDL.getEventsModule() ;
		/*
		myEvents.useScheduler() ;

		// Subscribes to the scheduler, which takes ownership of it:
		Rendering::StandardRenderer * renderer =
			new Rendering::StandardRenderer( myVideo.getScreenSurface() ) ;

		MyGUIView * myGUIView = new MyGUIView() ;

		// Ownership not taken:
		renderer->registerView( *myGUIView ) ;
		*/

		create_gui( myVideo.getScreenSurface() ) ;

		if ( ! isBatch )
			myEvents.enterMainLoop() ;

		LogPlug::info( "Stopping OSDL." ) ;
		OSDL::stop() ;

		//delete myGUIView ;

		LogPlug::info( "End of OSDL GUI test." ) ;

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

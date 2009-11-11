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
using namespace OSDL::Video ;
using namespace OSDL::Events ;


using namespace Ceylan::Log ;

#include <agar/config/have_opengl.h>
 
#include <agar/core.h>
#include <agar/gui.h>
#include <agar/gui/opengl.h>


/*
 * Test of OSDL GUI support, derived from Agar's demo in 
 * 'demos/textbox/textbox.c'.
 *
 */

static void SetDisable(AG_Event *event)
{

	AG_Textbox *textbox = (AG_Textbox *) AG_PTR(1);
	int flag = AG_INT(2);

	if (flag) {
		AG_WidgetDisable(textbox);
	} else {
		AG_WidgetEnable(textbox);
	}

}



static void SingleLineExample(void)
{
	AG_Window *win;
	AG_Textbox *textbox;

	win = AG_WindowNew(0);
	AG_WindowSetCaption(win, "Single-line Example");

	/*
	 * Create a single-line Textbox. TextboxSizeHint() requests an initial
	 * textbox size large enough to display the given string entirely.
	 */
	textbox = AG_TextboxNew(win, 0, "Static string: ");
	AG_TextboxSizeHint(textbox, "XXXXXXXXXXX");
	AG_TextboxPrintf(textbox, "Hello");
	AG_WidgetFocus(textbox);

	/* Bind checkboxes to some flags. */
	AG_SeparatorNewHoriz(win);
	AG_CheckboxNewFn(win, 0, "Disable input", SetDisable, "%p", textbox);
	AG_CheckboxNewFlag(win, 0, "Password input",
	    &textbox->ed->flags, AG_EDITABLE_PASSWORD);
	AG_CheckboxNewFlag(win, 0, "Force integer input",
	    &textbox->ed->flags, AG_EDITABLE_INT_ONLY);
	AG_CheckboxNewFlag(win, 0, "Force float input",
	    &textbox->ed->flags, AG_EDITABLE_FLT_ONLY);
	AG_CheckboxNewFlag(win, 0, "Disable emacs keys",
	    &textbox->ed->flags, AG_EDITABLE_NOEMACS);
	AG_CheckboxNewFlag(win, 0, "Disable traditional LATIN-1",
	    &textbox->ed->flags, AG_EDITABLE_NOLATIN1);

	AG_WindowSetPosition(win, AG_WINDOW_MIDDLE_LEFT, 0);
	AG_WindowShow(win);
}


static void MultiLineExample(const char *title, int wordwrap)
{

	AG_Window *win;
	AG_Textbox *textbox;
	char *someText;
	FILE *f;
	size_t size, bufSize;
	unsigned int flags = AG_TEXTBOX_MULTILINE|AG_TEXTBOX_CATCH_TAB;

	if (wordwrap) { flags |= AG_TEXTBOX_WORDWRAP; }

	win = AG_WindowNew(0);

	AG_WindowSetCaption(win, "My OSDL Example" );

	/*
	 * Create a multiline textbox.
	 *
	 * We use the CATCH_TAB flag so that tabs are entered literally in
	 * the string.
	 */
	textbox = AG_TextboxNew(win, flags, NULL);
	AG_Expand(textbox);

	/*
	 * Load the contents of this file into a buffer. Make the buffer a
	 * bit larger so the user can try entering text.
	 */
	if ((f = fopen( "testOSDLGUI.cc", "r")) != NULL) {
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		fseek(f, 0, SEEK_SET);
		bufSize = size+1024;
		someText = (char*)AG_Malloc(bufSize);
		fread(someText, size, 1, f);
		fclose(f);
		someText[size] = '\0';
	} else {
		someText = AG_Strdup("(Unable to open testOSDLGUI.cc)");
	}

	/*
	 * Bind the buffer's contents to the Textbox. The size argument to
	 * AG_TextboxBindUTF8() must include space for the terminating NUL.
	 */
	AG_TextboxBindUTF8(textbox, someText, bufSize);
	AG_TextboxSetCursorPos(textbox, 0);

	AG_CheckboxNewFn(win, 0, "Disable input", SetDisable, "%p", textbox);
#if 0
	AG_SeparatorNewHoriz(win);
	{
		AG_Label *lbl;

		lbl = AG_LabelNewPolled(win, 0, "Lines: %d",
		    &textbox->ed->yMax);
		AG_ExpandHoriz(lbl);

		lbl = AG_LabelNewPolled(win, 0, "Cursor position: %d",
		    &textbox->ed->pos);
		AG_ExpandHoriz(lbl);
	}
#endif
	AG_WindowSetGeometryAligned(win, AG_WINDOW_MC, 540, 380);
	AG_WindowShow(win);
}



class MyGUIView : public OSDL::Rendering::BaseGUIView
{

	public:
	
	
		MyGUIView() 
		{
	
			// Here is the place where we are to define the GUI elements:
			
			AG_BindGlobalKey(
				static_cast<SDLKey>( KeyboardHandler::EscapeKey ),
				static_cast<SDLMod>( KeyboardHandler::NoneModifier ), 
				Engine::Scheduler::StopExistingScheduler ) ;
				
			AG_BindGlobalKey( 
				static_cast<SDLKey>( KeyboardHandler::F8Key ),
				static_cast<SDLMod>( KeyboardHandler::NoneModifier ), 
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

			
		LogPlug::info( "Starting OSDL with video enabled." )	;
			
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
		
		myEvents.useScheduler() ;
		
		// Subscribes to the scheduler, which takes ownership of it:
		Rendering::StandardRenderer * renderer = 
			new Rendering::StandardRenderer( myVideo.getScreenSurface() ) ;
		
		MyGUIView * myGUIView = new MyGUIView() ;
		
		// Ownership not taken:
		renderer->registerView( *myGUIView ) ;
		
		if ( ! isBatch )
			myEvents.enterMainLoop() ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		delete myGUIView ;
		
		LogPlug::info( "End of OSDL video test." ) ;
		
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


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



/*
 * Implementation notes
 *
 * Currently, despite some tiny efforts to build a OSDL custom one, the GUI is
 * supported thanks to the use of only one back-end, CEGUI (see
 * http://www.cegui.org.uk).
 *
 * We were using previously Guichan and Agar (see http://libagar.org/), but were
 * not satisfied of them.
 *
 * Here are the parts of the GUI which are generic: aspects dealing more
 * precisely with video, audio, events, etc. are defined in the respective
 * modules.
 *
 * A GUI, in the course of an execution, may be enabled and disabled multiple
 * times.
 *
 */


#include "OSDLGUI.h"



#include <list>
using std::list ;

using std::string ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>              // for OSDL_USES_{CEGUI,AGAR} and al
#endif // OSDL_USES_CONFIG_H




#if OSDL_USES_AGAR


// Defines the most ancient version of libagar that is supported by OSDL:

const Ceylan::Version::VersionNumber oldestAgarSupportedMajor = 1 ;
const Ceylan::Version::VersionNumber oldestAgarSupportedMinor = 4 ;
const Ceylan::Version::VersionNumber oldestAgarSupportedPatch = 0 ;


#include <agar/config/have_opengl.h>

#include <agar/core.h>
#include <agar/gui.h>



/**
 * Class names, as managed by the event system, to parse AG_Event instances.
 *
 */

const std::string & TextBoxClassName   = "AG_Widget:AG_Textbox" ;
const std::string & WindowClassName    = "AG_Widget:AG_Window" ;



#endif // OSDL_USES_AGAR




#if OSDL_USES_CEGUI

/*
 * Defines the most ancient version of the CEGUI library that is supported by
 * OSDL:
 *
 */

const Ceylan::Version::VersionNumber oldestCEGUISupportedMajor = 0 ;
const Ceylan::Version::VersionNumber oldestCEGUISupportedMinor = 7 ;
const Ceylan::Version::VersionNumber oldestCEGUISupportedPatch = 5 ;

#include "CEGUI.h"
#include "RendererModules/OpenGL/CEGUIOpenGLRenderer.h"


/**
 * Base CEGUI-defined look.
 *
 */
const std::string & BaseLook = "TaharezLook" ;


#endif // OSDL_USES_CEGUI




using namespace Ceylan ;
using namespace Ceylan::Log ;


#include "OSDLBasic.h"      // for OSDL::GetVersion
#include "OSDLFont.h"       // for PointSize

#include "OSDLVideo.h"      // for VideModule
#include "OSDLAudio.h"      // for AudioModule
#include "OSDLEvents.h"     // for Eventsodule


using namespace OSDL ;
using namespace OSDL::Video::TwoDimensional::Text ;


// Replicating these defines allows to enable them on a per-class basis:
#if OSDL_VERBOSE_GUI_MODULE

#define LOG_DEBUG_GUI(message)   LogPlug::debug(message)
#define LOG_TRACE_GUI(message)   LogPlug::trace(message)
#define LOG_WARNING_GUI(message) LogPlug::warning(message)

#else // OSDL_VERBOSE_GUI_MODULE

#define LOG_DEBUG_GUI(message)
#define LOG_TRACE_GUI(message)
#define LOG_WARNING_GUI(message)

#endif // OSDL_VERBOSE_GUI_MODULE




GUIException::GUIException( const std::string & reason ) :
	OSDL::Exception( reason )
{


}


GUIException::~GUIException() throw()
{

}




GUIModule::GUIModule( const std::string & applicationName,
  Video::VideoModule & videoModule, Audio::AudioModule & audioModule,
  Events::EventsModule & eventsModule ) :
	Ceylan::Module(
		"OSDL GUI module",
		"This is the GUI module of OSDL",
		"http://osdl.sourceforge.net",
		"Olivier Boudeville",
		"olivier.boudeville@online.fr",
		OSDL::GetVersion(),
		"disjunctive LGPL/GPL" ),
	_applicationName( applicationName ),
	_video(  &videoModule ),
	_audio(  &audioModule ),
	_events( &eventsModule )
{

  send( "Initializing GUI subsystem." ) ;


#if OSDL_ARCH_NINTENDO_DS

	throw GUIException( "GUIModule constructor failed: "
		"not supported on the Nintendo DS." ) ;

#endif // OSDL_ARCH_NINTENDO_DS



#ifndef OSDL_USES_AGAR

#ifndef OSDL_USES_CEGUI

#error "No support available for any integrated GUI back-end."

	throw GUIException( "GUIModule constructor failed: "
		"no support available for any integrated GUI back-end." ) ;

#endif // OSDL_USES_CEGUI

#endif //OSDL_USES_CEGUI

#endif //OSDL_USES_AGAR



#if OSDL_USES_AGAR

	AG_AgarVersion linkTimeAgarVersion ;

	AG_GetVersion( &linkTimeAgarVersion ) ;

	send( "Using Agar backend for the GUI, linked against the "
		+ Ceylan::toNumericalString( linkTimeAgarVersion.major ) + "."
		+ Ceylan::toNumericalString( linkTimeAgarVersion.minor ) + "."
		+ Ceylan::toNumericalString( linkTimeAgarVersion.patch )
		+ ", release codenamed '"
		+ string( linkTimeAgarVersion.release ) + "'." ) ;

	// Implies a call to SDL_Init with SDL_INIT_TIMER and SDL_INIT_NOPARACHUTE:

#if OSDL_DEBUG_GUI

	int res = AG_InitCore( _applicationName.c_str(),
	  AG_VERBOSE | AG_NO_CFG_AUTOLOAD ) ;

#else // OSDL_DEBUG_GUI

	int res = AG_InitCore( _applicationName.c_str(), AG_NO_CFG_AUTOLOAD ) ;

#endif // OSDL_DEBUG_GUI

	if ( res == -1 )
		throw OSDL::GUIException( "GUIModule constructor failed: "
		  "initialization of the Agar core library failed: "
		  + GetBackendLastError() ) ;

	if ( ! AG_VERSION_ATLEAST( oldestAgarSupportedMajor,
			oldestAgarSupportedMinor, oldestAgarSupportedPatch ) )
		throw OSDL::GUIException( "The version of this Agar library in use "
			"is too old to be supported, needing at least "
			+ Ceylan::toNumericalString( oldestAgarSupportedMajor ) + "."
			+ Ceylan::toNumericalString( oldestAgarSupportedMinor ) + "."
			+ Ceylan::toNumericalString( oldestAgarSupportedPatch ) + "." );

	AG_SetString( agConfig, "font-path", "." ) ;

#endif // OSDL_USES_AGAR


#if OSDL_USES_CEGUI

	// Add compile-time / link-time version checking.

	send( "Using CEGUI backend for the GUI." ) ;

	/*
	 * See our CEGUI-SDL-all-widgets.cc example file to understand how SDL,
	 * OpenGL and CEGUI are to be used together.
	 *
	 */


	// First, let's set the preferred SDL settings for CEGUI:

	// Corresponds to a mere 'SDL_ShowCursor( SDL_DISABLE ) ;':

	if ( ! eventsModule.hasMouseHandler() )
	  throw( OSDL::GUIException( "A mouse handler is needed for the GUI." ) ) ;

	MouseHandler & mouseHandler = eventsModule.getMouseHandler() ;

	if ( ! mouseHandler.hasDefaultMouse() )
	  throw( OSDL::GUIException( "A default mouse is needed for the GUI." ) ) ;

	mouseHandler.getDefaultMouse().setCursorVisibility( false ) ;


	/*
	 * Corresponds to:
	 *
	 * SDL_EnableUNICODE( 1 ) ;
	 * SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY,
	 *   DL_DEFAULT_REPEAT_INTERVAL ) ;
	 *
	 */

	if ( ! eventsModule.hasKeyboardHandler() )
	  throw( OSDL::GUIException(
		  "A keyboard handler is needed for the GUI." ) ) ;

	KeyboardHandler::SetMode( KeyboardHandler::textInput ) ;


	// Then initialize CEGUI by itself:

	CEGUI::OpenGLRenderer::bootstrapSystem() ;

	// Initialises the required directories for the DefaultResourceProvider:

	CEGUI::DefaultResourceProvider & defaultResProvider =
		* static_cast<CEGUI::DefaultResourceProvider*>(
		  CEGUI::System::getSingleton().getResourceProvider() ) ;

	// CEGUI_INSTALL_PATH defined by OSDL's configure:
#ifdef CEGUI_INSTALL_PATH

	const string CEGUIInstallSharePath = CEGUI_INSTALL_PATH + "/share/CEGUI/" ;

	if ( ! Ceylan::System::Directory::Exists( CEGUIInstallSharePath ) )
	  throw( OSDL::GUIException( "GUIModule constructor failed: "
		  "CEGUI install shared directory ('" + CEGUIInstallSharePath
		  + "') does not exist." ) ) ;

	// For each resource type, sets a corresponding resource group directory:

	send( "Using CEGUI shared install directory '" + CEGUIInstallSharePath
	  + "'." ) ;

	defaultResProvider.setResourceGroupDirectory( "schemes",
	  CEGUIInstallSharePath + "schemes/" ) ;

	defaultResProvider.setResourceGroupDirectory( "imagesets",
	  CEGUIInstallSharePath + "imagesets/" ) ;

	defaultResProvider.setResourceGroupDirectory( "fonts",
	  CEGUIInstallSharePath + "fonts/" ) ;

	defaultResProvider.setResourceGroupDirectory( "layouts",
	  CEGUIInstallSharePath + "layouts/" ) ;

	defaultResProvider.setResourceGroupDirectory( "looknfeels",
	  CEGUIInstallSharePath + "looknfeel/" ) ;

	defaultResProvider.setResourceGroupDirectory( "lua_scripts",
	  CEGUIInstallSharePath + "lua_scripts/" ) ;

	defaultResProvider.setResourceGroupDirectory( "schemas",
	  CEGUIInstallSharePath + "xml_schemas/" ) ;

	defaultResProvider.setResourceGroupDirectory( "animations",
	  CEGUIInstallSharePath + "animations/" ) ;

	// Sets the default resource groups to be used:
	CEGUI::Imageset::setDefaultResourceGroup( "imagesets" ) ;
	CEGUI::Font::setDefaultResourceGroup( "fonts" ) ;
	CEGUI::Scheme::setDefaultResourceGroup( "schemes" ) ;
	CEGUI::WidgetLookManager::setDefaultResourceGroup( "looknfeels" ) ;
	CEGUI::WindowManager::setDefaultResourceGroup( "layouts" ) ;
	CEGUI::ScriptModule::setDefaultResourceGroup( "lua_scripts" ) ;
	CEGUI::AnimationManager::setDefaultResourceGroup( "animations" ) ;

	// Set-up default group for validation schemas:
	CEGUI::XMLParser * parser = CEGUI::System::getSingleton().getXMLParser() ;
	if ( parser->isPropertyPresent( "SchemaDefaultResourceGroup" ) )
	  parser->setProperty( "SchemaDefaultResourceGroup", "schemas" ) ;

	CEGUI::SchemeManager::getSingleton().create( BaseLook + ".scheme" ) ;

	// Enforces some defaults:

	System::getSingleton().setDefaultMouseCursor( BaseLook, "MouseArrow" ) ;

	FontManager::getSingleton().create( "DejaVuSans-10.font" ) ;

#endif // OSDL_USES_CEGUI



	/*
	 * Not doable, as common module not constructed/registered yet:

	CommonModule & common = OSDL::getExistingCommonModule() ;

	Video::VideoModule & video    = common.getVideoModule() ;
	Audio::AudioModule & audio    = common.getAudioModule() ;
	Events::EventsModule & events = common.getEventsModule() ;

	 *
	 */


	_video->setGUIEnableStatus(  true ) ;
	_audio->setGUIEnableStatus(  true ) ;
	_events->setGUIEnableStatus( true ) ;

	send( "GUI subsystem initialized." ) ;

	dropIdentifier() ;

}



GUIModule::~GUIModule() throw()
{

	send( "Stopping GUI subsystem." ) ;

	_events->setGUIEnableStatus( false ) ;
	_audio->setGUIEnableStatus(  false ) ;
	_video->setGUIEnableStatus(  false ) ;

#if OSDL_USES_AGAR

	// Not AG_Quit, which is AG_Destroy + exit(0):
	AG_Destroy() ;

#endif // OSDL_USES_AGAR

	send( "GUI subsystem stopped." ) ;

}



const string GUIModule::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "GUI module" ;

	if ( level == Ceylan::low )
		return res ;

	return res ;

}






// Static section.


// FIRST LOAD FROM BASIC FILE THEN FROM ABSTRACT CEYLAN FILE

GUILevelSurface & GUIModule::LoadPNG( const std::string & pngFilename )
{

#if OSDL_USES_AGAR

  AG_Surface * surface = AG_SurfaceFromPNG( pngFilename.c_str() ) ;

  if ( surface == 0 )
	throw GUIException( "GUIModule::LoadPNG failed for '"
	  + pngFilename + "': " + GetBackendLastError() ) ;

  return * surface ;

#else // OSDL_USES_AGAR

  throw GUIException( "GUIModule::LoadPNG failed: "
					"no Agar support available." ) ;

#endif // OSDL_USES_AGAR

}



GUILevelSurface & GUIModule::LoadJPEG( const std::string & jpegFilename )
{

#if OSDL_USES_AGAR

  AG_Surface * surface = AG_SurfaceFromJPEG( jpegFilename.c_str() ) ;

  if ( surface == 0 )
	throw GUIException( "GUIModule::LoadJPEG failed for '"
	  + jpegFilename + "': " + GetBackendLastError() ) ;

  return * surface ;

#else // OSDL_USES_AGAR

  throw GUIException( "GUIModule::LoadJPEG failed: "
					"no Agar support available." ) ;

#endif // OSDL_USES_AGAR

}



GUILevelFont & GUIModule::LoadFont( const std::string & fontFilename,
		PointSize pointSize )
{

#if OSDL_USES_AGAR

  AG_Font * font = AG_FetchFont( fontFilename.c_str(), pointSize,
	/* flags */ -1 ) ;

  if ( font == 0 )
	throw GUIException( "GUIModule::LoadFont failed for '"
	  + fontFilename + "': " + GetBackendLastError() ) ;

  //Ceylan::Byte & content = File::ReadWholeContent( fontFilename ) ;

  //AG_Font * font = AF_FontFromMemory( AG_TTFOpenFontFromMemory

  return * font ;

#else // OSDL_USES_AGAR

  throw GUIException( "GUIModule::LoadFont failed: "
					"no Agar support available." ) ;

#endif // OSDL_USES_AGAR

}



std::string GUIModule::GetBackendLastError()
{

#if OSDL_USES_AGAR

  return std::string( AG_GetError() ) ;

#else // OSDL_USES_AGAR

  throw GUIException( "GUIModule::GetBackendLastError failed: "
					"no Agar support available." ) ;

#endif // OSDL_USES_AGAR

}

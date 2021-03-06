/*
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#include "OSDLKeyboardHandler.h"

#include "OSDLController.h"      // for Controller
#include "OSDLEvents.h"          // for KeyPressed, EventsException
#include "OSDLBasic.h"           // for getExistingCommonModule


using std::string ;
using std::list ;
using std::map ;


using namespace Ceylan::Log ;    // for LogPlug
using namespace OSDL::Events ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>          // for OSDL_VERBOSE_KEYBOARD_HANDLER and al
#endif // OSDL_USES_CONFIG_H



#if OSDL_VERBOSE_KEYBOARD_HANDLER

#include <iostream>
#define OSDL_KEYBOARD_HANDLER_LOG( message ) std::cout << "[OSDL Keyboard Handler] " << message << std::endl ;

#else // OSDL_VERBOSE_KEYBOARD_HANDLER

#define OSDL_KEYBOARD_HANDLER_LOG( message )

#endif // OSDL_VERBOSE_KEYBOARD_HANDLER



#if OSDL_USES_SDL

const Ceylan::System::Millisecond KeyboardHandler::DefaultDelayBeforeKeyRepeat
	= SDL_DEFAULT_REPEAT_DELAY ;

const Ceylan::System::Millisecond KeyboardHandler::DefaulKeyRepeatInterval
	= SDL_DEFAULT_REPEAT_INTERVAL ;

#else // OSDL_USES_SDL

// Same values as SDL:
const Ceylan::System::Millisecond KeyboardHandler::DefaultDelayBeforeKeyRepeat
	= 500 ;

const Ceylan::System::Millisecond KeyboardHandler::DefaulKeyRepeatInterval
	= 30 ;

#endif // OSDL_USES_SDL




// Starts with raw input mode:
KeyboardMode KeyboardHandler::_CurrentMode = rawInput ;




KeyboardException::KeyboardException( const string & message ):
	EventsException( message )
{

}



KeyboardException::~KeyboardException() throw()
{

}





/**
 * The default key to handler mapper can reference this general-purpose key
 * handler.
 *
 * Can be used both for raw and input modes.
 *
 */
void doNothingKeyHandler( const KeyboardEvent & keyboardEvent )
{

	OSDL_KEYBOARD_HANDLER_LOG( EventsModule::DescribeEvent( keyboardEvent ) ) ;

}




/**
 * The default key to handler mapper can reference this general-purpose key
 * handler.
 *
 * Can be used both for raw and input modes.
 *
 * Respects following behaviour:
 *   - if 'q' key or escape is pressed, exits from the main loop
 *   - if F1 key is pressed, toggle between raw input and text input modes.
 *
 */
void smarterKeyHandler( const KeyboardEvent & keyboardEvent )
{

#if OSDL_USES_SDL

	OSDL_KEYBOARD_HANDLER_LOG(
		EventsModule::DescribeEvent( keyboardEvent ) ) ;

	// Reacts to key presses for both input modes (raw or text):

	if ( keyboardEvent.type != EventsModule::KeyPressed )
	{

		// Only key presses interest us.
		return ;

	}


	switch( static_cast<KeyboardHandler::KeyIdentifier>(
		keyboardEvent.keysym.sym ) )
	{

		case KeyboardHandler::EscapeKey:
		case KeyboardHandler::qKey:
			LogPlug::debug( "Input keypress requested exit." ) ;
			EventsModule::DescribeEvent( keyboardEvent ) ;
			OSDL::getExistingCommonModule().getEventsModule().requestQuit() ;
			break ;

		case KeyboardHandler::F1Key:
			// F1: toggle text/raw input mode:
			EventsModule::DescribeEvent( keyboardEvent ) ;
			if ( KeyboardHandler::GetMode() == rawInput )
				KeyboardHandler::SetMode( textInput ) ;
			else
				KeyboardHandler::SetMode( rawInput ) ;
			break ;

		/*

		case KeyboardHandler::F2Key:
			// F2: raw input mode:
			EventsModule::DescribeEvent( keyboardEvent ) ;
			KeyboardHandler::SetMode( rawInput ) ;
			break ;

		case KeyboardHandler::F3Key:
			// F3: text input mode:
			EventsModule::DescribeEvent( keyboardEvent ) ;
			KeyboardHandler::SetMode( textInput ) ;
			break ;

		*/

		default:
			EventsModule::DescribeEvent( keyboardEvent ) ;
			break ;
	}

#endif // OSDL_USES_SDL

}



KeyboardHandler::KeyboardHandler( KeyboardMode initialMode,
		bool useSmarterDefaultKeyHandler ) :
	InputDeviceHandler(),
	_rawKeyControllerMap(),
	_rawKeyHandlerMap(),
	_unicodeControllerMap(),
	_unicodeHandlerMap(),
	_defaultRawKeyController( 0 ),
	_focusController( 0 )
{

#if OSDL_USES_SDL

	send( "Initializing keyboard subsystem." ) ;

	// No SDL_InitSubSystem for keyboard.

	// Stores the previous Unicode mode:
	_unicodeInputWasActivated = ( SDL_EnableUNICODE( -1 ) == 1 ) ;

	if ( useSmarterDefaultKeyHandler )
	{

		setSmarterDefaultKeyHandlers() ;

	}
	else
	{

		_defaultRawKeyHandler  = doNothingKeyHandler ;
		_defaultUnicodeHandler = doNothingKeyHandler ;

	}

	SetMode( initialMode ) ;

	send( "Keyboard subsystem initialized." ) ;

	dropIdentifier() ;

#else // OSDL_USES_SDL

	throw InputDeviceHandlerException( "KeyboardHandler constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



KeyboardHandler::~KeyboardHandler() throw()
{

#if OSDL_USES_SDL

	send( "Stopping keyboard subsystem." ) ;

	if ( _defaultRawKeyController != 0 )
		delete _defaultRawKeyController ;

	// Avoid side-effects:
	if ( _unicodeInputWasActivated )
		SDL_EnableUNICODE( 1 ) ;
	else
		SDL_EnableUNICODE( 0 ) ;

	// No SDL_QuitSubSystem for keyboard.

	send( "Keyboard subsystem stopped." ) ;

#endif // OSDL_USES_SDL

}



void KeyboardHandler::linkToController( KeyIdentifier rawKey,
	OSDL::MVC::Controller & controller )
{

	_rawKeyControllerMap[ rawKey ] = & controller ;

}



void KeyboardHandler::unlinkFromController( KeyIdentifier rawKey,
	OSDL::MVC::Controller & controller )
{

	map<KeyIdentifier, OSDL::MVC::Controller*>::iterator itController
		= _rawKeyControllerMap.find( rawKey ) ;

	if ( itController != _rawKeyControllerMap.end() )
	{

		if ( (*itController).second != & controller )
			throw InputDeviceHandlerException(
				"KeyboardHandler::unlinkFromController failed: "
				"current controller was not the specified one." ) ;

		_rawKeyControllerMap.erase( itController ) ;

		OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::unlinkFromController: "
				"controller unlinked." ) ;

	}
	else
	{

		throw InputDeviceHandlerException(
			"KeyboardHandler::unlinkFromController failed: "
			"key binding not found." ) ;

	}

}



void KeyboardHandler::linkToController( Ceylan::Unicode unicode,
	OSDL::MVC::Controller & controller )
{

	_unicodeControllerMap[ unicode ] = & controller ;

}



void KeyboardHandler::linkToFocusController(
	OSDL::MVC::Controller & controller )
{

	_focusController = & controller ;

}



void KeyboardHandler::linkToHandler( KeyIdentifier rawKey,
	KeyboardEventHandler handler )
{

	_rawKeyHandlerMap[ rawKey ] = handler ;

}



void KeyboardHandler::linkToHandler( Ceylan::Unicode unicode,
	KeyboardEventHandler handler )
{

	_unicodeHandlerMap[ unicode ] = handler ;

}



void KeyboardHandler::setSmarterDefaultKeyHandlers()
{

	_defaultRawKeyHandler  = smarterKeyHandler ;
	_defaultUnicodeHandler = smarterKeyHandler ;

}




void KeyboardHandler::linkDefaultRawKeyController(
	OSDL::MVC::Controller & newDefaultController )
{

	_defaultRawKeyController = & newDefaultController ;

}



bool KeyboardHandler::unlinkDefaultRawKeyController()
{

	if ( _defaultRawKeyController != 0 )
	{

		_defaultRawKeyController = 0 ;
		return true ;

	}

	return false ;

}




void KeyboardHandler::setDefaultRawKeyHandler(
	KeyboardEventHandler newHandler )
{

	_defaultRawKeyHandler = newHandler ;

}



void KeyboardHandler::setDefaultUnicodeHandler(
	KeyboardEventHandler newHandler )
{

	_defaultUnicodeHandler = newHandler ;

}



const string KeyboardHandler::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "Keyboard handler in " ;

	switch( _CurrentMode )
	{

		case rawInput:
			res += "raw input" ;
			break ;

		case textInput:
			res += "text input" ;
			break ;

		default:
			res += "unknown (abnormal)" ;
			break ;
	}

	res += " mode" ;

	return res ;

}




KeyboardMode KeyboardHandler::GetMode()
{

	return _CurrentMode ;

}



void KeyboardHandler::SetMode( KeyboardMode newMode )
{

#if OSDL_USES_SDL

	OSDL_KEYBOARD_HANDLER_LOG( "Setting keyboard mode to "
		<< ( ( newMode == rawInput ) ?
			"raw input mode.": "text input mode." ) ) ;

	_CurrentMode = newMode ;

	switch( newMode )
	{

		case rawInput:
			SDL_EnableUNICODE( 0 ) ;
			break ;

		case textInput:
			SDL_EnableUNICODE( 1 ) ;
			SDL_EnableKeyRepeat( DefaultDelayBeforeKeyRepeat,
				DefaulKeyRepeatInterval ) ;
			break ;

		default:
			Ceylan::emergencyShutdown(
				"KeyboardHandler::SetMode: unknown keyboard mode: "
				+ Ceylan::toString( newMode ) + "." ) ;
			break ;

	}

#endif // OSDL_USES_SDL

}



string KeyboardHandler::DescribeKey( KeyIdentifier key )
{

#if OSDL_USES_SDL

	// @fixme add all remaining characters (see SDL_keysym.h)

	switch( key )
	{

		// Handle non printable characters specifically:
		case BackspaceKey:
			return "backspace" ;
			break ;

		case TabKey:
			return "tab" ;
			break ;

		case ClearKey:
			return "clear" ;
			break ;

		case EnterKey:
			return "enter" ;
			break ;

		case PauseKey:
			return "pause" ;
			break ;

		case EscapeKey:
			return "escape" ;
			break ;

		case SpaceKey:
			return "space" ;
			break ;

		case DeleteKey:
			return "delete" ;
			break ;

		default:
			// Handles other ASCII characters:
			if ( key < 256 )
				return Ceylan::toString( static_cast<char>( key ) )  ;
			else
				return "(unknown key: #" + Ceylan::toString( key ) + ")" ;
			break ;
	}

	return "(unexpected key)" ;

#else // OSDL_USES_SDL

	return "(no SDL support available)" ;

#endif // OSDL_USES_SDL

}



string KeyboardHandler::DescribeModifier( KeyModifier modifier )
{

#if OSDL_USES_SDL

	string res ;

	if ( modifier & LeftShiftModifier )
		res += " left-shift" ;

	if ( modifier & RightShiftModifier )
		res += " right-shift" ;

	if ( modifier & LeftControlModifier )
		res += " left-control" ;

	if ( modifier & RightControlModifier )
		res += " right-control" ;

	if ( modifier & LeftAltModifier )
		res += " left-alt" ;

	if ( modifier & RightAltModifier )
		res += " right-alt" ;

	if ( modifier & LeftMetaModifier )
		res += " left-meta" ;

	if ( modifier & RightMetaModifier )
		res += " right-meta" ;

	if ( modifier & NumModifier )
		res += " numlock" ;

	if ( modifier & CapsModifier )
		res += " capslock" ;

	if ( modifier & ModeModifier )
		res += " mode" ;

	if ( modifier & ReservedModifier )
		res += " reserved" ;

	if ( res.empty() )
		return "(none)" ;
	else
		return res ;

#else // OSDL_USES_SDL

	return "(no SDL support available)" ;

#endif // OSDL_USES_SDL

}



string KeyboardHandler::DescribeUnicode( Ceylan::Unicode value )
{

	if ( ( value & 0xff80 ) == 0 )
		return "Unicode character '"
			+ Ceylan::toString( static_cast<char>( value & 0x7F ) ) + "'" ;
	else
		return "International Unicode character" ;

}





// Protected section.



void KeyboardHandler::focusGained( const FocusEvent & keyboardFocusEvent )
{

	if ( _focusController != 0 )
		_focusController->keyboardFocusGained( keyboardFocusEvent ) ;

}



void KeyboardHandler::focusLost( const FocusEvent & keyboardFocusEvent )
{

	if ( _focusController != 0 )
		_focusController->keyboardFocusLost( keyboardFocusEvent ) ;

}



void KeyboardHandler::keyPressed( const KeyboardEvent & keyboardEvent )
{

#if OSDL_USES_SDL

	/*
	 * In raw or text mode, use a controller if available, otherwise an handler
	 * will be used, a specified one if any, else the default handler.
	 *
	 */

	if ( _CurrentMode == rawInput )
	{

		map<KeyIdentifier, OSDL::MVC::Controller*>::iterator itController
			= _rawKeyControllerMap.find(
				static_cast<KeyIdentifier>( keyboardEvent.keysym.sym ) ) ;

		if ( itController != _rawKeyControllerMap.end() )
		{

			OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyPressed: "
				"raw key sent to controller." ) ;

			(*itController).second->rawKeyPressed( keyboardEvent ) ;

		}
		else
		{

			// No controller registered, defaults to handler map:

			map<KeyIdentifier, KeyboardEventHandler>::iterator itHandler
				=  _rawKeyHandlerMap.find(
					static_cast<KeyIdentifier>( keyboardEvent.keysym.sym ) ) ;

			if ( itHandler != _rawKeyHandlerMap.end() )
			{

				OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyPressed "
					"sent to a raw key handler." ) ;

				(*itHandler).second( keyboardEvent ) ;

			}
			else
			{


				if ( _defaultRawKeyController != 0 )
				{

					OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyPressed "
						"sent to default raw key controller." ) ;

					_defaultRawKeyController->rawKeyPressed( keyboardEvent ) ;

				}
				else
				{

					OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyPressed "
						"sent to default raw key handler." ) ;

					_defaultRawKeyHandler( keyboardEvent ) ;

				}

			}


		}

		// Avoid handling the same key twice (ex: keyboard mode toggle...)
		return ;


	} // _CurrentMode == rawInput


	// Other possibility: in text input mode.

	if ( _CurrentMode == textInput )
	{

		map<Ceylan::Unicode, OSDL::MVC::Controller*>::iterator itController
			=  _unicodeControllerMap.find(
				static_cast<KeyIdentifier>( keyboardEvent.keysym.unicode ) ) ;

		if ( itController != _unicodeControllerMap.end() )
		{

			OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyPressed: "
				"Unicode sent to controller." ) ;

			(*itController).second->unicodeSelected( keyboardEvent ) ;

		}
		else
		{


			// No controller registered, default to handler map:

			map<Ceylan::Unicode, KeyboardEventHandler>::iterator itHandler
				=  _unicodeHandlerMap.find(
					static_cast<KeyIdentifier>(
						keyboardEvent.keysym.unicode ) ) ;

			if ( itHandler != _unicodeHandlerMap.end() )
			{

				OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyPressed "
					"sent to Unicode handler." ) ;

				(*itHandler).second( keyboardEvent ) ;

			}
			else
			{

				OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyPressed "
					"sent to default Unicode handler." ) ;

				_defaultUnicodeHandler( keyboardEvent ) ;

			}


		}


	} // _CurrentMode == textInput

#endif // OSDL_USES_SDL

}



void KeyboardHandler::keyReleased( const KeyboardEvent & keyboardEvent )
{

#if OSDL_USES_SDL

	/*
	 * In raw mode, use a controller if available, otherwise an handler will be
	 * used, a specified one if any, else the default handler.
	 *
	 */


	if ( _CurrentMode == rawInput )
	{

		map<KeyIdentifier, OSDL::MVC::Controller*>::iterator itController
			= _rawKeyControllerMap.find(
				static_cast<KeyIdentifier>( keyboardEvent.keysym.sym ) ) ;

		if ( itController != _rawKeyControllerMap.end() )
		{

			OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyReleased: "
				"raw key sent to controller." ) ;

			(*itController).second->rawKeyReleased( keyboardEvent ) ;

		}
		else
		{

			// No controller registered, default to handler map:

			map<KeyIdentifier, KeyboardEventHandler>::iterator itHandler
				=  _rawKeyHandlerMap.find(
					static_cast<KeyIdentifier>( keyboardEvent.keysym.sym ) ) ;

			if ( itHandler != _rawKeyHandlerMap.end() )
			{

				OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyReleased "
					"sent to a raw key handler." ) ;

				(*itHandler).second( keyboardEvent ) ;

			}
			else
			{

				OSDL_KEYBOARD_HANDLER_LOG( "KeyboardHandler::keyReleased "
					"sent to default raw key handler." ) ;

				_defaultRawKeyHandler( keyboardEvent ) ;

			}

		}


		// Avoid handling the same key twice (ex: keyboard mode toggle...)
		return ;

	}

	/*
	 * Release events do not have unicode information, so do nothing on text
	 * input mode.
	 *
	 * They are handled differently, with key repeats.
	 *
	 */

#endif // OSDL_USES_SDL

}

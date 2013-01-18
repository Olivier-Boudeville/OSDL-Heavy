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


#include "OSDLMouseHandler.h"

#include "OSDLMouse.h"             // for Mouse
#include "OSDLController.h"        // for Controller



using std::string ;
using std::list ;


using namespace Ceylan::Log ;      // for LogPlug
using namespace OSDL::Events ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>            // for OSDL_VERBOSE_MOUSE_HANDLER and al
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS





#if OSDL_VERBOSE_MOUSE_HANDLER

#include <iostream>
#define OSDL_MOUSE_HANDLER_LOG( message ) std::cout << "[OSDL Mouse Handler] " << message << std::endl ;

#else // OSDL_VERBOSE_MOUSE_HANDLER

#define OSDL_MOUSE_HANDLER_LOG( message )

#endif // OSDL_VERBOSE_MOUSE_HANDLER




MouseHandler::MouseHandler( bool useClassicalMice ) :
	InputDeviceHandler(),
	_miceCount( 0 ),
	_mice( 0 ),
	_useClassicalMice( useClassicalMice )
{

#if OSDL_USES_SDL

	send( "Initializing Mouse subsystem." ) ;

	// No SDL_InitSubSystem for mice.

	_miceCount = GetAvailableMiceCount() ;

	_mice = new Mouse *[ _miceCount ] ;

	for ( MouseNumber i = 0; i < _miceCount ; i++ )
		_mice[i] = new Mouse( i, _useClassicalMice ) ;

	send( "Mouse subsystem initialized." ) ;

	dropIdentifier() ;

#else // OSDL_USES_SDL

	throw InputDeviceHandlerException( "MouseHandler constructor failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



MouseHandler::~MouseHandler() throw()
{

	send( "Stopping Mouse subsystem." ) ;

	// Mouse instances are owned by the handler:
	blank() ;

	// No SDL_QuitSubSystem for Mouse.

	send( "Mouse subsystem stopped." ) ;

}



bool MouseHandler::hasDefaultMouse() const
{

	return ( _miceCount != 0 ) ;

}



Mouse & MouseHandler::getDefaultMouse()
{

	if ( _miceCount == 0 )
		throw MouseException( "MouseHandler::getDefaultMouse failed: "
			"no mouse detected." ) ;

	return *_mice[DefaultMouse] ;

}



void MouseHandler::linkToController( OSDL::MVC::Controller & controller )
{

	linkToController( DefaultMouse, controller ) ;

}



void MouseHandler::linkToController( MouseNumber mouseIndex,
	OSDL::MVC::Controller & controller )
{

	/*
	 * Beware, mouse list might be out of synch, test is not exactly
	 * equivalent to _miceCount.
	 *
	 */
	if ( mouseIndex >= static_cast<MouseNumber>( GetAvailableMiceCount() ) )
		throw MouseException( "MouseHandler::linkToController: index "
			+ Ceylan::toString( mouseIndex ) + " out of bounds ("
			+ Ceylan::toString( GetAvailableMiceCount() )
			+ " mice attached)." ) ;

	if ( mouseIndex >= _miceCount )
		throw MouseException( "MouseHandler::linkToController: index "
			+ Ceylan::toString( mouseIndex ) + " out of bounds ("
			+ Ceylan::toString( _miceCount )
			+ " mice attached according to internal mouse list)." ) ;

#if OSDL_DEBUG
	if ( _mice[ mouseIndex ] == 0 )
		throw MouseException( "MouseHandler::linkToController: "
			"no known mouse for index "
			+ Ceylan::toString( mouseIndex ) + "." ) ;
#endif // OSDL_DEBUG

	_mice[ mouseIndex ]->setController( controller ) ;

}



const string MouseHandler::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "Mouse handler " ;

	switch( _miceCount )
	{

		case 0:
			res += "does not manage any mouse" ;
			break ;

		case 1:
			res += "managing one mouse" ;
			break ;

		default:
			res += "managing " + Ceylan::toNumericalString( _miceCount )
			+ " mice" ;

	}

	if ( level == Ceylan::low )
		return res ;

	res += ". Listing detected mice: " ;

	list<string> mice ;

	for ( MouseNumber i = 0; i < _miceCount; i++ )
		if ( _mice[i] != 0 )
			mice.push_back( _mice[i]->toString( level ) ) ;
		else
			mice.push_back( "no mouse at index " + Ceylan::toString( i )
				+ " (abnormal)" ) ;

	return res + Ceylan::formatStringList( mice ) ;

}



MouseNumber MouseHandler::GetAvailableMiceCount()
{

	/*
	 * No way of guessing with SDL 1.2.x:
	 *
	 */
	return static_cast<MouseNumber>( 1 ) ;

}





// Protected section.



void MouseHandler::focusGained( const FocusEvent & mouseFocusEvent ) const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkMouseAt( DefaultMouse ) ;
#endif // OSDL_DEBUG

	_mice[ DefaultMouse ]->focusGained( mouseFocusEvent ) ;

#endif // OSDL_USES_SDL

}



void MouseHandler::focusLost( const FocusEvent & mouseFocusEvent ) const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkMouseAt( DefaultMouse ) ;
#endif // OSDL_DEBUG

	_mice[ DefaultMouse ]->focusLost( mouseFocusEvent ) ;

#endif // OSDL_USES_SDL

}



void MouseHandler::mouseMoved( const MouseMotionEvent & mouseMovedEvent ) const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkMouseAt( mouseMovedEvent.which ) ;
#endif // OSDL_DEBUG

	_mice[ mouseMovedEvent.which ]->mouseMoved( mouseMovedEvent ) ;

#endif // OSDL_USES_SDL

}



void MouseHandler::buttonPressed(
	const MouseButtonEvent & mouseButtonPressedEvent ) const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkMouseAt( mouseButtonPressedEvent.which ) ;
#endif // OSDL_DEBUG

	_mice[ mouseButtonPressedEvent.which ]->buttonPressed(
		mouseButtonPressedEvent ) ;

#endif // OSDL_USES_SDL

}


void MouseHandler::buttonReleased(
	const MouseButtonEvent & mouseButtonReleasedEvent ) const
{

#if OSDL_USES_SDL

#if OSDL_DEBUG
	checkMouseAt( mouseButtonReleasedEvent.which ) ;
#endif // OSDL_DEBUG

	_mice[ mouseButtonReleasedEvent.which ]->buttonReleased(
		mouseButtonReleasedEvent ) ;

#endif // OSDL_USES_SDL

}



void MouseHandler::blank()
{

	if ( _mice != 0 )
	{

		for ( MouseNumber c = 0; c < _miceCount; c++ )
		{
			delete _mice[c] ;
		}

		delete [] _mice ;
		_mice = 0 ;
	}

	_miceCount = 0 ;

}



void MouseHandler::checkMouseAt( MouseNumber index ) const
{

	if ( index >= _miceCount )
		Ceylan::emergencyShutdown(
			"MouseHandler::checkMouseAt: index "
			+ Ceylan::toNumericalString( index )
			+ " out of bounds (maximum value is "
			+ Ceylan::toNumericalString( _miceCount - 1 ) + ")." ) ;

	if ( _mice[ index ] == 0 )
		Ceylan::emergencyShutdown( "MouseHandler::checkMouseAt: "
			"no mouse intance at index "
			+ Ceylan::toNumericalString( index ) + "." ) ;

}

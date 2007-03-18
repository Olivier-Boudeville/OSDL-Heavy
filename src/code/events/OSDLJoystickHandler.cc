#include "OSDLJoystickHandler.h"

#include "OSDLJoystick.h"           // for Joystick
#include "OSDLClassicalJoystick.h"  // for ClassicalJoystick
#include "OSDLController.h"         // for axisChanged

#include "OSDLUtils.h"              // for getBackendLastError
#include "OSDLBasic.h"              // for CommonModule


#include "Ceylan.h"                 // for Ceylan logs


#include "SDL.h"                    // for SDL_JoystickEventState


using std::string ;
using std::list ;

using namespace Ceylan::Log ;

using namespace OSDL::Events ;


const string DebugPrefix = " " ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>             // for OSDL_VERBOSE_JOYSTICK_HANDLER and al
#endif // OSDL_USES_CONFIG_H


#if OSDL_VERBOSE_JOYSTICK_HANDLER

#include <iostream>
#define OSDL_JOYSTICK_HANDLER_LOG( message ) std::cout << "[OSDL Joystick Handler] " << message << std::endl ;

#else // OSDL_VERBOSE_JOYSTICK_HANDLER

#define OSDL_JOYSTICK_HANDLER_LOG( message )

#endif // OSDL_VERBOSE_JOYSTICK_HANDLER



JoystickHandler::JoystickHandler( bool useClassicalJoysticks ) 
		throw( InputDeviceHandlerException ) :
	InputDeviceHandler(),
	_joystickCount( 0 ),
	_joysticks( 0 ),
	_useClassicalJoysticks( useClassicalJoysticks )
{

	send( "Initializing joystick subsystem." ) ;

	if ( SDL_InitSubSystem( CommonModule::UseVideo ) 
			!= CommonModule::BackendSuccess )
		throw JoystickException( "JoystickHandler constructor : "
			"unable to initialize joystick subsystem : " 
			+ Utils::getBackendLastError() ) ;

	// We want joystick changes to be translated into events :
	SDL_JoystickEventState( SDL_ENABLE ) ;
	
	update() ;

	send( "Joystick subsystem initialized." ) ;

	dropIdentifier() ;
	
}


JoystickHandler::~JoystickHandler() throw()
{

	send( "Stopping joystick subsystem." ) ;

	// Joysticks instances are owned by the handler :
	blank() ;
	 
	SDL_QuitSubSystem( CommonModule::UseJoystick ) ;
	
	send( "Joystick subsystem stopped." ) ;
	
}



void JoystickHandler::update() throw() 
{

	/*
	 * Joystick instances have to be deleted so that the 
	 * _useClassicalJoysticks value is taken into account 
	 * (it might have changed since last call).
	 *
	 */ 
	blank() ;

	
	// Here the joystick array must be created.
	_joystickCount = GetAvailableJoystickCount() ;

	send( Ceylan::toString( _joystickCount ) + " joystick(s) auto-detected." ) ;
	
	/*
	 * This array, faster than a list, will contain _joystickCount pointers 
	 * to Joystick instances :
	 *
	 */
	_joysticks = new Joystick *[ _joystickCount ] ;
	
	for ( JoystickNumber i = 0; i < _joystickCount; i++ )
		if ( _useClassicalJoysticks )
			_joysticks[i] = new ClassicalJoystick( i ) ;
		else
			_joysticks[i] = new Joystick( i ) ;
	
}


void JoystickHandler::openJoystick( JoystickNumber index ) 
	throw( JoystickException )
{

	/*
	 * Beware, joystick list might be out of synch, test is not exactly
	 * equivalent to _joystickCount.
	 *
	 */	
	if ( index >= GetAvailableJoystickCount() )
		throw JoystickException( "JoystickHandler::openJoystick : index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( GetAvailableJoystickCount() )
			+ " joystick(s) attached)." ) ;
			
	if ( index >= _joystickCount )
		throw JoystickException( "JoystickHandler::openJoystick : index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( _joystickCount )
			+ " joystick(s) attached according to internal joystick list)." ) ;
	
#if OSDL_DEBUG
	if ( _joysticks[ index ] == 0 )
		throw JoystickException( "JoystickHandler::openJoystick : "
			"no known joystick for index " + Ceylan::toString( index ) + "." ) ;
#endif // OSDL_DEBUG
	
	if ( ! _joysticks[ index ]->isOpen() )
		_joysticks[ index ]->open() ;
						
}


void JoystickHandler::linkToController( JoystickNumber index, 
	OSDL::MVC::Controller & controller ) throw( JoystickException )
{

	/*
	 * Beware, joystick list might be out of synch, test is not exactly
	 * equivalent to _joystickCount.
	 *
	 */	
	if ( index >= static_cast<JoystickNumber>( GetAvailableJoystickCount() ) )
		throw JoystickException( "JoystickHandler::linkToController : index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( GetAvailableJoystickCount() )
			+ " joystick(s) attached)." ) ;
			
	if ( index >= _joystickCount )
		throw JoystickException( "JoystickHandler::linkToController : index " 
			+ Ceylan::toString( index) + " out of bounds (" 
			+ Ceylan::toString( _joystickCount )
			+ " joystick(s) attached according to internal joystick list)." ) ;
	
#if OSDL_DEBUG
	if ( _joysticks[ index ] == 0 )
		throw JoystickException( "JoystickHandler::linkToController : "
			"no known joystick for index " + Ceylan::toString( index ) + "." ) ;
#endif // OSDL_DEBUG

	_joysticks[ index ]->setController( controller ) ;
	
}
	

const string JoystickHandler::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{

	string res ;
	
	if ( _joystickCount > 0 )
		res = "Joystick handler managing " 
		+ Ceylan::toString( _joystickCount ) + " joystick(s)" ;
	else
		return "Joystick handler does not manage any joystick" ;

	if ( level == Ceylan::low ) 	
		return res ;
	
	res += ". Listing detected joystick(s) : " ;
		
	list<string> joysticks ;
		
	for ( JoystickNumber i = 0; i < _joystickCount; i++ )
		if ( _joysticks[i] != 0 )
			joysticks.push_back( _joysticks[i]->toString( level ) ) ;
		else
			joysticks.push_back( "no joystick at index " 
				+ Ceylan::toString( i ) + " (abnormal)" ) ;
		
	return res + Ceylan::formatStringList( joysticks ) ;
		
}



JoystickNumber JoystickHandler::GetAvailableJoystickCount() throw() 
{

	return static_cast<JoystickNumber>( SDL_NumJoysticks() ) ;
	
}



// Protected section.


void JoystickHandler::axisChanged( const JoystickAxisEvent & joystickEvent )
	const throw()
{

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->axisChanged( joystickEvent ) ;
	
}


void JoystickHandler::trackballChanged( 
	const JoystickTrackballEvent & joystickEvent ) const throw()
{

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->trackballChanged( joystickEvent ) ;
	
}


void JoystickHandler::hatChanged( const JoystickHatEvent & joystickEvent ) 
	const throw()
{

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->hatChanged( joystickEvent ) ;
	
}


void JoystickHandler::buttonPressed( const JoystickButtonEvent & joystickEvent )
	const throw()
{

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->buttonPressed( joystickEvent ) ;
	
}


void JoystickHandler::buttonReleased( 
	const JoystickButtonEvent & joystickEvent ) const throw()
{

#if OSDL_DEBUG
	checkJoystickAt( joystickEvent.which ) ;
#endif // OSDL_DEBUG
	
	_joysticks[ joystickEvent.which ]->buttonReleased( joystickEvent ) ;
	
}



void JoystickHandler::blank() throw()
{

	if ( _joysticks != 0 )
	{

		for ( JoystickNumber c = 0; c < _joystickCount; c++ )
		{
			delete _joysticks[c] ;
		}

		delete [] _joysticks ;
		_joysticks = 0 ;
	}
	
	_joystickCount = 0 ;
	
}


void JoystickHandler::checkJoystickAt( JoystickNumber index ) const throw()
{

	if ( index >= _joystickCount )
		Ceylan::emergencyShutdown( 
			"JoystickHandler::checkJoystickAt : index "
			+ Ceylan::toNumericalString( index ) 
			+ " out of bounds (maximum value is "
			+ Ceylan::toNumericalString( _joystickCount - 1 ) + ")." ) ; 
		
	if ( _joysticks[ index ] == 0 )
		Ceylan::emergencyShutdown( "JoystickHandler::checkJoystickAt : "
			"no joystick intance at index "
			+ Ceylan::toNumericalString( index ) + "." ) ;

	if ( ! _joysticks[ index ]->isOpen() )
		Ceylan::emergencyShutdown( 
			"JoystickHandler::checkJoystickAt : joystick at index "
			+ Ceylan::toNumericalString( index ) + " was not open." ) ; 
	
}


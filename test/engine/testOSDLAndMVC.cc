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
using namespace OSDL::Events ;
using namespace OSDL::Video ;

using namespace Ceylan::Log ;


#include <iostream>  // for cout
using std::cout ;
using std::endl ;

#include <string>
using std::string ;

#include <list>
using std::list ;


typedef Ceylan::Uint8 Direction ;



/**
 * Tests the whole event-driven MVC framework with the basic event loop:
 *
 *  - a specific controller is registered, so that it manages both the keyboard
 * arrows and the first joystick (if any)
 *
 *  - whenever an event occurs with the keyboard arrows or the first joystick,
 * this controller activates the model which registers the corresponding
 * requested direction and changes its own state
 *
 *  - then the model calls its views so that they can know its new state
 * (i.e. its direction) and produce their rendering accordingly (print one of
 * the '>', '^', '<' and 'v' symbols).
 *
 * @note This test is said to be event-driven since it does not involve a
 * scheduler: it is only when an input event occurs that the whole MVC chain
 * gets activated (controllers, then models, then views).
 *
 * @see testScheduledOSDLAndMVC.cc the version with scheduler
 *
 */
class MyMVCEvent : public Ceylan::MVCEvent
{

public:


  MyMVCEvent( Ceylan::EventSource & source ) throw():
	MVCEvent( source ),
	_direction( 1  )
  {

  }


  void setDirection( Direction newDirection ) throw()
  {

	_direction = newDirection ;

  }


  Direction getDirection() const throw()
  {

	return _direction ;

  }


protected:

  Direction _direction ;

} ;




class MyController : public OSDL::MVC::Controller
{

public:


  MyController() throw()
  {
	_eventForModel = new MyMVCEvent( *this ) ;
  }

  virtual ~MyController() throw()
  {
	delete _eventForModel ;
  }

  Direction getDirection() const throw()
  {

	return _direction ;

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
	case KeyboardHandler::EnterKeypadKey:
	  _direction = 5 ;
	  break ;

	default:
	  // Do nothing.
	  break ;
	}


	// We are event-driven here:
	propagateState() ;

  }



  /**
   * Propagates the state of this controller to the registered models.
   *
   * Necessary in an event-driven context.
   *
   */
  void propagateState()
  {

	_eventForModel->setDirection( _direction ) ;
	notifyAllListeners( *_eventForModel ) ;

  }


  void joystickFirstButtonPressed() throw()
  {

	_direction = 5 ;
	propagateState() ;

  }


  void joystickUp( AxisPosition leftExtent ) throw()
  {

	_direction = 1 ;
	propagateState() ;

  }


  void joystickDown( AxisPosition leftExtent ) throw()
  {

	_direction = 2;
	propagateState() ;

  }


  void joystickLeft( AxisPosition leftExtent ) throw()
  {

	_direction = 3 ;
	propagateState() ;

  }


  void joystickRight( AxisPosition leftExtent ) throw()
  {

	_direction = 4 ;
	propagateState() ;

  }


  const Ceylan::Event & getEventFor(
	const Ceylan::CallerEventListener & listener )
	throw( Ceylan::EventException )
  {

	return *_eventForModel ;

  }


  /// Not used here.
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

	case 5:
	  return "quit! (seen by controller)" ;
	}

	return "(unexpected direction selected): "
	  + Ceylan::toString( _direction ) ;

  }



private:


  /// 1: up, 2: down, 3: left, 4: right, 5: quit.
  Direction _direction ;

  MyMVCEvent * _eventForModel ;

} ;



// This is a periodical model.
class MyModel : public OSDL::MVC::PeriodicalModel
{


public:


  MyModel( Events::Period period ):
	PeriodicalModel( period, /* autoRegister */ false ),
	_actualDirection( 1 )
  {

	_eventForView = new MyMVCEvent( * this ) ;
	// No registering since no scheduler is used.

  }


  virtual ~MyModel() throw()
  {
	delete _eventForView ;
  }


  virtual void beNotifiedOf( const Ceylan::Event & newEvent ) throw()
  {

	const MyMVCEvent * event = dynamic_cast<const MyMVCEvent *>(
	  & newEvent ) ;

	if ( event != 0 )
	  _actualDirection = event->getDirection() ;

	// We are event-driven here:
	_eventForView->setDirection( _actualDirection ) ;
	notifyAllViews( *_eventForView ) ;

  }


  virtual const Ceylan::Event & getEventFor(
	const Ceylan::CallerEventListener & listener )
	throw( Ceylan::EventException )
  {

	return *_eventForView ;

  }


  virtual void onActivation( Events::SimulationTick newTick ) throw()
  {

	// Will not be called without a scheduler.

  }


private:

  Direction _actualDirection ;

  MyMVCEvent * _eventForView ;


} ;



class MyView: public Ceylan::View
{


public:


  MyView( Ceylan::Model & model, EventsModule & events ) throw():
	View( model ),
	_events( & events )
  {

  }


  /**
   * Example of what could be written to manage a view being scheduled
   * (rendering tick).
   *
   * This code is not used here, but shows how views may act.
   *
   */
  virtual void triggerRendering() throw()
  {

	// First retrieves from the model the necessary information:
	Ceylan::Model * myModel ;

	// Actually only one source:
	for ( std::list<Ceylan::EventSource *>::iterator it
			= _sources.begin() ; it != _sources.end(); it++ )
	{

	  /*
	   * That's why using the template-based version should be preferred (casts
	   * everywhere):
	   *
	   */
	  myModel = dynamic_cast<Ceylan::Model *>( *it ) ;

	  const MyMVCEvent * myModelEvent
		= dynamic_cast<const MyMVCEvent *>(
		  & myModel->getEventFor( * this ) ) ;

	  // Directions could be blended:
	  _actualDirection = myModelEvent->getDirection() ;

	}

	// Then render!
	renderModel() ;

  }


  virtual void renderModel() throw()
  {

	switch( _actualDirection )
	{

	case 1:
	  cout << "^" << endl ;
	  break ;

	case 2:
	  cout << "v" << endl ;
	  break ;

	case 3:
	  cout << "<" << endl ;
	  break ;

	case 4:
	  cout << ">" << endl ;
	  break ;

	case 5:
	  cout << "quit! "
		"(seen by the view, actual end of event loop)" << endl ;
	  _events->requestQuit() ;
	  break ;

	default:
	  cout << "(unexpected direction selected): "
		   << _actualDirection << endl ;
	  break ;
	}

  }


  virtual void beNotifiedOf( const Ceylan::Event & newEvent ) throw()
  {

	const MyMVCEvent * event = dynamic_cast<const MyMVCEvent *>(
	  & newEvent ) ;

	if ( event != 0 )
	{
	  _actualDirection = event->getDirection() ;
	}

	// Then render!
	renderModel() ;

  }


private:

  Direction _actualDirection ;
  EventsModule * _events ;

} ;



/**
 * Testing OSDL MVC integration with input layer, when it is event-driven (no
 * scheduler is used).
 *
 * This test creates MVC instances as automatic variables, to test the framework
 * robustness: it is a typical example of a misuse that should have led to a
 * crash, since the deallocation order of aController, aModel and aModel is not
 * mastered.
 *
 * Nevertheless counter-measures are applied and traced through the log system,
 * so that the user is notified that the life cycle of his objects is wrong.
 *
 * The right solution would have been to use blocks to control the variable
 * scopes, or to create instances thanks to new/delete pairs.
 *
 * The 'quit' request will be processed only by a view, after having following
 * the full MVC route.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	try
	{

	  LogPlug::info( "Testing OSDL event-driven MVC integration." ) ;

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

	  OSDL::CommonModule & myOSDL = OSDL::getCommonModule(
		CommonModule::UseJoystick | CommonModule::UseKeyboard ) ;

	  LogPlug::info( "Testing basic event handling." ) ;

	  LogPlug::info( "Getting events module." ) ;
	  EventsModule & myEvents = myOSDL.getEventsModule() ;

	  LogPlug::info( "Events module: " + myEvents.toString() ) ;

	  KeyboardHandler & myKeyboardHandler = myEvents.getKeyboardHandler() ;

	  myKeyboardHandler.setSmarterDefaultKeyHandlers() ;

	  JoystickHandler & myJoystickHandler = myEvents.getJoystickHandler() ;

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

	  // A SDL window is needed to have the SDL event system working:
	  myVideo.setMode( 640, 480, VideoModule::UseCurrentColorDepth,
		VideoModule::SoftwareSurface ) ;


	  /*
	   * Create and link MVC instances (new and pointers should be used to
	   * control deallocation order).
	   *
	   * This test relies on OSDL's MVC ability to overcome faulty life cycle,
	   * see the Error log channel to understand why (therefore it is also a
	   * test for robustness to misuse).
	   *
	   */

	  MyModel aModel( /* period */ 3 ) ;

	  MyView aView( aModel, myEvents ) ;

	  MyController aController ;

	  aModel.subscribeToController( aController ) ;


	  myKeyboardHandler.linkToController( KeyboardHandler::UpArrowKey,
		aController ) ;

	  myKeyboardHandler.linkToController( KeyboardHandler::DownArrowKey,
		aController ) ;

	  myKeyboardHandler.linkToController( KeyboardHandler::LeftArrowKey,
		aController ) ;

	  myKeyboardHandler.linkToController( KeyboardHandler::RightArrowKey,
		aController ) ;

	  myKeyboardHandler.linkToController( KeyboardHandler::EnterKey,
		aController ) ;

	  myKeyboardHandler.linkToController( KeyboardHandler::EnterKeypadKey,
		aController ) ;

	  if ( joyCount > 0 )
		myJoystickHandler.linkToController( /* JoystickNumber */ 0,
		  aController ) ;


	  if ( isBatch )
	  {

		LogPlug::warning( "Main loop not launched, as in batch mode." ) ;

	  }
	  else
	  {

		LogPlug::info( "Entering the event loop for event waiting "
		  "so that Controller can act." ) ;

		std::cout << "< Hit Enter or push the first button "
		  "of the first joystick (if any) "
		  "to end this event-driven MVC test >" << std::endl ;

		myEvents.enterMainLoop() ;
		LogPlug::info( "Exiting main loop." ) ;

	  }


	  LogPlug::info( "End of OSDL event-driven MVC test." ) ;

	  LogPlug::warning( "The MVC were allocated on the stack, hence their "
		"deallocation order is not mastered, messages in error log plug "
		"may appear." ) ;

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

  }

  OSDL::shutdown() ;

  return Ceylan::ExitSuccess ;

}

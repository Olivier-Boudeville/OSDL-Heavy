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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Events ;
using namespace OSDL::Video ;
using namespace OSDL::Engine ;


using namespace Ceylan::Log ;
using namespace Ceylan::Maths ;
using namespace Ceylan::Maths::Random ;


#include <list>
using std::list ;





/**
 * Test of OSDL scheduler, in batch (non-interactive, non-real time) mode, also
 * known as screenshot no-deadline mode.
 *
 * Some useful testing can be made during the two *virtual* seconds this test
 * should last (in batch mode; in interactive mode it should last for 30 virtual
 * seconds), including running it with various machine loads to check it does
 * not change anything on the result (still no skip, only an extended duration).
 *
 * @note As the scheduler has no deadline to meet, on most platforms it will be
 * far faster than the clock, and 30 virtual seconds may last for less than 2
 * real seconds.
 *
 * @see testOSDLScheduler.cc for the soft realtime version.
 *
 */



/**
 * The role of this object is to have the scheduler stop at given simulation
 * time.
 *
 * @note A scheduler already exists before any of these objects is created.
 *
 */
class SchedulerStopper : public OSDL::Engine::ProgrammedActiveObject
{

public:


  SchedulerStopper( SimulationTick stopSimulationTick,
	bool verbose = false )
	throw( SchedulingException ):
	ProgrammedActiveObject(
	  stopSimulationTick,
	  /* absolutelyDefined */ true,
	  /* autoregister */ true ),
	_stopTick( stopSimulationTick ),
	_verbose( verbose )
  {

	if ( _verbose )
	  LogPlug::info( "SchedulerStopper constructor: "
		"will stop at simulation tick #"
		+ Ceylan::toString( _stopTick ) + "."  ) ;

  }


  virtual void onActivation( Events::SimulationTick newTick ) throw()
  {

	if ( _verbose )
	  LogPlug::info( "SchedulerStopper::onActivation: "
		"activated for simulation tick "
		+ Ceylan::toString( newTick ) + "." ) ;

	if ( newTick == _stopTick )
	{
	  LogPlug::info( "SchedulerStopper::onActivation: "
		"stopping scheduler." ) ;
	  Scheduler::GetExistingScheduler().stop() ;
	}

  }


  virtual void onSkip( Events::SimulationTick newTick ) throw()
  {
	
	LogPlug::warning( "SchedulerStopper::onSkip: the simulation tick "
	  + Ceylan::toString( newTick ) + " had been skipped !" ) ;

	onActivation( newTick ) ;

  }


  /**
   * This pure method of EventListener is inherited since ActiveObjects are
   * Model instances.
   *
   */
  virtual void beNotifiedOf( const Ceylan::Event & newEvent ) throw()
  {
	LogPlug::trace( "SchedulerStopper is notified of "
	  + newEvent.toString() + "." ) ;
  }


private:

  SimulationTick _stopTick ;
  bool _verbose ;

} ;





/**
 * Testing the services of the OSDL scheduler for active objects, in batch mode.
 *
 * @see ActiveObject
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	try
	{


	  LogPlug::info( "Testing OSDL scheduler services in batch mode." ) ;

	  // Tells when the test will stop, by default after 30s (100 Hz):
	  Events::SimulationTick stopTick = 30 * 100 ;


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

		  /*
		   * Will stop the scheduler after 2 seconds (200 simulation ticks,
		   * since logic frequency is 100 Hz here).
		   *
		   */
		  stopTick = 2*100 ;

		  tokenEaten = true ;

		}

		if ( token == "--interactive" )
		{
		  LogPlug::info( "Interactive mode selected" ) ;
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


	  if ( ! Ceylan::System::areSubSecondSleepsAvailable() )
	  {

		LogPlug::info( "No subsecond sleep available, "
		  "scheduler cannot run, test finished." ) ;

		return Ceylan::ExitSuccess ;

	  }


	  LogPlug::info(
		"Starting OSDL with video and, therefore, events enabled." ) ;

	  OSDL::CommonModule & myOSDL = OSDL::getCommonModule(
		CommonModule::UseVideo | CommonModule::UseEvents ) ;

	  LogPlug::info( "Testing basic event handling." ) ;

	  LogPlug::info( "Getting events module." ) ;
	  EventsModule & myEvents = myOSDL.getEventsModule() ;

	  LogPlug::info( EventsModule::DescribeEnvironmentVariables() ) ;

	  LogPlug::info( "Displaying a dummy window "
		"to have access to an event queue." ) ;

	  LogPlug::info( "Getting video." ) ;
	  OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ;

	  // A SDL window is needed to have the SDL event system working:
	  myVideo.setMode( 640, 480, VideoModule::UseCurrentColorDepth,
		VideoModule::SoftwareSurface ) ;

	  LogPlug::info( "Asking for a scheduler to be used." ) ;
	  myEvents.useScheduler() ;

	  Scheduler::GetExistingScheduler().setScreenshotMode( true,
		/* frameFilenamePrefix */ "testOSDLSchedulerNoDeadline-" ) ;

	  LogPlug::info( "Create an active object whose role is "
		"to stop the scheduler at simulation tick "
		+ Ceylan::toString( stopTick ) + "." ) ;

	  const Ceylan::Uint32 stoppersCount = 300 ;

	  list<SchedulerStopper *> stoppers ;
	  WhiteNoiseGenerator stopTickRand( 0, stoppersCount ) ;

	  stoppers.push_back( new SchedulerStopper(
		  stopTick + stopTickRand.getNewValue(), /* verbose */ true ) ) ;

	  for ( Ceylan::Uint32 i = 1; i < stoppersCount; i++ )
	  {
		// All stoppers will stop at simulation tick 200 or later:
		stoppers.push_back( new SchedulerStopper(
			stopTick + stopTickRand.getNewValue() ) ) ;
	  }


	  LogPlug::info( "Entering the schedule loop." ) ;
	  myEvents.enterMainLoop() ;
	  LogPlug::info( "Exit from schedule loop." ) ;

	  for ( list<SchedulerStopper *>::iterator it = stoppers.begin();
			it != stoppers.end() ; it++ )
		delete (*it) ;

	  LogPlug::info( "stopping OSDL." ) ;
	  OSDL::stop() ;

	  LogPlug::info( "End of OSDL scheduler test." ) ;

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

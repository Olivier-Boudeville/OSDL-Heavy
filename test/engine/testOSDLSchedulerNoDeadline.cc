#include <list>
using std::list ;

#include "Ceylan.h"
using namespace Ceylan::Log ;
using namespace Ceylan::Maths::Random ;


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Events ;
using namespace OSDL::Engine ;


/**
 * Test of OSDL scheduler, in batch (non-interactive, non-real time) mode, also known as
 * screenshot no-deadline mode.
 *
 * Some useful testing can be made during the two seconds this test should last, including
 * running it with various machine loads to check it does not change anything on the result
 * (still no skip, only an extended duration).
 *
 *
 * @see testOSDLScheduler.cc
 *
 */
 
 
/**
 * The role of this object is to have the scheduler stop at given simulation time. 
 *
 * @note A scheduler already exists before any of these objects is created.
 *
 */
class SchedulerStopper : public OSDL::Engine::ActiveObject
{

	public:
	
	
		SchedulerStopper( SimulationTick stopSimulationTick ) throw( SchedulingException ) :
				ActiveObject( stopSimulationTick, /* absolutlyDefined */ true ) 
		{

			// Be also a periodic object :
			
			const Hertz desiredFrequency = 10 ;
			
			Hertz obtainedFrequency = setFrequency( desiredFrequency ) ;
			LogPlug::info( "SchedulerStopper constructor : for a desired activation frequency of "
				+ Ceylan::toString( desiredFrequency ) + " Hz, obtained "
				+ Ceylan::toString( obtainedFrequency ) + " Hz." ) ;
			
			// This active object registers itself to the scheduler.
			Scheduler::GetExistingScheduler().registerObject( * this ) ;
		}
		
		
		virtual void onActivation( Events::SimulationTick newTick ) throw()
		{
			LogPlug::info( "SchedulerStopper::onActivation : activated for simulation tick "
				+ Ceylan::toString( newTick ) + "." ) ;
			
			if ( newTick == 200 )
			{	
				LogPlug::info( "SchedulerStopper::onActivation : stopping scheduler." ) ;
				Scheduler::GetExistingScheduler().stop() ;
			}	
			
		}
		
		
		virtual void onSkip( Events::SimulationTick newTick ) throw()
		{
			LogPlug::warning( "SchedulerStopper::onSkip : the simulation tick "
				+ Ceylan::toString( newTick ) + " had been skipped !" ) ;
				
			onActivation( newTick ) ;
		}
		
		
		/**
		 * This pure method of EventListener is inherited since ActiveObjects are models.
		 *
		 *
		 */
		virtual void beNotifiedOf( const Ceylan::Event & newEvent ) throw()
		{
			LogPlug::trace( "SchedulerStopper is notified of " + newEvent.toString() + "." ) ;
		}
		
		
} ;



/**
 * Testing the services of the OSDL scheduler for active objects.
 *
 * @see ActiveObject
 *
 */

int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Testing OSDL scheduler services." ) ;

		LogPlug::info( "Starting OSDL with video and, therefore, events enabled." ) ;		
        OSDL::CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo ) ;		
			
		LogPlug::info( "Testing basic event handling." ) ;
		
		LogPlug::info( "Getting events module." ) ;
		EventsModule & myEvents = myOSDL.getEventsModule() ; 

		LogPlug::info( EventsModule::DescribeEnvironmentVariables() ) ;
				
		LogPlug::info( "Displaying a dummy window to have access to an event queue." ) ;
			
		LogPlug::info( "Getting video." ) ;
		OSDL::Video::VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		// A SDL window is needed to have the SDL event system working :
		myVideo.setMode( 640, 480, 16, OSDL::Video::VideoModule::SoftwareSurface ) ;
		
		LogPlug::info( "Ask for the scheduler to be used." ) ;
		myEvents.useScheduler() ;
		
		Scheduler::GetExistingScheduler().setScreenshotMode( true, 
			/* frameFilenamePrefix */ "testOSDLSchedulerNoDeadline-" ) ;
			
		/*
		 * Will stop the scheduler after 2 seconds 
		 * (200 simulation ticks, since logic frequency is 100 Hz here).
		 *
		 */
		SimulationTick stopTick = 200 ;
		LogPlug::info( "Create an active object whose role is to stop the scheduler "
			"at simulation tick " + Ceylan::toString( stopTick ) + "." ) ;
		
		const unsigned int stoppersCount = 300 ;
		
		list<SchedulerStopper *> stoppers ;
		WhiteNoiseGenerator stopTickRand( 0, stoppersCount ) ;
		for ( unsigned int i = 0; i < stoppersCount; i++ )
		{
			// All stoppers will stop at simulation tick 200 or later :
			stoppers.push_back( new SchedulerStopper( 200 + stopTickRand.getNewValue() ) ) ;		
		}
		
				
		LogPlug::debug( "After object registering : " 
			+ Scheduler::GetExistingScheduler().toString( Ceylan::high ) ) ;
			
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


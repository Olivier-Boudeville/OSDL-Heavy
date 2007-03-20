#ifndef OSDL_SCHEDULER_H_
#define OSDL_SCHEDULER_H_



// for ListOfActiveObjects, SchedulingException, etc. :
#include "OSDLEngineCommon.h"         
#include "OSDLEvents.h"               // for SimulationTick
#include "OSDLPeriodicSlot.h"         // for PeriodicSlot


// for inheritance, SingletonException, time units :
#include "Ceylan.h"                   


#include <string>
#include <list>
#include <map>



namespace OSDL
{

	namespace Events
	{
	
	
		// Scheduler is managed by the event module.
		class EventsModule ;
		
	}


	namespace Rendering
	{
	
	
		// The scheduler may use a dedicated renderer for rendering concerns.
		class Renderer ;
		
	}

	
	namespace Video
	{
	
		/*
		 * The scheduler fallbacks to the video module if no renderer is 
		 * being used.
		 *
		 */
		class VideoModule ;
		
	}
	
	
	namespace Engine 
	{
	
		
		// The scheduler schedules active objects.
		class ActiveObject ;


		// The scheduler uses periodic slots for periodically activated objects.
		class PeriodicSlot ;
		
				
		/**
		 * This scheduler manages active objects so that they are 
		 * in turn granted with the processing power they requested.
		 *
		 * Two kinds of active objects can be scheduled : 
		 *   - periodically activated objects. They just have to define 
		 * their period, expressed in simulation time
		 *   - objects whose activation is specifically determined, as a 
		 * set of simulation steps.
		 * These steps are defined relatively to when they have been 
		 * declared to the scheduler, or as absolute steps.
		 *
		 * @note For the application objects, the only relevant and 
		 * useful unit for time with respect to scheduling is the simulation
		 * time, expressed in Events::SimulationTick. 
		 * They should base their behaviour on this time, and only this time.
		 *
		 * Rendering task is scheduled thanks to Events::RenderingTick, 
		 * which allows to specify any frequency independently from logic
		 * frequency.
		 *
		 * Whereas the scheduler internal time (EngineTick) is kept in
		 * synchronism with actual user time (in quasi-hard real time), the 
		 * simulation and rendering times cannot ensure that deadlines are
		 * respected (because of many reasons, from the machine load to peak
		 * resource needs, OS scheduling and so on). 
		 * Therefore they are only in soft real-time, and it is the
		 * job of the scheduler to manage that everything is done on time, 
		 * on a best effort basis.
		 * Whenever a failure to meet a deadline occurs, the impacted 
		 * objects are provided a means of being noticed, in order to be 
		 * able to take any counter-measures they can.
		 *
		 * The scheduler has two main data structures :
		 *  - the 'periodic' one, which is composed of lists of active 
		 * objects for each used  periodicity, gathered thanks to periodic 
		 * slots
		 *	- the 'programmed' one, which allows objects with non periodic
		 *  behaviour to plan themselves their activation at specific 
		 * simulation ticks
		 *
		 * Our scheduler can follow two different behaviours :
		 *  - the default one, which focuses on interactive applications, 
		 * trying to give the user the best possible logic and, if possible,
		 * framerate, on soft real time (best effort)
		 *  - the screenshot mode, where rendering targets a particular
		 * framerate, and generates PNG screenshots meant to be gathered 
		 * in a movie (ex : MPEG). 
		 * This is a batch (non-interactive) mode, where the time necessary 
		 * to update the logic and the frames does not matter : no deadline 
		 * is to be respected in this mode.
		 * 
		 * Choosing a high logic frequency, and moreover higher than the
		 * rendering frequency, is the way to go, since it allows to get 
		 * rid of interpolation and resampling issues. 
		 * Clearly 33 Hz is the very least logic frequency one should use, 
		 * and 100 Hz or higher frequencies should be preferred.
		 * Logic frequency should be multiple times higher than rendering
		 * frequency.
		 *
		 * For the logic, please remember that the only time unit that 
		 * should be used is the simulation tick. 
		 * Do not try to depend on any real time measurement, since it 
		 * would bypass the efforts of the scheduler to provide the 
		 * application with a virtual fixed-rate logic time, and make it fail. 
		 *
		 * For the rendering, beware that some graphic primitives wait for
		 * vsync, which may freeze the process until the remaining part of 
		 * the, say, 1/60th of second is reached. 
		 * This hard real time practise would interfere badly with any
		 * scheduler. 
		 * You can disable vsync, but one might experience more tearing and
		 * shearing. 
		 * Conversely, the scheduler could take this behaviour into account 
		 * and synchronize itself on the display refresh rate. 
		 * It would have to manage a decrease in the frame rate if the scene
		 * complexity comes too close to have a rendering duration of the 
		 * magic times (1/60 of second, 1/30 etc.) : because of random
		 * variations, some frames would go slightly over or under the
		 * time, leading to an irregular framerate which may be visually
		 * disturbing. 
		 * Better switch to lower rendering rates and have a constant frame
		 * rate.
		 *
		 * Rendering is obtained thanks to a renderer, if available, which 
		 * is called at each rendering tick.
		 * If no renderer is being used, then the video module will be used, 
		 * and requested to redraw at each rendering tick. 
		 * The scheduled objects are in this case responsible for the 
		 * updating of the screen surface.
		 *
		 * @fixme add a means of knowing whether hardware synch with refresh
		 * rate is possible and, if so, use it not to schedule instead of 
		 * the scheduler, but to make the scheduler start at the right time,
		 * so that it keeps naturally the synchronisation with the refresh 
		 * rate. 
		 * Or force the engine tick to be a hardware-locked frame pace.
		 *
		 * Apart the logic and rendering frequencies, the scheduler have to
		 * handle the input frequency, which is the frequency at which input
		 * devices (such as keyboard, mouse, joysticks, etc.) are updated,
		 * according to pending low level events. 
		 * One of the purposes of input handling is to have the controllers
		 * updated so that the simulation world is interactive. 
		 * SDL input loop runs at 100 Hz, so higher frequencies would be
		 * useless.
		 *
		 * @note With default settings, simulation frequency granularity 
		 * should be less than + or -3 Hz on average.
		 *
		 * @note The Scheduler is meant to be used as a singleton : having
		 * multiple scheduler instances on activity would be meaningless, 
		 * and they would probably fail collectively. 
		 * 
		 * The scheduler has been made so that no multithread paradigm is
		 * needed, but it could be useful one day to parallelize (if possible)
		 * at least the simulation evaluation, since there may be
		 * numerous active objects that could take advantage of hyperthreading,
		 * dual cores etc.
		 * 
		 * For the simulation step, there could be too a two-pass algorithm :
		 * first compute, but do not apply, all the newer states of all 
		 * objects, then, second pass, apply them. 
		 * The problem is indeed that an object whose state depends on the 
		 * state of other objects should take them into account at time t, 
		 * when computing state at t+1, in their state at t, and not t+1. 
		 * If all objects were to be updated in a single row, the first 
		 * ones would be already at t+1 while the remaining would have 
		 * to be updated from t to t+1. 
		 * As among the remaining objects some may depend on the first ones,
		 * they would take into account t+1 states and not t states. 
		 * Moreover, as there may be cyclic dependencies (say, for collisions,
		 * where two objects became mutually dependent), no right order 
		 * can be found.  
		 * Therefore a two-pass algorithm may be helpful.
		 * 
		 * @note Some breathing time (letting the operating system schedule
		 * other processes) would be probably necessary, since for example
		 * the Linux kernel seems to take control away for a rather
		 * long while when it detects a program running at 100%. 
		 * However it would involve waiting regularly for 1ms (kernel 2.6) 
		 * or 10ms (kernel 2.4), and many skips would occur.
		 *
		 * @note The scheduler can run continuously for up to 48 days : after,
		 * engine ticks will overflow and it will lead to a scheduler shutdown.
		 *
		 */	
		class OSDL_DLL Scheduler : public Ceylan::Object
		{
		
		
			/// Used to measure time past deadlines.
			typedef Ceylan::Uint32 Delay ;
			
			
			public:
			
			
				
				/**
				 * Tells whether a renderer is available to this scheduler.
				 * If no renderer is registered, the video module will be 
				 * used instead.
				 *
				 */
				virtual bool hasRenderer() const throw() ;


				/**
				 * Returns currently used renderer, if any.
				 *
				 * @throw SchedulingException if no renderer is available.
				 *
				 */
				virtual Rendering::Renderer & getRenderer() const 
					throw( SchedulingException ) ;


				/**
				 * Assigns a new renderer, which will be called on every
				 * rendering tick, either to actually render, or to be 
				 * notified of a rendering skip.
				 *
				 * @param newRenderer the renderer to be used from now.
				 *
				 * @note Will delete any previously existing renderer, and 
				 * will take ownership of the specified renderer.
				 *
				 */
				virtual void setRenderer( Rendering::Renderer & newRenderer )
					throw() ;
				
				
				
				/** 
				 * Tells whether the application should run interactively, 
				 * in real time (if <b>on</b> is false), or if it should 
				 * run in record mode, with a virtual fixed framerate (as
				 * specified as <b>frameFrequency</b>), with logic and 
				 * rendering taking all the time they need. 
				 * 
				 * In this mode, even if the logic and the rendering tasks 
				 * last for two hours for each virtual second, the 
				 * resulting set of PNG files is made so that these
				 * files can be gathered to form an animation with, for
				 * instance, 25 frames per second. 
				 * When this animation will be played, the logic will seem
				 * to have acted with respect to the planned framerate, as if
				 * in real time. 
				 *
				 * @param on tells whether frames are to be rendered to 
				 * file (iff on), or if the application should run
				 * interactively.
				 *
				 * @param frameFilenamePrefix determines which prefix will 
				 * be used to name successive screenshots, in the form :
				 * 'frameFilenamePrefix'-n.png, n being the frame count of the
				 * animation. 
				 *
				 * @param frameFrequency tells how many frames should be
				 * rendered each second. 
				 * At least 25 frames per second is recommended to be able 
				 * to realize smooth animations.
				 *
				 * @see OSDL user's guide to know how simply create a MPEG 
				 * video out of the set of image files that is produced in
				 * screenshot mode.
				 *
				 */
				virtual void setScreenshotMode( bool on, 
					const std::string & frameFilenamePrefix, 
					Events::Hertz frameFrequency = 25 ) throw() ;
				
				
				/**
				 * Defines how many microseconds an engine tick should last, 
				 * and updates accordingly the simulation and rendering ticks.
				 * 
				 * For example, an engine tick duration of 1000 microseconds
				 * defines a scheduling frequency of 1 kHz (1000 Hz). 
				 * Then all scheduled events should then have a periodicity of
				 * at most 1000 Hz, probably far lower.
				 *
				 * @param engineTickDuration the period of time between two
				 * scheduler elementary ticks, expressed in microseconds.
				 *
				 * @note When changing this setting, in most cases the 
				 * scheduled objects are to be changed or updated.
				 *
				 * @see DefaultEngineTickDuration
				 *
				 */				
				virtual void setTimeSliceDuration( 
					Ceylan::System::Microsecond engineTickDuration ) throw() ;

	
				/**
				 * Returns the current time slice duration, as used by this 
				 * scheduler.
				 *
				 * @note Average settings would be 1000 microseconds = 
				 * 1 millisecond for each engine tick.
				 *
				 */
				virtual Ceylan::System::Microsecond getTimeSliceDuration() 
					const throw() ;
				
				
				/**
				 * Sets the simulation frequency, by evaluating the number 
				 * of engine ticks to be associated to one simulation tick 
				 * for <b>frequency</b> to be met.
				 *
				 * @throw SchedulingException if requested frequency is 
				 * superior to engine frequency.
				 *
				 * @see DefaultSimulationFrequency
				 *
				 */
				virtual void setSimulationFrequency( Events::Hertz frequency ) 
					throw( SchedulingException ) ;

				
				/**
				 * Returns the number of engine ticks which correspond to 
				 * one simulation tick.
				 *
				 * @note It allows to take into account the round-off that 
				 * had to be performed by the setSimulationFrequency method.
				 *
				 */
				virtual Events::Period getSimulationTickCount() const throw() ;
				
				
				/**
				 * Sets the rendering frequency, by evaluating the number 
				 * of engine ticks to be associated to one rendering tick.
				 *
				 * @throw SchedulingException if requested frequency is 
				 * superior to engine frequency.
				 *
				 * @see DefaultRenderingFrequency
				 *
				 */
				virtual void setRenderingFrequency( Events::Hertz frequency ) 
					throw( SchedulingException ) ;

				
				/**
				 * Returns the number of engine ticks which correspond to 
				 * one rendering tick.
				 *
				 * @note It allows to take into account the round-off that 
				 * had to be performed by the setRenderingFrequency method.
				 *
				 */
				virtual Events::Period getRenderingTickCount() const throw() ;
			
				
				/**
				 * Sets the screenshot frequency, by evaluating the number 
				 * of engine ticks to be associated to one screenshot tick 
				 * for <b>frequency</b> to be met.
				 *
				 * @throw SchedulingException if requested frequency is 
				 * superior to engine frequency.
				 *
				 * @see DefaultSimulationFrequency
				 *
				 */
				virtual void setScreenshotFrequency( Events::Hertz frequency ) 
					throw( SchedulingException ) ;

				
				/**
				 * Returns the number of engine ticks which correspond to 
				 * one screenshot tick.
				 *
				 * @note It allows to take into account the round-off that 
				 * had to be performed by the setScreenshotFrequency method.
				 *
				 */
				virtual Events::Period getScreenshotTickCount() const throw() ;
				
				
				/**
				 * Sets the input polling frequency, by evaluating the 
				 * number of engine ticks to be associated to one input tick.
				 *
				 * @throw SchedulingException if requested frequency is 
				 * superior to engine frequency.
				 *
				 * @see DefaultInputFrequency
				 *
				 */
				virtual void setInputPollingFrequency( 
					Events::Hertz frequency ) throw( SchedulingException ) ;


				/**
				 * Returns the number of engine ticks which correspond to 
				 * one input tick.
				 *
				 * @note It allows to take into account the round-off that 
				 * had to be performed by the setInputPollingFrequency method.
				 *
				 */
				virtual Events::Period getInputPollingTickCount() 
					const throw() ;

				
				/**
				 * Sets the idle function, which is called whenever the
				 * scheduler detects a period of idle activity. 
				 *
				 * The callback can be useful for example to let the process
				 * yield some CPU time  so that the OS does not consider it 
				 * as too CPU-hungry, and does not take counter-measures 
				 * against it.
				 *
				 * @note Ensure that the maximum time spent in the idle 
				 * callback is negligible compared to the period 
				 * corresponding to the maximum engine frequency being set, 
				 * so that it does not result in a too heavy load which 
				 * would make the scheduler fail constantly because of 
				 * its idle function. 
				 *
				 * @param idleCallback the idle callback, which can be 
				 * null (0) to specify no idle callback is wanted.
				 *
				 * @param callbackData the user-supplied data pointer 
				 * that the idle callback will be given, if not null.
				 *
				 * @param callbackExpectedMaxDuration the maximum duration,
				 * in microseconds, expected for this idle call-back. Helps
				 * the scheduler enforcing its target frequency. If this value
				 * is null, the idle callback will be launched once immediately
				 * (during the call of this method), and the measured duration,
				 * increased of 20%, will be kept as upper bound.
				 *
				 * Any prior callback or callback data will be replaced 
				 * by the one specified.
				 *
				 */	
				virtual void setIdleCallback( 
					Ceylan::System::Callback idleCallback = 0 , 
					void * callbackData = 0,
					Ceylan::System::Microsecond 
						callbackExpectedMaxDuration = 0 ) throw() ;
				
				
				
				
				/**
				 * Returns the current actual engine tick.
				 *
				 */
				virtual Events::EngineTick getCurrentEngineTick() 
					const throw() ;
				
				
				/**
				 * Returns the current actual simulation tick.
				 *
				 */
				virtual Events::SimulationTick getCurrentSimulationTick() 
					const throw() ;
				
				
				/**
				 * Returns the current actual rendering tick.
				 *
				 */
				virtual Events::RenderingTick getCurrentRenderingTick() 
					const throw() ;
				
				
				/**
				 * Returns the current actual input tick.
				 *
				 */
				virtual Events::RenderingTick getCurrentInputTick() 
					const throw() ;
				
				
				/**
				 * Registers specified active object in scheduling slots. 
				 *
				 * Depending on the object defining or not a periodic 
				 * and/or programmed activations, the scheduler will store 
				 * these informations in order to be able to activate
				 * periodically and/or at specific simulation ticks this object.
				 * 
				 * @param toRegister the active object to register to the
				 * scheduler.
				 *
				 * @todo Maybe add an optional time offset so that the life 
				 * of registered object can start later (the object can 
				 * however emulate easily this behaviour), even if periodic.
				 *
				 */
				virtual void registerObject( ActiveObject & toRegister ) 
					throw() ;
						
				
				/**
				 * Launches the schedule loop, which behaves as a 
				 * specialized never-ending event loop.
				 *
				 * @throw SchedulingException, when propagated from active
				 * objects having problems.
				 * 
				 */
				virtual void schedule() throw( SchedulingException ) ;
							
								
				/**
				 * Requests the scheduler to stop its activities at the end 
				 * of current engine tick.
				 *
				 */
				virtual void stop() throw() ;
				
				 						
	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;



				// Static section.


	       	   /**
	         	* Returns the one and only one scheduler instance, that 
				* should be already available.
	         	*
	         	* The returned value is a reference and not a pointer, 
				* to avoid any abnormal deallocation by its users, that 
				* should never deallocate the scheduler.
	         	*
				* @throw SchedulingException if there is no scheduler 
				* available.
				*
	         	*/
	        	static Scheduler & GetExistingScheduler() 
					throw( SchedulingException ) ;

			
	       	   /**
	         	* Returns the one and only scheduler instance.
	         	* If no scheduler was already available, a new singletoned
				* instance is created.
				*
	         	* The returned value is a reference and not a pointer, 
				* to avoid any abnormal deallocation by its users, that 
				* should never deallocate the scheduler.
	         	*
	         	*/
	        	static Scheduler & GetScheduler() throw() ;


				/**
				 * Deletes the existing shared scheduler.
				 *
				 * @throw SchedulingException if not scheduler was available.
				 *
				 */
	       		static void DeleteExistingScheduler() 
					throw( SchedulingException ) ;
		
		
				/**
				 * Deletes the shared scheduler, if any.
				 *
				 */
	       		static void DeleteScheduler() throw() ;
		
	
					
				/**
				 * Determines the default engine tick duration.
				 *
				 * @note A value of 1000 implies a one millisecond scheduling,
				 * corresponding to a scheduling frequency of 1 kHz. 
				 * Most interactive applications should be satisfied with 
				 * this setting.
				 *
				 */	
				static const Ceylan::System::Microsecond
					DefaultEngineTickDuration = 1000 ;
					
					
				/**
				 * Determines the default frequency for logic (simulation).
				 *
				 * @note A value of 100 Hz implies a 10 ms scheduling.
				 *
				 */	
				static const Events::Hertz DefaultSimulationFrequency = 100 ;
					
					
				/**
				 * Determines the default frequency for rendering.
				 *
				 * @note A value of 40 Hz (40 frames per second) implies a 
				 * 25 ms scheduling.
				 *
				 */	
				static const Events::Hertz DefaultRenderingFrequency = 40 ;
					
					
				/**
				 * By default, generated screenshots will be saved on the
				 * purpose of being gathered by a movie with 25 images 
				 * per second.
				 *
				 * @see OSDL users'guide to know how to generate such a movie.
				 *
				 */
				static const Ceylan::Uint32 DefaultMovieFrameFrequency = 25 ;
				
				
				/**
				 * Determines the default frequency for rendering.
				 *
				 * @note A value of 20 Hz (20 input polling per second) 
				 * implies a 50 ms scheduling.
				 *
				 */	
				static const Events::Hertz DefaultInputFrequency = 20 ;
					

					
			protected:
			

				/**
				 * Constructs a new scheduler.
				 *
				 * @note Protected to ensure the singleton pattern is 
				 * respected.
				 *
				 */
				explicit Scheduler() throw() ;
				
				
				/**
				 * Virtual destructor, any existing renderer is deleted
				 * whereas no active object is destroyed.
				 *
				 * @note Protected to ensure the singleton pattern is respected.
				 *
				 */
				virtual ~Scheduler() throw() ;


				/**
				 * Actual scheduling method used for real-time and/or
				 * interactive applications.
				 *
				 * @note Best efforts means that if deadlines cannot be met,
				 * some simulation or rendering steps might be skipped.
				 *
				 */
				virtual void scheduleBestEffort() throw( SchedulingException ) ;
				

				/**
				 * Actual scheduling method used for batch applications, 
				 * which do not care about keeping any user-defined pace.
				 *
				 * @param pollInputs tells whether inputs should be polled
				 * nevertheless.
				 *
				 * @note The program takes all the time it needs to perform
				 * its tasks, so that no simulation nor rendering step is
				 * skipped, even if the whole processing takes 
				 * significantly longer. 
				 * Useful to generate screenshots.
				 *
				 */
				virtual void scheduleNoDeadline( bool pollInputs = true ) 
					throw( SchedulingException ) ;
				
				
				/**
				 * Returns the value that <b>should</b> be the engine tick, 
				 * if no scheduling skip had happened.
				 *
				 * This engine tick is computed from the current time since 
				 * the beginning of scheduling, which is obtained thanks 
				 * to a call to a precise actual time measurement minus 
				 * the offset corresponding to the starting time of the 
				 * scheduling.
				 *
				 * @see Ceylan::getPreciseTime, _scheduleStartingSecond, 
				 * _scheduleStartingMicrosecond
				 *
				 */
				virtual Events::EngineTick computeEngineTickFromCurrentTime()
					throw() ;


				/**
				 * Takes care of the simulation by making live relevant 
				 * active objects, when a simulation step is reached.
				 *
				 * @param current the simulation tick to perform, for 
				 * objects to be able to behave appropriatly, for example 
				 * for first behaviour approach (stateless objects).
				 *
				 */
				virtual void scheduleSimulation( 
					Events::SimulationTick current ) throw() ;


				/**
				 * Takes care of the simulation of programmed objects, by
				 * activating the relevant ones.
				 *
				 * @param current the simulation tick to perform, for 
				 * programmed objects to know to which programmed step
				 * this activation corresponds.
				 *
				 */
				virtual void scheduleProgrammedObjects( 
					Events::SimulationTick current ) throw() ;


				/**
				 * Takes care of the simulation of periodic objects, by
				 * activating the relevant ones.
				 *
				 * @param currentSimulationTick the simulation tick to 
				 * perform, for objects to be able to behave appropriately, 
				 * for example for first behaviour approach (stateless objects).
				 *
				 */
				virtual void schedulePeriodicObjects( 
					Events::SimulationTick currentSimulationTick ) throw() ;


				/**
				 * Triggers the rendering.
				 *
				 * @param currentRenderingTick the current rendering tick.
				 *
				 */
				virtual void scheduleRendering( 
					Events::RenderingTick currentRenderingTick ) throw() ;
					 
					 
				/**
				 * Triggers the input update.
				 *
				 * @param currentInputTick the current input tick.
				 *
				 */
				virtual void scheduleInput( Events::InputTick currentInputTick )
					 throw() ;


				/**
				 * Helper method to schedule (execute) in turn every active
				 * object of the list.
				 *
				 * @param currentSimulationTick the simulation tick to be 
				 * passed to the activated objects.
				 *
				 * @param objectList the list of the active objects to 
				 * execute in turn.
				 *
				 */
				virtual void scheduleActiveObjectList( 
					Events::RenderingTick currentSimulationTick, 
					ListOfActiveObjects & objectList ) throw() ;
					
					
				/**
				 * Overridable method called when this scheduler detects that
				 * processing power was not high enough to perform on time
				 * specified simulation step, which will have therefore to 
				 * be skipped.
				 *
				 * By default, this method calls on all active objects whose
				 * simulation step has been skipped their onSkip method.
				 *
				 * @note If an object is registered multiple times for a 
				 * given skipped simulation tick (ex : multiple periodic 
				 * and/or programmed activations), then its onSkip
				 * method will be called as many times as it should have 
				 * been activated.
				 *
				 * @note Skipping methods should be, on average, faster 
				 * than their normally scheduled counterparts (onNextTick),
				 * otherwise the scheduler will have no chance of keeping
				 * up with the real time.
				 *
				 */
				virtual void onSimulationSkipped( 
						Events::SimulationTick skipped ) 
					throw( SchedulingException ) ;
				

				/**
				 * Overridable method called when this scheduler detects 
				 * that processing power was not high enough to perform on 
				 * time specified rendering step, which will have therefore 
				 * to be skipped.
				 *
				 * By default, this method does nothing except complaining 
				 * in the warning channel.
				 *
				 * @note Skipping methods should be, on average, faster 
				 * than their normally scheduled counterparts, otherwise 
				 * the scheduler will have no chance of keeping up with the 
				 * real time.
				 *
				 */
				virtual void onRenderingSkipped( 
						Events::RenderingTick skipped ) 
					throw( SchedulingException ) ;
								

				/**
				 * Overridable method called when this scheduler detects
				 * that processing power was not high enough to perform 
				 * on time specified input step, which will have therefore 
				 * to be skipped.
				 *
				 * By default, this method does nothing at all. As only 
				 * the freshest inputs are generally useful, most 
				 * applications will leave this method as a do-nothing one. 
				 *
				 * @note Skipping methods should be, on average, faster 
				 * than their normally scheduled counterparts, otherwise 
				 * the scheduler will have no chance of keeping up with 
				 * the real time.
				 *	 
				 */
				virtual void onInputSkipped( Events::InputTick skipped ) 
					throw( SchedulingException ) ;
								
				
				/**
				 * Called whenever the application is deemed idle.
				 * Applies the idle behaviour, which can be user-defined
				 * (setIdleCallback), or otherwise which will default to
				 * micro-sleeps.
				 *
				 * @see setIdleCallback.
				 *
				 */
				virtual void onIdle() throw() ;


				/**
				 * Threshold for the level of delay bucket, above which the
				 * scheduler will be stopped automatically.
				 *
				 */
				static const Delay ShutdownBucketLevel ;
				
				
				/**
				 * Called whenever the scheduler does not succeed in keeping
				 * up the pace with what the application demanded : if the
				 * runtime resources are not high enough, then the scheduler
				 * may fail regularly and progressively fill up its delay 
				 * bucket.
				 *
				 * In this case this method - made to be overloaded - is called
				 * with the current bucket value. It can then take 
				 * countermeasures (preferably without taking too much time,
				 * otherwise it will be responsible for the scheduling failure).
				 *
				 * The default implementation will send notifications in the
				 * error log channel if below ShutdownBucketLevel, and will
				 * stop the scheduler if higher.
				 *
				 * @see ShutdownBucketLevel
				 *
				 */
				virtual void onScheduleFailure( Delay currentBucket ) throw() ;


				
				/**
				 * Adds a programmed activation for <b>objectToProgram</b> 
				 * at simulation tick <b>targetTick</b>. 
				 *
				 * @param objectToProgram the object whose activation is 
				 * being programmed.
				 *
				 * @param targetTick the simulation tick when specified 
				 * object should be activated.
				 *
				 */
				virtual void programTriggerFor( ActiveObject & objectToProgram,
					Events::SimulationTick targetTick ) throw() ;
				

				/**
				 * Returns the internal periodic slot for specified period.
				 *
				 * If there is no slot for this period, a new slot is created
				 * and returned.
				 *
				 */
				virtual PeriodicSlot & getPeriodicSlotFor( 
					Events::Period period ) throw() ;
				


				// Variable section.
				
				
				/**
				 * Tells whether screenshot mode is activated (default :
				 * deactivated).
				 *
				 * @note Screenshot mode is also known as 'no deadline' 
				 * mode, as opposed to 'best effort' mode, which is in soft 
				 * real time.
				 *
				 */
				bool _screenshotMode ;		
				
				
				/**
				 * Records which prefix should be used to name numbered
				 * screenshot files. 
				 * 
				 * The purpose of these files is mainly to be gathered in a
				 * movie (ex : MPEG).
				 *
				 */
				std::string _frameFilenamePrefix ;		

				
				/**
				 * Records the user-defined screenshot frequency, which 
				 * is the targeted number of frames per second.
				 *
				 * Useful if the corresponding period had to be computed 
				 * again after the engine tick duration changed.
				 *
				 */
				Events::Hertz _desiredScreenshotFrequency ;
				
				
				/**
				 * Stores the period corresponding to the planned 
				 * framerate for the movie that would be generated, if
				 * screenshot mode is activated.		
				 *
				 * Movie periods, which are the duration between two 
				 * successive screenshots, are expressed in engine ticks.
				 *
				 */
				 Events::Period _screenshotPeriod ;		
				

/* 
 * Takes care of the awful issue of Windows DLL with templates.
 *
 * @see Ceylan's developer guide and README-build-for-windows.txt 
 * to understand it, and to be aware of the associated risks. 
 * 
 */
#pragma warning( push )
#pragma warning( disable : 4251 )
			
			
				/**
				 * A list containing all available periodic slots, sorted
				 * by increasing periods.
				 *
				 * @note This is a list of pointers and not of PeriodicSlot
				 * instances since adding a new slot would otherwise result 
				 * in a call to a copy constructor we do not want to define.
				 *
				 */	
				std::list<PeriodicSlot *> _periodicSlots ; 
				
				
				/**
				 * Map for programmed activations. 
				 *
				 * Each key of the map is a specific simulation tick, 
				 * whereas its value is a list of all objects having 
				 * requested to be scheduled at this particular simulation 
				 * tick.
				 *
				 * A map (namely, a hash table) has been prefered to a 
				 * list sorted in chronological order, since with a map, 
				 * even if reading whether there are programmed triggers 
				 * next simulation steps is slower (0(log n) instead of 
				 * 0(1)), insertion times are much faster, (0(n.log n) 
				 * instead of 0(n)). 
				 *
				 * As there may be numerous programmed triggers (ex : for
				 * playing sound delayed by distance), the map might be
				 * helpful here. 
				 *
				 */
				std::map<Events::SimulationTick, ListOfActiveObjects>
					_programmedActivated ;		
				
#pragma warning( pop ) 

				
				/**
				 * Defines the duration in microseconds of an elementary
				 * scheduler tick. 
				 *
				 */
				Ceylan::System::Microsecond _engineTickDuration ;
				
				
				/**
				 * Multiplicative factor that allows to convert seconds into  
				 * engine ticks.
				 *
				 * @note Useful to avoid overflow with conversions.
				 *
				 */
				Ceylan::Uint32 _secondToEngineTick ;
				
				
				/**
				 * Records the current engine tick.
				 *
				 * Useful to determine how much time have elapsed since 
				 * last schedule action.
				 *
				 * @note Under normal circumstances (not too heavy load), 
				 * the current engine tick multiplied by the engine tick
				 * duration should correspond to the current user time, 
				 * with the offset of the library initialization time t0 :
				 * (current time) - t0 = 
				 *  ( current engine tick ) * ( engine tick duration )
				 *
				 * The scheduler has for purpose to enforce this property, 
				 * but insufficient processing resources can make the engine
				 * time drift : hence one should not rely on this property 
				 * being verified. 
				 *
				 * Therefore, this data member is the only one that is
				 * authoritative (the only actual value).
				 *
				 */
				Events::EngineTick _currentEngineTick ;
				
				
				/**
				 * Records the current simulation tick.
				 *
				 * Useful to determine how much time have elapsed since 
				 * last simulation action.
				 *
				 * @note Under ideal circumstances (not too heavy load), 
				 * the current simulation tick multiplied by the simulation 
				 * tick duration should correspond to the current user time,
				 * with the offset of the library initialization time t0 :
				 * (current time) - t0 = 
				 *	( current simulation tick ) * ( simulation tick duration )
				 *
				 * The scheduler has for purpose to enforce this property, 
				 * but insufficient processing resources can make the 
				 * simulation time drift : one should not rely on this 
				 * property being verified. 
				 * Therefore, this data member is the only one that
				 * is authoritative (the only actual value).
				 *
				 */
				Events::SimulationTick _currentSimulationTick ;
				
				
				/**
				 * Records the current rendering tick.
				 *
				 * Useful to determine how much time have elapsed since 
				 * last rendering action.
				 *
				 * @note Under idel circumstances (not too heavy load), 
				 * the current rendering tick multiplied by the rendering 
				 * tick duration should correspond to the current user time,
				 * with the offset of the library initialization time t0:
				 * (current time) - t0 = 
				 *  ( current rendering tick ) * ( engine rendering duration )
				 *
				 * The scheduler has for purpose to enforce this property, 
				 * but insufficient processing resources can make the 
				 * rendering time drift : one should not rely on this property
				 * being verified. 
				 * Therefore, this data member is the only one that
				 * is authoritative (the only actual value).
				 *
				 */
				Events::RenderingTick _currentRenderingTick ;
				
				
				/**
				 * Records the current input tick.
				 *
				 * Useful to determine how much time have elapsed since 
				 * last input polling.
				 *
				 * @note Under ideal circumstances (not too heavy load), 
				 * the current input tick multiplied by the input tick 
				 * duration should correspond to the current user time, 
				 * with the offset of the library initialization time t0:
				 * (current time) - t0 
				 *  = ( current input tick ) * ( engine input duration )
				 *
				 * The scheduler has for purpose to enforce this property, 
				 * but insufficient processing resources can make the 
				 * input time drift : one should not rely on this property 
				 * being verified. 
				 * Therefore, this data member is the only one that
				 * is authoritative (the only actual value).
				 *
				 */
				Events::RenderingTick _currentInputTick ;
				
				
				/**
				 * Duration between two successive simulation steps, 
				 * expressed in engine ticks.
				 *
				 */
				Events::Period _simulationPeriod ;
				
				
				/**
				 * Duration between two successive rendering steps, 
				 * expressed in engine ticks.
				 *
				 */
				Events::Period _renderingPeriod ;
				
				
				/**
				 * Duration between two successive input steps, 
				 * expressed in engine ticks.
				 *
				 */
				Events::Period _inputPeriod ;
				
				
				/**
				 * Records the user-defined simulation frequency.
				 *
				 * Useful if the corresponding period had to be computed 
				 * again after the engine tick duration changed.
				 *
				 */
				Events::Hertz _desiredSimulationFrequency ;
				
				
				/**
				 * Records the user-defined rendering frequency.
				 *
				 * Useful if the corresponding period had to be computed 
				 * again after the engine tick duration changed.
				 *
				 */
				Events::Hertz _desiredRenderingFrequency ;
		
		
				/**
				 * Records the user-defined input frequency.
				 *
				 * Useful if the corresponding period had to be computed 
				 * again after the engine tick duration changed.
				 *
				 */
				Events::Hertz _desiredInputFrequency ;



				
				// Idle callback section.
				
				

				/**
				 * The idle callback, if any, to be called by the scheduler.
				 *
				 */
				Ceylan::System::Callback _idleCallback ;
						
								
				/**
				 * The data, if any, to provide to the idle callback.
				 *
				 */
				void * _idleCallbackData ;				
		
		
				/**
				 * An estimated upper bound of the duration of current idle
				 * callback. 
				 *
				 * Helps the scheduler to respect its expected pace.
				 *
				 */
				Ceylan::System::Microsecond _idleCallbackMaxDuration ;
		
		
				/**
				 * Count the number of idle calls made during the current
				 * scheduler session.
				 *
				 */
				Ceylan::Uint32 _idleCallsCount ;
		
		
		
				/// Tells whether the scheduler has been requested to stop.
				bool _stopRequested ;
		
		
				/**
				 * Stores the second when scheduling started.
				 *
				 */
				Ceylan::System::Second _scheduleStartingSecond ;
				
				
				/**
				 * Stores the microsecond when scheduling started.
				 *
				 */
				Ceylan::System::Microsecond _scheduleStartingMicrosecond ;		
		
		
		
				/**
				 * Records how many simulation ticks were recovered since 
				 * the scheduler was started.
				 *
				 */
				Ceylan::Uint32 _recoveredSimulationTicks ;
		
		
				/**
				 * Records how many simulation ticks were missed since 
				 * the scheduler was started.
				 *
				 */
				Ceylan::Uint32 _missedSimulationTicks ;
		
		
		
				/**
				 * Records how many rendering ticks were recovered since 
				 * the scheduler was started.
				 *
				 */
				Ceylan::Uint32 _recoveredRenderingTicks ;
		
		
				/**
				 * Records how many rendering ticks were missed since 
				 * the scheduler was started.
				 *
				 */
				Ceylan::Uint32 _missedRenderingTicks ;
		
		
		
				/**
				 * Records how many input ticks were recovered since 
				 * the scheduler was started.
				 *
				 */
				Ceylan::Uint32 _recoveredInputPollingTicks ;
		
		
				/**
				 * Records how many input ticks were missed since 
				 * the scheduler was started.
				 *
				 */
				Ceylan::Uint32 _missedInputPollingTicks ;
		
		
				/**
				 * Records how many schedule failures were reported.
				 *
				 * Happens when the delay bucket reaches its first threashold.
				 *
				 */
				Ceylan::Uint32 _scheduleFailureCount ;
		

				/**
				 * References the events module, if necessary.
				 *
				 * @note Used mostly when the scheduler has to poll 
				 * inputs during input tick.
				 *
				 */
				Events::EventsModule * _eventsModule ;
				
		
				/**
				 * References the renderer to be used, if any.
				 *
				 */		
				Rendering::Renderer * _renderer ;
		
		
				/**
				 * References the video module, if necessary.
				 *
				 * @note Used mostly when the scheduler is not linked to a
				 * renderer. 
				 * In this case, at each rendering tick the scheduler 
				 * calls the redraw method of the video module. 
				 * It allows scheduled objects to manage on their own the
				 * display, instead of using a full-blown dedicated renderer.
				 *
				 */
				Video::VideoModule * _videoModule ;
				
				
				 
			private:
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Scheduler( const Scheduler & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Scheduler & operator = ( const Scheduler & source ) throw() ;
							
				
				/// The internal single instance of scheduler, if any.
				static Scheduler * _internalScheduler ;
			
				
		} ;

	}

}


#endif // OSDL_SCHEDULER_H_


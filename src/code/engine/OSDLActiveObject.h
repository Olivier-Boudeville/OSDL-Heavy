#ifndef OSDL_ACTIVE_OBJECT_H_
#define OSDL_ACTIVE_OBJECT_H_



#include "OSDLPeriodicSlot.h"   // for Period, SchedulingException

#include "OSDLEvents.h"         // for SimulationTick

#include "Ceylan.h"             // for VerbosityLevels, inheritance

#include <string>
#include <list>



namespace OSDL
{


	namespace Engine 
	{
	
			
		/**
		 * Active objects are dynamic objects, i.e. which needs some CPU time, since they live. 
		 * They are therefore taken into account by our scheduler, which applies
		 * simple heuristics so that each active object is granted with the processing power it 
		 * requested, on a best-effort basis, according to an appropriate timing.
		 *
		 * These objects can be activated on a regular basis, a period being specified for them,
		 * or on specific simulation steps. In this case, a list of simulation steps is 
		 * to be specified, absolutely or relatively to the current simulation step. 
		 * An object can be activated on a periodic basis, as a programmed object, or both.
		 *
		 * An active object can be implemented according to one of the two classic approaches. 
		 * For a given simulation time n, the object behaviour can be computed :
		 *  1. only from the current simulation time, n
		 *  2. from the state of the object as it was on simulation time (n-1), and maybe other
		 * parameters
		 *
		 * The best approach is the first, since a failure to process time n can be corrected in
		 * time (n+1), whereas the second approach demands all simulation steps to be processed and
		 * therefore would not allow the scheduler in the hurry to skip any step. However, not all
		 * behaviours could follow the first approach. 
		 *
		 * An example of a successful first approach would be a windmill turning at a constant 
		 * rate, therefore registered as an active object with periodic scheduling. 
		 * The second approach would define the new angle of its wheel as the previous angle, plus
		 * the angular speed times the duration between two simulation ticks.
		 * The recommended first approach would be to compute the wheel angle from its initial
		 * time and position, and turn it from the angular speed times the simulation time elapsed
		 * since the birth of this active object. Alas, not all behaviours can follow the first
		 * approach. For the corresponding unfortunate objects, there is however a last chance to
		 * correct a simulation skip : the scheduler will call their onSkip method, which may be
		 * used to overcome such scheduling hiccups.    
		 *
		 * @note Active objects have to be explicitly registered to the scheduler, so that their
		 * actual birth can happen at any time after their creation. Objects will start to live as
		 * soon as they are registered to the scheduler. The simpler way of registering an active
		 * object to the already-existing scheduler is : 
		 * <code>Scheduler::GetExistingScheduler().registerObject( * this )</code>.
		 *
		 * @see Scheduler::registerAsPeriodic, Scheduler::registerAsProgrammed
		 * 
		 */
		class ActiveObject : public Ceylan::TextDisplayable
		{
		
		
			public:
			
			
				/**
				 * Constructor of a periodically activated object. 
				 *
				 * @param period tells how many simulation steps are to be waited by the scheduler
				 * until the active object is activated again. The period must not be null.
				 *
				 * @param policy allows to choose a scheduling policy, according to the quality of
				 * service this active object requires.
				 *
				 * @param weight evaluates how much processing power an activation of this object
				 * is to cost, on average. This helps the scheduler doing its job.
				 *
				 * @note Do not mix up this constructor with the one for programmed objects, since
				 * their arguments are almost the same.
				 *
				 */
				explicit ActiveObject( Events::Period period = 1, 
					ObjectSchedulingPolicy policy = relaxed, Weight weight = 1 ) throw() ;
				
				
				/**
				 * Constructor of a object activated only on specifically programmed
				 * simulation steps. 
				 *
				 *
				 * @param triggeringStates is the list of simulation ticks where the activation
				 * should occur. The object does not take ownership of this list, it will make
				 * a copy of it.
				 *
				 * @param absolutlyDefined tells if the specified steps are defined absolutly,
				 * or relatively to the current simulation step.
				 *
				 * @param policy allows to choose a scheduling policy, according to the quality of
				 * service this active object requires.
				 *
				 * @param weight evaluates how much processing power an activation of this object
				 * is to cost, on average. This helps the scheduler doing its job.
				 *
				 * @note Do not mix up this constructor with the one for periodic objects, since
				 * their arguments are almost the same.
				 *
				 */
				explicit ActiveObject( 
					const std::list<Events::SimulationTick> & triggeringStates, 
					bool absolutlyDefined = true, ObjectSchedulingPolicy policy = relaxed, 
					Weight weight = 1 ) throw() ;
				
				
				/**
				 * Constructor of a object activated only on a specific simulation step. 
				 *
				 * @param triggerTick is the simulation step when this object will be
				 * activated.
				 *
				 * @param absolutlyDefined tells if the specified steps are defined absolutly,
				 * or relatively to the current simulation step. No default value can be given
				 * for that parameter, since there would be an ambiguity with first constructor
				 * on any call with only one argument.
				 *
				 * @param policy allows to choose a scheduling policy, according to the quality of
				 * service this active object requires.
				 *
				 * @param weight evaluates how much processing power an activation of this object
				 * is to cost, on average. This helps the scheduler doing its job.
				 *
				 * @note Do not mix up this constructor with the one for programmed objects, since
				 * their arguments are almost the same.
				 *						 
				 */
				explicit ActiveObject( Events::SimulationTick triggerTick, 
					bool absolutlyDefined, ObjectSchedulingPolicy policy = relaxed, 
					Weight weight = 1 ) throw() ;
				
				
				/**
				 * Virtual destructor, which performs automatic unsubscribing of this object from
				 * the scheduler.
				 *
				 */
				virtual ~ActiveObject() throw() ;

				
				/**
				 * Returns the scheduling policy for this active object.
				 *
				 */
				virtual ObjectSchedulingPolicy getPolicy() const throw() ;
				
				
				/**
				 * Returns the average processing need for each activation of this object.
				 *
				 * @note The greater the weight is, the heavier processing should be. 
				 *
				 */
				virtual Weight getWeight() const throw() ;
				
				
				/**
				 * Returns the period for this active object.
				 *
				 * @note A null (0) value denotes a pure programmed active object.
				 *
				 */
				virtual Events::Period getPeriod() const throw() ;
				
				
				/**
				 * Sets a period for this object, which will be periodically scheduled accordingly.
				 *
				 * @param newPeriod the new period of activation for this object, expressed in
				 * simulation ticks.
				 *
				 * @note An object will be considered as a periodically-activated one iff its
				 * period is non-zero. Zero means not activated on a regular basis thanks to
				 * periodic slots.
				 *
				 */
				virtual void setPeriod( Events::Period newPeriod ) throw() ;
				 
				 
				/**
				 * Sets the period of this object so that it gets activated on specified frequency,
				 * making this object periodically activated if it was not, or changing its already
				 * defined period otherwise.
				 *
				 * @param newFrequency the desired frequency, expressed in Hertz.
				 *
				 * @return The actual chosen frequency, which may differ a bit from specified one
				 * since it depends on the simulation tick granularity. Notably, the obtained 
				 * frequency cannot exceed the specified simulation frequency for the scheduler.
				 * The actual frequency is nevertheless chosen as the nearest possible frequency.
				 *
				 * @note To be able to compute the actual period, expressed in simulation ticks,
				 * from the specified frequency, the scheduler that will be later used for this 
				 * object is needed.
				 *
				 * @throw SchedulingException if there was no existing scheduler available.
				 *
				 */ 
				virtual Events::Hertz setFrequency( Events::Hertz newFrequency ) 
					throw( SchedulingException ) ; 
					
					
				/**
				 * Tells whether this object has programmed activations set.
				 *
				 *
				 */
				virtual bool hasProgrammedActivations() const throw() ;
				

				/**
				 * Tells whether programmed activations are defined absolutely (from the origin
				 * of simulation times, i.e. the scheduler start) or relatively to the simulation
				 * tick of the object registering to the scheduler.
				 *
				 *
				 */
				bool areProgrammedActivationsAbsolute() const throw() ;
				
				
				/**
				 * Defines whether the programmed activations for this object are to be defined
				 * absolutely (from the origin of simulation times, i.e. the scheduler start) 
				 * or relatively to the simulation tick of the object registering to the scheduler.
				 *
				 */
				void absoluteProgrammedActivationsWanted( bool on ) throw() ;
				 
				
				/**
				 * Returns a reference to the internal list of programmed activations of this 
				 * object.
				 *
				 * @throw SchedulingException if the object had no programmed activations.
				 *
				 * @see hasProgrammedActivations to check for programmed activations before.
				 *
				 */
				virtual const std::list<Events::SimulationTick> & getProgrammedActivations() 
					const throw( SchedulingException ) ;
				
								  
				/**
				 * Sets the list of programmed activations for this object to specified value.
				 *
				 * @param newActivationsList the list of simulation ticks which will be copied
				 * internally.
				 *
				 * @note Any previously defined activations are lost.
				 *
				 * @see addProgrammedActivations
				 *
				 */
				virtual void setProgrammedActivations( 
					const std::list<Events::SimulationTick> & newActivationsList ) throw() ;
				
				
				/**
				 * Adds the specified list of simulation ticks to the list of activation ticks
				 * for this object.
				 *
				 * @param additionalActivationsList the list of additional simulation ticks which
				 * will activate the object too.
				 *
				 * @note The method will not ensure that each tick is present at most one time in
				 * the object activation list : one object can by programmed multiple times in a 
				 * single simulation tick.
				 *
				 */
				virtual void addProgrammedActivations( 
					const std::list<Events::SimulationTick> & additionalActivationsList ) throw() ;
				
				
				/**
				 * Ensures that the programmed ticks for this object are unique, which means that,
				 * after this method call, this object will not be scheduled multiple times for 
				 * any given simulation tick.
				 *
				 * @note One can wish an object is scheduled multiple times for a given simulation
				 * tick, so having unique elements in programmed lists is an option.
				 *
				 */
				virtual void setUniqueProgrammedTicks() throw() ;
				
				
				/**
				 * Returns the birth time, expressed in simulation ticks, of this active object.
				 *
				 *
				 */				
				virtual Events::SimulationTick getBirthTime() const throw() ;
				
				
				/**
				 * Sets the birth time, expressed in simulation ticks, of this active object.
				 *
				 * @note Called by the schedule, when registering this active object.
				 *
				 */				
				virtual void setBirthTime( Events::SimulationTick birthSimulationTick ) throw() ;
				
				
				/**
				 * Pure virtual method, which is called when this active object gets
				 * activated on behalf of the scheduler.
				 *
				 * @note This is where the objects are to live actually : their time slice is
				 * spent here.
				 *
				 * @note If, for any reason, there is a simulation skip, this method may not be
				 * called for some objects. Refer to the onSkip method, which would be called 
				 * instead (and, sadly, later). 
				 * 
				 * @see onSkip
				 *
				 */
				virtual void onActivation( Events::SimulationTick newTick ) throw() = 0 ;

				
				/**
				 * Called whenever the scheduler had to skip a period of activation for this 
				 * object. 
				 *
				 * Default behaviour for this method is : do nothing except complain in
				 * the warning channel.
				 *
				 * @param skippedStep tells which step was skipped.
				 *
				 * @throw SchedulingException if this active object cannot stand skipped 
				 * stimulation steps.
				 *
				 * @note Some objects may call the <b>onActivation</b> method for the skipped
				 * tick, to emulate the behaviour they could have if they had been scheduled at 
				 * the right time.
				 *
				 */
				virtual void onSkip( Events::SimulationTick skippedStep ) 
					throw( SchedulingException ) ;


				/**
				 * Called whenever the scheduler was unable to activate this object. This can happen
				 * for example when an absolute simulation step is specified whereas the current
				 * time is already in its future.
				 *
				 * Default behaviour for this method is : throw a SchedulingException.
				 *
				 * @param missedStep tells which step is out of range.
				 *
				 * @throw SchedulingException if this active object should not stand activation
				 * failures.
				 *
				 */				
				virtual void onImpossibleActivation( Events::SimulationTick missedStep ) 
					throw( SchedulingException ) ;
				
		
	            /**
	             * Returns an user-friendly description of the state of this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;
					
				
					
			protected:		
		
				
				/**
				 * Returns this object local time, as if the origin of time was this object 
				 * creation.
				 *
				 */
				Events::SimulationTick getLocalTime() const throw() ;


				
				/**
				 * Determines the scheduling policy of this object.
				 *
				 */
				ObjectSchedulingPolicy _policy ;
				
				
				/**
				 * Describe the average processing need for each activation of this object.
				 * The greater the weight is, the heavier processing should be. 
				 *
				 */
				 Weight _weight ;
				 
				
				/**
				 * Tells what is the requested scheduling period for this object.
				 *
				 * @note A null (0) value denotes a programmed active object.
				 *
				 */
				Events::Period _period ;
				
				
				/**
				 * Records the list of programmed simulation ticks that will be used by the  
				 * scheduler to activate this object.
				 *
				 * @note No particular order is to be enforced.
				 *
				 */
				std::list<Events::SimulationTick> * _programmedTriggerTicks ;
				 
				 
				/**
				 * Tells whether the programmed trigger ticks should be understood as absolute
				 * (i.e. if their simulation ticks should be taken litteraly), or as relative
				 * (defined as an offset from the moment the object is registered).
				 *
				 * Is true by default.
				 *
				 */
				bool _absoluteTriggers ; 
		
				 
				/**
				 * The date of birth of this object, expressed in simulation time.
				 *
				 * @note the date of birth is neither the creation time of this active object
				 * (since it could be created even before the scheduler exists), nor the time where
				 * this object is scheduled for the first time, but the time it was registered 
				 * to the scheduler.
				 *
				 */
				Events::SimulationTick _birthTime ;
				
				
		} ;

	}

}


#endif // OSDL_ACTIVE_OBJECT_H_

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


#ifndef OSDL_ACTIVE_OBJECT_H_
#define OSDL_ACTIVE_OBJECT_H_



#include "OSDLEngineCommon.h"   // for ObjectSchedulingPolicy, Weight, etc. 

#include "Ceylan.h"             // for VerbosityLevels, inheritance

#include <string>



namespace OSDL
{


	namespace Engine 
	{
	
	
			
		/**
		 * Active objects are dynamic objects, i.e. objects which need some 
		 * CPU time, since they "live", i.e. have a spontaneous behaviour.
		 * 
		 * Once registered, they are therefore taken into account by our
		 * scheduler, which applies simple heuristics so that each active
		 * object is granted with the processing power it requested, on a
		 * best-effort basis, according to an appropriate timing.
		 *
		 * The ActiveObject class is abstract, it has two main child classes,
		 * one for objects whose activation must be periodic
		 * (PeriodicalActiveObject) and one for objects that rely on 
		 * specifically programmed activation ticks (ProgrammedActiveObject).
		 *
		 * An active object can be implemented according to one of two
		 * classic approaches regarding time and state management.
		 *
		 * Indeed, for a given simulation time n, the object behaviour can be
		 * computed:
		 *  1. only from the current simulation time, n
		 *  2. from the state of the object as it was on simulation time (n-1),
		 * and maybe other parameters
		 *
		 * The best approach is the first, since a failure to process time n 
		 * can be fully corrected in time (n+1), whereas the second approach 
		 * demands all simulation ticks to be processed and therefore would 
		 * not allow the scheduler, if in the hurry, to skip any tick. 
		 * However, not all behaviours can respect the first approach. 
		 *
		 * An example of a successful first approach would be a windmill 
		 * turning at a constant rate, therefore registered as an active
		 * object with periodic scheduling. The trajectory of its wings would
		 * depend only on the simulation time, and therefore would not be
		 * affected by past scheduling skips. 
		 * The wheel angle would be computed only from its initial time and
		 * position, just adding to the initial angle the angular speed 
		 * times the simulation time elapsed since the birth of this active
		 * object, i.e. the computing for simulation time n depends only of
		 * n and not on the computings made the previous ticks.
		 *
		 * The second approach would define the new angle of its wheel 
		 * as the previous angle, plus the angular speed times the duration
		 * between two simulation ticks. If a simulation tick had to be skipped
		 * then the windmill would turn slower than requested.
		 *
		 * Unfortunately not all behaviours can follow the first approach.
		 *
		 * For the corresponding objects, there is however a last
		 * chance to correct a simulation skip: the scheduler will call their
		 * onSkip method, which may be used to overcome such scheduling hiccups.
		 *
		 * @note Active objects have to be explicitly registered to the
		 * scheduler, so that their actual birth can happen at any time 
		 * after their creation: active objects will start to live as soon as
		 * they are registered  to the scheduler and that scheduler runs 
		 * (these two events can happen in any order). 
		 *
		 * @note When the scheduler is running, the only way of deleting
		 * a scheduled object is to have it removed by another object, as
		 * they are the only code to be executed (and input or rendering tasks
		 * are not expected to remove active objects).
		 *
		 */
		class OSDL_DLL ActiveObject: public Ceylan::TextDisplayable
		{
		
		
			public:
			
			
				/**
				 * Constructor of an active object. 
				 *
				 * @param policy allows to choose a scheduling policy, 
				 * according to the quality of service this active object
				 * requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this object is to cost, on average. 
				 * This helps the scheduler doing its job.
				 *
				 */
				explicit ActiveObject( ObjectSchedulingPolicy policy = relaxed, 
					Weight weight = 1 ) throw() ;
								
				
				/**
				 * Virtual destructor.
				 *
				 * @note The object is expected to be already unsubscribed
				 * from the scheduler (probably by the destructor of its actual
				 * child class), otherwise an error is logged.
				 *
				 */
				virtual ~ActiveObject() throw() ;




				// Registering section.
				
				
				/**
				 * Registers this active object to the supposedly already
				 * existing scheduler.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				virtual void registerToScheduler() 
					throw( SchedulingException ) = 0 ;
				
				
				/**
				 * Unregisters this active object from the existing scheduler
				 * to whom it was supposedly registered.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				virtual void unregisterFromScheduler() 
					throw( SchedulingException ) = 0 ;
				
				
				
				
				// Settings section.

				
				/**
				 * Returns the scheduling policy for this active object.
				 *
				 */
				virtual ObjectSchedulingPolicy getPolicy() const throw() ;
				
				
				
				/**
				 * Returns the average processing need for each activation 
				 * of this object.
				 *
				 * @note The greater the weight is, the heavier processing
				 * should be. 
				 *
				 */
				virtual Weight getWeight() const throw() ;
				
				
				
				
				/**
				 * Returns the birth time, expressed in simulation ticks, 
				 * of this active object.
				 *
				 */				
				virtual Events::SimulationTick getBirthTick() const throw() ;
				
				
				
				/**
				 * Sets the birth time, expressed in simulation ticks, of 
				 * this active object.
				 *
				 * @note Called by the scheduler, when registering this 
				 * active object.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */				
				virtual void setBirthTick( 
						Events::SimulationTick currentSimulationTick ) 
					throw( SchedulingException ) ;

				
				
				/**
				 * Pure virtual method, which is called when this active 
				 * object gets activated on behalf of the scheduler.
				 *
				 * @note This is where the objects are to live actually: 
				 * their time slice is to be spent here.
				 *
				 * @note If, for any reason, there is a simulation skip, 
				 * this method may not be called for some objects. 
				 * Refer to the onSkip method, which would be called instead
				 * (and, sadly, later). 
				 * 
				 * @see onSkip.
				 *
				 */
				virtual void onActivation( 
					Events::SimulationTick newTick ) throw() = 0 ;


				
				/**
				 * Called whenever the scheduler had to skip a period of
				 * activation for this object. 
				 *
				 * Default behaviour for this method is: do nothing except
				 * complain in the warning channel.
				 *
				 * @param skippedTick tells which tick was skipped.
				 *
				 * @throw SchedulingException if this active object cannot 
				 * stand skipped stimulation ticks.
				 *
				 * @note Some objects may call the <b>onActivation</b> 
				 * method for the skipped tick, to emulate the behaviour 
				 * they would have had if they could have been scheduled at
				 * the right time.
				 *
				 */
				virtual void onSkip( Events::SimulationTick skippedTick ) 
					throw( SchedulingException ) ;


				/**
				 * Called whenever the scheduler was unable to activate 
				 * this object. 
				 *
				 * This can happen for example when an absolute simulation 
				 * tick is specified whereas the current time is already 
				 * in its future.
				 *
				 * Default behaviour for this method is: throw a
				 * SchedulingException.
				 *
				 * @param missedTick tells which tick is out of range.
				 *
				 * @throw SchedulingException if this active object 
				 * should not stand activation failures.
				 *
				 */				
				virtual void onImpossibleActivation( 
						Events::SimulationTick missedTick ) 
					throw( SchedulingException ) ;
				
				
				/*
				 * No onRegistering abstract method declared here, as 
				 * depending on the child class it takes different 
				 * parameters.
				 *
				 */
				 
		
	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall 
				 * settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;
					
				
					
			protected:		
		
				
				/**
				 * Returns this object local time, as if the origin of 
				 * time was this object creation.
				 *
				 */
				Events::SimulationTick getLocalTime() const throw() ;


				
				/**
				 * Determines the scheduling policy of this object.
				 *
				 */
				ObjectSchedulingPolicy _policy ;
				
				
				/**
				 * Describe the average processing need for each 
				 * activation of this object.
				 *
				 * The greater the weight is, the heavier processing should be. 
				 *
				 */
				 Weight _weight ;
				 
								
				/**
				 * Tells whether this active object has already been 
				 * registered to a scheduler.
				 *
				 */
				bool _registered ;
				
				 
				/**
				 * The date of birth of this object, expressed in simulation
				 * time.
				 *
				 * @note the date of birth corresponds to the first future
				 * simulation tick this object is known for the running 
				 * scheduler (either the scheduler start-up, if this object
				 * was registered before), or the simulation tick at which
				 * this object was dynamically registered to the scheduler.
				 *
				 */
				Events::SimulationTick _birthTick ;
				
				
		} ;

	}

}


#endif // OSDL_ACTIVE_OBJECT_H_


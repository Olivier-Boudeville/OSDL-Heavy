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


#ifndef OSDL_PROGRAMMED_ACTIVE_OBJECT_H_
#define OSDL_PROGRAMMED_ACTIVE_OBJECT_H_



#include "OSDLActiveObject.h"   // for inheritance
#include "OSDLEngineCommon.h"   // for ObjectSchedulingPolicy, Weight, etc. 


#include <string>
#include <list>




namespace OSDL
{



	namespace Engine 
	{
	
	
		
			
		/**
		 * Programmed active objects are objects that are to be scheduled in
		 * at specific simulation ticks, as opposed to periodically activated
		 * ones.
		 * 
		 * Thus programmed objects define a list of simulation ticks at
		 * which they should be scheduled.
		 * These ticks can be specified as absolute times or as offsets, i.e. 
		 * relatively to the simulation tick they were registered. 
		 *
		 * The birth time of an active object is the first time when it is
		 * registered and when the scheduler runs (this can happen in any 
		 * order). For example it is not the tick corresponding to the first
		 * time this programmed object is activated.
		 *
		 * @see also the PeriodicalActiveObject class.
		 *
		 */
		class OSDL_DLL ProgrammedActiveObject: public ActiveObject
		{
		
		
			public:
			
							
				
				/**
				 * Constructor of an object activated only on specifically
				 * programmed simulation ticks. 
				 *
				 * @param activationTicks is the list of simulation ticks 
				 * when this object should be activated.
				 * This object does not take ownership of this list, it 
				 * will make a copy of it.
				 *
				 * @param absolutelyDefined tells whether the specified ticks
				 * are defined absolutely, or relatively to the simulation 
				 * tick at which this object will be registered to the
				 * scheduler.
				 *
				 * @param autoRegister this object will automatically
				 * register to the scheduler iff true.
				 *
				 * @param policy allows to choose a scheduling policy, 
				 * according to the quality of service this active object
				 * requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this object is to cost on average. 
				 * This helps the scheduler doing its job.
				 *
				 * @throw SchedulingException if the construction failed.
				 *
				 */
				explicit ProgrammedActiveObject( 
					const SimulationTickList & activationTicks, 
					bool absolutelyDefined = true, 
					bool autoRegister = true,
					ObjectSchedulingPolicy policy = relaxed, 
					Weight weight = 1 ) ;
				
				
				
				/**
				 * Constructor of an object activated only on a specific
				 * simulation tick (one-shot, as opposed to a list). 
				 *
				 * @param activationTick is the simulation tick when this 
				 * object should be activated.
				 *
				 * @param absolutelyDefined tells if the specified tick is
				 * defined absolutely, or relatively to the current simulation
				 * tick. 
				 *
				 * @param autoRegister this object will automatically
				 * register to the scheduler iff true.
				 *
				 * @param policy allows to choose a scheduling policy, 
				 * according to the quality of service this active object
				 * requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this object is to cost on average. 
				 * This helps the scheduler doing its job.
				 *
				 * @throw SchedulingException if the construction failed.
				 *
				 */
				explicit ProgrammedActiveObject( 
					Events::SimulationTick activationTick, 
					bool absolutelyDefined = true, 
					bool autoRegister = false,
					ObjectSchedulingPolicy policy = relaxed, 
					Weight weight = 1 ) ;
				
				
				
				/**
				 * Virtual destructor.
				 *
				 * @note This object will automatically unsubscribe
				 * from the scheduler if needed.
				 *
				 */
				virtual ~ProgrammedActiveObject() throw() ;





				// Registering section.
				
				
				
				/**
				 * Registers this active object to the supposedly already
				 * existing scheduler.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				virtual void registerToScheduler() ;
				
				
				
				/**
				 * Unregisters this active object from the existing scheduler
				 * to whom it was supposedly registered.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				virtual void unregisterFromScheduler() ;
				
				
				
				
				
				// Settings section.
				
				

				/**
				 * Tells whether programmed activations are defined 
				 * absolutely (from the origin of simulation times, i.e. 
				 * the scheduler start) or relatively to the simulation
				 * tick of the object registering to the scheduler.
				 *
				 */
				bool areProgrammedActivationsAbsolute() const ;
				
				
				
				/**
				 * Defines whether the programmed activations for this 
				 * object are to be defined absolutely (from the origin of
				 * simulation times, i.e. the scheduler start) or relatively
				 * to the simulation tick of the object registering to 
				 * the scheduler.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				void absoluteProgrammedActivationsWanted( bool on ) ;
				 
				
				
				/**
				 * Returns a reference to the internal list of programmed
				 * activations of this object.
				 *
				 * These are the activations as they were specified: if 
				 * they were relative ticks, they are still offsets, not
				 * absolute times.
				 *
				 */
				virtual const SimulationTickList & getProgrammedActivations() 
					const ;
				
				
								  
				/**
				 * Sets the list of programmed activations for this object
				 * to specified value.
				 *
				 * @param newActivationsList the list of simulation ticks 
				 * which will be copied internally.
				 *
				 * @note Any previously defined activations are lost.
				 *
				 * @note Scheduling settings should not be changed once
				 * an active object has been registered to the scheduler, as
				 * the list has already been transmitted to it.
				 *
				 * @see addProgrammedActivations.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				virtual void setProgrammedActivations( 
					const SimulationTickList & newActivationsList ) ;
				
				
				
				/**
				 * Adds the specified list of simulation ticks to the 
				 * list of activation ticks for this object.
				 *
				 * @param additionalActivationsList the list of additional
				 * simulation ticks on which this object will be activated too.
				 *
				 * @note The method will ensure that this object will not be
				 * activated more than once on any given simulation tick.
				 *
				 * @note Scheduling settings should not be changed once
				 * an active object has been registered to the scheduler, as
				 * the list has already been transmitted to it.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				virtual void addProgrammedActivations( 
					const SimulationTickList & additionalActivationsList ) ;
				
					
					
				/**
				 * Automatically called by the scheduler at registration
				 * time.
				 *
				 * @note Registering does not always imply object birth, as
				 * if the scheduler is not currently running, then birth
				 * will happen only when it is started.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */				
				virtual void onRegistering() ;
				
				
			
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
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;
					
				
				
					
			protected:		
		
				
/* 
 * Takes care of the awful issue of Windows DLL with templates.
 *
 * @see Ceylan's developer guide and README-build-for-windows.txt 
 * to understand it, and to be aware of the associated risks. 
 * 
 */
#pragma warning( push )
#pragma warning( disable: 4251 )
						
				/**
				 * Records the list of programmed simulation ticks that 
				 * will be used by the scheduler to activate this object.
				 *
				 * @note Ticks are stored in chronological order.
				 *
				 */
				SimulationTickList _programmedTicks ;
				 
#pragma warning( pop ) 


				 
				/**
				 * Tells whether the programmed trigger ticks should be
				 * understood as absolute (i.e. if their simulation ticks
				 * should be taken litteraly), or as relative (defined as an
				 * offset from this object birth time).
				 *
				 * Is true by default.
				 *
				 */
				bool _absoluteTicks ; 
		
								
				
		} ;
		

	}

}



#endif // OSDL_PROGRAMMED_ACTIVE_OBJECT_H_


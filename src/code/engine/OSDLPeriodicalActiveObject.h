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


#ifndef OSDL_PERIODICAL_ACTIVE_OBJECT_H_
#define OSDL_PERIODICAL_ACTIVE_OBJECT_H_



#include "OSDLActiveObject.h"   // for inheritance
#include "OSDLPeriodicSlot.h"   // for Period, SchedulingException
#include "OSDLEngineCommon.h"   // for ObjectSchedulingPolicy, Weight, etc. 


#include <string>



namespace OSDL
{


	namespace Engine 
	{
	
	
			
		/**
		 * Periodical active objects are active objects that are to be 
		 * scheduled at a regular pace: for an object whose period is N,
		 * the scheduler is expected to trigger it every N of its fundamental
		 * simulation ticks. 
		 *
		 * Therefore a periodical object can run at any frequency that is
		 * a submultiple of the scheduler fundamental one.
		 * 
		 * The scheduler tries to balance the activation of the periodical
		 * objects sharing the same period.
		 * For example, objects whose period is 7 are stored in the slot 
		 * dedicated to the period 7, which has 7 sub-slots in which all these
		 * objects tend to be evenly dispatched. 
		 *
		 * Therefore every simulation tick one of these sub-slots will be
		 * scheduled (approximately 1/7 of the objects of period 7 will 
		 * therefore be activated), and the next simulation tick the next 
		 * sub-slot will be scheduled, etc.
		 *
		 * At the end, all objects will be scheduled at their requested 
		 * frequency, and their scheduling should be relatively balanced,
		 * from a simulation tick to another.
		 *
		 * Note that this scattering will not decrease the number of idle
		 * calls that can be performed, as they are to take place between
		 * simulation ticks, which will be scheduled regardless of whether
		 * there is a periodical object to activate. 
		 *
		 * The birth time of an active object is the first time when it is
		 * registered and when the scheduler runs (this can happen in any 
		 * order). For example it is not the tick corresponding to the first
		 * time this periodical object is activated.
		 *
		 * @see also the ProgrammedActiveObject class.
		 *
		 */
		class OSDL_DLL PeriodicalActiveObject: public ActiveObject
		{
		
		
			public:
			
						
				/**
				 * Constructor of a periodically activated object. 
				 *
				 * @param period tells how many simulation steps are to be
				 * waited by the scheduler until this active object is 
				 * activated again. The period must not be null.
				 *
				 * @param autoRegister this object will automatically
				 * register to the scheduler iff true.
				 *
				 * @param policy allows to choose a scheduling policy, 
				 * according to the quality of service this active object
				 * requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this object is to cost, on average. 
				 * This helps the scheduler doing its job.
				 *
				 * @throw SchedulingException if the construction failed.
				 *
				 */
				explicit PeriodicalActiveObject( 
					Events::Period period = 1, 
					bool autoRegister = true,
					ObjectSchedulingPolicy policy = relaxed, 
					Weight weight = 1 ) throw( SchedulingException ) ;
							
				
				
				/**
				 * Virtual destructor.
				 *
				 * @note This object will automatically unsubscribe
				 * from the scheduler if needed.
				 *
				 */
				virtual ~PeriodicalActiveObject() throw() ;




				// Registering section.
				
				
				/**
				 * Registers this active object to the supposedly already
				 * existing scheduler.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				virtual void registerToScheduler() 
					throw( SchedulingException ) ;
				
				
				/**
				 * Unregisters this active object from the existing scheduler
				 * to whom it was supposedly registered.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				virtual void unregisterFromScheduler() 
					throw( SchedulingException ) ;
				
				
				
				
				// Settings section.

				
				
				/**
				 * Returns the period for this active object.
				 *
				 */
				virtual Events::Period getPeriod() const throw() ;
				
				
				
				/**
				 * Sets a period for this object, which will be periodically
				 * scheduled accordingly.
				 *
				 * @param newPeriod the new period of activation for this
				 * object, expressed in simulation ticks. Must be strictly
				 * positive.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				virtual void setPeriod( Events::Period newPeriod ) 
					throw( SchedulingException ) ;
				 								 
				 
				 
				/**
				 * Sets the period of this object so that it gets activated 
				 * on specified frequency, making this object periodically
				 * activated if it was not, or changing its already defined
				 * period otherwise.
				 *
				 * @param newFrequency the desired frequency, expressed 
				 * in Hertz.
				 *
				 * @return The actual chosen frequency, which may differ a 
				 * bit from specified one since it depends on the simulation
				 * tick granularity. 
				 * Notably, the obtained frequency cannot exceed the 
				 * specified simulation frequency for the scheduler.
				 * The actual frequency is nevertheless chosen as the nearest
				 * possible frequency.
				 *
				 * @note To be able to compute the actual period, expressed 
				 * in simulation ticks, from the specified frequency, the
				 * scheduler that will be later used for this object is needed.
				 *
				 * @throw SchedulingException if there was no existing 
				 * scheduler available.
				 *
				 */ 
				virtual Ceylan::Maths::Hertz setFrequency( 
						Ceylan::Maths::Hertz newFrequency ) 
					throw( SchedulingException ) ; 
				
					
					
				/**
				 * Returns the number of the sub-slot this active object
				 * is referenced in.
				 *
				 * This sub-slot is set on registration: the scheduler calls 
				 * the onRegistering method of this object.
				 *
				 * @throw SchedulingException if the operation failed,
				 * notably if this object is not stored in a sub-slot 
				 * already.
				 *
				 */
				virtual Events::Period getSubslotNumber() const 
					throw( SchedulingException ) ;
				
					
					
				/**
				 * Automatically called by the scheduler at registration
				 * time.
				 *
				 * Sets the sub-slot, to ease deallocation.
				 *
				 * @note Registering does not always imply object birth, as
				 * if the scheduler is not currently running, then birth
				 * will happen only when it is started.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */	
				virtual void onRegistering( Events::Period subslot ) 
					throw( SchedulingException ) ;


					
				/**
				 * Sets the birth time, expressed in simulation ticks, of 
				 * this active object.
				 *
				 * @note Called by the scheduler, when registering this 
				 * active object. This object will compute its exact actual 
				 * begin tick, depending on the sub-slot it has been put in.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */				
				virtual void setBirthTick( 
						Events::SimulationTick currentSimulationTick ) 
					throw( SchedulingException ) ;

		
		
	            /**
	             * Returns a user-friendly description of the state of 
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
				 * Tells what is the requested scheduling period for this
				 * object.
				 *
				 */
				Events::Period _period ;
							
								
				/**
				 * Records in which sub-slot this object is stored.
				 * 
				 * @note Ranges in [1..Period] when registered.
				 *
				 */
				Events::Period _subslot ;
								
				
		} ;

	}

}


#endif // OSDL_PERIODICAL_ACTIVE_OBJECT_H_


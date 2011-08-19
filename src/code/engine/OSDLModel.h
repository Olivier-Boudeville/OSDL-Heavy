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


#ifndef OSDL_MODEL_H_
#define OSDL_MODEL_H_



#include "OSDLPeriodicalActiveObject.h"   // for inheritance
#include "OSDLProgrammedActiveObject.h"   // for inheritance
#include "OSDLEngineCommon.h"             // for ObjectSchedulingPolicy

#include "Ceylan.h"                       // for VerbosityLevels, inheritance


#include <string>
#include <list>




namespace OSDL
{



	namespace MVC
	{



		/*
		 * Here two kinds of event-based OSDL models are defined:
		 *   - PeriodicalModel
		 *   - ProgrammedModel
		 *
		 * @note In both cases, the Ceylan base event-driven MVC framework is
		 * used, not the templated Generic MVC system.
		 *
		 * @note Models may be scheduled even after they requested the scheduler
		 * to stop, as this request will not be taken into account before the
		 * current engine has been fully evaluated. Moreover some ticks might be
		 * skipped during that same tick, so special care must be taken when
		 * writing models (notably those executing a termination procedure, to
		 * ensure it is not run more than once).
		 *
		 */




		/**
		 * Event-based model, to be periodically activated by the scheduler.
		 *
		 * @see OSDL::Engine::Scheduler
		 *
		 */
		class OSDL_DLL PeriodicalModel : public Ceylan::Model,
			public Engine::PeriodicalActiveObject
		{


			public:



				/**
				 * Constructor of an OSDL MVC model which is to be periodically
				 * scheduled.
				 *
				 * @param period tells how many simulation ticks are to be
				 * waited by the scheduler until this model is activated again.
				 * The period must not be null.
				 *
				 * @param autoRegister tells whether this new Model is to
				 * automatically register itself to the scheduler. This requires
				 * the scheduler to exist already.
				 *
				 * @param policy allows to choose a scheduling policy, according
				 * to the quality of service this model requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this model is to cost on average. This helps
				 * the scheduler doing its job balance.
				 *
				 * @throw Engine::SchedulingException if the construction
				 * failed.
				 *
				 */
				explicit PeriodicalModel(
					Events::Period period = 1,
					bool autoRegister = true,
					Engine::ObjectSchedulingPolicy policy = Engine::relaxed,
					Engine::Weight weight = 1 ) ;



				/**
				 * Virtual destructor.
				 *
				 * @note This model will automatically unsubscribe from the
				 * scheduler if needed.
				 *
				 */
				virtual ~PeriodicalModel() throw() ;



				/**
				 * Returns an user-friendly description of the state of this
				 * object.
				 *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
				 *
				 */
				virtual const std::string toString(
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;


		} ;




		/**
		 * Event-based model, to be activated on specified ticks by the
		 * scheduler.
		 *
		 * @see OSDL::Engine::Scheduler
		 *
		 */
		class OSDL_DLL ProgrammedModel : public Ceylan::Model,
			public Engine::ProgrammedActiveObject
		{


			public:



				/**
				 * Constructor of an OSDL MVC model which is to be scheduled on
				 * programmed ticks.
				 *
				 * @param activationTicks is the list of simulation ticks when
				 * this model should be activated.
				 *
				 * This model does not take ownership of this list, it will make
				 * a copy of it.
				 *
				 * @param autoRegister tells whether this new Model is to
				 * automatically register itself to the scheduler. This requires
				 * the scheduler to exist already.
				 *
				 * @param policy allows to choose a scheduling policy, according
				 * to the quality of service this model requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this model is to cost on average. This helps
				 * the scheduler doing its job balance.
				 *
				 * @throw Engine::SchedulingException if the construction
				 * failed.
				 *
				 */
				explicit ProgrammedModel(
					const Engine::SimulationTickList & activationTicks,
					bool absolutelyDefined = true,
					bool autoRegister = true,
					Engine::ObjectSchedulingPolicy policy = Engine::relaxed,
					Engine::Weight weight = 1 ) ;



				/**
				 * Constructor of an OSDL MVC model which is to be scheduled on
				 * a specific programmed tick.
				 *
				 * @param activationTick is the simulation tick when this model
				 * should be activated.
				 *
				 * @param autoRegister tells whether this new Model is to
				 * automatically register itself to the scheduler. This requires
				 * the scheduler to exist already.
				 *
				 * @param policy allows to choose a scheduling policy, according
				 * to the quality of service this model requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this model is to cost on average. This helps
				 * the scheduler doing its job balance.
				 *
				 * @throw Engine::SchedulingException if the construction
				 * failed.
				 *
				 */
				explicit ProgrammedModel(
					Events::SimulationTick activationTick,
					bool absolutelyDefined = true,
					bool autoRegister = true,
					Engine::ObjectSchedulingPolicy policy = Engine::relaxed,
					Engine::Weight weight = 1 ) ;



				/**
				 * Virtual destructor.
				 *
				 * @note This model will automatically unsubscribe from the
				 * scheduler if needed.
				 *
				 */
				virtual ~ProgrammedModel() throw() ;



				/**
				 * Returns an user-friendly description of the state of this
				 * object.
				 *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
				 *
				 */
				virtual const std::string toString(
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;



		} ;


	}


}



#endif // OSDL_MODEL_H_

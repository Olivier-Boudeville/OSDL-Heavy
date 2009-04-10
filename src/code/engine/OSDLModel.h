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
		 * @note In both cases, the Ceylan base event-driven MVC framework
		 * is used, not the templated Generic MVC system.
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
				 * Constructor of an OSDL MVC model which is to be 
				 * periodically scheduled.
				 *
				 * @param period tells how many simulation ticks are to be
				 * waited by the scheduler until this model is activated again.
				 * The period must not be null.
				 *
				 * @param autoRegister tells whether this new Model is to
				 * automatically register itself to the scheduler. 
				 * It requires the scheduler to exist already.
				 *
				 * @param policy allows to choose a scheduling policy, 
				 * according to the quality of service this model requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this model is to cost on average. 
				 * This helps the scheduler doing its job balance.
				 *
				 * @throw Engine::SchedulingException if the construction
				 * failed.
				 *
				 */
				explicit PeriodicalModel( 
						Events::Period period = 1,
						bool autoRegister = true, 
						Engine::ObjectSchedulingPolicy policy = Engine::relaxed,
						Engine::Weight weight = 1 )
					throw( Engine::SchedulingException ) ;
				
							
				/**
				 * Virtual destructor.
				 *
				 * @note This model will automatically unsubscribe
				 * from the scheduler if needed.
				 *
				 */
				virtual ~PeriodicalModel() throw() ;


	            /**
	             * Returns an user-friendly description of the state 
				 * of this object.
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
				 * Constructor of an OSDL MVC model which is to be 
				 * scheduled on programmed ticks.
				 *
				 * @param activationTicks is the list of simulation ticks 
				 * when this model should be activated.
				 * This model does not take ownership of this list, it 
				 * will make a copy of it.
				 *
				 * @param autoRegister tells whether this new Model is to
				 * automatically register itself to the scheduler. 
				 * It requires the scheduler to exist already.
				 *
				 * @param policy allows to choose a scheduling policy, 
				 * according to the quality of service this model requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this model is to cost on average. 
				 * This helps the scheduler doing its job balance.
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
						Engine::Weight weight = 1 )
					throw( Engine::SchedulingException ) ;
				
				
				
				/**
				 * Constructor of an OSDL MVC model which is to be 
				 * scheduled on a specific programmed tick.
				 *
				 * @param activationTick is the simulation tick when this 
				 * model should be activated.
				 *
				 * @param autoRegister tells whether this new Model is to
				 * automatically register itself to the scheduler. 
				 * It requires the scheduler to exist already.
				 *
				 * @param policy allows to choose a scheduling policy, 
				 * according to the quality of service this model requires.
				 *
				 * @param weight evaluates how much processing power an
				 * activation of this model is to cost on average. 
				 * This helps the scheduler doing its job balance.
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
						Engine::Weight weight = 1 )
					throw( Engine::SchedulingException ) ;
				
							
				/**
				 * Virtual destructor.
				 *
				 * @note This model will automatically unsubscribe
				 * from the scheduler if needed.
				 *
				 */
				virtual ~ProgrammedModel() throw() ;


	            /**
	             * Returns an user-friendly description of the state 
				 * of this object.
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
			
				
		} ;

	}

}


#endif // OSDL_MODEL_H_

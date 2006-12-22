#ifndef OSDL_MODEL_H_
#define OSDL_MODEL_H_


#include "OSDLActiveObject.h"   // for inheritance
#include "OSDLEngineCommon.h"   // for ObjectSchedulingPolicy

#include "Ceylan.h"             // for VerbosityLevels, inheritance


#include <string>
#include <list>




namespace OSDL
{


	namespace MVC 
	{
	
			
		/**
		 * These specialized MVC models comply with the scheduler framework, since they are
		 * active objects.
		 *
		 * It allows the models to be scheduled, periodically and/or programmatically, during 
		 * simulation ticks.
		 *
		 * @see OSDL::Engine::Scheduler
		 *
		 */
		class Model : public Ceylan::Model, public Engine::ActiveObject
		{
		
		
			public:
			
			
				/**
				 * Constructor of an OSDL MVC model which is to be periodically scheduled.
				 *
				 * @param autoRegister tells whether this new Model is to automatically register
				 * itself to the scheduler. It requires the scheduler to exist already.
				 *
				 * @param period tells how many simulation steps are to be waited by the scheduler
				 * until the model is activated again. The period must not be null.
				 *
				 *
				 * @param policy allows to choose a scheduling policy, according to the quality of
				 * service this model requires.
				 *
				 * @param weight evaluates how much processing power an activation of this model
				 * is to cost, on average. This helps the scheduler doing its job.
				 *
				 * @note Do not mix up this constructor with the one for programmed models, since
				 * their arguments are almost the same.
				 *
				 */
				explicit Model( bool autoRegister = true, Events::Period period = 1, 
						Engine::ObjectSchedulingPolicy policy = Engine::relaxed, 
						Engine::Weight weight = 1 )
					throw( Engine::SchedulingException ) ;
				
				
				/**
				 * Constructor of an OSDL MVC model activated only on specifically programmed
				 * simulation steps. 
				 *
				 * @param autoRegister tells whether this new Model is to automatically register
				 * itself to the scheduler. It requires the scheduler to exist already.
				 *
				 * @param triggeringStates is the list of simulation ticks where the activation
				 * should occur. The model does not take ownership of this list, it will make
				 * a copy of it.
				 *
				 * @param absolutlyDefined tells if the specified steps are defined absolutly,
				 * or relatively to the current simulation step.
				 *
				 * @param policy allows to choose a scheduling policy, according to the quality of
				 * service this model requires.
				 *
				 * @param weight evaluates how much processing power an activation of this model
				 * is to cost, on average. This helps the scheduler doing its job.
				 *
				 * @note Do not mix up this constructor with the one for periodic models, since
				 * their arguments are almost the same.
				 *
				 */
				explicit Model(	bool autoRegister, 
					const std::list<Events::SimulationTick> & triggeringStates, 
					bool absolutlyDefined = true, 
					Engine::ObjectSchedulingPolicy policy = Engine::relaxed, 
					Engine::Weight weight = 1 )
						throw( Engine::SchedulingException ) ;
				
				
				/**
				 * Constructor of an OSDL MVC Model activated only on a specific simulation step. 
				 *
				 * @param autoRegister tells whether this new Model is to automatically register
				 * itself to the scheduler. It requires the scheduler to exist already.
				 *
				 * @param triggerTick is the simulation step when this model will be
				 * activated.
				 *
				 * @param absolutlyDefined tells if the specified steps are defined absolutly,
				 * or relatively to the current simulation step. No default value can be given
				 * for that parameter, since there would be an ambiguity with first constructor
				 * on any call with only one argument.
				 *
				 * @param policy allows to choose a scheduling policy, according to the quality of
				 * service this model requires.
				 *
				 * @param weight evaluates how much processing power an activation of this model
				 * is to cost, on average. This helps the scheduler doing its job.
				 *
				 * @note Do not mix up this constructor with the one for programmed models, since
				 * their arguments are almost the same.
				 *						 
				 */
				explicit Model( bool autoRegister, Events::SimulationTick triggerTick, 
					bool absolutlyDefined = true, 
					Engine::ObjectSchedulingPolicy policy = Engine::relaxed, 
					Engine::Weight weight = 1 ) 
						throw( Engine::SchedulingException ) ;
				
				
				
				/**
				 * Virtual destructor, which performs automatic unsubscribing of this model from
				 * the scheduler.
				 *
				 */
				virtual ~Model() throw() ;


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
			
				
				
		} ;

	}

}


#endif // OSDL_MODEL_H_

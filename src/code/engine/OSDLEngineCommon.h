#ifndef OSDL_ENGINE_COMMON_H_
#define OSDL_ENGINE_COMMON_H_


#include <string>
#include <list>

#include "OSDLException.h"            // for Exception


/*
 * This file has for purpose to solve all intricated dependency links existing between the 
 * scheduler, the active objects and the periodic slots.
 *
 *
 *
 */
 
 

namespace OSDL
{

	namespace Engine 
	{
	
		
		// Forward declaration.
		class ActiveObject ;
		
		
		/// Exception to be thrown when engine abnormal behaviour occurs.
		class EngineException : public OSDL::Exception
		{
			public:
		
				explicit EngineException( const std::string & reason ) throw() ;
				virtual ~EngineException() throw() ;
			
		} ;

		
		
		/// Exception to be thrown when scheduling abnormal behaviour occurs.
		class SchedulingException : public EngineException
		{
			public:
		
				explicit SchedulingException( const std::string & reason ) throw() ;
				virtual ~SchedulingException() throw() ;
			
		} ;


		/**
		 * Defines a list of pointer to active objects.
		 *
		 * Was not defined in OSDLScheduler to avoir circular dependency : OSDLPeriodicSlot
		 * should not depend on OSDLScheduler, since the opposite have to be true.
		 *
		 */
		typedef std::list<ActiveObject *> listActiveObjects ;
		


		/**
		 * Determines roughly how much the activation of a specific object will cost in CPU
		 * time. The greater the weight is, the heavier the needed processing resource should be.
		 *
		 */
		typedef unsigned int Weight ;
		
		
		/**
		 * Describes a scheduling policy.
		 *
		 * relaxed : the scheduler is allowed to defer activation of a few periods if needed. 
		 * Beside the jittering, some latency may occur, if the scheduler prefers putting this 
		 * object in a less overcrowed periodic slot than the current one.
		 *
		 * strict : enforce strict respect of specified parameters, namely for the period and the
		 * simulation step to begin with. Strict scheduling might be useful for animations or sound,
		 * for example.
		 *
		 */
		enum ObjectSchedulingPolicy { relaxed, strict } ;
		
	
	
	}
	
}	


#endif // OSDL_ENGINE_COMMON_H_

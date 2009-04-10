#ifndef OSDL_ENGINE_COMMON_H_
#define OSDL_ENGINE_COMMON_H_



#include "OSDLException.h"      // for Exception
#include "OSDLEvents.h"         // for SimulationTick

#include <string>
#include <list>



/*
 * This file has for purpose to solve all intricated dependency links 
 * existing between the scheduler, the active objects and the periodic slots.
 *
 */
 
 

namespace OSDL
{


	namespace Engine 
	{
	
	
		
		// Forward declarations:
		class ActiveObject ;

		class PeriodicalActiveObject ;
		class ProgrammedActiveObject ;
		
		
		/// Exception to be thrown when engine abnormal behaviour occurs.
		class OSDL_DLL EngineException : public OSDL::Exception
		{
			public:
		
				explicit EngineException( const std::string & reason ) throw() ;
				virtual ~EngineException() throw() ;
			
		} ;

		
		
		/// Exception to be thrown when scheduling abnormal behaviour occurs.
		class OSDL_DLL SchedulingException : public EngineException
		{
			public:
		
				explicit SchedulingException( const std::string & reason )
					throw() ;
				virtual ~SchedulingException() throw() ;
			
		} ;


		/**
		 * Defines a list of pointers to active objects.
		 *
		 * Was not defined in OSDLScheduler to avoir circular dependency:
		 * OSDLPeriodicSlot should not depend on OSDLScheduler, since the
		 * opposite has to be true.
		 *
		 */
		typedef std::list<PeriodicalActiveObject *> ListOfActiveObjects ;

		
		/**
		 * Defines a list of pointers to periodical active objects.
		 *
		 */
		typedef std::list<PeriodicalActiveObject *>
			ListOfPeriodicalActiveObjects ;


		/**
		 * Defines a list of pointers to programmed active objects.
		 *
		 */
		typedef std::list<ProgrammedActiveObject *>
			ListOfProgrammedActiveObjects ;



		/**
		 * Determines roughly how much the activation of a specific object 
		 * will cost in CPU time. 
		 *
		 * The greater the weight is, the heavier the needed processing 
		 * resource should be.
		 *
		 */
		typedef Ceylan::Uint16 Weight ;
		
		
		/**
		 * Describes a scheduling policy, among: 
		 *
		 * 	- relaxed: the scheduler is allowed to defer activation of a 
		 * few periods if needed. 
		 * Beside the jittering, some latency may occur, if the scheduler
		 * prefers putting this object in a less overcrowed periodic slot 
		 * than its current one.
		 *
		 *	- strict: enforce strict respect of specified parameters, 
		 * namely for the period and the simulation step to begin with. 
		 * Strict scheduling might be useful for animations or sound, for
		 * example.
		 *
		 */
		enum ObjectSchedulingPolicy { relaxed, strict } ;
		
	
		/// Type for lists of simulation ticks.
		typedef std::list<Events::SimulationTick> SimulationTickList ;
		
		
		/// Used to measure time past deadlines.
		typedef Ceylan::Uint32 Delay ;
			
		
	}
	
}	


#endif // OSDL_ENGINE_COMMON_H_


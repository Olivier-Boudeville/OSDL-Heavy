#ifndef OSDL_PERIODIC_SLOT_H_
#define OSDL_PERIODIC_SLOT_H_

 
#include "OSDLEvents.h"               // for SimulationTick, Period
#include "OSDLActiveObject.h"   	  // for ObjectSchedulingPolicy, etc.

// for ListOfActiveObjects, SchedulingException, etc. :
#include "OSDLEngineCommon.h"         



#include <string>
#include <list>
 


namespace OSDL
{


	namespace Engine 
	{
	
	
		// Periodic slots register active objects.
		class ActiveObject ;


		
		/**
		 * Records informations about a periodic slot.
		 * 
		 * Each of these periodic slots is internally divided into as 
		 * many sub-slots as the period length. 
		 * Each registered active object will be listed in at least a sub-slot. 
		 *
		 * For example, for a periodicity of 7 simulation steps, a dedicated
		 * PeriodicSlot will be created. 
		 * It will be in charge of all objects registered for this 
		 * periodicity, and will have internally a list of 7 sub-slots. 
		 * At each new simulation tick, all objects registered in the
		 * corresponding sub-slot (whose number will be :
		 * new simulation tick % 7), will be activated.
		 *
		 * So, at a given simulation step, all objects put in corresponding
		 * sub-slot will be activated. 
		 * On the next step, the list corresponding to the next sub-slot 
		 * will be activated, and so on. 
		 * Each object registered in a sub-slot will therefore be activated
		 * at the right frequency, as defined in the slot, whereas not all of
		 * these objects are activated in the same simulation time, to 
		 * avoid unbalanced load.
		 *
		 * A PeriodicSlot does not have to manage simulation time internally, 
		 * it therefore fully relies on the scheduler to pass it correct
		 * simulation ticks.
		 *
		 * @note This object has no virtual member so that there is no 
		 * 'virtual' overhead (vtable) : the scheduler will make an 
		 * intensive use of it.
		 *
		 */
		class OSDL_DLL PeriodicSlot /* no inheritance, no virtual methods */
		{
		
		
			public:
			
			
				/**
				 * Creates a scheduler slot for given non-null periodicity,
				 * expressed in simulation ticks.
				 *
				 * @param periodicity the chosen periodicity.
				 *
				 */
				explicit PeriodicSlot( Events::Period periodicity ) throw() ;
				
				
				/// Non-virtual destructor.
				~PeriodicSlot() throw() ;
				
				
				/**
				 * Adds a new active object. 
				 * It will be registered in a slot, depending on its
				 * scheduling policy.
				 *
				 * @note An object can be registered multiple times, even 
				 * in the same periodic slot.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				 void add( ActiveObject & newObject ) 
				 	throw( SchedulingException ) ;
				
				 
				/**
				 * Removes entirely an active object, possibly registered
				 * multiple time, from its slot.
				 *
				 * @throw SchedulingException if the object was not found in 
				 * any sub-slot of the slot.
				 *
				 */
				 void remove( ActiveObject & object ) 
				 	throw( SchedulingException ) ;
				 
				 
				/**
				 * Activates all objects registered in the slot 
				 * corresponding on this new tick.
				 *
				 * @note The simulation tick is specified so that it can be
				 * propagated to active objects. 
				 * No blocking checking is performed at the level of the
				 * periodic slot : it is assumed that no tick is skipped.
				 *
				 */ 
				void onNextTick( Events::SimulationTick newTick ) throw() ;
				
				
				/**
				 * Notices all relevant objects that specified simulation 
				 * tick has to be skipped.
				 * 
				 * Called by the scheduler whenever a simulation tick has 
				 * to be skipped.
				 *
				 * @throw SchedulingException if an object is unable to 
				 * overcome a simulation skip (reserve it to special uncommon
				 * fatal cases, since skips may happen at any time on most
				 * computers).
				 *
				 * @note Objects can have this method called multiple times 
				 * for only one skipped simulation step, if they were to be
				 * scheduled multiple times during the skipped simulation 
				 * tick (ex : an object both registered as programmed and 
				 * periodic).
				 *
				 */
				void onSimulationSkipped( Events::SimulationTick skipped ) 
					throw( SchedulingException ) ;
				
				
				/**
				 * Returns the period this slot is in charge of.
				 *
				 */
				Events::Period getPeriod() throw() ;
					
						
	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall 
				 * settings.
				 *
				 * @note Periodic slots do not really implement the
				 * Ceylan::TextDisplayable interface, since the signature 
				 * of the toString would require it is virtual, whereas this
				 * object is designed so that it has no virtual member, hence 
				 * no virtual table overhead.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;
			
			
			
			protected:			
			
			
				/**
				 * Returns the sub-slot number corresponding to specified
				 * simulation tick.
				 *
				 * @param tick the simulation tick to be translated into a
				 * sub-slot number for this periodic slot.
				 *
				 */
				Events::Period getSubSlotForSimulationTick(
					Events::SimulationTick tick ) const throw() ;
				
				
				/**
				 * Adds explicitly in specified slot the active object.
				 *
				 */				 
				void addInSlot( ActiveObject & newObject, 
					Events::Period targetSlot ) throw() ;

					
				/**
				 * Removes from specified slot all subscriptions of the 
				 * active object, and updates the slot weight.
				 *
				 * @return false if and only if object was not in slot.
				 *
				 */				 
				bool removeFromSlot( ActiveObject & object, 
					Events::Period targetSlot ) throw( SchedulingException) ;
				
				
				/**
				 * Returns the first slot whose total object weights is 
				 * minimal for this periodic slot.
				 *
				 * @note the returned period can index a slot whose list 
				 * pointer is null, since in all cases where all weights 
				 * are null, the first slot, potentially with no list created,
				 * is returned.
				 * 
				 */
				Events::Period getLeastBusySlot() const throw() ;
					
				
				/**
				 * Activates in turn all active objects registered in the
				 * specified slot, from the first object ever registered to 
				 * the last, if they had not been activated before.
				 * Each of them is given the specified simulation time.
				 *
				 * @param subSlot the sub-slot corresponding to this 
				 * simulation tick for this periodicity. 
				 * For example, for a periodicity of 7 simulation ticks, 
				 * the selected sub-slot could be (current simulation tick) % 7.
				 *
				 * @param currentTime the simulation time which should be 
				 * given to activated objects.
				 * 
				 */
				void activateAllObjectsInSubSlot( Events::Period subSlot, 
					Events::SimulationTick currentTime ) throw() ;
				
				
				/**
				 * Stores the periodicity, expressed in simulation steps,
				 * of this slot.
				 *
				 */
				Events::Period _period ;

				
				/// Records the slot corresponding to current simulation step. 
				Events::Period _currentSlot ;

				
				/**
				 * This is an array of pointers to lists : 
				 * ListOfActiveObjects * _slots[].
				 *
				 * The pointed array has as many elements (slots) as
				 * constructor-specified <b>periodicity</b>.
				 * 
				 */
				ListOfActiveObjects ** _slots ;	
				
				
				/**
				 * Records the weights of all active objects registered in 
				 * a slot.
				 *
				 */
				Weight * _slotWeights ;



			private:
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit PeriodicSlot( const PeriodicSlot & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				PeriodicSlot & operator = ( const PeriodicSlot & source )
					throw() ;
				
				
		} ;

	}
	
}


#endif // OSDL_PERIODIC_SLOT_H_

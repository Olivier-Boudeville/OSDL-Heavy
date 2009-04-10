#ifndef OSDL_PERIODIC_SLOT_H_
#define OSDL_PERIODIC_SLOT_H_

 
#include "OSDLPeriodicalActiveObject.h" // for ObjectSchedulingPolicy, etc.

// for ListOfPeriodicalActiveObjects, SchedulingException, etc.:
#include "OSDLEngineCommon.h"         



#include <string>
#include <list>
 


namespace OSDL
{


	namespace Engine 
	{
	
	
		// Periodic slots register periodical active objects.
		class PeriodicalActiveObject ;

		
		/**
		 * Records informations about a periodic slot.
		 * 
		 * Each of these periodic slots is internally divided into as 
		 * many sub-slots as the length of that slot period. 
		 *
		 * Each registered periodical active object will then be listed in
		 * exactly one sub-slot. 
		 *
		 * For example, when a PeriodicSlot will be created to manage all
		 * objects relying on a periodicity of 7 simulation ticks, that slot
		 * will have internally an array of 7 sub-slots. 
		 * At each new simulation tick, all objects registered in the
		 * corresponding sub-slot (whose number will be: 
		 * new simulation tick % 7), will be activated.
		 *
		 * So, at a given simulation tick, all objects put in corresponding
		 * sub-slot will be activated. 
		 * On the next tick, the list corresponding to the next sub-slot 
		 * will be activated, and so on. 
		 * Each object registered in a sub-slot will therefore be activated
		 * at the right frequency, as defined in the slot, whereas not all of
		 * these objects are activated at the same simulation time, to 
		 * avoid unbalanced load.
		 *
		 * A PeriodicSlot does not have to manage simulation time internally, 
		 * it therefore fully relies on the scheduler to pass it correct
		 * simulation ticks.
		 *
		 * @note This object has no virtual member so that there is no 
		 * 'virtual' overhead (vtable): the scheduler will make an 
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
				 * Adds a new periodical active object to that slot. 
				 *
				 * It will be registered in a sub-slot, depending on its
				 * scheduling policy.
				 *
				 * The slot weight will be updated.
				 *
				 * @return the sub-slot this object was registered in.
				 *
				 * @throw SchedulingException if the operation failed.
				 *
				 */
				 Events::Period add( PeriodicalActiveObject & newObject ) 
				 	throw( SchedulingException ) ;
							 
							 							 
				 
				 /**
				  * Removes specified periodical object from specified
				  * sub-slot, and updates the slot weight.
				  *
				  * @throw SchedulingException if the operation failed.
				  *
				  */
				 void removeFromSubslot( PeriodicalActiveObject & object )
				 	throw( SchedulingException ) ;
				 
				 
				 
				/**
				 * Activates all objects registered in the sub-slot 
				 * corresponding to this new tick.
				 *
				 * @note The simulation tick is specified so that it can be
				 * propagated to active objects. 
				 *
				 * No blocking checking that no simulation tick was skipped
				 * is performed at the level of the periodic slot.
				 *
				 * @return true iff this periodic slot should be kept.
				 *
				 */ 
				bool onNextTick( Events::SimulationTick newTick ) throw() ;
				
				
				
				/**
				 * Notifies all relevant objects that specified simulation 
				 * tick had to be skipped.
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
				 * for only one skipped simulation tick, if they were to be
				 * scheduled multiple times during the skipped simulation 
				 * tick (ex: an object both registered as programmed and 
				 * periodic).
				 *
				 */
				void onSimulationSkipped( Events::SimulationTick skipped ) 
					throw( SchedulingException ) ;
				
				
				
				/**
				 * Returns the period this slot is in charge of.
				 *
				 */
				Events::Period getPeriod() const throw() ;
					
					
					
				/**
				 * Returns true iff the slot is fully empty,, i.e. none of 
				 * its sub-slots contains any object.
				 *
				 */
				bool isEmpty() const throw() ;
					
				
						
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
				 * Returns the first sub-slot whose total object weight is 
				 * minimal for this periodic slot.
				 *
				 * @note the returned period number can index a slot whose list 
				 * pointer is null, since in all cases where all weights 
				 * are null, the first slot, potentially with no list created,
				 * is returned.
				 * 
				 */
				Events::Period getLeastBusySubSlot() const throw() ;
				
				
				
				/**
				 * Adds explicitly in specified sub-slot the active object.
				 *
				 */				 
				void addInSubSlot( PeriodicalActiveObject & newObject, 
					Events::Period targetSubSlot ) throw() ;
					
				
				/**
				 * Activates in turn all active objects registered in the
				 * specified sub-slot, from the first object ever registered to 
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
				 * @return true iff this periodic sub-slot should be kept.
				 *
				 */
				bool activateAllObjectsInSubSlot( Events::Period subSlot, 
					Events::SimulationTick currentTime ) throw() ;
				
				
				
				/**
				 * Stores the periodicity, expressed in simulation ticks,
				 * of this slot.
				 *
				 */
				Events::Period _period ;

				
				
				/// Records the slot corresponding to current simulation tick. 
				Events::Period _currentSubSlot ;

				
				
				/**
				 * This is an array of pointers to lists: 
				 * ListOfPeriodicalActiveObjects * _subslots[].
				 *
				 * The pointed array has as many elements (sub-slots) as
				 * constructor-specified <b>periodicity</b>.
				 *
				 * Ex: for a period of 3, we have list #0, #1 and #2.
				 *
				 */
				ListOfPeriodicalActiveObjects ** _subslots ;	
				
				
				
				/**
				 * Records the weights of all active objects registered in 
				 * a slot.
				 *
				 */
				Weight * _subslotWeights ;




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


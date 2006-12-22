#include "OSDLModel.h"


#include "OSDLEvents.h"           // for Period
#include "OSDLScheduler.h"        // for Scheduler

#include "Ceylan.h"               // for Log


using std::string ;
using std::list ;

using namespace Ceylan::Log ;


using namespace OSDL::Events ;
using namespace OSDL::Engine ;

using namespace OSDL::MVC ;



Model::Model( bool autoRegister, Period period, ObjectSchedulingPolicy policy, Weight weight ) 
		throw( SchedulingException ) : 
	ActiveObject( period, policy, weight )	
{
	if ( autoRegister )
		Scheduler::GetExistingScheduler().registerObject( * this ) ; 
}


Model::Model( bool autoRegister, const list<SimulationTick> & triggeringTicks,
	bool absolutlyDefined, ObjectSchedulingPolicy policy, Weight weight )
		throw(SchedulingException ) :
	ActiveObject( triggeringTicks, absolutlyDefined, policy, weight )
{
	if ( autoRegister )
		Scheduler::GetExistingScheduler().registerObject( * this ) ; 
}	


Model::Model( bool autoRegister, SimulationTick triggerTick, bool absolutlyDefined,
		ObjectSchedulingPolicy policy, Weight weight ) throw( SchedulingException ) :
	ActiveObject( triggerTick, absolutlyDefined, policy, weight )
{
	if ( autoRegister )
		Scheduler::GetExistingScheduler().registerObject( * this ) ; 
}	


Model::~Model() throw()
{

}


const string Model::toString( Ceylan::VerbosityLevels level ) const throw()
{	
	return "OSDL MVC Model, which is a specialized scheduled Ceylan Model : " 
		+ Ceylan::Model::toString( level ) ;
}


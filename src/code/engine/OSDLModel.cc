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



// PeriodicalModel section.


PeriodicalModel::PeriodicalModel( 
	Period period,
	bool autoRegister,  
	ObjectSchedulingPolicy policy,
	Weight weight ) 
		throw( SchedulingException ) : 
	Model(),	
	PeriodicalActiveObject( period, autoRegister, policy, weight )	
{

		
}



PeriodicalModel::~PeriodicalModel() throw()
{

}



const string PeriodicalModel::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{	

	return "OSDL MVC periodical model, which is a " 
		+ Ceylan::Model::toString( level ) 
		+ ", and a " + PeriodicalActiveObject::toString( level ) ;
		
}





// ProgrammedModel section.


ProgrammedModel::ProgrammedModel( 
	const list<SimulationTick> & triggeringTicks,
	bool absolutelyDefined,
	bool autoRegister, 
	ObjectSchedulingPolicy policy,
	Weight weight )
		throw(SchedulingException ) :
	Model(),	
	ProgrammedActiveObject( triggeringTicks, absolutelyDefined, autoRegister,
		policy, weight )
{

		
}	



ProgrammedModel::ProgrammedModel( 
	SimulationTick triggerTick, 
	bool absolutelyDefined, 
	bool autoRegister, 
	ObjectSchedulingPolicy policy, 
	Weight weight ) 
		throw( SchedulingException ) :
	Model(),	
	ProgrammedActiveObject( triggerTick, absolutelyDefined, autoRegister,
		policy, weight )
{

		
}	



ProgrammedModel::~ProgrammedModel() throw()
{

}



const string ProgrammedModel::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{	

	return "OSDL MVC programmed model, which is a " 
		+ Ceylan::Model::toString( level ) 
		+ ", and a " + ProgrammedModel::toString( level ) ;
		
}


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
		Weight weight ) : 
	Model(),	
	PeriodicalActiveObject( period, autoRegister, policy, weight )	
{

		
}



PeriodicalModel::~PeriodicalModel() throw()
{

}



const string PeriodicalModel::toString( Ceylan::VerbosityLevels level ) const
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
		Weight weight ) :
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
		Weight weight ) :
	Model(),	
	ProgrammedActiveObject( triggerTick, absolutelyDefined, autoRegister,
		policy, weight )
{

		
}	



ProgrammedModel::~ProgrammedModel() throw()
{

}



const string ProgrammedModel::toString( Ceylan::VerbosityLevels level ) const
{	

	return "OSDL MVC programmed model, which is a " 
		+ Ceylan::Model::toString( level ) 
		+ ", and a " + ProgrammedActiveObject::toString( level ) ;
		
}


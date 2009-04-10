#ifndef OSDL_WORLD_OBJECT_H_
#define OSDL_WORLD_OBJECT_H_


#include "Ceylan.h"     // for Model inheritance

#include <string>



namespace OSDL
{


	namespace Engine 
	{
	
	
	
		/**
		 * World objects are to be used to describe all objects a virtual 
		 * world can contain. 
		 * Whereas world objects are meant to be passive by default (they do
		 * not get a slice of CPU time during which they live), using 
		 * some specific child classes allows to have active world objects.
		 *
		 * Basic world objects will not therefore <b>act by themselves</b>,
		 * which does not imply they cannot achieve complex trigger-based
		 * behaviours.
		 * For example if a stone on the ground may not require periodic
		 * processings, as it remains static, it can be though taken into 
		 * account as soon as it is grabbed and thrown.
		 * 
		 * World objects are models, in the sense of the Model-Controller-View
		 * framework.
		 *
		 * @see Ceylan::Model
		 *
		 */
		class OSDL_DLL WorldObject : public Ceylan::Model
		{
		
			public:
			
				
				/**
				 * Constructs a new blank world object.
				 *
				 */
				WorldObject() throw() ;
				
				
				/// Virtual destructor.
				virtual ~WorldObject() throw() ;


	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
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


#endif // OSDL_WORLD_OBJECT_H_


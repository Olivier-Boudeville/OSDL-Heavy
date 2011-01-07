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


#ifndef OSDL_BOUNDING_BOX_2D_H_
#define OSDL_BOUNDING_BOX_2D_H_



#include "OSDLBoundingBox.h"       // for inheritance

#include "Ceylan.h"                // for VerbosityLevels, Bipoint

#include <string>
#include <list>




namespace OSDL
{



	namespace Engine 
	{
	
			
			
		/**
		 * 2D bounding boxes are specialized bounding boxes for objects in
		 * 2D space.
		 *
		 * They are locatable in 2D space, therefore are Locatable2D.
		 *
		 * It implies they own a 3x3 homogeneous matrix, which fully defines
		 * their position and angles in 2D space, relatively to a father
		 * Locatable.
		 *
		 * Their father Locatable can be either a Model, of View or another
		 * bounding box.
		 *
		 */
		class OSDL_DLL BoundingBox2D : public BoundingBox, 
			public Ceylan::Locatable2D
		{
		
		
			public:
			
			
			
				/**
				 * Constructor of a 2D bounding box object.
				 *
				 * @param father the father Locatable2D the box is defined
				 * relatively to.
				 *
				 * @param center the center of the 2D bounding box.
				 *
				 * @note No indication of direction is given for the 
				 * bounding box at this level, since for example circle box
				 * would not need it.
				 *
				 */
				explicit BoundingBox2D( Ceylan::Locatable2D & father, 
					const Ceylan::Maths::Linear::Bipoint & center ) ;
				
						
								
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~BoundingBox2D() throw() ;



	            /**
	             * Returns an user-friendly description of the state of
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall 
				 * settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;
				
				
				
				
			protected:
			
			
				
				/**
				 * Checks that specified bounding box is a two-dimensional one.
				 *
				 * @param box the bounding box to be checked.
				 *
				 * @throw BoundingBoxException if it is not a 2D bounding box.
				 *
				 * @return the successfully casted 2D box.
				 *
				 * @note Parameter should not be 'const' in order that
				 * method to be useful for its caller.
				 *
				 */
				static BoundingBox2D & CheckIs2D( BoundingBox & box ) ;			
					
					
					
					
			private:		
		
				
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
	        	explicit BoundingBox2D( const BoundingBox2D & source ) ;



				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
	        	BoundingBox2D & operator= ( const BoundingBox2D & source ) ;
				
				
				
		} ;
		

	}

}



#endif // OSDL_BOUNDING_BOX_2D_H_


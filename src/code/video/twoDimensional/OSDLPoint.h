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


#ifndef OSDL_POINT_H_
#define OSDL_POINT_H_


#include "Ceylan.h"            // for MatrixIndex, TextDisplayable

#include <string>




namespace OSDL
{

	
	
	namespace Video
	{
	
	
	
		namespace TwoDimensional
		{
		
		
		
		
			/**
		     * Represents an abstract point whose coordinates are integers.
			 *
			 * @note These integer points are especially useful to handle
			 * graphical user interface elements, such as widgets.
			 *
			 */
	    	class OSDL_DLL Point: public Ceylan::TextDisplayable
	   		{


	        	public:
	
		
					/// Basic abstract constructor of a point .
	            	Point() ;
										

					/// Virtual destructor of a point.
	            	virtual ~Point() throw() ;
	


					/**
					 * Flips the coordinates of this point (negates them all).
					 *
					 */
					virtual void flip() = 0 ;



	 	            /**
		             * Returns an user-friendly description of the state of 
					 * this object.
		             *
					 * @param level the requested verbosity level.
					 *
					 * @note Text output format is determined from 
					 * overall settings.
					 *
					 * @see Ceylan::TextDisplayable
		             *
		             */
			 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) const ;
						
						

					/**
					 * Tells what is the dimension of the space this 
					 * point is defined in.
					 *
					 */
					static const Ceylan::Maths::Linear::MatrixIndex
							Dimensions = 0 ;
						
					
	    	} ;
			
				
		}

	}
	
}



/// Stream operator for serialization.
OSDL_DLL std::ostream & operator << ( std::ostream & os, 
	const OSDL::Video::TwoDimensional::Point & p ) ;



#endif // OSDL_POINT_H_


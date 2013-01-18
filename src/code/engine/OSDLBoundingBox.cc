/* 
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#include "OSDLBoundingBox.h"



using namespace OSDL::Engine ;

using std::string ;



BoundingBoxException::BoundingBoxException( const string & reason ) :
	EngineException( "BoundingBoxException: " + reason )
{

}



BoundingBoxException::~BoundingBoxException() throw()
{

}





BoundingBox::BoundingBox()
{

}



BoundingBox::~BoundingBox() throw()
{

}



string BoundingBox::InterpretIntersectionResult( IntersectionResult result ) 
{

	switch( result )
	{
	
		case isSeparate:
			return "The two bounding boxes have no intersection." ;
			break ;
				
		case contains:
			return "The first bounding box contains strictly the second." ;
			break ;
				
		case isContained:
			return "The first bounding box is "
				"strictly contained by the second." ;
			break ;
				
		case intersects:
			return "The two bounding boxes intersect." ;
			break ;
				
		case isEqual:
			return "The two bounding boxes are equal." ;
			break ;
				
		default:
			throw BoundingBoxException(
				"BoundingBox::InterpretIntersectionResult: "
				"unknown intersection outcome." ) ;
			break ;
				
	}
	
	return "(abnormal interpretation of intersection result)" ;
	
}


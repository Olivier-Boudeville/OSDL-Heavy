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


#include "OSDLBoundingBox2D.h"



using namespace Ceylan::Maths::Linear ;

using namespace OSDL::Engine ;

using std::string ;




BoundingBox2D::BoundingBox2D( Locatable2D & father, const Bipoint & center ) :
	Locatable2D( father )
{

	setCenter( center ) ;
	
}



BoundingBox2D::~BoundingBox2D() throw()
{

}



const string BoundingBox2D::toString( Ceylan::VerbosityLevels level ) const
{	

	return "2D bounding box, whose center is " + getCenter().toString( level )
		+ ". From a referential point of view, this is a(n) " 
		+ Locatable2D::toString( level ) ;
		
}



BoundingBox2D & BoundingBox2D::CheckIs2D( BoundingBox & box ) 
{

	BoundingBox2D * box2D = dynamic_cast<BoundingBox2D *>( & box ) ;
	
	if ( box2D == 0 )
		throw BoundingBoxException( 
			"BoundingBox2D::CheckIs2D : specified box ("
			+ box.toString() + ") was not a two dimensional box." ) ;
	
	return * box2D ;
		
}


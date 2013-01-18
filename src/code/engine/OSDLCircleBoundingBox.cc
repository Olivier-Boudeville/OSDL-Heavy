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


#include "OSDLCircleBoundingBox.h"



using namespace Ceylan::Maths::Linear ;

using namespace OSDL::Engine ;

using std::string ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>               // for OSDL_DEBUG_BOUNDING_BOX and al
#endif // OSDL_USES_CONFIG_H


#if OSDL_DEBUG_BOUNDING_BOX

using namespace Ceylan::Log ;
#define OSDL_BOX_LOG(message) LogPlug::debug( message ) ;

#else // OSDL_DEBUG_BOUNDING_BOX

#define OSDL_BOX_LOG(message)

#endif // OSDL_DEBUG_BOUNDING_BOX



using Ceylan::Maths::Real ;



CircleBoundingBox::CircleBoundingBox( Locatable2D & father, 
		const Bipoint & center, Real radius ) :
	BoundingBox2D( father, center ),
	_radius( radius )
{

}



CircleBoundingBox::~CircleBoundingBox() throw()
{

}



Real CircleBoundingBox::getRadius() const
{

	return _radius ;
	
}



void CircleBoundingBox::setRadius( Real newRadius )
{

	_radius = newRadius ;
	
}



const string CircleBoundingBox::toString( Ceylan::VerbosityLevels level ) const
{	

	string res = "Circular 2D bounding box, whose center is " 
		+ getCenter().toString( level )
		+ " and whose radius is " + Ceylan::toString( _radius ) ;
	
	if ( level == Ceylan::low )
		return res ;
	
	return res + " From a referential point of view, this is a(n) " 
		+ Locatable2D::toString( level ) ;	
			
}



IntersectionResult CircleBoundingBox::doesIntersectWith( BoundingBox & other ) 
{
	
	BoundingBox2D & other2D = BoundingBox2D::CheckIs2D( other ) ;
	
	// Here we know we have a 2D box.
	
	/*
	 * For the moment, in the 2D boxes family, only the circle is supported:
	 * (for example, a 2D square box would be rejected).
	 *
	 */
	CircleBoundingBox * circleBox = dynamic_cast<CircleBoundingBox *>( 
		& other2D ) ;
	
	if ( circleBox == 0 )
		throw BoundingBoxException( "CircleBoundingBox::doesIntersectWith: "
			"intersection with specified 2D box ("
			+other.toString() + ") is not implemented for the moment." ) ;
	
	return compareWith( * circleBox ) ;

}



IntersectionResult CircleBoundingBox::compareWith( CircleBoundingBox & other )
{
	
	/*
	 * The first concern is to have both boxes expressed in the same
	 * referential.
	 * One optimization could be to stop at the first common ancester in 
	 * the referential tree, instead of evaluating everything in the root
	 * referential.
	 * However, thanks to referential caching, the overhead should be small.
	 *
	 */
			
	/*
	 * Two circles, one case out of five to discriminate:
	 * @see IntersectionResult
	 * Let's first retrieve centers and radii.
	 *
	 */
	 
	OSDL_BOX_LOG( "Comparing " + toString() + " to " + other.toString() ) ;
	
	Matrix3 & firstMatrix = * dynamic_cast<Matrix3*>( 
		& getGlobalReferential() ) ;	
		
	Bipoint firstCenter( firstMatrix.getElementAt( 2, 0 ),
		firstMatrix.getElementAt( 2, 1 ) ) ;	
		
	OSDL_BOX_LOG( "First matrix: " + firstMatrix.toString() ) ;
	
	Matrix3 & secondMatrix = * dynamic_cast<Matrix3*>( 
		& other.getGlobalReferential() ) ;
		
	Bipoint secondCenter( secondMatrix.getElementAt( 2, 0 ),
		secondMatrix.getElementAt( 2, 1 ) ) ;
		
	OSDL_BOX_LOG( "Second matrix: " + secondMatrix.toString() ) ;
	
	/*
	 * Do not try to avoid square roots by comparing square distances, 
	 * since (a+b)² >= a²+b²:
	 *
	 */
	
	Real centerDistance = Bipoint::Distance( firstCenter, secondCenter ) ;
	
	OSDL_BOX_LOG( "Distance between centers: " 
		+ Ceylan::toString( centerDistance ) ) ;
		   
	if ( centerDistance >= _radius + other.getRadius() )
		return isSeparate ;
	
	// We know they intersect.
	
	/*
	 * Draw circles internally tangent to understand.
	 *
	 */ 
	if ( _radius > centerDistance + other.getRadius() )
		return contains ;
		
	if ( other.getRadius() > centerDistance + _radius )
		return isContained ;
	
	// Floating point values are almost never equal:
	if ( Ceylan::Maths::AreRelativelyEqual( centerDistance, 0.0f )
		&& Ceylan::Maths::AreRelativelyEqual( _radius, other.getRadius() ) )	
		return isEqual ;
	
	// Last possible case:
	return intersects ;
	
}


	
CircleBoundingBox & CircleBoundingBox::CheckIsCircle( BoundingBox & box ) 
{

	// Avoid calling CheckIs2D, it would be inefficient.
	
	CircleBoundingBox * circleBox = dynamic_cast<CircleBoundingBox *>( & box ) ;
	
	if ( circleBox == 0 )
		throw BoundingBoxException( 
			"CircleBoundingBox::CheckIsCircle: specified box ("
			+ box.toString() + ") was not a circular (two dimensional) box." ) ;
	
	return * circleBox ;
		
}
	

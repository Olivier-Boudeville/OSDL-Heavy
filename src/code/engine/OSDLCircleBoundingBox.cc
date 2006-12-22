#include "OSDLCircleBoundingBox.h"


using namespace Ceylan::Maths::Linear ;

using namespace OSDL::Engine ;

using std::string ;


#ifdef OSDL_DEBUG_BOUNDING_BOX

using namespace Ceylan::Log ;
#define OSDL_BOX_LOG(message) LogPlug::debug( message ) ;

#else

#define OSDL_BOX_LOG(message)

#endif


using Ceylan::Maths::Real ;


CircleBoundingBox::CircleBoundingBox( Locatable2D & father, const Bipoint & center, Real radius )
		throw() :
	BoundingBox2D( father, center ),
	_radius( radius )
{

}


CircleBoundingBox::~CircleBoundingBox() throw()
{

}


Real CircleBoundingBox::getRadius() const throw()
{
	return _radius ;
}


void CircleBoundingBox::setRadius( Real newRadius ) throw()
{
	_radius = newRadius ;
}


const string CircleBoundingBox::toString( Ceylan::VerbosityLevels level ) const throw()
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
	throw( BoundingBoxException )
{
	
	BoundingBox2D & other2D = BoundingBox2D::CheckIs2D( other ) ;
	
	// Here we know we have a 2D box.
	
	/*
	 * For the moment, in the 2D boxes family, only the circle is supported :
	 * (for example, a 2D square box would be rejected).
	 *
	 */
	CircleBoundingBox * circleBox = dynamic_cast<CircleBoundingBox *>( & other2D ) ;
	
	if ( circleBox == 0 )
		throw BoundingBoxException( "CircleBoundingBox::doesIntersectWith : "
			"intersection with specified 2D box ("
			+other.toString() + ") is not implemented for the moment." ) ;
	
	return compareWith( * circleBox ) ;

}


IntersectionResult CircleBoundingBox::compareWith( CircleBoundingBox & other ) throw() 
{
	
	/*
	 * The first concern is to have both boxes expressed in the same referential.
	 * One optimization could be to stop at the first common ancester in the referential tree,
	 * instead of evaluating everything in the root referential.
	 * However, thanks to referential caching, the overhead should be small.
	 */
			
	/*
	 * Two circles, one case out of five to discriminate :
	 * @see IntersectionResult
	 * Let's first retrieve centers and radii.
	 *
	 */
	 
	OSDL_BOX_LOG( "Comparing " + toString() + " to " + other.toString() ) ;
	
	Matrix3 & firstMatrix = * dynamic_cast<Matrix3*>( & getGlobalReferential() ) ;	
	Bipoint firstCenter( firstMatrix.getElementAt( 2, 0 ), firstMatrix.getElementAt( 2, 1 ) ) ;	
	OSDL_BOX_LOG( "First matrix : " + firstMatrix.toString() ) ;
	
	Matrix3 & secondMatrix = * dynamic_cast<Matrix3*>( & other.getGlobalReferential() ) ;
	Bipoint secondCenter( secondMatrix.getElementAt( 2, 0 ), secondMatrix.getElementAt( 2, 1 ) ) ;
	OSDL_BOX_LOG( "Second matrix : " + secondMatrix.toString() ) ;
	
	// Do not try to avoid square roots by comparing square distances, since (a+b)² >= a²+b²:
	
	Real centerDistance = Bipoint::Distance( firstCenter, secondCenter ) ;
	OSDL_BOX_LOG( "Distance between centers : " + Ceylan::toString( centerDistance ) ) ;
		   
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
	
	// Beware, on most cases distances are not null because of numerical errors :
	if ( centerDistance == 0 && _radius == other.getRadius() /* useless test, but safe */ )	
		return isEqual ;
	
	// Last possible case :
	return intersects ;
	
}

	
CircleBoundingBox & CircleBoundingBox::CheckIsCircle( BoundingBox & box ) 
	throw (BoundingBoxException)
{

	// Avoid calling CheckIs2D, would be inefficient.
	
	CircleBoundingBox * circleBox = dynamic_cast<CircleBoundingBox *>( & box ) ;
	
	if ( circleBox == 0 )
		throw BoundingBoxException( "CircleBoundingBox::CheckIsCircle : specified box ("
			+ box.toString() + ") was not a circular (two dimensional) box." ) ;
	
	return * circleBox ;
		
}
	

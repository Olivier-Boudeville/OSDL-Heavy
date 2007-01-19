#include "OSDLBoundingBox2D.h"


using namespace Ceylan::Maths::Linear ;

using namespace OSDL::Engine ;

using std::string ;



BoundingBox2D::BoundingBox2D( Locatable2D & father, const Bipoint & center )
		throw() :
	Locatable2D( father )
{
	setCenter( center ) ;
}


BoundingBox2D::~BoundingBox2D() throw()
{

}


const string BoundingBox2D::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{	

	return "2D bounding box, whose center is " + getCenter().toString( level )
		+ ". From a referential point of view, this is a(n) " 
		+ Locatable2D::toString( level ) ;
		
}


BoundingBox2D & BoundingBox2D::CheckIs2D( BoundingBox & box ) 
	throw( BoundingBoxException )
{

	BoundingBox2D * box2D = dynamic_cast<BoundingBox2D *>( & box ) ;
	
	if ( box2D == 0 )
		throw BoundingBoxException( 
			"BoundingBox2D::CheckIs2D : specified box ("
			+ box.toString() + ") was not a two dimensional box." ) ;
	
	return * box2D ;
		
}


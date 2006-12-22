#include "OSDLBoundingBox.h"

using namespace OSDL::Engine ;

using std::string ;



BoundingBoxException::BoundingBoxException( const string & reason ) throw() :
	EngineException( "BoundingBoxException : " + reason )
{

}


BoundingBoxException::~BoundingBoxException() throw()
{

}




BoundingBox::BoundingBox() throw()
{

}


BoundingBox::~BoundingBox() throw()
{

}


string BoundingBox::InterpertIntersectionResult( IntersectionResult result ) 
	throw( BoundingBoxException )
{

	switch( result )
	{
	
		case isSeparate :
			return "The two bounding boxes have no intersection." ;
			break ;
				
		case contains :
			return "The first bounding box contains strictly the second." ;
			break ;
				
		case isContained :
			return "The first bounding box is strictly contained by the second." ;
			break ;
				
		case intersects :
			return "The two bounding boxes intersect." ;
			break ;
				
		case isEqual :
			return "The two bounding boxes are equal." ;
			break ;
				
		
		default:
			throw BoundingBoxException( "BoundingBox::InterpertIntersectionResult : "
				"unknown intersection outcome." ) ;
			break ;
				
	}
	
	return "(abnormal interpretation of intersection result)" ;
	
}


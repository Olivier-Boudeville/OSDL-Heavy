#include "OSDLPolygon.h"

#include "OSDLSurface.h"  // for Surface
#include "OSDLPoint2D.h"  // for Point2D
#include "OSDLVideo.h"    // for VideoModule

#include "OSDLVideo.h"    // for VideoModule


#include "SDL_gfxPrimitives.h"  // for graphic primitives exported by SDL_gfx




using std::string ;
using std::list ;

using namespace Ceylan::Maths ;
using namespace OSDL::Video ;


using Ceylan::Maths::Real ;


// A return code of 0 for SDL_gfx functions means no error, contrary to -1.



bool TwoDimensional::drawPie( Surface & targetSurface, 
	Coordinate xCenter, Coordinate yCenter, Length radius, 
	Ceylan::Maths::AngleInDegrees angleStart, Ceylan::Maths::AngleInDegrees angleStop,
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha ) throw()
{

	return ( ::filledPieRGBA( & targetSurface.getSDLSurface(), xCenter, yCenter,
			radius, static_cast<Coordinate>( angleStart ), static_cast<Coordinate>( angleStop ),
			red, green, blue, alpha ) == 0 ) ;
		
}


bool TwoDimensional::drawPie( Surface & targetSurface, 
	Coordinate xCenter, Coordinate yCenter, Length radius, 
	Ceylan::Maths::AngleInDegrees angleStart, Ceylan::Maths::AngleInDegrees angleStop, 
	Pixels::ColorDefinition colorDef ) throw()
{

	return ( ::filledPieColor( & targetSurface.getSDLSurface(), xCenter, yCenter,
			radius, static_cast<Coordinate>( angleStart ), static_cast<Coordinate>( angleStop ),
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;

}



bool TwoDimensional::drawTriangle( Surface & targetSurface, 
	Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2, Coordinate x3, Coordinate y3, 
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha, bool filled ) throw()
{

	if ( filled )
	{
				
		return ( ::filledTrigonRGBA( & targetSurface.getSDLSurface(), x1, y1, x2, y2, x3, y3,
			red, green, blue, alpha ) == 0 ) ;
			
	}
	else
	{
		
		if ( VideoModule::GetAntiAliasingState() )
		{
			return ( ::aatrigonRGBA( & targetSurface.getSDLSurface(), x1, y1, x2, y2, x3, y3,
				red, green, blue, alpha ) == 0 ) ;
			
		}
		else
		{
			return ( ::trigonRGBA( & targetSurface.getSDLSurface(), x1, y1, x2, y2, x3, y3,
				red, green, blue, alpha ) == 0 ) ;
		
		}
	
	}

}

	
bool TwoDimensional::drawTriangle( Surface & targetSurface, 
	Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2, Coordinate x3, Coordinate y3, 
	Pixels::ColorDefinition colorDef, bool filled ) throw()
{

	if ( filled )
	{
				
		return ( ::filledTrigonColor( & targetSurface.getSDLSurface(), x1, y1, x2, y2, x3, y3,
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
			
	}
	else
	{
		
		if ( VideoModule::GetAntiAliasingState() )
		{
			return ( ::aatrigonColor( & targetSurface.getSDLSurface(), x1, y1, x2, y2, x3, y3,
				Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
			
		}
		else
		{
			return ( ::trigonColor( & targetSurface.getSDLSurface(), x1, y1, x2, y2, x3, y3,
				Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
		
		}
	
	}

}

	

bool TwoDimensional::drawTriangle( Surface & targetSurface, 
	const Point2D & p1, const Point2D & p2, const Point2D & p3,
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha, bool filled ) throw()
{

	if ( filled )
	{
				
		return ( ::filledTrigonRGBA( & targetSurface.getSDLSurface(), 
			p1.getX(), p1.getY(),p2.getX(),p2.getY(), p3.getX(),p3.getY(),
			red, green, blue, alpha ) == 0 ) ;
			
	}
	else
	{
		
		if ( VideoModule::GetAntiAliasingState() )
		{
			return ( ::aatrigonRGBA( & targetSurface.getSDLSurface(), 			
				p1.getX(), p1.getY(),p2.getX(),p2.getY(), p3.getX(),p3.getY(),
				red, green, blue, alpha ) == 0 ) ;
			
		}
		else
		{
			return ( ::trigonRGBA( & targetSurface.getSDLSurface(), 
				p1.getX(), p1.getY(),p2.getX(),p2.getY(), p3.getX(),p3.getY(),
				red, green, blue, alpha ) == 0 ) ;
		
		}
	
	}

}

	
bool TwoDimensional::drawTriangle( Surface & targetSurface, 
	const Point2D & p1, const Point2D & p2, const Point2D & p3, 
	Pixels::ColorDefinition colorDef, bool filled ) throw()
{

	if ( filled )
	{
				
		return ( ::filledTrigonColor( & targetSurface.getSDLSurface(), 
			p1.getX(), p1.getY(),p2.getX(),p2.getY(), p3.getX(),p3.getY(),
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
			
	}
	else
	{
		
		if ( VideoModule::GetAntiAliasingState() )
		{
			return ( ::aatrigonColor( & targetSurface.getSDLSurface(), 
				p1.getX(), p1.getY(),p2.getX(),p2.getY(), p3.getX(),p3.getY(),
				Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
			
		}
		else
		{
			return ( ::trigonColor( & targetSurface.getSDLSurface(),
				p1.getX(), p1.getY(),p2.getX(),p2.getY(), p3.getX(),p3.getY(),
				Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
		
		}
	
	}

}


bool TwoDimensional::drawPolygon( Surface & targetSurface, const listPoint2D & summits,
	Coordinate x, Coordinate y,
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha, bool filled ) throw()
{
	
	
	// First, prepare data structure.
	
	
	/*
	 * If a large number of summits is to be used, dynamic allocation (new/delete) would be better.
	 *
	 */
	Ceylan::System::Size vertexCount = summits.size() ;
	 
	Coordinate abscissaArray[ vertexCount ] ;
	Coordinate ordinateArray[ vertexCount ] ;
	
	vertexCount = 0 ;
	
	for ( listPoint2D::const_iterator it = summits.begin(); it != summits.end(); it++ )
	{
		abscissaArray[ vertexCount ] = (*it)->getX() + x ;
		ordinateArray[ vertexCount ] = (*it)->getY() + y ;
		
		vertexCount++ ;
	}
	
		
	// Then, draws it.
	
	if ( filled )
	{
				
		return ( ::filledPolygonRGBA( & targetSurface.getSDLSurface(), 
			abscissaArray, ordinateArray, vertexCount, red, green, blue, alpha ) == 0 ) ;
			
	}
	else
	{
		
		if ( VideoModule::GetAntiAliasingState() )
		{
			return ( ::aapolygonRGBA( & targetSurface.getSDLSurface(), 
				abscissaArray, ordinateArray, vertexCount, red, green, blue, alpha ) == 0 ) ;
			
		}
		else
		{
			return ( ::polygonRGBA( & targetSurface.getSDLSurface(),
				abscissaArray, ordinateArray, vertexCount, red, green, blue, alpha ) == 0 ) ;
		
		}
	
	}

}	
	
	
bool TwoDimensional::drawPolygon( Surface & targetSurface, const listPoint2D & summits,
	Coordinate x, Coordinate y,	Pixels::ColorDefinition colorDef, bool filled ) throw()
{


	/*
	 * If a large number of summits is to be used, dynamic allocation (new/delete) would be better.
	 *
	 */
	Ceylan::System::Size vertexCount = summits.size() ;
	 
	Coordinate abscissaArray[ vertexCount ] ;
	Coordinate ordinateArray[ vertexCount ] ;
	
	vertexCount = 0 ;
	
	for ( listPoint2D::const_iterator it = summits.begin(); it != summits.end(); it++ )
	{
		abscissaArray[ vertexCount ] = (*it)->getX() + x ;
		ordinateArray[ vertexCount ] = (*it)->getY() + y ;
		
		vertexCount++ ;
	}
	
			
	// Then, draws it.
	
	if ( filled )
	{
				
		return ( ::filledPolygonColor( & targetSurface.getSDLSurface(), 
			abscissaArray, ordinateArray, vertexCount,
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
			
	}
	else
	{
		
		if ( VideoModule::GetAntiAliasingState() )
		{
			return ( ::aapolygonColor( & targetSurface.getSDLSurface(), 
				abscissaArray, ordinateArray, vertexCount,
				Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
			
		}
		else
		{
			return ( ::polygonColor( & targetSurface.getSDLSurface(),
				abscissaArray, ordinateArray, vertexCount,
				Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
		
		}
	
	}
	
}




	
using namespace OSDL::Video::TwoDimensional ;



Polygon::Polygon( listPoint2D & summits, bool listOwner ) throw() :
	Locatable2D(),
	_summits( & summits ),
	_listOwner( listOwner )
{

}


Polygon::~Polygon() throw()
{
		
		
	if ( _summits != 0 )
	{
	
		if ( _listOwner )
		{
		 
			for ( listPoint2D::iterator it = _summits->begin(); it != _summits->end(); it++ )
			{
				delete *it ;
			}

			// Even the list should be deallocated, <b>only</b> if list owner  :
			delete _summits ;
			
		}
				
		
	}	
	
}


bool Polygon::draw( Surface & targetSurface, Pixels::ColorDefinition colorDef, bool filled )
	const throw()
{

	listPoint2D * vertices = & Duplicate( *_summits ) ;
	Apply( getLocalMatrix(), * vertices ) ;
	
	/*
	 * [0;0] origin specified since center must have been taken into account within the homogeneous
	 * matrix :
	 *
	 */
	return drawPolygon( targetSurface, * vertices, 0, 0, colorDef, filled ) ;
	
}


listPoint2D & Polygon::getPoints() const throw()
{
	return * _summits ;
}


void Polygon::setPoints( listPoint2D & newList ) throw()
{

	if ( _listOwner )
	{
		if ( _summits != 0 )
			Delete( * _summits ) ;
	}

	_summits = & newList ;	
		
}


bool Polygon::isListOwner() const throw()
{
	return _listOwner ;
}

 	
const string Polygon::toString( Ceylan::VerbosityLevels level ) const throw()
{

	string res ;

	if ( _summits && ! _summits->empty() )
	{

		res = "Polygon defined by " + Ceylan::toString( _summits->size() ) + " summits" ;

		if ( level == Ceylan::low )
			return res ;
		
		res += ". Its summits are : " ;
			
		list<string> summitsCoordinates ;
		
		Ceylan::Uint32 count = 0 ;
		
		for ( listPoint2D::const_iterator it = _summits->begin(); it != _summits->end(); it++ )
		{
			count++ ;
			summitsCoordinates.push_back( "summit #" + Ceylan::toString( count ) + " : " 
				+ (*it)->toString( level ) ) ;				
		}
		
		res += Ceylan::formatStringList( summitsCoordinates ) ; 
		
		return res + "This polygon has for referential : " 
			+ Locatable2D::toString( level ) ;
			
	}
	
	return "Void polygon (no summit registered)" ;

}



// Static section.


Polygon & Polygon::CreateFlakeBranch( Length length, Length thickness, AngleInDegrees childAngle,
	Ratio branchingHeightRatio, Ratio scale ) throw()
{
	
	/*
	 * Points will be created, put in a list that will have to be deallocated explicitly by the 
	 * caller.
	 *
	 */
	
	AngleInRadians realAngle = Ceylan::Maths::DegreeToRadian( childAngle ) ;
	
	// Order of simpliest computation :
		
	Point2D * alpha = & Point2D::CreateFrom( 0, length ) ;
	Point2D * beta  = & Point2D::CreateFrom( thickness / 2, length ) ;

	Point2D * nu = & Point2D::CreateFrom( thickness / 2, 0 ) ;
	Point2D * mu = & Point2D::CreateFrom( thickness / 2, 
		length * branchingHeightRatio ) ;
		
	Point2D * omicron = & Point2D::CreateFrom( 0, 0 ) ;
		
	// gamma computed from the bottom :
	Point2D * gamma = & Point2D::CreateFrom( thickness / 2, 
		length * branchingHeightRatio + thickness * scale / Sin( realAngle ) ) ;

	// epsilon calculated from mu :
	Point2D * epsilon = & Point2D::CreateFrom( 
		thickness / 2 + length * scale * Sin( realAngle ),
		length * branchingHeightRatio  + scale * length * Cos( realAngle ) ) ;
		
	// delta calculated from epsilon :
	Point2D * delta = & Point2D::CreateFrom( 
		epsilon->getX() - thickness * scale * Cos( realAngle ), 
		epsilon->getY() + thickness * scale * Sin( realAngle )  ) ;
		
	listPoint2D * summits = new listPoint2D ;
		
	summits->push_back( alpha ) ;
	summits->push_back( beta ) ;
	summits->push_back( gamma ) ;
	summits->push_back( delta ) ;
	summits->push_back( epsilon ) ;
	summits->push_back( mu ) ;
	summits->push_back( nu ) ;
	summits->push_back( omicron ) ;
	
	
	 /*
	  * Useful for debugging purpose on a test application :
	  *
	  
	 screen.drawCross( alpha,	Red ) ;
	 screen.drawCross( beta,	Blue ) ;
	 screen.drawCross( gamma,	Green ) ;
	 screen.drawCross( delta,	White ) ;
	 screen.drawCross( epsilon, Grey ) ;
	 screen.drawCross( mu,  	Violet ) ;
	 screen.drawCross( nu,  	Brown ) ;
	 screen.drawCross( omicron, Cyan ) ;
	 
	  *
	  */
		
	// Copy constructors :
		
	Point2D * alpha1 = new Point2D( * alpha ) ;
	alpha1->flipX() ;
		
	Point2D * beta1 = new Point2D( * beta ) ;
	beta1->flipX() ;
		
	Point2D * gamma1 = new Point2D( * gamma ) ;
	gamma1->flipX() ;
		
	Point2D * delta1 = new Point2D( * delta ) ;
	delta1->flipX() ;
		
	Point2D * epsilon1 = new Point2D( * epsilon ) ;
	epsilon1->flipX() ;
		
	Point2D * mu1 = new Point2D( * mu ) ;
	mu1->flipX() ;
		
	Point2D * nu1 = new Point2D( * nu ) ;
	nu1->flipX() ;
	
	// Mere copy to avoid double referenced points, since the polygon may own its points.
	Point2D * omicron1 = new Point2D( * omicron ) ;

	summits->push_back( alpha1 ) ;
	summits->push_back( beta1 ) ;
	summits->push_back( gamma1 ) ;
	summits->push_back( delta1 ) ;
	summits->push_back( epsilon1 ) ;
	summits->push_back( mu1 ) ;
	summits->push_back( nu1 ) ;
	summits->push_back( omicron1 ) ;
	
	// Neither the summit list nor its points are owned by this polygon :
	return * new Polygon( * summits, /* pointOwner */ false ) ;

}


listPoint2D & Polygon::Duplicate( const listPoint2D & source ) throw()
{

	listPoint2D * newList = new listPoint2D ;
	
	for ( listPoint2D::const_iterator it = source.begin(); it != source.end(); it++ )
	{
		newList->push_back( new Point2D( * (*it) ) ) ;
	}
	return * newList ;
	
}


void Polygon::Delete( listPoint2D & listToBeDeleted ) throw()
{

	for ( listPoint2D::iterator it = listToBeDeleted.begin(); it != listToBeDeleted.end(); it++ )
	{
		delete (*it) ;
	}
	
	delete & listToBeDeleted ;

}


listPoint2D & Polygon::Append( listPoint2D & toBeAugmented, const listPoint2D & toAppend ) throw()
{

	for ( listPoint2D::const_iterator it = toAppend.begin(); it != toAppend.end(); it++ )
	{
		toBeAugmented.push_back( (*it) ) ;
	}
	
	// Non-necessary but useful for chaining :
	
	return toBeAugmented ;
	
}


listPoint2D & Polygon::Apply( const Linear::HomogeneousMatrix3 & transformation, 
	listPoint2D & sourceList ) throw()
{

	for ( listPoint2D::iterator it = sourceList.begin(); it != sourceList.end(); it++ )
	{
	
		// Linear operations performed on floating-point Vector2, not on integer Point2D :
		Linear::Vector2 temp( static_cast<Real>( (*it)->getX()), 
			static_cast<Real>( (*it)->getY()) ) ;
		(*it)->setFrom( transformation * temp ) ;
	}

	// Non-necessary but useful for chaining :
	
	return sourceList ;

}




// Polygon set.
						

PolygonSet::PolygonSet( bool listOwner ) throw() :
	Locatable2D(),
	_polygonList( 0 ),
	_listOwner( listOwner )
{

}


PolygonSet::PolygonSet( std::list<listPoint2D *> & polygonList, bool listOwner ) throw():
	Locatable2D(),
	_polygonList( & polygonList ),
	_listOwner( listOwner )
{

}


PolygonSet::~PolygonSet() throw()
{

	if ( _polygonList != 0 )
	{
	
		if ( _listOwner )
		{
		 
		 	// Deletes both lists and point in lists :
			for ( list<listPoint2D *>::iterator it = _polygonList->begin(); 
				it != _polygonList->end(); it++ )
			{
				Polygon::Delete( * (*it) ) ;
			}

			delete _polygonList ;
	
		}
				
	}	
	
}


void PolygonSet::addPointsOf( Polygon & newPolygon ) throw()
{

	if ( newPolygon.isListOwner() )
		Ceylan::emergencyShutdown( "PolygonSet::addPointsOf : "
			"added polygon should not own its points." ) ;
	
	addPointList( newPolygon.getPoints() ) ;	
	
}


void PolygonSet::addPointList( listPoint2D & listToAdd ) throw()
{

	if ( _polygonList == 0 )
		_polygonList = new list<listPoint2D *> ;
	_polygonList->push_back( & listToAdd ) ;
	
}


bool PolygonSet::draw( Surface & targetSurface, Coordinate x, Coordinate y,
	Pixels::ColorDefinition colorDef, bool filled )	const throw()
{

	bool result = true ;
	
	for ( list<listPoint2D *>::const_iterator it = _polygonList->begin(); 
		it != _polygonList->end(); it++ )
	{
	
		/*
		 * Avoid result = result && (*it)->draw( targetSurface, x, y, colorDef,* (*it) ) ;
		 * since if a drawing had to fail, next ones would not be attempted (shortcut boolean 
		 * evaluation).
		 *
		 */
		 
		if ( drawPolygon( targetSurface, * (*it), x, y, colorDef, filled ) == false )
			result = false ; 
	}
	
	return result ;
	
}


	
const string PolygonSet::toString( Ceylan::VerbosityLevels level ) const throw()
{

	string res ;

	if ( _polygonList && ( ! _polygonList->empty() ) )
	{

		res = "Polygon set made of " + Ceylan::toString( _polygonList->size() ) 
			+ " separate polygons" ;

		if ( level == Ceylan::low )
			return res ;
		
		res += ". Its polygons are : " ;
			
		list<string> summitsCoordinates ;
		
		unsigned int polygonCount = 0 ;
		unsigned int summitCount ;
				
		for ( list<listPoint2D *>::const_iterator itPolygonList = _polygonList->begin(); 
			itPolygonList != _polygonList->end(); itPolygonList++ )
		{	
			
			polygonCount++ ;
			
			res += "For polygon #" + Ceylan::toString( polygonCount ) + " : " ;
			 
			summitCount = 0 ;
			summitsCoordinates.clear() ;
			
			for ( listPoint2D::iterator it = (*itPolygonList)->begin(); 
				it != (*itPolygonList)->end(); it++ )
			{
				summitCount++ ;
				summitsCoordinates.push_back( "summit #" + Ceylan::toString( summitCount ) + " : " 
					+ (*it)->toString( level ) ) ;				
			}
		
			res += Ceylan::formatStringList( summitsCoordinates ) ; 
		
		}	
		
		return res + "This polygon set has for referential : " 
			+ Locatable2D::toString( level ) ;
	}
	
	return "Void set of polygons (no polygon registered)" ;

}


PolygonSet & PolygonSet::CreateFlake( Ceylan::Uint8 branchCount, Length length, Length thickness, 
	Ceylan::Maths::AngleInDegrees childAngle, Ratio branchingHeightRatio, Ratio scale ) throw()
{

	if ( branchCount == 0 )
		Ceylan::emergencyShutdown( "PolygonSet::CreateFlake : "
			"snow flakes should have at least one branch." ) ;
			
	// Flake branches do not own their points.		
	Polygon & modelBranch = Polygon::CreateFlakeBranch( length, thickness, childAngle,
		branchingHeightRatio, scale ) ;
	
	// List owner ensures all points and lists will be deleted when appropriate :
	PolygonSet & result = * new PolygonSet( /* listOwner */ true ) ;	
		

	// Creates the relevant transformation for branches (they are evenly placed) :
	Linear::HomogeneousMatrix3 transformation( 360 / branchCount, Linear::Vector2( 0, 0 ) ) ;

	// Creates first branch element :
	listPoint2D * lastlyCreatedBranchPoints = & modelBranch.getPoints() ;
	result.addPointList( * lastlyCreatedBranchPoints ) ;
	
	listPoint2D * newlyCreatedBranchPoints ;
	
	// Rotates in turn each of the other branches until a complete circle is made :	
	for ( Ceylan::Uint8 count = 1; count < branchCount; count++ )
	{
	
		newlyCreatedBranchPoints = & Polygon::Apply( transformation, 
			Polygon::Duplicate( * lastlyCreatedBranchPoints ) ) ;
			
		result.addPointList( * newlyCreatedBranchPoints ) ;
		lastlyCreatedBranchPoints = newlyCreatedBranchPoints ;
	
	}
	
	delete & modelBranch ;
	
	/*
	 * All lists and points (either coming from 'modelBranch' or duplicated) are registered in the
	 * polygon set, that will deallocate each of them upon its own deallocation.
	 *
	 */
	return result ;
	
}



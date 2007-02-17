#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Events ;
using namespace OSDL::Engine ;

using namespace Ceylan::Log ;
using namespace Ceylan::Maths::Linear ;


using Ceylan::Maths::Real ;


#include <list>
using std::list ;



/**
 * Test of OSDL circular bounding boxes.
 *
 * @see CircleBoundingBox
 *
 */
 
 
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Testing OSDL circular bounding boxes." ) ;

		// Check all combinations of intersections.

		// Create a common absolutly defined father for the two boxes : 
		Ceylan::Locatable2D father ;
		
		Real radius = 30 ;
		
		// Create the first bounding box.
		CircleBoundingBox first(  father, Bipoint( 0, 0 ), radius ) ;
		LogPlug::info( "First box is : " + first.toString() ) ;
		
		CircleBoundingBox second( father, Bipoint(), radius) ;
		
		Real limitRadius = first.getRadius() + second.getRadius() ;
		
		// Make the second box move until it collides.

		LogPlug::info( "First test involves boxes of same size." ) ;
			
		for ( Real x = -100; x < 100 ; x += 5 )
		{
			second.setCenter( x, 0 ) ;
			LogPlug::info( "Second box is " + second.toString( Ceylan::low ) 
				+ ". "
				+ BoundingBox::InterpretIntersectionResult(
					first.doesIntersectWith( second ) ) ) ;
					
			if ( x < -limitRadius && 
					first.doesIntersectWith( second ) != isSeparate )
				LogPlug::error( "First test : for a second box abscissa of " 
					+ Ceylan::toString( x ) 
					+ ", there should be no intersection." ) ;
					
			if ( x > -limitRadius && x < limitRadius 
					&& ( ! Ceylan::Maths::IsNull( x ) ) 
					&& first.doesIntersectWith( second ) != intersects )
				LogPlug::error( "First test : for a second box abscissa of " 
					+ Ceylan::toString( x ) 
					+ ", there should be intersection." ) ;
					
			if ( Ceylan::Maths::IsNull( x ) 
					&& first.doesIntersectWith( second ) != isEqual )
				LogPlug::error( "First test : for a second box abscissa of " 
					+ Ceylan::toString( x ) + ", there should be equality." ) ;
					
			if ( x > limitRadius 
					&& first.doesIntersectWith( second ) != isSeparate )
				LogPlug::error( "First test : for a second box abscissa of " 
					+ Ceylan::toString( x ) + 
					", there should be no intersection." ) ;				
		}
		


		LogPlug::info( "Second test involves a second box "
			"smaller than the first." ) ;

		first.setRadius( 50 ) ;
		second.setRadius( 20 ) ;
		
		Real firstLimitRadius  = first.getRadius() + second.getRadius() ;
		Real secondLimitRadius = Ceylan::Maths::Max( first.getRadius(),
				second.getRadius() ) 
			- 2 * Ceylan::Maths::Min( first.getRadius(), second.getRadius() ) ;
		
		LogPlug::info( "(first limit is " 
			+ Ceylan::toString( firstLimitRadius ) + ", second is "
			+ Ceylan::toString( secondLimitRadius ) + ")" ) ;
			
		// Make the second box move until it collides.
			
		for ( Real x = -100; x < 100 ; x += 5 )
		{
		
			second.setCenter( x, 0 ) ;
			LogPlug::info( "Second box is " + second.toString( Ceylan::low ) 
				+ ". " + BoundingBox::InterpretIntersectionResult(
					first.doesIntersectWith( second ) ) ) ;
					
			if ( x < -firstLimitRadius 
					&& first.doesIntersectWith( second ) != isSeparate )
				LogPlug::error( "Second test : for a second box abscissa of " 
					+ Ceylan::toString( x ) 
					+ ", there should be no intersection." ) ;
			
			if ( x > -secondLimitRadius && x < secondLimitRadius  
					&& first.doesIntersectWith( second ) != contains )
				{	
					LogPlug::error( "For a second box abscissa of " 
						+ Ceylan::toString( x ) 
						+ ", first should contain the second." ) ;
					break ;
				}
					
			if ( x > -firstLimitRadius && x < firstLimitRadius 
					&& x < -secondLimitRadius && x > secondLimitRadius
					&& first.doesIntersectWith( second ) != intersects )
				LogPlug::error( "Second test : for a second box abscissa of " 
					+ Ceylan::toString( x ) 
					+ ", there should be intersection." ) ;
					
			if ( x > firstLimitRadius 
					&& first.doesIntersectWith( second ) != isSeparate )
				LogPlug::error( "Second test : for a second box abscissa of " 
					+ Ceylan::toString( x ) 
					+ ", there should be no intersection." ) ;				
		}
		

		LogPlug::info( "Third test involves a second box bigger than first." ) ;

		first.setRadius( 20 ) ;
		second.setRadius( 30 ) ;
		
		firstLimitRadius  = first.getRadius() + second.getRadius() ;
		secondLimitRadius = Ceylan::Maths::Max( first.getRadius(),
				second.getRadius() )
			- Ceylan::Maths::Min( first.getRadius(), second.getRadius() ) ;


		LogPlug::info( "(first limit is " + Ceylan::toString( firstLimitRadius )
			+ ", second is " + Ceylan::toString( secondLimitRadius ) + ")" ) ;
		
		// Make the second box move until it collides.
			
		for ( Real x = -100; x < 100 ; x += 5 )
		{
			second.setCenter( x, 0 ) ;
			LogPlug::info( "Second box is " + second.toString( Ceylan::low ) 
				+ ". " + BoundingBox::InterpretIntersectionResult(
					first.doesIntersectWith( second ) ) ) ;
					
			if ( x < -firstLimitRadius 
					&& first.doesIntersectWith( second ) != isSeparate )
				LogPlug::error( "Third test : for a second box abscissa of " 
					+ Ceylan::toString( x ) 
					+ ", there should be no intersection." ) ;
			
			if ( x > -secondLimitRadius && x < secondLimitRadius 
					&& first.doesIntersectWith( second ) != isContained )
				{	
					LogPlug::error( "Third test : for a second box abscissa of "
						+ Ceylan::toString( x ) 
						+ ", first should contained in second." ) ;
					break ;
				}
					
			if ( x > -firstLimitRadius && x < firstLimitRadius 
					&& x < -secondLimitRadius && x > secondLimitRadius 
					&& first.doesIntersectWith( second ) != intersects )
				LogPlug::error( "Third test : for a second box abscissa of " 
					+ Ceylan::toString( x ) 
					+ ", there should be intersection." ) ;
					
			if ( x > firstLimitRadius 
					&& first.doesIntersectWith( second ) != isSeparate )
				LogPlug::error( "Third test : for a second box abscissa of " 
					+ Ceylan::toString( x ) 
					+ ", there should be no intersection." ) ;				
		}
						
		LogPlug::info( "stopping OSDL." ) ;		
		
		LogPlug::info( "End of OSDL circular bounding boxes test." ) ;

   }
	
    catch ( const OSDL::Exception & e )
    {
	
        LogPlug::error( "OSDL exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught : " 
			 + std::string( e.what() ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
	
        LogPlug::error( "Unknown exception caught" ) ;
       	return Ceylan::ExitFailure ;

    }

    return Ceylan::ExitSuccess ;

}


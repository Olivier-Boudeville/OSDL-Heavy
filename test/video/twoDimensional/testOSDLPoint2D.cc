#include "OSDL.h"
using namespace OSDL::Video::TwoDimensional ;

using namespace Ceylan::Log ;

#include <string>



/**
 * Small usage tests for Point2D.
 *
 */
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL Point2D" ) ;	

        Point2D p1( static_cast<OSDL::Video::Coordinate>( 14 ), 34 ) ;
        
		LogPlug::info( "Point located at ( 14 ; 34 ) displays as " 
			+ p1.toString() ) ;
 
 		LogPlug::info( "Getting first coordinate : "  
			+ Ceylan::toString( p1.getX() ) ) ;
			
 		LogPlug::info( "Getting second coordinate : " 
			+ Ceylan::toString( p1.getY() ) ) ;
		
 		LogPlug::info( "Setting first coordinate : 11 " ) ;
		p1.setX( 11 ) ;
		
 		LogPlug::info( "Setting second coordinate : 108 " ) ;
		p1.setY( 108 ) ;
		
 		LogPlug::info( "Getting first coordinate : "  
			+ Ceylan::toString( p1.getX() ) ) ;
 		LogPlug::info( "Getting second coordinate : " 
			+ Ceylan::toString( p1.getY() ) ) ;
		
		LogPlug::info( "Point now displays as " 
			+ p1.toString() ) ;
				
		LogPlug::info( "End of OSDL Point2D test" ) ;
		
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


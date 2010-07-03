/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Events ;


using namespace Ceylan::Log ;
using namespace Ceylan::Maths::Linear ;


#include <string>




/**
 * Small usage tests for very basic animation of a sunrise.
 *
 */

Length screenWidth  = 640 ;
Length screenHeight = 480 ;
  
  
/*
 
class Sun: public ActiveObject
{

	public:
	
		Sun( Events::Milliseconds startingTime, Point2D & origin, 
				Length initialRadius,
				Pixels::ColorDefinition initialColor ) throw():
			TimedDrawable( startingTime ),			
			_origin( origin.getX(), origin.getY() ),
			_currentPosition( origin ),
			_initialRadius( initialRadius ),
			_initialColor( initialColor )
		{
			
			
		}	

		virtual ~Sun() throw()
		{
		
		}
		
		
		virtual void updatePosition() throw()
		{
			_currentPosition.setTo( 
				_origin.getX()  +     EventsModule::getSimulationTime(),
				_origin.getX() + 2 * EventsModule::getSimulationTime() ) ;			
		}
		
		
		virtual void draw( Surface & targetSurface ) throw()
		{
			updatePosition() ;
			targetSurface.drawCircle( _currentPosition.getX(), 
				_currentPosition.getY(), _initialRadius, _initialColor ) ;
		}
		
		
	protected:
	
		Bipoint _origin ;
		Point2D _currentPosition ;
		Length _initialRadius ;
		ColorDefinition _initialColor ;
		
} ;


*/
 
 
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL Animation" ) ;	


		bool isBatch = false ;
		
		std::string executableName ;
		std::list<std::string> options ;
		
		Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;
		
		std::string token ;
		bool tokenEaten ;
		
		
		while ( ! options.empty() )
		{
		
			token = options.front() ;
			options.pop_front() ;

			tokenEaten = false ;
						
			if ( token == "--batch" )
			{
			
				LogPlug::info( "Batch mode selected" ) ;
				isBatch = true ;
				tokenEaten = true ;
			}
			
			if ( token == "--interactive" )
			{
				LogPlug::info( "Interactive mode selected" ) ;
				isBatch = false ;
				tokenEaten = true ;
			}
			
			if ( token == "--online" )
			{
				// Ignored:
				tokenEaten = true ;
			}
			
			if ( LogHolder::IsAKnownPlugOption( token ) )
			{
				// Ignores log-related (argument-less) options.
				tokenEaten = true ;
			}
			
			
			if ( ! tokenEaten )
			{
				throw Ceylan::CommandLineParseException( 
					"Unexpected command line argument: " + token ) ;
			}
		
		}
		
					
		
    	LogPlug::info( "Prerequisite: initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseEvents ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
		// Nothing for the moment.
							
		screen.update() ;	
		
		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;

		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;
				
		LogPlug::info( "End of OSDL Animation test" ) ;
		
    }
	
    catch ( const OSDL::Exception & e )
    {
	
        LogPlug::error( "OSDL exception caught: "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught: "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught: " 
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


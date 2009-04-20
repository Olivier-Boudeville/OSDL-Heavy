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
using namespace OSDL::Video::Pixels ;

using namespace Ceylan::Log ;


#include <string>



/**
 * Small usage tests for circles.
 *
 * @see testOSDLConic.cc
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL Circle" ) ;	
		

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


    	LogPlug::info( "Pre requesite: initializing the display" ) ;	
	         		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseEvents ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
    	LogPlug::info( "Drawing random coloured circles, discs, "
			"ellipses, filled or not." ) ;		
 		
		screen.lock() ;		
	
    	LogPlug::info( "Pre requesite: having three random generators" ) ;	
		
		Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0, 
			screenWidth ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0, 
			screenHeight ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator radiusRand( 0, 100 ) ;
		
		// Returns 0 or 1, as a coin:
		Ceylan::Maths::Random::WhiteNoiseGenerator coinRand( 0, 2 ) ;
		
		Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
	
		Coordinate x, y ;
			
		ColorElement red ;
		ColorElement green ;
		ColorElement blue ;
		ColorElement alpha ;
	
	
		for ( Ceylan::Uint32 i = 0; i < 200; i++ )
		{
								
			x = abscissaRand.getNewValue() ;			
			y = ordinateRand.getNewValue() ;
			
			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;
			
			if ( coinRand.getNewValue() == 0 )					
				screen.drawCircle( x, y, radiusRand.getNewValue(), 
					red, green, blue, alpha, 
					/* filled */ static_cast<bool>( coinRand.getNewValue() ) ) ;
			else
				screen.drawDiscWithEdge( x, y, 
					radiusRand.getNewValue(), radiusRand.getNewValue(), 
					Pixels::convertRGBAToColorDefinition(
						colorRand.getNewValue(), colorRand.getNewValue(), 
						colorRand.getNewValue(), colorRand.getNewValue() ), 
					Pixels::convertRGBAToColorDefinition(
						colorRand.getNewValue(), colorRand.getNewValue(), 
						colorRand.getNewValue(), colorRand.getNewValue() )
				 ) ;
					
		}
	
		screen.unlock() ;
				
		screen.update() ;	
		
		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;
			
				
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		LogPlug::info( "End of OSDL Circle test" ) ;
		
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


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
 * Small usage tests for Lines.
 *
 */
int main( int argc, char * argv[] ) 
{

	LogHolder myLog( argc, argv ) ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL lines." ) ;	


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
				// Ignored :
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
					"Unexpected command line argument : " + token ) ;
			}
		
		}

		
    	LogPlug::info( "Prerequisite : initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
				
    	LogPlug::info( "Drawing vertical lines" ) ;	
		
		screen.lock() ;

		for ( Ceylan::Uint32 x = 0; x < 250; x++ )
		{
					
			LogPlug::info( "Drawing from abscissa " + Ceylan::toString( x ) 
				+ " a vertical line whose color is [R,G,B] = [" 
				+ Ceylan::toString( x ) + ", " + Ceylan::toString( x ) + ", "
				+ Ceylan::toString( x ) + "] " ) ;
				
			screen.drawVerticalLine( x, 100, 300, x, x, x ) ;
		}
		
		
    	LogPlug::info( "Drawing horizontal lines" ) ;	
		
		for ( Ceylan::Uint32 y = 100; y < 300; y++ )
		{
					
			LogPlug::info( "drawing from ordinate " + Ceylan::toString( y ) 
				+ " an horizontal line whose color is [R,G,B] = [" 
				+ Ceylan::toString( y - 50 ) + ", " 
				+ Ceylan::toString( y - 50 ) + ", "
				+ Ceylan::toString( y - 50 ) + "] " ) ;

			screen.drawHorizontalLine( 300, 550, y, y-50, y-50, y-50 ) ;
				
		}
	
    	LogPlug::info( "Drawing random coloured lines" ) ;	

    	LogPlug::info( "Prerequisite : having three random generators" ) ;	
		
		Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 
			0, screenWidth ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 
			0, screenHeight ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
	
		Coordinate x1, y1, x2, y2 ;
		
		ColorElement red ;
		ColorElement green ;
		ColorElement blue ;
		ColorElement alpha ;
	
	
		for ( Ceylan::Uint32 i = 0; i < 10000; i++ )
		{
								
			x1 = abscissaRand.getNewValue() ;
			x2 = abscissaRand.getNewValue() ;
			
			y1 = ordinateRand.getNewValue() ;
			y2 = ordinateRand.getNewValue() ;
			
			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;
								
			screen.drawLine( x1, y1, x2, y2, red, green, blue, alpha ) ;
			
		}
	
		screen.unlock() ;
				
		screen.update() ;	
		
		if ( ! isBatch )
			myOSDL.getEventsModule().waitForAnyKey() ;
		
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;
				
		LogPlug::info( "End of OSDL Lines test" ) ;
		
		
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


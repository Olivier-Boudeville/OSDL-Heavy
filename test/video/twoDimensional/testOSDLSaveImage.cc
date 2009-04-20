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

using namespace Ceylan::Log ;


#include <string>




/**
 * Small usage tests for the saving of surfaces.
 *
 */
 int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;


    try 
	{
			

    	LogPlug::info( "Testing OSDL saving of surfaces." ) ;	
         
		
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
			
				// Ignored for this test.
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
		
		 
		OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseEvents ) ;		
				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length windowWidth  = 640 ;
		Length windowHeight = 480 ;
		
		myVideo.setMode( windowWidth, windowHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;

		
    	LogPlug::info( "Creating a silly image." ) ;	


    	LogPlug::info( "Pre requesite: having three random generators." ) ;	
		
		Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0, 
			windowWidth ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0, 
			windowHeight ) ;
			
		Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
		
				
    	LogPlug::info( "Drawing random pixels with random colors." ) ;	
		
		screen.lock() ;
		
		Coordinate abscissa ;
		Coordinate ordinate ;
		
		ColorElement red ;
		ColorElement green ;
		ColorElement blue ;
		ColorElement alpha ;
		
						
		for ( Ceylan::Uint32 j = 0; j < 50000; j++ )
		{
		
			abscissa = abscissaRand.getNewValue() ;
			ordinate = ordinateRand.getNewValue() ;
			
			red   = colorRand.getNewValue() ;
			green = colorRand.getNewValue() ;
			blue  = colorRand.getNewValue() ;
			alpha = colorRand.getNewValue() ;
								
			putRGBAPixel( screen, abscissa, ordinate, 
				red, green, blue, alpha ) ;
		}

			
		screen.unlock() ;

		screen.update() ;
		
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
		
		
		std::string bmpFile = std::string( argv[0] ) + ".bmp" ;
		std::string pngFile = std::string( argv[0] ) + ".png" ;
		
    	LogPlug::info( "Saving screen surface in " + bmpFile ) ;	
		screen.saveBMP( bmpFile ) ;
		
    	LogPlug::info( "Saving screen surface in " + pngFile ) ;	
		screen.savePNG( pngFile ) ;

					
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;
					
		LogPlug::info( "End of OSDL saving of surfaces." ) ;
	
		
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


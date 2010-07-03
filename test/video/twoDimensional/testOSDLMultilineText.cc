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
using namespace OSDL::Video::TwoDimensional::Text ;
using namespace OSDL::Video::Pixels ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;



#include <string>



/*
 * TrueType font directory is defined relatively to OSDL documentation 
 * tree, from executable build directory:
 *
 */
const std::string trueTypeFontDirFromExec = 
	"../../../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in build tree:
 *
 */
const std::string trueTypeFontDirForBuildPlayTests 
	= "../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in installed tree:
 *
 */
const std::string trueTypeFontDirForInstalledPlayTests 
	= "../OSDL/doc/web/common/fonts" ;


/*
 * Those are Larabie splendid fonts, see read_me.html under
 * trueTypeFontDirFromExec:
 *
 */
const std::string firstTrueTypeFontFile  = "cretino.ttf" ;



/**
 * Test of multiline text rendering.
 *
 */
int main( int argc, char * argv[] ) 
{



	LogHolder myLog( argc, argv ) ;
	
	bool gridWanted = false ;
	bool screenshotWanted = true ;
	
	// Thanks to the 'fortune' program for these wise sentences !
	
	const std::string firstText = 
		"This is quite a long text so that the multiline text rendering "
		"capabilities of OSDL can be tested in place, including text "
		"either justified or not. "
		"Technically, such long text is called \"A fucking long text\". "
		"Grabel's Law: 2 is not equal to 3 -- not even for large values of 2." ;

	const std::string secondText = 
		"As you grow older, you will still do foolish things, "
		"but you will do them with much more enthusiasm. "
		"Cats are intended to teach us that not everything in nature "
		"has a function. This end of text is written so that "
		"it cannot fit into the box at all" ;

	/*
	 * Very long words could be tested as well, but it would lead to partial
	 * renderings (as expected by the API).
	 *
		"youhaveanunusualunderstandingoftheproblemsofhumanrelationships." 
	 *
	 */
	 
	 		
    try 
	{
			

    	LogPlug::info( "Testing OSDL multiline text rendering service" ) ;	

	
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
		
		
		
    	LogPlug::info( "Prerequisite: initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo 
			| CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
	
		if ( gridWanted )
		{
		
			LogPlug::info( "Drawing a grid to check transparency of text." ) ;
			
			if ( ! screen.drawGrid() )
				LogPlug::error( "Grid rendering failed." ) ;
				
		}
		

		/*
		 * Find a valid TrueType font:
		 *
		 */
		TrueTypeFont::TrueTypeFontFileLocator.addPath( 
			trueTypeFontDirFromExec ) ;
			
		TrueTypeFont::TrueTypeFontFileLocator.addPath(
			trueTypeFontDirForBuildPlayTests ) ;
			
		TrueTypeFont::TrueTypeFontFileLocator.addPath(
			trueTypeFontDirForInstalledPlayTests ) ;
			
		LogPlug::info( "TrueType fonts will be found through: " 
			+ TrueTypeFont::TrueTypeFontFileLocator.toString() ) ;
			
		/*
		 * 'GlyphCached' is probably the worst case (hence the most 
		 * interesting to test) since not only a glyph-cache has to used, 
		 * but a temporary word cache has also to be used.
		 *
		 * Other possibility: RenderCache testedCache = Font::WordCached ;
		 *
		 */
		Font::RenderCache testedCache = Font::GlyphCached ;
		
		std::string firstTrueTypeFontPath = TrueTypeFont::FindPathFor(
			firstTrueTypeFontFile ) ;
			
		TrueTypeFont myFont( firstTrueTypeFontPath,	/* font index */ 0, 
			/* convertToDisplay */ true, /* render cache */ testedCache ) ;
		
		myFont.load( /* point size */ 20 ) ;
		
		LogPlug::info( "Successfully loaded following font: " 
			+ myFont.toString() ) ;
		
		Ceylan::Uint16 index ;
		
		Font::RenderQuality quality = /* Font:: */ Font::Blended ;
		
		LogPlug::info( "Rendering texts that can fit into their box." ) ;
		
		LogPlug::info( "Rendering non justified text." ) ;
		myFont.blitLatin1MultiLineText( screen, /* abscissa */ 10, 
			/* ordinate */ 10, /* box width */ 310, /* box height */ 230,
			firstText, index, quality, Pixels::Green, /* justified */ false ) ;
		
		LogPlug::info( "Non justified text rendered till character #" 
			+ Ceylan::toString( index ) + " out of " 
			+ Ceylan::toString( firstText.size() ) ) ;
		
		
		LogPlug::info( "Rendering justified text." ) ;
		myFont.blitLatin1MultiLineText( screen, /* abscissa */ 10, 
			/* ordinate */ 245, /* box width */ 310, /* box height */ 230,
			firstText, index, quality, Pixels::Green, /* justified */ true ) ;
		
		LogPlug::info( "Justified text rendered until character #" 
			+ Ceylan::toString( index ) + " out of " 
			+ Ceylan::toString( firstText.size() ) ) ;
	
	
			
		LogPlug::info( "Rendering texts that cannot fit into their box." ) ;
		
		LogPlug::info( "Rendering non justified text." ) ;
		myFont.blitLatin1MultiLineText( screen, /* abscissa */ 340, 
			/* ordinate */ 10, /* box width */ 290, /* box height */ 150,
			secondText, index, quality,	Pixels::Green, /* justified */ false ) ;
		
		LogPlug::info( "Non justified text rendered till character #" 
			+ Ceylan::toString( index ) + " out of " 
			+ Ceylan::toString( firstText.size() ) ) ;
		
		
		LogPlug::info( "Rendering justified text." ) ;
		myFont.blitLatin1MultiLineText( screen, /* abscissa */ 340, 
			/* ordinate */ 250, /* box width */ 290, /* box height */ 150,
			secondText, index, quality,	Pixels::Green, /* justified */ true ) ;
		
		LogPlug::info( "Justified text rendered until character #" 
			+ Ceylan::toString( index ) + " out of " 
			+ Ceylan::toString( firstText.size() ) ) ;
		
		screen.update() ;
		
		if ( screenshotWanted )
			screen.savePNG( argv[0] + std::string( ".png" ) ) ;
		
		if ( ! isBatch )	
			myOSDL.getEventsModule().waitForAnyKey() ;
		
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;
		
		
		LogPlug::info( "End of OSDL multiline text test." ) ;
		
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


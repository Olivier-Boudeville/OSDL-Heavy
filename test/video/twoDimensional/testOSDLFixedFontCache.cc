#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::TwoDimensional::Text ;
using namespace OSDL::Video::Pixels ;


#include "Ceylan.h"
using namespace Ceylan::Log ;
using namespace Ceylan::System ;


#include <string>


/*
 * Fixed font directory is defined in LOANI as
 * ${alternate_prefix}/OSDL-data/fonts/fixed, usually this pathname relative to the
 * install directory where this test executable should lie is :
 *
 */
const std::string fixedFontDirFromExec = "../../../../OSDL-data/fonts/fixed" ;


/*
 * Fixed font directory is defined in LOANI as
 * ${alternate_prefix}/OSDL-data/fonts/fixed, usually this pathname relative to
 * OSDL/OSDL-${OSDL_VERSION}/src/code where this test executable should be executed by 
 * 'playTests.sh' is :
 *
 */
const std::string fixedFontDirForPlayTests = "../../../OSDL-data/fonts/fixed" ;


const std::string firstTestSentence  = "Ceylan and OSDL rock !" ;
const std::string secondTestSentence = "Yeah man, they both rock" ;




/**
 * Small usage tests for caches of fixed font cache.
 *
 */
int main( int argc, char * argv[] ) 
{


	bool testGlyphCache = true ;
	bool testWordCache  = true ;
	bool testTextCache  = true ;
	
	
	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{
			

    	LogPlug::info( "Testing OSDL fixed font cache : "
			"inspect 'Debug' channel to check cache hits and misses" ) ;	
			
			
			
		LogPlug::info( "At the end of this test, one should see red letters ('a', 'b'), "
			"and two groups of two sentences, first green, then blue." ) ;
			
		
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo 
			| CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 

		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
		
		Pixels::ColorDefinition textColor = Pixels::Red  ;

		/*
		 * Wanting to use at first '6x12.fnt', and to find it from various locations.
		 *
		 */

		FixedFont::FixedFontFileLocator.addPath( fixedFontDirFromExec ) ;
		FixedFont::FixedFontFileLocator.addPath( fixedFontDirForPlayTests ) ;
			
				
		Surface * mySurface ;
		
		bool gridWanted = true ;		
		if ( gridWanted )
		{
			LogPlug::info( "Drawing a grid to check transparency of text." ) ;
			if ( ! screen.drawGrid() )
				LogPlug::error( "Grid rendering failed." ) ;
		}

		
		// Glyph cached section.
		
		
		if ( testGlyphCache )
		{
		
			LogPlug::info( "First : testing cache for glyph rendering." ) ;
					
			FixedFont myGlyphCachedFixedFont( 6, 12, /* renderingStyle */ Font::Normal, 
				/* convertToDisplay */ true, /* render cache */ Font::GlyphCached ) ;
						
			LogPlug::info( "Successfully loaded following font in initial state : " 
				+ myGlyphCachedFixedFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myGlyphCachedFixedFont.renderLatin1Glyph( 'a', Font::Solid, textColor ) ;
			mySurface->blitTo( screen, 20, 50 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after first render of 'a' : " 
				+ myGlyphCachedFixedFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myGlyphCachedFixedFont.renderLatin1Glyph( 'a', Font::Solid, textColor ) ;
			mySurface->blitTo( screen, 40, 50 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after second render of 'a' : " 
				+ myGlyphCachedFixedFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myGlyphCachedFixedFont.renderLatin1Glyph( 'a', Font::Solid, textColor ) ;
			mySurface->blitTo( screen, 60, 50 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after third render of 'a' : " 
				+ myGlyphCachedFixedFont.toString( Ceylan::high ) ) ;
		
		
			myGlyphCachedFixedFont.blitLatin1Glyph( screen, 30, 50, 'b', Font::Solid, textColor ) ; 
			LogPlug::info( "Font after first blit of 'b' : " 
				+ myGlyphCachedFixedFont.toString( Ceylan::high ) ) ;
		
			myGlyphCachedFixedFont.blitLatin1Glyph( screen, 50, 50, 'b', Font::Solid, textColor ) ; 
			LogPlug::info( "Font after second blit of 'b' : " 
				+ myGlyphCachedFixedFont.toString( Ceylan::high ) ) ;
		
			myGlyphCachedFixedFont.blitLatin1Glyph( screen, 70, 50, 'b', Font::Solid, textColor ) ; 
			LogPlug::info( "Font after third blit of 'b' : " 
				+ myGlyphCachedFixedFont.toString( Ceylan::high ) ) ;
		
		}
		

		
		// Word cached section.
		
		if ( testWordCache )
		{
		
		
			textColor = Pixels::Green ;
			
			LogPlug::info( "Second : testing cache for word rendering, with first test sentence : '"
				+ firstTestSentence 
				+ "', with second test sentence '" + secondTestSentence + "'." ) ;
					
			FixedFont myWordCachedFixedFont( 6, 12, /* renderingStyle */ Font::Normal, 
				/* convertToDisplay */ true, /* render cache */ Font::WordCached ) ;
						
			LogPlug::info( "Successfully loaded following font in initial state : " 
				+ myWordCachedFixedFont.toString( Ceylan::high ) ) ;
			

			mySurface = & myWordCachedFixedFont.renderLatin1Text( firstTestSentence, 
				Font::Solid, textColor ) ;
				
			mySurface->blitTo( screen, 20, 100 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after first render of first test sentence : " 
				+ myWordCachedFixedFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myWordCachedFixedFont.renderLatin1Text( firstTestSentence,
				 Font::Solid, textColor ) ;
			mySurface->blitTo( screen, 220, 100 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after second render of first test sentence : " 
				+ myWordCachedFixedFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myWordCachedFixedFont.renderLatin1Text( firstTestSentence, 
				 Font::Solid, textColor ) ;
				 
			mySurface->blitTo( screen, 420, 100 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after third render of first test sentence : " 
				+ myWordCachedFixedFont.toString( Ceylan::high ) ) ;


			myWordCachedFixedFont.blitLatin1Text( screen, 20, 150, 
				secondTestSentence, Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myWordCachedFixedFont.toString( Ceylan::high ) ) ;
		
			myWordCachedFixedFont.blitLatin1Text( screen, 220, 150, 
				secondTestSentence, Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myWordCachedFixedFont.toString( Ceylan::high ) ) ;
			
			myWordCachedFixedFont.blitLatin1Text( screen, 420, 150, 
				secondTestSentence, Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myWordCachedFixedFont.toString( Ceylan::high ) ) ;

		}



		// Text cached section.
		
	
		if ( testTextCache )
		{
		
			textColor = Pixels::Blue ;
							
			LogPlug::info( "Third : testing cache for text rendering, with first test sentence : '"
				+ firstTestSentence 
				+ "', with second test sentence '" + secondTestSentence + "'." ) ;


			FixedFont myTextCachedFixedFont( 6, 12, /* renderingStyle */ Font::Normal, 
				/* convertToDisplay */ true, /* render cache */ Font::TextCached ) ;
						
			LogPlug::info( "Successfully loaded following font in initial state : " 
				+ myTextCachedFixedFont.toString( Ceylan::high ) ) ;
	

			mySurface = & myTextCachedFixedFont.renderLatin1Text( firstTestSentence, 
				 Font::Solid, textColor ) ;
				 
			mySurface->blitTo( screen, 20, 200 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after first render of first test sentence : " 
				+ myTextCachedFixedFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myTextCachedFixedFont.renderLatin1Text( firstTestSentence, 
				Font::Solid, textColor ) ;
				
			mySurface->blitTo( screen, 220, 200 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after second render of first test sentence : " 
				+ myTextCachedFixedFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myTextCachedFixedFont.renderLatin1Text( firstTestSentence, 
				 Font::Solid, textColor ) ;
				 
			mySurface->blitTo( screen, 420, 200 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after third render of first test sentence being " 
				+ myTextCachedFixedFont.toString( Ceylan::high ) ) ;


			myTextCachedFixedFont.blitLatin1Text( screen, 20, 250, secondTestSentence, 
				 Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myTextCachedFixedFont.toString( Ceylan::high ) ) ;
		
			myTextCachedFixedFont.blitLatin1Text( screen, 220, 250, secondTestSentence,
				 Font::Solid, textColor ) ;		
				 
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myTextCachedFixedFont.toString( Ceylan::high ) ) ;
			
			myTextCachedFixedFont.blitLatin1Text( screen, 420, 250, secondTestSentence,
				 Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myTextCachedFixedFont.toString( Ceylan::high ) ) ;


		}
				
				
		screen.update() ;	
		myOSDL.getEventsModule().waitForAnyKey() ;
			
		LogPlug::info( "End of OSDL fixed font cache test" ) ;
	
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


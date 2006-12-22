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
 * TrueType font directory is defined relatively to OSDL documentation tree, usually this pathname
 * relative to the install directory where this test executable should lie is :
 * (to be reached from executable directory)
 *
 */
const std::string trueTypeFontDirFromExec = "../../../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree, usually this pathname
 * relative to the install directory where this test executable should lie is :
 * (to be reached from OSDL/OSDL-${OSDL_VERSION}/src/code)
 *
 */
const std::string trueTypeFontDirForPlayTests = "../src/doc/web/common/fonts" ;



// Those are Larabie splendid fonts, see read_me.html under trueTypeFontDirFromExec :
const std::string firstTrueTypeFontFile  = "cretino.ttf" ;
const std::string secondTrueTypeFontFile = "iomanoid.ttf" ;
const std::string thirdTrueTypeFontFile  = "stilltim.ttf" ;

const std::string firstTestSentence  = "Ceylan and OSDL rock !" ;
const std::string secondTestSentence = "Yeah man, they both rock" ;




/**
 * Small usage tests for caches of TrueType font cache.
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
			

		LogPlug::info( "Testing OSDL TrueType font cache : "
			"inspect 'Debug' channel to check cache hits and misses" ) ;	


			
		LogPlug::info( "At the end of this test, one should see red letters ('a', 'b'), "
			"and two groups of two sentences, first green, then blue." ) ;


		LogPlug::info( "Note that having compiled OSDL with the OSDL_DEBUG_FONT flag set "
			"allows for far more debug informations." ) ;

		
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

		TrueTypeFont::TrueTypeFontFileLocator.addPath( trueTypeFontDirFromExec ) ;
		TrueTypeFont::TrueTypeFontFileLocator.addPath( trueTypeFontDirForPlayTests ) ;
			
				
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
					
					
			TrueTypeFont myGlyphCachedTrueTypeFont( firstTrueTypeFontFile, 
				/* point size */ 15, /* font index */ 0, 
				/* convertToDisplay */ true, /* render cache */ Font::GlyphCached ) ;
						
			LogPlug::info( "Successfully loaded following font in initial state : " 
				+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myGlyphCachedTrueTypeFont.renderLatin1Glyph( 'a', Font::Solid, 
				textColor ) ;
			mySurface->blitTo( screen, 20, 50 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after first render of 'a' : " 
				+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myGlyphCachedTrueTypeFont.renderLatin1Glyph( 'a', Font::Solid, 
				textColor ) ;
			mySurface->blitTo( screen, 40, 50 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after second render of 'a' : " 
				+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myGlyphCachedTrueTypeFont.renderLatin1Glyph( 'a', Font::Solid, 
				textColor ) ;
			mySurface->blitTo( screen, 60, 50 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after third render of 'a' : " 
				+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
		
			myGlyphCachedTrueTypeFont.blitLatin1Glyph( screen, 30, 50, 'b', Font::Solid, 
				textColor ) ; 
			LogPlug::info( "Font after first blit  of 'b' : " 
				+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			myGlyphCachedTrueTypeFont.blitLatin1Glyph( screen, 50, 50, 'b', Font::Solid, 
				textColor ) ; 
			LogPlug::info( "Font after second blit  of 'b' : " 
				+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			myGlyphCachedTrueTypeFont.blitLatin1Glyph( screen, 70, 50, 'b', Font::Solid, 
				textColor ) ; 
			LogPlug::info( "Font after third blit  of 'b' : " 
				+ myGlyphCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
		}
		

		
		// Word cached section.
		
		if ( testWordCache )
		{
		
			LogPlug::info( "Second : testing cache for word rendering, with first test sentence : '"
				+ firstTestSentence 
				+ "', with second test sentence '" + secondTestSentence + "'." ) ;
				
			textColor = Pixels::Green ;
					
			TrueTypeFont myWordCachedTrueTypeFont( secondTrueTypeFontFile, 
				/* point size */ 15, /* font index */ 0, 
				/* convertToDisplay */ true, /* render cache */ Font::WordCached ) ;
						
			LogPlug::info( "Successfully loaded following font in initial state : " 
				+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;
			

			mySurface = & myWordCachedTrueTypeFont.renderLatin1Text( firstTestSentence, 
				Font::Solid, textColor ) ;
				
			mySurface->blitTo( screen, 20, 100 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after first render of first test sentence : " 
				+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myWordCachedTrueTypeFont.renderLatin1Text( firstTestSentence,
				 Font::Solid, textColor ) ;
			mySurface->blitTo( screen, 220, 100 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after second render of first test sentence : " 
				+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myWordCachedTrueTypeFont.renderLatin1Text( firstTestSentence, 
				 Font::Solid, textColor ) ;
				 
			mySurface->blitTo( screen, 420, 100 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after third render of first test sentence : " 
				+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;


			myWordCachedTrueTypeFont.blitLatin1Text( screen, 20, 150, 
				secondTestSentence, Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			myWordCachedTrueTypeFont.blitLatin1Text( screen, 220, 150, 
				secondTestSentence, Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;
			
			myWordCachedTrueTypeFont.blitLatin1Text( screen, 420, 150, 
				secondTestSentence, Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myWordCachedTrueTypeFont.toString( Ceylan::high ) ) ;

		}



		// Text cached section.
		
	
		if ( testTextCache )
		{
		
							
			LogPlug::info( "Third : testing cache for text rendering, with first test sentence : '"
				+ firstTestSentence 
				+ "', with second test sentence '" + secondTestSentence + "'." ) ;

			textColor = Pixels::Blue ;

			TrueTypeFont myTextCachedTrueTypeFont( thirdTrueTypeFontFile, 
				/* point size */ 28, /* font index */ 0, 
				/* convertToDisplay */ true, /* render cache */ Font::TextCached ) ;

						
			LogPlug::info( "Successfully loaded following font in initial state : " 
				+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;
	

			mySurface = & myTextCachedTrueTypeFont.renderLatin1Text( firstTestSentence, 
				 Font::Solid, textColor ) ;
				 
			mySurface->blitTo( screen, 20, 200 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after first render of first test sentence : " 
				+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myTextCachedTrueTypeFont.renderLatin1Text( firstTestSentence, 
				Font::Solid, textColor ) ;
				
			mySurface->blitTo( screen, 220, 200 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after second render of first test sentence : " 
				+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			mySurface = & myTextCachedTrueTypeFont.renderLatin1Text( firstTestSentence, 
				 Font::Solid, textColor ) ;
				 
			mySurface->blitTo( screen, 420, 200 ) ;
			delete mySurface ;
		
			LogPlug::info( "Font after third render of first test sentence being " 
				+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;


			myTextCachedTrueTypeFont.blitLatin1Text( screen, 20, 250, secondTestSentence, 
				 Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;
		
			myTextCachedTrueTypeFont.blitLatin1Text( screen, 220, 250, secondTestSentence,
				 Font::Solid, textColor ) ;		
				 
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;
			
			myTextCachedTrueTypeFont.blitLatin1Text( screen, 420, 250, secondTestSentence,
				 Font::Solid, textColor ) ;		
				
			LogPlug::info( "Font after first blit of second test sentence : " 
				+ myTextCachedTrueTypeFont.toString( Ceylan::high ) ) ;


		}
				
				
		screen.update() ;	
		myOSDL.getEventsModule().waitForAnyKey() ;
			
		LogPlug::info( "End of OSDL TrueType font cache test" ) ;
	
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


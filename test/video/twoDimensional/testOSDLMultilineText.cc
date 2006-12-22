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



/**
 * Test of multiline text rendering.
 *
 */
int main( int argc, char * argv[] ) 
{

	bool gridWanted = false ;
	bool screenshotWanted = true ;

	LogHolder myLog( argc, argv ) ;
	
	
	// Thanks to the 'fortune' program for these wise sentences !
	
	const std::string firstText = "This is quite a long text so that the multiline text rendering "
		"capabilities of OSDL can be tested in place, including text either justified or not."
		" Technically, such long text is called \"A fucking long text\". Grabel's Law: "
		"2 is not equal to 3 -- not even for large values of 2." ;

	const std::string secondText = "As you grow older, you will still do foolish things, "
		"but you will do them with much more enthusiasm. Cats are intended to teach us that not "
		"everything in nature has a function. This end of text is written so that "
		"it cannot fit into the box at all" ;
		
    try 
	{
			

    	LogPlug::info( "Testing OSDL multiline text rendering service" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo 
			| CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight, VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
	
		if ( gridWanted )
		{
			LogPlug::info( "Drawing a grid to check transparency of text." ) ;
			if ( ! screen.drawGrid() )
				LogPlug::error( "Grid rendering failed." ) ;
		}

		/*
		 * Find a valid TrueType font :
		 *
		 */
		TrueTypeFont::TrueTypeFontFileLocator.addPath( trueTypeFontDirFromExec ) ;
		TrueTypeFont::TrueTypeFontFileLocator.addPath( trueTypeFontDirForPlayTests ) ;
			
		LogPlug::info( "TrueType fonts will be found through : " 
			+ TrueTypeFont::TrueTypeFontFileLocator.toString() ) ;
			
		/*
		 * 'GlyphCached' is probably the worst case (hence the most interesting to test) since
		 * not only a glyph-cache has to used, but a temporary word cache has also to be used.
		 *
		 * Other possibility : RenderCache testedCache = Font::WordCached ;
		 *
		 */
		Font::RenderCache testedCache = Font::GlyphCached ;
				
		TrueTypeFont myFont( firstTrueTypeFontFile, /* point size */ 20, 
			/* font index */ 0, /* convertToDisplay */ true, 
			/* render cache */ testedCache ) ;
					
		LogPlug::info( "Successfully loaded following font : " + myFont.toString() ) ;
		
		Ceylan::Uint16 index ;
		
		Font::RenderQuality quality = /* Font:: */ Font::Blended ;
		
		LogPlug::info( "Rendering texts that can fit into their box." ) ;
		
		LogPlug::info( "Rendering non justified text." ) ;
		myFont.blitLatin1MultiLineText( screen, /* abscissa */ 10, /* ordinate */ 10, 
			/* box width */ 310, /* box height */ 230, firstText, index, quality,
			Pixels::Green, /* justified */ false ) ;
		
		LogPlug::info( "Non justified text rendered till character #" 
			+ Ceylan::toString( index ) + " out of " + Ceylan::toString( firstText.size() ) ) ;
		
		
		LogPlug::info( "Rendering justified text." ) ;
		myFont.blitLatin1MultiLineText( screen, /* abscissa */ 10, /* ordinate */ 245, 
			/* box width */ 310, /* box height */ 230, firstText, index, quality,
			Pixels::Green, /* justified */ true ) ;
		
		LogPlug::info( "Justified text rendered until character #" 
			+ Ceylan::toString( index ) + " out of " + Ceylan::toString( firstText.size() ) ) ;
	
	
			
		LogPlug::info( "Rendering texts that cannot fit into their box." ) ;
		
		LogPlug::info( "Rendering non justified text." ) ;
		myFont.blitLatin1MultiLineText( screen, /* abscissa */ 340, /* ordinate */ 10, 
			/* box width */ 290, /* box height */ 150, secondText, index, quality,
			Pixels::Green, /* justified */ false ) ;
		
		LogPlug::info( "Non justified text rendered till character #" 
			+ Ceylan::toString( index ) + " out of " + Ceylan::toString( firstText.size() ) ) ;
		
		
		LogPlug::info( "Rendering justified text." ) ;
		myFont.blitLatin1MultiLineText( screen, /* abscissa */ 340, /* ordinate */ 250, 
			/* box width */ 290, /* box height */ 150, secondText, index, quality,
			Pixels::Green, /* justified */ true ) ;
		
		LogPlug::info( "Justified text rendered until character #" 
			+ Ceylan::toString( index ) + " out of " + Ceylan::toString( firstText.size() ) ) ;
		
		screen.update() ;
		
		if ( screenshotWanted )
			screen.savePNG( argv[0] + std::string( ".png" ) ) ;
			
		myOSDL.getEventsModule().waitForAnyKey() ;
		
		
		LogPlug::info( "End of OSDL multiline text test" ) ;
		
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


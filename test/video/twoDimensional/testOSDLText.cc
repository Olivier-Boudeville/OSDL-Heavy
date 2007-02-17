#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::Pixels ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;


#include <string>
using std::string ;


const std::string trueTypeFirstFontName  = "stilltim.ttf" ;
const std::string trueTypeSecondFontName = "iomanoid.ttf" ;
const std::string trueTypeThirdFontName  = "minynbi.ttf" ;
const std::string trueTypeFourthFontName = "cretino.ttf" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * usually this pathname relative to the install directory where this test
 * executable should lie is :
 * (to be reached from executable directory)
 *
 */
const std::string trueTypeFontDirFromExec = 
	"../../../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * usually this pathname relative to the install directory where this test
 * executable should lie is :
 * (to be reached from OSDL/OSDL-${OSDL_VERSION}/src/code)
 *
 */
const std::string trueTypeFontDirForPlayTests = "../src/doc/web/common/fonts" ;



/*
 * Wanting to use at first '9x18B.fnt'.
 * Fixed font directory is defined in LOANI as
 * ${alternate_prefix}/OSDL-data/fonts/fixed, usually this pathname relative
 * to the install directory where this test executable should lie is :
 * (to be reached from executable directory)
 *
 */
const std::string fixedFontDirFromExec = "../../../../OSDL-data/fonts/fixed" ;


/*
 * Wanting to use at first '9x18B.fnt'.
 * Fixed font directory is defined in LOANI as
 * ${alternate_prefix}/OSDL-data/fonts/fixed, usually this pathname relative 
 * to the install directory where this test executable should lie is :
 * (to be reached from executable directory)
 *
 */
const std::string fixedFontDirForPlayTests = "../../../OSDL-data/fonts/fixed" ;



/**
 * Small global tests for all graphical text output : useful to check at a
 * glance that everything is still working.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{
			

    	LogPlug::info( "Testing all OSDL primitives for text output" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         
		 
		CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
    	LogPlug::info( "Font path is managed by " 
			+ Text::Font::FontFileLocator.toString() ) ;
					
					
		screen.lock() ;
		
		bool gridWanted = true ;		
		if ( gridWanted )
		{
			LogPlug::info( "Drawing a grid to check transparency of text." ) ;
			if ( ! screen.drawGrid() )
				LogPlug::error( "Grid rendering failed." ) ;
		}


		LogPlug::info( "Testing basic text output." ) ;
					
		if ( ! Text::printBasic( "This is a test for 'printBasic' primitive, "
			"which uses the built-in font.", screen, 10, 40, Pixels::Tomato ) )
				throw Ceylan::TestException( 
					"Test for 'printBasic' primitive failed" ) ;
					
					
					
		LogPlug::info( "Testing fixed font text output." ) ;
					

		Text::FixedFont::FixedFontFileLocator.addPath( fixedFontDirFromExec ) ;
		Text::FixedFont::FixedFontFileLocator.addPath( 
			fixedFontDirForPlayTests ) ;
				
		Text::FixedFont myFixedFont( 9, 18, 
			/* renderingStyle */ Text::Font::Bold, /* convertToDisplay */ true,
			/* render cache */ Text::Font::None ) ;
					
		LogPlug::info( "Successfully loaded following font : " 
			+ myFixedFont.toString() ) ;

		myFixedFont.blitLatin1Text( screen, 10, 50, 
			"This is a test for 'FixedFont' rendering,",
			Text::Font::Solid, Pixels::Chartreuse ) ;
			
		myFixedFont.blitLatin1Text( screen, 10, 70, 
			"which can use various fonts and attributes.",
			Text::Font::Solid, Pixels::Chartreuse ) ;


		LogPlug::info( "Testing TrueType font text output." ) ;
		
		const string firstTestSentence = 
			"This is a test for OSDL's TrueType Font rendering. It is ok ?"	;		
		const string secondTestSentence = "I hope so, coz it was quite "
			"a nightmare to have it all workin'"	;		

		Text::Font::RenderQuality quality = 
			/* Text::Font::Solid */ Text::Font::Blended ;
			
		Text::Font::RenderCache cache = 
			/* Text::Font::None */ Text::Font::WordCached ;
		
		Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
			trueTypeFontDirFromExec ) ;
			
		Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
			trueTypeFontDirForPlayTests ) ;
		
		Text::TrueTypeFont myFirstTrueTypeFont( trueTypeFirstFontName,
				/* point size */ 30, 
				/* renderingStyle */ Text::Font::Normal, 
				/* convertToDisplay */ true, 
				/* render cache */ cache ) ;

		LogPlug::info( "Successfully loaded following font : " 
			+ myFirstTrueTypeFont.toString() ) ;

		myFirstTrueTypeFont.blitLatin1Text( screen, 10, 100, 
			firstTestSentence, quality, Pixels::Orchid ) ;
			
		myFirstTrueTypeFont.blitLatin1Text( screen, 10, 140, 
			secondTestSentence,	quality, Pixels::Orchid ) ;
			
			

		Text::TrueTypeFont mySecondTrueTypeFont( trueTypeSecondFontName,
				/* point size */ 30, 
				/* renderingStyle */ Text::Font::Normal, 
				/* convertToDisplay */ true, 
				/* render cache */ cache ) ;

		LogPlug::info( "Successfully loaded following font : " 
			+ mySecondTrueTypeFont.toString() ) ;

		mySecondTrueTypeFont.blitLatin1Text( screen, 10, 200, 
			firstTestSentence, quality, Pixels::HotPink ) ;
			
		mySecondTrueTypeFont.blitLatin1Text( screen, 10, 240,
			secondTestSentence, quality, Pixels::HotPink ) ;



		Text::TrueTypeFont myThirdTrueTypeFont( trueTypeThirdFontName,
				/* point size */ 30, 
				/* renderingStyle */ Text::Font::Normal, 
				/* convertToDisplay */ true, 
				/* render cache */ cache ) ;

		LogPlug::info( "Successfully loaded following font : " 
			+ myThirdTrueTypeFont.toString() ) ;

		myThirdTrueTypeFont.blitLatin1Text( screen, 10, 300, firstTestSentence,
			quality, Pixels::PeachPuff ) ;
			
		myThirdTrueTypeFont.blitLatin1Text( screen, 10, 340, secondTestSentence,
			quality, Pixels::PeachPuff ) ;


		Text::TrueTypeFont myFourthTrueTypeFont( trueTypeFourthFontName,
				/* point size */ 30, 
				/* renderingStyle */ Text::Font::Normal, 
				/* convertToDisplay */ true, 
				/* render cache */ cache ) ;

		LogPlug::info( "Successfully loaded following font : " 
			+ myFourthTrueTypeFont.toString() ) ;

		myFourthTrueTypeFont.blitLatin1Text( screen, 10, 400, 
			firstTestSentence, quality, Pixels::LawnGreen ) ;
			
		myFourthTrueTypeFont.blitLatin1Text( screen, 10, 440,
			secondTestSentence,	quality, Pixels::LawnGreen ) ;


		screen.unlock() ;
				
		screen.update() ;	
		screen.savePNG( "testOSDLText-allfonts.png" ) ;
		myOSDL.getEventsModule().waitForAnyKey() ;
					
								
		LogPlug::info( "End of OSDL Text test" ) ;
		
		
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


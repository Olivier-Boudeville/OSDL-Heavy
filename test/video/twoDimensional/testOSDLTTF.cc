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
const std::string secondTrueTypeFontFile = "iomanoid.ttf" ;
const std::string thirdTrueTypeFontFile  = "stilltim.ttf" ;

/*
const std::string fourthTrueTypeFontFile = "earwigfa.ttf" ;
const std::string fifthTrueTypeFontFile  = "neurochr.ttf" ;
const std::string sixthTrueTypeFontFile  = "stilltim.ttf" ;
*/


void displayFont( Surface & screen, const std::string & fontFilename, 
	bool convertToDisplay, Font::RenderCache cache, 
		const std::string & executableName, bool screenshotWanted )
{

	screen.clear() ;
	
	Coordinate abscissa ;
	
	Ceylan::Uint32 scaleFactor = 75 ;
	
	
	for ( Ceylan::Uint32 i = 0; i < 8; i++ )
	{
	
		TrueTypeFont testedFont( fontFilename, 
			/* point size */ 15 + 5 * i, 
			/* font index */ 0, /* convertToDisplay */ convertToDisplay, 
			/* render cache */ cache ) ;
		
		
		LogPlug::debug( testedFont.describeGlyphFor( 'd' ) ) ;
		LogPlug::debug( testedFont.describeGlyphFor( 'i' ) ) ;
			
		abscissa = scaleFactor * i ;
		
		testedFont.blitLatin1Text( screen, abscissa, 0  , "did",
			/* quality */ Font::Solid, Pixels::Green ) ;
			
		testedFont.blitLatin1Text( screen, abscissa, 50  , 
			/* "averylongwordmydear" */ "Glive",
			/* quality */ Font::Solid, Pixels::Green ) ;
			
		testedFont.blitLatin1Text( screen, abscissa, 100, "tested",
			/* quality */ Font::Shaded, Pixels::Green ) ;
		
		testedFont.blitLatin1Text( screen, abscissa, 150, "called",
			/* quality */ Font::Blended, Pixels::Green ) ;
			
		testedFont.blitLatin1Text( screen, abscissa, 200, "equal",
			/* quality */ Font::Solid, Pixels::Green ) ;
			
		testedFont.blitLatin1Text( screen, abscissa, 250, "for",
			/* quality */ Font::Shaded, Pixels::Green ) ;
			
		testedFont.blitLatin1Text( screen, abscissa, 300, "foul",
			/* quality */ Font::Blended, Pixels::Green ) ;
			
		testedFont.blitLatin1Text( screen, abscissa, 350, "ghost",
			/* quality */ Font::Solid, Pixels::Green ) ;
			
		testedFont.blitLatin1Text( screen, abscissa, 400, "howl",
			/* quality */ Font::Shaded, Pixels::Green ) ;
			
		testedFont.blitLatin1Text( screen, abscissa, 450, "ink",	
			/* quality */ Font::Blended, Pixels::Green ) ;
	
	}		

	screen.update() ;	

	if ( screenshotWanted )	
		screen.savePNG( executableName + "-edges.png" ) ;

}

 

/**
 * Usage tests for Truetype font text output.
 *
 */
int main( int argc, char * argv[] ) 
{


	bool randomTestWanted  = true ;
	
	// Should be the only situation where blit blocks appear (if true):
	bool randomBackgroundColor = true ;
	
	bool qualityTestWanted = true ;
	
	bool screenshotWanted = true ;
	
	bool edgeTestWanted = true ;
		
	bool gridWanted = true ;
	
	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{
			

		LogPlug::info( "Testing OSDL TrueType font service" ) ;	
		
		LogPlug::info( 
			"Note that having compiled OSDL with the OSDL_DEBUG_FONT flag set "
			"allows for far more debug informations." ) ;

				
    	LogPlug::info( "Pre requesite: initializing the display" ) ;	
	      
		     

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
		
		 
		CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo 
			| CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;
				
    	LogPlug::info( "Font path is managed by " 
			+ Text::Font::FontFileLocator.toString() ) ;


		bool correct = false ;
		
		try
		{
			TrueTypeFont nonExistingFont( "NotExisting.ttf", 
				/* point size */ 20 ) ;
		}
		catch( const TextException & e )
		{
			LogPlug::info( 
				"Asking for a non-existing font failed as expected: " 
				+ e.toString() ) ;
			correct = true ;
		}
		
		if ( ! correct )
			throw Ceylan::TestException( 
				"Requesting a non-existing font did not fail as expected." ) ;
				
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
		
		
		if ( randomTestWanted )
		{			
			
	    	LogPlug::info( 
				"Writing at random places text with random color." ) ;	
		
	    	LogPlug::info( "Prerequesite: having four random generators" ) ;	
		
			Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0,
				screenWidth ) ;
				
			Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0,
				screenHeight ) ;
				
			Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
			Ceylan::Maths::Random::WhiteNoiseGenerator charRand( 0, 256 ) ;
	
			Coordinate x, y ;
					
			TrueTypeFont existingFont( firstTrueTypeFontFile, 
				/* point size */ 50, /* font index */ 0, 
				/* convertToDisplay */ true, /* render cache */ Font::None ) ;
						
			LogPlug::info( "Successfully loaded following font: " 
				+ existingFont.toString() ) ;
		
		
			ColorElement red ;
			ColorElement green ;
			ColorElement blue ;
		
			ColorDefinition textColor ;
		
			Surface * glyphSurface ;		
		
		
			if ( gridWanted )
				if ( ! screen.drawGrid() )
					LogPlug::error( "Grid rendering failed." ) ;
		
		
			Ceylan::Latin1Char toRender ;
			Font::RenderQuality quality ;
			
			for ( Ceylan::Uint32 i = 0; i < 400; i++ )
			{
										
				x = abscissaRand.getNewValue() ;			
				y = ordinateRand.getNewValue() ;
			
				red   = colorRand.getNewValue() ;
				green = colorRand.getNewValue() ;	
				blue  = colorRand.getNewValue() ;
				
				toRender = static_cast<Ceylan::Latin1Char>(
					charRand.getNewValue() ) ;

				textColor = convertRGBAToColorDefinition( 
					red, green, blue, Pixels::AlphaOpaque ) ;
			
				switch( i % 3 )
				{
				
					case 0:
						quality = Font::Solid ;
						break ;
						
					case 1:
						quality = Font::Shaded ;
						if ( randomBackgroundColor )
							existingFont.setBackgroundColor( 
								convertRGBAToColorDefinition(
									colorRand.getNewValue(),
									colorRand.getNewValue(),
									colorRand.getNewValue(),
									Pixels::AlphaOpaque ) ) ;
						break ;
						
					case 2:
						quality = Font::Blended ;
						break ;
						
					default:
						throw Ceylan::TestException( 
							"Unexpected random quality drawn." ) ;
						break ;
						
				}
				
				// Avoid too much logs:
				if ( i % 10 == 0 )
					LogPlug::debug( "Requesting rendering for character '" 
						+ Ceylan::toString( toRender )
						+ "' at (" + Ceylan::toString( x ) + ";" 
						+ Ceylan::toString( y ) + ") with color " 
						+ Pixels::toString( textColor ) + "." ) ;
			
				// Try renderLatin1GlyphAlpha instead to see it is not correct.

				/*
				 * Test both: generates an intermediate texture and blit it, 
				 * or blit directly:
				 *
				 */
				if ( i % 2 == 0 )
				{
					existingFont.blitLatin1Glyph( screen, x, y, 
						toRender, quality, textColor ) ;
				}	
				else
				{	
					glyphSurface = & existingFont.renderLatin1Glyph( toRender, 
						quality, textColor ) ;
				
					glyphSurface->blitTo( screen, x, y ) ;
					delete glyphSurface ;
				}
 
 			}
		
		
			Surface * textSurface = & existingFont.renderLatin1Text( 
				"You want to read a game-book",	
				Font::Shaded, Pixels::DarkOrange ) ;
		
			textSurface->blitTo( screen, 100, 100 ) ;

			if ( screenshotWanted )	
				textSurface->savePNG( std::string( argv[0] ) + "-text.png" ) ;
			
		
			delete textSurface ;
		
				
			screen.update() ;
				
			if ( screenshotWanted )	
				screen.savePNG( std::string( argv[0] ) + "-random.png" ) ;
				
			if ( ! isBatch ) 
				myOSDL.getEventsModule().waitForAnyKey() ;
			
		}
		
		
		if ( qualityTestWanted )
		{
							
			screen.clear() ;

			if ( gridWanted )
				if ( ! screen.drawGrid() )
					LogPlug::error( "Grid rendering failed." ) ;

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
					
			TrueTypeFont existingFont( firstTrueTypeFontFile, 50, 
				/* font index */ 0, /* convertToDisplay */ true, 
				/* render cache */ Font::None ) ;
						
			LogPlug::info( "Successfully loaded following font: " 
				+ existingFont.toString() ) ;

		
			existingFont.blitLatin1Text( screen, 5, 5, 
				"This is a text rendered with", Font::Solid, 
					Pixels::Moccasin );
				
			existingFont.blitLatin1Text( screen, 50, 55, 
				"the 'Solid' quality.", Font::Solid, 
					Pixels::Moccasin );
			
			
			existingFont.blitLatin1Text( screen, 5, 155, 
				"This is a text rendered with", Font::Shaded, 
					Pixels::Moccasin );

			existingFont.blitLatin1Text( screen, 50, 205, 
				"the 'Shaded' quality.", Font::Shaded, 
					Pixels::Moccasin );
			
			
			existingFont.blitLatin1Text( screen, 5, 305, 
				"This is a text rendered with", Font::Blended, 
					Pixels::Moccasin );

			existingFont.blitLatin1Text( screen, 50, 355, 
				"the 'Blended' quality.", Font::Blended, 
					Pixels::Moccasin );
			
			screen.update() ;	

			if ( screenshotWanted )	
				screen.savePNG( std::string( argv[0] ) + "-qualities.png" ) ;
			
			if ( ! isBatch ) 
				myOSDL.getEventsModule().waitForAnyKey() ;	
	
		}	
		
		
		if ( edgeTestWanted )
		{

			screen.clear() ;

			if ( gridWanted )
				if ( ! screen.drawGrid() )
					LogPlug::error( "Grid rendering failed." ) ;


			bool manyKeysToHit = false ;
			
			
			displayFont( screen, firstTrueTypeFontFile, 
				/* convertToDisplay */ true, 
				/* render cache */ Font::GlyphCached, 
				std::string( argv[0] ), screenshotWanted ) ;

			if ( ! isBatch )
				myOSDL.getEventsModule().waitForAnyKey() ;

			
			if ( manyKeysToHit )
			{
			
				displayFont( screen, firstTrueTypeFontFile, 
					/* convertToDisplay */ true, 
					/* render cache */ Font::WordCached,
					std::string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch )
					myOSDL.getEventsModule().waitForAnyKey() ;

				displayFont( screen, firstTrueTypeFontFile, 
					/* convertToDisplay */ true, 
					/* render cache */ Font::TextCached,
					std::string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch )
					myOSDL.getEventsModule().waitForAnyKey() ;

			}

		
			displayFont( screen, secondTrueTypeFontFile, 
				/* convertToDisplay */ false, 
				/* render cache */ Font::GlyphCached,
				std::string( argv[0] ), screenshotWanted ) ;

			if ( ! isBatch )
				myOSDL.getEventsModule().waitForAnyKey() ;
			
			
			if ( manyKeysToHit )
			{
			
				displayFont( screen, secondTrueTypeFontFile, 
					/* convertToDisplay */ false, 
					/* render cache */ Font::WordCached,
					std::string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch )
					myOSDL.getEventsModule().waitForAnyKey() ;

				displayFont( screen, secondTrueTypeFontFile, 
					/* convertToDisplay */ false, 
					/* render cache */ Font::TextCached,
					std::string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch )
					myOSDL.getEventsModule().waitForAnyKey() ;
		
			}
		
		
			displayFont( screen, thirdTrueTypeFontFile, 
				/* convertToDisplay */ true, 
				/* render cache */ Font::GlyphCached,
				std::string( argv[0] ), screenshotWanted ) ;

			if ( ! isBatch )
				myOSDL.getEventsModule().waitForAnyKey() ;
				
		
			if ( manyKeysToHit )
			{
			
				displayFont( screen, thirdTrueTypeFontFile, 
					/* convertToDisplay */ true, 
					/* render cache */ Font::WordCached,
					std::string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch ) 
					myOSDL.getEventsModule().waitForAnyKey() ;
				
		
				displayFont( screen, thirdTrueTypeFontFile, 
					/* convertToDisplay */ true, 
					/* render cache */ Font::TextCached,
					std::string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch ) 
					myOSDL.getEventsModule().waitForAnyKey() ;
				
			}

		}		

		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;
		
		
		LogPlug::info( "End of OSDL TrueTypeFont test." ) ;
							
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


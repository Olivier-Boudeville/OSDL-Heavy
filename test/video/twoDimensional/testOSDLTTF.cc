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
using namespace OSDL::Video::Pixels ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::TwoDimensional::Text ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;


#include <string>
using std::string ;



/*
 * TrueType font directory is defined relatively to OSDL documentation 
 * tree, from executable build directory:
 *
 */
const string trueTypeFontDirFromExec = 
	"../../../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in build tree:
 *
 */
const string trueTypeFontDirForBuildPlayTests 
	= "../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in installed tree:
 *
 */
const string trueTypeFontDirForInstalledPlayTests 
	= "../OSDL/doc/web/common/fonts" ;



/*
 * Those are Larabie splendid fonts, see read_me.html under
 * trueTypeFontDirFromExec:
 *
 */
const string firstTrueTypeFontFile  = "cretino.ttf" ;
const string secondTrueTypeFontFile = "iomanoid.ttf" ;
const string thirdTrueTypeFontFile  = "stilltim.ttf" ;

/*
const string fourthTrueTypeFontFile = "earwigfa.ttf" ;
const string fifthTrueTypeFontFile  = "neurochr.ttf" ;
const string sixthTrueTypeFontFile  = "stilltim.ttf" ;
*/


void displayFont( Surface & screen, const string & fontFilename, 
	bool convertToDisplay, Font::RenderCache cache, 
		const string & executableName, bool screenshotWanted )
{

	screen.clear() ;
	
	Coordinate abscissa ;
	
	Ceylan::Uint32 scaleFactor = 75 ;
	
	
	for ( Ceylan::Uint32 i = 0; i < 8; i++ )
	{
	
		string fontPath = TrueTypeFont::FindPathFor( fontFilename ) ;
		
		TrueTypeFont testedFont( fontPath, 
			/* font index */ 0, /* convertToDisplay */ convertToDisplay, 
			/* render cache */ cache ) ;
		
		testedFont.load( /* point size */ 15 + 5 * i ) ;
		
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

				
    	LogPlug::info( "Prerequisite: initializing the display." ) ;	
	      
		     

		bool isBatch = false ;
		
		string executableName ;
		std::list<string> options ;
		
		Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;
		
		string token ;
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
		

		// Initialization section.
		 
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


		// Section for standard operation, using standard files.

		bool correct = false ;
		
		try
		{
		
			TrueTypeFont nonExistingFont( "NotExisting.ttf" ) ;
			nonExistingFont.load() ;
			
		}
		catch( const FontException & e )
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
		
		LogPlug::debug( "Logs about rendering requests come from that test "
			"only, not from the OSDL library." ) ;
		
		if ( randomTestWanted )
		{			
			
	    	LogPlug::info( "Random test: "
				"writing at random places text with random color." ) ;	
		
	    	LogPlug::info( "Prerequisite: having four random generators" ) ;	
		
			Ceylan::Maths::Random::WhiteNoiseGenerator abscissaRand( 0,
				screenWidth ) ;
				
			Ceylan::Maths::Random::WhiteNoiseGenerator ordinateRand( 0,
				screenHeight ) ;
				
			Ceylan::Maths::Random::WhiteNoiseGenerator colorRand( 0, 256 ) ;
			Ceylan::Maths::Random::WhiteNoiseGenerator charRand( 0, 256 ) ;
	
			Coordinate x, y ;
					
			string firstTrueTypeFontPath = TrueTypeFont::FindPathFor(
				firstTrueTypeFontFile ) ;
				
			TrueTypeFont existingFont( firstTrueTypeFontPath, 
				/* font index */ 0, /* convertToDisplay */ true, 
				/* render cache */ Font::None, /* preload */ false ) ;

			existingFont.load( /* point size */ 50 ) ;
			
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
				textSurface->savePNG( string( argv[0] ) + "-text.png" ) ;
			
		
			delete textSurface ;
		
				
			screen.update() ;
				
			if ( screenshotWanted )	
				screen.savePNG( string( argv[0] ) + "-random.png" ) ;
				
			if ( ! isBatch ) 
				myOSDL.getEventsModule().waitForAnyKey() ;					
			
		}
		
		
		if ( qualityTestWanted )
		{
							
	    	LogPlug::info( "Quality test: "
				"rendering texts with different qualities." ) ;
					
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
			
			string firstTrueTypeFontPath = TrueTypeFont::FindPathFor(
				firstTrueTypeFontFile ) ;
				
			TrueTypeFont existingFont( firstTrueTypeFontPath,
				/* font index */ 0, /* convertToDisplay */ true, 
				/* render cache */ Font::None ) ;
			
			existingFont.load( /* point size */ 50 ) ;
				
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
				screen.savePNG( string( argv[0] ) + "-qualities.png" ) ;
			
			if ( ! isBatch ) 
				myOSDL.getEventsModule().waitForAnyKey() ;	
	
		}	
		
		
		if ( edgeTestWanted )
		{

	    	LogPlug::info( "Edge test: "
				"rendering texts at various positions." ) ;

			screen.clear() ;

			if ( gridWanted )
				if ( ! screen.drawGrid() )
					LogPlug::error( "Grid rendering failed." ) ;


			bool manyKeysToHit = false ;
			
			
			displayFont( screen, firstTrueTypeFontFile, 
				/* convertToDisplay */ true, 
				/* render cache */ Font::GlyphCached, 
				string( argv[0] ), screenshotWanted ) ;

			if ( ! isBatch )
				myOSDL.getEventsModule().waitForAnyKey() ;

			
			if ( manyKeysToHit )
			{
			
				displayFont( screen, firstTrueTypeFontFile, 
					/* convertToDisplay */ true, 
					/* render cache */ Font::WordCached,
					string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch )
					myOSDL.getEventsModule().waitForAnyKey() ;

				displayFont( screen, firstTrueTypeFontFile, 
					/* convertToDisplay */ true, 
					/* render cache */ Font::TextCached,
					string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch )
					myOSDL.getEventsModule().waitForAnyKey() ;

			}

		
			displayFont( screen, secondTrueTypeFontFile, 
				/* convertToDisplay */ false, 
				/* render cache */ Font::GlyphCached,
				string( argv[0] ), screenshotWanted ) ;

			if ( ! isBatch )
				myOSDL.getEventsModule().waitForAnyKey() ;
			
			
			if ( manyKeysToHit )
			{
			
				displayFont( screen, secondTrueTypeFontFile, 
					/* convertToDisplay */ false, 
					/* render cache */ Font::WordCached,
					string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch )
					myOSDL.getEventsModule().waitForAnyKey() ;

				displayFont( screen, secondTrueTypeFontFile, 
					/* convertToDisplay */ false, 
					/* render cache */ Font::TextCached,
					string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch )
					myOSDL.getEventsModule().waitForAnyKey() ;
		
			}
		
		
			displayFont( screen, thirdTrueTypeFontFile, 
				/* convertToDisplay */ true, 
				/* render cache */ Font::GlyphCached,
				string( argv[0] ), screenshotWanted ) ;

			if ( ! isBatch )
				myOSDL.getEventsModule().waitForAnyKey() ;
				
		
			if ( manyKeysToHit )
			{
			
				displayFont( screen, thirdTrueTypeFontFile, 
					/* convertToDisplay */ true, 
					/* render cache */ Font::WordCached,
					string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch ) 
					myOSDL.getEventsModule().waitForAnyKey() ;
				
		
				displayFont( screen, thirdTrueTypeFontFile, 
					/* convertToDisplay */ true, 
					/* render cache */ Font::TextCached,
					string( argv[0] ), screenshotWanted ) ;

				if ( ! isBatch ) 
					myOSDL.getEventsModule().waitForAnyKey() ;
				
			}

		}		



		// Section for font loading from archive-embedded file.

		LogPlug::info( 
        	"Now, trying to read a TTF font from an archive-embedded file." ) ;

		EmbeddedFileSystemManager::ArchiveFileLocator.addPath( "../../basic" ) ;
		
		const string archiveFilename = 
			"test-OSDLEmbeddedFileSystem-archive.oar" ;
		     
	 	string archiveFullPath ;
		
		try
		{
			
			// This is (implicitly) the standard filesystem manager here:
			archiveFullPath = EmbeddedFileSystemManager::FindArchivePath( 
				archiveFilename ) ;
				
		}
		catch( const EmbeddedFileSystemManagerException & e )
		{
		        
        	LogPlug::warning( "Test archive '" + archiveFilename 
            	+ "' not found (" + e.toString() + "), run the "
                "test/basic/create-testOSDLEmbeddedFileSystem-archive.sh "
				"script beforehand to have it ready for this test. "
				"Stopping now." ) ;
                
        	return Ceylan::ExitSuccess ;
            
        }

		LogPlug::info( "Test archive '" + archiveFilename 
        	+ "' found, mounting it." ) ;
			
		// Keep the standard manager, to restore it:
		FileSystemManager & standardFSManager =
        	FileSystemManager::GetExistingDefaultFileSystemManager() ;
        

        EmbeddedFileSystemManager & myEmbedddedManager = 		
        	EmbeddedFileSystemManager::GetEmbeddedFileSystemManager() ;
        
 		myEmbedddedManager.chooseBasicSettings( /* organization name */ "OSDL",
            /* application name */ "testOSDLTTF" ) ;

		myEmbedddedManager.setWriteDirectory( "." ) ;
        
 		myEmbedddedManager.mount( archiveFullPath ) ;

        // Thus the font will be searched in specified archive:    
        FileSystemManager::SetDefaultFileSystemManager( myEmbedddedManager,
        	/* deallocatePreviousIfAny */ false ) ;
			
        string targetEmbeddedTTF = "neurochr.ttf" ;

		// Preload implied; platform-independent paths:
					
		screen.lock() ;
		
		screen.fill( Pixels::Black ) ;
		
		screen.unlock() ;

		/*
		 * In a block to reduce scope and force early deletion, so that
		 * the corresponding file is closed and the archive can be
		 * unmounted.
		 *
		 */
		{ 
		
			TrueTypeFont embeddedFont( targetEmbeddedTTF,
				 /* font index */ 0, /* convertToDisplay */ true, 
				 /* render cache */ Font::None ) ;
			
			embeddedFont.load( /* point size */ 40 ),
			
			embeddedFont.blitLatin1Text( screen, 80, 50, 
				"I am a Truetype font", Font::Solid, Pixels::Red ) ;

			embeddedFont.blitLatin1Text( screen, 30, 150, 
				"read from an", Font::Solid, Pixels::Blue ) ;

			embeddedFont.blitLatin1Text( screen, 130, 250, 
				"embedded file", Font::Solid, Pixels::Green ) ;

		}
		
				
		screen.update() ;
		
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
					
 		myEmbedddedManager.umount( archiveFullPath ) ;

		/* 
		 * Will deallocate and replace embedded FS manager, so that the logs
		 * can be written as usual:
		 *
		 */
		FileSystemManager::SetDefaultFileSystemManager( standardFSManager ) ;


		
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
			 + string( e.what() ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
	
        LogPlug::error( "Unknown exception caught" ) ;
       	return Ceylan::ExitFailure ;

    }

    return Ceylan::ExitSuccess ;

}


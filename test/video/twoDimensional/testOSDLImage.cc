#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;

using namespace Ceylan::Log ;
using namespace Ceylan::System ;


#include <string>
using std::string ;



/// Images location:
const std::string firstImageFile  = "Soldier-heavy-purple-small.png" ;
const std::string secondImageFile = "Battle-three-actors-small.jpg"  ;


/*
 * Image font directory is defined relatively to OSDL documentation 
 * tree, from executable build directory:
 *
 */
const std::string imageDirFromExec = "../../../src/doc/web/images" ;


/*
 * Image font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in build tree:
 *
 */
const std::string imageDirForBuildPlayTests = "../src/doc/web/images" ;

/*
 * Image font directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in installed tree:
 *
 */
const std::string imageDirForInstalledPlayTests = "../OSDL/doc/web/images" ;



/**
 * Small usage tests for Image class.
 *
 */ 
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;


	bool screenshotWanted = true ;

    try 
	{
			

    	LogPlug::info( "Testing OSDL Image" ) ;	
         
		
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
		
		 
		// Initialization section.

		OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;		
		
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth, VideoModule::SoftwareSurface ) ;
			
		Surface & screen = myVideo.getScreenSurface() ;



		// Section for standard operation, using standard files.

		LogPlug::info( "Loading image located in " + firstImageFile ) ;
				

		Ceylan::System::FileLocator imageFinder ;
		
		imageFinder.addPath( imageDirFromExec ) ;
		imageFinder.addPath( imageDirForBuildPlayTests ) ;
		imageFinder.addPath( imageDirForInstalledPlayTests ) ;
		
		screen.lock() ;
		
		screen.fill( Pixels::Yellow ) ;
		screen.drawGrid() ;
		
		screen.unlock() ;

 		screen.loadImage( imageFinder.find( firstImageFile ), 
			/* blit only */ true ) ;
				
		screen.update() ;
		
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
			
			
		LogPlug::info( "Loading image located in "	+ secondImageFile 
			+ " with request of image format auto-detection." ) ;

		Surface & other = Surface::LoadImage( 
			imageFinder.find( secondImageFile ) ) ;
			
		other.blitTo( screen, 270, 155 ) ;
				
		screen.update() ;
		
		if ( screenshotWanted )
			screen.savePNG( std::string( argv[0] ) + ".png" ) ;
							
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
			
		delete & other ;
			
			
			
			
		// Section for image loading from archive-embedded file.
					
					
					
		LogPlug::info( 
        	"Now, trying to read images from an archive-embedded file." ) ;

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
                "create-testOSDLEmbeddedFileSystem-archive.sh script "
                "beforehand to have it ready for this test. Stopping now." ) ;
                
        	return 0 ;
            
        }

		LogPlug::info( "Test archive '" + archiveFilename 
        	+ "' found, mounting it." ) ;
			
		// Keep the standard manager, to restore it:
		FileSystemManager & standardFSManager =
        	FileSystemManager::GetExistingDefaultFileSystemManager() ;
        

        EmbeddedFileSystemManager & myEmbedddedManager = 		
        	EmbeddedFileSystemManager::GetEmbeddedFileSystemManager() ;
        
 		myEmbedddedManager.chooseBasicSettings( /* organization name */ "OSDL",
            /* application name */ "testOSDLImage" ) ;

		myEmbedddedManager.setWriteDirectory( "." ) ;
        
 		myEmbedddedManager.mount( archiveFullPath ) ;

        // Thus images will be searched in specified archive:    
        FileSystemManager::SetDefaultFileSystemManager( myEmbedddedManager,
        	/* deallocatePreviousIfAny */ false ) ;
			
        string targetEmbeddedPNG = "Soldier-heavy-purple-small.png" ;

		// Preload implied; platform-independent paths:
 		Surface & embeddedImage = Surface::LoadImage( 
			"test-OSDLEmbeddedFileSystem-archive/" + targetEmbeddedPNG  ) ;
				
		screen.lock() ;
		
		screen.fill( Pixels::Red ) ;
		
		screen.unlock() ;

		Surface & flippedImage = embeddedImage.flipVertical() ;

		delete & embeddedImage ;
		
		flippedImage.blitTo( screen, 80, 50 ) ;

		delete & flippedImage ;

        string targetEmbeddedJPG = "Rune-stone-small.jpg" ;
		
		// Preload implied; platform-independent paths:
 		Surface & otherEmbeddedImage = Surface::LoadJPG( 
			"test-OSDLEmbeddedFileSystem-archive/" 	+ targetEmbeddedJPG,  
			/* blitOnly */ false ) ;

		otherEmbeddedImage.blitTo( screen, 300, 50 ) ;
		
		delete & otherEmbeddedImage ;
		
		screen.update() ;
		
		if ( ! isBatch )				
			myOSDL.getEventsModule().waitForAnyKey() ;
					
 		myEmbedddedManager.umount( archiveFullPath ) ;

		// Will deallocate embedded FS manager; prepare for log writing:
		FileSystemManager::SetDefaultFileSystemManager( standardFSManager ) ;

		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;
		
		LogPlug::info( "End of OSDL Image test." ) ;
	
		
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


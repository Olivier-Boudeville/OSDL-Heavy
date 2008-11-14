#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Audio ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using std::string ;


/*
 * Music directory is defined relatively to OSDL documentation 
 * tree, from executable build directory :
 *
 */
const std::string musicDirFromExec = "../../src/doc/web/common/sounds" ;


/*
 * Music directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in build tree :
 *
 */
const std::string musicDirForBuildPlayTests 
	= "../src/doc/web/common/sounds" ;


/*
 * Music directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in installed tree :
 *
 */
const std::string musicDirForInstalledPlayTests 
	= "../OSDL/doc/web/common/sounds" ;




/**
 * Testing the music management of the OSDL audio module.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;

	
    try 
	{
	
	
		LogPlug::info( "Testing OSDL music services." ) ;


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
			
		LogPlug::info( "Starting OSDL with audio enabled." )	;
			
        CommonModule & myOSDL = getCommonModule( CommonModule::UseAudio ) ;		
			
		myOSDL.logState() ;
					
		LogPlug::info( "Testing real audio (audible)." ) ;
		
		LogPlug::info( "Getting audio module." ) ;
		AudioModule & myAudio = myOSDL.getAudioModule() ; 
		
		myAudio.logState() ;
											
		Ceylan::Maths::Hertz outputFrequency = 22050 ;
		
		SampleFormat outputSampleFormat = 
			AudioModule::NativeSint16SampleFormat ;
			
		ChannelFormat outputChannel = AudioModule::Stereo ;
		
		ChunkSize chunkSize = 4096 ;
		
		LogPlug::info( "Requesting a mixing mode at " 
			+ Ceylan::toString( outputFrequency ) + " Hz, with sample format " 
			+ sampleFormatToString( outputSampleFormat ) 
			+ ", channel format " 
			+ channelFormatToString( outputChannel ) 
			+ " and a chunk size of " + Ceylan::toString( chunkSize )
			+ " bytes." ) ;
			
		myAudio.setMode( outputFrequency, 
			/* sample format */ AudioModule::NativeSint16SampleFormat,
			outputChannel, chunkSize ) ;
		
		myAudio.logState() ;

		ChannelNumber count ;
		
		Ceylan::System::Millisecond latency = myAudio.getObtainedMode(
			outputFrequency, outputSampleFormat, count ) ;
			
		LogPlug::info( "Obtained a mixing mode at " 
			+ Ceylan::toString( outputFrequency ) + " Hz, with sample format " 
			+ sampleFormatToString( outputSampleFormat ) + " and " 
			+ Ceylan::toString( count ) 
			+ " channel(s), which results in a mean theoritical latency of " 
			+ Ceylan::toString( latency ) + " milliseconds." ) ;
					


		// Section for music playback from standard file.


		AudioModule::AudioFileLocator.addPath( musicDirFromExec ) ;
		AudioModule::AudioFileLocator.addPath( musicDirForBuildPlayTests ) ;
		AudioModule::AudioFileLocator.addPath( musicDirForInstalledPlayTests ) ;

		// WAV not allowed anymore as musics: string targetMusic = "OSDL.wav" ;
		string targetMusic = "welcome-to-OSDL.ogg" ;
		
		LogPlug::info( "Loading first music file '" + targetMusic 
			+ "' thanks to audio locator." ) ;
			
        // Preload implied:
		Music myFirstMusic( Audible::FindAudiblePath( targetMusic ) ) ;
		
		LogPlug::info( "Loaded music: " + myFirstMusic.toString() ) ;
		
		if ( ! isBatch )
		{
		
			LogPlug::info( "Playing music now." ) ;
		
			myFirstMusic.play() ;
		
			LogPlug::info( "Waiting for this music to finish." ) ;
		
			while ( myAudio.isMusicPlaying() )
				Ceylan::System::basicSleep( /* Microsecond */ 1000 ) ;

			LogPlug::info( "Music finished." ) ;
		
		}
		else
		{
		
			LogPlug::info( "In batch mode, hence first music not played." ) ;
			
		}
		
		LogPlug::info( "Loading second music file '" + targetMusic 
			+ "' thanks to audio locator." ) ;
			
		// Preload implied:
		Music mySecondMusic( Audible::FindAudiblePath( targetMusic ) ) ;
		
		LogPlug::info( "Loaded music: " + mySecondMusic.toString() ) ;


		if ( ! isBatch )
		{
			
			LogPlug::info( 
				"Running in interactive mode, more tests performed." ) ;
		
			LogPlug::info( "Playing music now, with fade in "
				"and from a position around the middle, and repeat once." ) ;
		
		
			// Position in seconds:
			MusicPosition startPosition ;
			
			if ( isBatch )
				startPosition = 5 ;
			else
				startPosition = 1 ;
				
			mySecondMusic.playWithFadeInFromPosition( 
				/* fadeInMaxDuration */ 5000, startPosition, 
				/* playCount */ 2 ) ;
		
			LogPlug::info( "Waiting for this music to finish." ) ;
		
			while ( myAudio.isMusicPlaying() )
				Ceylan::System::basicSleep( /* Microsecond */ 1000 ) ;

			LogPlug::info( "Music finished." ) ;
		
		}
		
		
		// Short enough even for batch mode:

		LogPlug::info( "Playing that music with a 2-second fade-in, "
			"and from the start." ) ;			
		
		mySecondMusic.rewind() ;
		
		mySecondMusic.playWithFadeIn( /* fadeInMaxDuration */ 2000 ) ;
		
		LogPlug::info( "Waiting for this music to finish." ) ;
		
		while ( myAudio.isMusicPlaying() )
			Ceylan::System::basicSleep( /* Microsecond */ 1000 ) ;

		LogPlug::info( "Music finished." ) ;




		// Section for music playback from archive-embedded file.
		
		
		LogPlug::info( 
        	"Now, trying to read music from an archive-embedded file." ) ;

		EmbeddedFileSystemManager::ArchiveFileLocator.addPath( "../basic" ) ;
		
		//const string archiveFilename = 
		//	"test-OSDLEmbeddedFileSystem-archive.oar" ;
		
		const string archiveFilename = 
			"test-archive.zip" ;
     
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
            /* application name */ "testOSDLMusic" ) ;

		myEmbedddedManager.setWriteDirectory( "." ) ;
        
 		myEmbedddedManager.mount( archiveFullPath ) ;

        // Thus music will be searched in specified archive:    
        FileSystemManager::SetDefaultFileSystemManager( myEmbedddedManager,
        	/* deallocatePreviousIfAny */ false ) ;
			
        string targetEmbeddedMusic = "myogg-cyphered.ogg" ;
		
		// Preload implied; platform-independent paths:
		Music myEmbeddedMusic(
        	"test-OSDLEmbeddedFileSystem-archive/" + targetEmbeddedMusic ) ;

		
		if ( ! isBatch )
		{
		
			LogPlug::info( "Playing embedded music now." ) ;
		
			myEmbeddedMusic.play() ;
		
			LogPlug::info( "Waiting for this embedded music to finish." ) ;
		
			while ( myAudio.isMusicPlaying() )
				Ceylan::System::basicSleep( /* Microsecond */ 1000 ) ;

			LogPlug::info( "Embedded music finished." ) ;
		
		}
		else
		{
		
			LogPlug::info( "In batch mode, hence embedded music not played." ) ;
			
		}

		// Otherwise archive could not be unmounted:
		myEmbeddedMusic.unload() ;
		
 		myEmbedddedManager.umount( archiveFullPath ) ;

		// Will deallocate embedded FS manager; prepare for log writing:
		FileSystemManager::SetDefaultFileSystemManager( standardFSManager ) ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		LogPlug::info( "End of OSDL music test." ) ;
		
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


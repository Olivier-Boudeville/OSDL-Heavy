#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Audio ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using std::string ;


/*
 * Sound directory is defined relatively to OSDL documentation 
 * tree, from executable build directory :
 *
 */
const std::string soundDirFromExec = "../../src/doc/web/common/sounds" ;


/*
 * Sound directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in build tree :
 *
 */
const std::string soundDirForBuildPlayTests 
	= "../src/doc/web/common/sounds" ;


/*
 * Sound directory is defined relatively to OSDL documentation tree,
 * from playTests.sh location in installed tree :
 *
 */
const std::string soundDirForInstalledPlayTests 
	= "../OSDL/doc/web/common/sounds" ;




/**
 * Testing the sound management of the OSDL audio module.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;

	
    try 
	{
	
	
		LogPlug::info( "Testing OSDL sound services." ) ;


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

		
					
		// Section for sound playback from standard file.

	
		AudioModule::AudioFileLocator.addPath( soundDirFromExec ) ;
		AudioModule::AudioFileLocator.addPath( soundDirForBuildPlayTests ) ;
		AudioModule::AudioFileLocator.addPath( soundDirForInstalledPlayTests ) ;

		string targetSound = "OSDL.wav" ;
		
		LogPlug::info( "Loading first sound file '" + targetSound 
			+ "' thanks to audio locator." ) ;
			
		// Preload implied:	
		Sound myFirstSound( Audible::FindAudiblePath( targetSound ) ) ;
		
		LogPlug::info( "Loaded sound: " + myFirstSound.toString() ) ;
		
		LogPlug::info( "Playing sound now." ) ;
		
		myFirstSound.play() ;
		
		LogPlug::info( "Waiting for this sound to finish." ) ;
		
		while ( myAudio.getPlayingChannelCount() != 0 )
			Ceylan::System::basicSleep( /* Microsecond */ 1000 ) ;

		LogPlug::info( "Sound finished." ) ;

		// Stereo sound, useful to test panning:
		targetSound = "welcome-to-OSDL.ogg" ;

		LogPlug::info( "Loading second sound file '" + targetSound 
			+ "' thanks to audio locator." ) ;
			
		// Preload implied:	
		Sound mySecondSound( Audible::FindAudiblePath( targetSound ) ) ;
		
		LogPlug::info( "Loaded sound: " + mySecondSound.toString() ) ;


		if ( ! isBatch )
		{
			
			LogPlug::info( 
				"Running in interactive mode, more tests performed." ) ;
		
			LogPlug::info( "Playing sound now." ) ;
		
			ChannelNumber channelNumber = mySecondSound.playReturnChannel() ;
		
			LogPlug::info( "Waiting for this sound to finish, playing in "
				"channel #" + Ceylan::toString( channelNumber ) + "." ) ;
		
			AudioChannel & channel = 
				myAudio.getMixingChannelAt( channelNumber ) ;
			
			Ceylan::Maths::Percentage percentage = 0 ;
			
			/*
			 * Sound lasts for about 7 seconds, time-slice will be 200 ms, so
			 * percentage increment will be 100*200/7000=3.
			 *
			 * Sound should go from the right to the left.
			 * 
			 */	
			while ( channel.isPlaying() )
			{
			
				percentage += 3 ;
				channel.setPanning( /* left */ percentage ) ;
				
				Ceylan::System::basicSleep( /* Microsecond */ 200000 ) ;
				
			}	

			LogPlug::info( "Sound finished." ) ;
		
		}


		// Short enough even for batch mode:

		LogPlug::info( "Playing that sound with a fade-in now, "
			"and only for 2 seconds." ) ;			
		
		mySecondSound.playWithFadeInForAtMostReturnChannel( 
			/* playbackMaxDuration */ 2000, /* fadeInMaxDuration */ 2000 ) ;
		
		LogPlug::info( "Waiting for this sound to finish." ) ;
		
		while ( myAudio.getPlayingChannelCount() != 0 )
			Ceylan::System::basicSleep( /* Microsecond */ 1000 ) ;

		LogPlug::info( "Sound finished." ) ;
		
		
		

		// Section for sound playback from archive-embedded file.
		
		
		LogPlug::info( 
        	"Now, trying to read sound from an archive-embedded file." ) ;

		EmbeddedFileSystemManager::ArchiveFileLocator.addPath( "../basic" ) ;
		
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
                "test/basic/create-testOSDLEmbeddedFileSystem-archive.sh 
				"script beforehand to have it ready for this test. "
				"Stopping now." ) ;
                
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
            /* application name */ "testOSDLSound" ) ;

		myEmbedddedManager.setWriteDirectory( "." ) ;
        
 		myEmbedddedManager.mount( archiveFullPath ) ;

        // Thus sound will be searched in specified archive:    
        FileSystemManager::SetDefaultFileSystemManager( myEmbedddedManager,
        	/* deallocatePreviousIfAny */ false ) ;
			
        string targetEmbeddedSound = "OSDL.wav" ;
		
		// Preload implied; platform-independent paths:
		Sound myEmbeddedSound(
        	"test-OSDLEmbeddedFileSystem-archive/" + targetEmbeddedSound ) ;

		
		if ( ! isBatch )
		{
		
			LogPlug::info( "Playing embedded sound now." ) ;
		
			myEmbeddedSound.play() ;
		
			LogPlug::info( "Waiting for this embedded sound to finish." ) ;
		
			while ( myAudio.getPlayingChannelCount() > 0 )
				Ceylan::System::basicSleep( /* Microsecond */ 1000 ) ;

			LogPlug::info( "Embedded sound finished." ) ;
		
		}
		else
		{
		
			LogPlug::info( "In batch mode, hence embedded sound not played." ) ;
			
		}

		// Otherwise archive could not be unmounted:
		myEmbeddedSound.unload() ;
		
 		myEmbedddedManager.umount( archiveFullPath ) ;

		// Will deallocate embedded FS manager; prepare for log writing:
		FileSystemManager::SetDefaultFileSystemManager( standardFSManager ) ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		LogPlug::info( "End of OSDL sound test." ) ;
		
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


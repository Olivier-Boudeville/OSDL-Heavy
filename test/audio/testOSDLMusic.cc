/*
 * Copyright (C) 2003-2013 Olivier Boudeville
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
using namespace OSDL::Audio ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using std::string ;


/*
 * Music directory is defined relatively to OSDL documentation tree, from
 * executable build directory:
 *
 */
const std::string musicDirFromExec = "../../src/doc/web/common/sounds" ;


/*
 * Music directory is defined relatively to OSDL documentation tree, from
 * playTests.sh location in build tree:
 *
 */
const std::string musicDirForBuildPlayTests
= "../src/doc/web/common/sounds" ;


/*
 * Music directory is defined relatively to OSDL documentation tree, from
 * playTests.sh location in installed tree:
 *
 */
const std::string musicDirForInstalledPlayTests
= "../OSDL/doc/web/common/sounds" ;



/*
 * Implementation notes:
 *
 * For this test, we used to use automatic variables for musics, like in:
 * 'Music myFirstMusic( Audible::FindAudiblePath( targetMusic ) ) ;'.
 *
 * This was a mistake, as this led these variables to be deallocated after OSDL
 * (hence SDL_mixer) was stopped, resulting in a crash:
 *
 * #0  0x00007fffee4e2760 in ?? ()
 * #1  0x00007ffff4efd01e in OGG_delete () from libSDL_mixer-1.2.so.0
 * #2  0x00007ffff4efa3bb in Mix_FreeMusic () from libSDL_mixer-1.2.so.0
 * #3  0x00007ffff7ac48d0 in OSDL::Audio::Music::unload (this=0x7fffffffc210)
 *  at OSDLMusic.cc:499
 * #4  0x00007ffff7ac431f in OSDL::Audio::Music::~Music (this=0x7fffffffc210,
 *__in_chrg=<value optimized out>) at OSDLMusic.cc:253
 * #5  0x0000000000404fa4 in main (argc=1, argv=0x7fffffffc9a8) at
 * testOSDLMusic.cc pointing to the creation of the music automatic variable.
 *
 * Now musics are dynamically created instead.
 *
 */



/**
 * Testing the music management of the OSDL audio module.
 *
 */
int main( int argc, char * argv[] )
{

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

	  LogPlug::info( "Starting OSDL with audio enabled." ) ;

	  CommonModule & myOSDL = getCommonModule( CommonModule::UseAudio ) ;

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

	  //myAudio.logState() ;

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

	  /*
	   * Preload implied; not an automatic variable to be able to deallocate
	   * it when wanted (before stopping OSDL):
	   *
	   */
	  Music * myFirstMusic = new Music(
		Audible::FindAudiblePath( targetMusic ) ) ;

	  LogPlug::info( "Loaded music: " + myFirstMusic->toString() ) ;

	  if ( ! isBatch )
	  {

		LogPlug::info( "Playing music now." ) ;

		myFirstMusic->play() ;

		LogPlug::info( "Waiting for this music to finish." ) ;

		while ( myAudio.isMusicPlaying() )
		  Ceylan::System::basicSleep( /* Microsecond */ 1000 ) ;

		LogPlug::info( "Music finished." ) ;

	  }
	  else
	  {

		LogPlug::info( "In batch mode, hence first music not played." ) ;

	  }

	  delete myFirstMusic ;


	  LogPlug::info( "Loading second music file '" + targetMusic
		+ "' thanks to audio locator." ) ;

	  // Preload implied, targeting actually the same file:
	  Music * mySecondMusic = new Music(
		Audible::FindAudiblePath( targetMusic ) ) ;

	  LogPlug::info( "Loaded music: " + mySecondMusic->toString() ) ;


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

		mySecondMusic->playWithFadeInFromPosition(
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

	  mySecondMusic->rewind() ;

	  mySecondMusic->playWithFadeIn( /* fadeInMaxDuration */ 2000 ) ;

	  LogPlug::info( "Waiting for this music to finish." ) ;

	  while ( myAudio.isMusicPlaying() )
		Ceylan::System::basicSleep( /* Microsecond */ 1000 ) ;

	  LogPlug::info( "Music finished." ) ;

	  delete mySecondMusic ;



	  // Section for music playback from an archive-embedded file.


	  LogPlug::info(
		"Now, trying to read music from an archive-embedded file." ) ;

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
		  "test/basic/create-testOSDLEmbeddedFileSystem-archive.sh "
		  "script beforehand to have it ready for this test. "
		  "Stopping now." ) ;

		OSDL::stop() ;

		OSDL::shutdown() ;

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
		/* application name */ "testOSDLMusic" ) ;

	  myEmbedddedManager.setWriteDirectory( "." ) ;

	  myEmbedddedManager.mount( archiveFullPath ) ;

	  // Thus music will be searched in specified archive:
	  FileSystemManager::SetDefaultFileSystemManager( myEmbedddedManager,
		/* deallocatePreviousIfAny */ false ) ;

	  string targetEmbeddedMusic = "welcome-to-OSDL.music" ;

	  // Preload implied; platform-independent paths:
	  Music * myEmbeddedMusic = new Music( targetEmbeddedMusic ) ;


	  if ( ! isBatch )
	  {

		LogPlug::info( "Playing embedded music now." ) ;

		myEmbeddedMusic->play() ;

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
	  myEmbeddedMusic->unload() ;

	  delete myEmbeddedMusic ;

	  myEmbedddedManager.umount( archiveFullPath ) ;

	  /*
	   * Will deallocate and replace embedded FS manager, so that the logs can
	   * be written as usual:
	   *
	   */
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

  }

  OSDL::shutdown() ;

  return Ceylan::ExitSuccess ;

}

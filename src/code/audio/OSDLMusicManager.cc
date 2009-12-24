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


#include "OSDLMusicManager.h"        // implies including Ceylan.h

#include "OSDLAudio.h"               // for AudioModule


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS



#if OSDL_USES_SDL
#include "SDL.h"                     // for SDL_InitSubSystem
#endif // OSDL_USES_SDL

#if OSDL_USES_SDL_MIXER
#include "SDL_mixer.h"               // for Mix_OpenAudio and al
#endif // OSDL_USES_SDL_MIXER


#define OSDL_DEBUG_MUSIC_PLAYBACK 1


// Replicating these defines allows to enable them on a per-class basis:
#if OSDL_DEBUG_MUSIC_PLAYBACK

#define LOG_DEBUG_AUDIO(message)   LogPlug::debug(message)
#define LOG_TRACE_AUDIO(message)   LogPlug::trace(message)
#define LOG_WARNING_AUDIO(message) LogPlug::warning(message)

#else // OSDL_DEBUG_MUSIC_PLAYBACK

#define LOG_DEBUG_AUDIO(message)
#define LOG_TRACE_AUDIO(message)
#define LOG_WARNING_AUDIO(message)

#endif // OSDL_DEBUG_MUSIC_PLAYBACK



using std::string ;
using std::list ;


using Ceylan::System::Millisecond ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL::Audio ;




/**
 * Implementation notes:
 *
 */




MusicManagerException::MusicManagerException( const string & reason ) :
	AudioException( reason )
{

}


	
MusicManagerException::~MusicManagerException() throw()
{

}





// Music playback setting section.


MusicPlaybackSetting::MusicPlaybackSetting( Music & music, 
		PlaybackCount count ) :
	_music( & music ),
	_count( count ),
	_fadeIn( false ),
	_fadeOut( false )
{

	if ( count == 0 )
		throw AudioException( "MusicPlaybackSetting construction failed: "
			"null playback count specified" ) ;
			
	if ( count < Audio::Loop )
		throw AudioException( "MusicPlaybackSetting construction failed: "
			"unexpected playback count specified" ) ;

	//_music->addRef() ;
}



MusicPlaybackSetting::~MusicPlaybackSetting() throw()
{

	// _music not owned.
	
}



void MusicPlaybackSetting::setFadeInStatus( bool on )
{

	_fadeIn = on ;
	
}



void MusicPlaybackSetting::setFadeOutStatus( bool on )
{

	_fadeOut = on ;
	
}



void MusicPlaybackSetting::startPlayback()
{
	
	
	if ( _fadeIn )
	{
	
		// Fade-in wanted.
	
		if ( _fadeOut )
		{
	
			// Fade-out wanted.
			
			// Fade-out not supported yet!		
			_music->playWithFadeIn( _count ) ;
		
		}
		else
		{
	
			// No fade-out wanted.
			_music->playWithFadeIn( _count ) ;
			
		}
		
	}
	else
	{
	
		// No fade-in wanted.
		
		if ( _fadeOut )
		{
	
			// Fade-out wanted.
	
			// Fade-out not supported yet!		
			_music->play( _count ) ;
		
		}
		else
		{
	
			// No fade-out wanted.
			_music->play( _count ) ;
			
		}
	}
	
}



void MusicPlaybackSetting::stopPlayback()
{

	_music->stop() ;

}



bool MusicPlaybackSetting::onPlaybackEnded()
{

	/*
	 * Expected to be called only once all the loops for that playback will be
	 * over.
	 *
	 */
	
	LogPlug::trace( "MusicPlaybackSetting::onPlaybackEnded: " 
		+ toString() ) ;
	
	//_music->removeRef() ;
	
	// One more playback triggered:
	
	/*	
	 * Count is directly managed by the Music::play* methods.
	 *
	 
	if ( _count == Loop )
		return true ;
		
	_count-- ;
	
	return ( _count != 0 ) ;
	 */
	
	// Therefore this playback has never to be kept:
	return false ; 
	
}



const string MusicPlaybackSetting::toString( Ceylan::VerbosityLevels level )
	const
{
	
	string res = "Music playback setting referencing '" 
		+ _music->toString( level )
		+ string( "', with " ) + ( _fadeIn ? "a " : "no " ) 
		+ string( "fade-in and " ) + ( _fadeOut ? "a " : "no " ) 
		+ string( "fade-out, " ) ;
	
	if ( _count == Loop )
		return res + "and (infinite) looping requested" ;
	else
		return res + "with " + Ceylan::toString( _count ) 
			+ " planned playbacks" ; 	
			
}





// Music manager section.


MusicManager * MusicManager::_CurrentMusicManager = 0 ;


MusicManager::MusicManager():
	_currentMusicPlayback( 0 )
{
			
	// Registers callback-like method:
	::Mix_HookMusicFinished( HandleMusicPlaybackFinishedCallback ) ;
	
}



MusicManager::~MusicManager() throw()
{

	Ceylan::checkpoint( "MusicManager destructor." ) ;
	
	send( "Deleting music manager, whose state was: " + toString() ) ;


	/*
	 * Removing first the next musics, as otherwise stopping the current one
	 * would make us continue with them:
	 *
	 */
		
	Ceylan::Uint32 playbackCount = _playList.size() ;
	
	if ( playbackCount != 0 )
	{
	
		LogPlug::warning( "MusicManager destructor: still "
			+ Ceylan::toString( playbackCount ) 
			+ " music playback(s) in playlist, removing them." ) ;	
	
		for ( list<MusicPlaybackSetting *>::iterator it = _playList.begin();
			it != _playList.end(); it++ )
		{
		
			delete( *it ) ;
			
		}
			
	}		

	stopCurrentMusicPlayback() ;
	
}



void MusicManager::enqueue( Music & music, PlaybackCount count,
	bool fadeIn, bool fadeOut )
{

	MusicPlaybackSetting * newSetting = new MusicPlaybackSetting( music,
		count ) ;
	
	if ( fadeIn ) 
		newSetting->setFadeInStatus( true ) ;
	
	if ( fadeOut ) 
		newSetting->setFadeOutStatus( true ) ;
	
	_playList.push_back( newSetting ) ;
		
	// Starts playback immediately if none is currently in progress:	
	if ( _currentMusicPlayback == 0 )
		startNextMusicPlayback() ;
	
}



bool MusicManager::isPlaying() const
{

	return _currentMusicPlayback != 0 ;
	
}



const string MusicManager::toString( Ceylan::VerbosityLevels level ) const
{
	
	string res = "Music manager with " ;
	
	if ( _currentMusicPlayback != 0 )
	{
	
		res += "following current playback: '" 
			+ _currentMusicPlayback->toString() + "'registered" ;
	 
	}
	else
	{
	
		res = "no current playback registered" ;
	
	}
	
	if ( level == Ceylan::low )
		return res + ". Current playlist has " 
			+ Ceylan::toString( _playList.size() ) + " entries" ;
	
	list<string> playbacks ;
	
	for ( list<MusicPlaybackSetting *>::const_iterator it = _playList.begin();
		it != _playList.end(); it++ )
	{
	
		playbacks.push_back( (*it)->toString( level ) ) ;
	
	}					
 	
	if ( playbacks.empty() )
		return res + ". Current playlist is empty" ;
	else
		return res + ". Current playlist is: " 
			+ Ceylan::formatStringList( playbacks ) ;
		
}









// Protected section.


void MusicManager::startNextMusicPlayback()
{

#if OSDL_DEBUG_MUSIC_PLAYBACK

	if ( _playList.empty() )
		throw MusicManagerException( "MusicManager::startNextMusicPlayback: "
			"called with an empty playlist." ) ;
	
	if ( _currentMusicPlayback != 0 )
		throw MusicManagerException( "MusicManager::startNextMusicPlayback: "
			"called while a music is still being played." ) ;
	
#endif // OSDL_DEBUG_MUSIC_PLAYBACK

	_currentMusicPlayback = _playList.front() ;
	
	_playList.pop_front() ;
	
	_currentMusicPlayback->startPlayback() ;

}



void MusicManager::stopCurrentMusicPlayback()
{

	Ceylan::checkpoint( "MusicManager::stopCurrentMusicPlayback." ) ;
	
	if ( _currentMusicPlayback != 0 )
	{
	
		_currentMusicPlayback->stopPlayback() ;
	
	}
	
#if OSDL_USES_SDL_MIXER
	
	LogPlug::trace( "MusicManager::stopCurrentMusicPlayback" ) ;
	
	/*
	 * No test performed about music availability:
	 * (and should trigger the proper onMusicPlaybackFinished callback)
	 *
	 */
	::Mix_HaltMusic() ;
			
#else // OSDL_USES_SDL_MIXER

	throw MusicManagerException( 
		"MusicManager::stopCurrentMusicPlayback failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER
	
	if ( _currentMusicPlayback != 0 )
	{
	
		delete _currentMusicPlayback ;
		_currentMusicPlayback = 0 ;
	
	}	
	else
	{
	
		LogPlug::warning( "MusicManager::stopCurrentMusicPlayback: "
			"no music was being played." ) ;
			
	}
	
}



void MusicManager::onMusicPlaybackFinished() 
{

	Ceylan::checkpoint( "MusicManager::onMusicPlaybackFinished." ) ;
	
	if ( _currentMusicPlayback == 0 )
	{
	
		LogPlug::debug( "MusicManager::onMusicPlaybackFinished failed: "
			"no music was being played. Supposing the music playback was "
			"triggered directly on the music, rather than thanks to "
			"this music manager." ) ;
	
		return ;
	
	}
		
	if ( _currentMusicPlayback->onPlaybackEnded() == false )
	{
	
		delete _currentMusicPlayback ;
	
		_currentMusicPlayback = 0 ;
	
	}
	
	if ( ! _playList.empty() )
		startNextMusicPlayback() ; 
	
}




// Private section.


/*
 * Non-static method or classical function needed for call-back.
 *
 * Overrides the default callback set by the audio module.
 *
 */
void MusicManager::HandleMusicPlaybackFinishedCallback()
{

	OSDL::Audio::getExistingMusicManager().onMusicPlaybackFinished() ;
	
}




// Friend function.


MusicManager & OSDL::Audio::getExistingMusicManager()
{

	if ( MusicManager::_CurrentMusicManager == 0 )
		throw MusicManagerException( "OSDL::Audio::getExistingMusicManager "
			"failed: no manager available." ) ;
	
	return * MusicManager::_CurrentMusicManager ;
		
}


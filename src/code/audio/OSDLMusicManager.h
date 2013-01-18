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


#ifndef OSDL_MUSIC_MANAGER_H_
#define OSDL_MUSIC_MANAGER_H_


#include "OSDLAudioCommon.h"    // for MusicManagerType, etc.
#include "OSDLAudible.h"        // for PlaybackCount


#include <string>





namespace OSDL
{



	namespace Audio
	{



		// A music manager controls them:
		class Music ;




		/// Exceptions raised by MusicManager instances.
		class OSDL_DLL MusicManagerException : public AudioException
		{
			public:

				MusicManagerException( const std::string & reason ) ;
				virtual ~MusicManagerException() throw() ;
		} ;


		// Forward declaration for the next declaration:
		class MusicManager ;



		/**
		 * This function is intended to be the usual means of getting a
		 * reference to the music manager, which must already exist.
		 *
		 * If not, a fatal error will be triggered (not an exception to avoid
		 * handling it in all user methods).
		 *
		 * @note This function is mainly useful for the OSDL internals.
		 *
		 * @note This method is not static to avoid pitfalls of static
		 * initializer ordering.
		 *
		 */
		OSDL_DLL MusicManager & getExistingMusicManager() ;



		/**
		 * Describes all information needed for the playback of a music.
		 *
		 * Used by the music manager.
		 *
		 */
		class OSDL_DLL MusicPlaybackSetting : public Ceylan::TextDisplayable
		{

			public:


				/**
				 * Defines a new set of settings for the playback of specified
				 * music, whose ownership is not taken.
				 *
				 * By default a playback will be done with no fade-in nor
				 * fade-out.
				 *
				 * @param music the music to playback.
				 *
				 * @param count the number of times this music should be played
				 * (-1 means infinite).
				 *
				 */
				explicit MusicPlaybackSetting( Music & music,
					PlaybackCount count = 1 ) ;


				/// Virtual destructor.
				virtual ~MusicPlaybackSetting() throw() ;



				/**
				 * The playback will start with a fade-in iff true is specified.
				 *
				 */
				virtual void setFadeInStatus( bool on ) ;



				/**
				 * The playback will end with a fade-out iff true is specified.
				 *
				 */
				virtual void setFadeOutStatus( bool on ) ;



				/**
				 * Starts immediately the play-back corresponding to these
				 * settings.
				 *
				 * Does not return whether this playback should be kept, as
				 * anyway it should be removed once the playback is over, so
				 * that no music still being played back is removed if its
				 * reference counter drops to zero.
				 *
				 */
				virtual void startPlayback() ;


				/**
				 * Stops immediately the play-back corresponding to these
				 * settings.
				 *
				 */
				virtual void stopPlayback() ;



				/**
				 * Called by the music manager whenever this playback is over.
				 *
				 * @return true iff this playback must be kept by the music
				 * manager.
				 *
				 */
				virtual bool onPlaybackEnded() ;



				/**
				 * Returns an user-friendly description of the state of this
				 * object.
				 *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
				 *
				 */
				virtual const std::string toString(
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;




			protected:


				/// The music to playback.
				Music * _music ;


				/// The number of remaining playbacks (can be Loop).
				PlaybackCount _count ;


				/// Tells whether the playback should start with a fade-in.
				bool _fadeIn ;


				/// Tells whether the playback should end with a fade-out.
				bool _fadeOut ;



			private:


				/**
				 * Copy constructor made private to ensure that it will be never
				 * called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 *
				 */
				explicit MusicPlaybackSetting(
					const MusicPlaybackSetting & source ) ;



				/**
				 * Assignment operator made private to ensure that it will be
				 * never called.
				 *
				 * The compiler should complain whenever this undefined operator
				 * is called, implicitly or not.
				 *
				 */
				MusicPlaybackSetting & operator = (
					const MusicPlaybackSetting & source ) ;

		} ;





		/**
		 * A music manager has for role to drive the playback of musics, based
		 * on a playlist.
		 *
		 * Although multiple managers can be used simultaneously, usely there is
		 * only one of them, statically registered by the audio module.
		 *
		 * @see Music
		 *
		 */
		class OSDL_DLL MusicManager : public Ceylan::Object
		{


			// To allow the audio module to remove the music manager:
			friend class AudioModule ;


			OSDL_DLL friend MusicManager &
				OSDL::Audio::getExistingMusicManager() ;


			public:



				/**
				 * Creates the music manager.
				 *
				 * @throw MusicManagerException if the creation failed,
				 * including if there is already a music manager registered.
				 *
				 */
				MusicManager() ;



				/// Virtual destructor.
				virtual ~MusicManager() throw() ;



				/**
				 * Enqueues specified music in the playlist of this music
				 * manager.
				 *
				 * @param music the music which will be played as soon as all
				 * other musics in the playlist (if any) will have been played
				 * in turn.
				 *
				 */
				virtual void enqueue( Music & music, PlaybackCount count = 1,
					bool fadeIn = false, bool fadeOut = false ) ;



				/**
				 * Returns true iff a music is playing.
				 *
				 */
				virtual bool isPlaying() const ;



				/**
				 * Returns an user-friendly description of the state of this
				 * object.
				 *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
				 *
				 */
				virtual const std::string toString(
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;





			protected:



				/**
				 * Starts the playblack of the next music in playlist.
				 *
				 * @note At least one music playback must be listed in the
				 * playlist.
				 *
				 */
				virtual void startNextMusicPlayback() ;



				/**
				 * Stops any current music playback.
				 *
				 */
				virtual void stopCurrentMusicPlayback() ;



				/**
				 * This method will be automatically called as soon as the music
				 * playback stops.
				 *
				 * @note Never call back-end functions (ex: SDL or SDL_mixer)
				 * from the callback.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual void onMusicPlaybackFinished() ;

/*
 * Takes care of the awful issue of Windows DLL with templates.
 *
 * @see Ceylan's developer guide and README-build-for-windows.txt to understand
 * it, and to be aware of the associated risks.
 *
 */
#pragma warning( push )
#pragma warning( disable: 4251 )

				/// The list of planned music playbacks.
				std::list<MusicPlaybackSetting *> _playList ;

#pragma warning( pop )


				/// The current music playback (if any).
				MusicPlaybackSetting * _currentMusicPlayback ;



				/// The current overall (static) music manager.
				static MusicManager * _CurrentMusicManager ;





			private:


				/**
				 * Callback to catch the end of the music playback.
				 *
				 * @note Replaces the default callback set by the audio module.
				 *
				 * This signature is mandatory.
				 *
				 */
				static void HandleMusicPlaybackFinishedCallback() ;


				/**
				 * Copy constructor made private to ensure that it will be never
				 * called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 *
				 */
				explicit MusicManager( const MusicManager & source ) ;



				/**
				 * Assignment operator made private to ensure that it will be
				 * never called.
				 *
				 * The compiler should complain whenever this undefined operator
				 * is called, implicitly or not.
				 *
				 */
				MusicManager & operator = ( const MusicManager & source ) ;



		} ;


	}


}



#endif // OSDL_MUSIC_MANAGER_H_

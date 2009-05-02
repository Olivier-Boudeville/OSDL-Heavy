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


#ifndef OSDL_CD_ROM_DRIVE_H_
#define OSDL_CD_ROM_DRIVE_H_



// for OSDL::CDROMDriveException, CDROMDriveNumber:
#include "OSDLCDROMDriveHandler.h"   


#include "Ceylan.h"                  // for inheritance


#include <string>
#include <list>
#include <map>



#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

// No need to include SDL header here:
struct SDL_CD ;
struct SDL_CDtrack ;

#endif //  ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 




namespace OSDL
{
	
	
#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

	/*
	 * No way of forward declaring LowLevelCDROM apparently:
	 * we would have liked to specify 'struct LowLevelThing ;' here and in the
	 * implementation file (.cc): 'typedef BackendThing LowLevelThing' but then
	 * the compiler finds they are conflicting declarations.
	 *
	 */
	typedef ::SDL_CD LowLevelCDROM ;
	typedef ::SDL_CDtrack LowLevelCDROMTrack ;
	
#else // OSDL_USES_SDL	

	struct LowLevelCDROM {} ;
	struct LowLevelCDROMTrack {} ;
		
#endif // OSDL_USES_SDL 

	
	
	// Tracks are parts of CD data that can be counted.	
	typedef Ceylan::Count TrackNumber ;
	
	
	/// Describes a number of CD frames.
	typedef Ceylan::Uint32 FrameCount ;
	
	
	/// Describes the different kinds of tracks.
	enum TrackType { audioTrack, dataTrack } ;
	
	
	
	
	/**
	 * Describes a CD track, it is just an object-oriented view of an 
	 * actual track.
	 *
	 */
	class OSDL_DLL CDTrack: public Ceylan::TextDisplayable
	{
	
		
		public:
		
		
		
			/**
			 * Creates a new CD track.
			 *
			 * Does not take ownership of the specified track.
			 *
			 * @throw CDROMDriveException if the operation failed or is not
			 * supported.
			 *
			 */
			CDTrack( const LowLevelCDROMTrack & track ) ;
			
			
			
			/// Virtual destructor, does not deallocate the track data.
			virtual ~CDTrack() throw() ;
			
			
			
			/// Return this track number (0-99).
			virtual TrackNumber getTrackNumber() const ;
			
			
			
			/**
			 * Returns this track type.
			 *
			 * @throw CDROMDriveException if the operation failed.
			 *
			 */
			virtual TrackType getTrackType() const ;
			
			
			
			/// Returns the length, in frames, of this track.
			virtual FrameCount getLength() const ;
			
			
			
			/// Returns the frame offset to the beginning of this track.
			virtual FrameCount getFrameOffset() const ;
		
		

	        /**
	         * Returns an user-friendly description of the state of this drive.
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
		
		
			const LowLevelCDROMTrack * _trackData ;
					
		
						
			
		private:	
		
		

			/**
			 * Copy constructor made private to ensure that it will 
			 * be never called.
			 *
			 * The compiler should complain whenever this undefined 
			 * constructor is called, implicitly or not.
			 * 
			 */			 
			explicit CDTrack( const CDTrack & source ) ;
			
			
			
			/**
			 * Assignment operator made private to ensure that it will 
			 * be never called.
			 *
			 * The compiler should complain whenever this undefined 
			 * operator is called, implicitly or not.
			 * 
			 */			 
			CDTrack & operator = ( const CDTrack & source ) ;
			
			
	} ;
	
	
	
	
	
	/**
	 * A CD-ROM drive allows to control a particular CD-ROM device 
	 * attached to the system.
	 *
	 */
	class OSDL_DLL CDROMDrive: public Ceylan::Object
	{
	
	
		// The CD-ROM drive handler must be able to instanciate CD-ROM drives.
		friend class CDROMDriveHandler ;
		
		
		
		public:
		


			/// Tells how many CD frames make one second.
			static const FrameCount FramesPerSecond ;
			
			
			
			/// Describes the status possibilites of a CD drive.
			enum Status { TrayEmpty, Stopped, Playing, Paused, InError } ; 
			
			
			
			/**
			 * Opens for access this CD-ROM drive, so that the CD that 
			 * must be in it can be directly read afterwards, a bit like
			 * 'mounting' this device.
			 *
			 * @note This has nothing to do with opening the drive tray.
			 *
			 * @throw CDROMDriveException if the drive could not be opened, 
			 * for example if it is busy, or with no disc, or already opened.
			 *
			 */
			virtual void open() ;
		 	
			
			
			/**
			 * Closes this CD-ROM drive, a bit like 'unmounting' this device.
			 *
			 * @note This has nothing to do with the drive tray.
			 *
			 * @throw CDROMDriveException if the drive could not be opened,
			 * for example if it is busy and not opened.
			 *
			 */
			virtual void close() ;
		 	
			
			
			/**
			 * Ejects the CD-ROM which is in the drive.
			 * The drive must be already opened.
			 *
			 * @throw CDROMDriveException if an error occured, 
			 * for example if drive was not open.
			 * 
			 */
			virtual void eject() const ;
			 
			 
			 
			/**
			 * Returns the current status of this drive.
			 *
			 * @throw CDROMDriveException if drive not already open.
			 *
			 * This method cannot be 'const' since status update flag 
			 * has to be updated.
			 *
			 */
			virtual Status getStatus() ;



			/**
			 * Returns the current status of this drive.
			 *
			 * @throw CDROMDriveException if drive not already open.
			 *
			 * @note Cannot update the status flag.
			 *
			 */
			virtual Status getConstStatus() const ;
			
			
			
			/**
			 * Tells whether there is a CD in drive.
			 *
			 * @return true iff there is a CD in drive.
			 *
			 * @throw CDROMDriveException if drive not already open.
			 *
			 * Cannot be 'const' since 'getStatus' neither.
			 *
			 */
			virtual bool isCDInDrive() ;
			
			
			
			/**
			 * Returns the number of tracks in the CD in this drive.
			 *
			 * @throw CDROMDriveException if an error occured, for 
			 * example if the drive is not already opened.
			 *
			 */
			virtual TrackNumber getTracksCount() const ;
			
			
			
			/**
			 * Returns the duration, in frames, of the specified track.
			 *
			 * @param targetTrack the track whose duration is to be known.
			 *
			 * @throw CDROMDriveException if an error occured, for 
			 * example if an invalid track number is given.
			 *
			 */
			virtual FrameCount getTrackDuration( TrackNumber targetTrack ) 
				const ;
			 
			 
			
			/**
			 * Returns the CD track of specified number.
			 *
			 * @param targetTrack the track to return.
			 *
			 * @note The caller must deallocate the returned track when
			 * finished with it.
			 *
			 * @throw CDROMDriveException if the operation failed.
			 *
			 */
			virtual CDTrack & getTrack( TrackNumber targetTrack ) const ;
				
				
				
			/**
			 * Plays the CD-ROM in drive, starting from specified CD 
			 * frame, for specified duration.
			 *
			 * @param startingFrame the frame to start.
			 *
			 * @param durationInFrames the duration, specified in frames.
			 *
			 * @throw CDROMDriveException if an error occured.
			 *
			 */
			virtual void playFrames( FrameCount startingFrame, 
				FrameCount durationInFrames ) ;
				
				
				
			/**
			 * Plays the CD-ROM in drive, starting from specified CD track, 
			 * for specified number of tracks. Frame offsets can be used to
			 * start after the beginning of the starting track and to stop 
			 * after the stopping track. 
			 *
			 * @note Data tracks are ignored. 
			 *
			 * @param startingTrack the track to start with.
			 *
			 * @param numberOfTracks the number of tracks to play from
			 * 'startingTrack'.
			 *
			 * @param startingFrameOffset the frame offset, from the 
			 * beginning of 'startingTrack', at which to start.
			 *
			 * @param stoppingFrameOffset the frame offset, from the 
			 * beginning of the last track (startingTrack+numberOfTracks), 
			 * at which to end playing.
			 *
			 * @throw CDROMDriveException if an error occured.
			 *
			 */
			virtual void playTracks( 
				TrackNumber startingTrack, 
				TrackNumber numberOfTracks,	
				FrameCount startingFrameOffset = 0,
				FrameCount stoppingFrameOffset = 0 ) ;
			
			
			
			/**
			 * Pauses play on this CD-ROM.
			 *
			 * @throw CDROMDriveException if an error occured.
			 *
			 */
			virtual void pause() const ;
			
			
			
			/**
			 * Resumes play on this CD-ROM.
			 *
			 * @throw CDROMDriveException if an error occured.
			 *
			 */
			virtual void resume() const ;
			
			

	        /**
	         * Returns an user-friendly description of the state of this drive.
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




			
			// Static section.
			
			
			
            /**
             * Returns a list of inserted disc media, such as DVD-ROM, HD-DVD,
             * CDRW, and Blu-Ray discs. 
             *
			 * @throw CDROMDriveException if the operation failed.
			 *
             * @note Only filesystems are searched for, thus for example an
             * audio CD will not be returned, unless there is a mounted
             * filesystem track on it.
             *
             */
            static std::list<std::string> GetListOfInsertedMedia() ;
            
			
            
			/**
			 * Returns the number of CD frames corresponding to the 
			 * specified time.
			 * 
			 * @param duration the duration in seconds to convert into 
			 * CD frames.
			 *
			 */
			static FrameCount ConvertTimeToFrameCount( 
				Ceylan::System::Second duration ) ;



			/**
			 * Returns the duration in seconds corresponding to the 
			 * specified number of CD frames.
			 * 
			 * @param duration the duration in seconds to convert into 
			 * CD frames.
			 *
			 * @param The returned number of second is rounded.
			 *
			 */
			static Ceylan::System::Second ConvertFrameCountToTime( 
				FrameCount duration ) ;




		protected:
		
			
			/// Records the number associated with this drive object.
			CDROMDriveNumber _driveNumber ;
	
	
		
		
		private:
	
	
	
			/// Telles whether track information about the CD is available.
			bool _statusUpdated ;
			
				
				
			/**
			 * Gathers some informations about this drive and its status.
			 *
			 * Tells too whether the drive is open for access (if non-null)
			 * or not (if null).
			 *
			 */
			LowLevelCDROM * _driveStatus ;
			
			
			
			/**
			 * A CD-ROM drive object (CDROMDrive) will be associated 
			 * uniquely to a CD-ROM device.
			 *
			 * This constructor is private so that the factory gets total
			 * control onto drive instance creations.
			 *
			 * @param driveNumber the drive number to take control of.
			 *
			 * @throw CDROMDriveException if specified drive number is 
			 * not in licit range.
			 *
			 */
			explicit CDROMDrive( CDROMDriveNumber driveNumber ) ;
			
			
			
			/// Virtual destructor.
			virtual ~CDROMDrive() throw() ;
			
			

			/**
			 * Copy constructor made private to ensure that it will be 
			 * never called.
			 * The compiler should complain whenever this undefined 
			 * constructor is called, implicitly or not.
			 * 
			 */			 
			explicit CDROMDrive( const CDROMDrive & source ) ;
			
			
			
			/**
			 * Assignment operator made private to ensure that it will 
			 * be never called.
			 * The compiler should complain whenever this undefined 
			 * operator is called, implicitly or not.
			 * 
			 */			 
			CDROMDrive & operator = ( const CDROMDrive & source ) ;
			
	
	
	} ;


}



#endif // OSDL_CD_ROM_DRIVE_H_


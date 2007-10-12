#include "OSDLCDROMDrive.h"


#include "OSDLUtils.h"   // for getBackendLastError


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS



#include <list>  


using std::list ;
using std::string ;


using namespace OSDL ;


#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

#include "SDL.h"					 // for SDL_CD, SDL_CDtrack, etc.

#endif // OSDL_USES_SDL



/** 
 * Dummy values will be returned by non-static methods in case there is no
 * SDL support available.
 *
 * It is not a problem as these methods cannot be called: constructors always
 * throw exceptions in that case, thus no instance can be available for these
 * method calls.
 *
 */ 




// CD track section.


CDTrack::CDTrack( const LowLevelCDROMTrack & track ) 
		throw( CDROMDriveException ):
	_trackData( & track )
{

#if ! OSDL_USES_SDL

	throw CDROMDriveException( "CDTrack constructor failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

}



CDTrack::~CDTrack() throw() 
{

	// _trackData not owned.
	
}



TrackNumber CDTrack::getTrackNumber() const throw()
{

#if OSDL_USES_SDL

	return _trackData->id ; 
	
#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL	
	
}



TrackType CDTrack::getTrackType() const throw( CDROMDriveException )
{

#if OSDL_USES_SDL

	switch( _trackData->type )
	{
	
		case SDL_AUDIO_TRACK:
			return audioTrack ;
			break ;
			
		case SDL_DATA_TRACK:
			return dataTrack ;
			break ;
		
		default:
			throw CDROMDriveException( "CDTrack::getTrackType failed: "
				"unexpected type (abnormal)." ) ;
			break ;
	
	}

#else // OSDL_USES_SDL

	throw CDROMDriveException( "CDTrack::getTrackType failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL	
				
}



FrameCount CDTrack::getLength() const throw()
{

#if OSDL_USES_SDL

	return _trackData->length ;
	
#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL	
	 
}



FrameCount CDTrack::getFrameOffset() const throw()
{

#if OSDL_USES_SDL

	return _trackData->offset ;
	 
#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL	
	
}



const string CDTrack::toString( Ceylan::VerbosityLevels level ) const throw()
{
	
	
#if OSDL_USES_SDL

	string res = "Track #" + Ceylan::toString( getTrackNumber() ) 
		+ ": this is " ;
	
	switch( getTrackType() )
	{
		
		case audioTrack:
			res += "an audio track, its length is " 
				+ Ceylan::toString( getLength() ) 
				+ " frames (about " 
				+ Ceylan::toString( 
					CDROMDrive::ConvertFrameCountToTime( getLength() ) ) 
				+ " seconds)" ;
			break ;
			
		case dataTrack:
			res += "a data track, its length is " 
				+ Ceylan::toString( getLength() ) 
				+ " frames" ;
			break ;
			
		default:
			res += "unexpected track type" ;
			break ;	
				
	}

	res += ". Current frame offset is " + Ceylan::toString( getFrameOffset() ) ;
	
	return res ;

#else // OSDL_USES_SDL
	
	return "" ;

#endif // OSDL_USES_SDL	
	
}






// CD-ROM drive section.		
	
	
#if OSDL_USES_SDL

const FrameCount CDROMDrive::FramesPerSecond = CD_FPS ;

#else // OSDL_USES_SDL

const FrameCount CDROMDrive::FramesPerSecond = 75 ;

#endif // OSDL_USES_SDL
	
	
				
CDROMDrive::CDROMDrive( CDROMDriveNumber driveNumber ) 
		throw( CDROMDriveException ):
	Object(),
	_driveNumber( driveNumber ),
	_statusUpdated( false ),
	_driveStatus( 0 )
{

#if ! OSDL_USES_SDL

	throw CDROMDriveException( "CDROMDrive constructor failed: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
		
}



CDROMDrive::~CDROMDrive() throw()
{
	
	if ( _driveStatus != 0 )
		close() ;
		
}



void CDROMDrive::open() throw( CDROMDriveException )
{

#if OSDL_USES_SDL

	if ( _driveStatus != 0 )
		throw CDROMDriveException( 
			"CDROMDrive::open: drive already opened." ) ;
		
	_driveStatus = SDL_CDOpen( _driveNumber ) ;
	
	if ( _driveStatus == 0 )
		throw CDROMDriveException( "CDROMDrive::open: open failed: "
			+ Utils::getBackendLastError() ) ;

#endif // OSDL_USES_SDL
	
}



void CDROMDrive::close() throw( CDROMDriveException )
{

#if OSDL_USES_SDL

	if ( _driveStatus == 0 )
		throw CDROMDriveException( 
			"CDROMDrive::close: drive was not already opened." ) ;
		
	SDL_CDClose( _driveStatus ) ;
	
	// Owned ?::free( _driveStatus ) ?
	_driveStatus = 0 ;
	
#endif // OSDL_USES_SDL
		
}



void CDROMDrive::eject() const throw( CDROMDriveException )
{

#if OSDL_USES_SDL
	if ( _driveStatus == 0 )
		throw CDROMDriveException( 
			"CDROMDrive::eject: drive was not already opened." ) ;

	if ( SDL_CDEject( _driveStatus ) != 0 )
		throw CDROMDriveException( 
			"CDROMDrive::eject: " + Utils::getBackendLastError() ) ;
		
#endif // OSDL_USES_SDL

}



CDROMDrive::Status CDROMDrive::getStatus() throw( CDROMDriveException )
{
			
	Status status = getConstStatus() ;
	
	_statusUpdated = true ;
	
	return status ;
	
}
		
		

CDROMDrive::Status CDROMDrive::getConstStatus() const 
	throw( CDROMDriveException )
{

#if OSDL_USES_SDL

	if ( _driveStatus == 0 )
		throw CDROMDriveException( 
			"CDROMDrive::getConstStatus: drive was not already opened." ) ;

		
	switch( SDL_CDStatus( _driveStatus ) )
	{
	
		case CD_TRAYEMPTY:
			return TrayEmpty ;
			break ;
			
		case CD_STOPPED:
			return Stopped ;
			break ;
						
		case CD_PLAYING:
			return Playing ;
			break ;
			
		case CD_PAUSED:
			return Paused ;
			break ;
			
		case CD_ERROR:
			return InError ;
			break ;
			
		default:
			throw CDROMDriveException( 
				"CDROMDrive::getConstStatus: unknown status read" ) ;
			break ;
				
	}
	
#else // OSDL_USES_SDL

	return InError ;
		
#endif // OSDL_USES_SDL

}



bool CDROMDrive::isCDInDrive() throw( CDROMDriveException )
{
	
	CDROMDrive::Status status =	getStatus() ;

	return ( ( status != TrayEmpty) && ( status != InError ) ) ;
	
}



TrackNumber CDROMDrive::getTracksCount() const throw( CDROMDriveException )
{

#if OSDL_USES_SDL

	if ( _driveStatus == 0 )
		throw CDROMDriveException( 
			"CDROMDrive::getTracksCount: drive was not already opened." ) ;

	return _driveStatus->numtracks ;
	
#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL

}



FrameCount CDROMDrive::getTrackDuration( TrackNumber targetTrack ) 
	const throw( CDROMDriveException )
{
	
#if OSDL_USES_SDL

	// Checks as well that the drive is opened:
	if ( targetTrack >= getTracksCount() )
		throw CDROMDriveException( "CDROMDrive::getTrackDuration: "
			"specified track (" + Ceylan::toString( targetTrack ) 
			+ ") out of range." ) ;
		
	return _driveStatus->track[targetTrack].length ; 
			
#else // OSDL_USES_SDL

	return 0 ;
	
#endif // OSDL_USES_SDL
	
}


	
CDTrack & CDROMDrive::getTrack( TrackNumber targetTrack ) const 
	throw( CDROMDriveException )
{

#if OSDL_USES_SDL

	// Checks as well that the drive is opened:
	if ( targetTrack >= getTracksCount() )
		throw CDROMDriveException( "CDROMDrive::getTrack: "
			"specified track (" + Ceylan::toString( targetTrack )
			+ ") out of range." ) ;

	return * new CDTrack( _driveStatus->track[ targetTrack ] ) ;
	
#else // OSDL_USES_SDL

	throw CDROMDriveException( "CDROMDrive::getTrack: "
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
	
}

	
					
void CDROMDrive::playFrames( FrameCount startingFrame, 
	FrameCount durationInFrames ) throw( CDROMDriveException )		
{

#if OSDL_USES_SDL

	if ( _driveStatus == 0 )
		throw CDROMDriveException( 
			"CDROMDrive::playFrames: drive was not already opened." ) ;

	if ( SDL_CDPlay( _driveStatus, startingFrame, durationInFrames) != 0 )
		throw CDROMDriveException( "CDROMDrive::playFrames failed: " 
			+ Utils::getBackendLastError() ) ;

#endif // OSDL_USES_SDL
	
}



void CDROMDrive::playTracks( TrackNumber startingTrack, 
	TrackNumber numberOfTracks, FrameCount startingFrameOffset, 
	FrameCount stoppingFrameOffset ) throw( CDROMDriveException )	
{

#if OSDL_USES_SDL

	// Updated status needed to play tracks (check that drive is opened too):
	if ( ! _statusUpdated )
		getStatus() ;
	
	if ( SDL_CDPlayTracks( _driveStatus, startingTrack, startingFrameOffset,
		numberOfTracks,	stoppingFrameOffset ) != 0 )
	{		
		throw CDROMDriveException( "CDROMDrive::playTracks failed: " 
			+ Utils::getBackendLastError() ) ;
	}		

#endif // OSDL_USES_SDL

}



void CDROMDrive::pause() const throw( CDROMDriveException )
{
	
#if OSDL_USES_SDL

	if ( _driveStatus == 0 )
		throw CDROMDriveException( 
			"CDROMDrive::pause: drive was not already opened." ) ;
		
	if ( SDL_CDPause( _driveStatus ) != 0 )
		throw CDROMDriveException( "CDROMDrive::pause failed: "
			+ Utils::getBackendLastError() ) ;

#endif // OSDL_USES_SDL
		
}



void CDROMDrive::resume() const throw( CDROMDriveException )
{
	
#if OSDL_USES_SDL

	if ( _driveStatus == 0 )
		throw CDROMDriveException( 
			"CDROMDrive::resume: drive was not already opened." ) ;
		
	if ( SDL_CDResume( _driveStatus ) != 0 )
		throw CDROMDriveException( "CDROMDrive::resume failed: "
			+ Utils::getBackendLastError() ) ;

#endif // OSDL_USES_SDL
		
}


		
const string CDROMDrive::toString( Ceylan::VerbosityLevels level ) const throw()
{
	
#if OSDL_USES_SDL

	string res ;

	if ( _driveNumber == 0 )
		res = "Default CD-ROM drive (#0)" ;
	else
		res = "CD-ROM drive #" + Ceylan::toString( _driveNumber ) ;
	
	res += ". System-dependent name for this drive is '" 
		+ string( SDL_CDName( _driveNumber ) ) ;
	
	res += "'. It is currently " ;
	
	if ( _driveStatus == 0 ) 
		res += "not " ;
	res += "opened for access" ;

	if ( level == Ceylan::low )	
		return res ;
		
		
	if ( _driveStatus != 0 )
	{
		
		res + ". " ;
		
		switch( getConstStatus() )
		{
	
			case TrayEmpty:
				res += "Tray is empty" ;
				break ;
			
			case Stopped:
				res += "Drive is stopped" ;
				break ;
		
			case Playing:
				res += "Drive is playing" ;
				break ;
		
			case Paused:
				res += "Drive is paused" ;
				break ;
			
			case InError:
				res += "Drive is in error" ;
				break ;
			
			default:	
				res += "Unknown drive status (abnormal)" ;
				break ;
	
		}		
			
		TrackNumber count = getTracksCount() ;
		
		res += ". There is/are " + Ceylan::toString( count ) + " track(s): " ;
		
		list<string> tracks ;
		
		for ( TrackNumber i = 0; i < count; i++ )
		{
			
			CDTrack currentTrack( _driveStatus->track[ i ] ) ;
			
			tracks.push_back( "Track #" + Ceylan::toString( i ) + ": " 
				+ currentTrack.toString( level ) ) ;
				
		}
		
		res += Ceylan::formatStringList( tracks ) ;		
	} 
		
	return res ;	
	
#else // OSDL_USES_SDL

	return "" ;
	
#endif // OSDL_USES_SDL
		
}
			   				


FrameCount CDROMDrive::ConvertTimeToFrameCount( 
	Ceylan::System::Second duration ) throw()
{

	return duration * FramesPerSecond ;
	
}


Ceylan::System::Second CDROMDrive::ConvertFrameCountToTime( 
	FrameCount duration ) throw()
{

	return duration / FramesPerSecond ;
	
}


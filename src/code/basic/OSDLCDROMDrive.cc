#include "OSDLCDROMDrive.h"


#include "OSDLUtils.h"   // for getBackendLastError

#include "SDL.h"         // for CD-ROM primitives

#include <list>  

using std::list ;
using std::string ;


using namespace OSDL ;


// CD track section.

CDTrack::CDTrack( const SDL_CDtrack & track ) throw() :
	_trackData( & track )
{

}


CDTrack::~CDTrack() throw() 
{
	// _trackData not owned.
}


TrackNumber CDTrack::getTrackNumber() const throw()
{
	return _trackData->id ; 
}


TrackType CDTrack::getTrackType() const throw()
{

	switch( _trackData->type )
	{
	
		case SDL_AUDIO_TRACK:
			return audioTrack ;
			break ;
			
		case SDL_DATA_TRACK:
			return dataTrack ;
			break ;
		
		default:
			Ceylan::emergencyShutdown( "CDTrack::getTrackType : unexpected type." ) ;
			// Do not care, just to avoid a warning :
			return dataTrack ;
			break ;
	
	}
				
}


FrameCount CDTrack::getLength() const throw()
{
	return _trackData->length ; 
}


FrameCount CDTrack::getFrameOffset() const throw()
{
	return _trackData->offset ; 
}


const string CDTrack::toString( Ceylan::VerbosityLevels level ) const throw()
{
	
	string res = "Track #" + Ceylan::toString( getTrackNumber() ) + " : this is " ;
	
	switch( getTrackType() )
	{
		
		case audioTrack:
			res += "an audio track, its length is " + Ceylan::toString( getLength() ) 
				+ " frames (about " 
				+ Ceylan::toString( CDROMDrive::ConvertFrameCountToTime( getLength() ) ) 
				+ " seconds)" ;
			break ;
			
		case dataTrack:
			res += "a data track, its length is " + Ceylan::toString( getLength() ) 
				+ " frames" ;
			break ;
			
		default:
			res += "unexpected track type" ;
			break ;	
				
	}

	res += ". Current frame offset is " +  Ceylan::toString( getFrameOffset() ) ;
	
	return res ;
	
}





// CD-ROM drive section.		
	
	
const FrameCount CDROMDrive::FramesPerSecond = CD_FPS ;

				
CDROMDrive::CDROMDrive( CDROMDriveNumber driveNumber ) throw( CDROMDriveException ) :
	Object(),
	_driveNumber( driveNumber ),
	_statusUpdated( false ),
	_driveStatus( 0 )
{
	
}


CDROMDrive::~CDROMDrive() throw()
{
	
	if ( _driveStatus != 0 )
		close() ;
		
}


void CDROMDrive::open() throw( CDROMDriveException )
{

	if ( _driveStatus != 0 )
		throw CDROMDriveException( "CDROMDrive::open : drive already opened." ) ;
		
	_driveStatus = SDL_CDOpen( _driveNumber ) ;
	
	if ( _driveStatus == 0 )
		throw CDROMDriveException( "CDROMDrive::open : open failed : "
			+ Utils::getBackendLastError() ) ;
	
}


void CDROMDrive::close() throw( CDROMDriveException )
{

	if ( _driveStatus == 0 )
		throw CDROMDriveException( "CDROMDrive::close : drive was not already opened." ) ;
		
	SDL_CDClose( _driveStatus ) ;
	
	// Owned ? ::free( _driveStatus ) ?
	_driveStatus = 0 ;
		
}


void CDROMDrive::eject() const throw( CDROMDriveException )
{

	if ( _driveStatus == 0 )
		throw CDROMDriveException( "CDROMDrive::eject : drive was not already opened." ) ;

	if ( SDL_CDEject( _driveStatus ) != 0 )
		throw CDROMDriveException( "CDROMDrive::eject : " + Utils::getBackendLastError() ) ;
		
}


CDROMDrive::Status CDROMDrive::getStatus() throw( CDROMDriveException )
{
			
	Status status = getConstStatus() ;
	
	_statusUpdated = true ;
	
	return status ;
	
}
		

CDROMDrive::Status CDROMDrive::getConstStatus() const throw( CDROMDriveException )
{

	if ( _driveStatus == 0 )
		throw CDROMDriveException( "CDROMDrive::getStatus : drive was not already opened." ) ;

		
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
			Ceylan::emergencyShutdown( "CDROMDrive::getStatus : unknown status read" ) ;
			// Do not care, just to avoid a warning :
			return InError ;			
			break ;
				
	}
	
}



bool CDROMDrive::isCDInDrive() throw( CDROMDriveException )
{
	
	CDROMDrive::Status status =	getStatus() ;

	return ( ( status != TrayEmpty) && ( status != InError ) ) ;
	
}

TrackNumber CDROMDrive::getTracksCount() const throw( CDROMDriveException )
{

	if ( _driveStatus == 0 )
		throw CDROMDriveException( "CDROMDrive::getTracksCount : drive was not already opened." ) ;

	return _driveStatus->numtracks ;
	
}


FrameCount CDROMDrive::getTrackDuration( TrackNumber targetTrack ) 
	const throw( CDROMDriveException )
{
	
	// Checks as well that the drive is opened :
	if ( targetTrack >= getTracksCount() )
		throw CDROMDriveException( "CDROMDrive::getTrackDuration : "
			"specified track (" + Ceylan::toString( targetTrack ) + ") out of range." ) ;
		
	return _driveStatus->track[targetTrack].length ; 		
	
}

	
CDTrack & CDROMDrive::getTrack( TrackNumber targetTrack ) const throw( CDROMDriveException )
{

	// Checks as well that the drive is opened :
	if ( targetTrack >= getTracksCount() )
		throw CDROMDriveException( "CDROMDrive::getTrack : "
			"specified track (" + Ceylan::toString( targetTrack ) + ") out of range." ) ;

	return * new CDTrack( _driveStatus->track[ targetTrack ] ) ;
	
}

					
void CDROMDrive::playFrames( FrameCount startingFrame, FrameCount durationInFrames ) 
	throw( CDROMDriveException )		
{

	if ( _driveStatus == 0 )
		throw CDROMDriveException( "CDROMDrive::playFrames : drive was not already opened." ) ;

	if ( SDL_CDPlay( _driveStatus, startingFrame, durationInFrames) != 0 )
		throw CDROMDriveException( "CDROMDrive::playFrames failed : " 
			+ Utils::getBackendLastError() ) ;
	
}


void CDROMDrive::playTracks( TrackNumber startingTrack, TrackNumber numberOfTracks,
	FrameCount startingFrameOffset, FrameCount stoppingFrameOffset ) throw( CDROMDriveException )	
{

	// Updated status needed to play tracks (check that drive is opened too) :
	if ( ! _statusUpdated )
		getStatus() ;
	
	if ( SDL_CDPlayTracks( _driveStatus, startingTrack, startingFrameOffset, numberOfTracks,
		 	stoppingFrameOffset ) != 0 )
	{		
		throw CDROMDriveException( "CDROMDrive::playFrames failed : " 
			+ Utils::getBackendLastError() ) ;
	}		

}


void CDROMDrive::pause() const throw( CDROMDriveException )
{
	
	if ( _driveStatus == 0 )
		throw CDROMDriveException( "CDROMDrive::pause : drive was not already opened." ) ;
		
	if ( SDL_CDPause( _driveStatus ) != 0 )
		throw CDROMDriveException( "CDROMDrive::pause failed : "
			+ Utils::getBackendLastError() ) ;
		
}


void CDROMDrive::resume() const throw( CDROMDriveException )
{
	
	if ( _driveStatus == 0 )
		throw CDROMDriveException( "CDROMDrive::resume : drive was not already opened." ) ;
		
	if ( SDL_CDResume( _driveStatus ) != 0 )
		throw CDROMDriveException( "CDROMDrive::resume failed : "
			+ Utils::getBackendLastError() ) ;
		
}

		
const string CDROMDrive::toString( Ceylan::VerbosityLevels level ) const throw()
{
	
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
		
		res += ". There is/are " + Ceylan::toString( count ) + " track(s) : " ;
		
		list<string> tracks ;
		
		for ( TrackNumber i = 0; i < count; i++ )
		{
			
			CDTrack currentTrack( _driveStatus->track[ i ] ) ;
			
			tracks.push_back( "Track #" + Ceylan::toString( i ) + " : " 
				+ currentTrack.toString( level ) ) ;
				
		}
		
		res += 	Ceylan::formatStringList( tracks ) ;		
	} 
		
	return res ;	
		
}
			   				


FrameCount CDROMDrive::ConvertTimeToFrameCount( Ceylan::System::Second duration ) throw()
{
	return duration * FramesPerSecond ;
}


Ceylan::System::Second CDROMDrive::ConvertFrameCountToTime( FrameCount duration ) throw()
{
	return duration / FramesPerSecond ;
}

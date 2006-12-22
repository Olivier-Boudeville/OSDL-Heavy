#include "OSDLCDROMDriveHandler.h"

#include "OSDLCDROMDrive.h"  // for CDROMDrive

#include "OSDLBasic.h"       // for CommonModule
#include "OSDLUtils.h"       // for getBackendLastError


#include "SDL.h"             // for CD-ROM primitives

#include <list>  

using std::list ;
using std::string ;
using std::map ;


using namespace OSDL ;



CDROMDriveException::CDROMDriveException( const std::string & message ) throw() :
	OSDL::Exception( message )
{

}


CDROMDriveException::~CDROMDriveException() throw()
{

}




CDROMDriveHandler::CDROMDriveHandler() throw( CDROMDriveException ) :
	 Object(),
	 _drives()
{
	
	send( "Initializing CD-ROM subsystem" ) ;
		
	if ( SDL_InitSubSystem( CommonModule::UseCDROM ) != CommonModule::BackendSuccess ) 
		throw CDROMDriveException( "CDROMDriveHandler constructor : "
				"unable to initialize CD-ROM subsystem : " + Utils::getBackendLastError() ) ;
				
	send( "CD-ROM subsystem initialized" ) ;				

}


CDROMDriveHandler::~CDROMDriveHandler() throw()
{

	send( "Stopping CD-ROM subsystem." ) ;

	for ( map<CDROMDriveNumber, CDROMDrive *>::const_iterator it = _drives.begin() ;
		it != _drives.end(); it++ )
	{
		delete (*it).second ;
	}
	
	// Useless but deemed safer :
	_drives.clear() ;
		
	SDL_QuitSubSystem( CommonModule::UseCDROM ) ;

	send( "CD-ROM subsystem stopped." ) ;

}
			


CDROMDriveNumber CDROMDriveHandler::GetAvailableCDROMDrivesCount() throw()
{
	return SDL_CDNumDrives() ;
}


CDROMDrive & CDROMDriveHandler::getCDROMDrive( CDROMDriveNumber number ) 
	throw( CDROMDriveException )
{
	
	map<CDROMDriveNumber, CDROMDrive *>::const_iterator it = _drives.find( number ) ;
	
	if ( it != _drives.end() )
	{
		
		// Drive found :
		return * (*it).second ;
	
	}
		
	// Else : drive not already created, let's try to create it :
	CDROMDrive * newDrive = new CDROMDrive( number ) ;
	
	// Here the creation succeeded (otherwise an exception is propagated as expected) :
	_drives[ number ] = newDrive ;
	
	return *newDrive ;
	
}

				
const std::string CDROMDriveHandler::toString( Ceylan::VerbosityLevels level ) const throw()
{
		
	CDROMDriveNumber driveNumber = GetAvailableCDROMDrivesCount() ;
	
	if ( driveNumber == 0 )
		return "No available CD-ROM drive found" ;
	
	if ( _drives.empty() )
		return "None of the " + Ceylan::toString( driveNumber ) 
			+ " available drive(s) is currently opened." ;
	
	// At least a drive is available, at least one is opened :
			
	list<string> descriptions ;
	
	for ( map<CDROMDriveNumber, CDROMDrive *>::const_iterator it = _drives.begin() ;
		it != _drives.end(); it++ )
	{
		descriptions.push_back( (*it).second->toString() ) ;
	}
	
	return "Out of " + Ceylan::toString( driveNumber ) + " available CD-ROM drive(s), " 
		+ Ceylan::toString( _drives.size() ) + " is/are opened : " 
		+ Ceylan::formatStringList( descriptions ) ;	

}				
			

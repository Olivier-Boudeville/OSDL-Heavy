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


#include "OSDLCDROMDriveHandler.h"

#include "OSDLCDROMDrive.h"  // for CDROMDrive

#include "OSDLBasic.h"       // for CommonModule
#include "OSDLUtils.h"       // for getBackendLastError



#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL
#include "SDL.h"             // for CD-ROM primitives
#endif // OSDL_USES_SDL


#include <list>



using std::list ;
using std::string ;
using std::map ;


using namespace OSDL ;



CDROMDriveException::CDROMDriveException( const std::string & message ) :
	OSDL::Exception( message )
{

}



CDROMDriveException::~CDROMDriveException() throw()
{

}




CDROMDriveHandler::CDROMDriveHandler() :
	 Object(),
	 _drives()
{

#if OSDL_USES_SDL

	send( "Initializing CD-ROM subsystem" ) ;

	if ( SDL_InitSubSystem( CommonModule::UseCDROM )
			!= CommonModule::BackendSuccess )
		throw CDROMDriveException( "CDROMDriveHandler constructor: "
				"unable to initialize CD-ROM subsystem: "
				+ Utils::getBackendLastError() ) ;

	send( "CD-ROM subsystem initialized" ) ;

#else // OSDL_USES_SDL

	throw CDROMDriveException( "CDROMDriveHandler constructor: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



CDROMDriveHandler::~CDROMDriveHandler() throw()
{

#if OSDL_USES_SDL

	send( "Stopping CD-ROM subsystem." ) ;

	for ( map<CDROMDriveNumber, CDROMDrive *>::const_iterator it
		= _drives.begin() ; it != _drives.end(); it++ )
	{
		delete (*it).second ;
	}

	// Useless but deemed safer:
	_drives.clear() ;

	SDL_QuitSubSystem( CommonModule::UseCDROM ) ;

	send( "CD-ROM subsystem stopped." ) ;

#endif // OSDL_USES_SDL

}



CDROMDriveNumber CDROMDriveHandler::GetAvailableCDROMDrivesCount()
{

#if OSDL_USES_SDL

	return SDL_CDNumDrives() ;

#else // OSDL_USES_SDL

	return 0 ;

#endif // OSDL_USES_SDL

}



CDROMDrive & CDROMDriveHandler::getCDROMDrive( CDROMDriveNumber number )
{

#if OSDL_USES_SDL

	map<CDROMDriveNumber, CDROMDrive *>::const_iterator it
	  = _drives.find( number ) ;

	if ( it != _drives.end() )
	{

		// Drive found:
		return * (*it).second ;

	}

	// Else: drive not already created, let's try to create it:
	CDROMDrive * newDrive = new CDROMDrive( number ) ;

	/*
	 * Here the creation succeeded (otherwise an exception is propagated as
	 * expected):
	 *
	 */
	_drives[ number ] = newDrive ;

	return *newDrive ;

#else // OSDL_USES_SDL

	throw CDROMDriveException( " CDROMDriveHandler::getCDROMDrive: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



const std::string CDROMDriveHandler::toString( Ceylan::VerbosityLevels level )
	const
{

#if OSDL_USES_SDL

	CDROMDriveNumber driveNumber = GetAvailableCDROMDrivesCount() ;

	if ( driveNumber == 0 )
		return "No available CD-ROM drive found" ;

	if ( _drives.empty() )
		return "None of the " + Ceylan::toString( driveNumber )
			+ " available drive(s) is currently opened." ;

	// At least a drive is available, at least one is opened:

	list<string> descriptions ;

	for ( map<CDROMDriveNumber, CDROMDrive *>::const_iterator it
		= _drives.begin() ; it != _drives.end(); it++ )
	{
		descriptions.push_back( (*it).second->toString() ) ;
	}

	return "Out of " + Ceylan::toString( driveNumber )
		+ " available CD-ROM drive(s), "
		+ Ceylan::toString( static_cast<Ceylan::Uint32>( _drives.size() ) )
		+ " is/are opened: " + Ceylan::formatStringList( descriptions ) ;

#else // OSDL_USES_SDL

	return "" ;

#endif // OSDL_USES_SDL

}

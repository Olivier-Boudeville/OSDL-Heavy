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



#include "OSDLEmbeddedFile.h"

#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for CEYLAN_ARCH_NINTENDO_DS and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_PHYSICSFS
#include "physfs.h"                  // for PHYSFS_close and al
#endif // OSDL_USES_PHYSICSFS



#include "OSDLEmbeddedFileSystemManager.h" // for the FileSystemManager


#include "Ceylan.h"                  // for all Ceylan services


#include <cstring>                   // for ::memcpy
//#include <cstdint>                 // for uintptr_t



/*
 * Implementation notes.
 *
 * @note In a non-static method, no static method should be used, as the former
 * is expected to use the embedded filesystem manager, whereas the latter shall
 * use the default filesystem manager, which may or may not be the embedded
 * one.
 *
 * Regarding cyphering, only the minimal transformation is done currently: only
 * a one-byte XOR pattern and a numerical offset is applied when reading and
 * writing.
 * Another pass could consist on hashing the filename, seeding a random
 * generator with this hash, and XOR'ing each byte of the file according to
 * the corresponding random series.
 *
 */



using std::string ;

using namespace Ceylan::System ;
using namespace Ceylan::Log ;

using namespace OSDL ;




EmbeddedFileException::EmbeddedFileException( const string & reason ) :
  FileException( reason )
{

}



EmbeddedFileException::~EmbeddedFileException() throw()
{

}





// EmbeddedFile implementation.



EmbeddedFile::~EmbeddedFile() throw()
{

  // Very useful to check that a datastream is freed indeed:
  //LogPlug::trace( "Destructor called for " + _name ) ;

  if ( isOpen() )
  {

	try
	{
	  close() ;
	}
	catch( const Stream::CloseException & e )
	{
	  LogPlug::error( "EmbeddedFile destructor: close failed: "
		+ e.toString() ) ;
	}

  }

}





// Constructors are in protected section.



// Implementation of instance methods inherited from File.



bool EmbeddedFile::isOpen() const
{

#if OSDL_USES_PHYSICSFS

  return ( _physfsHandle != 0 ) ;

#else // OSDL_USES_PHYSICSFS

  return false ;

#endif // OSDL_USES_PHYSICSFS

}



bool EmbeddedFile::close()
{

  if ( ! isOpen() )
  {

	LogPlug::warning( "EmbeddedFile::close: file '" +  _name
	  + "' does not seem to have been already opened." ) ;

	return false ;

  }
  else
  {

	// Let's close it.

#if OSDL_USES_PHYSICSFS

	if ( PHYSFS_close( _physfsHandle ) == 0 )
	  throw Stream::CloseException( "EmbeddedFile::close failed: "
		+ EmbeddedFileSystemManager::GetBackendLastError() ) ;

	_physfsHandle = 0 ;

	return true ;

#else // OSDL_USES_PHYSICSFS

	throw Stream::CloseException( "EmbeddedFile::close failed: "
	  "no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

  }

}



void EmbeddedFile::saveAs( const string & newName )
{

#if OSDL_USES_PHYSICSFS

  // Using the helper factory directly to access to the file descriptor:
  EmbeddedFile & f = EmbeddedFile::Create( newName ) ;
  serialize( *f._physfsHandle ) ;
  delete &f ;

#else // OSDL_USES_PHYSICSFS

  throw FileException(
	"EmbeddedFile::saveAs: not implemented on this platform." ) ;

#endif // OSDL_USES_PHYSICSFS

}




// Locking section.



/**
 * No locking offered by PhysicsFS.
 *
 */



void EmbeddedFile::lockForReading() const
{

  throw FileReadLockingFailed( "EmbeddedFile::lockForReading: "
	"lock feature not available" ) ;

}



void EmbeddedFile::unlockForReading() const
{

  throw FileReadUnlockingFailed(
	"EmbeddedFile::unlockForReading: lock feature not available" ) ;

}



void EmbeddedFile::lockForWriting() const
{

  throw FileWriteLockingFailed( "EmbeddedFile::lockForWriting: "
	"lock feature not available" ) ;

}



void EmbeddedFile::unlockForWriting() const
{

  throw FileWriteUnlockingFailed( "EmbeddedFile::unlockForWriting: "
	"lock feature not available" ) ;

}



bool EmbeddedFile::isLocked() const
{

  return false ;

}



time_t EmbeddedFile::getLastChangeTime() const
{

#if OSDL_USES_PHYSICSFS

  try
  {

	return getCorrespondingFileSystemManager().getEntryChangeTime( _name ) ;

  }
  catch( const Ceylan::Exception & e )
  {
	throw FileLastChangeTimeRequestFailed(
	  "EmbeddedFile::getLastChangeTime failed: "
	  + e.toString() ) ;
  }

#else // OSDL_USES_PHYSICSFS

  throw FileLastChangeTimeRequestFailed(
	"EmbeddedFile::getLastChangeTime failed: "
	"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}



Size EmbeddedFile::read( Ceylan::Byte * buffer, Size maxLength )
{

#if OSDL_USES_PHYSICSFS

  PHYSFS_sint64 objectCount = PHYSFS_read( _physfsHandle,
	static_cast<void *>( buffer ), /* object size */ 1, maxLength ) ;

  if ( objectCount == -1 )
  {
	throw InputStream::ReadFailedException(
	  "EmbeddedFile::read failed: "
	  + EmbeddedFileSystemManager::GetBackendLastError() ) ;
  }

  // else, objectCount expected to be non-negative:

  Size unsignedObjectCount = static_cast<Size>( objectCount ) ;

#if OSDL_DEBUG

  /*
   * This checking is expected to be useless, and would compare a signed
   * with an unsigned:
   *
   if ( unsignedObjectCount != objectCount )
   throw InputStream::ReadFailedException(
   "EmbeddedFile::read failed for '" + _name
   + "': read size too high for return variable." ) ;
  */

#endif // OSDL_DEBUG

  if ( unsignedObjectCount < maxLength )
  {

	// Not EOF? Must be an error:
	if ( ! ::PHYSFS_eof( _physfsHandle ) )
	  throw InputStream::ReadFailedException(
		"EmbeddedFile::read failed: "
		+ EmbeddedFileSystemManager::GetBackendLastError() ) ;
	/*

	  LogPlug::warning( "EmbeddedFile::read: only read "
	  + Ceylan::toString( static_cast<Size>( objectCount ) )
	  + " bytes, instead of requested "
	  + static_cast<Size>( static_cast<Size>( maxLength ) ) + " bytes: "
	  + EmbeddedFileSystemManager::GetBackendLastError()
	  + string( ", end of file " )
	  + ( ( PHYSFS_eof(_physfsHandle) == 0 ) ? "not ":"" )
	  + string( "reached." ) ) ;

	*/
  }


  // No exception excepted to be raised:
  if ( _cypher )
	DecypherBuffer( buffer, objectCount ) ;

  return unsignedObjectCount ;

#else // OSDL_USES_PHYSICSFS

  throw InputStream::ReadFailedException(
	"EmbeddedFile::read failed: "
	"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}



Size EmbeddedFile::write( const string & message )
{

  return write( message.c_str(), message.size() );

}



Size EmbeddedFile::write( const Ceylan::Byte * buffer, Size maxLength )
{

#if OSDL_USES_PHYSICSFS

  const Ceylan::Byte * actualBuffer ;

  if ( _cypher )
  {

	// Input buffer is read-only, let's copy it before modifying it:

	Ceylan::Byte * newBuffer = new Ceylan::Byte[maxLength] ;

	::memcpy( newBuffer, buffer, maxLength ) ;

	CypherBuffer( newBuffer, maxLength ) ;

	actualBuffer = newBuffer ;

  }
  else
  {

	actualBuffer = buffer ;

  }


  PHYSFS_sint64 objectCount = PHYSFS_write( _physfsHandle,
	static_cast<const void *>( actualBuffer ), /* object size */ 1,
	maxLength ) ;


  if ( _cypher )
	delete [] actualBuffer ;


  if ( objectCount == -1 )
	throw OutputStream::WriteFailedException(
	  "EmbeddedFile::write failed: "
	  + EmbeddedFileSystemManager::GetBackendLastError() ) ;
  else
	return static_cast<Size>( objectCount ) ;

#else // OSDL_USES_PHYSICSFS

  throw OutputStream::WriteFailedException(
	"EmbeddedFile::write failed: "
	"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}



Position EmbeddedFile::tell()
{

#if OSDL_USES_PHYSICSFS


  //LogPlug::trace( "EmbeddedFile::tell" ) ;

  PHYSFS_sint64 returnedPos = ::PHYSFS_tell( _physfsHandle ) ;

  if ( returnedPos < 0 )
	throw FileException( "EmbeddedFile::tell failed for '" + _name
	  + "': " + EmbeddedFileSystemManager::GetBackendLastError() ) ;

  Position pos = static_cast<Position>( returnedPos ) ;

  if ( pos != returnedPos )
	throw FileException( "EmbeddedFile::tell failed for '" + _name
	  + "': offset too high for position variable." ) ;

  /*
	LogPlug::trace( "EmbeddedFile::tell: returning position "
	+ Ceylan::toString(pos) ) ;
  */

  return pos ;

#else // OSDL_USES_PHYSICSFS

  throw FileException( "EmbeddedFile::tell failed: "
	"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}



void EmbeddedFile::seek( Position targetPosition )
{

#if OSDL_USES_PHYSICSFS

  /*
	LogPlug::trace( "EmbeddedFile::seek: seeking position "
	+ Ceylan::toString( targetPosition ) ) ;
  */

  int res = ::PHYSFS_seek( _physfsHandle, targetPosition ) ;

  if ( res == 0 )
	throw FileException( "EmbeddedFile::seek failed for '" + _name
	  + "': " + EmbeddedFileSystemManager::GetBackendLastError() ) ;

#else // OSDL_USES_PHYSICSFS

  throw Ceylan::System::FileException( "EmbeddedFile::seek failed: "
	"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}





// EmbeddedFile-specific methods.



Size EmbeddedFile::size() const
{

#if OSDL_USES_PHYSICSFS

  PHYSFS_sint64 fileSize = PHYSFS_fileLength( _physfsHandle ) ;

  if ( fileSize == - 1 )
	throw Ceylan::System::FileException(
	  "EmbeddedFile::size failed: "
	  + EmbeddedFileSystemManager::GetBackendLastError() ) ;

  Size res = static_cast<Size>( fileSize ) ;

  /*
   * Useless, and comparison between signed and unsigned integer expressions:
   *

   if ( res != fileSize )
   throw Ceylan::System::FileException( "EmbeddedFile::size failed: "
   "could not fit in returned variable." ) ;

  */

  return res ;

#else // OSDL_USES_PHYSICSFS

  throw Ceylan::System::FileException(
	"EmbeddedFile::size failed: no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}



void EmbeddedFile::serialize( PHYSFS_File & handle )
{

  // Let EmbeddedFileException propagate:
  FromHandletoHandle( *_physfsHandle, handle, size() ) ;

}



PHYSFS_File & EmbeddedFile::getPhysicsFSHandle() const
{

#if OSDL_USES_PHYSICSFS

  if ( _physfsHandle != 0 )
	return *_physfsHandle ;
  else
	throw EmbeddedFileException( "EmbeddedFile::getPhysicsFSHandle failed: "
	  "null handle found." ) ;

#else // OSDL_USES_PHYSICSFS

  throw EmbeddedFileException( "EmbeddedFile::getPhysicsFSHandle: "
	"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}




void EmbeddedFile::CypherBuffer( Ceylan::Byte * buffer,
  Ceylan::System::Size size )
{

  Ceylan::Byte XORByte = EmbeddedFileSystemManager::GetXORByte() ;

  for ( Size i = 0; i < size; i++ )
	buffer[i] = (buffer[i]^XORByte) + 117 ;

}



void EmbeddedFile::DecypherBuffer( Ceylan::Byte * buffer, Size size )
{

  Ceylan::Byte XORByte = EmbeddedFileSystemManager::GetXORByte() ;

  for ( Size i = 0; i < size; i++ )
  {
	buffer[i] = (buffer[i]-117)^XORByte ;

	/*
	  LogPlug::trace( "DecypherBuffer: "
	  + Ceylan::toHexString( buffer[i] ) ) ;
	*/
  }

}




StreamID EmbeddedFile::getStreamID() const
{

#if OSDL_USES_PHYSICSFS

  // Converting a pointer to an integer might loose precision:
  return static_cast<StreamID>( reinterpret_cast<uintptr_t>(
      & getPhysicsFSHandle() ) ) ;

#else // OSDL_USES_PHYSICSFS

  // No appropriate identifier found:
  return -1 ;

#endif // OSDL_USES_PHYSICSFS

}



const std::string EmbeddedFile::toString( Ceylan::VerbosityLevels level ) const
{

  return "Embedded file object for user-specified filename '"
	+ Ceylan::encodeToROT13( _name )
	+ "', corresponding to an actual name in archive '" + _name + "'" ;

}



EmbeddedFile & EmbeddedFile::Create( const std::string & filename,
  OpeningFlag createFlag,	PermissionFlag permissionFlag )
{

  // Ensures creation is requested:
  return * new EmbeddedFile( filename, createFlag | File::CreateFile,
	permissionFlag ) ;

}



EmbeddedFile & EmbeddedFile::Open( const std::string & filename,
  OpeningFlag openFlag )
{

  // Ensures creation is not requested:
  return * new EmbeddedFile( filename, openFlag & ~File::CreateFile
	/* use default permission flag */ ) ;

}





// Protected section.



EmbeddedFile::EmbeddedFile( const string & name, OpeningFlag openFlag,
  PermissionFlag permissions ) :
  File( Ceylan::encodeToROT13(name), openFlag, permissions ),
  _physfsHandle( 0 )
{

  // (File constructor may raise FileException)

#if OSDL_USES_PHYSICSFS

  _cypher = EmbeddedFileSystemManager::SecureEmbeddedFileSystemManager() ;

  if ( openFlag & NonBlocking )
	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
	  "cannot open a file in non-blocking mode." ) ;

  if ( openFlag & Synchronous )
	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
	  "cannot open a file in synchronous mode." ) ;

  if ( ! ( openFlag & Binary ) )
	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
	  "cannot open a file in non-binary mode." ) ;

  if ( openFlag != DoNotOpen )
	reopen() ;


#else // OSDL_USES_PHYSICSFS

  throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
	"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}





// Implementations of inherited methods.



FileSystemManager & EmbeddedFile::getCorrespondingFileSystemManager() const
{

  try
  {

	return EmbeddedFileSystemManager::GetEmbeddedFileSystemManager() ;

  }
  catch( const EmbeddedFileSystemManagerException & e )
  {

	throw FileDelegatingException(
	  "EmbeddedFile::getCorrespondingFileSystemManager failed: "
	  + e.toString() ) ;

  }

}



void EmbeddedFile::reopen()
{

#if OSDL_USES_PHYSICSFS

  PHYSFS_File * file ;

  if ( _openFlag & Ceylan::System::File::Write )
  {

	file = PHYSFS_openWrite( _name.c_str() ) ;

	if ( file == 0 )
	  throw FileOpeningFailed("EmbeddedFile::reopen for writing failed for file '"
		+ _name + "': "
		+ EmbeddedFileSystemManager::GetBackendLastError() ) ;

  }
  else if ( _openFlag & Ceylan::System::File::AppendFile )
  {

	file = PHYSFS_openAppend( _name.c_str() ) ;

	if ( file == 0 )
	  throw FileOpeningFailed(
		"EmbeddedFile::reopen for appending failed for file '"
		+ _name + "': "
		+ EmbeddedFileSystemManager::GetBackendLastError() ) ;

  }
  else if ( _openFlag & Ceylan::System::File::Read )
  {

	file = PHYSFS_openRead( _name.c_str() ) ;

	if ( file == 0 )
	{
	  throw FileOpeningFailed(
		"EmbeddedFile::reopen for reading failed for file '"
		+ _name + "': "
		+ EmbeddedFileSystemManager::GetBackendLastError() ) ;
	}
  }
  else
  {

	throw FileOpeningFailed(
	  "EmbeddedFile::reopen for reading failed for file '" + _name + "': "
	  "only possible opening flags are writing, appending or reading." ) ;
  }


  _physfsHandle = file ;

#else // OSDL_USES_PHYSICSFS

  throw Ceylan::System::FileOpeningFailed( "EmbeddedFile::reopen failed: "
	"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}



std::string EmbeddedFile::interpretState() const
{

  return "PhysicsFS-based embedded file" ;

}




// Private section.



void EmbeddedFile::FromHandletoHandle( PHYSFS_File & from, PHYSFS_File & to,
  Size length )
{

  // Note nevertheless this could be implemented like FromFDtoFD.

  throw EmbeddedFileException( "EmbeddedFile::FromHandletoHandle: "
	"operation not available on embedded filesystems." ) ;

}

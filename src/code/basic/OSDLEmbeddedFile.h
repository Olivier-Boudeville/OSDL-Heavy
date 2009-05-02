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


#ifndef OSDL_EMBEDDED_FILE_H_
#define OSDL_EMBEDDED_FILE_H_



#include "Ceylan.h"  // for inheritance and FileSystemManagerException



#if ! defined(OSDL_USES_PHYSICSFS) || OSDL_USES_PHYSICSFS 

// No need to include PhysicsFS header here:
struct PHYSFS_File ;

#endif //  ! defined(OSDL_USES_PHYSICSFS) || OSDL_USES_PHYSICSFS 




namespace OSDL
{


		
	/// Mother class for all exceptions related to embedded files.
	class OSDL_DLL EmbeddedFileException: public Ceylan::System::FileException
	{ 
	
		public: 
	   
			explicit EmbeddedFileException( const std::string & reason ) ;
		   
			virtual ~EmbeddedFileException() throw() ; 
			   
	} ;

	
	
	
	/**
	 * Encapsulates embedded files, i.e. files in a virtual filesystem stored
     * in an actual standard file, as provided by the PhysicsFS back-end.
	 *
	 * @see http://icculus.org/physfs/
     *
     * @note All files are managed in binary mode only. Reads and writes will
     * be cyphered, depending on the filesystem manager setting.
     *
	 * Actual files should be created and opened with respectively the
	 * File::Create and File::Open factories, that allow the
	 * user program to be cross-platform by hiding each filesystem-related
	 * per-platform specificity.
	 *
	 */
	class OSDL_DLL EmbeddedFile: public Ceylan::System::File
	{
	
			
		public:



			/**
			 * Destroys the embedded file reference object, does not remove
			 * the file itself.
			 *
			 * @note Will close automatically the file if needed.
			 *
			 * To remove the file from disk, use remove().
			 *
			 * @see remove
			 *
			 */
			virtual ~EmbeddedFile() throw() ;




			// Implementation of instance methods inherited from File.



			/**
			 * Returns true iff this file is open.
			 *
			 */
			virtual bool isOpen() const ;



			/**
			 * Closes the file for read/write actions.
			 *
             * @note May fail if the operating system was buffering writes to
             * the physical media, and, now forced to write those changes to
             * physical media, cannot store the data for some reason.
             * In such a case, the file handle stays open.
             *
			 * @return true iff an operation had to be performed.
			 *
			 * @throw Stream::CloseException if the close operation failed.
			 *
			 */
			virtual bool close() ;



			/**
			 * Saves the file under a new name.
			 *
			 * @param newName the name of the newly copied file.
			 *
             * @note Currently not implemented.
             *
			 * @throw FileException in all cases.
			 *
			 */
			virtual void saveAs( const std::string & newName ) ;




			// Locking section.

			 
			 
			/**
			 * Locks the file for reading.
			 *
			 * @throw FileReadLockingFailed if the operation failed or 
			 * if the file lock feature is not available.
			 *		 
			 */
			virtual void lockForReading() const ;



			/**
			 * Unlocks the file for reading.
			 *
			 * @throw FileReadUnlockingFailed if the operation failed or
			 * if the file lock feature is not available.
			 *		 
			 */
			virtual void unlockForReading() const ;



			/**
			 * Locks the file for writing.
			 *
			 * @throw FileWriteLockingFailed if the operation failed or 
			 * if the file lock feature is not available.
			 *		 
			 */
			virtual void lockForWriting() const ;



			/**
			 * Unlocks the file for writing.
			 *
			 * @throw FileWriteUnlockingFailed if the operation failed 
			 * or if the file lock feature is not available.
			 *		 
			 */
			virtual void unlockForWriting() const ;



			/**
			 * Tells whether the file is locked.
			 *
			 * @return true if the lock feature is available and the 
			 * file is locked, otherwise returns false, i.e. if the file
			 * is locked or if the lock feature is not available.
			 *		 
			 */
			virtual bool isLocked() const ;



			// size method inherited from File.



			/**
			 * Returns the latest change time of this embedded file.
			 *
			 * @throw FileLastChangeTimeRequestFailed if the 
			 * operation failed, or is not supported.
			 *
			 */
			virtual time_t getLastChangeTime() const ;



			/**
			 * Reads up to maxLength bytes from this file to specified
			 * buffer.
			 * The file must be opened for reading.
			 *
			 * @param buffer the buffer where to store read bytes. 
			 * Its size must be at least maxLength bytes.
			 *
			 * @param maxLength the maximum number of bytes that should 
			 * be read.
			 *
			 * @return The number of bytes actually read, which should
			 * be maxLength or lower.
			 *
			 * @throw InputStream::ReadFailedException if a read error
			 * occurred. Note that this is not a child class of 
			 * FileException, as it comes from an inherited interface.
			 *
             * @note If cyphering is enabled, it will be transparently managed.
             *
			 */
	 		virtual Ceylan::System::Size read( Ceylan::Byte * buffer, 
            	Ceylan::System::Size maxLength ) ;


			
			// readExactLength inherited.
						
							
			// hasAvailableData inherited.
			
			
			
			/**
			 * Writes message to this file.
			 * The file must be opened for writing.
			 *
			 * @param message the message to write to this file.
			 *
			 * @return The number of bytes actually written, which 
			 * should be equal to the size of the string or lower.
			 *
			 * @throw OutputStream::WriteFailedException if a write error
			 * occurred. Note that this is not a child class of 
			 * FileException, as it comes from an inherited interface.
			 *
             * @note If cyphering is enabled, it will be transparently managed.
             *
			 */
			virtual Ceylan::System::Size write( const std::string & message ) ;



			/**
			 * Writes up to maxLength bytes from the specified buffer
			 * to this file.
			 *
			 * @param buffer the buffer where to find bytes that must
			 * be written to this file.
			 * Its size must be at least maxLength bytes.
			 *
			 * @return The number of bytes actually written, which 
			 * should be equal to maxLength.
			 *
			 * @throw OutputStream::WriteFailedException if a write error
			 * occurred. Note that this is not a child class of 
			 * FileException, as it comes from an inherited interface.
			 *
             * @note If cyphering is enabled, it will be transparently managed.
             *
			 */
			virtual Ceylan::System::Size write( const Ceylan::Byte * buffer, 
				Ceylan::System::Size maxLength ) ;



			/**
			 * Determines current position within this embedded file.
			 *
			 * @return offset in bytes from start of file.
             *
			 * @throw FileException if the operation failed.
			 *
			 */
			virtual Ceylan::System::Position tell() ;



			/**
			 * Seeks to specified position within this embedded file.
			 *
			 * @param targetPosition this position corresponds to the
             * number of bytes from start of file to seek to.
             *
			 * @throw FileException if the operation failed.
			 *
			 */
			virtual void seek( Ceylan::System::Position targetPosition ) ;



			// open and remove inherited.



			// EmbeddedFile-specific methods.
			
			
		                
			/**
			 * Returns the file size, in bytes.
			 *
			 * @see GetSize
			 *
			 * @throw FileException, including FileLookupFailed if the file
			 * metadata could not be accessed or if the operation is not
			 * supported on this platform, and FileDelegatingException if
			 * the corresponding filesystem manager could not be used as
			 * expected.
			 *
			 */
			virtual Ceylan::System::Size size() const ;
    
	
    
   			/**
			 * Sends the file content to the <b>fd</b> PhysicsFS handle
			 * stream.
			 *
			 * @throw EmbeddedFileException if the operation failed or
			 * if the PhysicsFS handle feature is not available.
			 *		 
			 */
			virtual void serialize( PHYSFS_File & handle ) ;



			/**
			 * Returns the stream id, its PhysicsFS handle.
			 *
			 * @throw EmbeddedFileException if the operation failed or
			 * if the PhysicsFS handle feature is not available.
			 *
			 */
			PHYSFS_File & getPhysicsFSHandle() const ;



			/**
             * Cyphers specified buffer.
             *
             * @param buffer the buffer to update.
             *
             * @param size the number of bytes to cypher.
             *
             * @throw EmbeddedFileException if the operation failed.
             *
             * @note Public, so that very specific tools can call it 
             * (ex: cypherOSDLFile).
             *
             */
            static void CypherBuffer( Ceylan::Byte * buffer, 
            	Ceylan::System::Size size ) ;



			/**
             * Decyphers specified buffer.
             *
             * @param buffer the buffer to update.
             *
             * @param size the number of bytes to decypher.
             *
             * @throw EmbeddedFileException if the operation failed.
             *
             * @note Public, so that very specific tools can call it 
             * (ex: cypherOSDLFile).
             *
             */
            static void DecypherBuffer( Ceylan::Byte * buffer, 
            	Ceylan::System::Size size ) ;




			// Interface implementation.



			/**
			 * Returns this file's ID.
			 *
			 * @return the StreamID or -1 if nothing appropriate can
			 * be returned with the available features.
			 *
			 */
			virtual Ceylan::System::StreamID getStreamID() const ;



			// getInputStreamID inherited.
			
			
			// getOutputStreamID inherited.
			
			
			
        	/**
        	 * Returns an user-friendly description of the state of
			 * this object.
        	 *
			 * @param level the requested verbosity level.
			 *
			 * @note Text output format is determined from overall 
			 * settings.
			 *
			 * @see TextDisplayable
			 *
			 */
        	virtual const std::string toString( 
				Ceylan::VerbosityLevels level = Ceylan::high ) const ;

							



			/*
			 * Helper section.
			 *
			 * Factories still have to be public, to allow to create on
			 * specific cases (ex: process redirection) specifically
			 * embedded files, not only files.
			 *
			 */
			
	
	
			/**
			 * Returns an EmbeddedFile reference on a newly created file.
			 *
			 * By default, it creates a new file on disk. If the name
			 * corresponds to an already-existing file, it will be
			 * truncated and overwritten.
			 *
			 * @param filename the name of the file to be created.
			 *
			 * @param createFlag the flag describing the creation mode. 
             * Only binary access is performed.
			 *
			 * @param permissionFlag the flag describing the requested
			 * permissions.
			 *
			 * @see OpeningFlag, PermissionFlag
			 *
			 * @note This EmbeddedFile factory is only a helper method.
			 * Ceylan users should only use File::Create instead.
			 *
			 * @throw FileException, including FileCreationFailed if the
			 * operation failed or is not supported on this platform.
			 *
			 */
			static EmbeddedFile & Create( const std::string & filename, 
				Ceylan::System::OpeningFlag createFlag 
                    = Ceylan::System::File::CreateToWriteBinary,
				Ceylan::System::PermissionFlag permissionFlag 
                    = Ceylan::System::File::OwnerReadWrite ) ;

			
			
			/**
			 * Returns an EmbeddedFile reference on specified
			 * already-existing file, in platform-independent notation, 
             * which will be opened with specified settings.
			 *
             * If the file is open for writing or appending, then the write 
             * directory will be taken as the root of the writable filesystem.
             *
             * When a file is opened for writing, the specifiedfile is created
             * if it does not exist. Otherwise it is truncated to zero bytes,
             * and the writing offset is set to the start.
             *
             * When a file is opened for appending, the specifiedfile is created
             * if it does not exist. Otherwise the writing offset is set to 
             * the end of the file, so the first write will be the byte after
             * the end.
             *
             * If the file is open for reading, then the search path will be
             * checked one at a time until a matching file is found.
             *  The reading offset is set to the first byte of the file.
             *
			 * @param filename the name of the file to open.
			 *
			 * @param openFlag the flag describing the opening mode.
			 *
			 * @see OpeningFlag
			 *
			 * @note This EmbeddedFile factory is only a helper method.
			 * OSDL users should only use File::Open instead.
			 *
			 * @throw FileException, including FileOpeningFailed if the
			 * operation failed or is not supported on this platform.
			 *
			 */
			static EmbeddedFile & Open( const std::string & filename, 
				Ceylan::System::OpeningFlag openFlag 
                    = Ceylan::System::File::OpenToReadBinary ) ;
			
	
			
			
		protected:
    
	

			/**
			 * Constructs an Embedded file reference object.
			 * By default, it creates a new file on disk, if the name
			 * corresponds to an already-existing file, it will be
			 * truncated and overwritten.
			 *
			 * @param name the name of the file.
			 *
			 * @param openFlag the flag describing the opening mode.
			 *
			 * @param permissionFlag the flag describing the
			 * requested permissions, if this file is to be created.
			 * Otherwise (if the file already exists), this parameter
			 * is ignored.
			 *
			 * @see OpeningFlag, PermissionFlag
			 *
			 * @note If not specifically set, the file is open in text 
			 * mode: one should not forget to add the Binary flag. 
			 * The mistake can be detected when basic read() returns less
			 * than the requested size, or when readExactLength() never
			 * terminates.
			 *
			 * @note This constructor should not be called directly, the
			 * File factories (File::Create and File::Open) should be 
			 * used instead, as they allow to write code really independant
			 * from the running platform, not having to choose between the
			 * per-platform constructors.
			 *
			 * @throw FileManagementException if the operation failed.
			 *
			 */
			explicit EmbeddedFile( const std::string & name, 
				Ceylan::System::OpeningFlag openFlag 
                    = Ceylan::System::File::CreateToWriteBinary,
				Ceylan::System::PermissionFlag permissionFlag 
                    = Ceylan::System::File::OwnerReadWrite ) ;





			// Implementations of inherited methods.
			
			
			
			/**
			 * Returns the embedded filesystem manager.
			 *
			 * @throw FileDelegatingException if the operation failed.
			 *
			 */
			virtual Ceylan::System::FileSystemManager &
            	getCorrespondingFileSystemManager() const ;



			/**
			 * Tries to reopen file.
			 *
			 * @throw FileOpeningFailed if the operation failed.
			 *
			 */
			virtual void reopen() ;

			
			
            /**
             * Interprets the current state of this file.
             *
             */
			virtual std::string interpretState() const ;




		private:



			/**
			 * Internal PhysicsFS handle.
			 *
			 */
			PHYSFS_File * _physfsHandle ;



			/**
			 * Tells whether reads and writes to this file should be
             * cyphered.
			 *
			 */
			bool _cypher ;



			/**
			 * Copy constructor made private to ensure that it will 
			 * be never called.
			 *
			 * The compiler should complain whenever this undefined
			 * constructor is called, implicitly or not.
			 *
			 */
			EmbeddedFile( const EmbeddedFile & source ) ;



			/**
			 * Assignment operator made private to ensure that it will
			 * be never called.
			 *
			 * The compiler should complain whenever this undefined 
			 * operator is called, implicitly or not.
			 *
			 */
			EmbeddedFile & operator = ( const EmbeddedFile & source ) ;



			/**
			 * Transfers bytes from a PhysicsFS handle to another.
			 *
			 * @param from the source PhysicsFS handle.
			 *
			 * @param to the target PhysicsFS handle.
			 *
			 * @param length the length of the transfer, in bytes.
			 *
             * @note Currently not implemented.
             *  
			 * @throw EmbeddedFileException in all cases.
			 *
			 */
			static void FromHandletoHandle( PHYSFS_File & from, 
				PHYSFS_File & to, Ceylan::System::Size length ) ;


	} ;
	
	
}



#endif // OSDL_EMBEDDED_FILE_H_


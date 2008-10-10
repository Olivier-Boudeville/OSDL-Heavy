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
	   
			explicit EmbeddedFileException( 
				const std::string & reason ) throw() ;
		   
			virtual ~EmbeddedFileException() throw() ; 
			   
	} ;

	
	
	/**
	 * Encapsulates embedded files, i.e. files in a virtual filesystem stored
     * in an actual standard file, as provided by the PhysicsFS back-end.
	 *
	 * @see http://icculus.org/physfs/
     *
     * @note All files are managed in binary mode only.
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
			 * Closes the file for read/write actions.
			 *
             * @note May fail if the operating system was buffering writes to
             * the physical media, and, now forced to write those changes to
             * physical media, cannot store the data for some reason.
             * In such a case, the file handle stays open.
             *
			 * @return true iff an operation had to be performed.
			 *
			 * @throw Stream::CloseException if the close operation failed,
			 * including if the file was not already opened.
			 *
			 */
			virtual bool close() throw( Stream::CloseException ) ;


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
			virtual void saveAs( const std::string & newName )
				throw( Ceylan::System::FileException ) ;




			// Locking section.

			 
			/**
			 * Locks the file for reading.
			 *
			 * @throw FileReadLockingFailed if the operation failed or 
			 * if the file lock feature is not available.
			 *		 
			 */
			virtual void lockForReading() const 
				throw( Ceylan::System::FileReadLockingFailed ) ;


			/**
			 * Unlocks the file for reading.
			 *
			 * @throw FileReadUnlockingFailed if the operation failed or
			 * if the file lock feature is not available.
			 *		 
			 */
			virtual void unlockForReading() const 
				throw( Ceylan::System::FileReadUnlockingFailed ) ;


			/**
			 * Locks the file for writing.
			 *
			 * @throw FileWriteLockingFailed if the operation failed or 
			 * if the file lock feature is not available.
			 *		 
			 */
			virtual void lockForWriting() const 
				throw( Ceylan::System::FileWriteLockingFailed ) ;


			/**
			 * Unlocks the file for writing.
			 *
			 * @throw FileWriteUnlockingFailed if the operation failed 
			 * or if the file lock feature is not available.
			 *		 
			 */
			virtual void unlockForWriting() const 
				throw( Ceylan::System::FileWriteUnlockingFailed ) ;


			/**
			 * Tells whether the file is locked.
			 *
			 * @return true if the lock feature is available and the 
			 * file is locked, otherwise returns false, i.e. if the file
			 * is locked or if the lock feature is not available.
			 *		 
			 */
			virtual bool isLocked() const throw() ;


			// size method inherited from File.


			/**
			 * Returns the latest change time of this embedded file.
			 *
			 * @throw FileLastChangeTimeRequestFailed if the 
			 * operation failed, or is not supported.
			 *
			 */
			virtual time_t getLastChangeTime() const 
				throw( Ceylan::System::FileLastChangeTimeRequestFailed ) ;


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
			 */
	 		virtual Ceylan::System::Size read( Ceylan::Byte * buffer, 
            	Ceylan::System::Size maxLength ) 
				throw( InputStream::ReadFailedException ) ;

			
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
			 */
			virtual Ceylan::System::Size write( const std::string & message ) 
				throw( OutputStream::WriteFailedException ) ;


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
			 */
			virtual Ceylan::System::Size write( const Ceylan::Byte * buffer, 
					Ceylan::System::Size maxLength ) 
				throw( OutputStream::WriteFailedException ) ;


			// open and remove inherited.



			// EmbeddedFile-specific methods.
			
			
			
            /**
             * Returns the size, in bytes, of this embedded file.
             *
             * @throw FileSizeRequestFailed if the size could not be 
             * determined.
             *
             */ 
            virtual Ceylan::System::Size getSize() const throw( 
            	Ceylan::System::FileSizeRequestFailed ) ;
                
    
   			/**
			 * Sends the file content to the <b>fd</b> PhysicsFS handle
			 * stream.
			 *
			 * @throw EmbeddedFileException if the operation failed or
			 * if the PhysicsFS handle feature is not available.
			 *		 
			 */
			virtual void serialize( PHYSFS_File & handle ) const
				throw( EmbeddedFileException ) ;


			/**
			 * Returns the stream id, its PhysicsFS handle.
			 *
			 * @throw EmbeddedFileException if the operation failed or
			 * if the PhysicsFS handle feature is not available.
			 *
			 */
			PHYSFS_File & getPhysicsFSHandle() const 
				throw( EmbeddedFileException ) ;





			// Interface implementation.


			/**
			 * Returns this file's ID.
			 *
			 * @return the StreamID or -1 if nothing appropriate can
			 * be returned with the available features.
			 *
			 */
			virtual Ceylan::System::StreamID getStreamID() const throw() ;



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
				Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;

							


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
                    	= Ceylan::System::File::OwnerReadWrite ) 
				throw( Ceylan::System::FileException ) ;

			
			
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
                    	= Ceylan::System::File::OpenToReadBinary ) 
				throw( Ceylan::System::FileException ) ;
			
	
			
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
                    	= Ceylan::System::File::OwnerReadWrite ) 
				throw( Ceylan::System::FileManagementException ) ;



			// Implementations of inherited methods.
			
			
			/**
			 * Returns the embedded filesystem manager.
			 *
			 * @throw FileDelegatingException if the operation failed.
			 *
			 */
			virtual Ceylan::System::FileSystemManager &
            		getCorrespondingFileSystemManager()
				const throw( Ceylan::System::FileDelegatingException ) ;


			/**
			 * Tries to reopen file.
			 *
			 * @throw FileOpeningFailed if the operation failed.
			 *
			 */
			virtual void reopen() throw( Ceylan::System::FileOpeningFailed ) ;

			
            /**
             * Interprets the current state of this file.
             *
             */
			virtual std::string interpretState() const throw() ;



		private:



			/**
			 * Copy constructor made private to ensure that it will 
			 * be never called.
			 *
			 * The compiler should complain whenever this undefined
			 * constructor is called, implicitly or not.
			 *
			 */
			EmbeddedFile( const EmbeddedFile & source ) throw() ;


			/**
			 * Assignment operator made private to ensure that it will
			 * be never called.
			 *
			 * The compiler should complain whenever this undefined 
			 * operator is called, implicitly or not.
			 *
			 */
			EmbeddedFile & operator = ( const EmbeddedFile & source )
				throw() ;



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
					PHYSFS_File&  to, Ceylan::System::Size length )
				throw( EmbeddedFileException ) ;


			/**
			 * Internal PhysicsFS handle.
			 *
			 */
			PHYSFS_File * _physfsHandle ;


	} ;
	
	
}


#endif // OSDL_EMBEDDED_FILE_H_

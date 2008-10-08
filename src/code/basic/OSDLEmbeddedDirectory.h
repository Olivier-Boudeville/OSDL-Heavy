#ifndef OSDL_EMBEDDED_DIRECTORY_H_
#define OSDL_EMBEDDED_DIRECTORY_H_


#include "Ceylan.h"  // for inheritance and FileSystemManagerException


#include <list>
#include <string>




namespace OSDL
{



	/*
	 * Each Directory child class is linked to a corresponding filesystem
	 * manager child class, here it is the embedded one.
	 *
	 */
	class EmbeddedFileSystemManager ;
	

	
	/**
	 * Encapsulates embedded directories, i.e. directories in a virtual
     * filesystem stored in an actual standard file, as provided by the
     * PhysicsFS back-end.
	 *
	 * @see http://icculus.org/physfs/
	 *
	 * Actual directories should be created and opened with respectively the
	 * Directory::Create and Directory::Open factories, that allow the
	 * user program to be cross-platform by hiding each filesystem-related
	 * per-platform specificity.
	 *
	 * @see File, FileSystemManager for other file-related operations.
	 *
	 */
	class OSDL_DLL EmbeddedDirectory: public Ceylan::System::Directory
	{


		public:


			/**
			 * Destroys the directory reference, not the directory itself.
			 *
			 * @see Directory::Remove
			 *
			 * The destructor must be public as instances created by 
			 * factories have to be deallocated by the user.
			 *
			 */
			virtual ~EmbeddedDirectory() throw() ;



			// Instance methods.
			
			
			
			// Directory content subsection.
			
			
			
			/**q
			 * Tells whether this directory with a direct subdirectory named
             * <b>subdirectoryName</b> can be found in the search path.
			 *
			 * @param subdirectoryName the name of the directory entry to
			 * look-up, in the search path.
			 *
			 * @throw DirectoryLookupFailed is the operation failed or is
			 * not supported.
			 *
			 */
			virtual bool hasDirectory( 
					const std::string & subdirectoryName ) const
				throw( Ceylan::System::DirectoryLookupFailed ) ;


			/**
			 * Tells whether this directory with a direct file or symbolic
			 * link named <b>fileName</b> can be found in the search path.
			 *
			 * @param fileName the name of the file to look-up, in the search
             * path.
			 *
			 * @throw DirectoryLookupFailed is the operation failed or is
			 * not supported.
			 *
			 */
			virtual bool hasFile( const std::string & fileName ) const
				throw( Ceylan::System::DirectoryLookupFailed ) ;


			/**
			 * Tells whether the directory has a direct entry named 
			 * <b>entryName</b>, whatever the entry is (file, directory,
			 * named FIFO, socket, etc.).
			 *
			 * @param entryName the name of the entry to look-up.
			 *
			 * @throw DirectoryLookupFailed is the operation failed or is
			 * not supported.
			 *
			 */
			virtual bool hasEntry( const std::string & entryName ) const
				throw( Ceylan::System::DirectoryLookupFailed ) ;



			/**
			 * Returns the names of all direct subdirectories of this
			 * directory, in the specified list.
			 *
			 * @param subDirectories the caller-provided list in which
			 * subdirectories will be added.
			 *
			 * @throw DirectoryLookupFailed if the operation failed or
			 * is not supported.
			 *
			 * @note Aliases for current and parent directories (ex: '.'
			 * and '..') will be filtered out.
			 *
			 */
			virtual void getSubdirectories( 
					std::list<std::string> & subDirectories )
				const throw( Ceylan::System::DirectoryLookupFailed ) ;


			/**
			 * Returns the names of all regular files and symbolic links 
			 * of this directory, in the specified list.
			 *
			 * @param files the caller-provided list in which
			 * regular files and symbolic links will be added.
			 *
			 * @throw DirectoryLookupFailed if the operation failed or
			 * is not supported.
			 *
			 */
			virtual void getFiles( std::list<std::string> & files )
				const throw( Ceylan::System::DirectoryLookupFailed ) ;


			/**
			 * Returns the names of all files in specified search path,
             * stored in the specified list.
			 *
 			 * If "C:\mydir" is in the search path and contains a directory
             * "savegames" that contains "x.sav", "y.sav", and "z.sav", and
             * there is also a "C:\userdir" in the search path that has a
             * "savegames" subdirectory with "w.sav", then for the directory
             * corresponding to "savegames" this method will return:
             * [ "x.sav", "y.sav", "z.sav", "w.sav" ].
             *
             * There will be no duplicates, but the order the list is not
             * specified.
             *
			 * @param entries the caller-provided list in which 
			 * entries will be added.
			 *
			 * @throw DirectoryLookupFailed if the operation failed or
			 * is not supported.
			 *
			 */
			virtual void getEntries( std::list<std::string> & entries )
				const throw( Ceylan::System::DirectoryLookupFailed ) ;


			/**
			 * Returns the names of all direct entries of any type of 
			 * this directory (including files and directories), in the
			 * corresponding specified list.
			 *
			 * @param subDirectories the caller-provided list in which 
			 * subDirectories of this directory will be added.
			 *
			 * @param files the caller-provided list in which 
			 * files of this directory will be added.
			 *
			 * @param otherEntries the caller-provided list in which 
			 * other entries (named FIFO, sockets, etc.) of this
			 * directory will be added.
			 *
			 * @throw DirectoryLookupFailed if the operation failed or
			 * is not supported.
			 *
			 * @note Aliases for current and parent directories (ex: '.'
			 * and '..') will be filtered out.
			 *
			 */
			virtual void getSortedEntries( 
					std::list<std::string> & subDirectories,
					std::list<std::string> & files,
					std::list<std::string> & otherEntries )
				const throw( Ceylan::System::DirectoryLookupFailed ) ;




			// Other instance methods.
			

			// goDown, isValid, getPath, removeLeadingSeparator inherited.


			/**
			 * Returns a user-friendly description of the state of 
			 * this object.
			 *
			 * @param level the requested verbosity level.
			 *
			 * @note Text output format is determined from overall settings.
			 *
			 * @see TextDisplayable
			 *
			 */
	        virtual const std::string toString( 
					Ceylan::VerbosityLevels level = Ceylan::high )
				const throw() ;




			// Factory section.


			/**
			 * Returns an EmbeddedDirectory reference on a directory newly
			 * created on disk, thanks to the operating system facilities.
			 *
			 * @param newDirectoryName the name of the directory to create.
			 *
			 * This is an EmbeddedDirectory helper factory, user code should
			 * rely on the higher level and cross-platform 
			 * Directory::Create instead.
			 *
			 * @throw DirectoryException, including DirectoryCreationFailed
			 * if the operation failed or is not supported on this platform,
			 * or DirectoryDelegatingException if the relevant filesystem
			 * manager could not be retrieved.
			 *
			 */
			static EmbeddedDirectory & Create( 
					const std::string & newDirectoryName ) 
				throw( Ceylan::System::DirectoryException ) ;

			
			/**
			 * Returns an EmbeddedDirectory reference on specified
			 * already-existing directory, which will be "opened" (i.e.
			 * referred to), thanks to the operating system facilities.
			 *
			 * @param directoryName the name of the directory. If not
			 * specified (the string is empty), returns a reference to the
			 * current working directory.
			 *
			 * This is an EmbeddedDirectory helper factory, user code should
			 * rely on the higher level and cross-platform 
			 * Directory::Open instead.
			 *
			 * @throw DirectoryException, including DirectoryOpeningFailed
			 * if the operation failed or is not supported on this platform,
			 * or DirectoryDelegatingException if the relevant filesystem
			 * manager could not be retrieved.
			 *
			 */
			static EmbeddedDirectory & Open( 
					const std::string & directoryName = "" ) 
				throw( Ceylan::System::DirectoryException ) ;
				
				

			/**
			 * Creates an EmbeddedDirectory reference on a directory either
			 * already existing, or to be created on disk thanks to the
			 * operating system facilities.
			 *
			 * @param directoryName the name of the directory to create, 
             * specified in platform-independent notation in relation to the
             * write directory.
             * All missing parent directories are also created if they
             * do not exist.
			 * 
             * So if the write directory is set to "C:\mygame\writedir" and 
             * directoryName is "downloads/maps", then the directories
             * "C:\mygame\writedir\downloads" and
             * "C:\mygame\writedir\downloads\maps" will be created if possible.
             * If the creation of "maps" fails after we have successfully
             * created "downloads", then the method leaves the created 
             * directory behind and throws an exception.
             *
			 * This is an EmbeddedDirectory helper constructor, user code
			 * should rely on the higher level and cross-platform factories,
			 * Directory::Create and Directory::Open instead.
			 *
			 * @throw DirectoryException, including DirectoryCreationFailed
			 * if the operation failed or is not supported on this platform,
			 * or DirectoryDelegatingException if the relevant filesystem
			 * manager could not be retrieved.
			 *
			 */
			explicit EmbeddedDirectory( const std::string & directoryName,
					bool create = true ) 
				throw( Ceylan::System::DirectoryException ) ;

			
				
		protected:



			/**
			 * Returns the embedded filesystem manager.
			 *
			 * @note It is not necessarily the current default one.
			 *
			 * @throw DirectoryDelegatingException if the operation failed.
			 *
			 */
			virtual Ceylan::System::FileSystemManager &
            		getCorrespondingFileSystemManager()
				const throw( Ceylan::System::DirectoryDelegatingException ) ;



			/**
			 * Root directory prefix.
			 * @example "" on Unix, "c:" on Windows.
			 *
			 */
			static const std::string RootDirectoryPrefix ;


			/**
			 * Current directory name alias.
			 *
			 * @example Typically it is ".".
			 *
			 */
			static const std::string CurrentDirectoryAlias ;


			/**
			 * Upper directory alias.
			 *
			 * @example Typically it is "..".
			 *
			 */
			static const std::string UpperDirectoryAlias ;


			/**
			 * Directory separator.
			 * @example Slash or backslash.
			 */
			static const char Separator ;



		private:


			/**
			 * Copy constructor made private to ensure that it will 
			 * be never called.
			 * The compiler should complain whenever this undefined
			 * constructor is called, implicitly or not.
			 *
			 */
			EmbeddedDirectory( const EmbeddedDirectory & source ) throw() ;


			/**
			 * Assignment operator made private to ensure that it will
			 * be never called.
			 * The compiler should complain whenever this undefined
			 * operator is called, implicitly or not.
			 *
			 */
			EmbeddedDirectory & operator = ( 
				const EmbeddedDirectory & source ) throw() ;


	} ;


}


#endif // OSDL_EMBEDDED_DIRECTORY_H_


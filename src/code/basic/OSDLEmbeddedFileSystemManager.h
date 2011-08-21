/*
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#ifndef OSDL_EMBEDDED_FILE_SYSTEM_MANAGER_H_
#define OSDL_EMBEDDED_FILE_SYSTEM_MANAGER_H_



#include "Ceylan.h"  // for inheritance and FileSystemManagerException


#include <string>



namespace OSDL
{



	class OSDL_DLL EmbeddedFileSystemManagerException :
		public Ceylan::System::FileSystemManagerException
	{

		public:

			explicit EmbeddedFileSystemManagerException(
					const std::string & reason ) :
				Ceylan::System::FileSystemManagerException( reason )
			{

			}

	} ;




	/**
	 * Allows to interact with embedded filesystem services, i.e. opaque files
	 * storing a (most usually read-only) virtual filesystem.
	 *
	 * From the end-user point of view, only a small set of files and archives
	 * will be seen.
	 *
	 * From the application point of view, read/write operations will be
	 * seamlessly performed on an abstract filesystem, stored in the set of
	 * opaque files and archives.
	 *
	 * Default extension for archives is "OAR" (ex: "myArchive.oar"), for 'OSDL
	 * Archive'.
	 *
	 * Based on PhysicsFS (http://icculus.org/physfs/), with added features for
	 * weak cypher, for example to avoid users messing to easily mess with saved
	 * games.
	 *
	 * Use GetEmbeddedFileSystemManager to retrieve that manager.
	 *
	 */
	class OSDL_DLL EmbeddedFileSystemManager :
		public Ceylan::System::FileSystemManager, public Ceylan::Object
	{


	  friend class EmbeddedFile ;


	public:



			/*
			 * No static methods exposed: the user is expected to call them from
			 * the abstract mother classes: File, Directory and
			 * FileSystemManager.
			 *
			 * So only the pure virtual methods of FileSystemManager have to be
			 * defined in this child class, that must not be abstract.
			 *
			 */




			// EmbeddedFileSystemManager-specific section.



			/**
			 * Applies specified basic settings to the embedded filesystem: sets
			 * up sane, default paths, including a write directory chosen to be
			 * "user-directory"/."organization"/"application", like, on UNIX:
			 * "~/.OSDL/MyTestApplication".
			 *
			 * @param organizationName name of the company/group/etc.  to be
			 * used as a directory name. Keep it small, and no-frills.
			 *
			 * @param applicationName program-specific name of your application,
			 * to separate it from your other programs using an embedded
			 * filesystem.
			 *
			 * @param archiveExtension the file extension used by your program
			 * to specify an archive.
			 *
			 * For example, Quake 3 uses "pk3", even though they are just
			 * zipfiles.
			 *
			 * OSDL users are encouraged to use "oar", which stands for the
			 * "OSDL Archive" format.
			 *
			 * Do not specify the '.' char: if you want to look for ZIP files,
			 * specify "ZIP" and not ".ZIP".
			 *
			 * The archive search is case-insensitive.
			 *
			 * @param archiveFirst if true, archives will be prepended to the
			 * search path. False will append them. This parameter will be
			 * ignored if archiveExtension is empty.
			 *
			 * @param includeInsertedMedia if true, will include in the search
			 * path inserted disc media (such as DVD-ROM, HD-DVD, CDRW, and
			 * Blu-Ray discs) which store a filesystem, and if archiveExtension
			 * is not empty, will search them for archives.
			 *
			 * This may cause a significant amount of blocking while discs are
			 * accessed, and if there are no discs in the drive (or even not
			 * mounted on Unix systems), then they may not be made available
			 * anyhow. You may want to set this parameter to false here and
			 * handle the disc setup yourself.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation
			 * failed.
			 *
			 * @note If a non-empty string is specified for archiveExtension,
			 * then all archives with this extension found in default paths will
			 * be automatically mounted.
			 *
			 * Therefore an empty string should be specified to not dig out
			 * archives automatically.
			 *
			 */
			virtual void chooseBasicSettings(
					const std::string & organizationName,
					const std::string & applicationName,
					const std::string & archiveExtension = "oar",
					bool archiveFirst = true,
					bool includeInsertedMedia = false ) ;



			/**
			 * Tells whether the directory where the embedded filesystem will
			 * allow file writing is defined.
			 *
			 */
			virtual bool hasWriteDirectory() const ;



			/**
			 * Returns the directory where the embedded filesystem will allow
			 * file writing.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation
			 * failed, including if no write directory was set.
			 *
			 */
			virtual std::string getWriteDirectory() const ;



			/**
			 * Sets a new write path, i.e. the directory where the embedded
			 * filesystem may write its files.
			 *
			 * @param newWriteDirectory the new directory to be the root of the
			 * write directory, specified in platform-dependent notation.
			 * Specifying an empty string disables the write directory, so that
			 * no file can be opened for writing via this embedded file system.
			 *
			 * @note This call will fail (and fail to change the write
			 * directory) if at least a file remains open in the current write
			 * directory.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation
			 * failed.
			 *
			 */
			virtual void setWriteDirectory(
				const std::string & newWriteDirectory ) ;



			/**
			 * Mounts specified element (directory or archive) from actual
			 * filesystem on specified mount point in virtual tree: add this
			 * element to the search path.
			 *
			 * When you mount an archive, it is added to a virtual file system.
			 *
			 * All files in all of the archives are referenced into a single
			 * virtual hierarchical file tree.
			 *
			 * Two archives mounted at the same place (or an archive with files
			 * overlapping another mountpoint) may have overlapping files: in
			 * such a case, the file earliest in the search path is selected,
			 * and the other files are inaccessible to the application.
			 *
			 * This allows archives to be used to override previous revisions;
			 * you can use the mounting mechanism to place archives at a
			 * specific point in the file tree and prevent overlap; this is
			 * useful for downloadable mods that might trample over application
			 * data or each other, for example.
			 *
			 * The mountpoint does not need to exist prior to mounting, which is
			 * different than those, familiar with the Unix concept of
			 * "mounting", may not expect.
			 *
			 * As well, more than one archive can be mounted to the same
			 * mountpoint, or mountpoints and archive contents can overlap.
			 *
			 * The mechanism still functions as usual.
			 *
			 * @param newActualFilesystemElement the directory or archive to add
			 * to the search path, in platform-dependent notation.
			 *
			 * @param mountPointInVirtualTree location in the virtual tree this
			 * element will be mounted on, in platform-independent notation. An
			 * empty string is equivalent to "/".
			 *
			 * @param append if true append to search path, otherwise prepend.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation failed
			 * (bogus archive, directory missing, etc.).
			 *
			 */
			virtual void mount(
				const std::string & newActualFilesystemElement,
				const std::string & mountPointInVirtualTree = "",
				bool append = true ) ;



			/**
			 * Umounts specified element (directory or archive) of actual
			 * filesystem from virtual tree: removes this element from the
			 * search path.
			 *
			 * This must be a (case-sensitive) match to a directory or archive
			 * already in the search path, specified in platform-dependent
			 * notation.
			 *
			 * @param actualFilesystemElement the directory or archive to remove
			 * from search path.
			 *
			 * @note This call will fail (and fail to remove the element) if at
			 * least a file remains open in it.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation failed
			 * (bogus archive, directory missing, etc.).
			 *
			 */
			virtual void umount( const std::string & actualFilesystemElement ) ;



			/**
			 * Returns the location in virtual tree (i.e. the mount point) of
			 * the specified already-mounted actual filesystem element.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation failed
			 * (bogus archive, directory missing, etc.).
			 *
			 */
			virtual std::string getMountPointFor(
				const std::string & actualFilesystemElement ) const ;



			/**
			 * Returns the current search path, i.e. the ordered list of
			 * elements (directories or archives) from the actual filesystem
			 * that are scanned in turn when searching for a file in virtual
			 * tree.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation failed
			 * (bogus archive, directory missing, etc.).
			 *
			 */
			virtual std::list<std::string> getSearchPath() const ;





			// FileSystemManager-specific section.



			/**
			 * Tells whether the filesystem entry <b>entryPath</b> exists, be it
			 * a file, a symbolic link, a directory, a character or block
			 * device, a FIFO, a socket, etc.
			 *
			 * @note The specified entry will searched through the full search
			 * path. Entries that are symlinks are ignored, unless the symlink
			 * status has been set to true.
			 *
			 * @param entryPath the path of the entry to look-up, in
			 * platform-independent notation.
			 *
			 * @return true iff the entry exists.
			 *
			 * @throw EntryLookupFailed if the operation failed (existence test
			 * failed with no answer) or is not supported on this platform.
			 *
			 */
			virtual bool existsAsEntry( const std::string & entryPath ) const ;



			/**
			 * Not supported on embedded filesystems.
			 *
			 * @throw SymlinkFailed in all cases.
			 *
			 */
			virtual void createSymbolicLink( const std::string & linkTarget,
				const std::string & linkName ) ;



			/**
			 * Returns the change time time of the entry <b>entryPath</b>, be it
			 * a file, a directory, etc.
			 *
			 * @param entryPath the path of the entry.
			 *
			 * @throw GetChangeTimeFailed if the operation failed or is not
			 * supported.
			 *
			 */
			virtual time_t getEntryChangeTime( const std::string & entryPath ) ;




			// Accessors to FilesystemManager constants.



			/**
			 * Returns the root directory prefix, in the embedded filesystem.
			 *
			 * @example "" here.
			 *
			 */
			virtual const std::string & getRootDirectoryPrefix() const ;



			/**
			 * Returns the directory separator in the embedded filesystem, a
			 * Latin-1 character.
			 *
			 * @example Slash here, i.e. '/'.
			 *
			 */
			virtual Ceylan::Latin1Char getSeparator() const ;



			/*
			 * Already available from FileSystemManager:
			 *   - getSeparatorAsString()
			 *   - getAliasForCurrentDirectory()
			 *   - getAliasForParentDirectory()
			 *
			 */





			// File-related section.



			/**
			 * Returns an EmbeddedFile reference on a newly created file.
			 *
			 * By default, it creates a new file on disk. If the name
			 * corresponds to an already-existing file, it will be truncated and
			 * overwritten.
			 *
			 * @param filename the name of the file.
			 *
			 * @param createFlag the flag describing the creation mode.
			 *
			 * @param permissionFlag the flag describing the requested
			 * permissions. On platforms that do not manage permissions, this
			 * parameter will be ignored.
			 *
			 * @see OpeningFlag, PermissionFlag
			 *
			 * @note embeddedFile helper factory, has to be public but the user
			 * should not use it: File::Create is expected to be used instead,
			 * as it is the cross-platform factory intended for use.
			 *
			 * @throw FileException, including FileCreationFailed if the
			 * operation failed or is not supported on this platform.
			 *
			 */
			virtual Ceylan::System::File & createFile(
				const std::string & filename,
				Ceylan::System::OpeningFlag createFlag =
					Ceylan::System::File::CreateToWriteBinary,
				Ceylan::System::PermissionFlag permissionFlag =
					Ceylan::System::File::OwnerReadWrite ) ;



			/**
			 * Returns a File reference on specified already-existing file,
			 * which will be opened with specified settings.
			 *
			 * @param filename the name of the file.
			 *
			 * @param openFlag the flag describing the opening mode.
			 *
			 * @see OpeningFlag
			 *
			 * @note embeddedFile helper factory, has to be public but the user
			 * should not use it: File::Open is expected to be used instead, as
			 * it is the cross-platform factory intended for use.
			 *
			 * @throw FileException, including FileOpeningFailed if the
			 * operation failed or is not supported on this platform.
			 *
			 */
			virtual Ceylan::System::File & openFile(
				const std::string & filename,
				Ceylan::System::OpeningFlag openFlag
					= Ceylan::System::File::OpenToReadBinary ) ;



			/**
			 * Returns the location in the search path where the specified
			 * filename can be found.
			 *
			 * The file is specified in platform-independent notation.
			 *
			 * The returned filename will be the element of the search path
			 * where the file was found, which may be a directory, or an
			 * archive.
			 *
			 * Even if there are multiple matches in different parts of the
			 * search path, only the first one found is used, just like when
			 * opening a file.
			 *
			 * So, if you look for "maps/level1.map", and C:\\mygame is in your
			 * search path and C:\\mygame\\maps\\level1.map exists, then
			 * "C:\mygame" is returned.
			 *
			 * If any part of a match is a symbolic link, and symlinks have not
			 * been explicitly permitted , then it will be ignored, and the
			 * search for a match will continue.
			 *
			 * If you specify a fake directory that only exists as a mount
			 * point, it will be associated with the first archive mounted
			 * there, even though that directory is not necessarily contained in
			 * a real archive.
			 *
			 * @param filename the filename to look-up.
			 *
			 * This method will work as expected whether the symbolic link
			 * feature is enabled or not.
			 *
			 * @return the full actual location of the specified file.
			 *
			 * @throw FileLookupFailed if the operation failed (existence test
			 * failed with no answer) or is not supported on this platform, or
			 * EmbeddedFileSystemManagerException is the file was not found.
			 *
			 */
			virtual std::string getActualLocationFor(
				const std::string & filename ) const ;



			/**
			 * Determines if the first occurence of <b>filename</b> in the
			 * search path is a symbolic link ( directory entry.
			 *
			 * @param directoryPath the directory path to look-up, in
			 * platform-independent notation.
			 *
			 * @throw DirectoryLookupFailed if the operation failed (existence
			 * test failed with no answer) or is not supported on this platform.
			 *
			 */




			/**
			 *
			 * Tells whether the regular file or symbolic link <b>filename</b>
			 * exists (and is not a directory).
			 *
			 * @param filename the filename to look-up.
			 *
			 * This method will work as expected whether the symbolic link
			 * feature is enabled or not.
			 *
			 * @throw FileException, including FileLookupFailed if the operation
			 * failed (existence test failed with no answer) or is not supported
			 * on this platform, or FileDelegatingException if the relevant
			 * filesystem manager could not be retrieved.
			 *
			 * @note With embedded filesystems, one may use instead either
			 * getActualLocationFor or existsAsSymbolicLink.
			 *
			 * @throw FileLookupFailed in all cases.
			 *
			 */
			virtual bool existsAsFileOrSymbolicLink(
				const std::string & filename ) const ;



			/**
			 * Tells whether the symbolic link <b>filename</b> exists.
			 *
			 * @param linkName the filename to look-up.
			 *
			 * This method will work as expected whether the symbolic link
			 * feature is enabled or not.
			 *
			 * @throw FileLookupFailed if the operation failed (existence test
			 * failed with no answer) or is not supported on this platform.
			 *
			 */
			virtual bool existsAsSymbolicLink(
				const std::string & linkName ) const ;



			/**
			 * Removes the file or symbolic link from the filesystem.
			 *
			 * @param filename the filename to remove, specified in
			 * platform-independent notation in relation to the write directory.
			 *
			 * @throw FileRemoveFailed if the operation failed or is not
			 * supported on this platform.
			 *
			 * So if the write directory is set to "C:\mygame\writedir" and
			 * filename to remove is "downloads/maps/level1.map", then the file
			 * "C:\mygame\writedir\downloads\maps\level1.map" is removed from
			 * the physical filesystem, if it exists and the operating system
			 * permits the deletion.
			 *
			 * @note On Unix systems, deleting a file may be successful, but the
			 * actual file will not be removed until all processes that have an
			 * open filehandle to it (including your program) close their
			 * handles.
			 *
			 * Chances are, the bits that make up the file still exist, they are
			 * just made available to be written over at a later point.  Do not
			 * consider this a security method.
			 *
			 */
			virtual void removeFile( const std::string & filename ) ;



			/**
			 * Moves the file on filesystem.
			 *
			 * A special case of file moving is file renaming.
			 *
			 * @param sourceFilename the filename of the file to be moved.
			 *
			 * @param targetFilename the target filename of the moved file.
			 *
			 * @note Operation not supported on embedded files.
			 *
			 * @throw FileMoveFailed in all cases.
			 *
			 */
			virtual void moveFile( const std::string & sourceFilename,
				const std::string & targetFilename ) ;



			/**
			 * Copies the file on filesystem.
			 *
			 * @param sourceFilename the filename of the file to be copied.
			 *
			 * @param targetFilename the new filename of the copied file.
			 *
			 * @throw FileCopyFailed if the operation failed or is not supported
			 * on this platform.
			 *
			 */
			virtual void copyFile( const std::string & sourceFilename,
				const std::string & targetFilename ) ;



			/**
			 * Returns the size, in bytes, of the specified file.
			 *
			 * @param filename the filename whose size is searched.
			 *
			 * @note The file size may not be determined (ex: since the archive
			 * is "streamed").
			 *
			 * Also note that if another process/thread is writing to this file
			 * at the same time, then the information this function supplies
			 * could be incorrect before you get it.
			 *
			 * Use with caution, or better yet, do not use at all.
			 *
			 * @throw FileSizeRequestFailed if the operation failed (ex: file
			 * not found, size could not be determined, etc.) or is not
			 * supported on this platform.
			 *
			 */
			virtual Ceylan::System::Size
				getSize( const std::string & filename ) ;



			/**
			 * Returns the last change time of the specified file.
			 *
			 * @param filename the filename whose last change time is searched,
			 * in platform-independent notation.
			 *
			 * @return The modification time is returned as a number of seconds
			 * since the epoch (Jan 1, 1970).
			 *
			 * @note The exact derivation and accuracy of this time depends on
			 * the particular archiver.
			 *
			 * @throw FileLastChangeTimeRequestFailed if the operation failed,
			 * if there is no reasonable way to obtain this information for a
			 * particular archiver, or if it is not supported on this platform.
			 *
			 */
			virtual time_t getLastChangeTimeFile(
				const std::string & filename ) ;



			// transformIntoValidFilename inherited from File.



			/**
			 * Updates the last access and modification times of specified file.
			 *
			 * This is not expected to work for directories.
			 *
			 * @param filename the filename of the file whose times must be
			 * updated.
			 *
			 * @note On contrary to the UNIX command touch, if the specified
			 * file does not exist, it will not be created. A FileTouchFailed
			 * exception would be raised instead.
			 *
			 * @see File::Create to create empty files.
			 *
			 * @note Not supported on embedded files.
			 *
			 * @throw FileTouchFailed in all cases.
			 *
			 */
			virtual void touch( const std::string & filename ) ;



			// diff directly inherited from File.



			/**
			 * Enables or disables following of symbolic links.
			 *
			 * Some physical filesystems and archives contain files that are
			 * just pointers to other files.
			 *
			 * On the physical filesystem, opening such a link will
			 * (transparently) open the file that is pointed to.
			 *
			 * By default, a file will be checked to know whether it is really a
			 * symlink during open calls and, the operation will fail if it is.
			 *
			 * Otherwise, the link could take you outside the write and search
			 * paths, and compromise security.
			 *
			 * If you want to take that risk, call this function with a
			 * parameter set to true.
			 *
			 * @note Not following links is mainly in order to sandbox a
			 * program's scripting language, in case untrusted scripts try to
			 * compromise the system.
			 *
			 * Generally speaking, a user could very well have a legitimate
			 * reason to set up a symlink.
			 *
			 * Symlinks are only explicitly checked when dealing with filenames
			 * in platform-independent notation.
			 *
			 * That is, when setting up your search and write paths, etc.,
			 * symlinks are never checked for.
			 *
			 * Symbolic link permission can be enabled or disabled at any time,
			 * and is disabled by default.
			 *
			 * @param newStatus if true permits symlinks, if false denies
			 * linking.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation
			 * failed.
			 *
			 */
			virtual void allowSymbolicFiles( bool newStatus ) ;


			/**
			 * Returns a textual description of the files currently opened
			 * through this filesystem manager.
			 *
			 */
			std::string listOpenFiles() const ;




			// Directory-related section.


			// Factory-related subsection.



			/**
			 * Returns a Directory reference on a directory newly created on
			 * filesystem.
			 *
			 * @param newDirectoryName the name of the directory to create.
			 *
			 * @note embeddedDirectory helper factory, has to be public but the
			 * user should not use it: Directory::Create is expected to be used
			 * instead, as it is the cross-platform factory intended for use.
			 *
			 * @throw DirectoryException, including DirectoryCreationFailed if
			 * the directory creation failed.
			 *
			 */
			virtual Ceylan::System::Directory & createDirectory(
				const std::string & newDirectoryName ) ;



			/**
			 * Returns a Directory reference on specified already-existing
			 * directory, which will be "opened" (i.e. referred to).
			 *
			 * @param directoryName the name of the directory. If not specified
			 * (the string is empty), returns a reference to the current working
			 * directory.
			 *
			 * @note embeddedDirectory helper factory, has to be public but the
			 * user should not use it: Directory::Open is expected to be used
			 * instead, as it is the cross-platform factory intended for use.
			 *
			 * @throw DirectoryException, including DirectoryOpeningFailed if
			 * the directory opening failed.
			 *
			 */
			virtual Ceylan::System::Directory & openDirectory(
				const std::string & directoryName = "" ) ;



			/**
			 * Determines if the first occurence of <b>directoryPath</b> in the
			 * search path is really a directory entry.
			 *
			 * @param directoryPath the directory path to look-up, in
			 * platform-independent notation.
			 *
			 * @throw DirectoryLookupFailed if the operation failed (existence
			 * test failed with no answer) or is not supported on this platform.
			 *
			 */
			virtual bool existsAsDirectory(
				const std::string & directoryPath ) const ;



			/**
			 * Removes the directory from filesystem.
			 *
			 * @param directoryPath the path of the target directory, specified
			 * in platform-independent notation in relation to the write
			 * directory.
			 *
			 * @param recursive must be false, and the specified directory is
			 * expected to be empty. It will be removed. No automatic file
			 * deletion is offered, since the files in that directory could be
			 * eclipsed by identically named files found sooner in the search
			 * path.
			 *
			 * @throw DirectoryRemoveFailed if the operation failed or is not
			 * supported.
			 *
			 */
			virtual void removeDirectory( const std::string & directoryPath,
				bool recursive = false ) ;



			/**
			 * Moves the directory on filesystem.
			 *
			 * A special case of directory moving is directory renaming.
			 *
			 * @param sourceDirectoryPath the path of the directory to be moved.
			 *
			 * @param targetDirectoryPath the path of the target directory.
			 *
			 * @throw DirectoryMoveFailed if the operation failed or is not
			 * supported on this platform.
			 *
			 */
			virtual void moveDirectory(
				const std::string & sourceDirectoryPath,
				const std::string & targetDirectoryPath ) ;



			/**
			 * Copies the file on filesystem.
			 *
			 * @param sourceDirectoryPath the path of the directory to be
			 * copied.
			 *
			 * @param targetDirectoryPath the path of the target directory.
			 *
			 * @throw DirectoryCopyFailed if the operation failed or is not
			 * supported on this platform.
			 *
			 */
			virtual void copyDirectory(
				const std::string & sourceDirectoryPath,
				const std::string & targetDirectoryPath ) ;



			/**
			 * Returns the last change time of the specified directory.
			 *
			 * @param directoryPath the path of the directory whose last change
			 * time is searched.
			 *
			 * @throw DirectoryLastChangeTimeRequestFailed if the operation
			 * failed or is not supported on this platform.
			 *
			 */
			virtual time_t getLastChangeTimeDirectory(
				const std::string & directoryPath ) ;



			/**
			 * Returns whether specified string is a valid directory path
			 * (i.e. checks the name can be used, does not look-up any real
			 * filesystem entry).
			 *
			 * @param directoryString the directory string to examine.
			 *
			 * @note If no regular expression support is available, then the
			 * path will be deemed always correct.
			 *
			 */
			virtual bool isAValidDirectoryPath(
				const std::string & directoryString ) ;



			// removeLeadingSeparator inherited.



			/**
			 * Tells whether specified path is an absolute path.
			 *
			 * @param path the path that may be absolute.
			 *
			 */
			virtual bool isAbsolutePath( const std::string & path ) ;



			/**
			 * Returns the current working directory path.
			 *
			 * @return the path associated to the process, knowing that for
			 * embedded filesystems read and write paths can be different.
			 *
			 * @throw DirectoryGetCurrentFailed if the operation failed or is
			 * not supported on the target platform.
			 *
			 */
			virtual std::string getCurrentWorkingDirectoryPath() ;



			/**
			 * Changes current working directory to <b>newWorkingDirectory</b>.
			 *
			 * @param newWorkingDirectory the target working directory.
			 *
			 * @throw DirectoryChangeFailed if the operation failed or is not
			 * supported on the target platform.
			 *
			 */
			virtual void changeWorkingDirectory(
				const std::string & newWorkingDirectory ) ;



			/*
			 * Already available from FileSystemManager:
			 *   - splitPath
			 *   - joinPath (both)
			 *   - stripFilename
			 *
			 */



			// EmbeddedFileSystemManager own section.



			/**
			 * Returns the textual description of the latest error that
			 * occurred.
			 *
			 */
			static std::string GetBackendLastError() ;



			/**
			 * Returns the XOR byte that should be used for XOR'd reads and
			 * writes, should cyphering be enabled.
			 *
			 */
			static Ceylan::Byte GetXORByte() ;



			/**
			 * Returns a user-friendly description of the state of this object.
			 *
			 * @param level the requested verbosity level.
			 *
			 * @note Text output format is determined from overall settings.
			 *
			 * @see TextDisplayable
			 *
			 */
			virtual const std::string toString(
				Ceylan::VerbosityLevels level = Ceylan::high ) const ;






			// Static section to handle embedded filesystem manager.



			/**
			 * Returns a reference to the unique embedded filesystem manager.
			 *
			 * @param cypher file reads and writes will be cyphered iff true.
			 *
			 * Does not set this filesystem manager as the default one.
			 *
			 * Creates it if needed: this method ensures it remains a singleton.
			 *
			 * Must be public, as ancestor has to be able to call it.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation
			 * failed.
			 *
			 */
			static EmbeddedFileSystemManager &
				GetEmbeddedFileSystemManager( bool cypher = true ) ;



			/**
			 * Ensures that the embedded filesystem manager is actually
			 * available, by instanciating it if necessary, and tells whether
			 * cyphering is enabled.
			 *
			 * Does not set this filesystem manager as the default one.
			 *
			 * Creates it if needed: this method ensures it remains a singleton.
			 *
			 * Useful to ensure a static operation can rely on a preexisting
			 * manager.
			 *
			 * @return true iff I/O file cyphering is enabled.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation
			 * failed.
			 *
			 */
			static bool SecureEmbeddedFileSystemManager() ;



			/**
			 * Removes the current embedded filesystem manager, if any.
			 *
			 * Must be public, as ancestor has to be able to call it.
			 *
			 * @note Removing such manager, if it was set as the default one,
			 * will remove it as well.
			 *
			 */
			static void RemoveEmbeddedFileSystemManager() ;



			/**
			 * Constructs a reference to an Embedded filesystem, initializes
			 * accordingly any needed subsystem.
			 *
			 * Cannot be private, as has to be subclassed.
			 *
			 * Cannot be protected, as has to be instanciated by factories from
			 * the mother class.
			 *
			 * @param cypher if true, writes to files will be cyphered, and
			 * obviously reads will be decyphered, so that the content of
			 * written files will not be directly readable. If false, raw I/O
			 * will be performed, with no further transformation.
			 *
			 * @param trackOpenedFiles tells whether opened files should be
			 * tracked.
			 *
			 * @note Not to be called by the user.
			 *
			 * @see GetEmbeddedFileSystemManager instead.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation
			 * failed.
			 *
			 */
			explicit EmbeddedFileSystemManager( bool cypher = true,
			  bool trackOpenedFiles = true ) ;



			/**
			 * Destroys the Ceylan embedded filesystem reference, not the
			 * filesystem itself.
			 *
			 * Cannot be private as has to be subclassed.
			 *
			 */
			virtual ~EmbeddedFileSystemManager() throw() ;




			// Archive locator section.



			/**
			 * The name of the environment variable that may contain directory
			 * names that should contain embedded archive files.
			 *
			 */
			static std::string ArchivePathEnvironmentVariable  ;



			/**
			 * Allows to keep track of embedded archive directories.
			 *
			 * Automatically gathers the list of directories specified as the
			 * value of the environment variable named as specified in
			 * ArchivePathEnvironmentVariable (ARCHIVE_PATH).
			 *
			 */
			static Ceylan::System::FileLocator ArchiveFileLocator ;



			/**
			 * Helper method to find an OSDL archive specified by its filename,
			 * using archive locators.
			 *
			 * @note Starts by searching the current directory, before using the
			 * locator paths.
			 *
			 * @param archiveFilename the filename of the archive file.
			 *
			 * @throw EmbeddedFileSystemManagerException if the operation
			 * failed, including if the archive could not be found despite
			 * archive path locator.
			 *
			 */
			static std::string FindArchivePath(
				const std::string & archiveFilename ) ;




		private:


			/**
			 * Tells whether writes should be cyphered, and reads decyphered.
			 *
			 */
			bool _cypher ;



			/**
			 * The byte that would be used for one cypher-pass against read and
			 * written bytes.
			 *
			 */
			static const Ceylan::Byte XORByte ;




			// Directory constants.



			/**
			 * Root directory prefix.
			 *
			 * @example "" on Unix, "c:" on Windows.
			 *
			 */
			static const std::string RootDirectoryPrefix ;


			/**
			 * Directory separator.
			 *
			 * @example Slash or backslash.
			 *
			 */
			static const Ceylan::Latin1Char Separator ;



			/**
			 * Copy constructor made private to ensure that it will be never
			 * called.
			 *
			 * The compiler should complain whenever this undefined constructor
			 * is called, implicitly or not.
			 *
			 */
			EmbeddedFileSystemManager(
				const EmbeddedFileSystemManager & source ) ;


			/**
			 * Assignment operator made private to ensure that it will be never
			 * called.
			 *
			 * The compiler should complain whenever this undefined operator is
			 * called, implicitly or not.
			 *
			 */
			EmbeddedFileSystemManager & operator = (
				const EmbeddedFileSystemManager & source ) ;




			/// Pointer to the current embedded filesystem manager (if any).
			static EmbeddedFileSystemManager * _EmbeddedFileSystemManager ;



	} ;


}



#endif // OSDL_EMBEDDED_FILE_SYSTEM_MANAGER_H_

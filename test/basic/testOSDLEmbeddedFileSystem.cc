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


#include "OSDL.h"
using namespace OSDL ;

using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace std ;



/**
 * Test of the embedded filesystem layer provided by the OSDL basic module.
 *
 * @note Corresponds to the test of EmbeddedDirectory, EmbeddedFile and
 * EmbeddedFileSystemManager.
 *
 * @see the create-testOSDLEmbeddedFileSystem-archive.sh script, to generate
 * a test archive for this test.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	

    try 
	{

    	LogPlug::info( "Testing OSDL services for embedded filesystems." ) ;
	
		// No need to start general OSDL services:
        EmbeddedFileSystemManager & myFSManager = 		
        	EmbeddedFileSystemManager::GetEmbeddedFileSystemManager() ;
       
        LogPlug::info( "Selecting basic embedded settings." ) ;
        
        /*
         * Note this does not imply that EmbeddedFileSystemManager became
         * the default filesystem manager: it is still the standard one
         * (luckly for the log files to remain readable).
         *
         */
         
		myFSManager.chooseBasicSettings( /* organization name */ "OSDL",
            /* application name */ "testOSDLEmbeddedFileSystem" ) ;


        LogPlug::info( "Not loading any archive yet, "
			"as only testing write services." ) ;
        
		
        /*
         * If create-testOSDLEmbeddedFileSystem-archive.sh has been run,
         * test-OSDLEmbeddedFileSystem-archive.oar should be listed in the
         * search path:
         *
         */
        list<string> searchPath = myFSManager.getSearchPath() ;
        
        LogPlug::info( "Search path is: " 
        	+ Ceylan::formatStringList( searchPath ) ) ;
          
		LogPlug::info( "Write directory is: " 
        	+ myFSManager.getWriteDirectory() ) ;
            
        const string firstFilename = "test-write-file.txt" ;
        
        LogPlug::info( "Creating file '" + firstFilename 
        	+ "' at the root of write directory." ) ;
            
        /*
         * This filesystem is not set as the default, thus using it directly
         * here instead of File::Create.
         * Note the result is used as a generic file, regardless of its
         * creator.
         */
        Ceylan::System::File & myFile = 
        	myFSManager.createFile( firstFilename ) ;
  		
        string writtenContent = "Hello sandboxed embedded world!" ;
        Size writtenContentSize = writtenContent.size() ;
        
  		myFile.write( writtenContent ) ;
        
        delete & myFile ;
        
        
        // Check the writing and reading works in par:
        
        Ceylan::Byte * readBuffer = new Ceylan::Byte[writtenContentSize+1] ;
        
        Ceylan::System::File & myReadFile = 
        	myFSManager.openFile( firstFilename ) ;
        
		myReadFile.read( readBuffer, writtenContentSize ) ;
        
        myReadFile.close() ;
        readBuffer[writtenContentSize] = 0 ;
        
        string readContent( readBuffer ) ;
        
        if ( readContent == writtenContent )
        	LogPlug::info( 
            	"Read correctly exactly the same content as written." ) ;
        else
        	throw TestException( "Could not read the same content as written: "
            	"written '" + writtenContent + "', read '" + readContent 
                + "'." ) ;      
        
        delete [] readBuffer ;
        
        
		const string directoryName = "test-create-directory" ;
        LogPlug::info( "Creating directory '" + directoryName 
        	+ "' at the root of write directory." ) ;

		/*
         * Note that it is the embedded filesystem manager (myFSManager)
         * not the standard (native) that must be used, as platform-independent
         * paths are expected here:
         *
         */
		Ceylan::System::Directory & myDirectory = 
        	myFSManager.createDirectory( directoryName ) ;
        
        Ceylan::System::File & myOtherFile = myFSManager.createFile( 
			myFSManager.joinPath( myDirectory.getPath(), firstFilename ) ) ;
             
        myOtherFile.write( "In a test directory." ) ;
        
        delete & myOtherFile ;
        
		
		LogPlug::info( "Now testing reading from archives services." ) ;
		
		const string archiveFilename = 
        	"test-OSDLEmbeddedFileSystem-archive.oar" ;
     
        // This is (implicitly) the standard filesystem manager here:
        if ( ! File::ExistsAsFileOrSymbolicLink( archiveFilename ) )
        {
        
        	LogPlug::warning( "Test archive '" + archiveFilename 
            	+ "' not found, run the "
                "create-testOSDLEmbeddedFileSystem-archive.sh script "
                "beforehand to have it ready for this test. Stopping now." ) ;
                
        	return 0 ;
            
        }

		LogPlug::info( "Test archive '" + archiveFilename 
        	+ "' found, mounting it." ) ;
        
        /*
         * Here it is actually useless, as "oar" was (implicitly) specified for
         * the chooseBasicSettings call (due to the default parameter), thus
         * the archives with this extension in default paths have already been
         * mounted: 
         *
         */
 		myFSManager.mount( archiveFilename ) ;

		searchPath = myFSManager.getSearchPath() ;
        
        LogPlug::info( "Newer search path is: " 
        	+ Ceylan::formatStringList( searchPath ) ) ;
        
        // Platform-independent notation:   
		const string targetArchivedFilename =
        	"test-OSDLEmbeddedFileSystem-archive/first-file-to-read.txt" ;
       
        LogPlug::info( "Trying to load archive '" + targetArchivedFilename 
        	+ "', expected to be found in OSDL test archive." ) ;
        
        if ( ! myFSManager.existsAsEntry( targetArchivedFilename ) )
        	throw TestException( "File '" + targetArchivedFilename 
            	+ "' could not be found in archive." ) ;
        else
        	LogPlug::info( "File '" + targetArchivedFilename 
            	+ "' found in archive." ) ;   
        
        File & otherReadFile = myFSManager.openFile( targetArchivedFilename ) ;
                
        LogPlug::info( "Size of '" + targetArchivedFilename + "' is " 
        	+ Ceylan::toString( otherReadFile.size() ) + " bytes." ) ;
        
        const Size BufSize = 30 ;
        char buffer[BufSize] ;
        for ( unsigned int i=0; i<BufSize; i++ )
        	buffer[i]=0 ;
            
        Size read = otherReadFile.read( buffer, BufSize ) ;

        LogPlug::info( "Read " + Ceylan::toString( read ) + " bytes." ) ;
        
        string readString = string( buffer ) ;

        LogPlug::info( "Content of '" + targetArchivedFilename + "' is '" 
        	+ readString + "'." ) ;
        
        const string toRead = "First test file.\n" ;
        
        if ( readString != toRead )
       		throw TestException( "Read '" +  readString
            	+ "' instead of '" + toRead + "'." ) ;
        
        LogPlug::info( "Using tell to know current position: "
        	+ Ceylan::toString( otherReadFile.tell() ) );
        
		LogPlug::info( "Seeking seventh position (the 'e' of test)" ) ; 
        otherReadFile.seek( 7 ) ;
        otherReadFile.read( buffer, 1 ) ;
        
        if ( buffer[0] != 'e' ) 
      		throw TestException( "Could not seek and read test position #1." ) ;
        else
        	LogPlug::info( "Could seek and read test position." ) ;   
         	
        if ( otherReadFile.tell() != 8 )
      		throw TestException( "Could not tell position #1." ) ;
        else
        	LogPlug::info( "Could tell test position." ) ;   

 		myFSManager.umount( archiveFilename ) ;

 		delete & myFSManager ;
		        
		LogPlug::info( "End of OSDL embedded filesystem test." ) ;
 
 
    }
	
    catch ( const OSDL::Exception & e )
    {
	
        LogPlug::error( "OSDL exception caught: " 
			+ e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught: " 
			+ e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught: " 
			+ std::string( e.what() ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
	
        LogPlug::error( "Unknown exception caught" ) ;
       	return Ceylan::ExitFailure ;

    }

    return Ceylan::ExitSuccess ;

}


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
         * the default filesystem manager: it is still the standard one.
         *
         */
         
		myFSManager.chooseBasicSettings( /* organization name */ "OSDL",
            /* application name */ "testOSDLEmbeddedFileSystem" ) ;
        
        /*
         * If create-testOSDLEmbeddedFileSystem-archive.sh has been run,
         * test-OSDLEmbeddedFileSystem-archive.oar should be listed in the
         * search path:
         *
         */
        list<string> searchPath = myFSManager.getSearchPath() ;
        
        LogPlug::info( "Search path is: " 
        	+ Ceylan::formatStringList( searchPath ) ) ;
          
        const string writeDir = "my-test-write-directory" ;
            
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
  
  		myFile.write( "Hello sandboxed embedded world!" ) ;
        
        delete & myFile ;

		const string directoryName = "test-create-directory" ;
        LogPlug::info( "Creating directory '" + directoryName 
        	+ "' at the root of write directory." ) ;

		/*
         * Note that its is the embedded filesystem manager (myFSManager)
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
        
        File & myReadFile = myFSManager.openFile( targetArchivedFilename ) ;
                
        LogPlug::info( "Size of '" + targetArchivedFilename + "' is " 
        	+ Ceylan::toString( myReadFile.size() ) + " bytes." ) ;
        
        const Size BufSize = 30 ;
        char buffer[BufSize] ;
        for ( unsigned int i=0; i<BufSize; i++ )
        	buffer[i]=0 ;
            
        Size read = myReadFile.read( buffer, BufSize ) ;

        LogPlug::info( "Read " + Ceylan::toString( read ) + " bytes." ) ;
        
        string readString = string( buffer ) ;

        LogPlug::info( "Content of '" + targetArchivedFilename + "' is '" 
        	+ readString + "'." ) ;
        
        const string toRead = "First test file.\n" ;
        
        if ( readString != toRead )
       		throw TestException( "Read '" +  readString
            	+ "' instead of '" + toRead + "'." ) ;
        
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

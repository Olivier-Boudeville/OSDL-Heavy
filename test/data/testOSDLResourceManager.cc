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
using namespace OSDL::Data ;
using namespace OSDL::Audio ;
using namespace OSDL::Video ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

#include <string>
using std::string ;





/**
 * Testing OSDL Resource management services.
 *
 * @note the create-testOSDLEmbeddedFileSystem-archive.sh script should have
 * be run from the test/basic directory, so that this test is able to load 
 * the corresponding archive.
 *
 * See Ceylan test (generic/testCeylanCountedPointer.cc) for more details
 * about reference counting.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{ 

		
		LogPlug::info( "Testing OSDL resource management." ) ;


		bool isBatch = false ;
		
		std::string executableName ;
		std::list<std::string> options ;
		
		Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;
		
		std::string token ;
		bool tokenEaten ;
		
		
		while ( ! options.empty() )
		{
		
			token = options.front() ;
			options.pop_front() ;

			tokenEaten = false ;
						
			if ( token == "--batch" )
			{
			
				LogPlug::info( "Batch mode selected" ) ;
				isBatch = true ;
				tokenEaten = true ;
			}
			
			if ( token == "--interactive" )
			{
				LogPlug::info( "Interactive mode selected" ) ;
				isBatch = false ;
				tokenEaten = true ;
			}
			
			if ( token == "--online" )
			{
				// Ignored:
				tokenEaten = true ;
			}
			
			if ( LogHolder::IsAKnownPlugOption( token ) )
			{
				// Ignores log-related (argument-less) options.
				tokenEaten = true ;
			}
			
			
			if ( ! tokenEaten )
			{
				throw Ceylan::CommandLineParseException( 
					"Unexpected command line argument: " + token ) ;
			}
		
		}
		
		
		string resourceFilename =
			"../basic/test-OSDLEmbeddedFileSystem-archive.oar" ;
		
		LogPlug::info( "Loading test archive '" + resourceFilename + "'." ) ;	

		
		/*
		 * Needing to start general OSDL services, as we will for example load
		 * musics:
		 *
		 */
			
       getCommonModule( CommonModule::UseVideo | CommonModule::UseAudio ) ;		
			


        EmbeddedFileSystemManager & myFSManager = 		
        	EmbeddedFileSystemManager::GetEmbeddedFileSystemManager() ;
		
		myFSManager.chooseBasicSettings( /* organization name */ "OSDL",
            /* application name */ "testOSDLResourceManager" ) ;
			
        // This is (implicitly) the standard filesystem manager here:
        if ( ! File::ExistsAsFileOrSymbolicLink( resourceFilename ) )
        {
        
        	LogPlug::warning( "Test archive '" + resourceFilename 
            	+ "' not found, run the "
                "test/data/create-testOSDLEmbeddedFileSystem-archive.sh script "
                "beforehand to have it ready for this test. Stopping now." ) ;
                
        	return 0 ;
            
        }

		LogPlug::info( "Test archive '" + resourceFilename 
        	+ "' found, mounting it." ) ;

 		myFSManager.mount( resourceFilename ) ;
		
		FileSystemManager::SetDefaultFileSystemManager( myFSManager ) ;
			
		ResourceManager * myResourceManager = new ResourceManager(
			"resource-map.xml" ) ;
		
		LogPlug::info( "Resource manager initial state: " 
			+ myResourceManager->toString() ) ;
			
			
		/* 
		 * For the sake of this test, we know that 'welcome-to-OSDL.music'
		 * has the resource identifier #3.
		 *
		 * Normal use of the resource manager wold just to include the 
		 * automatically generated '../basic/resource-map.h' header file and to
		 * issue a getMusic( ResourceIndex::File_welcome_to_OSDL_music )
		 * (File_welcome_to_OSDL_music is defined as equal to 3, of course).
		 *
		 * We did not do it that way here as for this test we cannot include
		 * this header that may or may not exist.
		 *
		 */
		
		// Currently the music reference count is 1.
		
		{
		
			/*
			 * Defined in a block to force the counter pointer out of scope:
			 * (note: counter pointer should always be passed by value)
			 *
			 */
			Audio::MusicCountedPtr myMusic = myResourceManager->getMusic( 3  ) ;
		
			// Currently the music reference count is 2.
			
			LogPlug::info( "Test obtained following music: '" 
				+ myMusic->toString() + "', with following counter pointer:' "
				+ myMusic.toString() + "'." ) ;
		
			// Here the 
			LogPlug::info( 
				"Resource manager new state with one music reference: " 
				+ myResourceManager->toString() ) ;
				
		}		
		
		
		// Currently the music reference count is 1.
		
		LogPlug::info( 
			"Resource manager new state with no more music reference: "
			+ myResourceManager->toString() ) ;
			
		LogPlug::info( "Music counted pointer just gone out of scope, "
			"now deallocating resource manager." ) ;
					
		delete myResourceManager ;
			
		LogPlug::info( "End of OSDL resource management test." ) ;
		
 		myFSManager.umount( resourceFilename ) ;
		
		FileSystemManager::SetDefaultFileSystemManagerToPlatformDefault() ;

		
		LogPlug::info( "stopping OSDL." ) ;		
        //OSDL::stop() ;
		

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


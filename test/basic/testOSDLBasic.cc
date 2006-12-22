#include "OSDL.h"
using namespace OSDL ;

#include "Ceylan.h"
using namespace Ceylan::Log ;



/**
 * Real test for the basic module.
 *
 * Retrieves OSDL root module.
 *
 */
int main( int argc, char * argv[] ) 
{


	/*
	 * Verifies that the versions of the OSDL header files used to compile this test and 
	 * the one of the currently linked OSDL library are compatible :
	 *
	 */
	CHECK_OSDL_VERSIONS() ;	


	LogHolder myLog( argc, argv ) ;
	

    try 
	{

    	LogPlug::info( "Testing OSDL basic services." ) ;
	

        OSDL::CommonModule & myOSDL = OSDL::getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;
				
		LogPlug::info( myOSDL.toString() ) ;
		
		LogPlug::info( "This test has been compiled against the "
			+ Ceylan::Version( COMPILED_WITH_OSDL_VERSION ).toString() 
			+ " version of the OSDL header files." ) ;
		
		LogPlug::debug( "One can check that all these constants, except SDL_INIT_EVERYTHING, "
			"have no two same bit set to one :" ) ;
		
		// Use the default plug (not the HTML one) to have the number vertically aligned :
		
		LogPlug::debug( "SDL backend SDL_INIT_TIMER is equal to       : " 
			+ Ceylan::toString( SDL_INIT_TIMER, /* bitfield */ true ) ) ;
					
		LogPlug::debug( "SDL backend SDL_INIT_AUDIO is equal to       : " 
			+ Ceylan::toString( SDL_INIT_AUDIO, /* bitfield */ true ) ) ;
					
		LogPlug::debug( "SDL backend SDL_INIT_VIDEO is equal to       : " 
			+ Ceylan::toString( SDL_INIT_VIDEO, /* bitfield */ true ) ) ;
					
		LogPlug::debug( "SDL backend SDL_INIT_CDROM is equal to       : " 
			+ Ceylan::toString( SDL_INIT_CDROM, /* bitfield */ true ) ) ;
					
		LogPlug::debug( "SDL backend SDL_INIT_JOYSTICK is equal to    : " 
			+ Ceylan::toString( SDL_INIT_JOYSTICK, /* bitfield */ true ) ) ;
					
		LogPlug::debug( "OSDL UseKeyboard is equal to                 : " 
			+ Ceylan::toString( CommonModule::UseKeyboard, /* bitfield */ true ) ) ;
					
		LogPlug::debug( "OSDL UseMouse is equal to                    : " 
			+ Ceylan::toString( CommonModule::UseMouse, /* bitfield */ true ) ) ;
					
		LogPlug::debug( "SDL backend SDL_INIT_EVERYTHING is equal to  : " 
			+ Ceylan::toString( SDL_INIT_EVERYTHING, /* bitfield */ true ) ) ;
					
		LogPlug::debug( "SDL backend SDL_INIT_NOPARACHUTE is equal to : " 
			+ Ceylan::toString( SDL_INIT_NOPARACHUTE, /* bitfield */ true ) ) ;
					
		LogPlug::debug( "SDL backend SDL_INIT_EVENTTHREAD is equal to : " 
			+ Ceylan::toString( SDL_INIT_EVENTTHREAD, /* bitfield */ true ) ) ;
		
		LogPlug::debug( "UseKeyboard = " + Ceylan::toHexString( CommonModule::UseKeyboard ) ) ;
		LogPlug::debug( "UseMouse    = " + Ceylan::toHexString( CommonModule::UseMouse ) ) ;
					
					
        OSDL::stop() ;

		LogPlug::info( "End of OSDL basic test." ) ;
 
    }
	
    catch ( const OSDL::Exception & e )
    {
        LogPlug::error( "OSDL exception caught : " + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const Ceylan::Exception & e )
    {
        LogPlug::error( "Ceylan exception caught : " + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
        LogPlug::error( "Standard exception caught : " + std::string( e.what() ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
        LogPlug::error( "Unknown exception caught" ) ;
       	return Ceylan::ExitFailure ;

    }

    return Ceylan::ExitSuccess ;

}

#include "OSDL.h"
using namespace OSDL ;

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
	 * Verifies that the versions of the OSDL header files used to compile 
	 * this test and the one of the currently linked OSDL library are 
	 * compatible :
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
			+ Ceylan::LibtoolVersion( 
				OSDL::actualOSDLHeaderLibtoolVersion ).toString() 
			+ " version of the OSDL header files." ) ;
		
		LogPlug::debug( "One can check that all these constants, "
			"(except CommonModule::UseEverything), have no bit in common"
			"have no two same bit set to one :" ) ;
		
		
		LogPlug::debug( "CommonModule::UseTimer is equal to : " 
			+ Ceylan::toString( CommonModule::UseTimer, 
				/* bitfield */ true ) ) ;
					
		LogPlug::debug( "CommonModule::UseAudio is equal to       : " 
			+ Ceylan::toString( CommonModule::UseAudio, 
				/* bitfield */ true ) ) ;
					
		LogPlug::debug( "CommonModule::UseVideo is equal to       : " 
			+ Ceylan::toString( CommonModule::UseVideo, 
				/* bitfield */ true ) ) ;
					
		LogPlug::debug( "CommonModule::UseCDROM is equal to       : " 
			+ Ceylan::toString( CommonModule::UseCDROM, 
				/* bitfield */ true ) ) ;
					
		LogPlug::debug( "UseJoystick is equal to    : " 
			+ Ceylan::toString( CommonModule::UseJoystick, 
				/* bitfield */ true ) ) ;
					
		LogPlug::debug( "UseKeyboard is equal to                 : " 
			+ Ceylan::toString( CommonModule::UseKeyboard, 
				/* bitfield */ true ) ) ;
					
		LogPlug::debug( "UseMouse is equal to                    : " 
			+ Ceylan::toString( CommonModule::UseMouse, 
				/* bitfield */ true ) ) ;
					
		LogPlug::debug( "UseEverything is equal to  : " 
			+ Ceylan::toString( CommonModule::UseEverything, 
				/* bitfield */ true ) ) ;
					
		LogPlug::debug( "NoParachute is equal to : " 
			+ Ceylan::toString( CommonModule::NoParachute, 
				/* bitfield */ true ) ) ;
					
		LogPlug::debug( "UseEventThread is equal to : " 
			+ Ceylan::toString( CommonModule::UseEventThread, 
				/* bitfield */ true ) ) ;
		
									
        OSDL::stop() ;

		LogPlug::info( "End of OSDL basic test." ) ;
 
 
    }
	
    catch ( const OSDL::Exception & e )
    {
	
        LogPlug::error( "OSDL exception caught : " 
			+ e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught : " 
			+ e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught : " 
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


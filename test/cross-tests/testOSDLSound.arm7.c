

/**
 * ARM7 side of the OSDL sound test.
 *
 * @see testOSDLSound.arm9.cc for the ARM9 peer implementation.
 * 
 */
#include "OSDLARM7Base.h"



/* Example of application-specific command handler */
void handleReceivedApplicationCommand( FIFOCommandID commandID, 
	FIFOElement element )
{

	/*
	 * Here we are dealing with an OSDL-specific command.
	 *
	 * @note Cannot use switch here, as if using:
	 * const int MyConstant = 1 ;
	 * switch ( aValue )
	 * {
	 *   case MyConstant:
	 * etc.
	 *
	 * gcc says: 'case label does not reduce to an integer constant'
	 *
			
	switch ( commandID )
	{
	
			
		default:
			setError( UnexpectedApplicationCommand ) ;
			break ;
				
	}

	*/	 

	setError( UnexpectedApplicationCommand ) ;
	
}



int main(int argc, char ** argv) 
{

	
	initOSDL() ;

	/*
	 * Wait for ever, otherwise the runtime will believe the ROM has crashed.
	 *
	 */
	while( true )
		atomicSleep() ;

}


/**
 * Default OSDL ARM7 do-nothing implementation (ex: with no specific audio
 * support), just performing ARM handshake not to block the ARM9.
 *
 * @note Allows to test ARM9 code with no possible interference of ARM7
 * behaviour.
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
	{

		atomicSleep() ;

	}

}

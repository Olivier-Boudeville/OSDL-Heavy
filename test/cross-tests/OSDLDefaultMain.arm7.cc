
/*
 * Apparently the overhead due to the C++, to the STL and (marginally) to
 * the Ceylan library itself leads to having a ARM7 executable too big to
 * fit in its IWRAM.
 *
 * Hence for the moment the inclusion of the Ceylan header is commented out
 * and libnds is directly used instead.
 *
 */

#define USE_CEYLAN 0 

#if USE_CEYLAN

#include "Ceylan.h"

#else // USE_CEYLAN

#define ARM7
#include "nds.h"

#endif // USE_CEYLAN


/*
 * Directly obtained from libnds ARM7 template.
 * This is a stripped-down version (no sound, VBLANK kept for safety).
 *
 * @see ${libnds-sources}/basicARM7/source/defaultARM7.c
 *
 */


touchPosition first, tempPos ;


void VcountHandler() 
{

	// Updates the button state and the touchscreen:
	
	static int lastbut = -1 ;
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0 ;

	but = REG_KEYXY ;

	if ( ! ( (but ^ lastbut) & (1<<6) ) ) 
	{
 
		tempPos = touchReadXY() ;

		if ( tempPos.x == 0 || tempPos.y == 0 ) 
		{
		
			but |= (1 <<6) ;
			lastbut = but ;
			
		} 
		else 
		{
		
			x   = tempPos.x ;
			y   = tempPos.y ;
			xpx = tempPos.px ;
			ypx = tempPos.py ;
			z1  = tempPos.z1 ;
			z2  = tempPos.z2 ;
			
		}
		
	} 
	else 
	{
		lastbut = but ;
		but |= (1 <<6) ;
	}

	IPC->touchX	  = x ;
	IPC->touchY	  = y ;
	IPC->touchXpx = xpx ;
	IPC->touchYpx = ypx ;
	IPC->touchZ1  = z1 ;
	IPC->touchZ2  = z2 ;
	IPC->buttons  = but ;

}


// Empty, but kept to be able to wait for its interrupt. 
void VblankHandler() 
{


}



int main(int argc, char ** argv) 
{
	
	// Reset the clock if needed :
	rtcReset() ;
	
	irqInit() ;

	irqSet( IRQ_VBLANK, VblankHandler ) ;

	SetYtrigger( 80 ) ;
	irqSet( IRQ_VCOUNT, VcountHandler ) ;

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT ) ;

	IPC->mailBusy = 0 ;

	while ( true )
		swiWaitForVBlank() ;
		
}



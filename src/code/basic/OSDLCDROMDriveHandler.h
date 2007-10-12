#ifndef OSDL_CD_ROM_DRIVE_HANDLER_H_
#define OSDL_CD_ROM_DRIVE_HANDLER_H_


#include "OSDLException.h"   // for OSDL::Exception

#include "Ceylan.h"          // for inheritance


#include <string>
#include <map>


namespace OSDL
{

	
	
	/// Exception returned whenever an error concerning a CD-ROM drive occured.
	class OSDL_DLL CDROMDriveException: public OSDL::Exception
	{
	
		public:
			
			CDROMDriveException( const std::string & message ) throw() ;
			virtual ~CDROMDriveException() throw() ;
	
	} ;
	
	
	
	/**
	 * CD-ROM drive numbers are mainly used to identify a CD-ROM drive. 
	 * They range from 0 (included) to n, 0 being the default drive, if any.
	 *
	 */
	typedef Ceylan::Count CDROMDriveNumber ;
	
	
	// CD-ROM handler manages CD-ROM drives.
	class CDROMDrive ;
	
	
	/**
	 * A CD-ROM drive handler manages all CD-ROM device attached to the 
	 * system, so they can be used easily and safely, by ensuring at most 
	 * one CD-ROM drive object is associated to an actual device.
	 *
	 */
	class OSDL_DLL CDROMDriveHandler: public Ceylan::Object
	{
	
		
		public:
		
		
			/**
			 * Handler constructor. No CD-ROM drive specifically opened 
			 * by this operation.
			 *
			 * @throw CDROMDriveException if the CD-ROM subsystem could 
			 * not be initialized.
			 *
			 */
			CDROMDriveHandler() throw( CDROMDriveException ) ;
			
			
			// Virtual destructor.
			virtual ~CDROMDriveHandler() throw() ;
			
			
			/**
			 * Returns the number of CD-ROM drives attached to the system.
			 *
			 * If there is at least a CD-ROM drive, drive numbers are in
			 * [0,GetCDROMDrivesCount()-1].
			 *
			 */
			static CDROMDriveNumber GetAvailableCDROMDrivesCount() throw() ;
		
			
			/**
			 * Returns a CD-ROM drive object corresponding to the specified
			 * drive number, which will be automatically opened, if not 
			 * already. 
			 * This handler enforces that each actual CD-ROM drive device 
			 * is managed by at most one CDROMDrive object, hence getting
			 * multiple times the same drive number will always return the
			 * same instance.
			 * 
			 * @param number the number of the drive to retrieve, which 
			 * must be within [0;GetCDROMDrivesCount()-1]. The default value
			 * zero designates the default CD-ROM drive.
			 *
			 * @throw CDROMDriveException if specified drive number is not
			 * in licit range.
			 *
			 */		
			CDROMDrive & getCDROMDrive( CDROMDriveNumber number = 0 ) 
				throw( CDROMDriveException ) ;
		
		
	        /**
	         * Returns an user-friendly description of the state of this
			 * drive manager.
	         *
		     * @param level the requested verbosity level.
		     *
		     * @note Text output format is determined from overall settings.
		     *
		     * @see Ceylan::TextDisplayable
	         *
	         */
		    virtual const std::string toString( 
				Ceylan::VerbosityLevels level = Ceylan::high ) const throw() ;

		
			
		private:
	
			
/* 
 * Takes care of the awful issue of Windows DLL with templates.
 *
 * @see Ceylan's developer guide and README-build-for-windows.txt 
 * to understand it, and to be aware of the associated risks. 
 * 
 */
#pragma warning( push )
#pragma warning( disable: 4251 )
			
			/**
			 * Keeps track of drive numbers, so no CD-ROM device can be 
			 * linked to more than one CDROMDrive object.
			 *
			 */
			std::map<CDROMDriveNumber, CDROMDrive *> _drives ;
			
#pragma warning( pop ) 
			

			/**
			 * Copy constructor made private to ensure that it will be 
			 * never called.
			 *
			 * The compiler should complain whenever this undefined 
			 * constructor is called, implicitly or not.
			 * 
			 */			 
			explicit CDROMDriveHandler( const CDROMDriveHandler & source )
				throw() ;
			
			
			/**
			 * Assignment operator made private to ensure that it will 
			 * be never called.
			 *
			 * The compiler should complain whenever this undefined 
			 * operator is called, implicitly or not.
			 * 
			 */			 
			CDROMDriveHandler & operator = ( const CDROMDriveHandler & source )
				throw() ;
			
	
	} ;


}



#endif // OSDL_CD_ROM_DRIVE_HANDLER_H_


#ifndef OSDL_EXCEPTION_H_
#define OSDL_EXCEPTION_H_


#include "Ceylan.h"

#include <string>
#include <iostream>


namespace OSDL
{


	/**
	 * OSDL exception.
	 *
	 * All OSDL exceptions should inherit from it, directly or not.
	 *
	 */
    class Exception : public Ceylan::Exception
    {

        public:
  			
			
			/**
			 * Basic constructor.
			 *
			 * @param reason the message giving details about this 
			 * raised exception.
			 *
			 */
			explicit Exception( const std::string & reason ) throw() ;
			
			
			/// Basic destructor.
			virtual ~Exception() throw() ;


    } ;

}



/**
 * Operator used to display easily an exception's message in an output 
 * stream.
 *
 * The message is the one returned by toString with high level of detail
 * selected.
 *
 * @see toString.
 * 
 */
std::ostream & operator << ( std::ostream & os, const OSDL::Exception & e )
	throw() ;



#endif // OSDL_EXCEPTION_H_

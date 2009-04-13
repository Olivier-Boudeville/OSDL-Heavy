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
    class OSDL_DLL Exception : public Ceylan::Exception
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
OSDL_DLL std::ostream & operator << ( std::ostream & os, 
	const OSDL::Exception & e )	throw() ;



#endif // OSDL_EXCEPTION_H_


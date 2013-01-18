/*
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#ifndef OSDL_TEST_EXCEPTION_H_
#define OSDL_TEST_EXCEPTION_H_



#include "OSDLException.h"

#include <string>
#include <iostream>



namespace OSDL
{


	/// Test class for OSDL::Exception.
	class OSDL_DLL TestException : public Exception
	{

		public:

			explicit TestException( const std::string & reason ) ;

			virtual ~TestException() throw() ;

	} ;


}



OSDL_DLL std::ostream & operator << ( std::ostream & os,
	const OSDL::TestException & e ) ;



#endif // OSDL_TEST_EXCEPTION_H_

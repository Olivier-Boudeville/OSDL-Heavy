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


#ifndef OSDL_GUI_COMMON_H_
#define OSDL_GUI_COMMON_H_


#include "OSDLException.h"  // for inheritance.


#include <string>



namespace OSDL
{


	namespace Rendering 
	{
	
				
		/**
		 * The base GUI view allows to set-up and perform the rendering
		 * of a graphical user interface. 
		 *
		 * @see Renderer
		 *
		 */	
		class OSDL_DLL GUIException : public OSDL::Exception 
		{
				
			GUIException( const std::string & reason ) ;
			
			virtual ~GUIException() throw() ;
			
		} ;
		
		
	}
	
	
}

			
#endif // OSDL_GUI_COMMON_H_


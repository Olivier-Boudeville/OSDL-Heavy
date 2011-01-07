/*
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#ifndef OSDL_BASE_GUI_VIEW_H_
#define OSDL_BASE_GUI_VIEW_H_


//#include "OSDLEvents.h"               // for RenderingTick

//#include "OSDLRenderer.h"             // for inheritance

#include "Ceylan.h"

#include <string>
//#include <list>





namespace OSDL
{



	namespace Rendering
	{



		/**
		 * The base GUI view allows to set-up and perform the rendering of a
		 * graphical user interface.
		 *
		 * @see Renderer
		 *
		 */
		class OSDL_DLL BaseGUIView : public Ceylan::MVC::BaseView
		{


			public:



				/**
				 * Constructs a new base GUI view, used so that the graphical
				 * user interface can be rendered.
				 *
				 */
				explicit BaseGUIView() ;



				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~BaseGUIView() throw() ;


				/**
				 * Requests that view to render the GUI.
				 *
				 * In this case no specific model is used, as a GUI is a pure
				 * model-less view.
				 *
				 */
				virtual void render() ;


				/**
				 * Returns an user-friendly description of the state of this
				 * object.
				 *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
				 *
				 */
		 		virtual const std::string toString(
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;




			protected:



			private:



				/**
				 * Copy constructor made private to ensure that it will never be
				 * called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 *
				 */
				explicit BaseGUIView( const BaseGUIView & source ) ;



				/**
				 * Assignment operator made private to ensure that it will never
				 * be called.
				 *
				 * The compiler should complain whenever this undefined
				 * operator is called, implicitly or not.
				 *
				 */
				BaseGUIView & operator = ( const BaseGUIView & source ) ;




		} ;


	}

}



#endif // OSDL_BASE_GUI_VIEW_H_

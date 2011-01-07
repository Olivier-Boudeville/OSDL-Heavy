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


/**
 * Taken from OpenGL-intro-1.1.1
 * (http://www.libsdl.org/opengl/OpenGL-intro-1.1.1.zip)
 *
 * This code was created by Jeff Molofee '99
 * (ported to SDL by Sam Lantinga '2000)
 *
 * If you have found this code useful, please let him know.
 *
 * Visit him at www.demonews.com/hosted/nehe 
 *
 */

#include "OSDL.h"     // just for basic primitives such as getBackendLastError
using namespace OSDL ;

using namespace Ceylan::Log ;


#include <iostream> // for cerr


#include "SDL.h"
#include "SDL_opengl.h"



/* 
 * A general OpenGL initialization function. Sets all of the initial 
 * parameters. 
 */

void InitGL(int Width, int Height)	        
// We call this right after our OpenGL window is created.
{

  glViewport(0, 0, Width, Height) ;
  
  // This will clear the background color to black :
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f) ;		
  
  // Enables clearing of the depth buffer :
  glClearDepth(1.0) ;				

  // The type of depth test to do :		
  glDepthFunc(GL_LESS) ;		

  // Enables depth testing : 
  glEnable(GL_DEPTH_TEST) ;			
  
  // Enables smooth color shading :
  glShadeModel(GL_SMOOTH) ;			

  glMatrixMode(GL_PROJECTION) ;
  
  // Reset the projection matrix :
  glLoadIdentity() ;	
  			
  // Calculate the aspect ratio of the window :
  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f) ;	

  glMatrixMode(GL_MODELVIEW) ;
  
}


/* The main drawing function. */
void DrawGLScene()
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;		
  // Clear the screen and the depth buffer
  glLoadIdentity() ;				// Reset the view

  glTranslatef(-1.5f,0.0f,-6.0f) ;		
  // Move left 1.5 units and into the screen 6.0
	
  // draw a triangle
  glBegin(GL_POLYGON) ;				// start drawing a polygon
  glVertex3f( 0.0f, 1.0f, 0.0f) ;		// Top
  glVertex3f( 1.0f,-1.0f, 0.0f) ;		// Bottom Right
  glVertex3f(-1.0f,-1.0f, 0.0f) ;		// Bottom Left	
  glEnd() ;					// we're done with the polygon

  glTranslatef(3.0f,0.0f,0.0f) ;		        // Move Right 3 Units
	
  // draw a square (quadrilateral)
  glBegin(GL_QUADS) ;				// start drawing a polygon (4 sided)
  glVertex3f(-1.0f, 1.0f, 0.0f) ;		// Top Left
  glVertex3f( 1.0f, 1.0f, 0.0f) ;		// Top Right
  glVertex3f( 1.0f,-1.0f, 0.0f) ;		// Bottom Right
  glVertex3f(-1.0f,-1.0f, 0.0f) ;		// Bottom Left	
  glEnd() ;					// done with the polygon

  // swap buffers to display, since we're double buffered.
  SDL_GL_SwapBuffers() ;
  
}


int main(int argc, char **argv) 
{  
	
  LogHolder myLog( argc, argv ) ;
	
  LogPlug::info( "Testing basic OpenGL support" ) ;

  bool isBatch = false ;

  std::string executableName ;
  std::list<std::string> options ;

  Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;

  std::string token ;
  bool tokenEaten ;


  while ( ! options.empty() )
  {

      token = options.front() ;
      options.pop_front() ;

      tokenEaten = false ;
    			  
      if ( token == "--batch" )
      {
    	  LogPlug::info( "Batch mode selected" ) ;
    	  isBatch = true ;
    	  tokenEaten = true ;
      }
      
      if ( token == "--interactive" )
      {
    	  LogPlug::info( "Interactive mode selected" ) ;
    	  isBatch = false ;
    	  tokenEaten = true ;
      }
      
      
      if ( LogHolder::IsAKnownPlugOption( token ) )
      {
    	  // Ignores log-related (argument-less) options.
    	  tokenEaten = true ;
      }
      
      
      if ( ! tokenEaten )
      {
		  // No exception can be thrown because of SDLmain declaration :
		  LogPlug::fatal( "Unexpected command line argument : " + token ) ;
		  exit( 1 ) ;
      }

  }


  int done;

  /* Initialize SDL for video output */
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
  {
    std::cerr << "Unable to initialize SDL : " 
		<< Utils:: getBackendLastError() << std::endl ;
    return 1 ;
  }

  int screenWidth  = 640 ;
  int screenHeight = 480 ;
  	
  /* Create a 640x480 OpenGL screen */
  if ( SDL_SetVideoMode( screenWidth, screenHeight, /* current bpp */ 0,
  	SDL_OPENGL) == 0 ) 
  {
    std::cerr << "Unable to create OpenGL screen : " << 
		Utils:: getBackendLastError() << std::endl ;
    SDL_Quit() ;
    return 2 ;
  }

  /* Set the title bar in environments that support it */
  SDL_WM_SetCaption( "SDL with OpenGL example, taken from "
  	"Jeff Molofee's GL Code Tutorial, NeHe '99", 0) ;

  /* Loop, drawing and checking events */
  InitGL( screenWidth, screenHeight ) ;
  done = 0;

  if ( ! isBatch )
  	Ceylan::display( "< Press any key on SDL window to stop >" ) ;

  while ( ! done ) 
  {
  
    DrawGLScene() ;
	
	/*
	 * Added by OSDL to avoid display saturation which prevents inputs to 
	 * be processed on time :
	 *
	 */
	SDL_Delay(10) ;
	
    /* This could go in a separate function */
    { 
	
	  SDL_Event event;

	  if ( ! isBatch )
	  {
	
	      do 
	      {
		    	  
	    	  // Avoid busy waits :
	    	  SDL_WaitEvent( & event ) ;
	      
	      } while ( event.type != SDL_KEYDOWN ) ;
		  
		  done = true ;
		  
	  }
	  else
	  {
	      SDL_Delay( 1000 /* milliseconds */ ) ;
		  done = true ;
	  }

    }
	
  }
  SDL_Quit() ;
  return 0 ;
  
}


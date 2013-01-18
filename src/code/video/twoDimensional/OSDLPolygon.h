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


#ifndef OSDL_POLYGON_H_
#define OSDL_POLYGON_H_


#include "OSDLPixel.h"              // for Pixels::put, Pixels::Pixel

#include "Ceylan.h"                 // for TextDisplayable, AngleInDegrees

#include <string>
#include <list>


#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

#include "SDL.h"             // for ColorDefinition

#endif // OSDL_USES_SDL


namespace OSDL
{


	
	namespace Video
	{



		// Polygons are drawn to surfaces.
		class Surface ;
				
		
		
		namespace TwoDimensional
		{
		
		
		
			// Polygons are made from 2D points .
			class Point2D ;
				
				
				
			/**
			 * Definition so that readability increases.
			 *
			 * @note This is a list of <b>pointers</b> to Point2D instances.
			 *
			 */
			typedef std::list<Point2D *> listPoint2D ;
				
			
			
			/**
		 	 * Some polygone-based shapes:
			 * - polygon-approximation of a pie
			 * - triangle
			 * - Polygon: true locatable polygon class
			 * - PolygonSet: set of polygons defined in a same shared
			 * referential
			 *
			 */



			/*
			 * Methods could be inlined in a dedicated file for faster
			 * processing.
			 *
			 */
	
	
	
			/**
			 * Draws a polygon-based filled pie whose center is
			 * (xCenter,yCenter) of specified radius, with specified RGBA 
			 * color, on specified surface, starting from angleStart,
			 * stopping to angleStop.
			 *
			 * @param targetSurface the surface to draw to.
			 *
			 * @param xCenter the abscissa of the center of the pie.
			 *
			 * @param yCenter the ordinate of the center of the pie.
			 *
			 * @param radius the radius of the pie.
			 *
			 * @param angleStart starting angle of the pie, in degrees.
			 *
			 * @param angleStop stopping angle of the pie, in degrees.
			 *
			 * @param red the red color coordinate of fill color.
			 *
			 * @param green the green color coordinate of fill color.
			 *
			 * @param blue the blue color coordinate of fill color.
			 *
			 * @param alpha the alpha color coordinate of fill color.
			 *
			 * @return false if and only if something went wrong, for
			 * example if the surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			OSDL_DLL bool drawPie( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter, Length radius, 
				Ceylan::Maths::AngleInDegrees angleStart,
				Ceylan::Maths::AngleInDegrees angleStop,
				Pixels::ColorElement red, Pixels::ColorElement green, 
				Pixels::ColorElement blue, 
				Pixels::ColorElement alpha = Pixels::AlphaOpaque ) ;
	
	
	
			/**
			 * Draws a polygon-based filled pie whose center is
			 * (xCenter,yCenter) of specified radius, with specified RGBA 
			 * color, on specified surface, starting from angleStart,
			 * stopping to angleStop.
			 *
			 * @param targetSurface the surface to draw to.
			 *
			 * @param xCenter the abscissa of the center of the pie.
			 *
			 * @param yCenter the ordinate of the center of the pie.
			 *
			 * @param radius the radius of the pie.
			 *
			 * @param angleStart starting angle of the pie, in degrees.
			 *
			 * @param angleStop stopping angle of the pie, in degrees.
			 *
			 * @param colorDef the color definition of fill color.
			 *
			 * @return false if and only if something went wrong, for
			 * example if the surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			OSDL_DLL bool drawPie( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter, Length radius, 
				Ceylan::Maths::AngleInDegrees angleStart,
				Ceylan::Maths::AngleInDegrees angleStop, 
				Pixels::ColorDefinition colorDef ) ;
	
	

			/**
			 * Draws a triangle defined by specified three points, with
			 * specified RGBA color, on specified surface.
			 *
			 * @param filled tells whether the triangle should be filled.
			 *
			 * @return false if and only if something went wrong, for
			 * example if the surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note Will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			OSDL_DLL bool drawTriangle( Surface & targetSurface, 
				Coordinate x1, Coordinate y1, 
				Coordinate x2, Coordinate y2, 
				Coordinate x3, Coordinate y3, 
				Pixels::ColorElement red, Pixels::ColorElement green, 
				Pixels::ColorElement blue, 
				Pixels::ColorElement alpha = Pixels::AlphaOpaque,
				bool filled = true ) ;



			/**
			 * Draws a triangle defined by specified three points, with
			 * specified RGBA color, on specified surface.
			 *
			 * @param filled tells whether the triangle should be filled.
			 *
			 * @return false if and only if something went wrong, for
			 * example if the surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note Will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			OSDL_DLL bool drawTriangle( Surface & targetSurface, 
				Coordinate x1, Coordinate y1, 
				Coordinate x2, Coordinate y2, 
				Coordinate x3, Coordinate y3, 
				Pixels::ColorDefinition colorDef, bool filled = true ) ;
	
	

			/**
			 * Draws a triangle defined by specified three points, with
			 * specified RGBA color, on specified surface.
			 *
			 * @param filled tells whether the triangle should be filled.
			 *
			 * @return false if and only if something went wrong, for
			 * example if the surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note Will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			OSDL_DLL bool drawTriangle( Surface & targetSurface, 
				const Point2D & p1, const Point2D & p2, const Point2D & p3,
				Pixels::ColorElement red, Pixels::ColorElement green, 
				Pixels::ColorElement blue, 
				Pixels::ColorElement alpha = Pixels::AlphaOpaque,
				bool filled = true ) ;



			/**
			 * Draws a triangle defined by specified three points, with
			 * specified RGBA color, on specified surface.
			 *
			 * @param filled tells whether the triangle should be filled.
			 *
			 * @return false if and only if something went wrong, for
			 * example if the surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note Will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			OSDL_DLL bool drawTriangle( Surface & targetSurface, 
				const Point2D & p1, const Point2D & p2, const Point2D & p3,
				Pixels::ColorDefinition colorDef, bool filled = true ) ;
	


			/**
			 * Draws a polygon defined by specified list of points, with
			 * specified RGBA color, on specified surface.
			 *
			 * @param summits could be a 'const' list of 'const' Point2D
			 * pointers, but the STL would not be able to match it with 
			 * mostly used argument 'const list of Point2D non-const pointers'.
			 *
			 * @param x the abscissa the polygon should be translated of, 
			 * on screen.
			 *
			 * @param y the ordinate the polygon should be translated of, 
			 * on screen.
			 *
			 * @param filled tells whether the polygon should be filled.
			 *
			 * @return false if and only if something went wrong, for
			 * example if the surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note Will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */	
			OSDL_DLL bool drawPolygon( Surface & targetSurface, 
				const listPoint2D & summits,
				Coordinate x, Coordinate y,
				Pixels::ColorElement red, Pixels::ColorElement green, 
				Pixels::ColorElement blue, 
				Pixels::ColorElement alpha = Pixels::AlphaOpaque,
				bool filled = true ) ;				



			/**
			 * Draws a polygon defined by specified list of points, with
			 * specified RGBA color, on specified surface, at specified
			 * location.
			 *
			 * @param summits could be a 'const' list of 'const' Point2D
			 * pointers, but the STL would not be able to match it with 
			 * mostly used argument 'const list of Point2D non-const pointers'.
			 * 
			 * @param x the abscissa the polygon should be translated of, 
			 * on screen.
			 *
			 * @param y the ordinate the polygon should be translated of, 
			 * on screen.
			 *
			 * @param filled tells whether the polygon should be filled.
			 *
			 * @return false if and only if something went wrong, for
			 * example if the surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note Will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */	
			OSDL_DLL bool drawPolygon( Surface & targetSurface, 
				const listPoint2D & summits,
				Coordinate x, Coordinate y,
				Pixels::ColorDefinition colorDef, bool filled = true ) ;
				
				 
				 
			
			/**
			 * Polygons are defined as line segments passing through a 
			 * suite of 2D points and enclosing a surface, since the last 
			 * vector points towards the first.
			 *
			 * All these vectors are to be defined in the Polygon's own 2D
			 * referential.
			 *
			 * @note Polygons use <b>pointers</b> to Point2D so that these
			 * points can be shared by several polygons, for example in 
			 * the case of a vertex being common to three edges, hence 
			 * multiple polygons. 
			 * Therefore points, unless specified otherwise (see 
			 * pointOwner) are not owned by a polygon referencing them, 
			 * and only their particular list specified for a polygon is
			 * deallocated when the polygon is itself deallocated.
			 * The points themselves are left as they are.
			 *
			 */
			class OSDL_DLL Polygon: public Ceylan::Locatable2D
			{
			
			
				public:
				
				
				
					/**
					 * Creates a polygon from a suite of summit vertices. 
					 *
					 * @param summits the input list of summits for this
					 * polygon.
					 *
					 * @param listOwner tells whether this polygon owns the
					 * specified point list <b>and</b> the points in it 
					 * (and therefore will deallocate them) or not (in this
					 * case, neither the list object nor the points will be
					 * deallocated).
					 *
					 * @note Never put twice the same point object in the 
					 * list if the polygon is going to own its points, 
					 * since the duplicated points would be deleted twice,
					 * which results usually in a segmentation fault.
					 *
					 */
					explicit Polygon( listPoint2D & summits, 
						bool listOwner = false ) ;
					
			
			
					/// Basic virtual destructor.
					virtual ~Polygon() throw() ;
					
					
					
					/**
					 * Draws the polygon on specified surface, with 
					 * specified color.
					 *
					 * The polygon can be rendered after transformation 
					 * with the embedded matrix, the referential of this
					 * Locatable.
					 *
					 * In all cases the place where to render the polygon 
					 * on surface is determined by this inner matrix.
					 *
					 * @param targetSurface the surface to draw to.
					 *
					 * @param colorDef the polygon color.
					 *
					 * @param filled tells whether the polygon should be 
					 * filled.
					 *
					 * @return true iff rendering succeeded.
					 *
					 */
					virtual bool draw( Surface & targetSurface,
						Pixels::ColorDefinition colorDef = Pixels::White, 
						bool filled = true ) const ;
						
					
					
					/**
					 * Returns the points that this polygon gathered. 
					 *
					 * @note What can be done with these points depends 
					 * on whether the polygon owns its points or not.
					 *
					 */
					virtual listPoint2D & getPoints() const ;
						
						
						
					/**
					 * Sets the vertex list for this polygon.
					 *
					 * @param newList the new list of vertices.
					 *
					 * Any pre-existing owned list will be deallocated
					 * first.
					 *
					 */
					virtual void setPoints( listPoint2D & newList ) ;
					
					
					
						
					/// Tells whether the polygon owns its points.
					virtual bool isListOwner() const ;
				
										
					 	
	 	            /**
		             * Returns an user-friendly description of the state 
					 * of this object.
		             *
					 * @param level the requested verbosity level.
					 *
					 * @note Text output format is determined from 
					 * overall settings.
					 *
					 * @see Ceylan::TextDisplayable
		             *
		             */
			 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) const ;
					
					
					
					
					
					// Static section.
					
					
					
					/**
					 * Creates a polygon whose shape is a snow flake, and 
					 * which has no local referential registered.
					 *
					 * @note The caller should deallocate this polygon when
					 * finished with it.
					 *
					 * @param length defines the height of the main branch 
					 * of this flake.
					 *
					 * @param thickness defines the width of the main 
					 * branch of this flake.
					 *
					 * @param childAngle defines the angle, in degrees, 
					 * between the main branch and the second branch.
					 *
					 * @param branchingHeightRatio tells at which height 
					 * of the main branch the second shall begin.
					 *
					 * @param scale defines the ratio of the second branch
					 * relatively to the main one.
					 *
					 * @return A polygon which does not own its points, so 
					 * that they can be freely used thanks to getPoints(). 
					 * Hence the caller will have to deallocate them
					 * explicitly (deallocating the returned polygon will 
					 * not be enough).
					 *
					 * @see PolygonSet::CreateFlake	 
					 *
					 */
					static Polygon & CreateFlakeBranch( 
						Length length = 200,
						Length thickness = 20, 
						Ceylan::Maths::AngleInDegrees childAngle = 50, 
						Ceylan::Maths::Ratio branchingHeightRatio = 0.7, 
						Ceylan::Maths::Ratio scale = 0.3 ) ;
					
					
					
					/**
					 * Duplicates specified list of points: returns 
					 * another list containing clones of each points in
					 * <b>source</b>.
					 *
					 * @return The duplicated list of points.
					 *
					 */
					static listPoint2D & Duplicate( 
						const listPoint2D & source ) ;
	
	
	
					/**
					 * Deletes specified list of points, including the 
					 * points themselves.
					 *
					 */
					static void Delete( listPoint2D & listToBeDeleted ) ;
					
					
					
					/**
					 * Appends a shallow (not deep) copy of the 
					 * <b>toAppend</b> list to the <b>toBeAugmented</b> list.
					 *
					 * @note the <b>toBeAugmented</b> takes ownership of the
					 * points of <b>toAppend</b>, if <b>toBeAugmented</b> is
					 * a point owner list.
					 *
					 * @return toBeAugmented after it has been augmented 
					 * by toAppend's points.
					 * It is the same list as specified in argument, but
					 * modified: no copy is made. 
					 * Hence the returned value can be ignored, it is just
					 * convenient when chaining functions.
					 *
					 */
					static listPoint2D & Append( listPoint2D & toBeAugmented, 
						const listPoint2D & toAppend ) ;
						
						
						
					/**
					 * Applies <b>transformation</b>, specified as a 3x3
					 * homogeneous matrix, to each point of <b>sourceList</b>.
					 *
					 * @param transformation the 2D geometrical 
					 * transformation, specified as a 3x3 homogeneous matrix.
					 *
					 * @param sourceList the list of points to transform. 
					 * It will be updated thanks to the transformation.
					 *
					 * @return sourceList after all its points have been
					 * transformed. 
					 * It is the same list as specified in argument, but
					 * modified: no copy is made. 
					 * Hence the returned value can be ignored, it is just
					 * convenient when chaining functions.
					 *
					 */	
					static listPoint2D & Apply( 
						const Ceylan::Maths::Linear::HomogeneousMatrix3 
							& transformation,
						listPoint2D & sourceList ) ;
						
					
					
					
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

					/// List of the summits of the polygon.
					listPoint2D * _summits ;

#pragma warning( pop ) 
			
					
					
					/**
					 * Tells whether the polygon owns the lists and the 
					 * points that are stored in it.
					 *
					 */					
					bool _listOwner ;
					
				
				
					/**
					 * Copy constructor made private to ensure that 
					 * it will be never called.
					 *
					 * The compiler should complain whenever this 
					 * undefined constructor is called, implicitly or not.
					 *
					 */			 
					explicit Polygon( const Polygon & source ) ;
					
					
			
					/**
					 * Assignment operator made private to ensure 
					 * that it will be never called.
					 *
					 * The compiler should complain whenever this 
					 * undefined operator is called, implicitly or not.
					 *
					 */			 
					Polygon & operator = ( const Polygon & source ) ;

					
					
			} ;
			




			/**
			 * Polygon sets are collections of polygons defined in the same
			 * referential.
			 *
			 * Polygon sets are especially useful to model shapes that 
			 * are complex or split in multiple non-overlapping parts, not
			 * connex.
			 *
			 * @see Polygon
			 *
			 */
			class OSDL_DLL PolygonSet: public Ceylan::Locatable2D
			{
			
			
				public:
				
				
										
					/**
					 * Creates an empty set of polygons.
					 *
					 * @param listOwner tells whether this polygon owns the
					 * lists and the points in its polygon list (and 
					 * therefore will deallocate them) or not (in this case,
					 * neither the list nor the points will be deallocated).
					 *
					 */
					 explicit PolygonSet( bool listOwner = false ) ;
					 
					 
					 
					/**
					 * Creates an encapsulated set of polygons from a 
					 * list of suites of summit vertices. 
					 *
					 * @param polygonList a list of list of points as inputs.
					 *
					 * @param listOwner tells whether this polygon owns the
					 * lists and the points in its polygon list (and 
					 * therefore will deallocate them) or not (in this case,
					 * neither the list nor the points will be deallocated).
					 *
					 * @note The polygon set is not made of a list of 
					 * Polygons, since they would each have their own
					 * referential, while they should all be defined 
					 * relatively to the same.
					 *
					 * @note Never put twice the same point object in the
					 * list if the polygon is going to own its points, 
					 * since the duplicated points would be deleted twice,
					 * which results in a core dump.
					 *
					 */
					explicit PolygonSet( 
						std::list<listPoint2D *> & polygonList, 
						bool listOwner = false ) ;
					
			
			
					/// Basic virtual destructor.
					virtual ~PolygonSet() throw() ;
					
					
					
					/**
					 * Adds a new polygon to the polygon set, thanks 
					 * to the points of the specified polygon. 
					 *
					 * The polygon set takes ownership of the list and 
					 * the points in it.
					 *
					 * @param newPolygon the point-providing polygon.
					 *
					 * @note No added polygon must own its points.
					 *
					 * @note Only the points of the specified polygon are 
					 * taken into account, its referential, existing or not, 
					 * is ignored.
					 *
					 */
					virtual void addPointsOf( Polygon & newPolygon ) ;
					
					
					
					/**
					 * Adds a new list of points to the polygon set.
					 *
					 * @param listToAdd the list of points to be added.
					 *
					 * @note The ownership of the list is taken; for the 
					 * one of its points, it depends on whether the 
					 * polygon set has been constructed as a list owner
					 * (would then deallocate the points) or not.
					 *
					 */					 
					virtual void addPointList( listPoint2D & listToAdd ) ;
					
					
					
					/**
					 * Draws the polygon set on specified surface, with
					 * specified color, at specified place.
					 *
					 *
					 */
					virtual bool draw( Surface & targetSurface,
						Coordinate x, Coordinate y,
						Pixels::ColorDefinition colorDef = Pixels::White, 
						bool filled = true ) const ;
	
	
						
	 	            /**
		             * Returns an user-friendly description of the 
					 * state of this object.
		             *
					 * @param level the requested verbosity level.
					 *
					 * @note Text output format is determined from 
					 * overall settings.
					 *
					 * @see Ceylan::TextDisplayable
		             *
		             */
			 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) const ;
					
					
					
					
					// Static section.
					
					
					
					/**
					 * Creates a polygon whose shape is a snow flake.
					 *
					 * @note The caller should deallocate this polygon 
					 * when finished with it.
					 *
					 * @param branchCount tells how many branches the 
					 * flake should have (must not be null).
					 *
					 * @param length defines the height of the main branch 
					 * of this flake.
					 *
					 * @param thickness defines the width of the main 
					 * branch of this flake.
					 *
					 * @param childAngle defines the angle, in degrees, 
					 * between the main branch and the second branch.
					 *
					 * @param branchingHeightRatio tells at which height 
					 * of the main branch the second shall begin.
					 *
					 * @param scale defines the ratio of the second branch
					 * relatively to the main one.
					 *
					 * @see Polygon::CreateFlakeBranch	 
					 *
					 */
					static PolygonSet & CreateFlake( 
						Ceylan::Uint8 branchCount = 5, 
						Length length = 200, Length thickness = 20, 
						Ceylan::Maths::AngleInDegrees childAngle = 50, 
						Ceylan::Maths::Ratio branchingHeightRatio = 0.7, 
						Ceylan::Maths::Ratio scale = 0.3 ) ;
					
				
						
				protected:

				
/* 
 * Takes care of the awful issue of Windows DLL with templates.
 *
 * @see Ceylan's developer guide and README-build-for-windows.txt 
 * to understand it, and to be aware of the associated risks. 
 * 
 */
#pragma warning( push )
#pragma warning( disable: 4251 )
				
					/// List of the summits of the polygon.
					std::list<listPoint2D *> * _polygonList ;
			
#pragma warning( pop ) 

			
			
					/**
					 * Tells whether the polygon owns the embedded list,
					 * including its whole content.
					 *
					 */					
					bool _listOwner ;
					
					
					
					
				private:
				
				
				
					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit PolygonSet( const PolygonSet & source ) ;
					
					
					
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					PolygonSet & operator = ( const PolygonSet & source ) ;
						
					
			} ;	
					
				 
		}
		
	}
	
}



#endif // OSDL_POLYGON_H_


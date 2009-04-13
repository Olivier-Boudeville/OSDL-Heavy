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




/*
 * No include guards wanted: to be included each time that these defines 
 * can be screwed up.
 *
 */

#ifdef OSDL_RUNS_ON_WINDOWS

// Microsoft stupidly managed to redefine symbols in a header (winuser.h):

#ifdef LoadIcon
#undef LoadIcon
#endif // LoadIcon

#ifdef LoadImage
#undef LoadImage
#endif // LoadImage

#ifdef LoadImageA
#undef LoadImageA
#endif // LoadImageA

#endif // OSDL_RUNS_ON_WINDOWS


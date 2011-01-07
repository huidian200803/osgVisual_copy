/* -*-c++-*- osgVisual - Copyright (C) 2009-2010 Torben Dannhauer
 *
 * This library is based on OpenSceneGraph, open source and may be redistributed and/or modified under 
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * osgVisual requires for some proprietary modules a license from the correspondig manufacturer.
 * You have to aquire licenses for all used proprietary modules.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

// Basic ideas for memory leak debugging:
// http://weseetips.com/2008/06/17/how-to-detect-memory-leaks-by-using-crt/
// http://stackoverflow.com/questions/532917/dealing-with-incorrect-memory-leak-reports-on-windows
// http://msdn.microsoft.com/en-us/library/x98tx3cf.aspx

#ifdef _DEBUG
	#ifdef WIN32
		#ifndef USE_SKY_SILVERLINING
			#ifndef DBG_NEW
				#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
				#define new DBG_NEW
			#endif
		#endif
	#endif
#endif
/* -*-c++-*- osgVisual - Copyright (C) 2009-2011 Torben Dannhauer
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

#include "dataIO_slot.h"

REGISTER_OBJECT_WRAPPER( dataIO_slot,                 // The unique wrapper name
                         new osgVisual::dataIO_slot,             // The proto
                         osgVisual::dataIO_slot,                 // The class typename
						 "osg::Object osgVisual::dataIO_slot" )  // The inheritance relations
{
    // Serializers for different members    
	BEGIN_ENUM_SERIALIZER( dataDirection, TO_OBJ );
		ADD_ENUM_VALUE( TO_OBJ );
		ADD_ENUM_VALUE( FROM_OBJ );
		END_ENUM_SERIALIZER();
	BEGIN_ENUM_SERIALIZER( varType, DOUBLE );
		ADD_ENUM_VALUE( DOUBLE );
		ADD_ENUM_VALUE( STRING );
		END_ENUM_SERIALIZER();
	ADD_STRING_SERIALIZER( VariableName, "" );
	ADD_DOUBLE_SERIALIZER( Value, 0.0 );
	ADD_STRING_SERIALIZER( SValue, "" );	
}
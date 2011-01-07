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

#include "dataIO_executer.h"

REGISTER_OBJECT_WRAPPER( dataIO_executer,                 // The unique wrapper name
						new osgVisual::dataIO_executer,             // The proto
                         osgVisual::dataIO_executer,                 // The class typename
						 "osg::Node osgVisual::dataIO_executer" )  // The inheritance relations
{
    // Serializers for different members    
	BEGIN_ENUM_SERIALIZER( executerID, DO_NOTHING );
		ADD_ENUM_VALUE( GET_HAT );
		ADD_ENUM_VALUE( GET_HOT );
		ADD_ENUM_VALUE( GET_LAT );
		ADD_ENUM_VALUE( GET_LON );
		ADD_ENUM_VALUE( IS_COLLISION );
		ADD_ENUM_VALUE( IS_INSIDE_CLOUD );
		ADD_ENUM_VALUE( IS_WIND );
		ADD_ENUM_VALUE( IS_PRECIPITATION );
		ADD_ENUM_VALUE( DO_NOTHING );
		END_ENUM_SERIALIZER();
		ADD_LIST_SERIALIZER( DoubleParameter, osgVisual::dataIO_executer::parameterList );
	ADD_STRING_SERIALIZER( StringParameter, "" );	
}
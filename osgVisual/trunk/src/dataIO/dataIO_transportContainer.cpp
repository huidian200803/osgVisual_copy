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

#include "dataIO_transportContainer.h"
#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

REGISTER_OBJECT_WRAPPER( dataIO_transportContainer,                      // The unique wrapper name
                         new osgVisual::dataIO_transportContainer,             // The proto
                         osgVisual::dataIO_transportContainer,                 // The class typename
						 "osg::Object osgVisual::dataIO_transportContainer" )  // The inheritance relations
{
	ADD_INT_SERIALIZER( FrameID, 0 );
	ADD_MATRIXD_SERIALIZER( ViewMatrix, osg::Matrixd() );
	ADD_LIST_SERIALIZER( Executer, osgVisual::dataIO_transportContainer::executerList );
	ADD_LIST_SERIALIZER( IOSlots, osgVisual::dataIO_transportContainer::slotList );
}

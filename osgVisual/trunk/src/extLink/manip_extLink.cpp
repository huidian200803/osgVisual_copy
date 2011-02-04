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
#include <manip_extLink.h>



using namespace osg;
using namespace osgGA;
using namespace osgVisual;

extLinkManipulator::extLinkManipulator()
{
}

extLinkManipulator::~extLinkManipulator()
{
}

void extLinkManipulator::init(const GUIEventAdapter& ,GUIActionAdapter& )
{
}

void extLinkManipulator::getUsage(osg::ApplicationUsage& usage) const
{
}

bool extLinkManipulator::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
{
    switch(ea.getEventType())
    {
    case(GUIEventAdapter::FRAME):
        if (attachedObject.valid())
		{
			setByMatrix( attachedObject->getCameraMatrix() );
		}
        return false;
    default:
        break;
    }

    return false;
}

void extLinkManipulator::setByMatrix(const osg::Matrixd& matrix)
{
    manipMatrix = matrix;
}

osg::Matrixd extLinkManipulator::getMatrix() const
{
    return manipMatrix;
}

osg::Matrixd extLinkManipulator::getInverseMatrix() const
{
	return osg::Matrixd::inverse(manipMatrix);
}

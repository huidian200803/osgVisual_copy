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

#include <manip_freeSpaceMouse.h>



using namespace osg;
using namespace osgGA;
using namespace osgVisual;

FreeManipulator::FreeManipulator(SpaceMouse* spacemouse) : _spaceMouse(spacemouse)
{
}

FreeManipulator::~FreeManipulator()
{
}

void FreeManipulator::init(const GUIEventAdapter& ,GUIActionAdapter& )
{
}

void FreeManipulator::getUsage(osg::ApplicationUsage& usage) const
{
}

bool FreeManipulator::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
{
    switch(ea.getEventType())
    {
    case(GUIEventAdapter::FRAME):
        if (calcMovement())
            us.requestRedraw();
        return false;
    default:
        break;
    }

    return false;
}

void FreeManipulator::setByMatrix(const osg::Matrixd& matrix)
{
    _position = matrix.getTrans();
    _attitude = matrix.getRotate();
}

osg::Matrixd FreeManipulator::getMatrix() const
{
    return osg::Matrixd::rotate(_attitude)*osg::Matrixd::translate(_position);
}

osg::Matrixd FreeManipulator::getInverseMatrix() const
{
    return osg::Matrixd::translate(-_position)*osg::Matrixd::rotate(_attitude.inverse());
}

bool FreeManipulator::calcMovement()
{
    double dTX, dTY, dTZ;
    _spaceMouse->getTranslations(dTX, dTY, dTZ);

    double dRX, dRY, dRZ;
    _spaceMouse->getRotations(dRX, dRY, dRZ);

    osg::Matrix rotation_matrix;
    rotation_matrix.makeRotate(_attitude);

    osg::Vec3d lookVector = -getUpVector(rotation_matrix);
    osg::Vec3d sideVector = getSideVector(rotation_matrix);
    osg::Vec3d upVector = getFrontVector(rotation_matrix);

    lookVector.normalize();
    sideVector.normalize();
    upVector.normalize();

    // POSITION
    _position -= lookVector * dTZ;
    _position += upVector   * dTY;
    _position += sideVector * dTX;
	//std::cout << "Translation: "<< dTX * 1000000.0<< ", " << dTY * 1000000.0<< ", " << dTZ * 1000000.0<< std::endl;

    // ATTITUDE
    _attitude *= osg::Quat( dRY, osg::Vec3(0.0, 0.0, 1.0));		//Yaw
    _attitude *= osg::Quat( dRX, sideVector);					// Pitch
	_attitude *= osg::Quat( -dRZ, lookVector);					// Bank


    if (!_trackNodePath.empty())
    {
        osg::Vec3 trackNodeCenter = getTrackNodeCenter();

        osg::Matrix matrixLookAt;
        matrixLookAt.makeLookAt(_position, trackNodeCenter, osg::Vec3(0.0, 0.0, 1.0));

        osg::Quat attitude = matrixLookAt.getRotate().inverse();
        _attitude = attitude;
    }

    return true;
}

osg::Vec3 FreeManipulator::getTrackNodeCenter()
{
    osg::Matrixd localToWorld, worldToLocal;
    computeNodeLocalToWorld(localToWorld);
    computeNodeWorldToLocal(worldToLocal);

    osg::Vec3 center(0.0, 0.0, 0.0);
    if (validateNodePath())
        center = osg::Vec3d(_trackNodePath.back()->getBound().center())*localToWorld;
    else
        center = osg::Vec3d(0.0f,0.0f,0.0f)*localToWorld;

    return(center);
}

void FreeManipulator::computeNodeWorldToLocal(osg::Matrixd& worldToLocal) const
{
    if (validateNodePath())
    {
        worldToLocal = osg::computeWorldToLocal(getNodePath());
    }
}

void FreeManipulator::computeNodeLocalToWorld(osg::Matrixd& localToWorld) const
{
    if (validateNodePath())
    {
        localToWorld = osg::computeLocalToWorld(getNodePath());
    }

}

bool FreeManipulator::validateNodePath() const
{
    for(ObserverNodePath::const_iterator itr = _trackNodePath.begin();
        itr != _trackNodePath.begin();
        ++itr)
    {
        if (*itr==0) 
        {
            OSG_NOTIFY(osg::NOTICE)<<"Warning: tracked node path has been invalidated by changes in the scene graph."<<std::endl;
            const_cast<ObserverNodePath&>(_trackNodePath).clear();
            return false;
        }
    }
    return true;
}

osg::NodePath FreeManipulator::getNodePath() const
{
    osg::NodePath nodePath;
    for(ObserverNodePath::const_iterator itr = _trackNodePath.begin();
        itr != _trackNodePath.end();
        ++itr)
    {
        nodePath.push_back(const_cast<osg::Node*>(itr->get()));
    }
    return nodePath;
}

void FreeManipulator::setTrackNode(osg::Node* node)
{
    if (!node)
    {
        OSG_NOTIFY(osg::NOTICE)<<"NodeTrackerManipulator::setTrackNode(Node*):  Unable to set tracked node due to null Node*"<<std::endl;
        return;
    }

    osg::NodePathList nodePaths = node->getParentalNodePaths();
    if (!nodePaths.empty())
    {
        if (nodePaths.size()>1)
        {
            OSG_NOTIFY(osg::NOTICE)<<"osgGA::NodeTrackerManipualtor::setTrackNode(..) taking first parent path, ignoring others."<<std::endl;
        }

        OSG_NOTIFY(osg::INFO)<<"NodeTrackerManipulator::setTrackNode(Node*"<<node<<" "<<node->getName()<<"): Path set"<<std::endl;
        _trackNodePath.clear();
        setTrackNodePath( nodePaths.front() );
    }
    else
    {
        OSG_NOTIFY(osg::NOTICE)<<"NodeTrackerManipulator::setTrackNode(Node*): Unable to set tracked node due to empty parental path."<<std::endl;
    }
    
    OSG_NOTIFY(osg::INFO)<<"setTrackNode("<<node->getName()<<")"<<std::endl;
    for(unsigned int i=0; i<_trackNodePath.size(); ++i)
    {
        OSG_NOTIFY(osg::INFO)<<"  "<<_trackNodePath[i]->className()<<" '"<<_trackNodePath[i]->getName()<<"'"<<std::endl;
    }

}
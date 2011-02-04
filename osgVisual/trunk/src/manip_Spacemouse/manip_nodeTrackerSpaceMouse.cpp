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

#include <manip_nodeTrackerSpaceMouse.h>



using namespace osg;
using namespace osgVisual;


NodeTrackerSpaceMouse::NodeTrackerSpaceMouse(SpaceMouse* spacemouse) : _spaceMouse(spacemouse)
{
    _trackerMode = NODE_CENTER_AND_ROTATION; 
    _rotationMode = TRACKBALL; 

    _distance = 1.0;

    _thrown = false;
	_autohoming = false;
	_ah_init = false;
	_distanceDependendAV = false;

	TZ=0;
	RX=0;
	RY=0;
	RZ=0;

}


NodeTrackerSpaceMouse::~NodeTrackerSpaceMouse()
{
}

void NodeTrackerSpaceMouse::setTrackNode(osg::Node* node)
{
    if (!node)
    {
        osg::notify(osg::NOTICE)<<"NodeTrackerSpaceMouse::setTrackNode(Node*):  Unable to set tracked node due to null Node*"<<std::endl;
        return;
    }

    osg::NodePathList parentNodePaths = node->getParentalNodePaths();

    if (!parentNodePaths.empty())
    {
        osg::notify(osg::INFO)<<"NodeTrackerSpaceMouse::setTrackNode(Node*): Path set"<<std::endl;
        setTrackNodePath(parentNodePaths[0]);
    }
    else
    {
        osg::notify(osg::NOTICE)<<"NodeTrackerSpaceMouse::setTrackNode(Node*): Unable to set tracked node due to empty parental path."<<std::endl;
    }
}

osg::Node* NodeTrackerSpaceMouse::getTrackNode()
{
    osg::NodePath nodePath;
    if (_trackNodePath.getNodePath(nodePath)) return nodePath.back();
    else return 0;
}

const osg::Node* NodeTrackerSpaceMouse::getTrackNode() const
{
    osg::NodePath nodePath;
    if (_trackNodePath.getNodePath(nodePath)) return nodePath.back();
    else return 0;
}

void NodeTrackerSpaceMouse::setTrackerMode(TrackerMode mode)
{
    _trackerMode = mode;
}

void NodeTrackerSpaceMouse::setRotationMode(RotationMode mode)
{
    _rotationMode = mode;
    if (getAutoComputeHomePosition()) computeHomePosition();    
}

void NodeTrackerSpaceMouse::setNode(osg::Node* node)
{
    _node = node;
    
    if (_node.get())
    {
        const osg::BoundingSphere& boundingSphere=_node->getBound();
        const float minimumDistanceScale = 0.001f;
        _minimumDistance = osg::clampBetween(
            float(boundingSphere._radius) * minimumDistanceScale,
            0.00001f,1.0f);
            
        osg::notify(osg::INFO)<<"Setting Tracker manipulator _minimumDistance to "<<_minimumDistance<<std::endl;
    }
    if (getAutoComputeHomePosition()) computeHomePosition();    
}

const osg::Node* NodeTrackerSpaceMouse::getNode() const
{
    return _node.get();
}


osg::Node* NodeTrackerSpaceMouse::getNode()
{
    return _node.get();
}


void NodeTrackerSpaceMouse::home(const GUIEventAdapter& ,GUIActionAdapter& us)
{
    if (getAutoComputeHomePosition()) computeHomePosition();

    computePosition(_homeEye, _homeCenter, _homeUp);
    us.requestRedraw();
}

void NodeTrackerSpaceMouse::computeHomePosition()
{
    osg::NodePath nodePath;
    _trackNodePath.getNodePath(nodePath);

    osg::Node* node = nodePath.empty() ? getNode() : nodePath.back();
    
    if(node)
    {
        const osg::BoundingSphere& boundingSphere=node->getBound();

        setHomePosition(boundingSphere._center+osg::Vec3( 0.0,-3.5f * boundingSphere._radius,0.0f),
                        boundingSphere._center,
                        osg::Vec3(0.0f,0.0f,1.0f),
                        _autoComputeHomePosition);
    }
}



void NodeTrackerSpaceMouse::init(const GUIEventAdapter& ,GUIActionAdapter& )
{
    flushMouseEventStack();
}


void NodeTrackerSpaceMouse::getUsage(osg::ApplicationUsage& usage) const
{
    usage.addKeyboardMouseBinding("SN Tracker: Space","Toggle camera auto homing");
	usage.addKeyboardMouseBinding("SN Tracker: m","Toggle Nodetracker Mode");
	usage.addKeyboardMouseBinding("SN Tracker: n","Toggle distance dependend angular velocity");
    usage.addKeyboardMouseBinding("SN Tracker: +","When in stereo, increase the fusion distance");
    usage.addKeyboardMouseBinding("SN Tracker: -","When in stereo, reduce the fusion distance");
}

bool NodeTrackerSpaceMouse::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
{
    switch(ea.getEventType())
    {
        case(GUIEventAdapter::PUSH):
        {
            flushMouseEventStack();
            addMouseEvent(ea);
            if (calcMovement()) us.requestRedraw();
            us.requestContinuousUpdate(false);
            _thrown = false;
            return true;
        }

        case(GUIEventAdapter::RELEASE):
        {
            if (ea.getButtonMask()==0)
            {

                double timeSinceLastRecordEvent = _ga_t0.valid() ? (ea.getTime() - _ga_t0->getTime()) : DBL_MAX;
                if (timeSinceLastRecordEvent>0.02) flushMouseEventStack();

                if (isMouseMoving())
                {
                    if (calcMovement())
                    {
                        us.requestRedraw();
                        us.requestContinuousUpdate(true);
                        _thrown = true;
                    }
                }
                else
                {
                    flushMouseEventStack();
                    addMouseEvent(ea);
                    if (calcMovement()) us.requestRedraw();
                    us.requestContinuousUpdate(false);
                    _thrown = false;
                }

            }
            else
            {
                flushMouseEventStack();
                addMouseEvent(ea);
                if (calcMovement()) us.requestRedraw();
                us.requestContinuousUpdate(false);
                _thrown = false;
            }
            return true;
        }

        case(GUIEventAdapter::DRAG):
        {
            addMouseEvent(ea);
            if (calcMovement()) us.requestRedraw();
            us.requestContinuousUpdate(false);
            _thrown = false;
            return true;
        }

        case(GUIEventAdapter::MOVE):
        {
            return false;
        }

        case(GUIEventAdapter::KEYDOWN):
            if (ea.getKey()==' ')
            {
				if (_autohoming)
					_autohoming=false;
				else
					_autohoming=true;
                return true;
            }
			if (ea.getKey()=='m')
            {
				switch(_trackerMode)
				{
					case NODE_CENTER: _trackerMode=NODE_CENTER_AND_AZIM;
						break;
					case NODE_CENTER_AND_AZIM: _trackerMode=NODE_CENTER_AND_ROTATION;
						break;
					case  NODE_CENTER_AND_ROTATION: _trackerMode=NODE_CENTER;
						break;
					default: _trackerMode = NODE_CENTER;
						break;
				};
                return true;
            }
			if (ea.getKey()=='n')
            {
				if (_distanceDependendAV)
					_distanceDependendAV=false;
				else
					_distanceDependendAV=true;
                return true;
            }
            return false;
        case(GUIEventAdapter::FRAME):
            if (_thrown)
            {
                if (calcMovement()) us.requestRedraw();
            }

			computeHomePosition();
			computePosition(_homeEye, _homeCenter, _homeUp);
			if (calcMovementSpaceMouse())
				us.requestRedraw();

            return false;
        default:
            return false;
    }
}


bool NodeTrackerSpaceMouse::isMouseMoving()
{
    if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

    static const float velocity = 0.1f;

    float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
    float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
    float len = sqrtf(dx*dx+dy*dy);
    float dt = _ga_t0->getTime()-_ga_t1->getTime();

    return (len>dt*velocity);
}


void NodeTrackerSpaceMouse::flushMouseEventStack()
{
    _ga_t1 = NULL;
    _ga_t0 = NULL;
}


void NodeTrackerSpaceMouse::addMouseEvent(const GUIEventAdapter& ea)
{
    _ga_t1 = _ga_t0;
    _ga_t0 = &ea;
}

void NodeTrackerSpaceMouse::setByMatrix(const osg::Matrixd& matrix)
{
    osg::Vec3d eye,center,up;
    matrix.getLookAt(eye,center,up,_distance);
    computePosition(eye,center,up);
}

void NodeTrackerSpaceMouse::computeNodeCenterAndRotation(osg::Vec3d& nodeCenter, osg::Quat& nodeRotation) const
{
    osg::Matrixd localToWorld, worldToLocal;
    osg::NodePath nodePath;
    if (_trackNodePath.getNodePath(nodePath))
    {
        worldToLocal = osg::computeWorldToLocal(nodePath);
        localToWorld = osg::computeLocalToWorld(nodePath);
        nodeCenter = osg::Vec3d(nodePath.back()->getBound().center())*localToWorld;
    }
    else
    {
        nodeCenter = osg::Vec3d(0.0f,0.0f,0.0f)*localToWorld;
    }


    switch(_trackerMode)
    {
        case(NODE_CENTER_AND_AZIM):
        {
            CoordinateFrame coordinateFrame = getCoordinateFrame(nodeCenter);
            osg::Matrixd localToFrame(localToWorld*osg::Matrixd::inverse(coordinateFrame));

            double azim = atan2(-localToFrame(0,1),localToFrame(0,0));
            osg::Quat nodeRotationRelToFrame, rotationOfFrame;
            nodeRotationRelToFrame.makeRotate(-azim,0.0,0.0,1.0);
            rotationOfFrame = coordinateFrame.getRotate();
            nodeRotation = nodeRotationRelToFrame*rotationOfFrame;
            break;
        }
        case(NODE_CENTER_AND_ROTATION):
        {
            // scale the matrix to get rid of any scales before we extract the rotation.
            double sx = 1.0/sqrt(localToWorld(0,0)*localToWorld(0,0) + localToWorld(1,0)*localToWorld(1,0) + localToWorld(2,0)*localToWorld(2,0));
            double sy = 1.0/sqrt(localToWorld(0,1)*localToWorld(0,1) + localToWorld(1,1)*localToWorld(1,1) + localToWorld(2,1)*localToWorld(2,1));
            double sz = 1.0/sqrt(localToWorld(0,2)*localToWorld(0,2) + localToWorld(1,2)*localToWorld(1,2) + localToWorld(2,2)*localToWorld(2,2));
            localToWorld = localToWorld*osg::Matrixd::scale(sx,sy,sz);

            nodeRotation = localToWorld.getRotate();
            break;
        }
        case(NODE_CENTER):
        default:
        {
            CoordinateFrame coordinateFrame = getCoordinateFrame(nodeCenter);
            nodeRotation = coordinateFrame.getRotate();
            break;
        }
    }

}


osg::Matrixd NodeTrackerSpaceMouse::getMatrix() const
{
    osg::Vec3d nodeCenter;
    osg::Quat nodeRotation;
    computeNodeCenterAndRotation(nodeCenter,nodeRotation);
    return osg::Matrixd::translate(0.0,0.0,_distance)*osg::Matrixd::rotate(_rotation)*osg::Matrixd::rotate(nodeRotation)*osg::Matrix::translate(nodeCenter);
}

osg::Matrixd NodeTrackerSpaceMouse::getInverseMatrix() const
{
    osg::Vec3d nodeCenter;
    osg::Quat nodeRotation;
    computeNodeCenterAndRotation(nodeCenter,nodeRotation);
    return osg::Matrixd::translate(-nodeCenter)*osg::Matrixd::rotate(nodeRotation.inverse())*osg::Matrixd::rotate(_rotation.inverse())*osg::Matrixd::translate(0.0,0.0,-_distance);
}

void NodeTrackerSpaceMouse::computePosition(const osg::Vec3d& eye,const osg::Vec3d& center,const osg::Vec3d& up)
{
    if (!_node) return;

    // compute rotation matrix
    osg::Vec3 lv(center-eye);
    _distance = lv.length();
    
    osg::Matrixd lookat;
    lookat.makeLookAt(eye,center,up);
    
    _rotation = lookat.getRotate().inverse();
}

bool NodeTrackerSpaceMouse::calcMovement()
{
    // return if less then two events have been added.
    if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

    double dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
    double dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();


    float distance = sqrtf(dx*dx + dy*dy);
    // return if movement is too fast, indicating an error in event values or change in screen.
    if (distance>0.5)
    {
        return false;
    }
    
    // return if there is no movement.
    if (distance==0.0f)
    {
        return false;
    }

    osg::Vec3d nodeCenter;
    osg::Quat nodeRotation;
    computeNodeCenterAndRotation(nodeCenter, nodeRotation);

    unsigned int buttonMask = _ga_t1->getButtonMask();

    if (buttonMask==GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {

        if (_rotationMode==TRACKBALL)
        {

            // rotate camera.
            osg::Vec3 axis;
            double angle;

            double px0 = _ga_t0->getXnormalized();
            double py0 = _ga_t0->getYnormalized();

            double px1 = _ga_t1->getXnormalized();
            double py1 = _ga_t1->getYnormalized();


            trackball(axis,angle,px1,py1,px0,py0);

            osg::Quat new_rotate;
            new_rotate.makeRotate(angle,axis);

            _rotation = _rotation*new_rotate;
        }
        else
        {
            osg::Matrix rotation_matrix;
            rotation_matrix.makeRotate(_rotation);

            osg::Vec3d lookVector = -getUpVector(rotation_matrix);
            osg::Vec3d sideVector = getSideVector(rotation_matrix);
            osg::Vec3d upVector = getFrontVector(rotation_matrix);
            
            osg::Vec3d localUp(0.0f,0.0f,1.0f);

            osg::Vec3d forwardVector = localUp^sideVector;
            sideVector = forwardVector^localUp;

            forwardVector.normalize();
            sideVector.normalize();
            
            osg::Quat rotate_elevation;
            rotate_elevation.makeRotate(dy,sideVector);

            osg::Quat rotate_azim;
            rotate_azim.makeRotate(-dx,localUp);
            
            _rotation = _rotation * rotate_elevation * rotate_azim;
            
        }
        
        return true;

    }
    else if (buttonMask==GUIEventAdapter::MIDDLE_MOUSE_BUTTON ||
        buttonMask==(GUIEventAdapter::LEFT_MOUSE_BUTTON|GUIEventAdapter::RIGHT_MOUSE_BUTTON))
    {
        return true;
    }
    else if (buttonMask==GUIEventAdapter::RIGHT_MOUSE_BUTTON)
    {

        // zoom model.

        double fd = _distance;
        double scale = 1.0f+dy;
        if (fd*scale>_minimumDistance)
        {

            _distance *= scale;

        } else
        {
            _distance = _minimumDistance;
        }
        
        return true;

    }

    return false;
}

bool NodeTrackerSpaceMouse::calcMovementSpaceMouse()
{
	// TRACKBALL mode is not possible, because it uses the mouseevent, which does not happen with Spacemouse.

   double dTX, dTY, dTZ;	// Only TZ is used later.
    _spaceMouse->getTranslations(dTX, dTY, dTZ);

    double dRX, dRY, dRZ;
    _spaceMouse->getRotations(dRX, dRY, dRZ);

	//OSG_NOTIFY( osg::INFO ) << "Recieved Spacemousevalues: dRX:" << dRX << ", dRY:" << dRY << ", dRZ:" << dRZ << ", dTX:" << dTX << std::endl;

	if(!_ah_init)
	{
		TZ=dTZ;
		RX=dRX;
		RY=dRY;
		RZ=dRZ;
		_ah_init=true;
	}

	if (_autohoming)	// The factors are required to allow macroscopic movements.
	{
		TZ=dTZ*5;
		if (!_distanceDependendAV)
		{
			RX=dRX*100;
			RY=dRY*100;
			RZ=dRZ*100;
		}
	}
	else		// NOT Autohoming
	{
		TZ+=dTZ;
		//OSG_NOTIFY( osg::INFO ) << "Stored Spacemousevalues: RX:" << RX << ", RY:" << RY << ", RZ:" << RZ << ", TX:" << TX << std::endl;
		if (_distanceDependendAV)
		{
			RX+=(dRX*800/TZ);
			RY+=(dRY*800/TZ);
			RZ+=(dRZ*800/TZ);
		}
		else
		{
			RX+=dRX;
			RY+=dRY;
			RZ+=dRZ;
		}
	}

    osg::Vec3d nodeCenter;
    osg::Quat nodeRotation;
    computeNodeCenterAndRotation(nodeCenter, nodeRotation);

    // ROTATION PART
    if (_rotationMode==TRACKBALL)
    {
		// return if less then two events have been added.
		if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

        // rotate camera.
        osg::Vec3 axis;
        double angle;

        double px0 = _ga_t0->getXnormalized();
        double py0 = _ga_t0->getYnormalized();

        double px1 = _ga_t1->getXnormalized();
        double py1 = _ga_t1->getYnormalized();


        trackball(axis,angle,px1,py1,px0,py0);

        osg::Quat new_rotate;
        new_rotate.makeRotate(angle,axis);

        _rotation = _rotation*new_rotate;
    }
    else
    {
        osg::Matrix rotation_matrix;
        rotation_matrix.makeRotate(_rotation);

        osg::Vec3d lookVector = -getUpVector(rotation_matrix);
        osg::Vec3d sideVector = getSideVector(rotation_matrix);
        osg::Vec3d upVector = getFrontVector(rotation_matrix);
        
        osg::Vec3d localUp(0.0f,0.0f,1.0f);

        osg::Vec3d forwardVector = localUp^sideVector;
        sideVector = forwardVector^localUp;

        forwardVector.normalize();
        sideVector.normalize();
        
        osg::Quat rotate_elevation;
		rotate_elevation.makeRotate(osg::DegreesToRadians(RX*90), sideVector);

        osg::Quat rotate_azim;
		rotate_azim.makeRotate(osg::DegreesToRadians(RY*90), localUp);
        
        _rotation = _rotation * rotate_elevation * rotate_azim;
        
    }

    // TRANSLATION PART
    double scale = 1.0f+TZ/100;
	if (_distance*scale>_minimumDistance)
    {

        _distance *= scale;

    } else
    {
		TZ = (_minimumDistance/_distance - 1)*100;	// Reset TZ to the value f(_minimalDistance, _distance, scale-function). 
        _distance = _minimumDistance;
    }
    return true;
}

void NodeTrackerSpaceMouse::clampOrientation()
{
}


/*
 * This size should really be based on the distance from the center of
 * rotation to the point on the object underneath the mouse.  That
 * point would then track the mouse as closely as possible.  This is a
 * simple example, though, so that is left as an Exercise for the
 * Programmer.
 */
const float TRACKBALLSIZE = 0.8f;

/*
 * Ok, simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 *
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 */
void NodeTrackerSpaceMouse::trackball(osg::Vec3& axis,double & angle, double  p1x, double  p1y, double  p2x, double  p2y)
{
    /*
     * First, figure out z-coordinates for projection of P1 and P2 to
     * deformed sphere
     */

    osg::Matrix rotation_matrix(_rotation);


    osg::Vec3d uv = osg::Vec3d(0.0,1.0,0.0)*rotation_matrix;
    osg::Vec3d sv = osg::Vec3d(1.0,0.0,0.0)*rotation_matrix;
    osg::Vec3d lv = osg::Vec3d(0.0,0.0,-1.0)*rotation_matrix;

    osg::Vec3d p1 = sv*p1x+uv*p1y-lv*tb_project_to_sphere(TRACKBALLSIZE,p1x,p1y);
    osg::Vec3d p2 = sv*p2x+uv*p2y-lv*tb_project_to_sphere(TRACKBALLSIZE,p2x,p2y);

    /*
     *  Now, we want the cross product of P1 and P2
     */

// Robert,
//
// This was the quick 'n' dirty  fix to get the trackball doing the right 
// thing after fixing the Quat rotations to be right-handed.  You may want
// to do something more elegant.
//   axis = p1^p2;
axis = p2^p1;
    axis.normalize();

    /*
     *  Figure out how much to rotate around that axis.
     */
    double t = (p2-p1).length() / (2.0*TRACKBALLSIZE);

    /*
     * Avoid problems with out-of-control values...
     */
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    angle = inRadians(asin(t));

}


/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
double NodeTrackerSpaceMouse::tb_project_to_sphere(double  r, double  x, double  y)
{
    float d, t, z;

    d = sqrt(x*x + y*y);
                                 /* Inside sphere */
    if (d < r * 0.70710678118654752440)
    {
        z = sqrt(r*r - d*d);
    }                            /* On hyperbola */
    else
    {
        t = r / 1.41421356237309504880;
        z = t*t / d;
    }
    return z;
}

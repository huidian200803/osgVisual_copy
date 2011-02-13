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

#include <osg/Node>
#include <osg/Notify>
#include <osg/ArgumentParser>
#include <osgViewer/Viewer>

#include <visual_dataIO.h>

// OSG Eventhandler
#include <osgViewer/ViewerEventHandlers>

// OSG manipulator
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/NodeTrackerManipulator>

// Spacenavigator manipulator
#ifdef USE_SPACENAVIGATOR
#include <manip_spaceMouse.h>
#include <manip_nodeTrackerSpaceMouse.h>
#include <manip_freeSpaceMouse.h>
#endif

// Object mounted manipulator
#include <manip_objectMounted.h>




namespace osgVisual
{ 

class core_manipulator : public osg::Node
{
	#include <leakDetection.h>
public:
	core_manipulator();
	~core_manipulator(){};

	bool init(osgViewer::Viewer* viewer, osg::ArgumentParser& arguments, std::string configFilename, osg::Node* rootNode);
	void shutdown();
	void trackNode( osg::Node* node_ );
	void trackNode( int trackingID );
	int getCurrentTrackingID(){return _currentTrackingID;};

private:

#ifdef USE_SPACENAVIGATOR
	/**
	 * Spacemouse node tracker manipulator
	 */ 
	osg::ref_ptr<NodeTrackerSpaceMouse> _mouseTrackerManip;

	/**
	 * Space mouse hardware driver instance
	 */ 
	SpaceMouse* _mouse;
#endif

	/**
	 * This Matrix manipulator is used for controlling Camera by Nodes.
	 */ 
	osg::ref_ptr<objectMountedManipulator> _objectMountedCameraManip;

	/**
	 * Classical OSG NodeTrackerManipulator
	 */ 
	osg::ref_ptr<osgGA::NodeTrackerManipulator> _nt;

	/**
	 * ID of the currently tracked ID
	 */ 
	int _currentTrackingID;

	/**
	 * Pointer to the  scene root node
	 */ 
	osg::ref_ptr<osg::Node> _rootNode;
};

}	// END NAMESPACE
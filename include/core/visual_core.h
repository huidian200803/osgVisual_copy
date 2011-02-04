#pragma once
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

// XML Parser
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


#include <osg/Referenced>
#include <osg/Notify>

#include <osgViewer/Viewer>

#include <osgDB/ReadFile>

// OSG eventhandler
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

#ifdef USE_DISTORTION
// Distortion
#include <visual_distortion.h>
#endif

#ifdef USE_SKY_SILVERLINING
// Sky
#include <visual_skySilverLining.h>
#endif

// DataIO
#include <visual_dataIO.h>
#include <object_updater.h>

// visual_object
#include <visual_object.h>

// visual_hud
#include <visual_hud.h>
#include <visual_debug_hud.h>

// visual_draw2D
#include <visual_draw2D.h>
#include <visual_draw3D.h>

// visual util
#include <visual_util.h>

// visual_vista2D
#ifdef USE_VISTA2D
#include <visual_vista2D.h>
#endif

// Test
#include <dataIO_transportContainer.h>
#include <osgDB/OutputStream>
#include <iostream>




namespace osgVisual
{

class visual_core : public osg::Referenced
{
	#include <leakDetection.h>

public:
	visual_core(osg::ArgumentParser& arguments_);
	void initialize();
	void shutdown();

	void addManipulators();
	void parseScenery(xmlNode * a_node);
	bool loadTerrain(osg::ArgumentParser& arguments_);
	bool checkCommandlineArgumentsForFinalErrors();

	void setupScenery();
	void trackNode( osg::Node* node_ );
	
protected:
	virtual ~visual_core(void);


private:
	void mainLoop();

	/**
	 * Argument object, with contains all commandline arguments, which where called during programm sstartup
	 */ 
	osg::ArgumentParser& arguments;

	/**
	 * This CSN is at the top node on the complete scenery. Only the nodes for distortion are located above this node.
	 */ 
	osg::ref_ptr<osg::CoordinateSystemNode> rootNode;

	/**
	 * Reference pointer to the viewer instance. The viewer is the core of the graphical subsystem.
	 */ 
	osg::ref_ptr<osgViewer::Viewer> viewer;

	/**
	 * XML configuration filename.
	 */
	std::string configFilename;


#ifdef USE_SPACENAVIGATOR
	/**
	 * Spacemouse node tracker manipulator
	 */ 
	osg::ref_ptr<NodeTrackerSpaceMouse> mouseTrackerManip;

	/**
	 * Space mouse hardware driver instance
	 */ 
	SpaceMouse* mouse;
#endif

	/**
	 * This Matrix manipulator is used for controlling Camera by Nodes.
	 */ 
	osg::ref_ptr<objectMountedManipulator> objectMountedCameraManip;


#ifdef USE_SKY_SILVERLINING
	/**
	 * Silverlining Sky instance
	 */ 
	osg::ref_ptr<visual_skySilverLining> sky;

	xmlNode* skySilverliningConfig;
#endif

#ifdef USE_DISTORTION
	/**
	 * Distortion instance.
	 */ 
	osg::ref_ptr<visual_distortion> distortion;

	xmlNode* distortionConfig;
#endif

	osg::ref_ptr<visual_object> testObj;

	osg::ref_ptr<osgGA::NodeTrackerManipulator> nt;

	osg::ref_ptr<visual_debug_hud> hud;
};

}	// END NAMESPACE
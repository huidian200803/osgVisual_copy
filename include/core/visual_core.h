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

// Manipulator and eventhandler
#include <core_manipulator.h>


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

	void parseScenery(xmlNode * a_node);
	bool loadTerrain(osg::ArgumentParser& arguments_);
	bool checkCommandlineArgumentsForFinalErrors();

	void setupScenery();

protected:
	/**
	 * \brief Destrcutor
	 * 
	 */ 
	virtual ~visual_core(void);

private:
	/**
	 * \brief This function starts the main rendering loop
	 * 
	 */ 
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

	osg::ref_ptr<visual_debug_hud> hud;

	osg::ref_ptr<core_manipulator> manipulators;
};

}	// END NAMESPACE
#pragma once
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

#include <sstream>
#include <iostream>
#include <iomanip>

#include <osg/NodeCallback>

#include <visual_draw2D.h>
#include <visual_util.h>


namespace osgVisual
{

/**
 * \brief This class prints debug information about LAT, LON, HAT, HOT on screen.
 * 
 * 
 * @author Torben Dannhauer
 * @date  Jan 2010
 */ 
class visual_debug_hud : public osg::Geode
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Empty Constructor.
	 * 
	 */ 
	visual_debug_hud(void);

	/**
	 * \brief Empty Destructor.
	 * 
	 */ 
	~visual_debug_hud(void);

	/**
	 * \brief This function initializes the HUD.
	 * 
	 * @param viewer_ : Pointer to the viewer instance to get screen size and screen width.
	 * @param rootNode_ : Pointer to the rootnode of the Scene, which should be a CSN
	 * @return : True if initialization was successful.
	 */ 
	bool init(osgViewer::Viewer *viewer_, osg::CoordinateSystemNode* rootNode_ );

	void shutdown();

private:
	/**
	 * \brief This function contains the code to display in the HUD.
	 * 
	 * 
	 * @return Pointer to the Geode which contains the HUD content.
	 */ 
	osg::ref_ptr<osg::Geode> addContent();

	/**
	 * This variable contains the width of the rendering screen during the initialization of this node.
	 */ 
	int screen_width;

	/**
	 * This variable contains the height of the rendering screen during the initialization of this node.
	 */ 
	int screen_height;



	class HudUpdateCallback : public osg::NodeCallback
	{
	public:
		/**
		 * \brief Constructor
		 * 
		 * @param csn_ : Pointer to the Coordinate System Node. Necessary to extract lat, lon and height of the camera position.
		 * @param sceneCamera_ : Pointer to the scene camera (undistorted camera, type PRE_RENDER)
		 * @param sky_ : Pointer to the sky system.
		 */ 
		HudUpdateCallback(osg::CoordinateSystemNode* csn_, osg::Camera* sceneCamera_)
			: csn(csn_), sceneCamera(sceneCamera_) {};

		/**
		 * \brief This function is executed as callback during traversal. It updates values to display.
		 * 
		 */ 
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	private:
		/**
		 * Referenced pointer to the Coordinate System Node. Necessary to extract lat, lon and height of the camera position.
		 */ 
		osg::ref_ptr<osg::CoordinateSystemNode> csn;
		
		/**
		 * Referenced pointer to the scene camera (undistorted camera, type PRE_RENDER)
		 */ 
		osg::ref_ptr<osg::Camera> sceneCamera;

	};	// Nested class END

	/**
	 * Referenced Pointer to the updatecallback, which is installed at this node. 
	 */ 
	osg::ref_ptr<HudUpdateCallback> updateCallback;

	bool isInitialized;

	osg::ref_ptr<osgText::Text> textLat, textLon, textAlt, textHat, textHot;

	// Friend classes
	friend class HudUpdateCallback; // Damit der Callback auf alle Member zugreifen kann wie wenn er in der Klasse sitzen würde.
}; 

}	// END NAMESPACE
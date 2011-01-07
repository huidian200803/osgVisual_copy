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

#include <osg/Referenced>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/CoordinateSystemNode>
#include <osgText/Text>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

#include <visual_util.h>


namespace osgVisual
{
	#include <leakDetection.h>

/**
 * \brief This class is the interface to place 2D drawings in screen coordinates over the rendered 3D scene, but before POST_RENDER actions like distortion.
 * 
 * This class is realized as singleton to make sure that only one interface controls the 2D drawing.
 * It is derived vom osg::Geode to be includable into the scenegraph. At every frame, callbacks are possible to use for updating 2D drawings dynamically.
 * 
 * If you wish dynamic content in your drawings, implement callbacks in your content geodes. This class is not capable of callbacks, because it is not added into the scenegraph.
 * 
 * 
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 
class visual_draw2D : public osg::Referenced
{
private:
	/**
	 * \brief Constructor : Private accessible to prevent instantiation of thsi class by external caller.
	 * 
	 */ 
	visual_draw2D(void);

	/**
	 * \brief Copy-Constructor: Private accessible to prevent copies (from outside) of this class.
	 * 
	 * @param cc : Instance to copy. Not relevant because this funtion is not implemented. If this function is not definied,
	 * the compiler would create a public accessible dafault copy constructor.
	 */ 
	visual_draw2D(const visual_draw2D& cc);

public:
	/**
	 * \brief Destructor: Public accessible to allow external functions to clean up this singleton.
	 * 
	 */ 
	~visual_draw2D(void);

	/**
	 * \brief This function returns the singleton instance for usage.
	 * 
	 * @return Pointer to the instance.
	 */ 
	static visual_draw2D* getInstance();

	/**
	 * \brief This function initilizes the 2DDraw interface.
	 * 
	 * @param sceneGraphRoot_ : Scene root to add the 2D drawings to.
	 * @param viewer_ : Applications main viewer to get the screen resolution
	 * @return : True if successful.
	 */ 
	bool init( osg::CoordinateSystemNode *sceneGraphRoot_, osgViewer::Viewer *viewer_ );

	/**
	 * \brief This function shut "drawing interface down.
	 * 
	 * It removes all drawables and shuts them down to allow them to uninstall their updater. 
	 * 
	 */ 
	void shutdown();

	/**
	 * \brief Call this function to add a Geode with drawables which contain 2d content.
	 * 
	 * @param content_ : Geode with drawables to add.
	 * @param name_ : Name to set the geode to for further identification.
	 * @param renderBinDetail_ : Renderbin to add the content to. 99 is the default for HUD
	 * @return : true of adding was successful.
	 */ 
	bool addDrawContent( osg::Geode* content_, std::string name_,int renderBinDetail_ = 99 );

	/**
	 * \brief This function returns a content geode if the name matches the specified one.
	 * 
	 * @param name_ : content to search after.
	 * @return : Pointer to the found geode. If no Node with that name is found, NULL is returned.
	 */ 
	osg::Geode* getDrawContent( std::string name_ );

	/**
	 * \brief This function removes Drawcontent specified by name_.
	 * 
	 * @param name_ : Name of drawcontent to remove.
	 * @return : Returns true if drawcontent was found and successfully removed.
	 */ 
	bool removeDrawContent( std::string name_ );
	
	/**
	 * \brief This function removes all drawContents.
	 * 
	 */ 
	void removeAllDrawContents();

	/**
	 * \brief This function returns the number of registered drawContents.
	 * 
	 * @return : Number of registered drawContents.
	 */ 
	inline int getDrawContentNum();

private:
	/**
	 * This camera ist NESTED_RENDER and renders all "D drawings.
	 * 
	 * It is rendered after the 3D scene but before POST_RENDER cameras like distortion.
	 */ 
	osg::ref_ptr<osg::Camera> camera;

	/**
	 * This Modelviewematrix is the parent for all geodes which contain 2D drawings. To be known by the addContent() funtion, it is a classwide member attribute.
	 */ 
	osg::ref_ptr<osg::MatrixTransform> draw2DModelViewMatrix;

	/**
	 * This Projectionmatrix connects the global scenegraph with the 2D modelview matrix. It is used to disconnect the 2D drawing subgraph from the global scenegrpah on shutdown.
	 */ 
	osg::ref_ptr<osg::Projection> draw2DProjectionMatrix;

	/**
	 * This flag indicated whether the draw2D interface is initialized. 
	 */ 
	bool initialized;

	/**
	 * This variable contains the width of the rendering screen during the initialization of this node.
	 */ 
	int screen_width;

	/**
	 * This variable contains the height of the rendering screen during the initialization of this node.
	 */ 
	int screen_height;
};

}	// END NAMESPACE
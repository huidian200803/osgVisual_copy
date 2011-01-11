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

#include <string.h>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/CoordinateSystemNode>
#include <osg/ShapeDrawable>

#include <osgViewer/Viewer>

#include <OpenThreads/ReentrantMutex>

#include <osgUtil/LineSegmentIntersector>

#ifdef FUNFUNCTIONS_ENABLED
#ifdef WIN32
#include <osgViewer/api/Win32/GraphicsHandleWin32>
#include "C:\\Program Files/Microsoft SDKs/Windows/v6.1/Include/dwmapi.h"
#pragma comment ( lib, "C:\\Program Files/Microsoft SDKs/Windows/v6.1/Lib/dwmapi.lib" )
#endif
#endif



namespace osgVisual
{ 

/**
 * \brief This class provides utility functions. 
 * 
 * The utility functions are implemented as static functions if possible for easy use in other classes.
 * 
 * \todo check why lineIntersector is unstable, check if newer and better system is available.
 * 
 * @author Torben Dannhauer
 * @date  Jul 2009
 */ 
class util
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Construtor
	 * 
	 */ 
	util();

	/**
	 * \brief Destructor
	 * 
	 */ 
	~util();

	/**
	 * \brief This functions searches in the scene graph for a node with a name
	 * 
	 * This function searches in the scene graph unter the node "currNode" for a node named like the given search name.
	 * The functions returns the first node with that name.
	 * 
	 * @param searchName_ : Nodename to search for
	 * @param currNode_ : Scene graph to search in.
	 * @return : if Node found: Pointer to the node. If not found: NULL
	 */ 
	static osg::Node* findNamedNode(const std::string& searchName_, osg::Node* currNode_);

	/**
	 * \brief This function creates a red cylinder of the specified size tor testing purposes.
	 * 
	 * @param length_ : Length of the cylinder in meter.
	 * @param width_ : Diameter of the cylinder in meter.
	 * @param offset_ : Offset of the cylinder's origin in meter.
	 * @return : Referenced Pointer off the Geode with the created cylinder.
	 */ 
	static osg::ref_ptr<osg::Geode> getDemoCylinder(double length_, double width_, osg::Vec3 offset_ = osg::Vec3(0.0, 0.0, 0.0) );
	
	/**
	 * \brief This function creates a red sphere of the specified size tor testing purposes.
	 * 
	 * @param radius_ : Radius of the sphere in meter.
	 * @param offset_ : Offset of the sphere's origin in meter.
	 * @return : Referenced Pointer off the Geode with the created cylinder.
	 */ 
	static osg::ref_ptr<osg::Geode> getDemoSphere(double radius_, osg::Vec3 offset_ = osg::Vec3(0.0, 0.0, 0.0) );

	/**
	 * \brief This functions searches the scene graph for intersections of a line between two points and the model.
	 * 
	 * @param start_ : Point 1 for the intersection line
	 * @param end_ : Point 2 for the intersection line
	 * @param intersection_ : vektor to the intersection point if any is found.
	 * @param node_ : Node which is the root of the scene graph to check for intersections
	 * @param intersectTraversalMask_ : Bitwise mask wich controls which nodes should be checked for intersections (e.g. to check only terrain but not clouds or sky)
	 * @return returns : True if an intersection is found. 
	 */ 
	static bool intersect(const osg::Vec3d& start_, const osg::Vec3d& end_, osg::Vec3d& intersection_, osg::Node* node_, osg::Node::NodeMask intersectTraversalMask_=0xffffffff );

	/**
	 * \brief This function queries the height of terrain (hot) at a specified location.
	 * 
	 * @param hot_ : Reference to write the calculated hot into.
	 * @param rootNode_ : Node which is the root of the scene graph to calculate the hot.
	 * @param lat_ : Latitude of the position to calculate the hot.
	 * @param lon_ : Longitude of the position to calculate the hot.
	 * @param traversalMask_ : Bitwise mask wich controls which nodes should be checked for hot (e.g. to check only terrain but not clouds or sky)
	 * @return : True if hot calculation successful.
	 */ 
	static bool queryHeightOfTerrain(double& hot_, osg::Node* rootNode_, double lat_, double lon_, osg::Node::NodeMask traversalMask_=0xffffffff);

	/**
	 * \brief This function queries the height above terrain (hat) at a specified location in WGS84 coordinates.
	 * 
	 * @param hat_ : Reference to write the calculated hat into.
	 * @param rootNode_ : Node which is the root of the scene graph to calculate the hat.
	 * @param lat_ : Latitude of the position to calculate the hat. (e.g. lat of camera)
	 * @param lon_ : Longitude of the position to calculate the hat.(e.g. lon of camera)
	 * @param height_ : Height of the position to calculate the hat.(e.g. height of camera).
	 * @param traversalMask_ : Bitwise mask wich controls which nodes should be checked for hat (e.g. to check only terrain but not clouds or sky)
	 * @return :  True if hat calculation successful.
	 */ 
	static bool queryHeightAboveTerrainInWGS84(double& hat_, osg::Node* rootNode_, double lat_, double lon_, double height_, osg::Node::NodeMask traversalMask_=0xffffffff);

	/**
	 * \brief This function queries the height above terrain (hat) at a specified location in world coordinates.
	 * 
	 * @param hat_ : Reference to write the calculated hat into.
	 * @param rootNode_ : Node which is the root of the scene graph to calculate the hat.
	 * @param x_ : First value of the pointer which points to the position to calculate hat for.
	 * @param y_ : Second value of the pointer which points to the position to calculate hat for. 
	 * @param z_ : Third value of the pointer which points to the position to calculate hat for.
	 * @param traversalMask_ : Bitwise mask wich controls which nodes should be checked for hat (e.g. to check only terrain but not clouds or sky)
	 * @return :  True if hat calculation successful. 
	 */ 
	static bool queryHeightAboveTerrainInWorld(double& hat_, osg::Node* rootNode_, double x_, double y_, double z_, osg::Node::NodeMask traversalMask_=0xffffffff);

	/**
	 * \brief This function calculates the earth readius (vector length in world coordiantes from origin to location) at the specified location.
	 * 
	 * @param lat_ : Latitude in radians.
	 * @param lon_ : Longitude in radians.
	 * @param rootNode_ : Root node of the scene which coordinate system node should be used.
	 * @param radius_ : Calculated earth readius in meter.
	 * @return : True if calculation successful.
	 */ 
	static bool calculateEarthRadiusAtWGS84Coordinate(double lat_, double lon_, osg::Node* rootNode_, double& radius_);

	/**
	 * \brief This function converts a WGS84 position into global OpenGL XYZ coordinates.
	 * 
	 * @param lat_ : Latitude of the position to calculate XYZ.
	 * @param lon_ : Longitude of the position to calculate XYZ.
	 * @param height_ : Height of the position to calculate XYZ.
	 * @param rootNode_ : Node which is the root of the scene graph to calculate XYZ.
	 * @param x_ : X Coordinate in global OpenGL coordinates.
	 * @param y_ : Y Coordinate in global OpenGL coordinates.
	 * @param z_ : Z Coordinate in global OpenGL coordinates.
	 * @return : True if calculation successful.
	 */ 
	static bool calculateXYZAtWGS84Coordinate(double lat_, double lon_, double height_, osg::Node* rootNode_, double& x_, double& y_, double& z_);

	/**
	 * \brief This function calculates lat, lon and height of a specified camera by a specified coordinate system node. 
	 * 
	 * @param camera_ : Camera node which position should be calculated.
	 * @param rootNode_ : rootNode of the scene.
	 * @param lat_ : Latitude variable to save value in.
	 * @param lon_ : Longitude variable to save value in.
	 * @param height_ : Height: variable to save value in.
	 * @return : True if calculation was successful.
	 */ 
	static bool getWGS84ofCamera( osg::Camera* camera_, osg::Node* rootNode_, double& lat_, double& lon_, double& height_ ); 

	/**
	 * \brief This function returns the global XYZ coordinates of the specified camera.
	 * 
	 * @param camera_ : Camera to return the position.
	 * @param x_ : X coordinate.
	 * @param y_ : Y coordinate.
	 * @param z_ : Z coordinate.
	 */ 
	static void getXYZofCamera( osg::Camera* camera_, double& x_, double& y_, double& z_ ); 

	/**
	 * \brief This function disables the close button on the openGL's windowdecoration
	 * 
	 * To be honest, this function is only to play, I have found it on http://forum.openscenegraph.org and found it interesting enought to play along ;)
	 * 
	 * @param viewer_ : Pointer to the applications viewer
	 * @return : true if successful
	 */ 
	static bool removeClosebuttonOnGLWindow(osgViewer::Viewer* viewer_);

	/**
	 * \brief This function make the backgroud of the GL window blurred transparent, if an backgroud with alpha is set.
	 * 
	 * To be honest, this function is only to play, I have found it on http://forum.openscenegraph.org and found it interesting enought to play along ;)
	 * 
	 * @param viewer_ : Pointer to the applications viewer
	 * @return : true if successful
	 */ 
	static bool setTransparentWindowBackground(osgViewer::Viewer* viewer_);

	/**
	 * \brief Parses for the XML node of the specified module. The caller has to clean up the xmlDoc and the parser, beside it returns NULL because the queried modules is not configured.
	 * 
	 * To clean up, call this two functions:
	 *  xmlFreeDoc(doc);
	 *  xmlCleanupParser();
	 * 
	 * Example to use this function: 
	 *	xmlDoc* tmpDoc;
	 *  xmlNode* yourNode = util::getModuleXMLConfig( configFilename, "core", tmpDoc );
	 *  // use yourNode
	 *  if(yourNode)
	 *  {
	 *     xmlFreeDoc(tmpDoc); xmlCleanupParser();
	 *  }
	 * 
	 * @param configFilename : Config Filename to parse.
	 * @param moduleName : Module name to search for.
	 * @param doc : xmlDoc to use. Must be created outside that the caller can clean it up.
	 * @param disabled : Contains after return if the module was disabled. Only true if valid configuration and definitely disabled.
	 * @return : NULL on error, otherwise pointer to the xmlNode of the queried module.
	 */ 
	static xmlNode* getModuleXMLConfig(std::string configFilename, std::string moduleName, xmlDoc*& doc, bool& disabled);

	/**
	 * \brief Parses for the XML node of the scenery configuration. The caller has to clean up the xmlDoc and the parser, beside it returns NULL because the queried modules is not configured.
	 * 
	 * @param configFilename : Config Filename to parse.
	 * @param doc : xmlDoc to use. Must be created outside that the caller can clean it up.
	 * @return : NULL on error, otherwise pointer to the xmlNode of the scenery configuration.
	 */ 
	static xmlNode* getSceneryXMLConfig(std::string configFilename, xmlDoc*& doc);

	/**
	 * \brief This function returns the path of the terrainfile specified in the configuration file.
	 * 
	 * @param configFilename : Filename of the XML configuration file.
	 * @return : On error an empty string, otherwise the terrain path specified in the configuration file.
	 */ 
	static std::string getTerrainFromXMLConfig(std::string configFilename);

	/**
	 * \brief This function returns the path of the animationpath file specified in the configuration file.
	 * 
	 * @param configFilename : Filename of the XML configuration file.
	 * @return : On error an empty string, otherwise the path of the animationpath specified in the configuration file.
	 */ 
	static std::string getAnimationPathFromXMLConfig(std::string configFilename);

private: 
	/**
	 * \brief This functions checks a list of nodes and all of its children for the specified module configuration.
	 * 
	 * This function is used by getModuleXMLConfig() and works recursive
	 * 
	 * @param node : Node to search in
	 * @param moduleName :Module name to search for.
	 * @param disabled : Contains after return if the module was disabled. Only true if valid configuration and definitely disabled.
	 * @return : NULL if the module configuration was not found, otherwise pointer to the XML Node with the configuration for the specified module. 
	 */ 
	static xmlNode* checkXMLNodeChildrenForModule(xmlNode* node, std::string moduleName, bool& disabled);

	/**
	 * \brief This functions checks a list of nodes and all of its children for the scenery configuration.
	 * 
	 * @param node : Node to search in
	 * @return : NULL if the module configuration was not found, otherwise pointer to the XML Node with the configuration for the specified module. 
	 */ 
	static xmlNode* checkXMLNodeChildrenForScenery(xmlNode* node);
};

} //END NAMESPACE
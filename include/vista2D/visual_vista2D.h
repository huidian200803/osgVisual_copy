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

// Vista 2D
#include "VistaView.h"

// C++ libraries
#include <string>

// XML Parser
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

// OSG Includes
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/CoordinateSystemNode>

#include <osgDB/FileUtils>

// osgVisual Includes
#include <visual_util.h>

namespace osgVisual
{

/**
 * \brief This class integrates Vista2D by Wetzel Technology into OSG.
 * 
 * It wraps vista2D's OpenGL interface to be usable as a standart OSG Node. The Vista2D nodekit performs auto-linking
 * againt the Vista2D library, therefore only three shared libraries have to be copied to the binary folder of the project.
 * 
 * Vista2D ist a software to design HID. This Nodekit enables OSG to display Vista2D project files.
 * 
 * Please copy the following files to the binary folder of you application if you intend to use this nodekit:
 * libView.dll
 * fontLib.dll
 * freetype6.dll
 * 
 * The implementation of this class is based on osg::Drawable. The drawimplementations contains the Vista2D wrap.
 *  
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 
class visual_vista2D : public osg::Drawable
{
public:
	/**
	 * \brief Construtor: Creates a Vista2D environment which should be wrapt by this node.
	 * 
	 */ 
	visual_vista2D(void);

	/**
	 * \brief Empty destructor.
	 * 
	 */ 
	~visual_vista2D(void);

	/**
	 * \brief This static function must be called to instantiate a vista2D project.
	 * 
	 * @param sceneGraphRoot_ : Coordinate system node to display the Vista2D project in.
	 * @param configFileName : Filename of the XML configuration file.
	 * @return : True if successful.
	 */ 
	bool init( osg::CoordinateSystemNode *sceneGraphRoot_, std::string configFileName );

	/**
	 * \brief This function is requiered by subclassing osg::Drawable.
	 * 
	 * @return : Cloned object.
	 */ 
	osg::Object* cloneType() const;

	/**
	 * \brief This function is requiered by subclassing osg::Drawable.
	 * 
	 * @param : Copyoperator how to copy the object
	 * @return : Cloned object.
	 */ 
	osg::Object* clone(const osg::CopyOp&) const;

private:

	bool processXMLConfiguration();

	/**
	 * \brief This function initialized the visual_vista2D object after instantiation by createVistaOverlay()
	 */ 
	void startVista2D();

	/**
	 * \brief This function implements the pure OpenGL draw by calling Vista2D's draw funtion.
	 * 
	 * Because Vista2D manipulates OpenGLs stateset, this function saves the
	 * OSG statset at the beginning and restores it after the drawing of Vista2D 
	 * 
	 * @param renderInfo : Renderinfo of the drawing
	 */ 
	void drawImplementation(osg::RenderInfo& renderInfo) const;
	
	/**
	 * Pointer to Vista2D's view instance.
	 */ 
	Vista2D::VistaView* view;

	/**
	 * XML config filename
	 */ 
	std::string configFileName;

	/**
	 * Filename of the Vista2D project file.
	 */
	std::string vistaProjectfile;

	/**
	 * Should the background of the Vista2D project be painted?
	 */
	bool paintBackground;

	/**
	 * X-Position to draw.
	 */
	int position_x;

	/**
	 * Y-Position to draw.
	 */
	int position_y;

	/**
	 * Zoom factor to draw the project.
	 */
	double zoom;

	/**
	 * Flag if Vista2D should animate the project according to it's data sources.
	 */ 
	bool playanimation;
}; 

}	// END NAMESPACE
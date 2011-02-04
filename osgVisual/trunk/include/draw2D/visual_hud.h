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

#include <visual_draw2D.h>


namespace osgVisual
{

/**
 * \brief This class demonstrates how to build a Head Up Display.
 * 
 * Replace addContent() to integrate own content.
 * 
 * @todo : Check, if the update logic should be implemtented in draw2D or here in.
 * 
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 
class visual_hud : public osg::Geode
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Empty Constructor.
	 * 
	 */ 
	visual_hud(void);

	/**
	 * \brief Empty Destructor.
	 * 
	 */ 
	~visual_hud(void);

	/**
	 * \brief This function initializes the HUD.
	 * 
	 * @param viewer_ : Pointer to the viewer instance to get screen size and screen width.
	 * @return : True if initialization was successful.
	 */ 
	bool init(osgViewer::Viewer *viewer_ );

private:
	/**
	 * \brief This function contains the code to display in the HUD.
	 * 
	 * Subclass this function to integrate own content.
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
}; 

}	// END NAMESPACE
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
 * 
 * This file is based on the OSG example of the Silverlining SDK:
 * Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.
*/

#include <osg/Drawable>
#include <osgViewer/Viewer>
#include <SilverLining.h>



//class SilverLining::Atmosphere;
class Atmosphere;

namespace osgVisual
{
/**
 * \brief This class wraps Silverlining's endFrame() function which render clouds, fog and finishes the frame.
 * 
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 
class skySilverLining_cloudsDrawable : public osg::Drawable
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Constructor : Initializes attributes.
	 * 
	 */ 
    skySilverLining_cloudsDrawable();

	/**
	 * \brief Constructor : Initializes attributes.
	 * 
	 * @param view : Pointer to the viewer instance.
	 */ 
	skySilverLining_cloudsDrawable(osgViewer::Viewer* view);

	/**
	 * \brief This function checks if this object is the same kind as the specified one.
	 * 
	 * This function is required by subclassing osg::Drawable.
	 * 
	 * @param obj : object to compare this type to.
	 * @return : True if it is the same kind.
	 */ 
    virtual bool isSameKindAs(const Object* obj) const {
        return dynamic_cast<const skySilverLining_cloudsDrawable*>(obj)!=NULL;
    }

	/**
	 * \brief This function clones this class/type.
	 * 
	 * This function is required by subclassing osg::Drawable.
	 * 
	 * @return : Cloned instance.
	 */ 
    virtual Object* cloneType() const {
        return new skySilverLining_cloudsDrawable();
    }

	/**
	 * \brief This function clones this class/type.
	 * 
	 * This function is required by subclassing osg::Drawable.
	 * 
	 * @param copyop : Copy operator flag which indicates how to clone.
	 * @return : Cloned instance.
	 */ 
    virtual Object* clone(const osg::CopyOp& copyop) const {
        return new skySilverLining_cloudsDrawable();
    }

	/**
	 * \brief Draw implementation which will be drawn every frame and is the true payload of thes class.
	 * 
	 * @param renderInfo : rendering information.. (?)
	 */ 
    virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

protected:
	/**
	 * Pointer to the viewer instance.
	 */ 
    osgViewer::Viewer* _view;
};

}	// END NAMESPACE
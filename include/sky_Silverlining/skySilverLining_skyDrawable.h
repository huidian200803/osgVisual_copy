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
#include <osg/CoordinateSystemNode>
#include <osg/Fog>

#include <osgViewer/Viewer>
#include <OpenThreads/ScopedLock>
#include <OpenThreads/Mutex>

#include <skySilverLining_cloudLayerSlot.h>
#include <visual_util.h>

#include <SilverLining.h>
#include <iostream>

namespace osgVisual
{

class skySilverLining_atmosphereReference;

struct cloudlayerOrder 
{
	int slot;
	double lat;
	double lon;
	double baseLength;
	double baseWidth;
	double thickness;
	double baseHeight;
	double density;
	CloudTypes cloudtype;
	cloudLayerSlot* assocCloudLayerSlot;	// Pointer to the cloudlayerslot managed by visual_skySilverlining. 
};


class skySilverLining_skyDrawable : public osg::Drawable
{
	#include <leakDetection.h>
public:
	skySilverLining_skyDrawable();
	skySilverLining_skyDrawable(osgViewer::Viewer* view, osg::CoordinateSystemNode* csn_);

    virtual bool isSameKindAs(const Object* obj) const {
        return dynamic_cast<const skySilverLining_skyDrawable*>(obj)!=NULL;
    }
    virtual Object* cloneType() const {
        return new skySilverLining_skyDrawable();
    }
    virtual Object* clone(const osg::CopyOp& copyop) const {
        return new skySilverLining_skyDrawable();
    }

	void setSkyboxSize(double size) {_skyboxSize = size;}

    virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

	void addCloudLayerOrder(cloudlayerOrder newCL);

	void shutdown();

protected:
	void setLighting(SilverLining::Atmosphere *atm) const;
	void setSceneFog(SilverLining::Atmosphere *atmosphere) const;
	static void initializeSilverLining(skySilverLining_atmosphereReference *ar);
	void seedAndAddCloudLayers(SilverLining::Atmosphere *atmosphere);

    osgViewer::Viewer* _view;
	double _skyboxSize;

private:
	OpenThreads::Mutex cloudLayersToAddMutex;
	std::vector<cloudlayerOrder> pendingNewCloudLayers;
	bool newCloudLayersToAdd;
	osg::ref_ptr<osg::CoordinateSystemNode> sceneRoot;
};

}	// END NAMESPACE
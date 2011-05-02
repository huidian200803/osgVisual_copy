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

#include <skySilverLining_cloudsDrawable.h>
#include <SilverLining.h>
#include <skySilverLining_AtmosphereReference.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>


using namespace osgVisual;
using namespace SilverLining;

skySilverLining_cloudsDrawable::skySilverLining_cloudsDrawable()
        : osg::Drawable()
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

skySilverLining_cloudsDrawable::skySilverLining_cloudsDrawable(osgViewer::Viewer* view)
        : osg::Drawable()
        , _view(view)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
}

void skySilverLining_cloudsDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
	osgVisual::skySilverLining_atmosphereReference *ar = dynamic_cast<osgVisual::skySilverLining_atmosphereReference *>(renderInfo.getCurrentCamera()->getUserData());
	SilverLining::Atmosphere *atmosphere = 0;
	if (ar) atmosphere = ar->atmosphere;

	renderInfo.getState()->disableAllVertexArrays();

    if (atmosphere)
    {
        atmosphere->DrawObjects(true);
    }

	renderInfo.getState()->dirtyAllVertexArrays();
}

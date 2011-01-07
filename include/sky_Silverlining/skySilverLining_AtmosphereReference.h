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
 * 
 * This file is based on the OSG example of the Silverlining SDK:
 * Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.
*/

#include <SilverLining.h>
#include <osg/Referenced>


namespace osgVisual
{

/**
 * \brief This class holds the pointer to the atmosphere object and is bound to the rendering camera. 
 * 
 * This way all rendered elements can access the atmosphere. It also mirrors the atmosphere's init status 
 * to prevent crashing by access to the uninitialized object.
 * 
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 
class skySilverLining_atmosphereReference : public osg::Referenced
{
	#include <leakDetection.h>
public:
	/**
	 * Pointer to the atmosphere object.
	 */ 
	SilverLining::Atmosphere *atmosphere;

	/**
	 * This flag indicates whether the atmosphere is initialized and the pointer is valid.
	 */ 
	bool atmosphereInitialized;

	/**
	 * \brief Constructor to init the values.
	 * 
	 */ 
	skySilverLining_atmosphereReference() : atmosphereInitialized(false), atmosphere(0) {};
};

}	// END NAMESPACE
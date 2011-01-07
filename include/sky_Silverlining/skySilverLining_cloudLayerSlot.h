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
#include <string>


namespace osgVisual
{

/**
 * \brief This class represents a single cloudlayer slot which contains a cloud layer and corresponding additional information.
 * 
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 
class cloudLayerSlot
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Construtor, which initializes the cloudlayer information with valid default values.
	 * 
	 */ 
	cloudLayerSlot() 
	{
		slot_id = -1;
		used = false;
		enabled = false;
		cloudLayerHandle = -1;
		cloudLayerPointer = NULL;
		typeName = "";
	};

	/**
	 * ID of the cloudlayer slot.
	 */ 
	int slot_id;

	/**
	 * This flag indicates if the cloudlayer slot is in use.
	 * 
	 * @todo : Explain diff to 'enabled'.
	 */ 
	bool used;

	/**
	 * This flag indicates if the cloudlayer slot is enabled.
	 * 
	 * @todo : Explain diff to 'used'.
	 */ 
	bool enabled;

	/**
	 * This variable contains silverlining's cloudlayer handle-ID.
	 */ 
	int cloudLayerHandle;

	/**
	 * Pointer to the cloudlayer object.
	 */ 
	SilverLining::CloudLayer *cloudLayerPointer;

	/**
	 * Literal name of the cloudlayer type. e.g. CUMULUS_MEDIOCRIS.
	 */ 
	std::string typeName;
};

}	// END NAMESPACE
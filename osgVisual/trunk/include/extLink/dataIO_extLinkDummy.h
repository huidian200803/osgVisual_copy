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

#include <dataIO_extLink.h>
#include <osg/Notify>
#include <vector>

namespace osgVisual
{

/**
 * \brief This class is a dummy class for the externalLink.
 * 
 * All functions which are called always return true and send a debug string to stdout to announce that they were called.
 * 
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 
class dataIO_extLinkDummy :	public dataIO_extLink
{
public:
	dataIO_extLinkDummy(std::vector<dataIO_slot *>& dataSlots_);
	virtual ~dataIO_extLinkDummy(void);

	bool processXMLConfiguration(xmlNode* extLinkConfig_);
	bool init(xmlNode* configurationNode);
	void shutdown();

	bool readTO_OBJvalues();
	bool writebackFROM_OBJvalues();
	void provideSlots();
};

}	// END NAMESPACE

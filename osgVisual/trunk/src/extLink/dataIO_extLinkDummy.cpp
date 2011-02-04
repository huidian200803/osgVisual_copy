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

#include <dataIO_extLinkDummy.h>

#include <visual_dataIO.h>	// include in.cpp to avoid circular inclusion (visual_dataIO <-> extLinkDummy)


using namespace osgVisual;

dataIO_extLinkDummy::dataIO_extLinkDummy(std::vector<dataIO_slot *>& dataSlots_) : dataIO_extLink(dataSlots_)
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkDummy constructed" << std::endl;
}

dataIO_extLinkDummy::~dataIO_extLinkDummy(void)
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkDummy destroyed" << std::endl;
}

bool dataIO_extLinkDummy::init(xmlNode* configurationNode)
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkDummy init()" << std::endl;
	return true;
}

bool dataIO_extLinkDummy::processXMLConfiguration(xmlNode* extLinkConfig_)
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkDummy processXMLConfiguration()" << std::endl;
	return true;
}

void dataIO_extLinkDummy::shutdown()
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkDummy shutdown()" << std::endl;
}

bool dataIO_extLinkDummy::readTO_OBJvalues()
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkDummy readTO_OBJvalues()" << std::endl;

	return true;
}

bool dataIO_extLinkDummy::writebackFROM_OBJvalues()
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkDummy writebackFROM_OBJvalues()" << std::endl;

	return true;
}

void dataIO_extLinkDummy::provideSlots()
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkDummy provideSlots()" << std::endl;
}

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

#include "dataIO_clusterDummy.h"

using namespace osgVisual;

dataIO_clusterDummy::dataIO_clusterDummy()
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy constructed" << std::endl;
}

dataIO_clusterDummy::~dataIO_clusterDummy(void)
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy destructed" << std::endl;
}

bool dataIO_clusterDummy::init(xmlNode* configurationNode, osgViewer::Viewer* viewer_, clustermode clusterMode_, osgVisual::dataIO_transportContainer* sendContainer_, bool asAscii_)
{
	sendContainer = sendContainer_;
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy init();" << std::endl;
	return true;
}

bool dataIO_clusterDummy::processXMLConfiguration(xmlNode* clusterConfig_)
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy processXMLConfiguration();" << std::endl;
	return true;
}

void dataIO_clusterDummy::shutdown()
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy shutdown();" << std::endl;
}

bool dataIO_clusterDummy::sendTO_OBJvaluesToSlaves(osg::Matrixd viewMatrix_)
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy sendTO_OBJvaluesToSlaves()" << std::endl;

	return true;
}

bool dataIO_clusterDummy::readTO_OBJvaluesFromMaster()
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy readTO_OBJvaluesFromMaster()" << std::endl;

	return true;
}

void dataIO_clusterDummy::reportAsReadyToSwap()
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy reportAsReadyToSwap()" << std::endl;
}

bool dataIO_clusterDummy::waitForSwap()
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy waitForSwap()" << std::endl;

	return true;
}

bool dataIO_clusterDummy::waitForAllReadyToSwap()
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy waitForAllReadyToSwap()" << std::endl;

	return true;
}

bool dataIO_clusterDummy::sendSwapCommand()
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterDummy sendSwapCommand()" << std::endl;

	return true;
}
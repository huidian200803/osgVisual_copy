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
*/

#include <osg/Notify>
#include <osg/ArgumentParser>
#include <osgDB/Registry>
#include <osgDB/ReaderWriter>
#include <iostream>
#include <cstdlib>	// Clearscrean console

#include <dataIO_cluster.h>
#include <dataIO_clusterENet_implementation.h>

namespace osgVisual
{

/**
 * \brief This class is a ENet based cluster implementation class for osgVisuals cluster capabilities.
 * 
 * All functions which are called always return true and send a debug string to stdout to announce that they were called.
 * 
 * @author Torben Dannhauer
 * @date  July 2010
 */ 
class dataIO_clusterENet :	public dataIO_cluster
{
	#include <leakDetection.h>
public:
	dataIO_clusterENet();
	virtual ~dataIO_clusterENet(void);

	bool init(xmlNode* configurationNode, osgViewer::Viewer* viewer_, clustermode clusterMode_, osgVisual::dataIO_transportContainer* sendContainer_, bool asAscii_);
	bool processXMLConfiguration(xmlNode* clusterConfig_);
	void shutdown();

	void init();
	bool sendTO_OBJvaluesToSlaves(osg::Matrixd viewMatrix_);
	bool readTO_OBJvaluesFromMaster();
	void reportAsReadyToSwap();
	bool waitForSwap();
	bool waitForAllReadyToSwap();
	bool sendSwapCommand();

private:
	osg::ref_ptr<osgVisual::dataIO_clusterENet_implementation> enet_impl;
	std::string serverToConnect;
	osgVisual::dataIO_cluster::clustermode clusterMode;
	std::string receivedTransportContainer;

	osg::ref_ptr<osgDB::Options> readOptions;
	osg::ref_ptr<osgDB::Options> writeOptions;
	osg::ref_ptr<osgDB::ReaderWriter> rw;
};

} //END NAMESPACE
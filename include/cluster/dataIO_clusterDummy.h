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

#include <osg/notify>
#include <osg/ArgumentParser>

#include <dataIO_cluster.h>

namespace osgVisual
{

/**
 * \brief This class is a dummy class for the cluster.
 * 
 * All functions which are called always return true and send a debug string to stdout to announce that they were called.
 * 
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 
class dataIO_clusterDummy :	public dataIO_cluster
{
public:
	dataIO_clusterDummy();
	virtual ~dataIO_clusterDummy(void);

	void init( osg::ArgumentParser& arguments_, clustermode clusterMode_, osgVisual::dataIO_transportContainer* sendContainer_, bool compressionEnabled_, bool asAscii_ );
	void shutdown();

	void init();
	bool sendTO_OBJvaluesToSlaves();
	bool readTO_OBJvaluesFromMaster();
	void reportAsReadyToSwap();
	bool waitForSwap();
	bool waitForAllReadyToSwap();
	bool sendSwapCommand();
};

} //END NAMESPACE
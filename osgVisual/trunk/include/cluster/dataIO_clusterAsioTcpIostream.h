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

#include "dataIO_cluster.h"

#include <osg/notify>
#include <osg/ArgumentParser>

#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace osgVisual
{

/**
 * \brief This is a Boost ASIO iostream implementation of dataIO_cluster..
 * 
 * This class uses Boost ASIO and it iostream privider to fullfil the functions of dataIO_cluster.
 * It sends the clusterdata into the network to any connected client.
 * 
 * 
 * \todo Add getHorizonColor to transport it to slave for consistent fog.
 * @author Torben Dannhauer
 * @date  May 2010
 */ 
class dataIO_clusterAsioTcpIostream :	public dataIO_cluster
{

public:

	dataIO_clusterAsioTcpIostream();
	virtual ~dataIO_clusterAsioTcpIostream();

	void init( osg::ArgumentParser& arguments_, clustermode clusterMode_, osgVisual::dataIO_transportContainer* sendContainer_, bool compressionEnabled_, bool asAscii_ );
	void shutdown();
	bool sendTO_OBJvaluesToSlaves();
	bool readTO_OBJvaluesFromMaster();
	void reportAsReadyToSwap();
	bool waitForSwap();
	bool waitForAllReadyToSwap();
	bool sendSwapCommand();

private:
	/**
	 * This list contains the connections to communicate with in each frame. 
	 * 
	 * As slave, contains only one entry: connection to master.
	 * As Master,contains the connection to all slaves which use this connection.
	 */ 
	std::vector<boost::asio::ip::tcp::iostream> clientlists;

	boost::asio::io_service io_service;
};

}	// END NAMESPACE
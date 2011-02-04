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

#include "dataIO_clusterAsioTcpIostream.h"

using namespace osgVisual;

using boost::asio::ip::tcp;

dataIO_clusterAsioTcpIostream::dataIO_clusterAsioTcpIostream() 
{
	OSG_NOTIFY(osg::ALWAYS) << "dataIO_clusterAsioTcpIostream constructed" << std::endl;

	port = 5678;
	initialized = false;
}

dataIO_clusterAsioTcpIostream::~dataIO_clusterAsioTcpIostream()
{
	OSG_NOTIFY(osg::ALWAYS) << "dataIO_clusterAsioTcpIostream destructed" << std::endl;
}

void dataIO_clusterAsioTcpIostream::init( osg::ArgumentParser& arguments_, clustermode clusterMode_, osgVisual::dataIO_transportContainer* sendContainer_, bool compressionEnabled_, bool asAscii_ )
{
	OSG_NOTIFY(osg::ALWAYS) << "dataIO_clusterAsioTcpIostream initialized" << std::endl;

	clusterMode = clusterMode_;

	if(clusterMode == osgVisual::dataIO_cluster::MASTER)
	{
		

		tcp::endpoint endpoint(tcp::v4(), port);
		tcp::acceptor acceptor(io_service, endpoint);

										tcp::iostream stream;
									
										acceptor.accept(*stream.rdbuf());
										tcp::no_delay option(true);
										boost::system::error_code ec;
										stream.rdbuf()->set_option(option, ec);
										if (ec)
										{
											OSG_NOTIFY(osg::WARN) << "Error: Unable to set TCP_NODELAY option" << ec << std::endl;
											// An error occurred.
										}
	}

	if(clusterMode == osgVisual::dataIO_cluster::SLAVE)
	{

	}

	initialized = true;
}

void dataIO_clusterAsioTcpIostream::shutdown()
{
	OSG_NOTIFY(osg::ALWAYS) << "dataIO_clusterAsioTcpIostream shut down" << std::endl;

	if(initialized)
	{

	}
}

bool dataIO_clusterAsioTcpIostream::sendTO_OBJvaluesToSlaves()
{
	OSG_NOTIFY(osg::INFO) << "dataIO_clusterAsioTcpIostream::sendTO_OBJvaluesToSlaves()" << std::endl;

	return true;
}

bool dataIO_clusterAsioTcpIostream::readTO_OBJvaluesFromMaster()
{
	OSG_NOTIFY(osg::INFO) << "dataIO_clusterAsioTcpIostream::readTO_OBJvaluesFromMaster()" << std::endl;

	return true;
}

void dataIO_clusterAsioTcpIostream::reportAsReadyToSwap()
{
	OSG_NOTIFY(osg::INFO) << "dataIO_clusterAsioTcpIostream::reportAsReadyToSwap()" << std::endl;
}

bool dataIO_clusterAsioTcpIostream::waitForSwap()
{
	OSG_NOTIFY(osg::INFO) << "dataIO_clusterAsioTcpIostream::waitForAllReadyToSwap()" << std::endl;

	return true;
}

bool dataIO_clusterAsioTcpIostream::waitForAllReadyToSwap()
{
	OSG_NOTIFY(osg::INFO) << "dataIO_clusterAsioTcpIostream::waitForAllReadyToSwap()" << std::endl;

	return true;
}

bool dataIO_clusterAsioTcpIostream::sendSwapCommand()
{
	OSG_NOTIFY(osg::INFO) << "dataIO_clusterAsioTcpIostream::sendSwapCommand()" << std::endl;

	return true;
}
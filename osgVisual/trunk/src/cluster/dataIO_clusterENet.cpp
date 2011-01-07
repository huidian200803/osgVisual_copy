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

#include "dataIO_clusterENet.h"

using namespace osgVisual;

dataIO_clusterENet::dataIO_clusterENet()
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterENet constructed" << std::endl;

	serverToConnect = "unknown";
	hardSync = false;	// integrate into init()
	port = 12345;	// integrate into init()
}


dataIO_clusterENet::~dataIO_clusterENet(void)
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterENet destructed" << std::endl;
}


bool dataIO_clusterENet::init(xmlNode* configurationNode, osgViewer::Viewer* viewer_, clustermode clusterMode_, osgVisual::dataIO_transportContainer* sendContainer_, bool asAscii_)
{
	if (!configurationNode || !processXMLConfiguration(configurationNode))
		return false;

	OSG_NOTIFY( osg::ALWAYS ) << "clusterENet init();" << std::endl;
	
	// Store viewer
	viewer = viewer_;

	// Configure the clustermode
	clusterMode = clusterMode_;

	// store sendContainer
	sendContainer = sendContainer_;

	// Configure Compression and instantiate read/write-options
	std::string readOptionString = "";
	std::string writeOptionString = "";
	if(asAscii_)
	{
		readOptionString = "Ascii";
		writeOptionString = "Ascii";
	}
	if (compressionEnabled)
		writeOptionString+=" Compressor=zlib";
	readOptions = new osgDB::Options( readOptionString.c_str() );
	writeOptions = new osgDB::Options( writeOptionString.c_str() );

	// Get ReaderWriter
	rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");	
	
	// create ENet implementation object.
	enet_impl = new osgVisual::dataIO_clusterENet_implementation(receivedTransportContainer);

	// initialize ENet implementation
	if(clusterMode == MASTER)
	{
		std::cout << "Init dataIO_cluster_ENet as Server on port " << port << std::endl;
		enet_impl->init(dataIO_clusterENet_implementation::SERVER, port);

		initialized = true;
	}
	if(clusterMode == SLAVE)
	{
		// Init ENet
		enet_impl->init(dataIO_clusterENet_implementation::CLIENT, port);

		// Connect to server with 5 retries:
		bool connected = false;
		for(int i=0; i<5; i++)
		{
			std::cout << "Try to connect to server " << serverToConnect << std::endl;
			if( enet_impl->connectTo( serverToConnect.c_str(), 5000 ) )
			{
				// Connect successful.
				initialized = true;
				connected = true;
				break;
			}
		}	// For END
		if(!connected)
		{
			initialized = false;
			std::cout << "Finally failed to establish connection to server " << serverToConnect << std::endl;
			return false;
		}
	}	// IF SLAVE END

	return true;
}

bool dataIO_clusterENet::processXMLConfiguration(xmlNode* clusterConfig_)
{
				// Extract cluster role
				xmlAttr  *attr = clusterConfig_->properties;
				while ( attr ) 
				{ 
					std::string attr_name=reinterpret_cast<const char*>(attr->name);
					std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
					if( attr_name == "implementation" )
					{
						if(attr_value != "enet")
						{
							OSG_NOTIFY( osg::ALWAYS ) << "WARNING: Cluster configuration does not match the 'enet' implementation, falling back to clusterDummy" << std::endl;
							return false;
						}
					}
					if( attr_name == "hardsync" )
					{
						if(attr_value == "yes")
							hardSync = true;
						else
							hardSync = false;
					}
					if( attr_name == "master_ip" )
					{
						serverToConnect = attr_value;
					}
					if( attr_name == "port" )
					{
						std::istringstream i(attr_value);
						if (!(i >> port))
						{
							OSG_NOTIFY( osg::ALWAYS ) << "WARNING: Cluster configuration : Invalid port number '" << attr_value << "', falling back to clusterDummy" << std::endl;
							return false;
						}
					}
					if( attr_name == "use_zlib_compressor" )
					{
						if(attr_value == "yes")
							compressionEnabled = true;
						else
							compressionEnabled = false;
					}
					attr = attr->next; 
				}	// WHILE attrib END

	return true;
}


void dataIO_clusterENet::shutdown()
{
	OSG_NOTIFY( osg::ALWAYS ) << "clusterENet shutdown();" << std::endl;
}


bool dataIO_clusterENet::sendTO_OBJvaluesToSlaves(osg::Matrixd viewMatrix_) 
{
	//OSG_NOTIFY( osg::ALWAYS ) << "clusterENet sendTO_OBJvaluesToSlaves()" << std::endl;
	
	if(sendContainer.valid())
	{
		// Pack FrameID & Viewmatrix
		sendContainer->setFrameID(viewer->getFrameStamp()->getFrameNumber());
		sendContainer->setViewMatrix(viewMatrix_);

		// Writing node to stream
		std::stringstream myOstream;
		if ( rw )
		{
			osgDB::ReaderWriter::WriteResult wr = rw->writeObject( *sendContainer.get(), myOstream, writeOptions );
			if (wr.success() )			
			{
				// Send Data via ENet:
				//OSG_NOTIFY( osg::ALWAYS ) << "dataIO_clusterUDP::sendTO_OBJvaluesToSlaves() - Bytes to send: " << myOstream.str().length() << std::endl;
				//OSG_NOTIFY( osg::ALWAYS ) << "Send: " << myOstream.str() << std::endl;
				//OSG_NOTIFY( osg::ALWAYS ) << "Sent Framenumber: " << viewer->getFrameStamp()->getFrameNumber() << std::endl;
				ENetPacket * packet = enet_packet_create (myOstream.str().c_str(), 
												  myOstream.str().size(), 
												  ENET_PACKET_FLAG_RELIABLE);
		
				// Send data
				enet_impl->sendPacket( packet, 0, 0, true);
			}
			else OSG_NOTIFY( osg::WARN ) << "ERROR: dataIO_clusterENet::sendTO_OBJvaluesToSlaves() :: Save failed: " << wr.message() << std::endl;
		}
		else OSG_NOTIFY( osg::WARN ) << "ERROR: dataIO_clusterENet::sendTO_OBJvaluesToSlaves() :: Unable to get readerWriter for osgb" << std::endl;
	}
	else OSG_NOTIFY( osg::WARN ) << "ERROR: dataIO_clusterENet::sendTO_OBJvaluesToSlaves() :: Invalid transportContainer" << std::endl;


	enet_impl->processEvents();	// As Master: process events AFTER doing anything to have up to have the "sent" commands in queue.
	return true;
}


bool dataIO_clusterENet::readTO_OBJvaluesFromMaster()
{
	//OSG_NOTIFY( osg::ALWAYS ) << "clusterENet readTO_OBJvaluesFromMaster()" << std::endl;
	enet_impl->processEvents();	// As Slave: process events BEFORE doing anything to have up to date values.

	int bytes_received = receivedTransportContainer.size();
	if (bytes_received > 0 )
	{
		//OSG_NOTIFY( osg::ALWAYS ) << "dataIO_clusterENet::readTO_OBJvaluesFromMaster() - Bytes received: " << bytes_received << std::endl;
		//OSG_NOTIFY( osg::ALWAYS ) << "Received: " << std::endl << receivedTransportContainer << std::endl;
		

		// Unserialize data
		if ( rw )
		{
			std::stringstream tmp;
			tmp  << receivedTransportContainer;
			osgDB::ReaderWriter::ReadResult rr = rw->readObject( tmp, readOptions );
			if (rr.success())
			{
				sendContainer = dynamic_cast<osgVisual::dataIO_transportContainer*>(rr.takeObject());
				if (sendContainer)
				{
					OSG_NOTIFY( osg::ALWAYS ) << "Received:: Settings Viewmatrix...FrameID is: " << sendContainer->getFrameID() << std::endl;
					// Restore Viewmatrix 
					viewer->getCamera()->setViewMatrix(sendContainer->getViewMatrix());
				}
				else
					OSG_NOTIFY( osg::WARN ) << "ERROR: dataIO_clusterENet::readTO_OBJvaluesFromMaster() - Unable to cast converted node to transportContainer" << std::endl;
			}
			else
				OSG_NOTIFY( osg::WARN ) << "ERROR: dataIO_clusterENet::readTO_OBJvaluesFromMaster() - Unable to convert stream to node" << std::endl;
		}
		else
			OSG_NOTIFY( osg::WARN ) << "ERROR: dataIO_clusterENet::readTO_OBJvaluesFromMaster() - Unable to get readerWriter for osgb" << std::endl;
	}	// IF bytes recv > 0 END



	return true;
}


void dataIO_clusterENet::reportAsReadyToSwap()
{
	if(!hardSync)
		return;

	OSG_NOTIFY( osg::ALWAYS ) << "clusterENet reportAsReadyToSwap()" << std::endl;
}

bool dataIO_clusterENet::waitForSwap()
{
	if(!hardSync)
		return true;

	OSG_NOTIFY( osg::ALWAYS ) << "clusterENet waitForSwap()" << std::endl;

	return true;
}


bool dataIO_clusterENet::waitForAllReadyToSwap()
{
	if(!hardSync)
		return true;

	OSG_NOTIFY( osg::ALWAYS ) << "clusterENet waitForAllReadyToSwap()" << std::endl;

	return true;
}


bool dataIO_clusterENet::sendSwapCommand()
{
	if(!hardSync)
		return true;

	OSG_NOTIFY( osg::ALWAYS ) << "clusterENet sendSwapCommand()" << std::endl;

	return true;
}
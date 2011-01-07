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

#include <osg/Referenced>
#include <osgViewer/Viewer>

#include "dataIO_slot.h"

#include "dataIO_transportContainer.h"

// XML Parser
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


namespace osgVisual
{

/**
 * \brief This class is the interface definition class for valid cluster implementations.
 * 
 * This class is an abstract class, thus cannot be instantiated. Derive this class for usage.
 * 
 * @author Torben Dannhauer
 * @date  Feb 2010
 */ 
class dataIO_cluster :	public osg::Referenced
{
	#include <leakDetection.h>
public:
	/**
	 * This enum defines the three modi of the cluster engine.
	 */ 
	enum clustermode {MASTER, SLAVE, STANDALONE};

	/**
	 * \brief Empty constructor.
	 * 
	 */ 
	dataIO_cluster():hardSync(false) {}

	/**
	 * \brief Empty destructor
	 * 
	 * @return 
	 */ 
	virtual ~dataIO_cluster() {}

	/**
	 * \brief Pure virtual function for initialization. Must be implemented in derived class.
	 * 
	 */ 
 	virtual bool init( xmlNode* configurationNode, osgViewer::Viewer* viewer_, clustermode clusterMode_, osgVisual::dataIO_transportContainer* sendContainer_, bool asAscii_) = 0;

	/**
	 * \brief Pure virtual function for initialization. Must be implemented in derived class.
	 * 
	 */ 
	virtual bool processXMLConfiguration(xmlNode* clusterConfig_) = 0;
	
	/**
	 * \brief Acess function to retrieve whether hardSync is enabled.
	 * 
	 * @return : True if hardsync is enabled
	 */ 
	bool isHardSyncEnabled(){return hardSync;};

	/**
	 * \brief Pure virtual function for initialization. Must be implemented in derived class.
	 * 
	 */ 
	virtual void shutdown() = 0;

	/**
	 * \brief This function sets the current transportContainer the cluster should send.
	 * 
	 * The previous container is removed, if set. (smartpointer, will delete itself.). If the cluster is in slave mode, the send_container is ignored.
	 * 
	 * @param sendContainer_ : New container to set.
	 */ 
	void setSendTransportContainer( osgVisual::dataIO_transportContainer* sendContainer_ ) {sendContainer=sendContainer_;}

	/**
	 * \brief Pure virtual function for sending TO_OBJ values via cluster to slave. Must be implemented in derived class.
	 * 
	 * @param viewMatrix_ :  Viewmatrix to send to the Slave.
	 * @return : See derived class.
	 */ 
	virtual bool sendTO_OBJvaluesToSlaves(osg::Matrixd viewMatrix_) = 0;

	/**
	 * \brief Pure virtual function for reading TO_OBJ values from master. Must be implemented in derived class.
	 * 
	 * @return : See derived class.
	 */ 
	virtual bool readTO_OBJvaluesFromMaster() = 0;

	/**
	 * \brief Pure virtual function for reporting slave as ready to swap. Must be implemented in derived class.
	 * 
	 */ 
	virtual void reportAsReadyToSwap() = 0;

	/**
	 * \brief Pure virtual function for waiting as slave for swap command. Must be implemented in derived class.
	 * 
	 * @return : See derived class.
	 */ 
	virtual bool waitForSwap() = 0;

	/**
	 * \brief Pure virtual function for waiting as master for all slaves reported as ready to swap. Must be implemented in derived class.
	 * 
	 * @return : See derived class.
	 */ 
	virtual bool waitForAllReadyToSwap() = 0;

	/**
	 * \brief Pure virtual function for sending swap command to all slaves. Must be implemented in derived class.
	 * 
	 * @return : See derived class.
	 */ 
	virtual bool sendSwapCommand() = 0;



protected:
	/**
	 * This variable only mirrors the variable of dataIO. will be set during initialize.
	 */ 
	osgVisual::dataIO_cluster::clustermode clusterMode;

	/**
	 * Flag to indicate the cluster's initialization status
	 */ 
	bool initialized;

	/**
	 * Port to use for the cluster network traffic
	 */ 
	int port;

	/**
	 * Flag if hardSync is enabled
	 */ 
	bool hardSync;

	/**
	 * // Flag if OSGs compression alorithm should be used.
	 */ 
	bool compressionEnabled; 

	/**
	 * Referenced Pointer to the applications viewer.
	 */ 
	osg::ref_ptr<osgViewer::Viewer> viewer;

	/**
	 * Pointerto the send container of dataIO.
	 */ 
	osg::ref_ptr<osgVisual::dataIO_transportContainer> sendContainer;

	/**
	 * Temporary buffer to store the data into which was recieved via cluster.
	 */ 
	std::string receivedTransportContainer;
};

}	// END NAMESPACE
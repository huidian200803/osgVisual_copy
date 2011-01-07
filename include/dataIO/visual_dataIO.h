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
#include <osg/Referenced>
#include <osg/ArgumentParser>

#include <osgViewer/Viewer>

// osgVisual specifiy includes
#include <visual_util.h>

// Cluster
#include <dataIO_clusterDummy.h>
#ifdef USE_CLUSTER_ASIO_TCP_IOSTREAM
	#include <dataIO_clusterAsioTcpIostream.h>
#endif
#ifdef USE_CLUSTER_ENET
	#include <dataIO_clusterENet.h>
#endif
	



//ExtLink 
#include <dataIO_extLinkDummy.h>
#ifdef USE_EXTLINK_VCL
	#include <dataIO_extLinkVCL.h>
#endif


// Slot and transportContainer definitions
#include <dataIO_slot.h>
#include <dataIO_transportContainer.h>

// XML Parser
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

// C++ stl libraries
#include <vector>




namespace osgVisual {

/**
 * \brief Zentrale Klasse für Datenanbindung des Sichtsystemnodes an das Gesamtsichtsystem bzw. dem Simulator.
 * 
 * Damit nur eine Klasse instantiiert werden kann, ist diese Klasse als Singleton realisiert.
 * 
 * 
 * @author Torben Dannhauer
 * @date  Nov 2009
 */ 
class visual_dataIO : public osg::Referenced
{
	#include <leakDetection.h>
private:
	class dataIO_eventCallback : public osg::NodeCallback
	{
	public: 
		/**
		 * \brief Constructor, for setting the member variables.
		 * 
		 * @param viewer_ : Pointer to the viewer.
		 * @param sceneCamera_ : Pointer to the scene camera.
		 */ 
		dataIO_eventCallback(visual_dataIO* dataIO_):dataIO(dataIO_){};

		/**
		 * \brief This function is execute d as callback during update traversal.
		 * 
		 */ 
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	private:
		visual_dataIO* dataIO;
	};
	osg::ref_ptr<dataIO_eventCallback> eventCallback;

	class dataIO_finalDrawCallback : public osg::Camera::DrawCallback
	{
	public:
		/**
		 * \brief Constructor
		 * 
		 */ 
		dataIO_finalDrawCallback(visual_dataIO* dataIO_):dataIO(dataIO_){};
		
		/**
		 * \brief Operator executed at callback
		 * 
		 */ 
		virtual void operator () (const osg::Camera& camera) const;
	private:
		visual_dataIO* dataIO;
	};

	osg::ref_ptr<dataIO_finalDrawCallback> finalDrawCallback;

	/**
	 * \brief Constructor: It is private to prevent creating instances via ptr* = new ..().
	 * 
	 */ 
	visual_dataIO();

	/**
	 * \brief Copy-Constuctor: It is private to prevent getting instances via copying dataIO instance.
	 * 
	 * @param cc : Instance to copy from.
	 */ 
	visual_dataIO(const visual_dataIO& cc);

	/**
	 * \brief This function parses the XML config file for dataIO relevant parameters.
	 * 
	 * @return : True if parsing was successful.
	 */ 
	bool processXMLConfiguration(xmlNode* extLinkConfig_); 

	/**
	 * \brief Todo: required?
	 * 
	 * @return 
	 */ 
	osg::Matrixd calcViewMatrix();

	/**
	 * Pointer to the base class of the extLink implementation.
	 */ 
	osg::ref_ptr<dataIO_extLink> extLink;

	/**
	 * Pointer to the base class of the cluster implementation.
	 */ 
	osg::ref_ptr<dataIO_cluster> cluster;

	/**
	 * Referenced pointer to the applications viewer.
	 */ 
	osg::ref_ptr<osgViewer::Viewer> viewer;

	/**
	 * Referenced pointer transport contained user to transport the set of all slots to all rendering machines.
	 */ 
	osg::ref_ptr<osgVisual::dataIO_transportContainer> slotContainer;

	/**
	 * List of SLOT variables dataIO provides.
	 */ 
	std::vector<dataIO_slot*> dataSlots;

	/**
	 * Flag to indicate if dataIO is initialized.
	 */ 
	bool initialized;

	/**
	 * Curent clustermode of the application. Can be MASTER, SLAVE or STANDALONE.
	 */ 
	osgVisual::dataIO_cluster::clustermode clusterMode;

	/**
	 * XML config filename
	 */ 
	std::string configFileName;

	/**
	 * The eventCallback-class is friend to be able to work with all dataIO members without setters/getters.
	 */ 
	friend class dataIO_eventCallback;

	/**
	 *  The FinalDrawCallback-class is friend to be able to work with all dataIO members without setters/getters.
	 */ 
	friend class dataIO_finalDrawCallback;

public:
	/**
	 * \brief Public destructor to allow singleton cleanup from extern
	 * 
	 */ 
	~visual_dataIO();

	/**
	 * \brief This function returns an pointer to the singleton instance of dataIO. If no instance exist, it will be instantiated silently.
	 * 
	 * After instantiation, dataIO still needs to bei initiialized to configure working mode etc!
	 * 
	 * @return : Pointer to the instance. 
	 */ 
	static visual_dataIO* getInstance();

	void init(osgViewer::Viewer* viewer_,osg::ArgumentParser& arguments_, std::string configFileName);
	void shutdown();
	bool isMaster(){if (clusterMode==osgVisual::dataIO_cluster::MASTER) return true; else return false;};
	bool isSlave(){if (clusterMode==osgVisual::dataIO_cluster::SLAVE) return true; else return false;};
	bool isStandalone(){if (clusterMode==osgVisual::dataIO_cluster::STANDALONE) return true; else return false;};

// SLOT Access functions
	void* getSlotPointer(std::string slotName_, osgVisual::dataIO_slot::dataDirection direction_, osgVisual::dataIO_slot::varType variableTyp_ );
	double getSlotDataAsDouble(std::string variableName_, osgVisual::dataIO_slot::dataDirection direction_ );
	std::string getSlotDataAsString(std::string variableName_, osgVisual::dataIO_slot::dataDirection direction_ );
	osgVisual::dataIO_slot* setSlotData(std::string variableName_, osgVisual::dataIO_slot::dataDirection direction_, std::string sValue_ );
	osgVisual::dataIO_slot* setSlotData(std::string variableName_, osgVisual::dataIO_slot::dataDirection direction_, double value_ );

	int getSlotNum() {return dataSlots.size();}

};


} // END namespace osgVisual

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

#include "visual_dataIO.h"

using namespace osgVisual;

visual_dataIO::visual_dataIO()
{
	OSG_NOTIFY( osg::ALWAYS ) << "visual_dataIO constructed" << std::endl;

	initialized = false;
	clusterMode = osgVisual::dataIO_cluster::STANDALONE;
	// Create Transport-Container:
	slotContainer = new osgVisual::dataIO_transportContainer();
}

visual_dataIO::~visual_dataIO()
{
	// Delete all slots:
	for(unsigned int i=0;i<dataSlots.size();i++)
	{
		delete dataSlots[i];
	}
	dataSlots.clear();
	
	OSG_NOTIFY( osg::ALWAYS ) << "visual_dataIO destructed" << std::endl;
}

visual_dataIO* visual_dataIO::getInstance()
{
	static visual_dataIO instance; 
	return &instance; 
};

void visual_dataIO::init(osgViewer::Viewer* viewer_, std::string configFileName)
{
	OSG_NOTIFY( osg::ALWAYS ) << "visual_dataIO initialize..";

	// Init variables
	viewer = viewer_;

	// Process XML configuration
	this->configFileName = configFileName;
	xmlNode *extLinkConfig=NULL;
	if(!processXMLConfiguration(extLinkConfig))
		OSG_FATAL << "ERROR: visual_dataIO::init() - Failed to initialize dataIO via XML configuration!";

	// Create extLink.
	#ifdef USE_EXTLINK_DUMMY
		extLink = new dataIO_extLinkDummy( dataSlots );
	#endif
	#ifdef USE_EXTLINK_VCL
		extLink = new dataIO_extLinkVCL( dataSlots );
	#endif
	extLink->init();

	
	// Install callbacks to perform DataIO activities every frame:
	//// EventCallback at the absolute beginning of the frame
	eventCallback = new dataIO_eventCallback(this);
	viewer->getCamera()->setEventCallback( eventCallback );
	//// FinalDrawCallback at the end of event and update handling, but BEFORE rendering the frame
	finalDrawCallback = new dataIO_finalDrawCallback(this);
	viewer->getCamera()->setFinalDrawCallback( finalDrawCallback );

	initialized = true;
}

bool visual_dataIO::processXMLConfiguration(xmlNode* extLinkConfig_)
{
	// Init XML
	xmlDoc* tmpDoc;
	bool disabled;
	xmlNode* config = util::getModuleXMLConfig( configFileName, "dataio", tmpDoc, disabled );
	xmlNode* clusterConfig = NULL;

	if( disabled)
		OSG_NOTIFY( osg::ALWAYS ) << "..disabled by XML configuration file. dataIO can't be disabled. Ignoring." << std::endl;
	else
		OSG_NOTIFY( osg::ALWAYS ) << std::endl;

	// extract configuration values
	if(config)
	{
		xmlNode* a_node = config->children;

		for (xmlNode *cur_node = a_node; cur_node; cur_node = cur_node->next)
		{
			std::string node_name=reinterpret_cast<const char*>(cur_node->name);
			//OSG_ALWAYS << "----visual_distortion::processXMLConfiguration() - node type="<< cur_node->type <<", name=" << cur_node->name << std::endl;

			// Check for dataio node
			if(cur_node->type == XML_ELEMENT_NODE && node_name == "dataio")
			{
				// Extract cluster role
				xmlAttr  *attr = cur_node->properties;
				while ( attr ) 
				{ 
					std::string attr_name=reinterpret_cast<const char*>(attr->name);
					std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
					if( attr_name == "clusterrole" )
					{
						if(attr_value == "master")
						{
							OSG_NOTIFY( osg::ALWAYS ) << "Configure osgVisual as MASTER" << std::endl;
							clusterMode = osgVisual::dataIO_cluster::MASTER;
						}
						else if(attr_value == "slave")
						{
							OSG_NOTIFY( osg::ALWAYS ) << "Configure osgVisual as SLAVE" << std::endl;
							clusterMode = osgVisual::dataIO_cluster::SLAVE;
							slotContainer = NULL;	// Slave only recieves container, therefor set this Pointer NULL (created instance will be deleted because it is an auto pointer).
						}
						else if(attr_value == "standalone")
						{
							OSG_NOTIFY( osg::ALWAYS ) << "Configure osgVisual as STANDALONE" << std::endl;
							clusterMode = osgVisual::dataIO_cluster::STANDALONE;
						}
					}
					attr = attr->next; 
				}	// WHILE attrib END
			}

			// Check for cluster node
			if(cur_node->type == XML_ELEMENT_NODE && node_name == "cluster")
			{
				// Pass cluster configuration to the used cluster implementation. 
				// The implementation will use the configuration if it matches, otherwise falls back to dummy cluster
				clusterConfig = cur_node;
			}

			// Check for extLink node
			if(cur_node->type == XML_ELEMENT_NODE && node_name == "extlink")
			{
				// Check Attributes to determine if the dummy implementation or any other implementation must be instantiated

				// Pass XML attributes to extlink to analyse and configure it by extLink itself
			}

		}	// FOR all nodes END



		// Create Cluster.
		#ifdef USE_CLUSTER_ASIO_TCP_IOSTREAM
			cluster = new dataIO_clusterAsioTcpIostream();
		#endif 
		#ifdef USE_CLUSTER_ENET
			cluster = new dataIO_clusterENet();
		#endif
		if( !cluster.valid() || !clusterConfig || !cluster->init(clusterConfig, viewer, clusterMode, slotContainer, false) )
		{
			cluster = new dataIO_clusterDummy();
			cluster->init(clusterConfig, viewer, clusterMode, slotContainer, false);
		}



		// clean up
		xmlFreeDoc(tmpDoc); xmlCleanupParser();
		return true;
	}	// IF Config valid END
	else
	{
		OSG_WARN << "ERROR: visual_data::processXMLConfiguration() - Module configuration not found" << std::endl;
		return false;
	}
	return true;
}

void visual_dataIO::shutdown()
{
	if(initialized)
	{
		OSG_NOTIFY( osg::ALWAYS ) << "Shutdown visual_dataIO..." << std::endl;

		viewer->getCamera()->removeEventCallback( eventCallback );
		eventCallback = NULL;
		viewer->getCamera()->setFinalDrawCallback( NULL );
		finalDrawCallback = NULL;
		
		viewer = NULL;
		

		if(cluster.valid())
			cluster->shutdown();
		if(extLink.valid())
		extLink->shutdown();
	}
}

void visual_dataIO::dataIO_eventCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	// perform all actions for the eventDrawCallback.
	OSG_NOTIFY( osg::INFO ) << "---- Executing EventCallback.." <<  std::endl;

	switch( dataIO->clusterMode )
	{
		case osgVisual::dataIO_cluster::MASTER : 
			{
				dataIO->extLink->readTO_OBJvalues();
				dataIO->cluster->sendTO_OBJvaluesToSlaves(dataIO->calcViewMatrix());
			}
			break;
		case osgVisual::dataIO_cluster::SLAVE : 
			{
				dataIO->cluster->readTO_OBJvaluesFromMaster();
			}
			break;
		case osgVisual::dataIO_cluster::STANDALONE : 
			{
				dataIO->extLink->readTO_OBJvalues();
			}
			break;
		default:
			OSG_NOTIFY( osg::FATAL ) << "ERROR: Unkown clustermode!" <<  std::endl;
			break;
	};
	traverse(node, nv);
}

void visual_dataIO::dataIO_finalDrawCallback::operator() (const osg::Camera& camera) const
{
	// perform all actions for the initialDrawCallback.
	OSG_NOTIFY( osg::INFO ) << "---- Executing InitialDrawCallback.." << std::endl;

	switch( dataIO->clusterMode )
	{
		case osgVisual::dataIO_cluster::MASTER : 
			{
				dataIO->extLink->writebackFROM_OBJvalues();
				dataIO->cluster->waitForAllReadyToSwap();
				dataIO->cluster->sendSwapCommand();
			}
			break;
		case osgVisual::dataIO_cluster::SLAVE : 
			{
				dataIO->cluster->reportAsReadyToSwap();
				dataIO->cluster->waitForSwap();
			}
			break;
		case osgVisual::dataIO_cluster::STANDALONE : 
			{
				dataIO->extLink->writebackFROM_OBJvalues();
			}
			break;
		default:
			OSG_NOTIFY( osg::FATAL ) << "ERROR: visual_dataIO::dataIO_finalDrawCallback::operator() - Unkown clustermode!" <<  std::endl;
			break;
	};
}

void* visual_dataIO::getSlotPointer(std::string variableName_, osgVisual::dataIO_slot::dataDirection direction_, osgVisual::dataIO_slot::varType variableTyp_ )
{
	// iterate through slotlist. If found, return pointer, else add slot to list and return pointer
	for (unsigned int i=0; i<dataSlots.size(); i++)
	{
		// Check if this variable name&-type already exists
		if( dataSlots[i]->variableName == variableName_ && dataSlots[i]->direction == direction_  && dataSlots[i]->variableType ==  variableTyp_)
		{
			//OSG_NOTIFY( osg::INFO ) << "visual_dataIO::getSlotPointer() - Slot found at position " << i << std::endl;
			// Return pointer to the value
			return dataSlots[i];
		}
	}

	// Slot does not exist -> add it to slot list
	//OSG_NOTIFY( osg::INFO ) << "visual_dataIO::getSlotPointer() - Slot not found, will add as new slot " << std::endl;
	dataIO_slot* newSlot = new dataIO_slot();
	newSlot->variableName = variableName_;
	newSlot->variableType = variableTyp_;
	newSlot->value = 0;
	newSlot->sValue = "";
	dataSlots.push_back( newSlot );
	return dataSlots.back();
}

double visual_dataIO::getSlotDataAsDouble(std::string variableName_, osgVisual::dataIO_slot::dataDirection direction_ )
{
	// iterate through slotlist. If found, return value
	for (unsigned int i=0; i<dataSlots.size(); i++)
	{
		// Check if this variable name&-type already exists
		if( dataSlots[i]->variableName == variableName_ && dataSlots[i]->direction == direction_  && dataSlots[i]->variableType == osgVisual::dataIO_slot::DOUBLE )
		{
			//OSG_NOTIFY( osg::INFO ) << "visual_dataIO::getSlotDataAsDouble() - Slot found at position " << i << std::endl;
			return dataSlots[i]->value;
		}
	}
	return 0;
}

std::string visual_dataIO::getSlotDataAsString(std::string variableName_, osgVisual::dataIO_slot::dataDirection direction_ )
{
	// iterate through slotlist. If found, return value
	for (unsigned int i=0; i<dataSlots.size(); i++)
	{
		// Check if this variable name&-type already exists
		if( dataSlots[i]->variableName == variableName_ && dataSlots[i]->direction == direction_  && dataSlots[i]->variableType == osgVisual::dataIO_slot::STRING )
		{
			//OSG_NOTIFY( osg::INFO ) << "visual_dataIO::getSlotDataAsDouble() - Slot found at position " << i << std::endl;
			return dataSlots[i]->sValue;
		}
	}
	return "";
}

osgVisual::dataIO_slot* visual_dataIO::setSlotData(std::string variableName_, osgVisual::dataIO_slot::dataDirection direction_, std::string sValue_ )
{
	bool slotFound = false;
	// iterate through slotlist. If found, return pointer, else add slot to list
	for (unsigned int i=0; i<dataSlots.size(); i++)
	{
		// Check if this variable name&-type already exists
		if( dataSlots[i]->variableName == variableName_ && dataSlots[i]->direction == direction_ && dataSlots[i]->variableType ==  osgVisual::dataIO_slot::STRING)
		{
			// Update value
			dataSlots[i]->sValue = sValue_;
			slotFound = true;
			return dataSlots[i];
		}
		
	}

	if (!slotFound)
	{
		// Slot does not exist -> add it to slot list
		dataIO_slot* newSlot = new dataIO_slot();
		newSlot->variableName = variableName_;
		newSlot->direction = direction_;
		newSlot->variableType = osgVisual::dataIO_slot::STRING;
		newSlot->value = 0;
		newSlot->sValue = sValue_;
		dataSlots.push_back( newSlot );
		return dataSlots.back();
	}

	return NULL;
}

osgVisual::dataIO_slot* visual_dataIO::setSlotData(std::string variableName_, osgVisual::dataIO_slot::dataDirection direction_, double value_ )
{
	// iterate through slotlist. If found, return pointer, else add slot to list
	bool slotFound = false;
	for (unsigned int i=0; i<dataSlots.size(); i++)
	{
		// Check if this variableName & -type already exists
		if( dataSlots[i]->variableName == variableName_ && dataSlots[i]->direction == direction_ && dataSlots[i]->variableType ==  osgVisual::dataIO_slot::DOUBLE)
		{
			// Update value
			//OSG_NOTIFY( osg::ALWAYS ) << "setSlotData: " << variableName_ << " - value: " << value_ << std::endl;
			dataSlots[i]->value = value_;
			slotFound = true;
			return dataSlots[i];
		}	
	}

	if (!slotFound)
	{
		// Slot does not exist -> add it to slot list
		dataIO_slot* newSlot = new dataIO_slot();
		newSlot->variableName = variableName_;
		newSlot->direction = direction_;
		newSlot->variableType = osgVisual::dataIO_slot::DOUBLE;
		newSlot->value = value_;
		newSlot->sValue = "";
		dataSlots.push_back( newSlot );
		return dataSlots.back();
	}

	return NULL;
}

osg::Matrixd visual_dataIO::calcViewMatrix()
{
	return viewer->getCameraManipulator()->getInverseMatrix();
}
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

#include <dataIO_extLinkVCL.h>

#include <visual_dataIO.h>	// include in.cpp to avoid circular inclusion (visual_dataIO <-> extLinkVCL)

using namespace osgVisual;

dataIO_extLinkVCL::dataIO_extLinkVCL(std::vector<dataIO_slot *>& dataSlots_) : dataIO_extLink(dataSlots_)
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkVCL constructed" << std::endl;
}

dataIO_extLinkVCL::~dataIO_extLinkVCL(void)
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkVCL destroyed" << std::endl;
}

void dataIO_extLinkVCL::init()
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkVCL init()" << std::endl;

	VCLConfigFilename = "osgVisual.xml";
	
	if ( osgDB::fileExists( VCLConfigFilename ) )
	{
		CVCLIO::GetInstance().LoadProject(VCLConfigFilename.c_str());

		parseVCLConfig();
		
		initialized = true;
	}
	else
	{
		OSG_NOTIFY( osg::FATAL ) << "ERROR: Could not find VCL Configuration file " << VCLConfigFilename << std::endl;
		exit(-1);
	}

}

void dataIO_extLinkVCL::shutdown()
{
	OSG_NOTIFY( osg::ALWAYS ) << "extLinkVCL shutdown()" << std::endl;
}

bool dataIO_extLinkVCL::readTO_OBJvalues()
{
	OSG_NOTIFY( osg::INFO ) << "extLinkVCL readTO_OBJvalues()" << std::endl;

	// perform external data exchange
	CVCLIO::GetInstance().DoDataExchange();

	// read TO_OBJ values from VCL
	for(unsigned int i=0;i<extLinkChannels.size();i++)
	{
		if(extLinkSlots[i]->getdataDirection() == osgVisual::dataIO_slot::TO_OBJ)
		{
			//Copy data from VCL to slot. IMPORTANT: Due to VCL's string incapability only double slots are filled.
			osgVisual::visual_dataIO::getInstance()->setSlotData( extLinkSlots[i]->getVariableName(), osgVisual::dataIO_slot::TO_OBJ, extLinkChannels[i]->GetValue() );
		}	// IF (TO_OBJ) END
	}

	return true;
}

bool dataIO_extLinkVCL::writebackFROM_OBJvalues()
{
	OSG_NOTIFY( osg::INFO ) << "extLinkVCL writebackFROM_OBJvalues()" << std::endl;

	// write FROM_OBJ values into VCL
	for(unsigned int i=0;i<extLinkChannels.size();i++)
	{
		if(extLinkSlots[i]->getdataDirection() == osgVisual::dataIO_slot::FROM_OBJ && extLinkSlots[i]->getvarType() == osgVisual::dataIO_slot::DOUBLE)
		{
			//Copy data from slot to VCL. IMPORTANT: Due to VCL's string incapability only double slots are filled.
			extLinkChannels[i]->SetValue( osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble( extLinkSlots[i]->getVariableName(), osgVisual::dataIO_slot::FROM_OBJ ) );
		}	// IF (FROM_OBJ) END
	}

	/* In VCL no VCL dataexchange is performed, 
	it is postponed until the next frame beginning,
	where TO_OBJ exchange calls CVCLIO::GetInstance().DoDataExchange();.
	*/
	return true;
}

bool dataIO_extLinkVCL::parseVCLConfig()
{
	configFileValid = false;
	xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
  	
	doc = xmlReadFile(VCLConfigFilename.c_str(), NULL, 0);
	if (doc == NULL)
	{
		configFileValid = false;
		std::cout << "error: could not parse file" << VCLConfigFilename;
	}
	else
	{
		//  Get the root element node
		root_element = xmlDocGetRootElement(doc);

		// Parse the XML document.
		checkXMLNode(root_element);

		// free the document
		xmlFreeDoc(doc);;
	}
	// Free the global variables that may have been allocated by the parser.
	xmlCleanupParser();

	if(!configFileValid)
		OSG_ALWAYS << "ERROR: XML file seems not to be a valid VCL configuration file!" << std::endl;

	return true;
}

void dataIO_extLinkVCL::checkXMLNode(xmlNode * a_node)
{
    for (xmlNode *cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		std::string node_name=reinterpret_cast<const char*>(cur_node->name);
		if(cur_node->type == XML_ELEMENT_NODE && node_name == "CONFIGURATION")
		{
			//OSG_DEBUG << "XML node CONFIGURATION found" << std::endl;
			configFileValid = true;
		}

        if (cur_node->type == XML_ELEMENT_NODE && node_name == "CHANNEL")
		{
			//OSG_DEBUG << "XML node CHANNEL found" << std::endl;

			// Extract channel infos like name, direction and extract channels
			std::string name;
			dataIO_slot::dataDirection direction;
			xmlAttr  *attr = cur_node->properties;
			while ( attr ) 
			{ 
				std::string attr_name=reinterpret_cast<const char*>(attr->name);
				if( attr_name == "name" )
					name = reinterpret_cast<const char*>(attr->children->content);
				if( attr_name == "multicast_in_group" )
					direction = dataIO_slot::TO_OBJ;
				if( attr_name == "multicast_out_group" )
					direction = dataIO_slot::FROM_OBJ;
				//std::cout << "Attribute name: " << attr->name << " value: " << attr->children->content << std::endl;
				attr = attr->next; 
			} 
			addChannels(cur_node->children, name, direction );
	
            //OSG_DEBUG << "node type=Element, name:" << cur_node->name << std::endl;
			//OSG_DEBUG << "Processing children at " << cur_node->children << std::endl;
        }	// IF(CHANNEL) END

		// Iterate to the next nodes to find channels.
        checkXMLNode(cur_node->children);		
    }	// FOR END
}

void dataIO_extLinkVCL::addChannels(xmlNode * a_node, std::string channelName_, dataIO_slot::dataDirection direction_ )
{
	if(!a_node)
		return;

	OSG_ALWAYS << "dataIO_extL) : Processing entries for channel " << channelName_ << " with the direction " << direction_ << std::endl;

	for (xmlNode *cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		if (cur_node->type != XML_ELEMENT_NODE)	// only emelent nodes are relevant entries. otherwise skip this iteration. VCL files only contain text nodes in comments or as untrimmed element nodes.
			continue;		

		// Extract ENTRY - name (from the Channel <ENTRY> in the XML file
		std::string entryName;
		xmlAttr  *attr = cur_node->properties;
		while ( attr ) 
		{ 
			std::string attr_name=reinterpret_cast<const char*>(attr->name);
			if( attr_name == "name" )
				entryName = reinterpret_cast<const char*>(attr->children->content);
			//OSG_DEBUG << "Attribute name: " << attr->name << " value: " << attr->children->content << std::endl;
			attr = attr->next; 
		} 

		// Store VCL variable
		CVCLVariable<double>* tmp = new CVCLVariable<double>;
		extLinkChannels.push_back(tmp);

		// Attach VCL variable to channel:
		//OSG_DEBUG << "attaching.... name: " << channelName_ << ", entryName: " << entryName << std::endl;
		if( !tmp->Attach(channelName_.c_str(), entryName.c_str() ) )
			OSG_ALWAYS << "ERROR - dataIO_extLinkVCL::addChannels(): unable to attach VCL variable entryName: " << entryName << " to channel: " << channelName_ << std::endl;

		// Set SLOT data and store SLOT pointer 
		osgVisual::dataIO_slot* tmpSlot = osgVisual::visual_dataIO::getInstance()->setSlotData( entryName, direction_, 0 );
		extLinkSlots.push_back( tmpSlot );
	}	// FOR each ENTRY END
}
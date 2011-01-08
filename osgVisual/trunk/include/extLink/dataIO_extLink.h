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
#include <osg/Node>
#include <dataIO_slot.h>

// XML Parser
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


namespace osgVisual
{ 

/**
 * \brief This class is the interface definition class for valid externalLink (extLink) implementations.
 * 
 * This class is an abstract class, thus cannot be instantiated. Derive this class for usage.
 * 
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 

class dataIO_extLink : public osg::Referenced
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Empty constructor
	 * 
	 */ 
	dataIO_extLink(std::vector<osgVisual::dataIO_slot *>& dataSlots_) : dataSlots(dataSlots_){}

	/**
	 * \brief Empty destructor
	 * 
	 * @return 
	 */ 
	virtual ~dataIO_extLink() {}

	/**
	 * \brief Pure virtual function for initialization. Must be implemented in derived class.
	 * 
	 */ 
	virtual bool init( xmlNode* configurationNode) = 0;

	/**
	 * \brief Pure virtual function for XML configuration. Must be implemented in derived class.
	 * 
	 */ 
	virtual bool processXMLConfiguration(xmlNode* extLinkConfig_) = 0;

	/**
	 * \brief Pure virtual function for shutdown. Must be implemented in derived class.
	 * 
	 */ 
	virtual void shutdown() = 0;

	/**
	 * \brief Pure virtual function for reading TO_OBJ values form the external link. Must be implemented in derived class.
	 * 
	 * @return : See derived class.
	 */ 
	virtual bool readTO_OBJvalues() = 0;

	/**
	 * \brief Pure virtual function for writing return values back to the external link. Must be implemented in derived class.
	 * 
	 * @return : See derived class.
	 */ 
	virtual bool writebackFROM_OBJvalues() = 0;

protected:
	/**
	 * Nested external link for more then one external source.
	 */ 
	osg::ref_ptr<dataIO_extLink> extLink;

	/**
	 * Flag to indicate if this class is initialized.
	 */ 
	bool initialized;

	/**
	 * Reference to dataIO's central managed dataSlots.
	 * This central dataSlot array is filled with available slots by this extLink class.
	 */ 
	std::vector<dataIO_slot *>& dataSlots;
 
};

}	// END NAMESPACE
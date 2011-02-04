#pragma once
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

#include <dataIO_extLink.h>	// Base class
#include <osg/Notify>
#include <osgDB/FileUtils>

// VCL
#include <winsock2.h>

// XML Parser
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


// VCL
//// To keep the VS 2005 Solution file clean from VCL dependencies: Link dependencies here:
#ifndef _DEBUG
#pragma comment ( lib, "../../VCL_1.0.3.4/source/3rdParty/expatpp/lib/releasemtdll/libexpatMT.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/3rdParty/expatpp/lib/releasemtdll/expatpp.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/3rdParty/expatpp/lib.x64/releasemtdll/libexpatMT.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/3rdParty/expatpp/lib.x64/releasemtdll/expatpp.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/lib/x86/VCL1_34.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/lib/x64/VCL1_34.lib" )
#endif
#ifdef _DEBUG
#pragma comment ( lib, "../../VCL_1.0.3.4/source/3rdParty/expatpp/lib/debugmtdll/libexpatMT_d.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/3rdParty/expatpp/lib/debugmtdll/expatpp_d.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/3rdParty/expatpp/lib.x64/debugmtdll/libexpatMT_d.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/3rdParty/expatpp/lib.x64/debugmtdll/expatpp_d.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/lib/x86/VCL1_34D.lib" )
#pragma comment ( lib, "../../VCL_1.0.3.4/source/lib/x64/VCL1_34D.lib" )
#endif
//// include VCL header
#ifdef WIN32
#include <wtypes.h>
#endif
#include "VCLDefinitions.h"
#include "VCLIO.h"
#include "VCLIOChannel.h"
#include "VCLIOChannelEntry.h"
#include "VCLVariable.h"
#include "VCLSocketDefs.h"
#include <conio.h>
#include "stdlib.h"
#include <iostream>


namespace osgVisual
{ 

/**
 * \brief This class is a VCL-based implementation of the externalLink.

 * 
 * @author Torben Dannhauer
 * @date  Nov 2009
 */ 
class dataIO_extLinkVCL :	public dataIO_extLink
{
#include <leakDetection.h>
public:
	dataIO_extLinkVCL(std::vector<dataIO_slot *>& dataSlots_);
	virtual ~dataIO_extLinkVCL(void);

	bool init(xmlNode* configurationNode);
	bool processXMLConfiguration(xmlNode* extLinkConfig_);
	void shutdown();

	bool readTO_OBJvalues();
	bool writebackFROM_OBJvalues();


private:
	bool parseVCLConfig();
	void checkXMLNode(xmlNode * a_node);
	void addChannels(xmlNode * a_node, std::string channelName_, dataIO_slot::dataDirection direction_ );

	std::string VCLConfigFilename;
	std::vector< CVCLVariable<double>* > extLinkChannels;
	std::vector< osgVisual::dataIO_slot* > extLinkSlots;
	bool configFileValid;

};

}	// END NAMESPACE
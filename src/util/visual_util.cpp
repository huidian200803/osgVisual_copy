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

#include <visual_util.h>
#include <osg/Material>

using namespace osgVisual;

util::util(void)
{
}

util::~util(void)
{
}

xmlNode* util::getSceneryXMLConfig(std::string configFilename, xmlDoc*& doc)
{
	doc = NULL;
	xmlNode *root_element = NULL;

	// Check for valid parameters
	if(configFilename == "")
	{
		OSG_ALWAYS << "ERROR - util::getModuleXMLConfig() : Invalid Configuration Filename!" << std::endl;
		return NULL;
	}

	// It is a valid XML document?
	doc = xmlReadFile(configFilename.c_str(), NULL, 0);	
	if (doc == NULL)
	{
		OSG_ALWAYS << "ERROR - util::getModuleXMLConfig() : " << configFilename << " is not a valid XML file!" << std::endl;
		return NULL;
	}

	//  Get the root element node
	root_element = xmlDocGetRootElement(doc);

	// If file is a valid osgVisual config file, check all the root xml node and all of it's children of osgvisualconfiguration for the specified module
	xmlNode* tmpNode = checkXMLNodeChildrenForScenery(root_element);

	if( !tmpNode ) // if no valid node was found: clena up. Otherwise: the caller has to clean up.
	{
		xmlFreeDoc(doc);	// free the document
		xmlCleanupParser();	// Free the global variables that may have been allocated by the parser.
		return NULL;
	}
	else
		return tmpNode;
}

xmlNode* util::checkXMLNodeChildrenForScenery(xmlNode* node)
{
	for (xmlNode *cur_node = node; cur_node; cur_node = cur_node->next)	// iterate through all elements 
	{
		// Is the node the one we are searching for?
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			std::string node_name=reinterpret_cast<const char*>(cur_node->name);
			if (node_name == "scenery")
			{
				OSG_DEBUG << "XML node scenery found" << std::endl;
				return cur_node;
			}
			else	// Otherwise: check its children..
			{
				xmlNode* tmp_XmlNode = checkXMLNodeChildrenForScenery(cur_node->children);
				if(tmp_XmlNode)
					return tmp_XmlNode;
			}
		}	// IF NODE TYPE = ELEMENT END

		// Proceed with next node in this loop.
	}
	return NULL;
}

xmlNode* util::getModuleXMLConfig(std::string configFilename, std::string moduleName, xmlDoc*& doc, bool& disabled)
{
	doc = NULL;
	disabled = false;
	xmlNode *root_element = NULL;

	// Check for valid parameters
	if(configFilename == "")
	{
		OSG_ALWAYS << "ERROR - util::getModuleXMLConfig() : Invalid Configuration Filename!" << std::endl;
		return NULL;
	}
	if(moduleName == "")
	{
		OSG_ALWAYS << "ERROR - util::getModuleXMLConfig() : Invalid Module Filename!" << std::endl;
		return NULL;
	}

	// It is a valid XML document?
	doc = xmlReadFile(configFilename.c_str(), NULL, 0);	
	if (doc == NULL)
	{
		OSG_ALWAYS << "ERROR - util::getModuleXMLConfig() : " << configFilename << " is not a valid XML file!" << std::endl;
		return NULL;
	}

	//  Get the root element node
	root_element = xmlDocGetRootElement(doc);

	// Check if it is an osgVisual configuration file
	std::string node_name=reinterpret_cast<const char*>(root_element->name);
	if(!(root_element->type == XML_ELEMENT_NODE && node_name == "osgvisualconfiguration"))
	{
		OSG_ALWAYS << "ERROR - util::getModuleXMLConfig() : " << configFilename << " is not an osgVisual configuration file!" << std::endl;
		return NULL;
	}

	// If file is a valid osgVisual config file, check all the root xml node and all of it's children of osgvisualconfiguration for the specified module
	xmlNode* tmpNode = checkXMLNodeChildrenForModule(root_element, moduleName, disabled);

	if( !tmpNode ) // if no valid node was found or the module is disabled: clean up. Otherwise: the caller has to clean up.
	{
		xmlFreeDoc(doc);	// free the document
		xmlCleanupParser();	// Free the global variables that may have been allocated by the parser.
		return NULL;
	}
	else
		return tmpNode;

}

xmlNode* util::checkXMLNodeChildrenForModule(xmlNode* node, std::string moduleName, bool& disabled)
{
	for (xmlNode *cur_node = node; cur_node; cur_node = cur_node->next)	// iterate through all XML elements 
	{
		// Is the node the one we are searching for?
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			// Extract Node Name and the first attribute: "module name"
			std::string node_name = reinterpret_cast<const char*>(cur_node->name);
			std::string modName = "";
			xmlAttr  *attr = cur_node->properties;
			while ( attr ) 
			{ 
				std::string attr_name=reinterpret_cast<const char*>(attr->name);
				std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
				if( attr_name == "name" )
					modName = attr_value;
				attr = attr->next;
			}
			
			// Check each node for the searched module
			if (node_name == "module" && modName == moduleName)
			{
				// Check if the module is active
				xmlAttr  *attr = cur_node->properties;
				while ( attr ) 
				{ 
					std::string attr_name=reinterpret_cast<const char*>(attr->name);
					std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
					if( attr_name == "enabled" && attr_value == "yes" )
					{
						OSG_DEBUG << "Found XML module configuration for " << moduleName << std::endl;
						return cur_node;
					}
					if( attr_name == "enabled" && attr_value == "no" )
					{
						disabled = true;
						OSG_DEBUG << "Found XML module configuration for " << moduleName << ", but it is DISABLED." << std::endl;
						return NULL;
					}
					attr = attr->next;
				}
			}
			else	// Otherwise: check its children..
			{
				xmlNode* tmp_XmlNode = checkXMLNodeChildrenForModule(cur_node->children, moduleName, disabled);
				if(tmp_XmlNode)
					return tmp_XmlNode;
			}
		}	// IF NODE TYPE = ELEMENT END

		// Proceed with next node in this loop.
	}
	return NULL;
}

osg::Node* util::findNamedNode(const std::string& searchName_, osg::Node* currNode_)
{
   osg::Group* currGroup;
   osg::Node* foundNode;

   // check to see if we have a valid (non-NULL) node.
   // if we do have a null node, return NULL.
   if ( !currNode_)
   {
      return NULL;
   }

   // We have a valid node, check to see if this is the node we 
   // are looking for. If so, return the current node.
   if (currNode_->getName() == searchName_)
   {
      return currNode_;
   }

   // We have a valid node, but not the one we are looking for.
   // Check to see if it has children (non-leaf node). If the node
   // has children, check each of the child nodes by recursive call.
   // If one of the recursive calls returns a non-null value we have
   // found the correct node, so return this node.
   // If we check all of the children and have not found the node,
   // return NULL
   currGroup = currNode_->asGroup(); // returns NULL if not a group.
   if ( currGroup ) 
   {
      for (unsigned int i = 0 ; i < currGroup->getNumChildren(); i ++)
      { 
         foundNode = findNamedNode(searchName_, currGroup->getChild(i));
         if (foundNode)
		 {
			 std::cout << "Node gefunden in Ebene: " << i << std::endl;
            return foundNode; // found a match!
		}
      }
      return NULL; // We have checked each child node - no match found.
   }
   else 
	   return NULL; // leaf node, no match 
}

osg::ref_ptr<osg::Geode> util::getDemoCylinder(double length_, double width_, osg::Vec3 offset_ )
{
	osg::ref_ptr<osg::Geode> cyl = new osg::Geode();
	osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable(new osg::Cylinder( offset_, width_, length_ ));
	osg::Vec4 color = osg::Vec4(255.0, 0.0, 0.0, 1.0);
	shape->setColor( color );
	cyl->addDrawable( shape );

	return cyl;
}

osg::ref_ptr<osg::Geode> util::getDemoSphere(double radius_, osg::Vec3 offset_ )
{
	osg::ref_ptr<osg::Geode> sphere = new osg::Geode();
	osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable(new osg::Sphere( offset_, radius_ ) );
	osg::Vec4 color = osg::Vec4(255.0, 0.0, 0.0, 1.0);
	shape->setColor( color );
	sphere->addDrawable( shape );

	return sphere;
}

bool util::intersect(const osg::Vec3d& start_, const osg::Vec3d& end_, osg::Vec3d& intersection_, osg::Node* node_, osg::Node::NodeMask intersectTraversalMask_ )
{
	osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(start_,end_);

	osgUtil::IntersectionVisitor iv(lsi.get());
	iv.setTraversalMask(intersectTraversalMask_);
    
	node_->accept(iv);
    
	if (lsi->containsIntersections())
	{
		intersection_ = lsi->getIntersections().begin()->getWorldIntersectPoint();
		return true;	// Intersect found
	}
	return false;	// No intersect found
}

bool util::queryHeightOfTerrain(double& hot_, osg::Node* rootNode_, double lat_, double lon_, osg::Node::NodeMask traversalMask_)
{
	// Get ellipsoid model
	osg::CoordinateSystemNode* csn = dynamic_cast<osg::CoordinateSystemNode*>(rootNode_);
	if ( !csn )
	{
		OSG_NOTIFY( osg::FATAL ) << "util::queryHeightOfTerrain() :: Invalid CSN!" << std::endl;
		return false;
	}
	osg::EllipsoidModel* ellipsoid = csn->getEllipsoidModel();
	if ( !ellipsoid )
	{
		OSG_NOTIFY( osg::FATAL ) << "util::queryHeightOfTerrain() :: Invalid ellipsoid!" << std::endl;
		return false;
	}

	// Setup both endpoints of intersect line
	double X,Y,Z;
	ellipsoid->convertLatLongHeightToXYZ(lat_, lon_, 30000, X, Y, Z);
	osg::Vec3d s = osg::Vec3d(X, Y, Z);
	ellipsoid->convertLatLongHeightToXYZ(lat_, lon_, -30000, X, Y, Z);
	osg::Vec3d e = osg::Vec3d(X, Y, Z);

	// Query intersection point
	osg::Vec3d ip;
	if ( util::intersect(s, e, ip, rootNode_, traversalMask_) )
	{
		double lat2_, lon2_;
		ellipsoid->convertXYZToLatLongHeight( ip.x(), ip.y(), ip.z(), lat2_, lon2_, hot_ );	// Convert Intersection Point back to Lat Lon, HOT.
		//OSG_NOTIFY(osg::ALWAYS) << "lat: "<< osg::RadiansToDegrees(lat2_) <<", Lon: " << osg::RadiansToDegrees(lon2_) << ", Hot: " << hot_ << std::endl;
		return true;
	}

	// If no intersection point found: set HOT to zero and return false.
	hot_ = 0;
	return false;
}

bool util::queryHeightAboveTerrainInWGS84(double& hat_, osg::Node* rootNode_, double lat_, double lon_, double height_, osg::Node::NodeMask traversalMask_)
{
	// Get HOT by asking util::queryHeightOfTerrain() :)
	double HOT;
	if ( !util::queryHeightOfTerrain(HOT, rootNode_, lat_, lon_, traversalMask_) )
	{
		OSG_NOTIFY( osg::INFO ) << "util::queryHeightAboveTerrainInWGS84() :: Unable to get HOT, will use 0 for HOT!" << std::endl;
	}

	// Calculate HAT
	hat_ = height_ - HOT;
	return true;
}

bool util::queryHeightAboveTerrainInWorld(double& hat_, osg::Node* rootNode_, double x_, double y_, double z_, osg::Node::NodeMask traversalMask_)
{
	// Get ellipsoid model
	osg::CoordinateSystemNode* csn = dynamic_cast<osg::CoordinateSystemNode*>(rootNode_);
	if ( !csn )
	{
		OSG_NOTIFY( osg::FATAL ) << "util::queryHeightAboveTerrainInWorld() :: Invalid CSN!" << std::endl;
		return false;
	}
	osg::EllipsoidModel* ellipsoid = csn->getEllipsoidModel();
	if ( !ellipsoid )
	{
		OSG_NOTIFY( osg::FATAL ) << "util::queryHeightAboveTerrainInWorld() :: Invalid ellipsoid!" << std::endl;
		return false;
	}

	// Transform XYZ into LatLonHeight
	double lat_, lon_, height_;
	ellipsoid->convertXYZToLatLongHeight(x_, y_, z_, lat_, lon_, height_);

	// ask util::queryHeightAboveTerrainInWGS84() to calc HAT :)
	if( !util::queryHeightAboveTerrainInWGS84(hat_, rootNode_, lat_, lon_, height_, traversalMask_ ) )
	{
		OSG_NOTIFY( osg::FATAL ) << "util::queryHeightAboveTerrainInWorld() :: Unable to get HAT!" << std::endl;
		return false;
	}

	return true;
}

bool util::calculateEarthRadiusAtWGS84Coordinate(double lat_, double lon_, osg::Node* rootNode_, double& radius_)
{
	// Calculate radius:
	double x, y, z;
	
	if ( util::calculateXYZAtWGS84Coordinate(lat_, lon_, 0.0, rootNode_, x, y, z) )
	{
		radius_ = sqrt( pow(x, 2) + pow(y, 2) + pow(z, 2) );
		return true;
	}
	else
	{
		OSG_NOTIFY( osg::FATAL ) << "util::calculateEarthRadiusAtWGS84Coordinate() :: Unable to calculate Earth Radius!" << std::endl;
		return false;
	}
}

bool util::calculateXYZAtWGS84Coordinate(double lat_, double lon_, double height_, osg::Node* rootNode_, double& x_, double& y_, double& z_)
{
	// Get ellipsoid model
	osg::CoordinateSystemNode* csn = dynamic_cast<osg::CoordinateSystemNode*>(rootNode_);
	if ( !csn )
		return false;
	osg::EllipsoidModel* ellipsoid = csn->getEllipsoidModel();
	if ( !ellipsoid )
		return false;

	// Calculate xyz:
	ellipsoid->convertLatLongHeightToXYZ( lat_, lon_, height_, x_, y_, z_);
	return true;
}

bool util::getWGS84ofCamera( osg::Camera* camera_, osg::Node* rootNode_, double& lat_, double& lon_, double& height_ )
{
	// Get ellipsoid model
	osg::CoordinateSystemNode* csn = dynamic_cast<osg::CoordinateSystemNode*>(rootNode_);
	if ( !csn )
		return false;
	osg::EllipsoidModel* ellipsoid = csn->getEllipsoidModel();
	if ( !ellipsoid )
		return false;

	osg::Vec3d eye, dir, up;
	camera_->getViewMatrixAsLookAt(eye,dir,up); // Get XYZ from camera
	ellipsoid->convertXYZToLatLongHeight(eye.x(), eye.y(), eye.z(), lat_, lon_, height_);
	return true;
}

void util::getXYZofCamera( osg::Camera* camera_, double& x_, double& y_, double& z_ )
{
	osg::Vec3d eye, dir, up;
	camera_->getViewMatrixAsLookAt(eye,dir,up); // Get XYZ from camera
	x_ = eye.x();
	y_ = eye.y();
	z_ = eye.z();
}

bool util::removeClosebuttonOnGLWindow(osgViewer::Viewer* viewer_)
{
#ifdef FUNFUNCTIONS_ENABLED
#ifdef WIN32
	osgViewer::ViewerBase::Windows wins;
	viewer_->getWindows(wins);
	osgViewer::GraphicsHandleWin32* hwnd = dynamic_cast<osgViewer::GraphicsHandleWin32*>(wins[0]);
	
	HMENU hMenu = GetSystemMenu(hwnd->getHWND(), FALSE);
	::EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | (MF_DISABLED | MF_GRAYED)); 
#endif
#endif
	return true;
}

bool util::setTransparentWindowBackground(osgViewer::Viewer* viewer_)
{
#ifdef FUNFUNCTIONS_ENABLED
#ifdef WIN32
	osgViewer::ViewerBase::Windows wins;
	viewer_->getWindows(wins);
	osgViewer::GraphicsHandleWin32* hwnd = dynamic_cast<osgViewer::GraphicsHandleWin32*>(wins[0]);
	HWND _hwnd = hwnd->getHWND();
	viewer_->getDisplaySettings()->setMinimumNumAlphaBits(8);

   // Create and populate the Blur Behind structure
   DWM_BLURBEHIND bb = {0};
   // Disable Blur Behind and Blur Region;
   bb.dwFlags = DWM_BB_ENABLE;
   bb.fEnable = true;
   bb.hRgnBlur = NULL;

   // Ensable Blur Behind
   HRESULT hr = DwmEnableBlurBehindWindow(_hwnd, &bb);
   if (SUCCEEDED(hr))
      return true;
   else
	   return false;

#endif
#endif
	return true;
}

std::vector<std::string> util::getTerrainFromXMLConfig(std::string configFilename)
{
	xmlDoc* tmpDoc;
	xmlNode* sceneryNode = util::getSceneryXMLConfig(configFilename, tmpDoc);
	std::vector<std::string> filenames;

	// Iterate through nodes and search for terrian entry
	for (xmlNode *cur_node = sceneryNode->children; cur_node; cur_node = cur_node->next)
	{
		std::string node_name=reinterpret_cast<const char*>(cur_node->name);

		if(cur_node->type == XML_ELEMENT_NODE && node_name == "terrain")
		{
			xmlAttr  *attr = cur_node->properties;
			while ( attr ) 
			{ 
				std::string attr_name=reinterpret_cast<const char*>(attr->name);
				std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
				if( attr_name.find("filename") != std::string::npos )
				{
					filenames.push_back(attr_value);
				}
				attr = attr->next; 
			}
		}
	}// FOR all nodes END

	// Cleanup
	if(sceneryNode)
	{
		xmlFreeDoc(tmpDoc); xmlCleanupParser();
	}

	return filenames;
}

std::string util::getAnimationPathFromXMLConfig(std::string configFilename)
{
	xmlDoc* tmpDoc;
	xmlNode* sceneryNode = util::getSceneryXMLConfig(configFilename, tmpDoc);
	std::string animationpath = "";

	// Iterate through nodes and search for terrian entry
	for (xmlNode *cur_node = sceneryNode->children; cur_node; cur_node = cur_node->next)
	{
		std::string node_name=reinterpret_cast<const char*>(cur_node->name);

		if(cur_node->type == XML_ELEMENT_NODE && node_name == "animationpath")
		{
			xmlAttr  *attr = cur_node->properties;
			while ( attr ) 
			{ 
				std::string attr_name=reinterpret_cast<const char*>(attr->name);
				std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
				if( attr_name == "filename" )
				{
					animationpath = attr_value;
				}
				attr = attr->next; 
			}
		}
	}// FOR all nodes END

	// Cleanup
	if(sceneryNode)
	{
		xmlFreeDoc(tmpDoc); xmlCleanupParser();
	}

	return animationpath;
}

double util::strToDouble(std::string s)
{
	double tmp;
	std::stringstream sstr(s);
    if (!(sstr >> tmp))
	{
		OSG_ALWAYS << __FUNCTION__ << "Warning:Unable to convert "<< s <<" to double, using 0.0 as default!" << std::endl;
		return 0.0;
	}
	else
		return tmp;
}

int util::strToInt(std::string s)
{
	int tmp;
	std::stringstream sstr(s);
	if (!(sstr >> tmp))
	{
		OSG_ALWAYS << __FUNCTION__ << "Warning:Unable to convert "<< s <<" to int, using 0 as default!" << std::endl;
		return 0;
	}
	else
		return tmp;
}

bool util::strToBool(std::string s)
{
	if(s=="yes")
		return(true);
	if(s=="no")
		return(false);
	OSG_ALWAYS << __FUNCTION__ << "Warning:Unable to convert "<< s <<" to bool, using false as default!" << std::endl;
	return(false);
}

void util::AddCylinderBetweenPoints(osg::Vec3d StartPoint, osg::Vec3d EndPoint, float radius, float length, osg::Vec4d CylinderColor, osg::Group *pAddToThisGroup)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::Vec3d center;
	float height;
	osg::ref_ptr<osg::Cylinder> cylinder;
	osg::ref_ptr<osg::Drawable> cylinderDrawable;
	osg::ref_ptr<osg::Material> pMaterial;

	//height = (StartPoint-EndPoint).length();
	height  = length;
	center = osg::Vec3( (StartPoint.x() + EndPoint.x()) / 2, (StartPoint.y() + EndPoint.y()) / 2, (StartPoint.z() + EndPoint.z()) / 2);

	// This is the default direction for the cylinders to face in OpenGL
	osg::Vec3d z = osg::Vec3d(0,0,1);

	// Get diff between two points you want cylinder along
	osg::Vec3d p = StartPoint - EndPoint;

	// Get CROSS product (the axis of rotation)
	osg::Vec3d t = z ^ p;

	// Get angle. length is magnitude of the vector
	double angle = acos( (z * p) / p.length());

	// Create a cylinder between the two points with the given radius
	cylinder = new osg::Cylinder(center,radius,height);
	cylinder->setRotation(osg::Quat(angle, osg::Vec3(t.x(), t.y(), t.z())));

	cylinderDrawable = new osg::ShapeDrawable(cylinder );
	geode->addDrawable(cylinderDrawable);

	// Set the color of the cylinder that extends between the two points.
	pMaterial = new osg::Material;
	pMaterial->setDiffuse( osg::Material::FRONT, CylinderColor);
	geode->getOrCreateStateSet()->setAttribute( pMaterial, osg::StateAttribute::OVERRIDE );

	// Add the cylinder between the two points to an existing group
	pAddToThisGroup->addChild(geode);
}
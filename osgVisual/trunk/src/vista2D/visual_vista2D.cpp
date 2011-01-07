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

#include "visual_vista2D.h"


using namespace osgVisual;

visual_vista2D::visual_vista2D(void)
{
	OSG_NOTIFY (osg::ALWAYS ) << "visual_vista2D instantiated." << std::endl;

   // Create a Vista2D View
   view = new Vista2D::VistaView();

   	vistaProjectfile = "";
	paintBackground = false;
	playanimation = false;
	position_x = 0;
	position_y = 0;
	zoom = 1;
}

visual_vista2D::~visual_vista2D(void)
{
	view->stopView();
}

void visual_vista2D::startVista2D()
{
    /***************
    load view
    ***************/
	view->load( vistaProjectfile );
    view->setBackgroundMode(paintBackground);	// don't paint background
	view->setPosition( position_x, position_y);
	view->setZoom( zoom );

    /***************
    start animation with
    self running datasource
    ***************/
    view->startView (playanimation);
}

bool visual_vista2D::processXMLConfiguration()
{
	// Init XML
	xmlDoc* tmpDoc;
	bool disabled;
	xmlNode* config = util::getModuleXMLConfig( configFileName, "vista2d", tmpDoc, disabled );

	if( disabled)
	{
		OSG_NOTIFY( osg::ALWAYS ) << "..disabled by XML configuration file." << std::endl;
		return false;
	}
	
	// extract configuration values
	if(config)
	{
		xmlNode* a_node = config->children;

		for (xmlNode *cur_node = a_node; cur_node; cur_node = cur_node->next)
		{
		std::string node_name=reinterpret_cast<const char*>(cur_node->name);
			//OSG_ALWAYS << "----visual_vista2D::processXMLConfiguration() - node type="<< cur_node->type <<", name=" << cur_node->name << std::endl;

			// Check for vista2d node
			if(cur_node->type == XML_ELEMENT_NODE && node_name == "vista2d")
			{
				// Check attributes
				xmlAttr  *attr = cur_node->properties;
				while ( attr ) 
				{ 
					std::string attr_name=reinterpret_cast<const char*>(attr->name);
					std::string attr_value=reinterpret_cast<const char*>(attr->children->content);

					if( attr_name == "filename" )
					{
						vistaProjectfile=attr_value;
					}

					if( attr_name == "paintBackground" )
					{
						paintBackground = (attr_value == "yes") ? true : false;
					}

					if( attr_name == "position_x" )
					{
						std::stringstream sstr(attr_value);
						sstr >> position_x;
					}

					if( attr_name == "position_y" )
					{
						std::stringstream sstr(attr_value);
						sstr >> position_y;
					}

					if( attr_name == "zoom" )
					{
						std::stringstream sstr(attr_value);
						sstr >> zoom;
					}

					if( attr_name == "playanimation" )
					{
						playanimation = (attr_value == "yes") ? true : false;
					}

					attr = attr->next;
				}	// WHILE attr END
			}	// IF node == vista2d END
		}	// FOR end
	
		// clean up
		xmlFreeDoc(tmpDoc); xmlCleanupParser();
		return true;
	}	// IF Config valid END
	else
	{
		OSG_WARN << "ERROR: visual_vista2D::processXMLConfiguration() - Module configuration not found" << std::endl;
		return false;
	}

	return true;
}

bool visual_vista2D::init( osg::CoordinateSystemNode *sceneGraphRoot_, std::string configFileName )
{
	OSG_NOTIFY (osg::ALWAYS ) << "visual_vista2D initialize..";  // The sentence is finished by the init result...

	this->configFileName = configFileName;

	// Process XML configuration
	if(!processXMLConfiguration())
		return false;	// Abort vista2D initialization.

	// Check if Vista2D project file exists
	if ( !osgDB::fileExists(vistaProjectfile) )
	{
		OSG_ALWAYS << "WARNING: visual_vista2D::init() - Specified vista2D projectfile '"<< vistaProjectfile <<"' does not exist! Skip using Vista2D!" << std::endl;
		return false;
	}

   osg::Geode* vista2Dgeode = new osg::Geode();
   // Projection node for defining view frustrum for the vista2D-SceneOverlay:
   osg::Projection* vista2DProjectionMatrix = new osg::Projection;

    // Initialize the projection matrix for viewing everything we
   // will add as descendants of this node. Use screen coordinates
   // to define the horizontal and vertical extent of the projection
   // matrix. Positions described under this node will equate to
   // pixel coordinates.
   vista2DProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,1600,0,900)); /** todo: use screen size dynamically. */
  
   // For the HUD model view matrix use an identity matrix:
   osg::MatrixTransform* vista2DModelViewMatrix = new osg::MatrixTransform;
   vista2DModelViewMatrix->setMatrix(osg::Matrix::identity());

   // Make sure the model view matrix is not affected by any transforms
   // above it in the scene graph:
   vista2DModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

   // Set Stateset of vista2DGeode Renderbin to render quite late..
	osg::StateSet* vista2DoverlayStateSet = vista2Dgeode->getOrCreateStateSet();
	vista2DoverlayStateSet->setRenderBinDetails( 99, "RenderBin");

   // Add the HUD projection matrix as a child of the root node
   // and the HUD model view matrix as a child of the projection matrix
   // Anything under this node will be viewed using this projection matrix
   // and positioned with this model view matrix.
   vista2DProjectionMatrix->addChild(vista2DModelViewMatrix);

   // Add the Geometry node to contain HUD geometry as a child of the
   // HUD model view matrix.
   vista2DModelViewMatrix->addChild( vista2Dgeode );

	// Add Payload: Vista2D			
	startVista2D();

	// Add this class to the scene Graph
	this->setUseDisplayList( false );
	vista2Dgeode->addDrawable( this );

	sceneGraphRoot_->addChild( vista2DProjectionMatrix );
	return true;
}

void visual_vista2D::drawImplementation(osg::RenderInfo& renderInfo) const
{
	// save OSG-State
	osg::ref_ptr<osg::StateSet> clonedStateSet = reinterpret_cast<osg::StateSet*>( renderInfo.getCurrentCamera()->getOrCreateStateSet()->clone(osg::CopyOp::DEEP_COPY_ALL) );

	// Draw View
    view->draw();

	// restore OSG-State
	renderInfo.getCurrentCamera()->setStateSet( clonedStateSet );

}

osg::Object* visual_vista2D::cloneType() const
{
	return NULL;
}

osg::Object* visual_vista2D::clone(const osg::CopyOp& copyop) const
{ 
	return NULL;
}
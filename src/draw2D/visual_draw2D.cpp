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

#include <visual_draw2D.h>

using namespace osgVisual;

visual_draw2D::visual_draw2D(void)
{
	OSG_NOTIFY( osg::ALWAYS ) << "visual_draw2D constructed" << std::endl;
	initialized = false;
}

visual_draw2D::~visual_draw2D(void)
{
	OSG_NOTIFY( osg::ALWAYS ) << "visual_draw2D destroyed: " << std::endl;
}

visual_draw2D* visual_draw2D::getInstance()
{
	static visual_draw2D instance; 
	return &instance; 
};

bool visual_draw2D::init( osg::CoordinateSystemNode *sceneGraphRoot_, osgViewer::Viewer *viewer_ )
{
    screen_width = viewer_->getCamera()->getViewport()->width();
    screen_height = viewer_->getCamera()->getViewport()->height();
 
   // Projection node for defining view frustrum for the HUD:
	draw2DProjectionMatrix = new osg::Projection;

   // Initialize the projection matrix for viewing everything we
   // will add as descendants of this node. Use screen coordinates
   // to define the horizontal and vertical extent of the projection
   // matrix. Positions described under this node will equate to
   // pixel coordinates.
   draw2DProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,screen_width,0,screen_height));

   // For the HUD model view matrix use an identity matrix:
   draw2DModelViewMatrix = new osg::MatrixTransform;
   draw2DModelViewMatrix->setMatrix(osg::Matrix::identity());

   // Make sure the model view matrix is not affected by any transforms
   // above it in the scene graph:
   draw2DModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   

   // Add the HUD projection matrix as a child of the root node
   // and the HUD model view matrix as a child of the projection matrix
   // Anything under this node will be viewed using this projection matrix
   // and positioned with this model view matrix.
   draw2DProjectionMatrix->addChild(draw2DModelViewMatrix);
   sceneGraphRoot_->addChild( draw2DProjectionMatrix );

   initialized = true;
   return true;
}

void visual_draw2D::shutdown()
{
	if (!initialized)
		return;

	// Remove all Drawcontens
	removeAllDrawContents();

	// Remove Projectionmatrix
	draw2DProjectionMatrix->removeChild( draw2DModelViewMatrix );
	draw2DProjectionMatrix = NULL;

	initialized = false;
}

bool visual_draw2D::addDrawContent( osg::Geode* content_, std::string name_,int renderBinDetail_ )
{
	if (!initialized)
		return false;

	// Add the Geometry node to contain HUD geometry as a child of the
    // HUD model view matrix.
	draw2DModelViewMatrix->addChild( content_ );
	content_->setName( name_ );
	
	osg::StateSet* stateSet = content_->getOrCreateStateSet();
	stateSet->setRenderBinDetails( renderBinDetail_, "RenderBin");
	// disable depth test to ensure that it is always drawn.
	stateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);

	return true;
}

osg::Geode* visual_draw2D::getDrawContent( std::string name_ )
{
	osg::Node* tmp = util::findNamedNode(name_, draw2DModelViewMatrix);
	if(tmp)
	{
		osg::Geode* tmp2 = tmp->asGeode();
		if (tmp2)
			return tmp2;
		else
			return NULL;
	}
	else 
		return NULL;
}

bool visual_draw2D::removeDrawContent( std::string name_ )
{
	osg::Node* tmp = util::findNamedNode(name_, draw2DModelViewMatrix);
	if(tmp)
	{
		draw2DModelViewMatrix->removeChild( tmp );
		return true;
	}
	else
		return false;
}
	
void visual_draw2D::removeAllDrawContents()
{
	if (getDrawContentNum() > 0)
		draw2DModelViewMatrix->removeChildren(0, getDrawContentNum() );
}

int visual_draw2D::getDrawContentNum()
{
	return draw2DModelViewMatrix->getNumChildren();
}
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

#include <visual_draw3D.h>

using namespace osgVisual;

visual_draw3D::visual_draw3D(void)
{
}

visual_draw3D::~visual_draw3D(void)
{
}

void visual_draw3D::init( osg::CoordinateSystemNode* rootNode_, osgViewer::Viewer *viewer_ )
{
	osg::Matrix testmatrix = viewer_->getCamera()->getViewMatrix();

	testmatrix.makeTranslate( 2.0, 2.0, 2.0 );
	//testmatrix.
	osg::ref_ptr<osg::Geode> hudGeode = new osg::Geode();
	rootNode_->addChild( hudGeode );


    // create background
	
	// configuring background shape
	float depth = -0.1; // Paint background behind text. 
	osg::Vec4f bgColor_rgba = osg::Vec4(1.0f,1.0,0.8f,0.2f);
	osg::Vec3 hudCenter = osg::Vec3(0.0, 0.0, 0.0);
	float baseSize = 200;
	float addSize = 70;


	osg::Geometry* hudBackground = new osg::Geometry;

	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(hudCenter+osg::Vec3(baseSize+addSize, baseSize, depth));
	vertices->push_back(hudCenter+osg::Vec3(baseSize, baseSize+addSize, depth));
	vertices->push_back(hudCenter+osg::Vec3(-baseSize , baseSize+addSize, depth));
	vertices->push_back(hudCenter+osg::Vec3(-baseSize-addSize, baseSize,depth));
	vertices->push_back(hudCenter+osg::Vec3(-baseSize-addSize, -baseSize,depth));
	vertices->push_back(hudCenter+osg::Vec3(-baseSize , -baseSize-addSize, depth));
	vertices->push_back(hudCenter+osg::Vec3(baseSize, -baseSize-addSize, depth));
	vertices->push_back(hudCenter+osg::Vec3(baseSize+addSize, -baseSize, depth));
	hudBackground->setVertexArray(vertices);
	
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
	hudBackground->setNormalArray(normals);
	hudBackground->setNormalBinding(osg::Geometry::BIND_OVERALL);
	
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( bgColor_rgba );
	hudBackground->setColorArray(colors);
	hudBackground->setColorBinding(osg::Geometry::BIND_OVERALL);
	
	hudBackground->addPrimitiveSet(new osg::DrawArrays(GL_POLYGON,0,8));
	
	osg::StateSet* stateset = hudBackground->getOrCreateStateSet();
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

}

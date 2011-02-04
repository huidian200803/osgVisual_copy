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

#include <visual_hud.h>

using namespace osgVisual;

visual_hud::visual_hud(void)
{
}

visual_hud::~visual_hud(void)
{
}

bool visual_hud::init( osgViewer::Viewer *viewer_ )
{	
    screen_width = viewer_->getCamera()->getViewport()->width();
    screen_height = viewer_->getCamera()->getViewport()->height();
   	
	visual_draw2D::getInstance()->addDrawContent( addContent(), "HUD" );

	return true;
}

osg::ref_ptr<osg::Geode> visual_hud::addContent()
{
	osg::ref_ptr<osg::Geode> hudGeode = this;

	// turn lighting off
    osg::StateSet* stateset = hudGeode->getOrCreateStateSet();
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    // create background
	{
		// configuring background shape
		float depth = -0.1; // Paint background behind text. 
		osg::Vec4f bgColor_rgba = osg::Vec4(1.0f,1.0,0.8f,0.2f);
		osg::Vec3 hudCenter = osg::Vec3((float)(screen_width/2), (float)(screen_height/2), 0.0);
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

		hudGeode->addDrawable(hudBackground);    
	}
    
	// Add Text: 
	//osg::Vec3 position(550.0f,450.0f,0.0f);
	osg::Vec3 position(100.0f,100.0f,0.0f);
	osg::Vec3 delta(0.0f,-120.0f,0.0f);
	std::string timesFont("fonts/arial.ttf");
	{
		osgText::Text* text = new  osgText::Text;
		hudGeode->addDrawable( text );

		text->setFont(timesFont);
		text->setPosition(position);
		text->setText("Head Up Displays\nare simple ;-)");

		position += delta;
	}

	return hudGeode.get();
}
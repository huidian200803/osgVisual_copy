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

#include <visual_debug_hud.h>

using namespace osgVisual;

visual_debug_hud::visual_debug_hud(void)
{
}

visual_debug_hud::~visual_debug_hud(void)
{
}

bool visual_debug_hud::init( osgViewer::Viewer *viewer_, osg::CoordinateSystemNode* rootNode_  )
{	



    screen_width = viewer_->getCamera()->getViewport()->width();
    screen_height = viewer_->getCamera()->getViewport()->height();
   	
	visual_draw2D::getInstance()->addDrawContent( addContent(), "HUD" );

	// Set callback.
	updateCallback = new HudUpdateCallback( rootNode_, viewer_->getCamera() );
	this->setEventCallback( updateCallback );

	isInitialized = true;

	return true;
}

void visual_debug_hud::shutdown()
{
	if (isInitialized)
	{	
		// Remove Draw Content 
		visual_draw2D::getInstance()->removeDrawContent("HUD");

		// Remove updatecallback
		this->removeEventCallback( updateCallback );
		updateCallback = NULL;
	}
}

osg::ref_ptr<osg::Geode> visual_debug_hud::addContent()
{
	osg::ref_ptr<osg::Geode> hudGeode = this;

	// turn lighting off
	osg::ref_ptr<osg::StateSet> stateset = hudGeode->getOrCreateStateSet();
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);


    
	// Add Text: 
	osg::Vec3 position(screen_width-180.0f,120.0f,0.0f);
	osg::Vec3 delta(0.0f,-25.0f,0.0f);
	std::string timesFont("fonts/arial.ttf");
	{
		textLat = new osgText::Text;
		hudGeode->addDrawable( textLat );
		textLat->setFont(timesFont);
		textLat->setPosition(position);
		textLat->setCharacterSize(21);
		textLat->setDataVariance(osg::Object::DYNAMIC); 

		position += delta;

		textLon = new osgText::Text;
		hudGeode->addDrawable( textLon );
		textLon->setFont(timesFont);
		textLon->setPosition(position);
		textLon->setCharacterSize(21);
		textLon->setDataVariance(osg::Object::DYNAMIC); 

		position += delta;

		textAlt = new osgText::Text;
		hudGeode->addDrawable( textAlt );
		textAlt->setFont(timesFont);
		textAlt->setPosition(position);
		textAlt->setCharacterSize(21);
		textAlt->setDataVariance(osg::Object::DYNAMIC); 

		position += delta;

		textHat = new osgText::Text;
		hudGeode->addDrawable( textHat );
		textHat->setFont(timesFont);
		textHat->setPosition(position);
		textHat->setCharacterSize(21);
		textHat->setDataVariance(osg::Object::DYNAMIC); 

		position += delta;

		textHot = new osgText::Text;
		hudGeode->addDrawable( textHot );
		textHot->setFont(timesFont);
		textHot->setPosition(position);
		textHot->setCharacterSize(21);
		textHot->setDataVariance(osg::Object::DYNAMIC); 

		position += delta;
	}

	return hudGeode.get();
}

void visual_debug_hud::HudUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	visual_debug_hud* hud = dynamic_cast<visual_debug_hud*>(node);
	if ( !hud )
	{
		OSG_NOTIFY(osg::FATAL) << "ERROR : No object found. Unable to apply this callback!" << std::endl;
		return;
	}

	// Retrieving new HUD values

	double lat = 0;
	double lon = 0;
	double hat = 0;
	double hot = 0;
	double alt = 0;

	util::getWGS84ofCamera( sceneCamera, csn, lat, lon, alt ); 
	util::queryHeightAboveTerrainInWGS84(hat, csn, lat, lon, alt);
	util::queryHeightOfTerrain(hot, csn, lat, lon);


	// Updating Display Elements:
	std::ostringstream valuestring;
	
	valuestring.str("");
	valuestring << osg::RadiansToDegrees( lat );
	hud->textLat->setText("LAT: "+valuestring.str());

	valuestring.str("");
	valuestring << osg::RadiansToDegrees( lon );
	hud->textLon->setText("LON: "+valuestring.str());

	valuestring << std::fixed;
	valuestring.precision(2);
	valuestring.fill('0');

	valuestring.width(8);
	valuestring.str("");
	valuestring << alt;
	hud->textAlt->setText("ALT: "+valuestring.str());


	valuestring.width(8);
	valuestring.str("");
	valuestring << hat;
	hud->textHat->setText("HAT: "+valuestring.str());

	valuestring.width(8);
	valuestring.str("");
	valuestring << hot;
	hud->textHot->setText("HOT: "+valuestring.str());
}
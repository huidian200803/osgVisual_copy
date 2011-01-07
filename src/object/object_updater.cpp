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

#include <object_updater.h>

using namespace osgVisual;

object_updater::object_updater(osgVisual::visual_object* object_ )
{
}

object_updater::~object_updater(void)
{
}

void object_updater::preUpdate(osgVisual::visual_object* object_ )
{
	OSG_NOTIFY( osg::INFO ) << "preUpdate visual Object " << object_->getName() << std::endl;
	// perform this preUpdater...
	//For each visual_object.member,
	//	try to search according variable in dataIO with direction TO_OBJ and copy value to visual_object.

	
	object_->lat = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(object_->getName()+"_POS_LAT", osgVisual::dataIO_slot::TO_OBJ ));
	object_->lon = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(object_->getName()+"_POS_LON", osgVisual::dataIO_slot::TO_OBJ ));
	object_->alt = osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(object_->getName()+"_POS_ALT", osgVisual::dataIO_slot::TO_OBJ );
	object_->azimuthAngle_psi = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(object_->getName()+"_ROT_Z", osgVisual::dataIO_slot::TO_OBJ ));
	object_->pitchAngle_theta = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(object_->getName()+"_ROT_Y", osgVisual::dataIO_slot::TO_OBJ ));
	object_->bankAngle_phi = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(object_->getName()+"_ROT_X", osgVisual::dataIO_slot::TO_OBJ ));

	//object_->setCameraOffsetTranslation(0.0, -150.0, 50.0);	// Trans: (rechts davon, longitudinal, vertikal)
	object_->setCameraOffsetTranslation( 150.0, 0.0, 30.0);
	object_->setCameraOffsetRotation( osg::DegreesToRadians(0.0), osg::DegreesToRadians(-15.0), osg::DegreesToRadians(-90.0) );
	


	// Finally execute nested PreUpdater
	if ( updater.valid() )
		updater->preUpdate(object_);
}

void object_updater::postUpdate(osgVisual::visual_object* object_ )
{
	OSG_NOTIFY( osg::INFO ) << "postUpdate visual Object " << object_->getName() << std::endl;

	// Finally execute nested PostUpdater
	if ( updater.valid() )
		updater->postUpdate(object_);
}

void object_updater::addUpdater( object_updater* updater_ )
{
	if (updater.valid())
		updater->addUpdater( updater_ );
	else
		updater = updater_;
}
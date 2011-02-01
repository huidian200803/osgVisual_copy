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

	if(!updater_lat.empty())
		object_->lat = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(updater_lat, osgVisual::dataIO_slot::TO_OBJ ));
	if(!updater_lon.empty())
		object_->lon = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(updater_lon, osgVisual::dataIO_slot::TO_OBJ ));
	if(!updater_alt.empty())
		object_->alt = osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(updater_alt, osgVisual::dataIO_slot::TO_OBJ );
	if(!updater_rot_z.empty())
		object_->azimuthAngle_psi = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(updater_rot_z, osgVisual::dataIO_slot::TO_OBJ ));
	if(!updater_rot_y.empty())
		object_->pitchAngle_theta = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(updater_rot_y, osgVisual::dataIO_slot::TO_OBJ ));
	if(!updater_rot_x.empty())
		object_->bankAngle_phi = osg::DegreesToRadians(osgVisual::visual_dataIO::getInstance()->getSlotDataAsDouble(updater_rot_x, osgVisual::dataIO_slot::TO_OBJ ));
	if(!updater_label.empty())
		object_->updateLabelText("autoupdated", osgVisual::visual_dataIO::getInstance()->getSlotDataAsString(updater_label, osgVisual::dataIO_slot::TO_OBJ ));

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

void object_updater::setUpdaterSlotNames( osgVisual::visual_object* object_, std::string lat_, std::string lon_, std::string alt_, std::string rot_x_, std::string rot_y_, std::string rot_z_, std::string label_)
{
	if(lat_!="")
		updater_lat = lat_;
	else
		updater_lat = object_->getName()+"_POS_LAT";

	if(lon_!="")
		updater_lon = lon_;
	else
		updater_lon = object_->getName()+"_POS_LON";

	if(alt_!="")
		updater_alt = alt_;
	else
		updater_alt = object_->getName()+"_POS_ALT";

	if(rot_x_!="")
		updater_rot_x = rot_x_;
	else
		updater_rot_x = object_->getName()+"_ROT_X";

	if(rot_y_!="")
		updater_rot_y = rot_y_;
	else
		updater_rot_y = object_->getName()+"_ROT_Y";

	if(rot_z_!="")
		updater_rot_z = rot_z_;
	else
		updater_rot_z = object_->getName()+"_ROT_Z";

	if(label_!="")
		updater_label = label_;
	else
	{
		updater_label = object_->getName()+"_LABEL";
		object_->addLabel("autoupdated", " ");
	}
}
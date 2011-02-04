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

#include <osg/Node>
#include <osg/Notify>
#include <visual_object.h>

namespace osgVisual
{ 
class visual_object;	// Forward declaration


/**
 * \brief This class is the base class for visual_object updater.
 * 
 * This base implementation updates the object position with the slotnames
 *  object_->getName()+"_POS_LAT", object_->getName()+"_POS_LON", object_->getName()+"_POS_ALT",
 * object_->getName()+"_ROT_X", object_->getName()+"_ROT_Y", object_->getName()+"_ROT_Z", 
 * 
 * To implement custom updaters, please derive from this class and overwrite the functions preUpdate(..) and postUpdate(..).
 * Take care to call nested preUpdater and postUpdater to allow updater chains.
 * 
 * @author Torben Dannhauer
 * @date  Apr 2010
 */ 
class object_updater : public osg::Node
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Empty constructor
	 * 
	 * @param object_ : Pointer to the object, calling the constructor (currently not used)
	 */ 
	object_updater(osgVisual::visual_object* object_ );

	/**
	 * \brief Empty destructor
	 * 
	 * @return 
	 */ 
	virtual ~object_updater(void);

	/**
	 * \brief This function is called by visual_object before rendering (preFrame).
	 * 
	 * This updater function should be used to update the objects state like size, position, attitude etc.
	 * 
	 * @param object_ : Pointer to the object calling this updater.
	 */ 
	virtual void preUpdate(osgVisual::visual_object* object_ );

	/**
	 * \brief This function is called by visual_object after rendering (postFrame).
	 * 
	 * This updater function should be used for all actions which require the object status of that frame like HAT/HOT etc.
	 * 
	 * @param object_ : Pointer to the object calling this updater.
	 */ 
	virtual void postUpdate(osgVisual::visual_object* object_);

	/**
	 * \brief Returns pointer to this updater.
	 * 
	 * This function is used for updater list management.
	 * 
	 * @return : Pointer to this updater.
	 */ 
	object_updater* getPointer(){return this;};

	/**
	 * \brief This function adds an object_updater as nested updater.
	 * 
	 * The nested Updater will be executed in pre and post traversal after this updater.
	 * 
	 * @param updater_ 
	 */ 
	void addUpdater( object_updater* updater_ );

	/**
	 * \brief This function defines the slots to use for updating the object.
	 * 
	 * If a lot is empty, the objectname will be used with a suffix describing the channel. For details look for the implementation.
	 * 
	 * @param lat_ 
	 * @param lon_ 
	 * @param alt_ 
	 * @param rot_x_ 
	 * @param rot_y_ 
	 * @param rot_z_ 
	 * @param label_ 
	 */ 
	void setUpdaterSlotNames( osgVisual::visual_object* object_, std::string lat_, std::string lon_, std::string alt_, std::string rot_x_, std::string rot_y_, std::string rot_z_, std::string label_);


protected:
	/**
	 * Pointer to nested updater.
	 */ 
	osg::ref_ptr<object_updater> updater;

	/**
	 * Names of the Slots the updater should use
	 */ 
	std::string updater_lat, updater_lon, updater_alt, updater_rot_x, updater_rot_y, updater_rot_z, updater_label;

};

}	// END NAMESPACE

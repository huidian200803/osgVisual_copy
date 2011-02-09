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

#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osg/NodeCallback>
#include <osg/CoordinateSystemNode>
#include <osg/Notify>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osgText/Text>

#include <osgGA/CameraManipulator>

#include <object_updater.h>
#include <visual_dataIO.h>
#include <visual_util.h>

#include <string.h>
#include <iostream>

// XML Parser
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

namespace osgVisual
{
class visual_objectPositionCallback;
class object_updater;
}

/**
 * \brief Standard namespace of osgVisual
 * 
 */ 
namespace osgVisual
{

/**
 * \brief This class provides object management for displaying objects in the 3D scene.
 * 
 * It allows to control an object in position, attitude and size. To control this object automatically, use object_updaters.
 * To object itself is invisible, you have to connect a visible geometry to this object.
 * To display geometries, it provides function to load geometries by geometry pointer or filename.
 * To correct wrong coordinate frames in the geometries to display,
 * you can configure a geometry off set in translation in translation and rotation
 * For object mounted camera, you can configure the camera offset in rotation and translation.
 * 
 * All angles are defined in degree.
 * 
 * Derive from this class to implement your custom visual_object.
 * 
 * \todo: Labelmanagement to allow to display a label attached to this object: set/unset label, size, color, offset to object, LOD for label display.
 * 
 * @author Torben Dannhauer
 * @date  Apr 2010
 */ 
class visual_object  : public osg::MatrixTransform
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Constuctor: Adds this object to the scenegraph,
	 * initializes this object and installs the callback to calculate its local to world matrix
	 * 
	 * @param sceneRoot_ : Scenegraph to add this object to.
	 * @param nodeName_ : Name of this object, is used for further identification.
	 */ 
	visual_object( osg::CoordinateSystemNode* sceneRoot_, std::string nodeName_ );
	
	/**
	 * \brief Empty destructor.
	 * 
	 */ 
	~visual_object();

	static visual_object* createNodeFromXMLConfig(osg::CoordinateSystemNode* sceneRoot_, xmlNode* a_node);


/** @name Position and attitude
 *  These functions control objects position and attitude
 */
/*@{*/

	/**
	 * \brief This function updates the objects position and attitude. 
	 * 
	 * @param lat_ : Latitude to set.
	 * @param lon_ : Longitude to set.
	 * @param alt_ : Altitude over ellipsoid to set.
	 * @param azimuthAngle_psi_ : Psi angle () to set. 
	 * @param pitchAngle_theta_ : Theta angle () to set.
	 * @param bankAngle_phi_ : Phi angle () to set.
	 * 
	 * All angles are rad!
	 * 
	 * \todo: Erklären welche Wirkung die drei Winkel haben.
	 */ 
	void setNewPositionAttitude( double lat_, double lon_, double alt_, double azimuthAngle_psi_, double pitchAngle_theta_, double bankAngle_phi_ );
	
	/**
	 * \brief This function updates the objects position.
	 * 
	 * @param lat_ : Latitude to set.
	 * @param lon_ : Longitude to set.
	 * @param alt_ : Altitude over ellipsoid to set.
	 */ 
	void setNewPosition( double lat_, double lon_, double alt_ );

	/**
	 * \brief This function updates the objects attitude.
	 * 
	 * @param azimuthAngle_psi_ : Psi angle () to set. 
	 * @param pitchAngle_theta_ : Theta angle () to set.
	 * @param bankAngle_phi_ : Phi angle () to set.
	 * 
	 * All angles are rad!
	 * 
	 * \todo: Erklären welche Wirkung die drei Winkel haben.
	 */ 
	void setNewAttitude( double azimuthAngle_psi_, double pitchAngle_theta_, double bankAngle_phi_ );
/*@}*/
/** @name Geometry management
 *  These functions control which geometry visual_object should display.
 */
/*@{*/
	/**
	 * \brief This function configures the geometry offset
	 * 
	 * @param rotX_ : Geometry rotation along the X axis.
	 * @param rotY_ : Geometry rotation along the Y axis.
	 * @param rotZ_ : Geometry rotation along the Z axis.
	 * 
	 * It rotates along the axis of the visual_object, not along the geometry axis!
	 * 
	 * \todo: Erklären, welche Achsen welche Richtung sind.
	 */ 
	void setGeometryOffset( double rotX_, double rotY_, double rotZ_ );

	/**
	 * \brief This function set the scale to all 3 axis. Use this function to scale objects without distortion.
	 * 
	 * @param scale_ : Scale to set for every axis.
	 */ 
	void setScale( double scale_ );

	/**
	 * \brief This function sets the scale factor for all 3 axis independently.
	 * 
	 * @param scaleX_ : Scalefactor along X axis.
	 * @param scaleY_ : Scalefactor along Y axis.
	 * @param scaleZ_ : Scalefactor along Z axis.
	 */ 
	void setScale( double scaleX_, double scaleY_, double scaleZ_ );

	/**
	 * \brief this function loads a geometry from a file and connects it to this visual_object. All filetypes OSG is aware of can be used.
	 * 
	 * @param filename_ : File to load and connect.
	 * @return : True if loading was successfully
	 */ 
	bool loadGeometry( std::string filename_);

	/**
	 * \brief This function connects a geometry to this visual_object.
	 * 
	 * @param geometry_ : Geometry to connect.
	 * @return : True if successful.
	 */ 
	bool setGeometry(osg::Node* geometry_);

	/**
	 * \brief This function disconnects the connected object.
	 * 
	 */ 
	void unsetGeometry();

	/**
	 * \brief This function returns the pointer to the loaded geometry.
	 *
	 * It returns NULL if no geometry is connected.
	 * 
	 * @return : Pointer to the geometry if loaded, otherwise NULL.
	 */ 
	osg::Node* getGeometry(){return geometry;}

/*@}*/
/** @name Object updater
 *  These functions control objects updater.
 */
/*@{*/
	/**
	 * \brief This function adds an Updater to this object.
	 * 
	 * The updaters postUpdate und preUpdate functions are executed during eventTraversal traversal.
	 * The preUdate is executed before the objects matrix is calculated from position, the postUpdate is executed after the matrix calculation. 
	 * 
	 * @param updater_ : Updater to add.
	 */ 
	void addUpdater( object_updater* updater_ );

	/**
	 * \brief Removes all updater from the visual_object. 
	 * 
	 */ 
	void clearAllUpdater();

	/**
	 * \brief This function returns a vector containing pointer to all updater of the object.
	 * 
	 * @return : vector with pointers to all object_updater.
	 */ 
	std::vector<object_updater*> getUpdaterList();

/*@}*/
/** @name Camera management
 *  These functions control the camera attitude and position.
 */
/*@{*/
	/**
	 * \brief This function returns the camera matrix of the scene camera.
	 * 
	 * @return : Camera matrix
	 */ 
	osg::Matrixd& getCameraMatrix() {return cameraMatrix;};

	/**
	 * \brief This function rotates and translate the objects camera position.
	 * 
	 *  The translation coordinate frame is object fixed, the rotation coordinate frame is camera fixed.
	 * 
	 * @param x_ : Translation along object's x axis.
	 * @param y_ : Translation along object's y axis.
	 * @param z_ : Translation along object's z axis (positive axis points downwards).
	 * @param rotX_ : Rotation around camera's x axis in [rad].
	 * @param rotY_ : Rotation around camera's y axis in [rad]. 
	 * @param rotZ_ : Rotation around camera's z axis in [rad].
	 */ 
	void setCameraOffset(double x_, double y_, double z_, double rotX_, double rotY_, double rotZ_);

	/**
	 * \brief This function configures the camera offset rotation in that object.
	 * 
	 * The coordinate frame for this rotation is camera fixed, NOT object fixed 
	 * (e.g. rotation around x axis (bank) rotatates always around the cameras visual axis, 
	 * independent from pitch or azimuth)
	 * 
	 * @param rotX_ : Rotation around camera's x axis in [rad].
	 * @param rotY_ : Rotation around camera's y axis in [rad]. 
	 * @param rotZ_ : Rotation around camera's z axis in [rad].
	 */ 
	void setCameraOffsetRotation(double rotX_, double rotY_, double rotZ_);

	/**
	 * \brief This function configures the camera offset translation in that object.
	 * 
	 * @param x_ : Translate in longitudinal direction. x>0: Camera is in front if the ob
	 * ject, x<0 behind the object.
	 * @param y_ : Translate in transversal direction. x>0: Camera is right of the object, x<0 left of the object.
	 * @param z_ : Translate in vertical direction. x>0: Camera is below the Object, x<0 above the object. (Positiv Z axis points downwards!)
	 */ 
	void setCameraOffsetTranslation( double x_, double y_, double z_);

	/**
	 * \brief This function sets the trackingId to allow to identify the visual_obejct for tracking purposes.
	 * 
	 * See also getTrackingId() and trackingId
	 * 
	 * @param trackindID_ : trackingId to set.
	 */ 
	void setTrackingId(int trackingId_) {trackingId = trackingId_;};

	/**
	 * \brief This function returns the trackingId to allow to identify the visual_obejct for tracking purposes.
	 * 
	 * @return : -1 If no trackingId is set, othwerwise the trackingId.
	 */ 
	int getTrackingId() {return trackingId;};

/*@}*/
/** @name Label management
 *  These functions allow to display labels attached to visual_object.
 */
/*@{*/
	/**
	 * \brief This function removes all labels attached to this object.
	 * 
	 */ 
	void clearLabels();

	/**
	 * \brief This function adds a label to the object.  
	 * 
	 * @param idString_ : Identifiy string, which is used to find and manipulate the label later.
	 * @param label_ : Message to display as label.
	 * @param color_ : Textcolor. Defaults to white.
	 * @param offset_ : Offset im meter in the objects local coordinate frame
	 */ 
	void addLabel(std::string idString_, std::string label_, osg::Vec4 color_ = osg::Vec4(1.0f,1.0f,1.0f,1.0f), osg::Vec3 offset_ = osg::Vec3(0,0,0));
	
	/**
	 * \brief This function removes the specified label.
	 * 
	 * @param idString_ : idString of the label to remove.
	 * @return : True if a label with the specified idString_ was found, false if no label with that idString was found.
	 */ 
	bool removeLabel(std::string idString_);

	/**
	 * \brief Update the message of the label specified with the idString_.
	 * 
	 * @param idString_ : idString of the label to update.
	 * @param label_ : New message to set.
	 * @return : Return true if the specified label was found, return false if the specified label was not found.
	 */ 
	bool updateLabelText(std::string idString_, std::string label_);

	/**
	 * \brief returns the message string of the specified label.
	 * 
	 * @param idString_ : idString of the label to return message.
	 * @return : Pointer to the Label (osgText::Text) if found, otherwise NULL
	 */ 
	osgText::Text* getLabel(std::string idString_); 

	/**
	 * \brief This functions sets the draw mode of the specified label.
	 * 
	 * @param idString_ : Label to configure.
	 * @param drawAsOverlay : Set true to draw the label always over the scene. Set false to draw it 3D into the scene (maybe occluded party by objects)
	 * @return : Return true if the specified label was found, return false if the specified label was not found. 
	 */ 
	bool setDrawLabelAsOverlay(std::string idString_, bool drawAsOverlay);

	/**
	 * \brief This function returns if the specified label is configured as overlay.
	 * 
	 * @param idString_ : Label to configure.
	 * @return : True if configured as overlay, false if configured for simple draw
	 */ 
	bool getDrawLabelAsOverlay(std::string idString_ );
/*@}*/

protected:
	/**
	 * \brief Callback class for updating the visual_object while traversing the scenegraph in the event stage.
	 * 
	 * @author Torben Dannhauer
	 * @date  Jul 2009
	 */ 
	class visual_objectPositionCallback : public osg::NodeCallback
	{
	public:
		/**
		 * \brief Constructor: Empty
		 * 
		 */ 
		visual_objectPositionCallback()
		{
		   // nothing
		}
	    
		/**
		 * \brief This function is executed by the callback during update traversal.
		 * 
		 */ 
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	};

	osg::Vec3 upVector;
	
// Position
	/**
	 * Latitude of the object in RAD.
	 */ 
	double lat;

	/**
	 * Longitude of the object in RAD.
	 */ 
	double lon;

	/**
	 * Altitude of the object over the ellipsoid in meter. 
	 */ 
	double alt;

// Attitude
	/**
	 * Azimuth angle (Rotation along Z axis, "heading") of the object in RAD.
	 */ 
	double azimuthAngle_psi;

	/**
	 * Pitch ("nose relative to horizon") angle of the object in RAD.
	 */ 
	double pitchAngle_theta;

	/**
	 * Bank angle of the object in RAD.
	 */ 
	double bankAngle_phi;

// Scale
	/**
	 * Object scale along X axis.
	 */ 
	double scaleX;

	/**
	 * Object scale along Y axis.
	 */ 
	double scaleY;

	/**
	 * Object scale along Z axis.
	 */ 
	double scaleZ;

// Camera offset
	/**
	 * This is the matrix containing the camera position for the  manipulators
	 */ 
	osg::Matrixd cameraMatrix;

	/**
	 * This is the camera matrix containing offset translation
	 */ 
	osg::Matrix cameraTranslationOffset;

	/**
	 * This is the camera matrix containing offset rotation
	 */ 
	osg::Matrix cameraRotationOffset;

	/**
	 * Geometry offset (If a model's coordinate system has wrong orientation.)
	 */ 
	osg::Quat geometry_offset_rotation;	

	/**
	 * Pointer to the geometry, associated with this object.
	 */ 
	osg::ref_ptr<osg::Group> geometry;

	/**
	 * Pointer to the updater class which updates this visual_object.
	 */ 
	osg::ref_ptr<object_updater> updater;

	/**
	 * Tracking ID of the visual_object. Used to identify which node should be tracked by tracking-manipulators.
	 */ 
	int trackingId;

// Labels
	/**
	 * Pointer to the labels group which holds all labels associated with this object.
	 */ 
	osg::ref_ptr<osg::Geode> labels;

	// Friend classes
	friend class visual_objectPositionCallback; // To allow the callback access to all member variables.
	friend class object_updater;	// To allow updater to modify all members.

};

} // END NAMESPACE
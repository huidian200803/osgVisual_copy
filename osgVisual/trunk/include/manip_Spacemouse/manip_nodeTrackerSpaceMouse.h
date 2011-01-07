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

#include <osgGA/OrbitManipulator>

#include <osg/ObserverNodePath>
#include <osg/Quat>
#include <osg/Notify>
#include <osg/Transform>

// Spacemouse implementation
#include <manip_spaceMouse.h>



using namespace osgGA;

namespace osgVisual
{ 

/**
 * \brief This class provides a nodetracker manipulator controlled be a Space Navigator(SN) 3D mouse by 3DConnexion.
 * 
 * This nodetracker is similar to standard osg nodetracker manipulator with the difference,
 * that the camera is not controlled by mouse but by the Space Navigator.
 * 
 * NodeTrackerSpaceMouse provides two modi: Using Space Navigator to control the camera movement, or using Space Navigator to control camera position.
 * The first one holds camera position if the SN is released to it's origin position, the latter one returns camera position to origin if SN is released and returned to origin ("autohoming").
 * You can switch between both modi with Space.
 * 
 * To toggle between the tracking modes NODE_CENTER, NODE_CENTER_AND_AZIM and NODE_CENTER_AND_ROTATION, press 'm'.
 * 
 * To toggle adjustment of angular velocity by distance to tracked node, press 'n'.
 * 
 * @author Torben Dannhauer
 * @date  Apr 2010
 */ 
class NodeTrackerSpaceMouse : public osgGA::OrbitManipulator
{
	#include <leakDetection.h>
    public:
		/**
		 * \brief Constructor
		 * 
		 * @param spacemouse : Pointer to the driver interface class.
		 */ 
        NodeTrackerSpaceMouse(SpaceMouse* spacemouse); 

		/**
		 * \brief This function returns the class name.
		 * 
		 * @return : Char array of the class name.
		 */ 
        virtual const char* className() const { return "NodeTrackerSpaceMouse"; }


        void setTrackNodePath(const osg::NodePath& nodePath) { _trackNodePath.setNodePath(nodePath); }
        void setTrackNodePath(const osg::ObserverNodePath& nodePath) { _trackNodePath = nodePath; }
        osg::ObserverNodePath& getTrackNodePath() { return _trackNodePath; }

        void setTrackNode(osg::Node* node);
        osg::Node* getTrackNode();
        const osg::Node* getTrackNode() const;

		/**
		 * Lists the tracking modes the nodetracker can provide.
		 */ 
        enum TrackerMode 
        {
            /** Track the center of the node's bounding sphere, but not rotations of the node.  
              * For databases which have a CoordinateSystemNode, the orientation is kept relative the coordinate frame if the center of the node.
              */
            NODE_CENTER,
            /** Track the center of the node's bounding sphere, and the azimuth rotation (about the z axis of the current coordinate frame).
              * For databases which have a CoordinateSystemNode, the orientation is kept relative the coordinate frame if the center of the node.
              */
            NODE_CENTER_AND_AZIM,
            /** Tack the center of the node's bounding sphere, and the all rotations of the node.
              */
            NODE_CENTER_AND_ROTATION
        };
        
		/**
		 * \brief Sets the tracking mode.
		 * 
		 * See TrackerMode
		 * 
		 * @param mode : Mode to set
		 */ 
        void setTrackerMode(TrackerMode mode);

		/**
		 * \brief Returns current tracking mode.
		 * 
		 * @return : Current tracking mode.
		 */ 
        TrackerMode getTrackerMode() const { return _trackerMode; }


		/**
		 * Lists the rotation modes the nodetracker provides.
		 * 
		 * At the moment, only ELEVATION_AZIM is supported, TRACKBALL is blocked to prevent software crashes.
		 */ 
        enum RotationMode 
        {
            /** Use a trackball style manipulation of the view direction w.r.t the tracked orientation.
              */
            TRACKBALL,
            /** Allow the elevation and azimuth angles to be adjust w.r.t the tracked orientation.
              */
            ELEVATION_AZIM
        };
        
		/**
		 * \brief Sets the rotation mode.
		 * 
		 * See RotationMode
		 * 
		 * @param mode : Mode to set.
		 */ 
        void setRotationMode(RotationMode mode);

		/**
		 * \brief Returns curent rotation mode.
		 * 
		 * See RotationMode
		 * 
		 * @return : Current rotation mode.
		 */ 
        RotationMode getRotationMode() const { return _rotationMode; }


        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
        virtual void setByMatrix(const osg::Matrixd& matrix);

        /** set the position of the matrix manipulator using a 4x4 Matrix.*/
        virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }

        /** get the position of the manipulator as 4x4 Matrix.*/
        virtual osg::Matrixd getMatrix() const;

        /** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
        virtual osg::Matrixd getInverseMatrix() const;

        /** Get the FusionDistanceMode. Used by SceneView for setting up stereo convergence.*/
        virtual osgUtil::SceneView::FusionDistanceMode getFusionDistanceMode() const { return osgUtil::SceneView::USE_FUSION_DISTANCE_VALUE; }

        /** Get the FusionDistanceValue. Used by SceneView for setting up stereo convergence.*/
        virtual float getFusionDistanceValue() const { return _distance; }

        /** Attach a node to the manipulator. 
            Automatically detaches previously attached node.
            setNode(NULL) detaches previously nodes.
            Is ignored by manipulators which do not require a reference model.*/
        virtual void setNode(osg::Node*);

        /** Return node if attached.*/
        virtual const osg::Node* getNode() const;

        /** Return node if attached.*/
        virtual osg::Node* getNode();

        virtual void computeHomePosition();

        /** Move the camera to the default position. 
            May be ignored by manipulators if home functionality is not appropriate.*/
        virtual void home(const GUIEventAdapter& ea,GUIActionAdapter& us);
        
        /** Start/restart the manipulator.*/
        virtual void init(const GUIEventAdapter& ea,GUIActionAdapter& us);

        /** handle events, return true if handled, false otherwise.*/
        virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& us);

        /** Get the keyboard and mouse usage of this manipulator.*/
        virtual void getUsage(osg::ApplicationUsage& usage) const;

    protected:

        virtual ~NodeTrackerSpaceMouse();

        /** Reset the internal GUIEvent stack.*/
        void flushMouseEventStack();

        /** Add the current mouse GUIEvent to internal stack.*/
        void addMouseEvent(const GUIEventAdapter& ea);

        void computeNodeCenterAndRotation(osg::Vec3d& center, osg::Quat& rotation) const;

        void computePosition(const osg::Vec3d& eye,const osg::Vec3d& lv,const osg::Vec3d& up);

        /** For the give mouse movement calculate the movement of the camera.
            Return true is camera has moved and a redraw is required.*/
        bool calcMovement();

		/**
		 * \brief Retieves the movements from driver interface and calculates manipulator movements.
		 * 
		 * @return : True on success.
		 */ 
		bool calcMovementSpaceMouse();
        
        void trackball(osg::Vec3& axis,double& angle, double p1x, double p1y, double p2x, double p2y);
        double tb_project_to_sphere(double r, double x, double y);


        /** Check the speed at which the mouse is moving.
            If speed is below a threshold then return false, otherwise return true.*/
        bool isMouseMoving();

		/** Empty */
        void clampOrientation();


        // Internal event stack comprising last two mouse events.
        osg::ref_ptr<const GUIEventAdapter> _ga_t1;
        osg::ref_ptr<const GUIEventAdapter> _ga_t0;

		/**
		 * Pointer to node to track.
		 */ 
        osg::observer_ptr<osg::Node> _node;

        osg::ObserverNodePath   _trackNodePath;

		/**
		 * Current Trackermode. See TrackerMode. 
		 */ 
        TrackerMode             _trackerMode;

		/**
		 * Curent Rotation Mode. See RotationMode.
		 */ 
        RotationMode            _rotationMode;

        bool                    _thrown;

		/**
		 * Indicates if auto homing is enabled.
		 */ 
		bool					_autohoming;
		
		/**
		 * Indiccates if nodetracker is initialized.
		 */ 
		bool					_ah_init;

		/**
		 * Indicates if angular velocity is adjusted by distance to tracked node.
		 */ 
		bool					_distanceDependendAV;

        osg::Quat               _nodeRotation;
        osg::Quat               _rotation;
        float                   _distance;
		float					_lastDistance;

		/** 
		 * Pointer to Space Navigator interfacce class.
		 */ 
		SpaceMouse* _spaceMouse;

		/**
		 * Discances in translation and rotation of Space Navigator. Used for non-autohoming modus.
		 */ 
		double TZ, RX, RY, RZ;

};

}	//	END NAMESPACE


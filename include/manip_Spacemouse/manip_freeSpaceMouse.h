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

// Spacemouse implementation
#include <manip_spaceMouse.h>

// OSG includes
#include <osgGA/CameraManipulator>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

#include <osg/Quat>
#include <osg/Notify>
#include <osg/BoundsChecking>
#include <osg/observer_ptr>


namespace osgVisual
{ 

/**
 * \brief This class is a free spacemouse manipulator. It uses the 3DConnexion SpaceNavigator, which interface is implemented in manip_spaceMouse.h
 * 
 * @author Torben Dannhauer
 * @date  Aug 2009
 */ 
class FreeManipulator : public osgGA::CameraManipulator
{
	#include <leakDetection.h>
    public:
		/**
		 * \brief Constructor
		 * 
		 * @param spacemouse : Pointer to the space mouse driver instance.
		 */ 
        FreeManipulator(SpaceMouse* spacemouse);

		/**
		 * \brief This function returns the classname.
		 * 
		 * @return Classname of this manipulator.
		 */ 
        virtual const char* className() const { return "FreeManipulator"; }

		/**
		 * \brief Set the position of the matrix manipulator using a 4x4 Matrix.
		 * 
		 * @param matrix : Matrix to set.
		 */ 
        virtual void setByMatrix(const osg::Matrixd& matrix);

		/**
		 * \brief Set the position of the matrix manipulator using a 4x4 Matrix.
		 * 
		 * @param matrix : Inverse Matrix to set.
		 */ 
        virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }

        /** get the position of the manipulator as 4x4 Matrix.*/
        virtual osg::Matrixd getMatrix() const;

        /** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
        virtual osg::Matrixd getInverseMatrix() const;

		/**
		 * \brief Start/restart the manipulator.
		 * 
		 * No further implementation
		 * 
		 * @param ea : GUI eventadapter to use for initialization. 
		 * @param us : GUI actionadapter to use for initialization. 
		 */ 
        virtual void init(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

        /** handle events, return true if handled, false otherwise.*/
        virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

        /** Get the keyboard and mouse usage of this manipulator.*/
        virtual void getUsage(osg::ApplicationUsage& usage) const;

        void setTrackNode(osg::Node* node);
        osg::Node* getTrackNode() {  return _trackNodePath.empty() ? 0 : _trackNodePath.back().get();  }

        void computeNodeWorldToLocal(osg::Matrixd& worldToLocal) const;
        void computeNodeLocalToWorld(osg::Matrixd& localToWorld) const;
        bool validateNodePath() const;
        osg::NodePath getNodePath() const;

        typedef std::vector< osg::observer_ptr<osg::Node> >   ObserverNodePath;

        ObserverNodePath      _trackNodePath;


        void setTrackNodePath(const osg::NodePath& nodePath)
        {
           _trackNodePath.clear();
           _trackNodePath.reserve(nodePath.size());
           std::copy(nodePath.begin(), nodePath.end(), std::back_inserter(_trackNodePath));
        }

        osg::Vec3 getTrackNodeCenter();

    protected:
		/**
		 * \brief Destructor. Because it is protected, no explicit delete myPointer by external caller is possible.
		 * This forces the class to be used with osg::ref pointers.
		 */ 
        virtual ~FreeManipulator();

		/**
		 * \brief This function queries the space mouse driver instance and calculates the resultung movement of the manipulator.
		 * 
		 * @return : True if calculation was successful.
		 */ 
        bool calcMovement();

		/**
		 * Position of the manipulator.
		 */ 
        osg::Vec3d  _position;

		/**
		 * Attitude of the manipulator.
		 */ 
        osg::Quat   _attitude;

		/**
		 * Pointer to the space mouse driver instance.
		 */ 
        SpaceMouse* _spaceMouse;
};

}	// END NAMESPACE
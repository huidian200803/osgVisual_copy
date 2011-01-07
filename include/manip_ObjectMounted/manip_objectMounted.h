#pragma once 
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

#include <osgGA/CameraManipulator>

#include <osg/Quat>
#include <osg/observer_ptr>

#include <visual_object.h>


namespace osgVisual
{ 

/**
 * \brief This class is a free spacemouse manipulator. It uses the 3DConnexion SpaceNavigator, which interface is implemented in manip_spaceMouse.h
 * 
 * @author Torben Dannhauer
 * @date  Aug 2009
 */ 
class objectMountedManipulator : public osgGA::CameraManipulator
{
	#include <leakDetection.h>
    public:
		/**
		 * \brief Constructor
		 * 
		 */ 
        objectMountedManipulator();

		/**
		 * \brief This function returns the classname.
		 * 
		 * @return Classname of this manipulator.
		 */ 
        virtual const char* className() const { return "objectMountedManipulator"; }

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

		virtual void setAttachedObject( 
			osgVisual::visual_object* object_ ) {attachedObject = object_;};

		virtual osgVisual::visual_object* getAttachedObject() {return attachedObject.get();};


    protected:
		/**
		 * \brief Destructor. Because it is protected, no explicit delete myPointer by external caller is possible.
		 * This forces the class to be used with osg::ref pointers.
		 */ 
        virtual ~objectMountedManipulator();

		osg::Matrixd manipMatrix;
	

		osg::ref_ptr<osgVisual::visual_object> attachedObject;
};

}	// END NAMESPACE

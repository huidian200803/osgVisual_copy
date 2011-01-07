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

#include <osg/Node>
#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

#include <string>

namespace osgVisual {

class dataIO_executer : public osg::Node
{
	#include <leakDetection.h>
public:
	dataIO_executer() {id=DO_NOTHING;}
	~dataIO_executer() {}

	// Standart functions of the base-class, required for serialization
	//// Copy constructor using CopyOp to manage deep vs shallow copy.
	dataIO_executer(const dataIO_executer& ex, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) : osg::Node(ex, copyop) {} 
	//// osg::Objects functions
	osg::Object* cloneType() const { return new dataIO_executer (); }
	osg::Object* clone(const osg::CopyOp& copyop) const { return new dataIO_executer (*this,copyop); }
	bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const dataIO_executer *>(obj)!=NULL; }
	const char* className() const { return "dataIO_executer"; }
	const char* libraryName() const { return "osgVisual"; }

	typedef std::vector<double> parameterList;
	enum executerID {
		GET_HAT,
		GET_HOT,
		GET_LAT,
		GET_LON,
		IS_COLLISION, 
		IS_INSIDE_CLOUD,
		IS_WIND,
		IS_PRECIPITATION,
		DO_NOTHING
	};
	

private: 
	executerID id;
	parameterList doubleParameter;
	std::string stringParameter;

// Access functions for Serializer
public:
	void setexecuterID(executerID id_) {id=id_;}
	executerID getexecuterID() const {return id;}

	void setDoubleParameter(const parameterList& doubleParameter_) {doubleParameter=doubleParameter_;}
	const parameterList& getDoubleParameter() const {return doubleParameter;}

	void setStringParameter(const std::string& stringParameter_) {stringParameter=stringParameter_;}
	const std::string& getStringParameter() const {return stringParameter;}
};

} // END NAMESPACE
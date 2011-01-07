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

#include <osg/Object>
#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

#include <string>


namespace osgVisual {

class dataIO_slot : public osg::Object
{
	#include <leakDetection.h>
public:
	META_Object(osgVisual,dataIO_slot);
	dataIO_slot(const osgVisual::dataIO_slot& slot_, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY):
            Object(slot_,copyop),
			direction(slot_.direction),
			variableType(slot_.variableType),
			variableName(slot_.variableName),
			value(slot_.value),
			sValue(slot_.sValue){}

	dataIO_slot(){};
	~dataIO_slot(){};

	dataIO_slot & operator=(const dataIO_slot &rhs)
	{
		direction=rhs.direction;
		variableType=rhs.variableType;
		variableName=rhs.variableName;
		value=rhs.value;
		sValue=rhs.sValue;	

		return *this;
	}


	/**
	 * This enums defines the data flow direction of a slot.
	 */ 
	enum dataDirection {TO_OBJ, FROM_OBJ};

	/**
	 * This enum defines the two valid valuetypes.
	 */ 
	enum varType {DOUBLE, STRING};

	dataDirection direction;
	varType variableType;
	std::string variableName;
	double value;
	std::string sValue;


// Access functions for Serializer
	void setdataDirection(dataDirection direction_){direction=direction_;}
	dataDirection getdataDirection() const {return direction;}

	void setvarType(varType variableType_){variableType=variableType_;}
	varType getvarType() const {return variableType;}

	const std::string& getVariableName() const {return variableName;}
	void setVariableName(const std::string& variableName_){variableName=variableName_;}

	double getValue() const {return value;}
	void setValue(double value_){value=value_;}

	const std::string& getSValue() const {return sValue;}
	void setSValue(const std::string& sValue_){sValue=sValue_;}
};

}	// END NAMESPACE
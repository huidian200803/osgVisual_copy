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

#include <osg/Object>
#include <osg/Matrixd>
#include <osg/Vec3d>

#include <dataIO_executer.h>
#include <dataIO_slot.h>

#include <vector>

namespace osgVisual {

class dataIO_transportContainer : public osg::Object
{
	#include <leakDetection.h>
public:
	META_Object(osgVisual,dataIO_transportContainer);	// Required for serializer
	dataIO_transportContainer(const osgVisual::dataIO_transportContainer& tC_, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY):	// Required for serializer
			Object(tC_,copyop),
			frameID(tC_.frameID),
			viewMatrix(tC_.viewMatrix),
			executer(tC_.executer),
			ioSlots(tC_.ioSlots){}
	dataIO_transportContainer(){}
	virtual ~dataIO_transportContainer(){}


	typedef std::vector<osg::ref_ptr<dataIO_executer> > executerList;
	typedef std::vector<osg::ref_ptr<dataIO_slot> > slotList;

private:
	int frameID;
	osg::Matrixd viewMatrix;
	executerList executer;
	slotList ioSlots;

// Access functions for Serializer
public:
	int getFrameID() const {return frameID;}
	void setFrameID(int frameID_ ){frameID=frameID_;}

	void setViewMatrix(const osg::Matrixd& viewMatrix_){viewMatrix = viewMatrix_;}
	const osg::Matrixd& getViewMatrix() const {return viewMatrix;}

	void setExecuter(const executerList& executer_) {executer=executer_;}
	const executerList& getExecuter() const {return executer;}

	void setIOSlots(const slotList& ioSlots_) {ioSlots=ioSlots_;}
	const slotList& getIOSlots() const {return ioSlots;}


	// Usage functions
	void addExecuter( osg::ref_ptr<dataIO_executer> executerToAdd_ ) {executer.push_back(executerToAdd_);}
	void removeAllExecuter() { executer.clear();}

	void addSlot( osg::ref_ptr<dataIO_slot> slotToAdd_ ) {ioSlots.push_back(slotToAdd_);}
	void removeAllSlots() {ioSlots.clear();}

};

} // END NAMESPACE
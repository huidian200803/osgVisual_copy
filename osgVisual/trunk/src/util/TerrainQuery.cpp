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

#include <osg/CoordinateSystemNode>
#include <terrainQuery.h>

#include <osg/Notify>
#include <osgUtil/LineSegmentIntersector>

using namespace osgSim;

HeightAboveTerrain::HeightAboveTerrain(bool loadHighestLOD)
{
    _lowestHeight = -1000.0;
    
	if(loadHighestLOD)
		setDatabaseCacheReadCallback(new DatabaseCacheReadCallback);
}

void HeightAboveTerrain::clear()
{
    _HATList.clear();
}

unsigned int HeightAboveTerrain::addPoint(const osg::Vec3d& point)
{
    unsigned int index = _HATList.size();
    _HATList.push_back(HAT(point));
    return index;
}

void HeightAboveTerrain::computeIntersections(osg::Node* scene, osg::Node::NodeMask traversalMask)
{
    osg::CoordinateSystemNode* csn = dynamic_cast<osg::CoordinateSystemNode*>(scene);
    osg::EllipsoidModel* em = csn ? csn->getEllipsoidModel() : 0;

    osg::ref_ptr<osgUtil::IntersectorGroup> intersectorGroup = new osgUtil::IntersectorGroup();

    for(HATList::iterator itr = _HATList.begin();
        itr != _HATList.end();
        ++itr)
    {
        if (em)
        {
        
            osg::Vec3d start = itr->_point;
            osg::Vec3d upVector = em->computeLocalUpVector(start.x(), start.y(), start.z());

            double latitude, longitude, height;
            em->convertXYZToLatLongHeight(start.x(), start.y(), start.z(), latitude, longitude, height);
            osg::Vec3d end = start - upVector * (height - _lowestHeight);            
            
            itr->_hat = height;

            OSG_NOTICE<<"lat = "<<latitude<<" longitude = "<<longitude<<" height = "<<height<<std::endl;

            osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(start, end);
            intersectorGroup->addIntersector( intersector.get() );
        }
        else
        {
            osg::Vec3d start = itr->_point;
            osg::Vec3d upVector (0.0, 0.0, 1.0);
            
            double height = start.z();
            osg::Vec3d end = start - upVector * (height - _lowestHeight);            

            itr->_hat = height;

            osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector( start, end);
            intersectorGroup->addIntersector( intersector.get() );
        }
    }
    
    _intersectionVisitor.reset();
    _intersectionVisitor.setTraversalMask(traversalMask);
    _intersectionVisitor.setIntersector( intersectorGroup.get() );
    
    scene->accept(_intersectionVisitor);
    
    unsigned int index = 0;
    osgUtil::IntersectorGroup::Intersectors& intersectors = intersectorGroup->getIntersectors();
    for(osgUtil::IntersectorGroup::Intersectors::iterator intersector_itr = intersectors.begin();
        intersector_itr != intersectors.end();
        ++intersector_itr, ++index)
    {
        osgUtil::LineSegmentIntersector* lsi = dynamic_cast<osgUtil::LineSegmentIntersector*>(intersector_itr->get());
        if (lsi)
        {
            osgUtil::LineSegmentIntersector::Intersections& intersections = lsi->getIntersections();
            if (!intersections.empty())
            {
                const osgUtil::LineSegmentIntersector::Intersection& intersection = *intersections.begin();
                osg::Vec3d intersectionPoint = intersection.matrix.valid() ? intersection.localIntersectionPoint * (*intersection.matrix) :
                                               intersection.localIntersectionPoint;
                // HAT
				_HATList[index]._hat = (_HATList[index]._point - intersectionPoint).length();

				// HOT
				if (em)
				{
					double latitude, longitude, height;
					em->convertXYZToLatLongHeight(intersectionPoint.x(), intersectionPoint.y(), intersectionPoint.z(), latitude, longitude, height);
					_HATList[index]._hot = height;
				}
				else
				{
					_HATList[index]._hot = intersectionPoint.z();
				}

            }
        }
    }
    
}

double HeightAboveTerrain::computeHeightAboveTerrain(osg::Node* scene, const osg::Vec3d& point, bool loadHighestLOD, osg::Node::NodeMask traversalMask)
{
    HeightAboveTerrain hat(loadHighestLOD);
    unsigned int index = hat.addPoint(point);
    hat.computeIntersections(scene, traversalMask);
    return hat.getHeightAboveTerrain(index);
}

double HeightAboveTerrain::computeHeightOfTerrain(osg::Node* scene, const osg::Vec3d& point, bool loadHighestLOD, osg::Node::NodeMask traversalMask)
{
	HeightAboveTerrain hat(loadHighestLOD);
    unsigned int index = hat.addPoint(point);
    hat.computeIntersections(scene, traversalMask);
    return hat.getHeightOfTerrain(index);
}

void HeightAboveTerrain::setDatabaseCacheReadCallback(DatabaseCacheReadCallback* dcrc)
{
    _dcrc = dcrc;
    _intersectionVisitor.setReadCallback(dcrc);
}

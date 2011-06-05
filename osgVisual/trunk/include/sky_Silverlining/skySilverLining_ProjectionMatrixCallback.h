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
 * 
 * This file is based on the OSG example of the Silverlining SDK:
 * Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.
*/

#include <SilverLining.h>
#include <osg/Camera>
#include <osg/CullSettings>
#include <osgAnimation/Animation>
#include <skySilverLining_skyDrawable.h>
#include <visual_util.h>



namespace osgVisual
{

/**
 * \brief This class intercepts OSG's method for automatically computing the near and far clip planes, taking into
    account SilverLining's objects in the scene (sky box, clouds, and possibly an atmospheric limb.)
 * 
 * @todo : Create interface to control nearplane while tracking near plane.
 * 
 * @author Torben Dannhauer
 * @date  Sep 2009
 */ 
class skySilverLining_projectionMatrixCallback : public osg::CullSettings::ClampProjectionMatrixCallback 
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Constructor : Initializes the attributes with valid values.
	 * 
	 * @todo : why is CSN required.
	 * 
	 * @param atm : Pointer to the atmosphere object.
	 * @param cam : Pointer to the rendering camera.
	 * @param csn_ : Pointer to the coordinatesystemode.
	 */ 
	skySilverLining_projectionMatrixCallback(SilverLining::Atmosphere *atm, osg::Camera *cam, osg::CoordinateSystemNode *csn_) : osg::CullSettings::ClampProjectionMatrixCallback(),
        atmosphere(atm), camera(cam), csn(csn_), skyDrawable(0), minFar(90000), maxNear(2)
    {
		#include <leakDetection.h>

        stockCullVisitor = osgUtil::CullVisitor::create();
		//stockCullVisitor->setNearFarRatio( 0.00005 );

		samplerClipPlanes = new osgAnimation::Vec2LinearSampler;
		keys = samplerClipPlanes->getOrCreateKeyframeContainer();

		//											Flughöhe				near		far
		keys->push_back(osgAnimation::Vec2Keyframe(      0.0, osg::Vec2(     1.0,     80000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe(     50.0, osg::Vec2(     5.0,     90000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe(    500.0, osg::Vec2(     5.0,    100000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe(   1000.0, osg::Vec2(    10.0,    100000.0)));
	//keys->push_back(osgAnimation::Vec2Keyframe(  10000.0, osg::Vec2(   100.0,   1000000.0)));
	//keys->push_back(osgAnimation::Vec2Keyframe(1000000.0, osg::Vec2(100000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe(   10000.0, osg::Vec2(   50.0,   1000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe( 1000000.0, osg::Vec2(  100.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe( 2000000.0, osg::Vec2(  250.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe( 3000000.0, osg::Vec2(  500.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe( 4000000.0, osg::Vec2( 1000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe( 5000000.0, osg::Vec2(10000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe( 6000000.0, osg::Vec2(  10000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe( 7000000.0, osg::Vec2( 100000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe( 8000000.0, osg::Vec2(1000000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe( 9000000.0, osg::Vec2(1250000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe(10000000.0, osg::Vec2(2500000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe(11000000.0, osg::Vec2(5000000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe(12000000.0, osg::Vec2(7500000.0, 200000000.0)));
		keys->push_back(osgAnimation::Vec2Keyframe(50000000.0, osg::Vec2(15000000.0,2000000000.0)));

    }

	/**
	 * \brief This functions sets the pointer to the skyDrawable. This pointer is requiered to adjust the skybox size.
	 * 
	 * @param sd : Pointer to the skyDrawable to store.
	 */ 
	void setSkyDrawable(skySilverLining_skyDrawable *sd) {skyDrawable = sd;}

protected:
	/**
	 * \brief Implementation for float values to clamp the projection matrix.
	 * 
	 * @param projection : Projection Matrix.
	 * @param znear : Near value of the projection matrix.
	 * @param zfar : Far value of the projection matrix.
	 * @return : True if successful
	 */ 
    bool clampProjectionMatrixImplementation(osg::Matrixf& projection, double& znear, double& zfar) const
    {
   	   computeNearFar( znear, zfar );
	   //OSG_NOTIFY( osg::ALWAYS ) << "znear: " << znear << ", zfar: " << zfar << std::endl;

	   if (skyDrawable)
	   {
	       skyDrawable->setSkyboxSize(((zfar - znear) * 0.5 + znear) * 2.0);
	   }
		
	   double fovy, ar, zn, zf;
       camera->getProjectionMatrixAsPerspective(fovy, ar, zn, zf);
	   camera->setProjectionMatrixAsPerspective(fovy, ar, znear, zfar);

		return true;
    }

	/**
	 * \brief Implementation for double values to clamp the projection matrix.
	 * 
	 * @param projection : Projection Matrix.
	 * @param znear : Near value of the projection matrix.
	 * @param zfar : Far value of the projection matrix.
	 * @return : True if successful
	 */ 
    bool clampProjectionMatrixImplementation(osg::Matrixd& projection, double& znear, double& zfar) const
    {
	   computeNearFar( znear, zfar );
	   //OSG_NOTIFY( osg::ALWAYS ) << "znear: " << znear << ", zfar: " << zfar << std::endl;

	   if (skyDrawable)
	   {
	       skyDrawable->setSkyboxSize(((zfar - znear) * 0.5 + znear) * 2.0);
	   }
		
	   double fovy, ar, zn, zf;
       camera->getProjectionMatrixAsPerspective(fovy, ar, zn, zf);
	   //OSG_NOTIFY( osg::ALWAYS ) << "znear: " << znear << ", zfar: " << zfar << ", zn: " << zn << std::endl; 
	   camera->setProjectionMatrixAsPerspective(fovy, ar, znear, zfar);

		return true;
    }

	/**
	 * \brief This function computes the near and far value against the cloud objects of silverlining.
	 * 
	 * @param znear : Referenced to store the calculated near value into.
	 * @param zfar : Referenced to store the calculated far value into.
	 */ 
    void computeNearFarClouds(double &znear, double &zfar) const
    {
		const double fudge = 5000.0;
        double minDist = 1E10; double maxDist = -1E10;

		// Retrieve all atmosphere objects.
 		SL_VECTOR(SilverLining::ObjectHandle) &objects = atmosphere->GetObjects();
		int size = objects.size();
		
		osg::Vec3d eye, dirVector, upVector;
		camera->getViewMatrixAsLookAt(eye,dirVector,upVector); // Get XYZ from camera
		//osg::Vec3d camPos = camera->getViewMatrix().getTrans();
		osg::Vec3d camPos = eye;
        osg::Vec3d up = camPos;
        up.normalize();
        osg::Vec3d down = up * -1.0;

		double dToOrigin = camPos.length();

		// Calculations if atmopshere is rendered from space
		bool hasLimb = atmosphere->GetConfigOptionBoolean("enable-atmosphere-from-space");
		OSG_ALWAYS << "has limb " << hasLimb << std::endl;
		if (hasLimb)
		{
			// Compute distance to edge of atmospheric limb
			double earthRadius = atmosphere->GetConfigOptionDouble("earth-radius-meters");
			double atmosphereHeight = earthRadius + atmosphere->GetConfigOptionDouble("atmosphere-height");
			double atmosphereThickness = atmosphere->GetConfigOptionDouble("atmosphere-scale-height-meters") + earthRadius;
			if (dToOrigin > atmosphereThickness) // Bail if limb not rendered
			{
				double dToLimbEdge = sqrt(dToOrigin * dToOrigin + atmosphereHeight * atmosphereHeight);
				maxDist = dToLimbEdge;
				double dToLimbBase = sqrt(dToOrigin * dToOrigin + earthRadius * earthRadius);
				minDist = dToLimbBase;
			}
		}

		// Walk through all objects and check if they influence the near plane or far plane
		SL_VECTOR(SilverLining::ObjectHandle)::iterator it;
        for (it = objects.begin(); it != objects.end(); it++)
        {
            float ox, oy, oz;
            atmosphere->GetObjectPosition(*it, ox, oy, oz);
            osg::Vec3d oPos(ox, oy, oz);
            double oHeight = oPos.length();
            osg::Vec3d testVec = camPos;
			// Compare distance to origin of the camera and the object. wenn objecte nah aneinander: 
			//vertikal shiften, um eine erkennbare Distanz errechnen zu können.
            if (dToOrigin > oHeight)
            {
                testVec = camPos + down * 100.0; // 100 for floating point precision problems
            }
            else
            {
                testVec = camPos + up * 100.0;
            }
            
            const double farFarAway = 500000;

            double dist = atmosphere->GetObjectDistance(*it, testVec.x(), testVec.y(), testVec.z(),
				(float)camPos.x(), (float)camPos.y(), (float)camPos.z());


            if (dist < farFarAway) // an intersection was found
            {
                if (dist < minDist) minDist = dist;
                if (dist > maxDist) maxDist = dist;
            }
        }	// For (all objects) END

        double fovy, ar, zn, zf;
        camera->getProjectionMatrixAsPerspective(fovy, ar, zn, zf);
        double halfFovx = (fovy * ar) * 0.5;
        minDist *= cos(halfFovx * (3.14159265 / 180.0));

        znear = minDist - fudge;
        zfar = maxDist + fudge;

		//clamp the near plane if low above ground.
	    double x,y,z,hat;
	    util::getXYZofCamera( camera, x, y, z );
		if( util::queryHeightAboveTerrainInWorld( hat, csn, x, y, z ) )
		{
			if (hat < 80 )	
			{
				znear = maxNear;
			}
		}
	    //OSG_NOTIFY( osg::ALWAYS ) << "Near: " << znear << ", Far: " << zfar << std::endl;
    }

	void computeNearFar( double& near_, double& far_ ) const
	{
		double x,y,z,hat;
		near_ = 2;
		far_ = 125000;
	    util::getXYZofCamera( camera, x, y, z );
		if( util::queryHeightAboveTerrainInWorld( hat, csn, x, y, z ) )
		{
			osg::Vec2 planesNearFar;
			samplerClipPlanes->getValueAt(hat, planesNearFar);

			near_ = planesNearFar.x();
			far_  = planesNearFar.y();

			//OSG_NOTIFY( osg::ALWAYS ) << "hat: " << hat << ", near: " << near_ << ", far: " << far_ << std::endl;
		} // IF HAT END
		else 
			OSG_NOTIFY( osg::FATAL ) << "ERROR: Unable to Compute NearFar!" << std::endl;

		if (near_ < 0.5 ) near_ = 0.5;
	}

    
	/**
	 * Pointer to the Cullvisitor which applies the projection matrix.
	 */ 
	osg::ref_ptr<osgUtil::CullVisitor> stockCullVisitor;

	/**
	 * Pointer to silverlinings atmosphere instance.
	 */ 
    SilverLining::Atmosphere *atmosphere;

	/**
	 * Pointer to the camera on which the callback is installed
	 */ 
    osg::Camera *camera;

	/**
	 * Pointer to the skyDrawable
	 */ 
	skySilverLining_skyDrawable *skyDrawable;

	/**
	 * ?
	 * @todo: wofür ist diese variable nötig?
	 */ 
	osg::CoordinateSystemNode *csn;


	osg::ref_ptr<osgAnimation::Vec2LinearSampler> samplerClipPlanes;
	osg::ref_ptr<osgAnimation::Vec2KeyframeContainer> keys;

	/**
	 * Minimal far distance. The far plane is never nearer than this value.
	 */ 
	double minFar;

	/**
	 * Maximal near distance. The near plane is never farther than this value. 
	 * It is used to clamp the near plane on a fix near value if the camera is low above ground
	 */ 
	double maxNear;
};

}	// END NAMESPACE

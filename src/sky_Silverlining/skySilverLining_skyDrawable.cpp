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

#include <skySilverLining_skyDrawable.h>
#include <SilverLining.h>
#include <skySilverLining_AtmosphereReference.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>



using namespace SilverLining;
using namespace osgVisual;

skySilverLining_skyDrawable::skySilverLining_skyDrawable()
        : osg::Drawable()
		, _view(0)
		, _skyboxSize(0)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
	pendingNewCloudLayers.clear();
	newCloudLayersToAdd = false;
}

skySilverLining_skyDrawable::skySilverLining_skyDrawable(osgViewer::Viewer* view,  osg::CoordinateSystemNode* csn_)
        : osg::Drawable()
        , _view(view)
		, _skyboxSize(0)
{
    setDataVariance(osg::Object::DYNAMIC);
    setUseVertexBufferObjects(false);
    setUseDisplayList(false);
	pendingNewCloudLayers.clear();
	newCloudLayersToAdd = false;
	sceneRoot = csn_;
}

void skySilverLining_skyDrawable::setLighting(SilverLining::Atmosphere *atmosphere) const
{
    osg::Light *light = _view->getLight();
    osg::Vec4 ambient, diffuse;
    osg::Vec3 direction;

    if (atmosphere && light)
    {
        float ra, ga, ba, rd, gd, bd, x, y, z;
        atmosphere->GetAmbientColor(&ra, &ga, &ba);
        atmosphere->GetSunOrMoonColor(&rd, &gd, &bd);
        atmosphere->GetSunOrMoonPosition(&x, &y, &z);

        direction = osg::Vec3(x, y, z);
        ambient = osg::Vec4(ra, ga, ba, 1.0);
        diffuse = osg::Vec4(rd, gd, bd, 1.0);

        // xform the light direction into camera coordinates
        osg::Quat view = _view->getCamera()->getViewMatrix().getRotate();
        //direction = view * direction;
        direction.normalize();

        light->setAmbient(ambient);
        light->setDiffuse(diffuse);
        light->setSpecular(osg::Vec4(0,0,0,1));
        light->setPosition(osg::Vec4(direction.x(), direction.y(), direction.z(), 0));
    }
}

void skySilverLining_skyDrawable::setSceneFog(SilverLining::Atmosphere *atmosphere) const
{    
	osg::ref_ptr<osg::Fog> fog = new osg::Fog;
    fog->setMode(osg::Fog::EXP);
	fog->setUseRadialFog( true );

	//_objectsNode  = erde oder object, das mit nebel bedacht werden soll	
	sceneRoot->getOrCreateStateSet()->setAttributeAndModes(fog.get());

	//float hazeR, hazeG, hazeB;
	//double hazeDepth, hazeDensity;
	//atmosphere->GetHaze(hazeR, hazeG, hazeB, hazeDepth, hazeDensity);

    //hazeDensity = 1.0 / 40000;
	//hazeDensity = 0;
	float hazeDensity = 1.0 / atmosphere->GetConditions()->GetVisibility();

    // Decrease fog density with altitude, to avoid fog effects through the vacuum of space.
    static const double H = 8435.0; // Pressure scale height of Earth's atmosphere
    double isothermalEffect = exp(-(atmosphere->GetConditions()->GetLocation().GetAltitude() / H));     
    if (isothermalEffect <= 0) isothermalEffect = 1E-9;
    if (isothermalEffect > 1.0) isothermalEffect = 1.0;
    hazeDensity *= isothermalEffect;

    bool silverLiningHandledTheFog = false;

    if (atmosphere->GetFogEnabled())
    {
            float density, r, g, b;
            // Note, the fog color returned is already lit
            atmosphere->GetFogSettings(&density, &r, &g, &b);

            if (density > hazeDensity)
            {
				fog->setDensity(density);
				fog->setColor(osg::Vec4(r, g, b, 1.0));

				silverLiningHandledTheFog = true;
            }
    }
    
    if (!silverLiningHandledTheFog)
    {
		float r, g, b;
		atmosphere->GetHorizonColor(0, &r, &g, &b);// New version of this call: since SL_1.94 /** \todo transmit the yaw value of the center channel to all slaves for consistent fog color. */

		fog->setDensity(hazeDensity);
		fog->setColor(osg::Vec4(r, g, b, 1.0));
    }

}

void skySilverLining_skyDrawable::initializeSilverLining(skySilverLining_atmosphereReference *ar)
{
    if (ar && !ar->atmosphereInitialized)
    {
        ar->atmosphereInitialized = true; // only try once.
		SilverLining::Atmosphere *atmosphere = ar->atmosphere;

		if (atmosphere)
        {

			srand(1234); // constant random seed to ensure consistent clouds across windows

            // Update the path below to where you installed SilverLining's resources folder.
            //int ret = atmosphere->Initialize(SilverLining::Atmosphere::OPENGL, "C:\\Program Files\\SilverLining SDK\\resources\\", true, 0);
			int ret = atmosphere->Initialize(SilverLining::Atmosphere::OPENGL, "../resources/sky_silverlining/", true, 0);
            if (ret != SilverLining::Atmosphere::E_NOERROR)
            {
                printf("SilverLining failed to initialize; error code %d.\n", ret);
                printf("Check that the path to the SilverLining installation directory is set properly ");
                printf("in skySilverLining_skyDrawable.cpp (in SkyDrawable::initializeSilverLining)\n");
                exit(0);
            }

            // Let SilverLining know which way is up. OSG usually has Z going up.
			atmosphere->SetUpVector(0.662994, 0.136169, 0.736136);
			atmosphere->SetRightVector(-0.201185, 0.979553, 0.0);

            // Set our location (change this to your own latitude and longitude)
            SilverLining::Location loc;
            loc.SetAltitude(0);
            loc.SetLatitude(47);
            loc.SetLongitude(11);
            atmosphere->GetConditions()->SetLocation(loc);

            // Set the system time in PST
            SilverLining::LocalTime t;
            t.SetFromSystemTime();
			t.SetTimeZone(CET);
            atmosphere->GetConditions()->SetTime(t);

			// Setting Up Visibility
			atmosphere->GetConditions()->SetVisibility(40000);

			// Setting Up Haze
			atmosphere->GetConditions()->SetTurbidity(2.2);

			// Setting up time passage
			atmosphere->GetConditions()->EnableTimePassage( true, 5000 );
			
		}	// if atmosphere ENDE

    }	// If atmosphere not initialized ENDE
}

void skySilverLining_skyDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
	skySilverLining_atmosphereReference *ar = dynamic_cast<skySilverLining_atmosphereReference *>(renderInfo.getCurrentCamera()->getUserData());
	SilverLining::Atmosphere *atmosphere = 0;

	if (ar) atmosphere = ar->atmosphere;

	renderInfo.getState()->disableAllVertexArrays();

    if (atmosphere)
    {
        initializeSilverLining(ar);
		const_cast<skySilverLining_skyDrawable*>(this)->seedAndAddCloudLayers(atmosphere);

		atmosphere->DrawSky(true, true, _skyboxSize);
        setLighting(atmosphere);
		setSceneFog(atmosphere);
    }

	renderInfo.getState()->dirtyAllVertexArrays();
}

void skySilverLining_skyDrawable::addCloudLayerOrder(cloudlayerOrder newCL)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> sLock(cloudLayersToAddMutex); 
	pendingNewCloudLayers.push_back( newCL );
	newCloudLayersToAdd = true;
}

void skySilverLining_skyDrawable::seedAndAddCloudLayers(SilverLining::Atmosphere *atmosphere)
{
	// Only try to add if anything to do..
	if ( newCloudLayersToAdd )
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> sLock(cloudLayersToAddMutex); 

		for ( unsigned int i=0; i<pendingNewCloudLayers.size(); i++)	// Configure & Seed cloudLayer
		{
			// Calculation earth radius on specified lat lon position
			double radius;
			cloudlayerOrder newCL = pendingNewCloudLayers[i];
			if ( !util::calculateEarthRadiusAtWGS84Coordinate(newCL.lat, newCL.lon, sceneRoot, radius) )
			{
				OSG_ALWAYS << "ERROR: skySilverLining_skyDrawable::seedAndAddCloudLayers() - Unable to deterine earth radius for lat=" <<newCL.lat<< " lon="<<newCL.lat<<std::endl;
				return;
			}

			// generate Cloud Layer
			SilverLining::CloudLayer *cloudLayer_;
			cloudLayer_ = SilverLining::CloudLayerFactory::Create(newCL.cloudtype);
			cloudLayer_->SetBaseAltitude( newCL.baseHeight + radius);
			cloudLayer_->SetThickness(newCL.thickness);
			cloudLayer_->SetBaseLength(newCL.baseLength);
			cloudLayer_->SetBaseWidth(newCL.baseWidth);
			cloudLayer_->SetDensity(newCL.density);
			cloudLayer_->SetLayerPosition( 0.0, 0.0 );
			cloudLayer_->GenerateShadowMaps(false);
			//cloudLayer_->SetIsInfinite( true );


			//Save cloudlayer information into SLOT
			newCL.assocCloudLayerSlot->used = true;
			newCL.assocCloudLayerSlot->enabled = true;
			newCL.assocCloudLayerSlot->cloudLayerPointer = cloudLayer_;
			switch(newCL.cloudtype)
			{
			case 0: newCL.assocCloudLayerSlot->typeName = "CIRROCUMULUS";				// High planar cloud puffs		
				break;
			case 1: newCL.assocCloudLayerSlot->typeName = "CIRRUS_FIBRATUS";			// High, thicker and fibrous clouds that signal changing weather
				break;
			case 2: newCL.assocCloudLayerSlot->typeName = "STRATUS";					// Low clouds represented as a slab
				break;
			case 3: newCL.assocCloudLayerSlot->typeName = "CUMULUS_MEDIOCRIS";          // Low, puffy clouds on fair days
				break;
			case 4: newCL.assocCloudLayerSlot->typeName = "CUMULUS_CONGESTUS";          // Large cumulus clouds that could turn into a thunderhead
				break;
			case 5: newCL.assocCloudLayerSlot->typeName = "CUMULONIMBUS_CAPPILATUS";    // Big storm clouds.
				break;
			case 6: newCL.assocCloudLayerSlot->typeName = "STRATOCUMULUS";				// Low, dense, puffy clouds with some sun breaks between them.
				break;
			default: OSG_NOTIFY( osg::FATAL ) << "skySilverLining_skyDrawable::seedAndAddCloudLayers() - Invalid cloud type." << std::cout;
				 break;
			};

			// Seed Cloudlayer
			cloudLayer_->SeedClouds(*atmosphere);
	
			// Add cloudLayer to atmosphere
			newCL.assocCloudLayerSlot->cloudLayerHandle = atmosphere->GetConditions()->AddCloudLayer( cloudLayer_ );
		}

		//Clear the ToDo List
		pendingNewCloudLayers.clear();

		// Note nothing to do.
		newCloudLayersToAdd = false;
	}
}

void skySilverLining_skyDrawable::shutdown()
{
	sceneRoot = NULL;
}

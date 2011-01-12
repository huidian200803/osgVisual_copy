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

#include <visual_skySilverLining.h>

using namespace osgVisual;

visual_skySilverLining::visual_skySilverLining(osgViewer::Viewer* viewer_, std::string configFileName, bool& disabled)
{
	OSG_NOTIFY( osg::ALWAYS ) << "Initialize visual_skySilverlining..." << std::endl;

	atmosphereInitialized = false;
	postInitialized = false;
	atmosphere = NULL;
	viewer = viewer_;
	lat = 0;
	lon = 0;

	for( int i=0; i<MAX_CLOUDLAYER_SLOTS; i++ )
	{
		cloudLayerSlots.push_back( cloudLayerSlot() );
		cloudLayerSlots.back().slot_id = i;
	}

	// Check if the module is en- or diabled by XML configuration.
	xmlDoc* tmpDoc;
	xmlNode* config = util::getModuleXMLConfig( configFileName, "sky_silverlining", tmpDoc, disabled );
	if( disabled)
		OSG_NOTIFY( osg::ALWAYS ) << "..disabled by XML configuration file." << std::endl;
 
	if(config)
		xmlFreeDoc(tmpDoc); xmlCleanupParser();

}

visual_skySilverLining::~visual_skySilverLining(void)
{
	this->removeUpdateCallback( updateCallback );
	if ( atmosphere != NULL )
		delete atmosphere;
}

void visual_skySilverLining::skyUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	//std::cout << "Sky silverlining update callback" << std::endl;
	// Check if atmosphere is initialized.
	if (!sky->isInitialized())
		return;

	// on first time: perform Post-Init.
	sky->postInit();

	// Update sky
	double lat, lon, height;
	util::getWGS84ofCamera( sceneCamera, csn, lat, lon, height );
	//std::cout << "lat: " << osg::RadiansToDegrees(lat) << ", lon: " << osg::RadiansToDegrees(lon) << ", height: " << height << std::endl;
	sky->setLocation( lat, lon, height );
}

void visual_skySilverLining::setDateTime( int year_, int month_, int day_, int hour_, int minute_, int second_, bool daylightSaving_, double timezoneOffset_ )
{
	// Check if atmosphere is initialized.
	if (!isInitialized())
		return;

	SilverLining::LocalTime t = atmosphere->GetConditions()->GetTime();
	t.SetYear( year_ );
	t.SetMonth( month_ );
	t.SetDay( day_ );
	t.SetHour( hour_ );
	t.SetMinutes( minute_ );
	t.SetSeconds( second_ );
	t.SetObservingDaylightSavingsTime( daylightSaving_ );
	t.SetTimeZone( timezoneOffset_ );
}

void visual_skySilverLining::setTime( int hour_, int minute_, int second_ )
{
	// Check if atmosphere is initialized.
	if (!isInitialized())
		return;
	
	SilverLining::LocalTime t = atmosphere->GetConditions()->GetTime();
	t.SetHour( hour_ );
	t.SetMinutes( minute_ );
	t.SetSeconds( second_ );
	atmosphere->GetConditions()->SetTime( t );
}

void visual_skySilverLining::setDate( int year_, int month_, int day_ )
{
	// Check if atmosphere is initialized.
	if (!isInitialized())
		return;

	SilverLining::LocalTime t = atmosphere->GetConditions()->GetTime();
	t.SetYear( year_ );
	t.SetMonth( month_ );
	t.SetDay( day_ );
}

void visual_skySilverLining::setDateByEpoch( int secondsSince1970_ )
{
	// Check if atmosphere is initialized.
	if (!isInitialized())
		return;
	
	SilverLining::LocalTime t = atmosphere->GetConditions()->GetTime();
	t.SetFromEpochSeconds( secondsSince1970_ );
}

void visual_skySilverLining::setLocation(double lat_, double lon_, double alt_)
{
	// Check if atmosphere is initialized.
	if (!isInitialized())
		return;

	lat = lat_;
	lon = lon_;
	height = alt_;
	
	SilverLining::Location loc = atmosphere->GetConditions()->GetLocation();
	loc.SetAltitude(alt_);
	loc.SetLatitude(lat_);
	loc.SetLongitude(lon_);		
	atmosphere->GetConditions()->SetLocation( loc );

	updateUpVector();
}


bool visual_skySilverLining::isInitialized()
{
	// Check if atmosphere is initialized. If not: make a deep lookup. If initialized, perform. Otherwise return
	if (!atmosphereInitialized)
	{
		skySilverLining_atmosphereReference *ar = dynamic_cast<skySilverLining_atmosphereReference *>(viewer->getCamera()->getUserData());
		if (ar != NULL )
		{
			if (ar->atmosphereInitialized)
				atmosphereInitialized = true;
		}
	}
	return(atmosphereInitialized);
}

void visual_skySilverLining::init(osg::Group *distortedRoot, osg::CoordinateSystemNode *sceneGraphRoot)
{
	sceneRoot = sceneGraphRoot;

	// Use projection matrix callback oder fixed Cullsettings?
	bool useProjMatrixCallback = true;

	// add Sky to SceneGraphRoot
	sceneGraphRoot->addChild( this );

	// Deactivate culling for the sky node (required by the silverlining sky framework)
	this->setCullingActive(false);

	// Instantiate an Atmosphere and associate it with this camera. If you have multiple cameras
	// in multiple contexts, be sure to instantiate seperate Atmosphere objects for each.
    // ***IMPORTANT!**** Check that the path to the resources folder for SilverLining in SkyDrawable.cpp
    // SkyDrawable::initializeSilverLining matches with where you installed SilverLining.
	atmosphere = new SilverLining::Atmosphere(SILVERLINING_LICENSEE, SILVERLINING_LICENSE);

    // Add the sky (calls Atmosphere::BeginFrame and handles initialization once you're in
    // the rendering thread)
	skyDrawable = new skySilverLining_skyDrawable(viewer, sceneRoot);

	if(distortedRoot)	// if distortion used:
	{
		int rootKids = distortedRoot->getNumChildren();
		for (int i = 0; i < rootKids; i++)
		{
			osg::Node *n = distortedRoot->getChild(i);
			osg::Camera *cam = dynamic_cast<osg::Camera*>(n);
			if (cam && cam->getRenderOrder() == osg::Camera::PRE_RENDER)
				sceneCamera = cam;
		}
	}
	else	// if no distortion used:
		sceneCamera = viewer->getCamera();

	osg::Camera *mainCamera = viewer->getCamera();
	if (!useProjMatrixCallback)
	{
		mainCamera->setClearMask(0);
		mainCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		double fovy, aspect, zNear, zFar;
		mainCamera->getProjectionMatrixAsPerspective(fovy, aspect, zNear, zFar);
		mainCamera->setProjectionMatrixAsPerspective(fovy, aspect, 2, 125000);
	}
	else
	{
		cb = new skySilverLining_projectionMatrixCallback( atmosphere, viewer->getCamera(), sceneRoot);
		sceneCamera->setClampProjectionMatrixCallback(cb);
		cb->setSkyDrawable(skyDrawable);
	}

	// append atmosphere pointer to the cameras.
	sceneCamera->setClearMask(0);
	osg::ref_ptr<skySilverLining_atmosphereReference> ar = new skySilverLining_atmosphereReference;
	ar->atmosphere = atmosphere;
	sceneCamera->setUserData(ar);
	mainCamera->setUserData(ar);

	// Create and install updateCallback (for position etc.)
	updateCallback = new skyUpdateCallback( sceneGraphRoot, sceneCamera, this );
	this->setUpdateCallback( updateCallback );

    // Use a RenderBin to enforce that the sky gets drawn first, then the scene, then the clouds
	skyDrawable->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");

    // Add the models
    sceneGraphRoot->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");

    // Add the clouds (note, you need this even if you don't want clouds - it calls
    // Atmosphere::EndFrame() )
	cloudsDrawable = new skySilverLining_cloudsDrawable(viewer);
	cloudsDrawable->getOrCreateStateSet()->setRenderBinDetails(99, "RenderBin");

	// Add drawable to this geode to get rendered
	this->addDrawable(skyDrawable);
	this->addDrawable(cloudsDrawable);

	//SilverLining::Atmosphere::EnableHDR( true );
}

void visual_skySilverLining::postInit()
{
	// Only allow one execution
	//if(postInitialized)
	//	return;
	//else postInitialized = true;

	// Execute Updatecallback once before adding Clouds.
	//updateCallback->operator ()(this, NULL);

	//atmosphere->GetConditions()->SetFog( 0.8, 1, 1, 1);	// use this for simulation real fog.

	//Todo: secure memory-manager of timer*. oder remove paragraph
	//MyMillisecondTimer *timer = new MyMillisecondTimer();
 //   atmosphere->GetConditions()->SetMillisecondTimer(timer);
	//atmosphere->GetConditions()->EnableTimePassage(true, -1);
}

void visual_skySilverLining::updateUpVector()
{
	double x,y,z;
    util::getXYZofCamera(sceneCamera, x, y, z);

    osg::Vec3d up(x, y, z);

    up.normalize();
    osg::Vec3d north(0, 0, 1);	// Z is north
    osg::Vec3d east = north ^ up;	// Cross product
    east.normalize();

    atmosphere->SetUpVector(up.x(), up.y(), up.z());
    atmosphere->SetRightVector(east.x(), east.y(), east.z());

    osg::Matrixd proj = sceneCamera->getProjectionMatrix();
    double dProj[16];

    int i = 0;
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            dProj[i] = proj(row, col);
            i++;
        }
    }
    //atmosphere->SetProjectionMatrix(dProj);
}

void visual_skySilverLining::shutdown()
{
	if (isInitialized())
	{
		// Remove this Node from scenegraph
		sceneRoot->removeChild( this );
		
		// Remove updatecallback
		this->removeUpdateCallback( updateCallback );
		updateCallback = NULL;

		// delete drawables
		skyDrawable->shutdown();
		this->removeDrawable(skyDrawable);
		this->removeDrawable(cloudsDrawable);
	}
}

void visual_skySilverLining::setVisibility(double visibility_)
{
	if (isInitialized())
	{
		atmosphere->GetConditions()->SetVisibility( visibility_ );
	}

}

double visual_skySilverLining::getVisibility()
{
	if (isInitialized())
	{
		return atmosphere->GetConditions()->GetVisibility();
	}
	else
		return -1;
}

void visual_skySilverLining::setTurbidity(double turbidity_)
{
	if (isInitialized())
	{
		atmosphere->GetConditions()->SetTurbidity( turbidity_ );
	}
}

double visual_skySilverLining::getTurbidity()
{
	if (isInitialized())
	{
		return atmosphere->GetConditions()->GetTurbidity();
	}
	else
		return -1;
}

void visual_skySilverLining::clearAllWindVolumes()
{
	if (isInitialized())
	{
		atmosphere->GetConditions()->ClearWindVolumes();
	}
}

bool visual_skySilverLining::insideWind(double height_, double& bottom_, double& top_, double& speed_, double& direction_)
{
	if (isInitialized())
	{
		// Calculation earth radius on the wind positionl approximated through the now used position.
		double radius;
		if ( util::calculateEarthRadiusAtWGS84Coordinate(lat, lon, sceneRoot, radius) )
		{
			// go through all wind volumes an check them for inside()
			//std::map<int, SilverLining::WindVolume> windvolumes = atmosphere->GetConditions()->GetWindVolumes();
			SL_MAP(int, SilverLining::WindVolume) windvolumes = atmosphere->GetConditions()->GetWindVolumes();
			for(unsigned int i=0; i<windvolumes.size(); i++)
			{
				if( windvolumes[i].Inside(radius + height_) )
				{
					// save wind data
					bottom_ = windvolumes[i].GetMinAltitude();
					top_ = windvolumes[i].GetMaxAltitude() - radius;
					speed_ = windvolumes[i].GetWindSpeed() - radius;
					direction_ = windvolumes[i].GetDirection();
					// return that wind was found
					return true;
				}
			}	// For END
		}	// If valid radius END
	}	// If initialized() END
	bottom_ = -1;
	top_ = -1;
	speed_ = -1;
	direction_ = -1;
	return false;
}

void visual_skySilverLining::addWindVolume(double bottom_, double top_, double speed_, int direction_)
{
	if (isInitialized())		
	{
		// Calculation earth radius on current lat lon position
		double radius;
		if ( util::calculateEarthRadiusAtWGS84Coordinate(lat, lon, sceneRoot, radius) )
		{
			// correct wind value:
			if ( direction_ < 180 )
				direction_ += 180;
			else direction_ -= 180;

			// Setting up Wind
			SilverLining::WindVolume wv;
			wv.SetDirection( direction_ );
			wv.SetMinAltitude( radius + bottom_ );
			wv.SetMaxAltitude( radius + top_ );
			wv.SetWindSpeed( speed_ );
			atmosphere->GetConditions()->SetWind(wv);
		}
	}
}

void visual_skySilverLining::setLightPollution(double lightPollution_)
{
	if (isInitialized())
	{
		return atmosphere->GetConditions()->SetLightPollution( lightPollution_ );
	}
}

double visual_skySilverLining::getLightPollution()
{
	if (isInitialized())
	{
		return atmosphere->GetConditions()->GetLightPollution();
	}
	else
		return -1;

}

void visual_skySilverLining::addCloudLayer(int slot_, double baseLength_, double baseWidth_, double thickness_, double baseHeight_, double density_, CloudTypes cloudtype_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		// create cloudlayer order and pass to skyDrawable to instantiate.
		cloudlayerOrder newCL;
		newCL.slot = slot_;
		newCL.lat = lat;
		newCL.lon = lon;
		newCL.baseLength = baseLength_;
		newCL.baseWidth = baseWidth_;
		newCL.thickness = thickness_;
		newCL.baseHeight = baseHeight_;
		newCL.density = density_;
		newCL.cloudtype = cloudtype_;
		newCL.assocCloudLayerSlot = &cloudLayerSlots[slot_];
		
		skyDrawable->addCloudLayerOrder( newCL );

	}	 // If isInitialized() END
}

void visual_skySilverLining::removeCloudLayer( int slot_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		atmosphere->GetConditions()->RemoveCloudLayer( cloudLayerSlots[slot_].cloudLayerHandle );
		cloudLayerSlots[slot_].used = false;
		cloudLayerSlots[slot_].cloudLayerHandle = -1;
		cloudLayerSlots[slot_].cloudLayerPointer = NULL;
		cloudLayerSlots[slot_].enabled = false;
	}
}

void visual_skySilverLining::clearAllSlots()
{
	if (isInitialized())
	{
		atmosphere->GetConditions()->RemoveAllCloudLayers();
		for( int i=0; i<MAX_CLOUDLAYER_SLOTS; i++ )
		{
			cloudLayerSlots[i].used = false;
			cloudLayerSlots[i].cloudLayerHandle = -1;
			cloudLayerSlots[i].cloudLayerPointer = NULL;
			cloudLayerSlots[i].enabled = false;
		}
	}
}

SilverLining::CloudLayer* visual_skySilverLining::getCloudLayer( int slot_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		if ( cloudLayerSlots[slot_].used )
			return cloudLayerSlots[slot_].cloudLayerPointer; 
	}

	return NULL;
}

void visual_skySilverLining::setEnabled(int slot_, bool enabled_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		cloudLayerSlots[slot_].enabled = enabled_;
	}
}

bool visual_skySilverLining::isEnabled( int slot_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		return cloudLayerSlots[slot_].enabled;
	}
	return false;
}

void visual_skySilverLining::fadeVisibility( int slot_, int fadetimeMS_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		if (cloudLayerSlots[slot_].enabled)
		{
			cloudLayerSlots[slot_].enabled = false;
			cloudLayerSlots[slot_].cloudLayerPointer->SetEnabled( false, fadetimeMS_ );
		}
		else
		{
			cloudLayerSlots[slot_].enabled = true;
			cloudLayerSlots[slot_].cloudLayerPointer->SetEnabled( true, fadetimeMS_ );
		}
	}
}

std::string visual_skySilverLining::getCloudLayerTypeName( int slot_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		return cloudLayerSlots[slot_].typeName;
	}
	return "";
}

void visual_skySilverLining::clearGlobalPrecipitation()
{
	if (isInitialized())
	{
		atmosphere->GetConditions()->SetPrecipitation( SilverLining::CloudLayer::NONE, 0.0 );
	}
}

void visual_skySilverLining::setGlobalPrecipitation( double rate_mmPerHour_rain_, double rate_mmPerHour_drySnow_, double rate_mmPerHour_wetSnow_, double rate_mmPerHour_sleet_ )
{
	if ( isInitialized() )
	{
		// Delete old Precipitation
		atmosphere->GetConditions()->SetPrecipitation( SilverLining::CloudLayer::NONE, 0.0 );
		
		// Set new Precipitation
		atmosphere->GetConditions()->SetPrecipitation( SilverLining::CloudLayer::RAIN, rate_mmPerHour_rain_ );
		atmosphere->GetConditions()->SetPrecipitation( SilverLining::CloudLayer::WET_SNOW, rate_mmPerHour_drySnow_ );
		atmosphere->GetConditions()->SetPrecipitation( SilverLining::CloudLayer::DRY_SNOW, rate_mmPerHour_wetSnow_ );
		atmosphere->GetConditions()->SetPrecipitation( SilverLining::CloudLayer::SLEET, rate_mmPerHour_sleet_ );
	}
}
	
bool visual_skySilverLining::getOverallPrecipitationAtLocation( double& rate_mmPerHour_rain, double& rate_mmPerHour_drySnow, double& rate_mmPerHour_wetSnow, double& rate_mmPerHour_sleet, double lat_, double lon_, double height_ )
{
	if (isInitialized())
	{
		// Init
		bool hasPrecipitation = false;
		double x = 0;
		double y = 0;
		double z = 0;
		//// If -1 : Use sky internal values
		if ( lat_ == -1 )
			lat_ = lat;
		if ( lon_ == -1 )
			lon_ = lon;
		if ( height_ == -1 )
			height_ = height;

		// Set precipitation to zero;
		rate_mmPerHour_rain = 0;
		rate_mmPerHour_drySnow = 0;
		rate_mmPerHour_wetSnow = 0;
		rate_mmPerHour_sleet = 0;

		// Get global position
		util::calculateXYZAtWGS84Coordinate(lat_, lon_, height_, sceneRoot, x, y, z);

		// Look up every cloud layer for it's precipitation.
		for( int i=0; i<MAX_CLOUDLAYER_SLOTS; i++ )
		{
			if ( cloudLayerSlots[i].used )	// IF used, Pointer should be valid
			{
				if( cloudLayerSlots[i].cloudLayerPointer->HasPrecipitationAtPosition(x, y, z) )
				{
					hasPrecipitation = true;
					SL_MAP (int, double) precipitationMap = cloudLayerSlots[i].cloudLayerPointer->GetPrecipitation();
					for( SL_MAP (int, double)::iterator it = precipitationMap.begin(); it != precipitationMap.end(); it++ )
					{
						switch(it->first)
						{
							case SilverLining::CloudLayer::RAIN : rate_mmPerHour_rain += it->second;
								break;
							case SilverLining::CloudLayer::DRY_SNOW : rate_mmPerHour_drySnow += it->second;
								break;
							case SilverLining::CloudLayer::WET_SNOW : rate_mmPerHour_wetSnow += it->second;
								break;
							case SilverLining::CloudLayer::SLEET : rate_mmPerHour_sleet += it->second;
								break;
							default: OSG_NOTIFY( osg::FATAL ) << "ERROR: visual_skySilverLining::getOverallPrecipitationAtLocation() : Wrong precipitation type in map!" << std::endl;
								break;
						};
					}
				}	// If slot has Precipitation END
			}	// If used END
		}	// For all slots END

		OSG_NOTIFY( osg::ALWAYS ) << "Rain: " << rate_mmPerHour_rain << ", dry snow: " << rate_mmPerHour_drySnow << ", wet snow: " << rate_mmPerHour_wetSnow << ", sleet: " << rate_mmPerHour_sleet << std::endl;
		return hasPrecipitation;

	}	// If initialized END
	return false;
}


bool visual_skySilverLining::getSlotPrecipitationAtLocation( int slot_, double& rate_mmPerHour_rain, double& rate_mmPerHour_drySnow, double& rate_mmPerHour_wetSnow, double& rate_mmPerHour_sleet, double lat_, double lon_, double height_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		if( cloudLayerSlots[slot_].used )	// IF used, Pointer should be valid
		{
			double x = 0;
			double y = 0;
			double z = 0;
			rate_mmPerHour_rain = 0;
			rate_mmPerHour_drySnow = 0;
			rate_mmPerHour_wetSnow = 0;
			rate_mmPerHour_sleet = 0;
			//// If -1 : Use sky internal values
			if ( lat_ == -1 )
				lat_ = lat;
			if ( lon_ == -1 )
				lon_ = lon;
			if ( height_ == -1 )
				height_ = height;

			// Get global position
			util::calculateXYZAtWGS84Coordinate(lat_, lon_, height_, sceneRoot, x, y, z);

			// Check for precipitation
			if( cloudLayerSlots[slot_].cloudLayerPointer->HasPrecipitationAtPosition(x, y, z) )
			{
				SL_MAP (int, double) precipitationMap = cloudLayerSlots[slot_].cloudLayerPointer->GetPrecipitation();
				for( SL_MAP (int, double)::iterator it = precipitationMap.begin(); it != precipitationMap.end(); it++ )
				{
					switch(it->first)
					{
						case SilverLining::CloudLayer::RAIN : rate_mmPerHour_rain = it->second;
							break;
						case SilverLining::CloudLayer::DRY_SNOW : rate_mmPerHour_drySnow = it->second;
							break;
						case SilverLining::CloudLayer::WET_SNOW : rate_mmPerHour_wetSnow = it->second;
							break;
						case SilverLining::CloudLayer::SLEET : rate_mmPerHour_sleet = it->second;
							break;
						default: OSG_NOTIFY( osg::FATAL ) << "ERROR: visual_skySilverLining::getSlotPrecipitationAtLocation() : Wrong precipitation type in map!" << std::endl;
							break;
					};
				}	// FOR END
				OSG_NOTIFY( osg::ALWAYS ) << "Rain: " << rate_mmPerHour_rain << ", dry snow: " << rate_mmPerHour_drySnow << ", wet snow: " << rate_mmPerHour_wetSnow << ", sleet: " << rate_mmPerHour_sleet << std::endl;
				return true;
			}	// If slot has Precipitation END
			else 
				return false;
		}	// If used END
		else
			return false;
	}	// If initialized END
	return false;
}

bool visual_skySilverLining::getSlotPrecipitation( int slot_, double& rate_mmPerHour_rain, double& rate_mmPerHour_drySnow, double& rate_mmPerHour_wetSnow, double& rate_mmPerHour_sleet )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		if( cloudLayerSlots[slot_].used )	// IF used, Pointer should be valid
		{
			rate_mmPerHour_rain = 0;
			rate_mmPerHour_drySnow = 0;
			rate_mmPerHour_wetSnow = 0;
			rate_mmPerHour_sleet = 0;

			// Check for precipitation

				SL_MAP (int, double) precipitationMap = cloudLayerSlots[slot_].cloudLayerPointer->GetPrecipitation();
				for( SL_MAP (int, double)::iterator it = precipitationMap.begin(); it != precipitationMap.end(); it++ )
				{
					switch(it->first)
					{
						case SilverLining::CloudLayer::RAIN : rate_mmPerHour_rain = it->second;
							break;
						case SilverLining::CloudLayer::DRY_SNOW : rate_mmPerHour_drySnow = it->second;
							break;
						case SilverLining::CloudLayer::WET_SNOW : rate_mmPerHour_wetSnow = it->second;
							break;
						case SilverLining::CloudLayer::SLEET : rate_mmPerHour_sleet = it->second;
							break;
						default: OSG_NOTIFY( osg::FATAL ) << "ERROR: visual_skySilverLining::getSlotPrecipitation() : Wrong precipitation type in map!" << std::endl;
							break;
					};
				}	// FOR END
				
				if ( rate_mmPerHour_rain>0 || rate_mmPerHour_drySnow>0 || rate_mmPerHour_wetSnow>0 || rate_mmPerHour_sleet>0)
				{
					OSG_NOTIFY( osg::ALWAYS ) << "Rain: " << rate_mmPerHour_rain << ", dry snow: " << rate_mmPerHour_drySnow << ", wet snow: " << rate_mmPerHour_wetSnow << ", sleet: " << rate_mmPerHour_sleet << std::endl;
					return true;
				}
				else
					return false;
		}	// If used END
		else
			return false;
	}	// If initialized END
	return false;
}

void visual_skySilverLining::clearAllPrecipitation( int slot_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		if( cloudLayerSlots[slot_].used )	// IF used, Pointer should be valid
		{
			cloudLayerSlots[slot_].cloudLayerPointer->SetPrecipitation( SilverLining::CloudLayer::NONE, 0.0 );
			cloudLayerSlots[slot_].cloudLayerPointer->SetPrecipitation( SilverLining::CloudLayer::NONE, 0.0 ); // Second Call to clear precipitation rate.
		}
	}
}

void visual_skySilverLining::setSlotPrecipitation( int slot_, double rate_mmPerHour_rain_, double rate_mmPerHour_drySnow_, double rate_mmPerHour_wetSnow_, double rate_mmPerHour_sleet_ )
{
	if (isInitialized() && slot_ >= 0 && slot_ < MAX_CLOUDLAYER_SLOTS)
	{
		if( cloudLayerSlots[slot_].used )	// IF used, Pointer should be valid
		{
			cloudLayerSlots[slot_].cloudLayerPointer->SetPrecipitation( SilverLining::CloudLayer::RAIN, rate_mmPerHour_rain_ );
			cloudLayerSlots[slot_].cloudLayerPointer->SetPrecipitation( SilverLining::CloudLayer::DRY_SNOW, rate_mmPerHour_drySnow_ );
			cloudLayerSlots[slot_].cloudLayerPointer->SetPrecipitation( SilverLining::CloudLayer::WET_SNOW, rate_mmPerHour_wetSnow_ );
			cloudLayerSlots[slot_].cloudLayerPointer->SetPrecipitation( SilverLining::CloudLayer::SLEET, rate_mmPerHour_sleet_ );
		}
	}
}

void visual_skySilverLining::configureCloudlayerbyXML( xmlNode* cloudlayerNode_ )
{
	std::string node_name=reinterpret_cast<const char*>(cloudlayerNode_->name);
	if(cloudlayerNode_->type == XML_ELEMENT_NODE && node_name == "cloudlayer")
	{
		int slot = -1;
		bool enabled;
		int fadetime = -1, baselength = -1, basewidth = -1, thickness = -1, baseHeight = -1 ;
		float density = -1.0, rate_mmPerHour_rain = -1.0, rate_mmPerHour_drySnow = -1.0, rate_mmPerHour_wetSnow = -1.0, rate_mmPerHour_sleet = -1.0;
		CloudTypes ctype = CUMULUS_CONGESTUS; 



		xmlAttr  *attr = cloudlayerNode_->properties;
		while ( attr ) 
		{ 
			std::string attr_name=reinterpret_cast<const char*>(attr->name);
			std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
			if( attr_name == "slot" )
			{
				std::stringstream sstr(attr_value);
				sstr >> slot;
			}
			if( attr_name == "enabled" )
			{
				if(attr_value=="yes")
					enabled = true;
				else
					enabled = false;
			}
			if( attr_name == "fadetime" )
			{
				std::stringstream sstr(attr_value);
				sstr >> fadetime;
			}
			if( attr_name == "type" )
			{
				if(attr_value=="CIRROCUMULUS")
					ctype = CIRROCUMULUS;
				if(attr_value=="CIRRUS_FIBRATUS")
					ctype = CIRRUS_FIBRATUS;
				if(attr_value=="STRATUS")
					ctype = STRATUS;
				if(attr_value=="CUMULUS_MEDIOCRIS")
					ctype = CUMULUS_MEDIOCRIS;
				if(attr_value=="CUMULUS_CONGESTUS")
					ctype = CUMULUS_CONGESTUS;
				if(attr_value=="CUMULONIMBUS_CAPPILATUS")
					ctype = CUMULONIMBUS_CAPPILATUS;
				if(attr_value=="STRATOCUMULUS")
					ctype = STRATOCUMULUS;
			}
			attr = attr->next; 
		}

		cloudlayerNode_ = cloudlayerNode_->children;
		if(!cloudlayerNode_)
		{
			OSG_NOTIFY( osg::ALWAYS ) << "ERROR - visual_skySilverLining::configureCloudlayerbyXML: Missing geometry specification for a cloudlayer." << std::endl;
			return;
		}

		for (xmlNode *cur_node = cloudlayerNode_; cur_node; cur_node = cur_node->next)
		{
			if(cur_node->type == XML_ELEMENT_NODE && node_name == "geometry")
			{
				xmlAttr  *attr = cloudlayerNode_->properties;
				while ( attr ) 
				{ 
					std::string attr_name=reinterpret_cast<const char*>(attr->name);
					std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
					if( attr_name == "baselength" )
					{
						std::stringstream sstr(attr_value);
						sstr >> baselength;
					}
					if( attr_name == "basewidth" )
					{
						std::stringstream sstr(attr_value);
						sstr >> basewidth;
					}
					if( attr_name == "thickness" )
					{
						std::stringstream sstr(attr_value);
						sstr >> thickness;
					}
					if( attr_name == "baseHeight" )
					{
						std::stringstream sstr(attr_value);
						sstr >> baseHeight;
					}
					if( attr_name == "density" )
					{
						std::stringstream sstr(attr_value);
						sstr >> density;
					}
					attr = attr->next; 
				}
			}

			if(cur_node->type == XML_ELEMENT_NODE && node_name == "precipitation")
			{
				xmlAttr  *attr = cloudlayerNode_->properties;
				while ( attr ) 
				{ 
					std::string attr_name=reinterpret_cast<const char*>(attr->name);
					std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
					if( attr_name == "rate_mmPerHour_rain" )
					{
						std::stringstream sstr(attr_value);
						sstr >> rate_mmPerHour_rain;
					}
					if( attr_name == "rate_mmPerHour_drySnow" )
					{
						std::stringstream sstr(attr_value);
						sstr >> rate_mmPerHour_drySnow;
					}
					if( attr_name == "rate_mmPerHour_wetSnow" )
					{
						std::stringstream sstr(attr_value);
						sstr >> rate_mmPerHour_wetSnow;
					}
					if( attr_name == "rate_mmPerHour_sleet" )
					{
						std::stringstream sstr(attr_value);
						sstr >> rate_mmPerHour_sleet;
					}
					attr = attr->next; 
				}
			}
		}

		if(slot!=-1 && baselength!=-1 && basewidth!=-1 && thickness!=-1 && baseHeight!=-1 && density!=-1 )
			addCloudLayer( slot, baselength, basewidth, thickness, baseHeight, density, ctype );

		if(slot!=-1 && rate_mmPerHour_rain!=-1 && rate_mmPerHour_drySnow!=-1 && thickness!=-1 && baseHeight!=-1 && density!=-1 )
			setSlotPrecipitation( slot, rate_mmPerHour_rain, rate_mmPerHour_drySnow, rate_mmPerHour_wetSnow, rate_mmPerHour_sleet );
	}
	else
		OSG_NOTIFY( osg::ALWAYS ) << "ERROR - visual_skySilverLining::configureCloudlayerbyXML: Node is not a cloudlayer node." << std::endl;
}
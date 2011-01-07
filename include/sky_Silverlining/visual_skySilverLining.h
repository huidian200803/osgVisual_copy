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
 * 
 * This file is based on the OSG example of the Silverlining SDK:
 * Copyright (c) 2008 Sundog Software, LLC. All rights reserved worldwide.
*/

#include <osg/NodeCallback>
#include <osg/Geode>

#include <SilverLining.h>
#include <MillisecondTimer.h>
#ifdef WIN32
	#include <windows.h>
#endif
#include <skySilverLining_skyDrawable.h>
#include <skySilverLining_cloudsDrawable.h>
#include <skySilverLining_AtmosphereReference.h>
#include <skySilverLining_ProjectionMatrixCallback.h> 
#include <skySilverLining_cloudLayerSlot.h>

#include <visual_util.h>

namespace osgVisual
{

#define MAX_CLOUDLAYER_SLOTS 5 
/**
 * \brief This class is the container class for the sky, stars and clouds
 * 
 * This class is the container class for the sky, stars and clouds. It is responsible for initialization and updating of the sky system.
 * It is realized as Geode that it is possible to install an updateCallback on this class.
 * This way, the sky position is always updated during update traversal of the scene graph.
 * 
 * The NodeCallback class is nested into this class that no external user can use this callback.
 * 
 * @todo Add Cloudlayer->set/getalpha() function
 * @todo Terrain shadows do not exist
 * @todo Calibrate visual range with test objects. Populate a Lookuptable to use for true visibility ranges.
 * @todo Implement an abstract sky interface class to allow other sky implementations than silverlining with identical interfaces
 * 
 * @author Torben Dannhauer
 * @date  Jul 2009
 */ 
class visual_skySilverLining : public osg::Geode
{
	//#include <leakDetection.h>
public:
	/**
	 * \brief Constructor.
	 * 
	 * @param viewer_ : Pointer to the applications viewer
	 * @param configFileName : Filename of the XML configfile to read the configuration from. Currently not used because skySilverlining currently does not need any configuration parameters.
	 */ 
	visual_skySilverLining(osgViewer::Viewer* viewer_, std::string configFileName);

	/**
	 * \brief Destrcutor: The updateCallback is removed and the atmosphere, if instantiated.
	 * 
	 */ 
	virtual ~visual_skySilverLining(void);

	/**
	 * \brief Returns a Pointer to the atmosphere object.
	 * 
	 * @return Pointer to the atmosphere object.
	 */ 
	SilverLining::Atmosphere* getAtmosphere() {return atmosphere;};

	/**
	 * \brief Checks if the sky system is initialized.
	 * 
	 * Checks if the sky system is initialized. If not, it makes a deep lookup at the main cameras userdata, if atmosphere is initialized.
	 * If this returns true, the local variable is updated with ne new initialize status. If this deep lookup returns false, this method returns also false
	 * asks the cameras user data if initialzed. If that is
	 * 
	 * @return : Returns the status of initialization.
	 */ 
	bool isInitialized();
	
	/**
	 * \brief This function initializes the sky framework if its used with a PRE_RENDER camera like the distortion module.
	 * 
	 * @param distortedRoot : osg::Group* which is the rootNode over distortion and scene
	 * @param sceneGraphRoot : osg::Group* which is the rootNode and coordinateSystemNode of the scene.
	 */ 
	void init(osg::Group *distortedRoot, osg::CoordinateSystemNode *sceneGraphRoot);

	/**
	 * \brief This function can be called by the updateCallback. If the postInitialization for adding clouds etc. has be called already, this function will skip.
	 * 
	 * If this function is called before sky is initalized, all actions are skipped.
	 * A standart usage is adding objects like clouds and winds, configuring time, location and visibility.
	 * 
	 */ 
	void postInit();

	/**
	 * \brief This function shuts the sky framework down.
	 * 
	 */ 
	void shutdown();


/** @name Time
 *  The following functions control the simulated time and date. 
 */
//@{
	/**
	 * \brief Sets the new Date and Time to the sky system. 
	 * 
	 * Update is only performed if the sky systme is initialized.
	 * 
	 * @param year_ : Year to set. SilverLining can only handle Gregorian calendar years, which means that years before 1582 will not be accurately simulated.
	 * @param month_ : Month to set. Ranging from 1 (Jan) to 12 (Dec)
	 * @param day_ : Day to set Ranging from 1 to 31
	 * @param hour_ : Hour to set. Ranging from 0 to 23
	 * @param minute_ : Minute to set. Ranging from 0 to 59.
	 * @param second_ : Second to set. Ranging from 0 to 59.
	 * @param daylightSaving_ : Configures, if the configured DateTime is in daylight savings or without daylight savings.
	 * @param timezoneOffset_ : Configures the timezone. Hour offset from GMT, ignoring daylight savings time.
	 */ 
	void setDateTime( int year_, int month_, int day_, int hour_, int minute_, int second_, bool daylightSaving_, double timezoneOffset_ );

	/**
	 * \brief Set time for the sky system.
	 * 
	 * @param hour_ : Hour to set. Ranging from 0 to 23
	 * @param minute_ : Minute to set. Ranging from 0 to 59.
	 * @param second_ : Second to set. Ranging from 0 to 59.
	 */ 
	void setTime( int hour_, int minute_, int second_ );

	/**
	 * \brief Sets the date for the sky system.
	 * 
	 * @param year_ : Year to set. SilverLining can only handle Gregorian calendar years, which means that years before 1582 will not be accurately simulated.
	 * @param month_ : Month to set. Ranging from 1 (Jan) to 12 (Dec)
	 * @param day_ : Day to set Ranging from 1 to 31
	 */ 
	void setDate( int year_, int month_, int day_ );

	/**
	 * \brief Configures the date and time by epoche (Seconds elapsed since midnight, January 1, 1970 UTC) 
	 * 
	 * This function ignores the daylight savings and timezone.
	 * 
	 * @param secondsSince1970_ : Seconds elapsed since midnight, January 1, 1970 UTC
	 */ 
	void setDateByEpoch(int secondsSince1970_ );

//@}

/** @name Location
 *  The following functions control the location to simulate. 
 */
//@{
	/**
	 * \brief Configures the sky system to simulate a specific location.
	 * 
	 * @param lat_ : Latitude in radians
	 * @param lon_ : Longitude in radians
	 * @param alt_ : Altimeter in meters above see level.
	 */ 
	void setLocation(double lat_, double lon_, double alt_);
//@}

/** @name Visibility and Haze
 *  The following functions control the atmosperic effects like visibility range and haze. 
 */
//@{
	/**
	 * \brief This functions sets the visibility range in meters to simulate.
	 * 
	 * @param visibility_ : Visibility range in meters.
	 */ 
	void setVisibility(double visibility_);

	/**
	 * \brief This function returns the visibility range in meters which is simulated. 
	 * 
	 * @return : Visibility range in meters.
	 */ 
	double getVisibility();

	/**
	 * \brief This function sets the haze factor to simulate. 
	 * 
	 * 1 = Pure air, range 256 km
	 * 2 = very clear, range 50 km
	 * 3 = clear, range 15 km
	 * 7 = light haze, range 8 km
	 * 20 = haze, range 3 km
	 * 50 = thin fog, range 2 km
	 * 
	 * @param turbidity_ : Haze factor to simulate. 
	 */ 
	void setTurbidity(double turbidity_);

	/**
	 * \brief This function returns the simulated haze factor.
	 * 
	 * @return : Haze factor.
	 */ 
	double getTurbidity();
//@}

/** @name Light pollution
 *  The following functions control the light pollution to simulate at night. 
 */
//@{
	/**
	 * \brief This functions sets the light pollution in Watt/m2 
	 * 
	 * @param lightPollution_ : Light pollution to set.
	 */ 
	void setLightPollution(double lightPollution_);

	/**
	 * \brief This function returns the simulated light pollution in Watt/m2.
	 * 
	 * @return : Light pollution in Watt/m2
	 */ 
	double getLightPollution();
//@}

/** @name Clouds
 *  The following functions control the available slots for cloudLayer. In each slot a cloud layer can be adde and its perscpitation or
 * baseAltitude updated. All other values are not updateable, therefore a delete and recreation of the cloud layer would be necessary. 
 */
//@{
	/**
	 * \brief This function adds a Cloudlayer to a sepcified SLOT. If the slot is used, this function returns immediately.
	 * 
	 * If the slot number is out of range, nothing happens.
	 * 
	 * Currently supported CloudTypes are:
	 *
	 * \li CloudTypes::CIRROCUMULUS - High planar cloud puffs.
	 * \li CloudTypes::CIRRUS_FIBRATUS - High, wispy cirrus clouds.
     * \li CloudTypes::NIMBOSTRATUS - Low stratus decks.
     * \li CloudTypes::CUMULONIMBUS_CAPPILATUS - A large thunderhead with lightning and rain.
     * \li CloudTypes::CUMULUS_MEDIOCRIS - Small, puffy clouds.
     * \li CloudTypes::CUMULUS_CONGESTUS - Larger cumulus clouds with flattened bottoms.
     * \li CloudTypes::CUMULUS_MEDIOCRIS_INFINITE - A cumulus mediocris layer where the clouds wrap around
     *  the camera location at all times, to create the illusion of an infinitely large cloud layer.
     * \li CloudTypes::CUMULUS_CONGESTUS_INFINITE - A cumulus congestus layer where the clouds wrap around
     *  the camera location at all times, to create the illusion of an infinitely large cloud layer.
	 * 
	 * @param slot_ : Slot to use.
	 * @param baseLength_ : Baselength in meter of the cloud layer to generate.
	 * @param baseWidth_ : Basewideth in meter of the cloud layer to generate.
	 * @param thickness_ : Thicknes of the could layer in meter.
	 * @param baseHeight_ : Height of the cloud base in WGS84.
	 * @param density_ : Coverage of the cloud layer in percent (0.0 - 1.0).
	 * @param cloudtype_ : Type of the cloudlayer to generate.
	 */ 
	void addCloudLayer(int slot_, double baseLength_, double baseWidth_, double thickness_, double baseHeight_, double density_, CloudTypes cloudtype_ );

	/**
	 * \brief This function remove the cloudlayer in the specified slot and marks the slot as free.
	 * 
	 * If the slot number is out of range, nothing happens.
	 * 
	 * @param slot_ : Slot number to clean 
	 */ 
	void removeCloudLayer( int slot_ );
	
	/**
	 * \brief This function removes the cloud layers from all slots
	 * 
	 */ 
	void clearAllSlots();

	/**
	 * \brief This function returns a pointer to the cloudLayer this slot contains.
	 * 
	 * If the slot number is out of range, nothing happens.
	 * 
	 * @param slot_ : Slot to query for it's cloudLayer pointer.
	 * @return : Pointer to the contained cloud layer.
	 */ 
	SilverLining::CloudLayer* getCloudLayer( int slot_ );
	
	/**
	 * \brief This function configure the cloud layer als enabled (show clouds) or disabled (hide clouds).
	 * 
	 * If the slot number is out of range, nothing happens.
	 * 
	 * @param slot_ : Cloudlayer to hide/show.
	 * @param enabled_ : Control where hide or show should be configured.
	 */ 
	void setEnabled(int slot_, bool enabled_ );

	/**
	 * \brief This function returns if the specified cloud layer is en- or disabled (hidden or shown).
	 * 
	 * If the slot number is out of range, nothing happens.
	 * 
	 * @param slot_ : Slot zu query.
	 * @return : true if clouds are visible, false if clouds are hidden.
	 */ 
	bool isEnabled( int slot_ );

	/**
	 * \brief This functions fade the clouds in or out, depending on the visibility tsatus before.
	 * 
	 * If the slot number is out of range, nothing happens.
	 * 
	 * @param slot_ : Slot to fade.
	 * @param fadetimeMS_ : Milliseconds to fade the cloud layer.
	 */ 
	void fadeVisibility( int slot_, int fadetimeMS_);

	/**
	 * \brief 
	 * 
	 * If the slot number is out of range, nothing happens.
	 * 
	 * @param slot_ 
	 * @return : String with the literal name of the cloud layer type.
	 */ 
	std::string getCloudLayerTypeName( int slot_ );

//@}

/** @name Wind
 *  Description of wind. Attention: Wind volumes are not geocentric: If far enough away from upvector location, the earth will curve away under the wind volume.
 */
//@{
	/**
	 * \brief This function adds a wind volume to the scene, with the specified characteristics.
	 * 
	 * @param bottom_ : Height in WGS84 in metern of the wind volumes bottom. 
	 * @param top_ : Height in WGS84 in metern of the wind volumes top. 
	 * @param speed_ : Windspeed in the wind volume in meter per seconds.
	 * @param direction_ : Wind direction in the wind volume in degree (wind origin, not wind destination).
	 * 
	 */ 
	void addWindVolume(double bottom_, double top_, double speed_, int direction_);

	/**
	 * \brief This function deletes all wind volumes.
	 * 
	 */ 
	void clearAllWindVolumes();

	/**
	 * \brief This functions checks wether the specified altitude in MSL is inside a wind volume.
	 * 
	 * If multiple wind volumes are nested, this function returns the first matching wind volume.
	 * Because geocentric wind is not implemented, the altitude information is always base on the location where the wind was added.
	 * To avoid multiple winds, don't use nested wind volumes.
	 * 
	 * @param height_ : Height in meters in WGS84 to check for wind.
	 * @param bottom_ : If checking altitude is inside a volume, the bottom altitude of the volume in meters above MSL is written into, otherwise -1.
	 * @param top_ : If checking altitude is inside a volume, the top altitude of the volume in meters above MSL is written into, otherwise -1.
	 * @param speed_ : If checking altitude is inside a volume, the wind speed in the wind volume is written into, otherwise -1.
	 * @param direction_ : If checking altitude is inside a volume, the wind direction (0 deg = from north, 90 deg = from east) in the wind volume is written into, otherwise -1.
	 * @return : True if checking altitude is inside a wind volume, otherwise false.
	 */ 
	bool insideWind(double height_, double& bottom_, double& top_, double& speed_, double& direction_);

	
//@}

/** @name Precipitation
 *  The following functions control the rate and type of precipitation. It is possible to configure it on an global or a per cloud basis. 
 */
//@{
	// Per SLOT
	/**
	 * \brief This functions enables the specified precipitation rate and precipitation type at the specified cloudLayer slot.
	 * 
	 * @param slot_ : CloudLayer slot to enable precipitation in.
	 * @param rate_mmPerHour_rain_ : Precipitation rate in mm/hour of rain.
	 * @param rate_mmPerHour_drySnow_ : Precipitation rate in mm/hour of dry snow.
	 * @param rate_mmPerHour_wetSnow_ : Precipitation rate in mm/hour of wet snow.
	 * @param rate_mmPerHour_sleet_ : Precipitation rate in mm/hour of sleet.
	 */ 
	void setSlotPrecipitation( int slot_, double rate_mmPerHour_rain_, double rate_mmPerHour_drySnow_, double rate_mmPerHour_wetSnow_, double rate_mmPerHour_sleet_ );

	/**
	 * \brief This function retrieves the precipitation of a cloudlayer slot.
	 * 
	 * @param slot_ : Slot to check for precipitation. 
	 * @param rate_mmPerHour_rain : Precipitation rate in mm/hour of rain.
	 * @param rate_mmPerHour_drySnow : Precipitation rate in mm/hour of dry snow.
	 * @param rate_mmPerHour_wetSnow : Precipitation rate in mm/hour of wet snow.
	 * @param rate_mmPerHour_sleet : Precipitation rate in mm/hour of sleet.
	 * @return : True if cloudLayer has precipitation.
	 */ 
	bool getSlotPrecipitation( int slot_, double& rate_mmPerHour_rain, double& rate_mmPerHour_drySnow, double& rate_mmPerHour_wetSnow, double& rate_mmPerHour_sleet );
	
	/**
	 * \brief This function checks a specified location for precipitation at the specified cloudLayer slot.
	 * 
	 * @param slot_ : CloudLayer slot to check for specification.
	 * @param rate_mmPerHour_rain : Precipitation rate in mm/hour of rain.
	 * @param rate_mmPerHour_drySnow : Precipitation rate in mm/hour of dry snow.
	 * @param rate_mmPerHour_wetSnow : Precipitation rate in mm/hour of wet snow.
	 * @param rate_mmPerHour_sleet : Precipitation rate in mm/hour of sleet.
	 * @param lat_ : Latitude to check for precipitation. If it defaults to -1, the internal sky latitude will be used.
	 * @param lon_ : Longitude to check for precipitation. If it defaults to -1, the internal sky longitude will be used.
	 * @param height_ : Height in WGS84 to check for precipitation. If it defaults to -1, the internal sky height will be used.
	 * @return : Returns true if precipitation at the specified location is true.  
	 */ 
	bool getSlotPrecipitationAtLocation( int slot_, double& rate_mmPerHour_rain, double& rate_mmPerHour_drySnow, double& rate_mmPerHour_wetSnow, double& rate_mmPerHour_sleet, double lat_=-1, double lon_=-1, double height_=-1 );

	/**
	 * \brief This function clears all types of precipitation of the specified slot.
	 * 
	 * @param slot_ : Slot toe clear precipitation.
	 */ 
	void clearAllPrecipitation( int slot_ );

	// Global
	/**
	 * \brief This function clears the global precipitation.
	 * 
	 */ 
	void clearGlobalPrecipitation();

	/**
	 * \brief This function sets the cloud independent, global precipitation.
	 * 
	 * @param rate_mmPerHour_rain_ : Precipitation rate in mm/hour of rain.
	 * @param rate_mmPerHour_drySnow_ : Precipitation rate in mm/hour of dry snow.
	 * @param rate_mmPerHour_wetSnow_ : Precipitation rate in mm/hour of wet snow.
	 * @param rate_mmPerHour_sleet_ : Precipitation rate in mm/hour of sleet.
	 */ 
	void setGlobalPrecipitation( double rate_mmPerHour_rain_, double rate_mmPerHour_drySnow_, double rate_mmPerHour_wetSnow_, double rate_mmPerHour_sleet_ );
	
	// Misc
	/**
	 * \brief This function returns the overall precipitation rate of all cloud dependent precipitation.
	 * 
	 * This function adds the precipitation of all cloudLayer seperated for all types of precipitation.
	 * Global precipitation is ignored, because precipitation without clouds is artificial and only reasonable for visualiszation purposes.
	 * 
	 * @param rate_mmPerHour_rain : Precipitation rate in mm/hour of rain.
	 * @param rate_mmPerHour_drySnow : Precipitation rate in mm/hour of dry snow.
	 * @param rate_mmPerHour_wetSnow : Precipitation rate in mm/hour of wet snow.
	 * @param rate_mmPerHour_sleet : Precipitation rate in mm/hour of sleet.
	 * @param lat_ : Latitude to check for precipitation. If it defaults to -1, the internal sky latitude will be used.
	 * @param lon_ : Longitude to check for precipitation. If it defaults to -1, the internal sky longitude will be used.
	 * @param height_ : Height in WGS84 to check for precipitation. If it defaults to -1, the internal sky height will be used.
	 * @return : True if precipitation lookup was successful.
	 */ 
	bool getOverallPrecipitationAtLocation( double& rate_mmPerHour_rain, double& rate_mmPerHour_drySnow, double& rate_mmPerHour_wetSnow, double& rate_mmPerHour_sleet, double lat_=-1, double lon_=-1, double height_=-1 );
//@}





private:
	/**
	 * \brief Nested Callback class
	 * 
	 * @author Torben Dannhauer
	 * @date  Jul 2009
	 */ 
	class skyUpdateCallback : public osg::NodeCallback
	{
	public:
		/**
		 * \brief Constructor
		 * 
		 * @param csn_ : Pointer to the Coordinate System Node. Necessary to extract lat, lon and height of the camera position.
		 * @param sceneCamera_ : Pointer to the scene camera (undistorted camera, type PRE_RENDER)
		 * @param sky_ : Pointer to the sky system.
		 */ 
		skyUpdateCallback(osg::CoordinateSystemNode* csn_, osg::Camera* sceneCamera_, visual_skySilverLining* sky_)
			: csn(csn_), sceneCamera(sceneCamera_), sky(sky_) {};

		/**
		 * \brief This function is executed as callback during traversal. It updates the skys displayed location.
		 * 
		 */ 
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	private:
		/**
		 * Referenced pointer to the Coordinate System Node. Necessary to extract lat, lon and height of the camera position.
		 */ 
		osg::ref_ptr<osg::CoordinateSystemNode> csn;
		
		/**
		 * Referenced pointer to the scene camera (undistorted camera, type PRE_RENDER)
		 */ 
		osg::ref_ptr<osg::Camera> sceneCamera;

		/**
		 * Referenced pointer to the sky system.
		 */ 
		osg::ref_ptr<visual_skySilverLining> sky;
	};	// Nested class END

	/**
	 * \brief This function updates the atmosphere's upVector, 
	 * if delta_lat or delta_lon (from the existing location
	 * and the location of the last upVector update) is larger than N degree.
	 * If an update is requiered, the existing location is noted as the last update location.
	 * 
	 * Iis called by this->setLocation.
	 * 
	 */ 
	void updateUpVector();

	/**
	 * Pointer to the atmosphere object of Silverlining.
	 */ 
	SilverLining::Atmosphere *atmosphere;

	/**
	 * Referenced Pointer to the viewer
	 */ 
	osg::ref_ptr<osgViewer::Viewer> viewer;

	/**
	 * Referenced Pointer to the camera which pre renders the scene
	 */ 
	osg::ref_ptr<osg::Camera> sceneCamera;
 
	/**
	 * Referenced Pointer to the coordinate system node. Required for WGS84 transformations.
	 */ 
	osg::ref_ptr<osg::CoordinateSystemNode> sceneRoot;

	/**
	 * Referenced Pointer to the updatecallback, which is installed at this node. 
	 */ 
	osg::ref_ptr<skyUpdateCallback> updateCallback;

	/**
	 * Referenced Pointer to the ProjectionMatrix callback.
	 */ 
	osg::ref_ptr<skySilverLining_projectionMatrixCallback> cb;

	/**
	 * Referenced Pointer to the sky Drawable, which renders the skybox.
	 */ 
	osg::ref_ptr<skySilverLining_skyDrawable> skyDrawable;

	/**
	 * Referenced Pointer to the cloudsDrawable, which renders the clouds.
	 */ 
	osg::ref_ptr<skySilverLining_cloudsDrawable> cloudsDrawable;

	/**
	 * Flag which contains if the sky system is initialized. This status is only a local
	 * copy if the cameras userdata which contain the primary information if the sky system is initialized
	 */ 
	bool atmosphereInitialized;

	/**
	 * This variable indicates whether postinit() was called already.
	 */ 
	bool postInitialized;

	/**
	 * This vector contains all available slots for cloud layer.
	 */ 
	std::vector<cloudLayerSlot> cloudLayerSlots;

		/**
	 * Latitude, which mirrors the latitude of the last sky update. This is neccesary if any weather effect is added to calculate the earth radius on that location for appropriate level above MSL
	 */ 
	double lat;

	/**
	 * Longitude, which mirrors the latitude of the last sky update. This is neccesary if any weather effect is added to calculate the earth radius on that location for appropriate level above MSL
	 */ 
	double lon;

	/**
	 * Height, which mirrors the height of the last sky update.
	 */ 
	double height;

};

using namespace SilverLining;

//class MyMillisecondTimer : public SilverLining::MillisecondTimer
//{
//public:
//	virtual unsigned long GetMilliseconds() const
//        {
//			return timeGetTime() * 50;
//        }
//};


} // END NAMESPACE
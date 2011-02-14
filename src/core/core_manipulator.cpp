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

#include <core_manipulator.h>

using namespace osgVisual;

core_manipulator::core_manipulator()
{
#ifdef USE_SPACENAVIGATOR
	_mouse = NULL;
#endif
	_currentTrackingID = -1;

}

bool core_manipulator::init(osgViewer::Viewer* viewer, osg::ArgumentParser& arguments, std::string configFilename, osg::Node* rootNode)
{
	_rootNode = rootNode;
	_viewer = viewer;

	// Setup manipulators
	if(!visual_dataIO::getInstance()->isSlave()) // set up the camera manipulators if not slave.
    {
        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

        keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
        keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
        keyswitchManipulator->addMatrixManipulator( '3', "Terrain", new osgGA::TerrainManipulator() );
		_nt = new osgGA::NodeTrackerManipulator();
		_nt->setTrackNode(NULL);
		keyswitchManipulator->addMatrixManipulator( '4', "NodeTrackerManipulator", _nt );
		
#ifdef USE_SPACENAVIGATOR
		// SpaceNavigator manipulator
		_mouse = new SpaceMouse();
		_mouse->initialize();
		_mouseTrackerManip = new NodeTrackerSpaceMouse(_mouse);
		_mouseTrackerManip->setTrackerMode(NodeTrackerSpaceMouse::NODE_CENTER);
		_mouseTrackerManip->setRotationMode(NodeTrackerSpaceMouse::ELEVATION_AZIM);
		_mouseTrackerManip->setAutoComputeHomePosition( true );
		keyswitchManipulator->addMatrixManipulator( '5', "Spacemouse Node Tracker", _mouseTrackerManip );
		keyswitchManipulator->addMatrixManipulator( '6', "Spacemouse Free (Airplane)", new FreeManipulator(_mouse) );
#endif

		// objectMounted Manipulator for Camera control by Nodes
		_objectMountedCameraManip = new objectMountedManipulator();
		keyswitchManipulator->addMatrixManipulator( '7', "Object mounted Camera", _objectMountedCameraManip );

		// Animation path manipulator
		std::string pathfile = util::getAnimationPathFromXMLConfig(configFilename);
        char keyForAnimationPath = '8';
		if( pathfile != "" )
        {
            osgGA::AnimationPathManipulator* apm = new osgGA::AnimationPathManipulator(pathfile);
            if (apm || !apm->valid()) 
            {
                unsigned int num = keyswitchManipulator->getNumMatrixManipulators();
                keyswitchManipulator->addMatrixManipulator( keyForAnimationPath, "Path", apm );
                keyswitchManipulator->selectMatrixManipulator(num);
                ++keyForAnimationPath;
            }
        }

        viewer->setCameraManipulator( keyswitchManipulator.get() );
    }	// If not Slave END

    viewer->addEventHandler( new osgGA::StateSetManipulator(_rootNode->getOrCreateStateSet()) );		// add the state manipulator
    viewer->addEventHandler(new osgViewer::ThreadingHandler);				// add the thread model handler
    viewer->addEventHandler(new osgViewer::WindowSizeHandler);				// add the window size toggle handler
    viewer->addEventHandler(new osgViewer::StatsHandler);					// add the stats handler
    viewer->addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));			// add the help handler
    viewer->addEventHandler(new osgViewer::RecordCameraPathHandler);		// add the record camera path handler
    viewer->addEventHandler(new osgViewer::LODScaleHandler);				// add the LOD Scale handler
    viewer->addEventHandler(new osgViewer::ScreenCaptureHandler);			// add the screen capture handler

	// Install Callback
	_callback = new core_manipulatorCallback(this);
	viewer->getCamera()->addEventCallback(_callback);

	return true;
}

void core_manipulator::shutdown()
{
	// Remove and Delete Callback
	_viewer->getCamera()->removeEventCallback(_callback);
	_callback= NULL;

#ifdef USE_SPACENAVIGATOR
	//Delete SpaceMouse driver
	if(_mouse)
	{
		_mouse->shutdown();
		delete _mouse;
	}
	_mouseTrackerManip = NULL;
#endif

	_objectMountedCameraManip = NULL;
	_nt = NULL;
	_rootNode = NULL;
	_viewer = NULL;
}

void core_manipulator::trackNode( int trackingID )
{
	osg::ref_ptr<osg::Node> tmp = visual_object::findNodeByTrackingID(trackingID, _rootNode);
	if(tmp.valid())
	{
		_currentTrackingID = trackingID;
		trackNode(tmp);
	}
}

void core_manipulator::trackNode( osg::Node* node_ )
{
	if(!node_)
		return;

	osg::Node* node = NULL;
	// Check if tracked node is a visual_object
	osgVisual::visual_object* trackedObject = dynamic_cast<osgVisual::visual_object*>(node_);
	if(trackedObject)
	{
		node = trackedObject->getGeometry();

		// Object mounted manipulator ( Only working with visual_object, not with osg::Node )
		if (_objectMountedCameraManip.valid())
			_objectMountedCameraManip->setAttachedObject( trackedObject );
	}
	else
		node = node_;

	// Spacemouse Node Tracker
#ifdef USE_SPACENAVIGATOR
	if (_mouseTrackerManip.valid())
	{
		_mouseTrackerManip->setTrackNode( node );
		_mouseTrackerManip->setMinimumDistance( 100 );
	}
#endif

	// Classical OSG Nodetracker
	if(_nt.valid())
	{
		osgGA::NodeTrackerManipulator::TrackerMode trackerMode = osgGA::NodeTrackerManipulator::NODE_CENTER;
		osgGA::NodeTrackerManipulator::RotationMode rotationMode = osgGA::NodeTrackerManipulator::ELEVATION_AZIM;
		_nt->setTrackerMode(trackerMode);
		_nt->setRotationMode(rotationMode);
		_nt->setMinimumDistance( 100 );
		_nt->setTrackNode( node );
		_nt->setAutoComputeHomePosition( true );
		_nt->setDistance( 250 );
	}
}

void core_manipulator::core_manipulatorCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	//OSG_NOTIFY( osg::ALWAYS ) << "---- Executing core_manipulatorCallback .." <<  std::endl;

	if(!_manipulators->_updaterSlot.empty())
	{
		int idToTrack = visual_dataIO::getInstance()->getSlotDataAsDouble(_manipulators->_updaterSlot, osgVisual::dataIO_slot::TO_OBJ );
		if(idToTrack!=_manipulators->_currentTrackingID)
			_manipulators->trackNode(idToTrack);
	}

	traverse(node, nv);
}
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


#include <visual_core.h>
#include <visual_util.h>
#include <osgTerrain/Terrain>
#include <osgDB/FileNameUtils>

using namespace osgVisual;

visual_core::visual_core(osg::ArgumentParser& arguments_) : arguments(arguments_)
{
	OSG_NOTIFY( osg::ALWAYS ) << "visual_core instantiated." << std::endl;
}

visual_core::~visual_core(void)
{
	OSG_NOTIFY( osg::ALWAYS ) << "visual_core destroyed." << std::endl;
}

void visual_core::initialize()
{
	OSG_NOTIFY( osg::ALWAYS ) << "Initialize visual_core..." << std::endl;

	// Check for config file to provide it to all modules during initialization.
	if( arguments.read("-c", configFilename) || arguments.read("--config", configFilename) )
	{
		if( !osgDB::fileExists(configFilename) )
			configFilename = "";
		else
			OSG_ALWAYS << "Using configuration file: " << configFilename << std::endl;
	}

	// Configure osg to use KdTrees
	osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::ReaderWriter::Options::BUILD_KDTREES);

	// Configure Multisampling
	osg::DisplaySettings::instance()->setNumMultiSamples(4);

	// Setup pathes
	osgDB::Registry::instance()->getDataFilePathList().push_front("H:\AllInOnDB");
	osgDB::Registry::instance()->getDataFilePathList().push_back( "D:\\DA\\osgVisual\\models" );

	
	// Setup viewer
	viewer = new osgViewer::Viewer(arguments);

	// Setup coordinate system node
	rootNode = new osg::CoordinateSystemNode;	// todo memleakf
	rootNode->setEllipsoidModel(new osg::EllipsoidModel());

	// Test memory leak (todo)
	double* test = new double[1000];

	//osg::DisplaySettings::instance()->setNumOfDatabaseThreadsHint( 8 );

	// Show model
	viewer->setSceneData( rootNode );

	osg::Group* distortedSceneGraph = NULL;
#ifdef USE_DISTORTION
	// Initialize distortion
	distortion = new visual_distortion( viewer, arguments, configFilename );
	distortedSceneGraph = distortion->initialize( rootNode, viewer->getCamera()->getClearColor() );
#endif

#ifdef USE_SKY_SILVERLINING
	// Initialize sky
	bool disabled = false;	// to ask if the skyp is disabled or enabled
	sky = new visual_skySilverLining( viewer, configFilename, disabled );
	if(disabled)
		sky = NULL;
	if(sky.valid())
		sky->init(distortedSceneGraph, rootNode);	// Without distortion: distortedSceneGraph=NULL
#endif

	// Initialize DataIO interface
	visual_dataIO::getInstance()->init(viewer, configFilename);

	// Add manipulators for user interaction - after dataIO to be able to skip it in slaves rendering machines.
	manipulators = new core_manipulator();
	manipulators->init( viewer, arguments, configFilename, rootNode);

	// create the windows and run the threads.
	viewer->realize();

	loadTerrain(arguments);

	// All modules are initialized - now check arguments for any unused parameter.
	checkCommandlineArgumentsForFinalErrors();

	// Run visual main loop
	mainLoop();
}

void visual_core::mainLoop()
{
	int framestoScenerySetup = 5;
	// run main loop
	while( !viewer->done() )
    {
		// setup scenery
		if(framestoScenerySetup-- == 0)
			setupScenery();

		// next frame please....
        viewer->advance();

		/*double hat, hot, lat, lon, height;
		util::getWGS84ofCamera( viewer->getCamera(), rootNode, lat, lon, height);
		if (util::queryHeightOfTerrain( hot, rootNode, lat, lon) && util::queryHeightAboveTerrainInWGS84( hat, rootNode, lat, lon, height ) )
			OSG_NOTIFY( osg::ALWAYS ) << "HOT is: " << hot << ", HAT is: " << hat << std::endl;*/
	
		// perform all queued events
		viewer->eventTraversal();

		// update the scene by traversing it with the the update visitor which will
        // call all node update callbacks and animations.
        viewer->updateTraversal();
		
        // Render the Frame.
        viewer->renderingTraversals();

    }	// END WHILE
}

void visual_core::shutdown()
{
	OSG_NOTIFY( osg::ALWAYS ) << "Shutdown visual_core..." << std::endl;

	// Shutdown Dbug HUD
	if(hud.valid())
		hud->shutdown();
	// Unset scene data
	viewer->setSceneData( NULL );

#ifdef USE_SKY_SILVERLINING
	// Shutdown sky
	if( sky.valid() )
		sky->shutdown();
#endif

#ifdef USE_DISTORTION
	// Shutdown distortion
	if( distortion.valid() )
		distortion->shutdown();
#endif

	// Shutdown data
	rootNode = NULL;

	// Shutdown dataIO
	visual_dataIO::getInstance()->shutdown();

	// Shutdown manipulators
	if(manipulators.valid())
		manipulators->shutdown();

	// Destroy osgViewer
	viewer = NULL;
}

bool visual_core::loadTerrain(osg::ArgumentParser& arguments_)
{
	std::vector<std::string> terrainFile = util::getTerrainFromXMLConfig(configFilename);

	// Add each terrain path to the FilePath list to help OSG to find the subtiles. 
	for(unsigned int i=0;i<terrainFile.size();i++)
		osgDB::Registry::instance()->getDataFilePathList().push_back(osgDB::getFilePath(terrainFile[i]));

	osg::ref_ptr<osg::Node> model = osgDB::readNodeFiles(terrainFile);
	if( model.valid() )
	{
        osgTerrain::Terrain* terrain = util::findTopMostNodeOfType<osgTerrain::Terrain>(model.get());
		if (!terrain)
		{
			terrain = new osgTerrain::Terrain;
			terrain->addChild(model.get());

			model = terrain;			
		}
		rootNode->addChild( terrain );
		return true;
	}
	else
	{
        OSG_NOTIFY( osg::FATAL ) << "Load terrain: No data loaded" << std::endl;
        return false;
    }    

	return false;
}

void visual_core::parseScenery(xmlNode* a_node)
{
	OSG_ALWAYS << "parseScenery()" << std::endl;

	a_node = a_node->children;

	for (xmlNode *cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		std::string node_name=reinterpret_cast<const char*>(cur_node->name);

		// terrain is parsend seperately
		// animationpath is parsend seperately in util::getAnimationPathFromXMLConfig(..) which invokes this function.

		if(cur_node->type == XML_ELEMENT_NODE && node_name == "models")
		{
			for (xmlNode *modelNode = cur_node->children; modelNode; modelNode = modelNode->next)
			{
				std::string name=reinterpret_cast<const char*>(modelNode->name);
				if(modelNode->type == XML_ELEMENT_NODE && name == "model")
				{
					visual_object::createNodeFromXMLConfig(rootNode, modelNode);
				}
				if(modelNode->type == XML_ELEMENT_NODE && name == "trackmodel")
				{
					// Extract track-ID and track the model
					xmlAttr  *attr = modelNode->properties;
					while ( attr ) 
					{ 
						std::string attr_name=reinterpret_cast<const char*>(attr->name);
						std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
						if( attr_name == "id" ) manipulators->trackNode( util::strToInt(attr_value) );
						if( attr_name == "updater_slot" ) manipulators->setTrackingIdUpdaterSlot(attr_value);
						attr = attr->next; 
					}
					
				}
			}
		}

#ifdef USE_SKY_SILVERLINING
		if(cur_node->type == XML_ELEMENT_NODE && node_name == "datetime")
		{
			int day=-1, month=-1, year=-1, hour=-1, minute=-1;

			xmlAttr  *attr = cur_node->properties;
			while ( attr ) 
			{ 
				std::string attr_name=reinterpret_cast<const char*>(attr->name);
				std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
				if( attr_name == "day" ) day = util::strToInt(attr_value);
				if( attr_name == "month" ) month = util::strToInt(attr_value);
				if( attr_name == "year" ) year = util::strToInt(attr_value);
				if( attr_name == "hour" ) hour = util::strToInt(attr_value);
				if( attr_name == "minute" ) minute = util::strToInt(attr_value);

				attr = attr->next; 
			}
			if(sky.valid())
			{
				if(day!=0 && month!=0 && year!=0)
					sky->setDate(year, month, day);
				sky->setTime(hour,minute,00);
			}
		}

		if(cur_node->type == XML_ELEMENT_NODE && node_name == "visibility")
		{
			float range = 50000, turbidity=2.2;
			xmlAttr  *attr = cur_node->properties;
			while ( attr ) 
			{ 
				std::string attr_name=reinterpret_cast<const char*>(attr->name);
				std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
				if( attr_name == "range" ) range = util::strToDouble(attr_value);
				if( attr_name == "turbidity" ) turbidity = util::strToDouble(attr_value);

				attr = attr->next; 
			}

			if(sky.valid())
			{
				sky->setVisibility( range );
				sky->setTurbidity( turbidity );
			}
		}

		if(cur_node->type == XML_ELEMENT_NODE && node_name == "clouds")
		{
			if(sky.valid())
				sky->configureCloudlayerbyXML( cur_node );
		}

		if(cur_node->type == XML_ELEMENT_NODE && node_name == "windlayer")
		{
			float bottom = 0.0, top=5000.0, speed=25.0, direction=0.0;
			xmlAttr  *attr = cur_node->properties;
			while ( attr ) 
			{ 
				std::string attr_name=reinterpret_cast<const char*>(attr->name);
				std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
				if( attr_name == "bottom" ) bottom = util::strToDouble(attr_value);
				if( attr_name == "top" ) top = util::strToDouble(attr_value);
				if( attr_name == "speed" ) speed = util::strToDouble(attr_value);
				if( attr_name == "direction" ) direction = util::strToDouble(attr_value);

				attr = attr->next; 
			}
			if(sky.valid())
			{
				sky->addWindVolume( bottom, top, speed, direction );
			}
		}

		// Track Node

#endif
	}// FOR all nodes END

}

bool visual_core::checkCommandlineArgumentsForFinalErrors()
{
	// Setup Application Usage
	arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
	arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" is the new FSD visualization tool, written by Torben Dannhauer");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [OSG options] -c XML-Configurationfile");
	arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");
	arguments.getApplicationUsage()->addCommandLineOption("-c or --config","XML configuration filename");


    // if user request help write it out to cout.
    if (arguments.read("-h") || arguments.read("--help"))
    {
        arguments.getApplicationUsage()->write(std::cout);
		//cause the viewer to exit and shut down clean.
        viewer->setDone(true);
    }

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
		//cause the viewer to exit and shut down clean.
        viewer->setDone(true);
    }

	 // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return false;
    }
	return true;
}

void visual_core::setupScenery()
{
	// Parse Scenery from Configuration file
	xmlDoc* tmpDoc;
	xmlNode* sceneryNode = util::getSceneryXMLConfig(configFilename, tmpDoc);
	parseScenery(sceneryNode);
	if(sceneryNode)
	{
		xmlFreeDoc(tmpDoc); xmlCleanupParser();
	}

	osgTerrain::Terrain* terrain = util::findTopMostNodeOfType<osgTerrain::Terrain>(rootNode);
    if (!terrain)
    {
        OSG_ALWAYS << "No TerrainNode found!" << std::endl;
    }
	else
	{
		//OSG_ALWAYS << "BorderEqual activated" << std::endl;
		//terrain->setEqualizeBoundaries(true);
	}

	//testObj = new visual_object( rootNode, "testStab", objectMountedCameraManip );
	//testObj->setNewPosition( osg::DegreesToRadians(47.7123), osg::DegreesToRadians(12.84088), 600 );
	///* using a huge cylinder to test position & orientation */
	//testObj->setGeometry( util::getDemoCylinder(5000.0, 20.0 ) );
	//testObj->addUpdater( new object_updater(testObj) );
	//testObj->setTrackingId(2);

	//osg::ref_ptr<visual_object> testObj2 = new visual_object( rootNode, "neuschwanstein" );	// todo memleak
	////testObj2->setNewPosition( osg::DegreesToRadians(47.8123), osg::DegreesToRadians(12.94088), 600 );
	//testObj2->setNewPosition( osg::DegreesToRadians(47.557523564234), osg::DegreesToRadians(10.749646398595), 950 );
	//testObj2->loadGeometry( "../models/neuschwanstein.osgb" );
	////testObj2->addUpdater( new object_updater(testObj2) );
	//testObj2->setTrackingId(3);

	//osg::ref_ptr<visual_object> testObj3 = new visual_object( rootNode, "SAENGER1" );	// todo memleak
	//testObj3->setNewPosition( osg::DegreesToRadians(47.8123), osg::DegreesToRadians(12.94088), 600 );
	//testObj3->loadGeometry( "../models/saenger1.flt" );
	//testObj3->addUpdater( new object_updater(testObj3) );
	//testObj3->setTrackingId(4);

	//osg::ref_ptr<visual_object> testObj4 = new visual_object( rootNode, "SAENGER2" );	// todo memleak
	//testObj4->setNewPosition( osg::DegreesToRadians(47.8123), osg::DegreesToRadians(12.94088), 650 );
	//testObj4->loadGeometry( "../models/saenger2.flt" );
	//testObj4->addUpdater( new object_updater(testObj4) );
	//testObj4->addLabel("testLabel", "Object4 :)",osg::Vec4(1.0f,0.25f,1.0f,1.0f));
	//testObj4->setTrackingId(2);

	//osg::ref_ptr<visual_object> testObj5 = new visual_object( rootNode, "SAENGER" );	// todo memleak
	//testObj5->setNewPosition( osg::DegreesToRadians(47.8123), osg::DegreesToRadians(12.94088), 550 );
	//testObj5->loadGeometry( "../models/saengerCombine.flt" );
	////testObj5->setScale( 2 );
	//testObj5->addUpdater( new object_updater(testObj5) );
	//testObj5->setTrackingId(6);

	//manipulators->trackNode( testObj4 );

	// Load EDDF
	//std::string filename = "D:\\DA\\EDDF_test\\eddf.ive";
	//if( !osgDB::fileExists(filename) )
	//{
	//	OSG_NOTIFY(osg::ALWAYS) << "Warning: EDDF Model not loaded. File '" << filename << "' does not exist. Skipping.";
	//}
	//// read model
	//osg::ref_ptr<osg::Node> tmpModel = osgDB::readNodeFile( filename );
	//if (tmpModel.valid())
	//	rootNode->addChild( tmpModel );
	
 
	visual_draw2D::getInstance()->init( rootNode, viewer );
	//osg::ref_ptr<visual_hud> hud = new visual_hud();
	hud = new visual_debug_hud();
	hud->init( viewer, rootNode );
	
	

	//osg::ref_ptr<visual_draw3D> test = new visual_draw3D();
	//test->init( rootNode, viewer );

	//// Creating Testclasses
	//osg::ref_ptr<osgVisual::dataIO_transportContainer> test = new osgVisual::dataIO_transportContainer();
	//osg::ref_ptr<osgVisual::dataIO_executer> testEx = new osgVisual::dataIO_executer();
	//osg::ref_ptr<osgVisual::dataIO_slot> testSlot = new osgVisual::dataIO_slot();
	//test->setFrameID( 22 );
	//test->setName("ugamoep");
	//testEx->setexecuterID( osgVisual::dataIO_executer::IS_COLLISION );
	//testSlot->setVariableName(std::string("HalloName"));
	//testSlot->setdataDirection( osgVisual::dataIO_slot::TO_OBJ );
	//testSlot->setvarType( osgVisual::dataIO_slot::DOUBLE );
	//testSlot->setValue( 0.12345 );
	//test->addExecuter( testEx );
	//test->addSlot( testSlot );

	visual_dataIO::getInstance()->setSlotData("TestSlot1", osgVisual::dataIO_slot::TO_OBJ, 0.12345);

}
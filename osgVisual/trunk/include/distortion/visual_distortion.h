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

#include <osg/NodeCallback>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/Notify>
#include <osg/Referenced>

#include <osgViewer/Viewer>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIActionAdapter>

#include <visual_util.h>

#include <string>
#include <iostream>
#include <fstream>

// XML Parser
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


namespace osgVisual
{


//class CameraConfigParser;

/**
 * \brief This class distorts and blends the rendered image for projection in curved screens and multi channel setups.
 * 
 * @todo Distortion algorithm seems to be erronous, small artefact lines are visible.
 * 
 * @author Torben Dannhauer
 * @date  Jul 2009
 */ 
class visual_distortion : public osg::Node
{
	#include <leakDetection.h>
public:
	/**
	 * \brief Constructor
	 * 
	 */ 
	visual_distortion(osgViewer::Viewer* viewer_, osg::ArgumentParser& arguments_, std::string configFileName);

	/**
	 * \brief Destructor
	 * 
	 */ 
	virtual ~visual_distortion(void);

	/**
	 * \brief This function parses the XML config file for distortion relevant parameters.
	 * 
	 * @return : True if parsing was successful.
	 */ 
	bool processXMLConfiguration(); 

	/**
	 * \brief This function initializes the distortion node and creates the distortion camera and scene graph.
	 * 
	 * @param subgraph : Undistorted scene graph.
	 * @param clearColor : Clear color for distortion.
	 * @return : Group node which contains the distortion and the undistorted scene graph ( distortedGraph ).
	 */ 
	osg::Group* initialize(osg::Group* subgraph, const osg::Vec4& clearColor );

	/**
	 * \brief This function shuts the distortion module down.
	 * 
	 */ 
	void shutdown();

	/**
	 * \brief This function enabels or disables the distortion
	 */ 
	void toggleDistortion();

	/**
	 * \brief This functions returns a pointer to the scene camera, which renders the undistorted scene (PRE_RENDER).
	 * 
	 * @return Pointer to the scene camera.
	 */ 
	osg::Camera* getSceneCamera() {return sceneCamera;};

	/**
	 * \brief This function returns the distortedSceneGraph if the distortion module is initialized.
	 * 
	 * @return NULL if not Initialized, Otherwith the distorted sceneGraph.
	 */ 
	osg::Group* getDistortedSceneGraph() { if(initialized) return distortedGraph; else return NULL;};

	/**
	 * \brief The function returns the frustum values which are parsed by the nested cameraConfigParser class.
	 * 
	 * @return : Vector of double values which represent the frustum dataset. The order is the standart glu order.
	 */ 
	std::vector<double> getFrustumDataset() {return parser->getFrustumDataset();};
			
	/**
	 * \brief This function returns the retation values for the rendering camera for XYZ axis in degree.
	 * 
	 * @return : Vector of double values which represent the rotation values for XYZ axis in degree.
	 */ 
	std::vector<double> getRotationDataset() {return parser->getRotationDataset();};

	/**
	 * \brief This function returns the translation values for the rendering camera along the XYZ axis in meter.
	 * 
	 * @return : Vector of double values which represent the translation values for XYZ axis in meter.
	 */ 
	std::vector<double> getTranslationDataset() {return parser->getTranslationDataset();};

private:
	/**
	 * \brief Callback class for update callback.
	 * 
	 * This nested class is used for updating the distortion node every frame by installing this class as updateCallback for the distortion node.
	 * @author Torben Dannhauer
	 * @date  Jul 2009
	 */ 
	class distortionUpdateCallback : public osg::NodeCallback
	{
	public: 
		/**
		 * \brief Constructor, for setting the member variables.
		 * 
		 * @param viewer_ : Pointer to the viewer.
		 * @param sceneCamera_ : Pointer to the scene camera.
		 */ 
		distortionUpdateCallback(osgViewer::Viewer* viewer_, osg::Camera* sceneCamera_)
			: viewer(viewer_), sceneCamera(sceneCamera_) {};

		/**
		 * \brief This function is executed as callback during update traversal.
		 * 
		 */ 
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	private:
		/**
		 * Referenced Pointer to the applications viewer.
		 */ 
		osg::ref_ptr<osgViewer::Viewer> viewer;

		/**
		 * Referenced Pointer to the scene camera.
		 */ 
		osg::ref_ptr<osg::Camera> sceneCamera;
	};

	/**
	 * \brief This nested class is a keyboard event adapter which allows distortion toggle. It never blocks the key event, therefore multiple events on a single key are possible.
	 * 
	 * @author Torben Dannhauer
	 * @date  Jul 2009
	 */ 
	class ToggleDistortionKBEventHandler : public osgGA::GUIEventHandler
	{
	public:
		/**
		 * \brief Constructor
		 * 
		 * @param distortion_ : Pointer to the distortion class.
		 * @param viewer_ : Reference to the applications viewer.
		 */ 
		ToggleDistortionKBEventHandler(visual_distortion* distortion_): distortion(distortion_) {}

		/**
		 * \brief This function is executed if an keyboard event appears
		 * 
		 * @param ea : GUI event adapter.
		 * @param : GUI action adapter.
		 * @return : Returns always false to pass the event to other event adapter if installed.
		 */    
		virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
		{
			if ( ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN &&  ea.getKey() == 'd' )
			{
				distortion->toggleDistortion();
				return false;
			}
			else
				return false;	// return false means: hand over the event to the next Eventhandler.  True means: absorb the event.
		}

	private:
		/**
		 * Pointer to he distortion class. INside this EventHandler a Referenced Pointer ist _NOT_ used, 
		 * because a circulare reference (distortion->kBEventHandler, kbEventHandler->distortion) would block both pointer to unref.
		 */ 
		visual_distortion* distortion;
	};

	/**
	 * \brief This nested class reads the Projection Designer's project file and extracts the frustum, translation and rotation values
	 * 
	 * @author Torben Dannhauer
	 * @date  Aug 2009
	 */ 
	class CameraConfigParser : public osg::Referenced
	{
	public:
		CameraConfigParser(void)
		{
			configParsed = false;
			frustumValues.clear();
			rotationValues.clear();
			rotationValues.resize(3,0);
			translationValues.clear();
			translationValues.resize(3,0);
		}

		virtual ~CameraConfigParser(void) {};

		bool parseConfigFile(const char * filename)
		{
		  if (! osgDB::fileExists(filename) )
		  {
			  OSG_NOTIFY( osg::FATAL ) << "ERROR: File '" << filename << "' does not exist." << std::endl;
			  return false;
		  }

		  frustumValues.clear();
		  rotationValues.clear();
		  rotationValues.resize(3,0);
		  translationValues.clear();
		  translationValues.resize(3,0);

		  std::string line;
		  std::ifstream configFile (filename);

		  if (configFile.is_open())
		  {
			while (! configFile.eof() )
			{
			  getline (configFile,line);
			  //cout << line << endl;

			  // Auf Frustum untersuchen
			  if( line.find("Frustum") != std::string::npos )
			  {
				  extractFrustum(line);
			  }

			  // Auf Rotation untersuchen
			  if( line.find("Rotate") != std::string::npos )
			  {
				 extractRotation(line);
			  }

			  // Auf Translation untersuchens
			  if( line.find("Translate") != std::string::npos )
			  {
				 extractTranslation(line);
			  }

			}
			configFile.close();
			configParsed = true;
			return true;
		  }
		  else 
		  {
			  OSG_NOTIFY( osg::FATAL ) << "Unable to open file:" << filename << std::endl;
			  frustumValues.clear();
			  configParsed = false;
			  return false;
		  }
		}

		bool extractFrustum(std::string data_)
		{
			data_.replace(0,16,"");		// Delete leading spaces and "Frustum"
			data_.replace(data_.length()-1,1,"");	// Delete trailing ";"
			for( int i=0; i<6; i++)
			{
				size_t pos = data_.find(" ");    // Position of first value in string
				if( pos == -1) pos = data_.length();
				frustumValues.push_back( atof( data_.substr(0, pos).data() ) );
				data_.replace(0,pos+1,"");
			}
			return true;
		}

		bool extractRotation(std::string data_)
		{
			data_.replace(0,15,"");		// Delete leading spaces and "Rotate"
			data_.replace(data_.length()-1,1,"");	// Delete trailing ";"

			size_t valuePos = data_.find(" ");    // First Value is RotationValue
			std::string axis = data_.substr( valuePos+1);
			switch(axis.find("1"))
			{
				case 0: rotationValues[0] = atof( data_.substr(0, valuePos).data() ); // X-Axis
					break;
				case 2: rotationValues[1] = atof( data_.substr(0, valuePos).data() ); // Y-Axis
					break;
				case 4: rotationValues[2] = atof( data_.substr(0, valuePos).data() ); // Z-Axis
					break;
			};
			return true;
		}

		bool extractTranslation(std::string data_)
		{
			data_.replace(0,18,"");		// Delete leading spaces and "Translate"
			data_.replace(data_.length()-1,1,"");	// Delete trailing ";"
			for( unsigned int i=0;i<translationValues.size(); i++)
			{
				size_t valuePos = data_.find(" ");
				std::string value = data_.substr(0, valuePos);
				data_.replace(0,valuePos+1,"");
				translationValues[i] = atof( value.data() );
			}
			return true;
		}

		std::vector<double> getFrustumDataset() {return(frustumValues);}
			
		std::vector<double> getRotationDataset() {return(rotationValues);}

		std::vector<double> getTranslationDataset() {return(translationValues);}

		bool isConfigParsed() {return configParsed;};
	private:
		bool configParsed;
		std::vector<double> frustumValues;
		std::vector<double> rotationValues;
		std::vector<double> translationValues;
	};



	/**
	 * Updatecallback as member variable of this class.
	 */ 
	osg::ref_ptr<distortionUpdateCallback> updateCallback;

	/**
	 * Keyboard Eventhandler for distortion toggle.
	 */ 
	//osg::ref_ptr<ToggleDistortionKBEventHandler> kBEventHandler;
	ToggleDistortionKBEventHandler* kBEventHandler;

	/**
	 * \brief This functions creates the distortion environment.
	 * 
	 * @param subgraph : Undistorted scene graph to distort.
	 * @param clearColor : OpenGL clear color to use for the distortion.
	 * @return : Group node which contains the distortion and the undistorted scene graph.
	 */ 
	osg::Group* createPreRenderSubGraph(osg::Group* subgraph, const osg::Vec4& clearColor );

	/**
	 * \brief This function loads a shader source file
	 * 
	 * @param shader : Shader to load source in
	 * @param fileName : Filename of shader source file.
	 * @return : Returns true if successful.
	 */ 
	bool loadShaderSource( osg::Shader* shader, const std::string& fileName );

	/**
	 * \brief This function loads a shader Texture file
	 * 
	 * @param fileName : Texture file to load.
	 * @return : Returns the loaded Texture.
	 */ 
	osg::Texture* loadTexture( const std::string& fileName );
	
	/**
	 * Referenced Pointer to the loaded distortion Map
	 */ 
	osg::ref_ptr<osg::Texture> distortMapTexture;

	/**
	 * Referenced Pointer to the loaded blend map
	 */ 
	osg::ref_ptr<osg::Texture> blendMapTexture;


	/**
	 * Flag which indicates the initialization status
	 */ 
	bool initialized;

	/**
	 * Flag that indicates if distortion is actually enabled. This is imported for the toggleDistortion() function.
	 */ 
	bool distortionEnabled;

	/**
	 * Reference pointer to the distorted scene graph.
	 */ 
	osg::ref_ptr<osg::Group> distortedGraph;

	/**
	 * Reference pointer to the undistorted scene graph.
	 */ 
	osg::ref_ptr<osg::Group> cleanGraph;

	/**
	 * Reference pointer to the scen camera (PRE_RENDER)
	 */ 
	osg::ref_ptr<osg::Camera> sceneCamera;

	/**
	 * Referenced pointer to the main applications viewer
	 */ 
	osg::ref_ptr<osgViewer::Viewer> viewer;

	/**
	 * Parser object for parsing the camera configuration file which controlls frustum and camera PAT
	 */ 
	osg::ref_ptr<CameraConfigParser> parser;

	/**
	 * Texture width for render to texture (RTT).
	 */ 
	unsigned tex_width;

	/**
	 * Texture height for using render to texture (RTT).
	 */ 
    unsigned tex_height;

	/**
	 * Indicates of texture rectangle should be used.
	 */ 
	bool useTextureRectangle;

	/**
	 * Indicates if shader distortion should be used.
	 */ 
	bool useShaderDistortion;

	/**
	 * Indicates if high definition rendering shold be used.
	 */ 
	bool useHDR;

	/**
	 * Filename of the distorion map.
	 */ 
	std::string distortMapFileName;

	/**
	 * Filename of the blend map.
	 */ 
	std::string blendMapFileName;

	/**
	 * XML config filename
	 */ 
	std::string configFileName;

	/**
	 * Target implementation which should be used to distort.
	 */ 
	osg::Camera::RenderTargetImplementation renderImplementation;

	/**
	 * Reference to the global ArgumentParser. The arguments are required to add the help entry for the toggle distortion key. All other configuration is located in the XML file.
	 */ 
	osg::ArgumentParser& arguments;
};

}	// END NAMESPACE
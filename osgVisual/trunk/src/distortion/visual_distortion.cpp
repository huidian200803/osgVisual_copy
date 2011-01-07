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

#include <visual_distortion.h>

using namespace osgVisual;

visual_distortion::visual_distortion(osgViewer::Viewer* viewer_, osg::ArgumentParser& arguments_, std::string configFileName) : viewer(viewer_), arguments(arguments_)
{
	OSG_NOTIFY (osg::ALWAYS ) << "visual_distortion instantiated." << std::endl;

	this->configFileName = configFileName;
	initialized = false;
	distortionEnabled = false;
	parser = new CameraConfigParser();

	distortedGraph = NULL;
	cleanGraph = NULL;

	distortMapTexture = NULL;
	blendMapTexture = NULL;
}

visual_distortion::~visual_distortion(void)
{
	OSG_NOTIFY (osg::ALWAYS ) << "visual_distortion destroyed." << std::endl;
}

bool visual_distortion::processXMLConfiguration()
{
	// Init XML
	xmlDoc* tmpDoc;
	bool disabled;
	xmlNode* config = util::getModuleXMLConfig( configFileName, "distortion", tmpDoc, disabled );

	if( disabled)
	{
		OSG_NOTIFY( osg::ALWAYS ) << "..disabled by XML configuration file." << std::endl;
		return false;
	}
	
	// extract configuration values
	if(config)
	{
		OSG_NOTIFY( osg::ALWAYS ) << "..using distortion." << std::endl;

		xmlNode* a_node = config->children;

		for (xmlNode *cur_node = a_node; cur_node; cur_node = cur_node->next)
		{
			std::string node_name=reinterpret_cast<const char*>(cur_node->name);
			//OSG_ALWAYS << "----visual_distortion::processXMLConfiguration() - node type="<< cur_node->type <<", name=" << cur_node->name << std::endl;

			// Check for distortion node
			if(cur_node->type == XML_ELEMENT_NODE && node_name == "distortion")
			{
				xmlAttr  *attr = cur_node->properties;
				while ( attr ) 
				{ 
					std::string attr_name=reinterpret_cast<const char*>(attr->name);
					std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
					if( attr_name == "channelname" )
					{
						std::string  channelname = attr_value;
						OSG_NOTIFY(osg::ALWAYS) << "visual_distortion: Channelname:" << channelname << std::endl;

						std::string pre_cfg("..\\resources\\distortion\\view_");
						std::string pre_distortion("..\\resources\\distortion\\distort_");
						std::string pre_blend("..\\resources\\distortion\\blend_");
						std::string post_cfg(".cfg");
						std::string post_distortion(".bmp");
						std::string post_blend(".bmp");

						// load channel config
						parser->parseConfigFile((pre_cfg+channelname+post_cfg).data());
						// load channel blendmap
						blendMapFileName = (pre_blend+channelname+post_blend).data();
						// load channel distortionmap
						distortMapFileName = (pre_distortion+channelname+post_distortion).data(); 
						// set channel frustum
						if( parser->isConfigParsed())
 							viewer->getCamera()->setProjectionMatrixAsFrustum((parser->getFrustumDataset())[0], (parser->getFrustumDataset())[1], (parser->getFrustumDataset())[2], (parser->getFrustumDataset())[3], (parser->getFrustumDataset())[4], (parser->getFrustumDataset())[5]);
						else
							OSG_NOTIFY(osg::WARN) << "WARNING: Unable to parse Frustum values from '" << pre_cfg<<channelname<<post_cfg << "' -- continue without valid frustum values." << std::endl;
					}
					if( attr_name == "renderimplemmentation" )
					{
						if(attr_value=="fbo")
							renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
						if(attr_value=="pbuffer")
							renderImplementation = osg::Camera::PIXEL_BUFFER;
						if(attr_value=="pbuffer-rtt")
							renderImplementation = osg::Camera::PIXEL_BUFFER_RTT;
						if(attr_value=="fb")
							renderImplementation = osg::Camera::FRAME_BUFFER;
						if(attr_value=="window")
							renderImplementation = osg::Camera::SEPERATE_WINDOW;           
					}
					if( attr_name == "width" )
					{
						std::stringstream sstr(attr_value);
						sstr >> tex_width;
					}
					if( attr_name == "height" )
					{
						std::stringstream sstr(attr_value);
						sstr >> tex_height;
					}
					if( attr_name == "useshader" )
						useShaderDistortion = (attr_value == "yes") ? true : false;
					if( attr_name == "hdr" )
						useHDR = (attr_value == "yes") ? true : false;
					if( attr_name == "usetexturerectangle" )
						useTextureRectangle = (attr_value == "yes") ? true : false;


					attr = attr->next; 
				}	// WHILE attrib END
			}	// IF Node == distortion END

			// Check for distortionmap node
			if(cur_node->type == XML_ELEMENT_NODE && node_name == "distortionmap")
			{
				xmlAttr  *attr = cur_node->properties;
				while ( attr ) 
				{ 
					std::string attr_name=reinterpret_cast<const char*>(attr->name);
					std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
					if( attr_name == "filename" )
					{
						distortMapFileName = attr_value;
					}
					attr = attr->next; 
				}
			}

			// Check for distortionmap node
			if(cur_node->type == XML_ELEMENT_NODE && node_name == "blendmap")
			{
				xmlAttr  *attr = cur_node->properties;
				while ( attr ) 
				{ 
					std::string attr_name=reinterpret_cast<const char*>(attr->name);
					std::string attr_value=reinterpret_cast<const char*>(attr->children->content);
					if( attr_name == "filename" )
					{
						blendMapFileName = attr_value;
					}
					attr = attr->next; 
				}
			}
		}	// FOR all nodes END

		// clean up
		xmlFreeDoc(tmpDoc); xmlCleanupParser();
		return true;
	}	// IF Config valid END
	else
	{
		OSG_WARN << "ERROR: visual_distortion::processXMLConfiguration() - Module configuration not found" << std::endl;
		return false;
	}

	return true;
}

osg::Group* visual_distortion::initialize(osg::Group* subgraph, const osg::Vec4& clearColor )
{
	OSG_NOTIFY (osg::ALWAYS ) << "visual_distortion initialize..";  // The sentence is finished by the init result...

	// Initialize variables:
	tex_width = 2048;
    tex_height = 2048;
	renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
	useTextureRectangle = false;
	useShaderDistortion = false;
	useHDR = false; 
	distortMapFileName = "..\\resources\\distortion\\distort_distDisabled.bmp";
	blendMapFileName = "..\\resources\\distortion\\blend_distDisabled.bmp";

	// Process XML configuration
	if(!processXMLConfiguration())
		return NULL;	// Abort distortion initialization.

	// Add this node to the scenegraph to get updated during update traversal.
	subgraph->addChild( this );

	// add the keyboard handler for toggle distortion
	arguments.getApplicationUsage()->addKeyboardMouseBinding("d", "Toggle Distortion.");
	kBEventHandler = new ToggleDistortionKBEventHandler( this );
	viewer->addEventHandler( kBEventHandler );

	cleanGraph = subgraph;
	distortedGraph = createPreRenderSubGraph( subgraph, clearColor );

	// Create and install updateCallback (to get called for copying the main cameras view matrixes to the PRE_RENDER camera)
	// -- must be called _AFTER_ createPreRenderSubGraph() (necessary because sceneCamera is set by createPreRenderSubGraph())
	updateCallback = new distortionUpdateCallback( viewer, sceneCamera );
	this->setUpdateCallback( updateCallback );

	// Note down state flags..
	initialized = true;
	distortionEnabled = true;

	distortionEnabled = true;
	viewer->setSceneData( distortedGraph );
	
	return distortedGraph;
}

void visual_distortion::shutdown()
{
	if(initialized)
	{
		// Remove this Node from scenegraph
		cleanGraph->removeChild( this );

		// Remove update callback
		this->removeUpdateCallback( updateCallback );
		updateCallback = NULL;

		OSG_NOTIFY (osg::ALWAYS ) << "visual_distortion shutdown complete." << std::endl;
	}
}

void visual_distortion::distortionUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	// Copy Main Camera's matrixes to the PRE_RENDER Camera.
	//std::cout << "distortion updatecallback" << std::endl;
	sceneCamera->setViewMatrix( viewer->getCamera()->getViewMatrix() );
	sceneCamera->setProjectionMatrix( viewer->getCamera()->getProjectionMatrix() );
}

void visual_distortion::toggleDistortion()
{
	// Toggle
	//// If not Distorted: enabled distortion
	if( !distortionEnabled )
	{
		distortionEnabled = true;
		viewer->setSceneData( distortedGraph );
	}
	else	// .. otherwise disable distortion
	{
		distortionEnabled = false;
		viewer->setSceneData( cleanGraph );
	}
}


osg::Group* visual_distortion::createPreRenderSubGraph(osg::Group* subgraph, const osg::Vec4& clearColor )
{
    if (!subgraph) return 0;

    // create a group to contain the flag and the pre rendering camera.
    osg::Group* parent = new osg::Group;

    // texture to render to and to use for rendering of flag.
    osg::Texture* texture = 0;
    if (useTextureRectangle)
    {
        osg::TextureRectangle* textureRect = new osg::TextureRectangle;
        textureRect->setTextureSize(tex_width, tex_height);
        textureRect->setInternalFormat(GL_RGBA);
        textureRect->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
        textureRect->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
        
        texture = textureRect;
    }
    else
    {
        osg::Texture2D* texture2D = new osg::Texture2D;
        texture2D->setTextureSize(tex_width, tex_height);
        texture2D->setInternalFormat(GL_RGBA);
        texture2D->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
        texture2D->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
        
        texture = texture2D;
    } 

    if (useHDR)
    {
        texture->setInternalFormat(GL_RGBA16F_ARB);
        texture->setSourceFormat(GL_RGBA);
        texture->setSourceType(GL_FLOAT);
    }

    // load a distortion map texture file
	if ( osgDB::fileExists( distortMapFileName ) )
		distortMapTexture = loadTexture(distortMapFileName);
	else
	{
		OSG_NOTIFY(osg::FATAL) << "ERROR: Distortionmap file'" << distortMapFileName << "' not found! Please change the channelname, filename or copy the desired file to the applications root folder!" << std::endl;
		exit(-1);
	}

    // load a blend map texture file
	if ( osgDB::fileExists( blendMapFileName ) )
		blendMapTexture = loadTexture(blendMapFileName);
	else
	{
		OSG_NOTIFY(osg::FATAL) << "ERROR: Blendmap file'" << blendMapFileName << "' not found! Please change the channelname, filename or copy the desired file to the applications root folder!" << std::endl;
		exit(-1);
	}

    // set up the plane to render the rendered view.
    {
        // create the quad to visualize.
        osg::Geometry* polyGeom = new osg::Geometry();

        polyGeom->setSupportsDisplayList(false);

        osg::Vec3 origin(0.0f,0.0f,0.0f);
        osg::Vec3 xAxis(1.0f,0.0f,0.0f);
        osg::Vec3 yAxis(0.0f,1.0f,0.0f);
        osg::Vec3 zAxis(0.0f,0.0f,1.0f);
        float height = 1024.0f;
        float width = 1280.0f;
        int noSteps = 128;
        if (useShaderDistortion)
            noSteps = 2;

        osg::Vec3Array* vertices = new osg::Vec3Array;
        osg::Vec2Array* texcoords = new osg::Vec2Array;
        osg::Vec2Array* texcoords2 = useShaderDistortion?NULL:(new osg::Vec2Array);
        osg::Vec4Array* colors = new osg::Vec4Array;

        osg::Vec3 bottom = origin;
        osg::Vec3 dx = xAxis*(width/((float)(noSteps-1)));
        osg::Vec3 dy = yAxis*(height/((float)(noSteps-1)));

        osg::Vec2 bottom_texcoord(0.0f,0.0f);
        osg::Vec2 dx_texcoord(1.0f/(float)(noSteps-1),0.0f);
        osg::Vec2 dy_texcoord(0.0f,1.0f/(float)(noSteps-1));

        osg::Vec3 cursor = bottom;
        osg::Vec2 texcoord = bottom_texcoord;
        int i,j;
        for(i=0;i<noSteps;++i)
        {
            osg::Vec3 cursor = bottom+dy*(float)i;
            osg::Vec2 texcoord = bottom_texcoord+dy_texcoord*(float)i;
            for(j=0;j<noSteps;++j)
            {
                vertices->push_back(cursor);
                if (distortMapTexture && !useShaderDistortion)
                {
                    osg::Vec2 imgcoord = osg::Vec2((distortMapTexture->getImage(0)->s()-1) * texcoord.x(),
                                                   (distortMapTexture->getImage(0)->t()-1) * texcoord.y());
                    unsigned char* pPixel = &distortMapTexture->getImage(0)->data()[(int)imgcoord.y()*distortMapTexture->getImage(0)->getRowSizeInBytes()+
                                                                                    (int)imgcoord.x()*distortMapTexture->getImage(0)->getPixelSizeInBits()/8];
                    texcoords->push_back(osg::Vec2(((float)pPixel[2] / 255.0f + (pPixel[0] % 16)) / 16.0f,
                                                   1.0f - ((float)pPixel[1] / 255.0f + (pPixel[0] / 16)) / 16.0f));
                    texcoords2->push_back(osg::Vec2(texcoord.x(), texcoord.y()));
                }
                else
                    texcoords->push_back(osg::Vec2(texcoord.x(), texcoord.y()));
                colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

                cursor += dx;
                texcoord += dx_texcoord;
            }
        }

        // pass the created vertex array to the points geometry object.
        polyGeom->setVertexArray(vertices);

        polyGeom->setColorArray(colors);
        polyGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        polyGeom->setTexCoordArray(0,texcoords);
        if (!useShaderDistortion)
            polyGeom->setTexCoordArray(2,texcoords2);

        for(i=0;i<noSteps-1;++i)
        {
            osg::DrawElementsUShort* elements = new osg::DrawElementsUShort(osg::PrimitiveSet::QUAD_STRIP);
            for(j=0;j<noSteps;++j)
            {
                elements->push_back(j+(i+1)*noSteps);
                elements->push_back(j+(i)*noSteps);
            }
            polyGeom->addPrimitiveSet(elements);
        }

        // new we need to add the texture to the Drawable, we do so by creating a 
        // StateSet to contain the Texture StateAttribute.
        osg::StateSet* stateset = polyGeom->getOrCreateStateSet();
        stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
        stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

        osg::Geode* geode = new osg::Geode();
        geode->addDrawable(polyGeom);

        if (useShaderDistortion)
        {
            // apply the distortion map texture
            if (distortMapTexture)
                geode->getOrCreateStateSet()->setTextureAttributeAndModes(1, distortMapTexture, osg::StateAttribute::ON);
        }

        // apply the blend map texture
        if (blendMapTexture)
            geode->getOrCreateStateSet()->setTextureAttributeAndModes(2, blendMapTexture, osg::StateAttribute::ON);

        // set up the camera to render the textured quad
        osg::Camera* camera = new osg::Camera;

        // just inherit the main cameras view
        camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        camera->setViewMatrix(osg::Matrix::identity());
        //sceneCamera->setProjectionMatrixAsOrtho2D(0,viewer->getCamera()->getViewport()->width(),0,viewer->getCamera()->getViewport()->height());
		camera->setProjectionMatrixAsOrtho2D(0,1280,0,1024);
		/** \todo: use screen size dynamically */

        // set the camera to render before the main camera.
        camera->setRenderOrder(osg::Camera::POST_RENDER, 200);

        // only clear the depth buffer
        camera->setClearMask(0);

        // add subgraph to render
        camera->addChild(geode);

        if (useShaderDistortion)
        {
            // create shaders for distortion
            osg::StateSet* ss = geode->getOrCreateStateSet();

            osg::Program* distortProgram = new osg::Program;
            distortProgram->setName( "distortion" );
            osg::Shader* distortVertObj = new osg::Shader( osg::Shader::VERTEX );
            osg::Shader* distortFragObj = new osg::Shader( osg::Shader::FRAGMENT );

            if (loadShaderSource( distortVertObj, "../resources/distortion/shader.vert" ) &&
                loadShaderSource( distortFragObj, "../resources/distortion/shader.frag" ) &&
                distortMapTexture)
            {
                distortProgram->addShader( distortFragObj );
                distortProgram->addShader( distortVertObj );
                ss->addUniform( new osg::Uniform("textureImage", 0) );
                ss->addUniform( new osg::Uniform("textureDistort", 1) );
                ss->addUniform( new osg::Uniform("textureBlend", 2) );
                ss->setAttributeAndModes(distortProgram, osg::StateAttribute::ON);
            }
			else
			{
				OSG_NOTIFY(osg::FATAL) << "##################################" << std::endl << "## Unable to activate ShaderDistortion!" << std::endl << "## ABORT" << std::endl << "##################################" << std::endl;
				exit(0);
			}
        }

        parent->addChild(camera);
    }

    // then create the camera node to do the render to texture
    {    
        osg::Camera* camera = new osg::Camera;

        // set up the background color and clear mask.
        camera->setClearColor(clearColor);
        camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // just inherit the main cameras view
		/* ABSOLUTE_RF required to make intersections possible.
		Disadvantage of ABOLUTE_RF : the maincameras view matrix and projection
		matrix has to copied to the PRE_RENDER camera by our self.
		Therefore an update callback is installed.
		*/
		camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);	
        camera->setProjectionMatrix(osg::Matrixd::identity());
        camera->setViewMatrix(osg::Matrixd::identity());

        // set viewport
        camera->setViewport(0,0,tex_width,tex_height);

        // set the camera to render before the main camera.
        camera->setRenderOrder(osg::Camera::PRE_RENDER, 0);

		sceneCamera = camera;

        // tell the camera to use OpenGL frame buffer object where supported.
        camera->setRenderTargetImplementation(renderImplementation);

        // attach the texture and use it as the color buffer.
        camera->attach(osg::Camera::COLOR_BUFFER, texture);	// No Multisampling/Antialiasing
		//camera->attach(osg::Camera::COLOR_BUFFER, texture, 0, 0, false, 4, 4); // 4x Multisampling/Antialiasing

        // add subgraph to render
        camera->addChild(subgraph);

        parent->addChild(camera);
    }    
    return parent;
}


bool visual_distortion::loadShaderSource( osg::Shader* shader, const std::string& fileName )
{
    std::string foundFileName = osgDB::findDataFile(fileName);
    if (foundFileName.length() != 0 )
    {
        return shader->loadShaderSourceFromFile( foundFileName.c_str() );
    }

    OSG_NOTIFY(osg::WARN) << "File \"" << fileName << "\" not found." << std::endl;

    return false;
}

osg::Texture* visual_distortion::loadTexture( const std::string& fileName )
{
    std::string foundFileName = osgDB::findDataFile(fileName);
    if (foundFileName.length() != 0 )
    {
        // load distortion map texture file
        osg::Image* image = osgDB::readImageFile(foundFileName);
        if (image)
        {
			osg::Texture2D* texture = new osg::Texture2D;
            texture->setImage(image);
            texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
            texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);
            texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
            texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
            return texture;
        }
    }

    OSG_NOTIFY(osg::WARN) << "File \"" << fileName << "\" not found." << std::endl;

    return NULL;
}
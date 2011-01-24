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

#include <visual_object.h>

using namespace osgVisual;

visual_object::visual_object( osg::CoordinateSystemNode* sceneRoot_, std::string nodeName_)
{
	// Add this node to Scenegraph
	sceneRoot_->addChild( this );

	// Set Nodename for further identification
	this->setName( nodeName_ );

	// Set callback.
	/** \todo: welcher update ist der richtige? voraussichtlich event.) */
	//this->setUpdateCallback( new visual_objectPositionCallback() );
	this->setEventCallback( new visual_objectPositionCallback() );

	// Init Position and Attitude
	lat = 0;
	lon = 0;
	alt = 0;

	azimuthAngle_psi = 0;
	pitchAngle_theta = 0;
	bankAngle_phi = 0;

	geometry_offset_rotation.makeRotate( 0.0, 1.0, 1.0, 1.0 );

	// Init Scale factor
	scaleX = 1.0;
	scaleY = 1.0;
	scaleZ = 1.0;

	// Init cameraOffset 
	cameraTranslationOffset.makeTranslate( osg::Vec3d(0.0, 0.0, 0.0) );	// Trans: (y, x, -z_down)
	cameraRotationOffset.makeRotate( osg::DegreesToRadians( 90.0 ), osg::Vec3(1, 0, 0) );	// Rot: (-y, +x , -z)

	// Geometrynode hinzufügen
	geometry = new osg::Group();
	this->addChild( geometry );

	// Labelnode hinzufügen
	labels = new osg::Geode();
	this->addChild( labels ); 
}

visual_object::~visual_object()
{

}

visual_object* visual_object::createNodeFromXMLConfig(osg::CoordinateSystemNode* sceneRoot_, xmlNode* a_node)
{
	if(a_node == NULL)
		return NULL;

	OSG_NOTIFY( osg::ALWAYS ) << __FUNCTION__ << "Try to creating a new Model.." << std::endl;
	//osg::ref<visual_object> object = new visual_object( root, nodeName);

	std::string objectname="", filename="", label="";
	bool dynamic = false;
	double lat=0.0, lon=0.0, alt=0.0, rot_x=0.0, rot_y=0.0, rot_z=0.0;
	double cam_trans_x=0.0, cam_trans_y=0.0, cam_trans_z=0.0, cam_rot_x=0.0, cam_rot_y=0.0, cam_rot_z=0.0;
	double geometry_rot_x=0.0, geometry_rot_y=0.0, geometry_rot_z=0.0;
	double geometry_scale_x=1.0, geometry_scale_y=1.0, geometry_scale_z=1.0;
	osg::ref_ptr<osgVisual::object_updater> updater = NULL;

	osgVisual::visual_object* object = new osgVisual::visual_object( sceneRoot_, objectname );
	object->lat = lat;
	object->lon = lon;
	object->alt = alt;
	object->azimuthAngle_psi = rot_x;
	object->pitchAngle_theta = rot_y;
	object->bankAngle_phi = rot_z;
	if(label!="")
		object->addLabel("XML_defined_label", label);
	if(dynamic)
	{
		updater = new osgVisual::object_updater(object);
	}
	object->setCameraOffset( cam_trans_x, cam_trans_y, cam_trans_z, cam_rot_x, cam_rot_y, cam_rot_z);
	if(filename!="")
	{
		object->loadGeometry( filename );
		object->setGeometryOffset( geometry_rot_x, geometry_rot_y, geometry_rot_z );
		object->setScale( geometry_scale_x, geometry_scale_y, geometry_scale_z ); 
	}

	if(updater.valid())
 		object->addUpdater( updater );	

	/*
        - updater [optional]: Channels to use for position and attitude update

        <updater>
          <translation-slots>todo</translation-slots>
        </updater>
    */

	OSG_NOTIFY( osg::ALWAYS ) << "Done." << std::endl;
	return object;
}

void visual_object::setNewPositionAttitude( double lat_, double lon_, double alt_, double azimuthAngle_psi_, double pitchAngle_theta_, double bankAngle_phi_ )
{
	lat = lat_;
	lon = lon_;
	alt = alt_;

	azimuthAngle_psi = azimuthAngle_psi_;
	pitchAngle_theta = pitchAngle_theta_;
	bankAngle_phi = bankAngle_phi_;
}

void visual_object::setNewPosition( double lat_, double lon_, double alt_ )
{
	lat = lat_;
	lon = lon_;
	alt = alt_;
}

void visual_object::setNewAttitude( double azimuthAngle_psi_, double pitchAngle_theta_, double bankAngle_phi_ )
{
	azimuthAngle_psi = azimuthAngle_psi_;
	pitchAngle_theta = pitchAngle_theta_;
	bankAngle_phi = bankAngle_phi_;
}

void visual_object::setGeometryOffset( double rotX_, double rotY_, double rotZ_ )
{
	geometry_offset_rotation.makeRotate( rotX_, osg::Vec3f(1.0, 0.0, 0.0), 
						rotY_, osg::Vec3f(0.0, 1.0, 0.0),
						rotZ_, osg::Vec3f(0.0, 0.0, 1.0) );
}

void visual_object::setScale( double scale_ )
{
	scaleX = scale_;
	scaleY = scale_;
	scaleZ = scale_;
}

void visual_object::setScale( double scaleX_, double scaleY_, double scaleZ_ )
{
	scaleX = scaleX_;
	scaleY = scaleY_;
	scaleZ = scaleZ_;
}

bool visual_object::loadGeometry(std::string filename_)
{
	// Check if file exists
	if( !osgDB::fileExists(filename_) )
	{
		OSG_NOTIFY(osg::FATAL) << "Error: Model not loaded. File '" << filename_ << "' does not exist." << std::endl;
	}

	osg::ref_ptr<osg::Node> tmpModel = osgDB::readNodeFile( filename_ );
	
	if( tmpModel.valid() )
	{
		// remove old geometry
		geometry->removeChildren(0, geometry->getNumChildren());

		// add new geometry
		geometry->addChild( tmpModel.get() );
		return true;
	}
	else
	{
		std::cout <<": No model loaded: " << filename_ << std::endl;
        return false;
    }
}

bool visual_object::setGeometry(osg::Node* geometry_)
{
	// remove old geometry
	geometry->removeChildren(0, geometry->getNumChildren());

	// add new geometry
	geometry->addChild( geometry_ );

	return true;
}

void visual_object::unsetGeometry()
{
	// remove old geometry
	geometry->removeChildren(0, geometry->getNumChildren());
}

void visual_object::addUpdater( object_updater* updater_ )
{
	if ( updater.valid() )
		updater->addUpdater( updater_ );
	else
		updater = updater_;
}

void visual_object::clearAllUpdater()
{
	// release only first updater. Because smartpointer: Will be deleted if not referenced.
	if ( updater.valid() )
		updater = NULL;
}

std::vector<object_updater*> visual_object::getUpdaterList()
{
	// iterate through updater and add all pointer.
	std::vector<object_updater*> updaterList;
	osg::ref_ptr<object_updater> tmpUpdater = updater;

	while (tmpUpdater.valid())
	{
		updaterList.push_back( tmpUpdater );
		tmpUpdater = tmpUpdater->getPointer();
	}

	// return list
	return updaterList;
}

void visual_object::visual_objectPositionCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	visual_object* object = dynamic_cast<visual_object*>(node);
	if ( !object )
	{
		OSG_NOTIFY(osg::FATAL) << "ERROR : No object found. Unable to apply this callback!" << std::endl;
		return;
	}

	// execute preUpdater to get new data of this object.
	if ( object->updater.valid() )
		object->updater->preUpdate(object);
    
	// Nodepath from this node to absolute parent (if no endnode specified)
	osg::NodePath nodePath = nv->getNodePath();

	// If Nodepath != empty, then mt = last element of node path
	osg::MatrixTransform* mt = nodePath.empty() ? 0 : dynamic_cast<osg::MatrixTransform*>(nodePath.back());
	if (mt)
	{
		osg::CoordinateSystemNode* csn = 0;

		// find coordinate system node from our parental chain: traverse chain and try to convert every node to a csn.
		unsigned int i;
		for(i=0; i<nodePath.size() && csn==0; ++i)	// "&& csn" means: exit loop if csn found
		{
			csn = dynamic_cast<osg::CoordinateSystemNode*>(nodePath[i]);	// dynamic_cast returns 0 if dynamic_cast fails.
		}
        
		// Wenn csn gefunden:
		if (csn)
		{
			// Ellipsoidmodel erfragen
			osg::EllipsoidModel* ellipsoid = csn->getEllipsoidModel();
			if (ellipsoid)
			{
				osg::Matrix inheritedMatrix;

				// durch den _restlichen_ Nodepath durchgehen und alle anfallenden Transformationen durchführen.
				for(i+=1; i<nodePath.size()-1; ++i)
				{
					osg::Transform* transform = nodePath[i]->asTransform();	// Versuchen, den Node zu einer Transformation zu konvertieren
                    
					// wenn Node wirklich Trafo, dann die Tranformationsmatrix von Nodekoordinaten nach Global auf inheritedMatrix draufschlagen.
					if (transform) transform->computeLocalToWorldMatrix(inheritedMatrix, nv);
				}
                
				osg::Matrixd matrix(inheritedMatrix);

				// Set position
				ellipsoid->computeLocalToWorldTransformFromLatLongHeight(object->lat, object->lon, object->alt, matrix);

				// Set Upvector for position
				double X,Y,Z;
				util::calculateXYZAtWGS84Coordinate(object->lat, object->lon, object->alt, csn, X, Y, Z );
				object->upVector = ellipsoid->computeLocalUpVector(X,Y,Z);

				// Set scale
				osg::Matrixd scaleMatrix;
				scaleMatrix.makeScale( object->scaleX, object->scaleY, object->scaleZ );
				matrix.preMult( scaleMatrix );

				// Set rotation
				// rotation von links ranmultiplizieren, entspricht: matrix = rotation * matrix. Da rotation ein Quat ist, wäre die direkte Multiplikation nur über Umwege machbar.
				// Rotate Object to Attitude.
				osg::Matrixd rotationMatrix;
				// Move Model by Azimuth
				rotationMatrix.makeRotate( -object->azimuthAngle_psi, osg::Vec3d(0.0, 0.0, 1.0) );
				matrix.preMult(rotationMatrix);	
				// Move Model by Pitch
				rotationMatrix.makeRotate( object->pitchAngle_theta, osg::Vec3d(1.0, 0.0, 0.0) );
				matrix.preMult(rotationMatrix);
				// Move Model by Bank
				rotationMatrix.makeRotate( object->bankAngle_phi, osg::Vec3d(0.0, 1.0, 0.0) );
				matrix.preMult(rotationMatrix);

				// Also update camera matrix (without geometry offset, because camera is interested in the objects matrix, not in the model's matrix.)
				object->cameraMatrix = matrix;
				/** \todo : Clean up camera matrix management: try to solve it with a single matrix. (each frame two matrix mults less) */
				// dont know, why this rotation is necessary - maybe manipulator and node MatrixTransform interpret a matrix in different way?
				object->cameraMatrix.preMult( object->cameraTranslationOffset );
				object->cameraMatrix.preMult( object->cameraRotationOffset );
							

				// Set geometry correction
				matrix.preMultRotate( object->geometry_offset_rotation );

				// Set cumulated object matrix as the matrix of this matrix transform
				mt->setMatrix(matrix);
			}
		}        
	}
      
	// Call any nested callbacks.
	traverse(node,nv);

	// If SLAVE: execute postUpdater to pass new data of this object to dataIO.
	if( visual_dataIO::getInstance()->isSlave() )
	{
		if ( object->updater.valid() )
			object->updater->postUpdate(object);
	}

}   // Callbackfunction [ Operater() ] END

void visual_object::setCameraOffsetTranslation( double x_, double y_, double z_)
{
	cameraTranslationOffset.makeTranslate( osg::Vec3d(x_, y_, z_) );	// Trans: (rechts davon, longitudinal, vertikal)
}

void visual_object::setCameraOffset(double x_, double y_, double z_, double rotX_, double rotY_, double rotZ_)
{
	setCameraOffsetTranslation( x_, y_, z_);
	setCameraOffsetRotation( rotX_, rotY_, rotZ_);
}

void visual_object::setCameraOffsetRotation(double rotX_, double rotY_, double rotZ_)
{
	osg::Matrix tmp;
	cameraRotationOffset.makeRotate( osg::DegreesToRadians( 90.0 ), osg::Vec3(1, 0, 0) );
	tmp.makeRotate( -rotZ_, osg::Vec3d(0.0, 1.0, 0.0) );
	cameraRotationOffset.preMult(tmp);
	tmp.makeRotate( rotY_, osg::Vec3d(1.0, 0.0, 0.0) );	
	cameraRotationOffset.preMult(tmp);
	tmp.makeRotate( -rotX_, osg::Vec3d(0.0, 0.0, 1.0) );	
	cameraRotationOffset.preMult(tmp);
}

void visual_object::clearLabels()
{
	labels->removeDrawables(0, labels->getNumDrawables());
}

void visual_object::addLabel(std::string idString_, std::string label_, osg::Vec4 color_, osg::Vec3 offset_)
{
	osg::ref_ptr<osgText::Text> text = new osgText::Text();

	text->setName(idString_);
	text->setText(label_);
	text->setColor(color_);
	text->setFont("fonts/arial.ttf");
	text->setCharacterSizeMode(osgText::Text::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
	text->setAutoRotateToScreen(true);
	text->setPosition(offset_);

	text->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	labels->addDrawable( text );
}

bool visual_object::removeLabel(std::string idString_)
{
	osg::Node* labelToRemove = util::findNamedNode(idString_, this);

	if(labelToRemove)
	{
		removeChild( labelToRemove );
		return true;
	}
	else
		return false;
}

bool visual_object::updateLabelText(std::string idString_, std::string label_)
{
	osg::Node* labelToUpdate = util::findNamedNode(idString_, this);

	if(labelToUpdate)
	{
		osgText::Text* text = dynamic_cast<osgText::Text*>(labelToUpdate);
		if(text)
		{
			text->setText(label_);
			return true;
		}
		return false;
	}
	return false;
}

osgText::Text* visual_object::getLabel(std::string idString_)
{
	osg::Node* labelToFind = util::findNamedNode(idString_, this);

	if(labelToFind)
	{
		osgText::Text* text = dynamic_cast<osgText::Text*>(labelToFind);
		if(text)
			return text;
	}
	return NULL;

}

bool visual_object::setDrawLabelAsOverlay(std::string idString_, bool drawAsOverlay)
{
	osg::Node* labelToFind = util::findNamedNode(idString_, this);

	if(labelToFind)
	{
		if (drawAsOverlay)
			labelToFind->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		else
			labelToFind->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
		return true;
	}
	else 
		return false;
}

bool visual_object::getDrawLabelAsOverlay(std::string idString_)
{
	osg::Node* labelToFind = util::findNamedNode(idString_, this);

	if(labelToFind)
	{
		if(labelToFind->getOrCreateStateSet()->getMode(GL_DEPTH_TEST) == osg::StateAttribute::OFF)
			return false;
		else 
			return true;
	}
	return false;
}
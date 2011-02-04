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

const unsigned int MAX_NUM_EVENTS = 10;

#ifdef _DEBUG
	#ifdef WIN32
		// Declare this in header.
		#define _CRTDBG_MAP_ALLOC
		#include <stdlib.h>
		#include <crtdbg.h>
	#endif 
#endif 


//#include <windows.h>

#include <osg/ArgumentParser>
#include <osg/Referenced>
#include <osgDB/Registry>

#include <visual_core.h>




int main(int argc, char** argv)
{
	
#ifdef _DEBUG
	#ifdef WIN32
		#include <leakDetection.h>	// for main: must be inside function. In classes: headerfile inside calss is sufficient
		int tmp_flag;

		HANDLE log_file = CreateFile("mem_log.txt", GENERIC_WRITE,FILE_SHARE_WRITE,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW |
			_CRTDBG_MODE_DEBUG);
		_CrtSetReportMode(_CRT_WARN,_CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
		_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW |
			_CRTDBG_MODE_DEBUG);

		// output to the file
		_CrtSetReportFile(_CRT_ASSERT, log_file);
		_CrtSetReportFile(_CRT_WARN, log_file);
		_CrtSetReportFile(_CRT_ERROR, log_file);

		tmp_flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		tmp_flag |= _CRTDBG_ALLOC_MEM_DF;
		tmp_flag |= _CRTDBG_DELAY_FREE_MEM_DF;
		tmp_flag |= _CRTDBG_LEAK_CHECK_DF;

		_CrtSetDbgFlag(tmp_flag);
	#endif 
#endif 

    // Use an ArgumentParser object to manage the program arguments.
	osg::ArgumentParser arguments(&argc,argv);

    // Instantiate osgVisual main class
	osg::ref_ptr<osgVisual::visual_core> core = new osgVisual::visual_core(arguments);

    // Visual initialisieren
    core->initialize();

	// Shut osgVisual down
	core->shutdown();

	// Set Pointer to null to destroy the objects before this function ends
	core = NULL;

	return 0;
}
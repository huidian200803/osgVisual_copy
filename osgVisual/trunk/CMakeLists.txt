#################################################################################################
# This file allows to configure osgVisual with CMAKE to generate native build solutions.		#
#																								#
# Specify the following environment variables to use osgVisual and this CMAKE script:			#
#	* OSG_ROOT: Path to your 32 bit (x86) OSG, e.g. D:\OSG\OpenSceneGraph-2.9.8_x86				# 
#	* OSG_ROOT_X64: Path to your 64 bit (x64) OSG, e.g. D:\OSG\OpenSceneGraph-2.9.8_x64			#
#	* OSG: Path to your 32 bit (x86) OSG and 3rdParty binary folder,							#
#		 e.g. %OSG_ROOT%\bin;D:\OSG\3rdParty\bin\x86											#
#	* OSG_X64: Path to your 64 bit (x64) OSG and 3rdParty binary folder,						#
#		 e.g. %OSG_ROOT_X64%\bin;D:\OSG\3rdParty\bin\x64										#
#	* PATH: Extend with %OSG% or %OSG_X64% to use OSG and osgVisual from command line,			#
#		 e.g. PATH=%PATH%;%OSG%																	#
#																								#
#	NOTE:	On Visual Studio and x64 Arch, CMAKE uses the project settings to specify the		#
#			correct x64 OSG path, so you can use the 32bit OSG in your system path.				#
#																								#
#																								#
#################################################################################################

cmake_minimum_required(VERSION 2.8)


PROJECT(osgVisual)
#SET(CMAKE_MODULE_PATH "${osgVisual_SOURCE_DIR}/../CMakeModules;${CMAKE_MODULE_PATH}")

# Check Architecture
IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    MESSAGE( STATUS "32 bit architecture detected" )
    SET(DESTINATION_ARCH "x86")
ENDIF()
IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( STATUS "64 bit architecture detected" )
    SET(DESTINATION_ARCH "x64")
ENDIF()


# Get required packages
IF(DESTINATION_ARCH STREQUAL "x64")
	SET(OSG_ROOT_BKP $ENV{OSG_ROOT})	# BACKUP old OSG_ROOT
	SET(ENV{OSG_ROOT} $ENV{OSG_ROOT_X64})	# Replace OSGROOT with OSG_ROOT_X64 to force CMAKE to find the correct OSG version (64 bit)
ENDIF ()

SET(OpenSceneGraph_MARK_AS_ADVANCED ON)
FIND_PACKAGE(OpenSceneGraph 2.9.9 REQUIRED osgViewer OpenThreads osgDB osgTerrain osgGA osgText osgSim osgUtil)
FIND_PACKAGE(OpenGL REQUIRED)


IF(DESTINATION_ARCH STREQUAL "x64")
	SET(ENV{OSG_ROOT} ${OSG_ROOT_BKP})	# Restore old OSG_ROOT
	UNSET(OSG_ROOT_BKP)					# Delete backup
ENDIF ()

# LIBXML2
SET(LIBXML2_LIBRARY ../../../OpenSceneGraph/3rdParty_x86_x64/${DESTINATION_ARCH}/lib/libxml2 CACHE FILEPATH "Select the libxml2 library to link osgVisual against. REQUIRED!" )
SET(LIBXML2_INCLUDE ../../../OpenSceneGraph/3rdParty_x86_x64/${DESTINATION_ARCH}/include CACHE PATH "Select the libxml2 include path to link osgVisual against. REQUIRED!" )
INCLUDE_DIRECTORIES( ${LIBXML2_INCLUDE} )	

# Module sky-silverlining
# OPTION( OPTION_VAR "description string" [initial value] )
IF ( WIN32 )
	
	SET(USE_SKY_SILVERLINING ON CACHE BOOL "Enable to use Sundog-soft's Silverlining SDK for sky & weather")
	IF(USE_SKY_SILVERLINING )
	
		SET(checkSL $ENV{SILVERLINING_PATH})
		IF(NOT checkSL)
			MESSAGE( "You have activated SilverLining [SL] in CMake, but the SL installation path could not be found! Please deactivate SL or setup it's pathes in CMake manually!" )
		ENDIF()

	
		SET(SOURCES
			${SOURCES}
			include/sky_Silverlining/skySilverLining_AtmosphereReference.h
			include/sky_Silverlining/skySilverLining_cloudLayerSlot.h
			include/sky_Silverlining/skySilverLining_cloudsDrawable.h
			include/sky_Silverlining/skySilverLining_ProjectionMatrixCallback.h
			include/sky_Silverlining/skySilverLining_skyDrawable.h
			include/sky_Silverlining/visual_skySilverLining.h
			src/sky_Silverlining/skySilverLining_cloudsDrawable.cpp
			src/sky_Silverlining/skySilverLining_skyDrawable.cpp
			src/sky_Silverlining/visual_skySilverLining.cpp			
		)
		
		IF (DESTINATION_ARCH STREQUAL "x86")
			SET(SILVERLINING_ARCH "win32")
		ELSEIF ( DESTINATION_ARCH STREQUAL "x64" )
			SET(SILVERLINING_ARCH "x64")
		ENDIF ()
		SET(SILVERLINING_LIBRARY_RELEASE $ENV{SILVERLINING_PATH}\\lib\\vc9\\${SILVERLINING_ARCH}\\SilverLining-MT-DLL.lib CACHE FILEPATH "Select the release library of Sundog-Soft's Silverlining SDK to link against." )
		SET(SILVERLINING_LIBRARY_DEBUG $ENV{SILVERLINING_PATH}\\lib\\vc9\\${SILVERLINING_ARCH}\\SilverLining-MTD-DLL.lib CACHE FILEPATH "Select the debug library of Sundog-Soft's Silverlining SDK to link against." )
		SET(SILVERLINING_INCLUDE_DIR "$ENV{SILVERLINING_PATH}\\Public Headers" CACHE PATH "Enter the path to the the public Include directory of Sundog-Soft's Silverlining SDK" )
		SET(SILVERLINING_LICENSEE "Your user name" CACHE STRING "Enter your Silverlining licensee name. If you license available, osgVisual will work with silverlining as 5 minute demo with the default value." )
		SET(SILVERLINING_LICENSE "Your license code" CACHE STRING "Enter your Silverlining license key. If you license available, osgVisual will work with silverlining as 5 minute demo with the default value." )
		INCLUDE_DIRECTORIES(${SILVERLINING_INCLUDE_DIR} include/sky_Silverlining)
		ADD_DEFINITIONS( "-DUSE_SKY_SILVERLINING" )
		ADD_DEFINITIONS( "-DSILVERLINING_LICENSEE=\"${SILVERLINING_LICENSEE}\"" )
		ADD_DEFINITIONS( "-DSILVERLINING_LICENSE=\"${SILVERLINING_LICENSE}\"" )
		
	ELSE(USE_SKY_SILVERLINING)
		UNSET(SILVERLINING_LIBRARY_RELEASE CACHE)
		UNSET(SILVERLINING_LIBRARY_DEBUG CACHE)
		UNSET(SILVERLINING_INCLUDE_DIR CACHE)
		UNSET(SILVERLINING_LICENSEE)
		UNSET(SILVERLINING_LICENSE)
	ENDIF(USE_SKY_SILVERLINING)
ENDIF(WIN32)
IF(UNIX)
	SET(USE_SKY_SILVERLINING ON CACHE BOOL "Enable to use Sundog-soft's Silverlining SDK for sky & weather")
	IF(USE_SKY_SILVERLINING )
		SET(SOURCES
			${SOURCES}
			include/sky_Silverlining/skySilverLining_AtmosphereReference.h
			include/sky_Silverlining/skySilverLining_cloudLayerSlot.h
			include/sky_Silverlining/skySilverLining_cloudsDrawable.h
			include/sky_Silverlining/skySilverLining_ProjectionMatrixCallback.h
			include/sky_Silverlining/skySilverLining_skyDrawable.h
			include/sky_Silverlining/visual_skySilverLining.h
			src/sky_Silverlining/skySilverLining_cloudsDrawable.cpp
			src/sky_Silverlining/skySilverLining_skyDrawable.cpp
			src/sky_Silverlining/visual_skySilverLining.cpp			
		)
		
		IF (DESTINATION_ARCH STREQUAL "x86")
			SET(SILVERLINING_ARCH "win32")
		ELSEIF ( DESTINATION_ARCH STREQUAL "x64" )
			SET(SILVERLINING_ARCH "x64")
		ENDIF ()
		SET(SILVERLINING_LIBRARY_RELEASE $ENV{SILVERLINING_PATH}\\lib\\vc9\\${SILVERLINING_ARCH}\\SilverLining-MT-DLL.lib CACHE FILEPATH "Select the release library of Sundog-Soft's Silverlining SDK to link against." )
		SET(SILVERLINING_LIBRARY_DEBUG $ENV{SILVERLINING_PATH}\\lib\\vc9\\${SILVERLINING_ARCH}\\SilverLining-MTD-DLL.lib CACHE FILEPATH "Select the debug library of Sundog-Soft's Silverlining SDK to link against." )
		SET(SILVERLINING_INCLUDE_DIR "$ENV{SILVERLINING_PATH}\\Public Headers" CACHE PATH "Enter the path to the the public Include directory of Sundog-Soft's Silverlining SDK" )
		SET(SILVERLINING_LICENSEE "Your user name" CACHE STRING "Enter your Silverlining licensee name. If you license available, osgVisual will work with silverlining as 5 minute demo with the default value." )
		SET(SILVERLINING_LICENSE "Your license code" CACHE STRING "Enter your Silverlining license key. If you license available, osgVisual will work with silverlining as 5 minute demo with the default value." )
		INCLUDE_DIRECTORIES(${SILVERLINING_INCLUDE_DIR} "include/sky_Silverlining" )
		ADD_DEFINITIONS( "-DUSE_SKY_SILVERLINING" )
		ADD_DEFINITIONS( "-DSILVERLINING_LICENSEE=\"${SILVERLINING_LICENSEE}\"" )
		ADD_DEFINITIONS( "-DSILVERLINING_LICENSE=\"${SILVERLINING_LICENSE}\"" )
		
	ELSE(USE_SKY_SILVERLINING)
		UNSET(SILVERLINING_LIBRARY_RELEASE CACHE)
		UNSET(SILVERLINING_LIBRARY_DEBUG CACHE)
		UNSET(SILVERLINING_INCLUDE_DIR CACHE)
		UNSET(SILVERLINING_LICENSEE)
		UNSET(SILVERLINING_LICENSE)
	ENDIF(USE_SKY_SILVERLINING)
ENDIF()

# Module distortion
SET(USE_DISTORTION OFF CACHE BOOL "Enable to use distortion with Projection Designer's distortion maps")
IF(USE_DISTORTION)
	SET(SOURCES
		${SOURCES}
		include/distortion/visual_distortion.h
		src/distortion/visual_distortion.cpp
	)
	INCLUDE_DIRECTORIES(include/distortion)
	ADD_DEFINITIONS( "-DUSE_DISTORTION" )
ENDIF(USE_DISTORTION)


# Module Vista2D - Only available under Win32
if( WIN32 )
	SET(USE_VISTA2D OFF CACHE BOOL "Enable to use Vista2D by Wetzel Technology")
	IF( USE_VISTA2D )
		INCLUDE_DIRECTORIES( include/vista2D )
		SET(SOURCES
			${SOURCES}
			include/vista2D/visual_vista2D.h
			src/vista2D/visual_vista2D.cpp	
		)
		SET(VISTA2D_INCLUDE_DIR . CACHE PATH "Select the INCLUDE directory path for Vista2D by Wetzel Technology." )
		SET(VISTA2D_LIBRARY_DEBUG . CACHE FILEPATH "Select debug LIBRARY path for linking against Vista2D by Wetzel Technology." )
		SET(VISTA2D_LIBRARY_RELEASE . CACHE FILEPATH "Select release LIBRARY path for linking against Vista2D by Wetzel Technology." )
		INCLUDE_DIRECTORIES(${VISTA2D_INCLUDE_DIR})
		ADD_DEFINITIONS( "-DUSE_VISTA2D" )
	ELSE(USE_VISTA2D)
		UNSET(VISTA2D_INCLUDE_DIR CACHE)
		UNSET(VISTA2D_LIBRARY_DEBUG CACHE)
		UNSET(VISTA2D_LIBRARY_RELEASE CACHE)
	ENDIF(USE_VISTA2D)
ENDIF(WIN32)


# Module Spacenavigator manipulator - ONLY under WIN32
if( WIN32 )
	# No header or linking required, because WIN32 COM interface used.
	SET(USE_SPACEMOUSE ON CACHE BOOL "Enable to use camera manipulator based on Spacenavigator by 3DConnexion")
	IF( USE_SPACEMOUSE )
		SET(SOURCES
			${SOURCES}
			include/manip_Spacemouse/manip_spaceMouse.h
			include/manip_Spacemouse/manip_freeSpaceMouse.h
			include/manip_Spacemouse/manip_nodeTrackerSpaceMouse.h
			src/manip_Spacemouse/manip_spaceMouse.cpp
			src/manip_Spacemouse/manip_freeSpaceMouse.cpp
			src/manip_Spacemouse/manip_nodeTrackerSpaceMouse.cpp
		)
		INCLUDE_DIRECTORIES(include/manip_Spacemouse)
		ADD_DEFINITIONS( "-DUSE_SPACENAVIGATOR" )
	ENDIF(USE_SPACEMOUSE)
ENDIF(WIN32)


# Module cluster
SET(SOURCES
	${SOURCES}
	include/cluster/dataIO_cluster.h
	include/cluster/dataIO_clusterDummy.h
	src/cluster/dataIO_clusterDummy.cpp
)
SET(USE_CLUSTER_ASIO_TCP_IOSTREAM OFF CACHE BOOL "Enable to use the Boost ASIO TCP iostream implementation for the cluster interface")
SET(USE_CLUSTER_ENET ON CACHE BOOL "Enable to use the ENet reliable UDP library implementation for the cluster interface")
IF( USE_CLUSTER_ASIO_TCP_IOSTREAM )
		SET(SOURCES
			${SOURCES}
			include/cluster/dataIO_clusterAsioTcpIostream.h
			src/cluster/dataIO_clusterAsioTcpIostream.cpp
		)
		ADD_DEFINITIONS( "-DUSE_CLUSTER_ASIO_TCP_IOSTREAM" )
		ADD_DEFINITIONS( "-D_WIN32_WINNT=0x0501" )
		SET(BOOST_LIBRARY_PATH "../../../3rdParty_x86_x64/lib/${DESTINATION_ARCH}/" CACHE PATH "Select the library folder of Boost libraries to link against." )
		SET(BOOST_INCLUDE_DIR "../../../3rdParty_x86_x64/include/boost-1.43.0/" CACHE PATH "Enter the path to the the include directory of Boost" )
		INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})
		LINK_DIRECTORIES(${BOOST_LIBRARY_PATH})
ELSE()
		UNSET(BOOST_LIBRARY_PATH CACHE)
		UNSET(BOOST_ASIO_INCLUDE_DIR CACHE)	
ENDIF()

IF( USE_CLUSTER_ENET )
		SET(SOURCES
			${SOURCES}
			include/cluster/dataIO_clusterENet.h
			src/cluster/dataIO_clusterENet.cpp
			include/cluster/dataIO_clusterENet_implementation.h
			src/cluster/dataIO_clusterENet_implementation.cpp
			src/cluster/enet/callbacks.c
			src/cluster/enet/compress.c
			src/cluster/enet/host.c
			src/cluster/enet/list.c
			src/cluster/enet/packet.c
			src/cluster/enet/peer.c
			src/cluster/enet/protocol.c
			src/cluster/enet/unix.c
			src/cluster/enet/win32.c
			include/cluster/enet/callbacks.h
			include/cluster/enet/enet.h
			include/cluster/enet/list.h
			include/cluster/enet/protocol.h
			include/cluster/enet/time.h
			include/cluster/enet/types.h
			include/cluster/enet/unix.h
			include/cluster/enet/utility.h
			include/cluster/enet/win32.h
		)
		ADD_DEFINITIONS( "-DUSE_CLUSTER_ENET" )	
ENDIF()




# Module extLink
SET(SOURCES
	${SOURCES}
	include/extLink/dataIO_extLinkDummy.h
	src/extLink/dataIO_extLinkDummy.cpp
	include/extLink/dataIO_extLink.h
	include/extLink/manip_extLink.h
	src/extLink/manip_extLink.cpp
)
IF ( WIN32 )
	SET(USE_EXTLINK_VCL ON CACHE BOOL "Enable to use the VCL Implementation for the externalLink interface")
	IF( USE_EXTLINK_VCL )
		SET(EXTLINK_VCL_INCLUDE_DIR ../../VCL_1.0.3.4/source/include/VCL CACHE PATH "Select the INCLUDE directory path for extLink VCL Implementation." )
		SET(SOURCES
			${SOURCES}
			include/extLink/dataIO_extLinkVCL.h
			src/extLink/dataIO_extLinkVCL.cpp
		)
		INCLUDE_DIRECTORIES(${EXTLINK_VCL_INCLUDE_DIR})
		ADD_DEFINITIONS( "-DUSE_EXTLINK_VCL" )
		#Linking: VCL uses #pragma to link, no need for linking configuration in CMAKE
	ELSE(USE_EXTLINK_VCL)
		UNSET(EXTLINK_VCL_INCLUDE_DIR CACHE)
	ENDIF(USE_EXTLINK_VCL)
ELSE(WIN32)
	UNSET(EXTLINK_VCL_INCLUDE_DIR CACHE)


ENDIF(WIN32)



# Set core sources
SET(SOURCES
	${SOURCES}
	# Core
	include/core/visual_core.h
	src/core/visual_core.cpp
	src/core/osgVisual.cpp
	include/core/core_manipulator.h
	src/core/core_manipulator.cpp
	# Memory Leak debugging
	include/core/leakDetection.h
	# Util
	include/util/visual_util.h
	src/util/visual_util.cpp
	# Draw 2D
	include/draw2D/visual_draw2D.h
	src/draw2D/visual_draw2D.cpp
	# Debug HUD
	include/draw2D/visual_debug_hud.h
	src/draw2D/visual_debug_hud.cpp
	# Example HUD
	include/draw2D/visual_hud.h
	src/draw2D/visual_hud.cpp
	# Draw 3D
	include/draw3D/visual_draw3D.h
	src/draw3D/visual_draw3D.cpp
	# Object Mounted Manipulator
	include/manip_ObjectMounted/manip_objectMounted.h
	src/manip_ObjectMounted/manip_objectMounted.cpp
	# Objects
	include/object/visual_object.h
	include/object/object_updater.h
	src/object/visual_object.cpp
	src/object/object_updater.cpp
	# DataIO
	include/dataIO/visual_dataIO.h
	include/dataIO/dataIO_transportContainer.h
	include/dataIO/dataIO_slot.h
	include/dataIO/dataIO_executer.h
	src/dataIO/visual_dataIO.cpp
	src/dataIO/dataIO_transportContainer.cpp
	src/dataIO/dataIO_slot.cpp
	src/dataIO/dataIO_executer.cpp
)

INCLUDE_DIRECTORIES(include/core include/util include/draw2D include/draw3D include/object include/manip_ObjectMounted)
INCLUDE_DIRECTORIES(include/dataIO include/cluster include/extLink ${OPENSCENEGRAPH_INCLUDE_DIRS} .)


# Executable Output 
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
ADD_EXECUTABLE(osgVisual ${SOURCES})


# Linking
TARGET_LINK_LIBRARIES(osgVisual ${OPENSCENEGRAPH_LIBRARIES} ${OPENGL_LIBRARIES} ${LIBXML2_LIBRARY})


IF(USE_SKY_SILVERLINING )
	IF(WIN32)
		TARGET_LINK_LIBRARIES(osgVisual "winmm.lib")
	ENDIF(WIN32)
	TARGET_LINK_LIBRARIES(osgVisual debug ${SILVERLINING_LIBRARY_DEBUG} optimized ${SILVERLINING_LIBRARY_RELEASE})
ENDIF(USE_SKY_SILVERLINING)
		
IF(USE_VISTA2D)
	TARGET_LINK_LIBRARIES(osgVisual  debug ${VISTA2D_LIBRARY_DEBUG} optimized ${VISTA2D_LIBRARY_RELEASE})
ENDIF(USE_VISTA2D)

IF(USE_CLUSTER_ENET AND WIN32)
	TARGET_LINK_LIBRARIES(osgVisual "winmm.lib" "ws2_32.lib" )
ENDIF(USE_CLUSTER_ENET AND WIN32)

# CMAKE Fix for VS to not prepend build type to path.
IF(MSVC)
	SET_TARGET_PROPERTIES(osgVisual PROPERTIES PREFIX "../") 
ENDIF(MSVC)
SET_TARGET_PROPERTIES( osgVisual PROPERTIES DEBUG_POSTFIX d )

# Adding definitions to disable multicore compiling
IF(MSVC)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP1")
ENDIF(MSVC)

# Generating vcproj file, if MSVC to preset debugging parameter	 - based on http://stackoverflow.com/questions/1005901/how-to-set-path-environment-variable-using-cmake-and-visual-studio-to-run-test
IF (MSVC)
    # Set up a default .user file that contains the debugging setup
    ## Find user and system name
	SET(SYSTEM_NAME $ENV{USERDOMAIN} CACHE STRING "Build System Name")
	SET(USER_NAME $ENV{USERNAME} CACHE STRING "Build System UserName")
	SET(USERFILE_COMMAND_DEBUG "$(TargetPath)" CACHE STRING "Command to execute on debugging (VS Project settings)" )
	SET(USERFILE_WORKING_DIRECTORY_DEBUG "bin" CACHE STRING "Working directory on debugging (VS Project settings)" )
	SET(USERFILE_COMMAND_ARGUMENTS_DEBUG " --window 100 100 800 600 --config osgVisualConfig.xml" CACHE STRING "Command arguments (VS Project settings)" )
	SET(USERFILE_REMOTE_MACHINE_DEBUG ${USER_NAME} CACHE STRING "Remote Debugging Machine (VS Project settings)")
	IF (DESTINATION_ARCH STREQUAL "x64")
		SET(USERFILE_ENVIRONMENT "PATH=%OSG_X64%;%PATH%" CACHE STRING "Environment Variables that VS should merge on thy fly with system environment to run osgVisual (VS Project settings)")
	ENDIF ()


    # Only do this if one doesn't already exist
    SET (USER_FILE ${PROJECT_NAME}.vcproj.${SYSTEM_NAME}.${USER_NAME}.user)
    IF (NOT EXISTS "${USER_FILE}")
        message (STATUS "Creating ${USER_FILE} with default debug settings")
        # These are the configuration variables
        IF (CMAKE_GENERATOR MATCHES "Win64")
            SET (USERFILE_PLATFORM "x64")
        ELSE ()
            SET (USERFILE_PLATFORM "Win32")
        ENDIF ()

		# Configure the template file
		# MSC_VER 1300: MSC 13.0, VC 7.0, VS 2002
		# MSC_VER 1310: MSC 13.1, VC 7.1, VS 2003
		# MSC_VER 1400: MSC 14.0, VC 8.0, VS 2005
		# MSC_VER 1500: MSC 15.0, VC 9.0, VS 2008
		# MSC_VER 1600: MSC ??.0, VC 10.0, VS 2010
		IF (MSVC_VERSION EQUAL 1400)
			CONFIGURE_FILE(VC80.vcproj.user.template ${USER_FILE} @ONLY)
		ENDIF()
		IF (MSVC_VERSION EQUAL 1500)
			CONFIGURE_FILE(VC90.vcproj.user.template ${USER_FILE} @ONLY)
		ENDIF()		
	ELSE()
		message (STATUS "Userfile ${USER_FILE} already exist: Skipping")
    ENDIF()
ENDIF()



# This file handles the complexity of doing a "super build".
#
# The superbuild builds the libraries VTK and ITK and the simi-qt code
# all as external projects. It is provided to setup the development environment
# so that the libraries are the correct versions and have the correct build options
#
# Look at "src/CMakeList.txt" for the actuall building of the simi-qt
# project.
#
#

PROJECT(simi-qt)

cmake_minimum_required(VERSION 2.8)

SET(DO_SUPER_BUILD OFF CACHE BOOL "Build Simi-Qt as an external project along with selected libraries. Set to OFF to use system libraries.")

#We hide this to prevent confusion as this option does nothing
#during a super build and when it isn't a super build the option
#SQ_BUILD_TYPE is used
MARK_AS_ADVANCED(CMAKE_BUILD_TYPE)

IF(DO_SUPER_BUILD)
	#Workaround: Cmake <2.8.4 ExternalProjects Module doesn't support CMAKE_CACHE_ARGS option
	IF( ${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 8.3 )
		#Use system's ExternalProject module
		MESSAGE(STATUS "Using system's External Project module")
		INCLUDE(ExternalProject)
	ELSE()
		#Use ExternalProject module from cmake 2.8.7 release
		MESSAGE(WARNING "Using Project's ExternalProject module rather than system's" )
		INCLUDE(external/ExternalProject.cmake)
	ENDIF()

	#User can disable and use system library instead
	SET(BUILD_VTK ON CACHE BOOL "Build VTK library. Set to OFF to use system library.")

	#Make sure superbuild options are visible
	INCLUDE(external/external-superbuild-hide.cmake)
	SB_DISPLAY_OPTIONS(ON)

	#Add external projects to this list
	SET(SIMI_QT_DEP "")

	IF(BUILD_VTK)
		INCLUDE(external/external-vtk.cmake)
	ELSE()
		#Hide VTK build options
		INCLUDE(external/external-vtk-hide.cmake)
		VTK_DISPLAY_OPTIONS(OFF)
	ENDIF()

	#Build Simi-Qt as external project
	INCLUDE(external/external-simi-qt.cmake)
	
ELSE()
	#Hide VTK options as not doing super build
	INCLUDE(external/external-vtk-hide.cmake)
	VTK_DISPLAY_OPTIONS(OFF)

	#Hide superbuild options as not doing super build
	INCLUDE(external/external-superbuild-hide.cmake)
	SB_DISPLAY_OPTIONS(OFF)

	#Build Simi-Qt as a normal project
	INCLUDE(simi-qt-cache-build.cmake)

	ENABLE_TESTING(TRUE)

	#Use cache value to set build type 
	SET(CMAKE_BUILD_TYPE ${SQ_BUILD_TYPE} CACHE STRING "" FORCE)

	ADD_SUBDIRECTORY( src/ )
ENDIF()

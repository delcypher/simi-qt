PROJECT(SIMI_QT)

cmake_minimum_required(VERSION 2.8)

SET(DO_SUPER_BUILD ON CACHE BOOL "Build Simi-Qt as an external project. Set false to use system libraries.")

IF(DO_SUPER_BUILD)
	INCLUDE(ExternalProject)

	#User can disable and use system library instead
	SET(BUILD_ITK ON CACHE BOOL "Build ITK")

	#Add external projects to this list
	SET(SIMI_QT_DEP "")

	IF(BUILD_ITK)
		INCLUDE(external/external-itk.cmake)
	ENDIF()
	
	#Build Simi-Qt as external project
	INCLUDE(external/external-simi-qt.cmake)
	
ELSE()
	#Hide ITK options as not doing super build
	INCLUDE(external/external-itk-hide.cmake)
	ITK_DISPLAY_OPTION(OFF)

	#Build Simi-Qt as a normal project
	INCLUDE(simi-qt-cache-build.cmake)

	#Use cache value to set build type 
	SET(CMAKE_BUILD_TYPE ${SQ_BUILD_TYPE} CACHE STRING "" FORCE)

	FIND_PACKAGE(ITK)

	IF(ITK_FOUND)
		INCLUDE(${ITK_USE_FILE})
	ELSE(ITK_FOUND)
		MESSAGE(FATAL_ERROR
		"ITK not found. Please set ITK_DIR.")
	ENDIF(ITK_FOUND)
	
	ADD_EXECUTABLE(HelloWorld hello.cxx )
	TARGET_LINK_LIBRARIES(HelloWorld ITKCommon)

ENDIF()

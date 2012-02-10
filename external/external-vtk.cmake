#Cmake file for fetching and building VTK

INCLUDE(external/external-vtk-hide.cmake)

SET(VTK_BASE_DIR ${PROJECT_BINARY_DIR}/vtk CACHE PATH "Directory for VTK library source and builds")
SET(VTK_GIT_URL "git://vtk.org/VTK.git" CACHE STRING "Git clone path for VTK library")
SET(VTK_GIT_TAG "v5.8.0" CACHE STRING "Git tag to checkout, usually a version")

VTK_DISPLAY_OPTIONS(ON)

IF(NOT VTK_BUILD_TYPE)
  set(VTK_BUILD_TYPE Debug CACHE STRING "Build type for the VTK library" FORCE)
  # Set the possible values of build type for cmake-gui/ccmake
  set_property(CACHE VTK_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
ENDIF()


ExternalProject_Add(VTK
PREFIX ${VTK_BASE_DIR}
GIT_REPOSITORY ${VTK_GIT_URL}
GIT_TAG ${VTK_GIT_TAG}
CMAKE_CACHE_ARGS 
	-DBUILD_EXAMPLES:BOOL=OFF
	-DBUILD_TESTING:BOOL=OFF 
	-DBUILD_SHARED_LIBS:BOOL=ON 
	-DCMAKE_BUILD_TYPE:STRING=${VTK_BUILD_TYPE}
	-DVTK_USE_QT:BOOL=ON
BINARY_DIR ${VTK_BASE_DIR}/bin
BUILD_IN_SOURCE 0
INSTALL_COMMAND "" #Don't install
)


#Add VTK to list of simi-qt dependencies
SET(SIMI_QT_DEP ${SIMI_QT_DEP} VTK)

ExternalProject_Get_Property(VTK BINARY_DIR)

#Use binary build directory of VTK from now for FIND_PACKAGE(VTK)
SET(VTK_DIR ${BINARY_DIR} CACHE PATH "The path to an VTK build tree/installation" FORCE)

#Cmake file for fetching and building ITK

INCLUDE(external/external-itk-hide.cmake)

SET(ITK_BASE_DIR ${SIMI_QT_BINARY_DIR}/itk CACHE PATH "Directory for ITK source and builds")
SET(ITK_GIT_URL "git://itk.org/ITK.git" CACHE STRING "Git clone path for ITK")
SET(ITK_GIT_TAG "v4.0.0" CACHE STRING "Git tag to checkout, usally a version")

ITK_DISPLAY_OPTION(ON)

IF(NOT ITK_BUILD_TYPE)
  set(ITK_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui/ccmake
  set_property(CACHE ITK_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
ENDIF()


ExternalProject_Add(ITK
PREFIX ${ITK_BASE_DIR}
GIT_REPOSITORY ${ITK_GIT_URL}
GIT_TAG ${ITK_GIT_TAG}
CMAKE_CACHE_ARGS -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_BUILD_TYPE:STRING=${ITK_BUILD_TYPE}
BINARY_DIR ${ITK_BASE_DIR}/bin
BUILD_IN_SOURCE 0
INSTALL_COMMAND "" #Don't install
)


#Add ITK to list of simi-qt dependencies
SET(SIMI_QT_DEP ${SIMI_QT_DEP} ITK)

ExternalProject_Get_Property(ITK BINARY_DIR)

#Use binary build directory of ITK from now for FIND_PACKAGE(ITK)
SET(ITK_DIR ${BINARY_DIR} CACHE PATH "The path to an ITK build tree/installation" FORCE)

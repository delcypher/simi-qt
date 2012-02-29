#This is the CMakeLists.txt file for the simi-qt project 

FIND_PACKAGE(VTK)
IF(VTK_FOUND)
	INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
	MESSAGE(FATAL_ERROR
	"VTK not found. Please set VTK_DIR.")
ENDIF(VTK_FOUND)

#Decide if we should request QtTest library
FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui QtTest)
INCLUDE(${QT_USE_FILE})

#Try and retrieve build information if wanted
IF(BUILD_WITH_VERSION_INFO)
	#Attempt to detect version from git
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
	include(GetGitRevisionDescription)
	git_get_exact_tag(SIMI_VERSION --tags)

	if(NOT SIMI_VERSION)
		get_git_head_revision(SIMI_REFSPEC SIMI_HASHVAR)
		set(SIMI_VERSION "<br/><b>Refspec:</b> ${SIMI_REFSPEC}<br/><b>SHA1:</b>${SIMI_HASHVAR}")
	endif()
ELSE()
	set(SIMI_VERSION "Unknown")
ENDIF()

#Build compile time constant file
configure_file(compiletimeconstants.h.in ${CMAKE_CURRENT_BINARY_DIR}/compiletimeconstants.h @ONLY)

#Enable Compiler warnings for g++
if(CMAKE_COMPILER_IS_GNUCXX)
	MESSAGE(STATUS "Enabling warnings for GNU compiler")
	add_definitions(-Wall)
endif()

#Source file definitions
SET(SIMI_SOURCES main.cpp mainwindow.cpp imagepairmanager.cpp seedpointmanager.cpp viewmanager.cpp)

#Headers that need to have moc run on them (generates moc_*.cpp)
# Note that new cmake >=2.8.6 supports the variable AUTOMOC which means
# using QT4_WRAP_CPP isn't necessary but because most development machines
# we have are using have old version we'e stuck with the old way of making this work
SET(SIMI_HEADERS mainwindow.h imagepairmanager.h seedpointmanager.h viewmanager.h)

#User interface files (*.ui) that will have uic run on them (generates ui_*.h)
SET(SIMI_FORMS mainwindow.ui)

#Resources files that will have rcc run on them (generates qrc_*.cpp)
SET(SIMI_RESOURCES mainwindow.qrc )

#Run Qt intermediate programs
QT4_WRAP_UI(SIMI_FORMS_HEADERS ${SIMI_FORMS})
QT4_WRAP_CPP(SIMI_HEADERS_MOC ${SIMI_HEADERS})
QT4_ADD_RESOURCES(SIMI_RESOURCES_RCC ${SIMI_RESOURCES})

#Need to add build directory as include path so that CMake can find intermediate files (from moc, uic and rcc)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}
		${SIMI_SOURCES}
		${SIMI_HEADERS_MOC}
		${SIMI_FORMS_HEADERS}
		${SIMI_RESOURCES_RCC}
		)

TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} 
			${QT_LIBRARIES} 
			${VTK_LIBRARIES}
			QVTK
			)
	

#Add tests but don't built from target 'all'
ADD_SUBDIRECTORY( tests/ EXCLUDE_FROM_ALL)

#This is the CMakeLists.txt file for the simi-qt project 

FIND_PACKAGE(ITK)

IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR
	"ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK)
IF(VTK_FOUND)
	INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
	MESSAGE(FATAL_ERROR
	"VTK not found. Please set VTK_DIR.")
ENDIF(VTK_FOUND)


FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui)
INCLUDE(${QT_USE_FILE})

#Attempt to detect version from git
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GetGitRevisionDescription)
git_get_exact_tag(SIMI_VERSION --tags)

if(NOT SIMI_VERSION)
	get_git_head_revision(SIMI_REFSPEC SIMI_HASHVAR)
	set(SIMI_VERSION "<br/><b>Refspec:</b> ${SIMI_REFSPEC}<br/><b>SHA1:</b>${SIMI_HASHVAR}")
endif()

#Build compile time constant file
configure_file(compiletimeconstants.h.in ${CMAKE_CURRENT_BINARY_DIR}/compiletimeconstants.h @ONLY)

#Enable Compiler warnings for g++
if(CMAKE_COMPILER_IS_GNUCXX)
	MESSAGE(STATUS "Enabling warnings for GNU compiler")
	add_definitions(-Wall)
endif()

#Source file definitions
SET(SIMI_SOURCES main.cpp mainwindow.cpp contrastdialog.cpp)

#Headers that need to have moc run on them (generates moc_*.cpp)
SET(SIMI_HEADERS mainwindow.h contrastdialog.h)

#User interface files (*.ui) that will have uic run on them (generates ui_*.h)
SET(SIMI_FORMS mainwindow.ui contrastdialog.ui)

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
			ITKCommon 
			${QT_LIBRARIES} 
			${VTK_LIBRARIES}
			QVTK
			)
			

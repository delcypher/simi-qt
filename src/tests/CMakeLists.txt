#This file defines the unit tests
#
# It would be nice to use AUTOMOC here
# but we're stuck with < CMake 2.86
# so everything is a bit clunky


###User Interface unit test

#Remove main.cpp and use our own
LIST(REMOVE_ITEM SIMI_SOURCES "main.cpp")

#Unset variables from parent directory as they stop build from working
unset(SIMI_HEADERS_MOC)
unset(SIMI_FORMS_HEADERS)
unset(SIMI_RESOURCES_RCC)

#If using GNU compiler enable coverage generation
IF(CMAKE_COMPILER_IS_GNUCXX)
	MESSAGE(STATUS "Enabling coverage generation for GNU compiler")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
	SET(COVERAGE_LIB gcov)
ELSE()
	SET(COVERAGE_LIB "")
ENDIF()


#Include parent directory for mainwindow.h
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src")

PREPEND_SOURCES(SIMI_F SIMI_FORMS "../")
QT4_WRAP_UI(SIMI_FORMS_HEADERS ${SIMI_F})

PREPEND_SOURCES(SIMI_HP SIMI_HEADERS "../")
QT4_WRAP_CPP(UI_TEST_HEADERS_MOC ${SIMI_HP} ui_tester.h)


PREPEND_SOURCES(SIMI_R SIMI_RESOURCES "../")
QT4_ADD_RESOURCES(SIMI_RESOURCES_RCC ${SIMI_R})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


ADD_EXECUTABLE(ui_tester
		${UI_TEST_HEADERS_MOC}
		${SIMI_PREPEND_SOURCES}
		${SIMI_FORMS_HEADERS}
		${SIMI_RESOURCES_RCC}
		ui_tester.cpp
		)

TARGET_LINK_LIBRARIES(ui_tester
			${QT_LIBRARIES} 
			${VTK_LIBRARIES}
			QVTK
			${COVERAGE_LIB}
			)
ADD_TEST(UserInterfaceTester ui_tester)
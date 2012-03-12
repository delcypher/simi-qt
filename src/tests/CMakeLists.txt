#This file defines the unit tests
#
# It would be nice to use AUTOMOC here
# but we're stuck with < CMake 2.86
# so everything is a bit clunky

INCLUDE("cmake/prependlist.cmake")

#We maintain a list of targets that are test units.
#This is to work around a CMake bug where running make test does not build the tests!
SET( UNIT_TESTS "")

#Unset variables from parent directory as they stop build from working if we use those variable names
unset(SIMI_HEADERS_MOC)
unset(SIMI_FORMS_HEADERS)
unset(SIMI_RESOURCES_RCC)

#Include parent directory for the header files
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src")

#Include this folder for the intermediate build files
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#If using GNU compiler enable coverage generation
INCLUDE("cmake/lcov.cmake")

###START OF UNIT TESTS


###User Interface unit test
INCLUDE("ui_tester.cmake")


###END OF UNIT TESTS

#Add custom target "all_tests" that will build everthing in UNIT_TESTS
ADD_CUSTOM_TARGET(all_tests COMMAND ${CMAKE_COMMAND} -E echo "Built unit tests:${UNIT_TESTS}"
		DEPENDS ${UNIT_TESTS})

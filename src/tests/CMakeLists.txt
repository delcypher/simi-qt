#This file defines the unit tests
#
# It would be nice to use AUTOMOC here
# but we're stuck with < CMake 2.86
# so everything is a bit clunky

INCLUDE("cmake/prependlist.cmake")

#We maintain a list of targets that are test units.
#This is to work around a CMake bug where running make test does not build the tests!
SET( UNIT_TESTS "")

###START OF UNIT TESTS


###User Interface unit test
INCLUDE("ui_tester.cmake")


###END OF UNIT TESTS

#Add custom target "all_tests" that will build everthing in UNIT_TESTS
ADD_CUSTOM_TARGET(all_tests COMMAND ${CMAKE_COMMAND} -E echo "Built unit tests:${UNIT_TESTS}"
		DEPENDS ${UNIT_TESTS})

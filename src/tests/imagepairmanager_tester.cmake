
#Put QObject classes through MOC, we unset incase other test units have used this variable name
UNSET(TEST_HEADERS_MOC)
QT4_WRAP_CPP(TEST_HEADERS_MOC "../imagepairmanager.h" imagepairmanager_tester.h)

#Set the name of the unit test
SET(UNIT_TEST_NAME "imagepairmanager_tester")

ADD_EXECUTABLE(${UNIT_TEST_NAME}
		${TEST_HEADERS_MOC}
		imagepairmanager_tester.cpp
		"../imagepairmanager.cpp"
		)

TARGET_LINK_LIBRARIES(${UNIT_TEST_NAME}
			${QT_LIBRARIES} 
			${VTK_LIBRARIES}
			QVTK
			${COVERAGE_LIB}
			)
ADD_TEST(ImagePairManagerTester ${UNIT_TEST_NAME})

#Add the unit test to the list of unit tests that could be built
LIST(APPEND UNIT_TESTS ${UNIT_TEST_NAME})

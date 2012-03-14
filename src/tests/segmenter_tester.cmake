#Remove main.cpp and use our own

#Put QObject classes through MOC
QT4_WRAP_CPP(TEST_HEADERS_MOC "../segmenter.h" segmenter_tester.h)

#Set the name of the unit test
SET(UNIT_TEST_NAME "segmenter_tester")

ADD_EXECUTABLE(${UNIT_TEST_NAME}
		${TEST_HEADERS_MOC}
		segmenter_tester.cpp
		"../segmenter.cpp"
		)

TARGET_LINK_LIBRARIES(${UNIT_TEST_NAME}
			${QT_LIBRARIES} 
			${VTK_LIBRARIES}
			QVTK
			${COVERAGE_LIB}
			)
ADD_TEST(SegmenterTester ${UNIT_TEST_NAME})

#Add the unit test to the list of unit tests that could be built
LIST(APPEND UNIT_TESTS ${UNIT_TEST_NAME})

#Clean up the TEST_HEADERS_MOC variable so other classes can use it
UNSET(TEST_HEADERS_MOC)

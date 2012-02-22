#This file defines the unit tests
#
#

###User Interface unit test
PREPEND_SOURCES(SIMI_PREPEND_SOURCES SIMI_SOURCES "../")

ADD_EXECUTABLE(ui_tester
		${SIMI_PREPEND_SOURCES}
		${SIMI_HEADERS_MOC}
		${SIMI_FORMS_HEADERS}
		${SIMI_RESOURCES_RCC}
		)

TARGET_LINK_LIBRARIES(ui_tester
			${QT_LIBRARIES} 
			${VTK_LIBRARIES}
			QVTK
			)
ADD_TEST(UserInterfaceTester uitester)

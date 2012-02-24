#Lcov cmake file

#Set compiler and linker flags

#If using GNU compiler enable coverage generation
IF(CMAKE_COMPILER_IS_GNUCXX)
	MESSAGE(STATUS "Enabling coverage generation for GNU compiler for tests")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
	SET(COVERAGE_LIB gcov)

	FIND_PROGRAM(LCOV_TOOL "lcov")
	FIND_PROGRAM(GENHTML_TOOL "genhtml")

	IF(NOT LCOV_TOOL OR NOT GENHTML_TOOL)
		MESSAGE(STATUS "Cannot find lcov. Cannot add targets for generating code coverage information")
	ELSE()
		MESSAGE(STATUS "Found lcov. Adding coverage targets")
		#Create directory to hold coverage information
		SET(COVERAGE_INFO_DIR "${CMAKE_CURRENT_BINARY_DIR}/coverage")
		SET(COVERAGE_INFO_FILE "${CMAKE_CURRENT_BINARY_DIR}/unit_tests.info")
		FILE(MAKE_DIRECTORY "${COVERAGE_INFO_DIR}")

		#Create directory for generated HTML coverage information
		SET(COVERAGE_HTML_DIR "${CMAKE_BINARY_DIR}/coverage_html/")
		FILE(MAKE_DIRECTORY "${COVERAGE_HTML_DIR}")

		SET(LCOV_SCAN_DIR "${CMAKE_CURRENT_BINARY_DIR}/")

		#Add coverage_clean target to setup counters for lcov
		ADD_CUSTOM_TARGET(coverage_clean)

		ADD_CUSTOM_COMMAND(TARGET coverage_clean
					POST_BUILD
					COMMAND ${LCOV_TOOL} -d "${LCOV_SCAN_DIR}" --zerocounters
					COMMENT "Zeroing counters"
					)

		#Add coverage_gen target to generate 
		ADD_CUSTOM_TARGET(coverage_gen)
		ADD_CUSTOM_COMMAND(TARGET coverage_gen
					POST_BUILD
					COMMENT "Cleaning up HTML directory ${COVERAGE_HTML_DIR}"
					COMMAND ${CMAKE_COMMAND} -E remove_directory "${COVERAGE_HTML_DIR}"
					COMMAND ${CMAKE_COMMAND} -E make_directory "${COVERAGE_HTML_DIR}"
					COMMAND ${LCOV_TOOL} -d "${LCOV_SCAN_DIR}" --capture --output-file "${COVERAGE_INFO_FILE}"
					COMMAND ${LCOV_TOOL} -d "${LCOV_SCAN_DIR}" --remove "${COVERAGE_INFO_FILE}" '/usr/include/*' --output-file "${COVERAGE_INFO_FILE}"
					COMMAND ${GENHTML_TOOL} -o "${COVERAGE_HTML_DIR}" --title "Unit tests" "${COVERAGE_INFO_FILE}"
					)


	ENDIF()
ELSE()
	SET(COVERAGE_LIB "")
ENDIF()

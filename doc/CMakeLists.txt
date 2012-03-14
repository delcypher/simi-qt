#-- Add an Option to toggle the generation of the API documentation
option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML/LaTeX based API documentation" OFF)
if(BUILD_DOCUMENTATION)
	FIND_PACKAGE(Doxygen)
	if (NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
	endif()

	SET(DOC_OUTPUT_DIR "${PROJECT_BINARY_DIR}/doc/Doxygen" CACHE FILEPATH "The directory that will contain generated Doxygen documentation")

	#-- Configure the Template Doxyfile for our specific project
	configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/doc/Doxyfile  @ONLY IMMEDIATE)

	#-- Add a custom target to run Doxygen when ever the project is built
	add_custom_target (docs ALL 
			COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/doc/Doxyfile
			SOURCES ${PROJECT_BINARY_DIR}/doc/Doxyfile)
endif()

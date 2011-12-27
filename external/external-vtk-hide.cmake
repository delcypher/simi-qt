# VTK_DISPLAY_OPTIONS( [ON | OFF])
#
# Show or hide (marks as advanced) cache variables
# for building the VTK library. Usually we want to hide
# these variables when we are not doing a superbuild
# so we don't confuse the user with options that are not
# relevant.
macro(VTK_DISPLAY_OPTIONS display)
	IF(${display})
		set(_CLEAR CLEAR)
	else()
		set(_CLEAR FORCE)
	endif()

	SET(VTK_VARIABLES 
		VTK_BASE_DIR
		VTK_GIT_URL
		VTK_GIT_TAG
		VTK_BUILD_TYPE
	)

	foreach(VTK_VARIABLE ${VTK_VARIABLES})
		mark_as_advanced(${_CLEAR} ${VTK_VARIABLE})
	endforeach(VTK_VARIABLE)
endmacro(VTK_DISPLAY_OPTIONS)

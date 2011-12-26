# ITK_DISPLAY_OPTIONS( [ON | OFF])
#
# Show or hide (marks as advanced) cache variables
# for building the ITK library. Usually we want to hide
# these variables when we are not doing a superbuild
# so we don't confuse the user with options that are not
# relevant.
macro(ITK_DISPLAY_OPTIONS display)
	IF(${display})
		set(_CLEAR CLEAR)
	else()
		set(_CLEAR FORCE)
	endif()

	SET(ITK_VARIABLES 
		ITK_BASE_DIR
		ITK_GIT_URL
		ITK_GIT_TAG
		ITK_BUILD_TYPE
		BUILD_ITK
	)

	foreach(ITK_VARIABLE ${ITK_VARIABLES})
		mark_as_advanced(${_CLEAR} ${ITK_VARIABLE})
	endforeach(ITK_VARIABLE)
endmacro(ITK_DISPLAY_OPTIONS)

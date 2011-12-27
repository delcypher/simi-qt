# SB_DISPLAY_OPTIONS( [ON | OFF])
#
# Show or hide (marks as advanced) cache variables
# for doing superbuild. Usually we want to hide
# these variables when we are not doing a superbuild
# so we don't confuse the user with options that are not
# relevant.
macro(SB_DISPLAY_OPTIONS display)
	IF(${display})
		set(_CLEAR CLEAR)
	else()
		set(_CLEAR FORCE)
	endif()

	SET(SB_VARIABLES 
		BUILD_ITK
		BUILD_VTK
	)

	foreach(SB_VARIABLE ${SB_VARIABLES})
		mark_as_advanced(${_CLEAR} ${SB_VARIABLE})
	endforeach(SB_VARIABLE)
endmacro(SB_DISPLAY_OPTIONS)

#If we're not building ITK we should hide (mark as advanced) the cache variables set so we don't confuse the user

#
#display =ON mark as normal cache value
#display =OFF mark as advanced cache value
macro(ITK_DISPLAY_OPTION display)
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
endmacro(ITK_DISPLAY_OPTION)

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
	mark_as_advanced(${_CLEAR} ITK_BASE_DIR)
	mark_as_advanced(${_CLEAR} ITK_GIT_URL)
	mark_as_advanced(${_CLEAR} ITK_GIT_TAG)
	mark_as_advanced(${_CLEAR} ITK_BUILD_TYPE)
endmacro(ITK_DISPLAY_OPTION)

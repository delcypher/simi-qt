#This sets the default cache values for the "simi-qt" project.

MACRO(SQ_SET_CACHE varname value type string_description)
	SET(${varname} ${value} CACHE ${type} ${string_description})

	IF(DO_SUPER_BUILD)
		#If doing superbuild build cache value list
		SET(SQ_CMAKE_CACHE_SB 
		${SQ_CMAKE_CACHE_SB} -D${varname}:${type}=${value})
	ENDIF()
ENDMACRO(SQ_SET_CACHE)

SQ_SET_CACHE(SQ_BUILD_TYPE "Release" STRING "Build type for the simi-qt component")
# Set the possible values of build type for cmake-gui
set_property(CACHE SQ_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

SQ_SET_CACHE(BUILD_WITH_VERSION_INFO TRUE BOOL "Use Git to put tag or ref information into the compiled application.")


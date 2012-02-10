#Treat simi-qt project as external project so we can do a "super build" where the needed libraries are built from source first

INCLUDE(simi-qt-cache-build.cmake)

ExternalProject_Add(SIMI_QT_EXTERNAL
DEPENDS ${SIMI_QT_DEP} #Add libraries that might of been built
PREFIX ${PROJECT_BINARY_DIR}
DOWNLOAD_COMMAND "" #Don't download anything
SOURCE_DIR ${SIMI_QT_SOURCE_DIR}
CMAKE_CACHE_ARGS -DDO_SUPER_BUILD:BOOL=OFF
	-DITK_DIR:PATH=${ITK_DIR} ${SQ_CMAKE_CACHE_SB}
BINARY_DIR ${SIMI_QT_BINARY_DIR}
INSTALL_COMMAND "" #Don't install
)

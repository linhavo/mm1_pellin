cmake_minimum_required (VERSION 2.6)
SET(CPACK_SOURCE_GENERATOR TBZ2)
SET(CPACK_PACKAGE_VERSION_MAJOR 0)
SET(CPACK_PACKAGE_VERSION_MINOR 1)
SET(CPACK_PACKAGE_VERSION_PATCH 0)
SET(CPACK_PACKAGE_VENDOR IIM)
SET(CPACK_PACKAGE_CONTACT travnicek@iim.cz)
SET(CPACK_SYSTEM_NAME linux)
INCLUDE(CPack)
INCLUDE(FindPkgConfig)
#SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
project (iimaudio)


OPTION (BUILD_SHARED_LIBS "Build shared libraries." ON)
OPTION (BUILD_EXAMPLES "Build example applications" ON)

#SET(CMAKE_BUILD_TYPE RelWithDebInfo)

include_directories( ${CMAKE_SOURCE_DIR}/include )

IF (UNIX)
add_definitions("-Wall -Wextra -pedantic -std=c++0x")
ENDIF ()

add_subdirectory(src)
add_subdirectory(examples)



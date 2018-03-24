cmake_minimum_required(VERSION 2.6)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_SOVERSION ${PROJECT_VERSION_MAJOR})

if(${CMAKE_VERSION} VERSION_GREATER 3.0)
	cmake_policy(SET CMP0048 NEW)
	project(pega-texto-debugger VERSION ${PROJECT_VERSION})
else()
	project(pega-texto-debugger)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wno-parentheses")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

# Build stuff
option(BUILD_SHARED_LIBS "Build as a shared library" ON)
if(UNIX)
	option(LINK_SONAME_FILE "Create an additional symlink and pkg-config script for library based on it's SONAME" ON)
endif()
add_subdirectory(src)
# add_subdirectory(pkg-config)

# # Examples
# option(COMPILE_EXAMPLES "Compile the examples" OFF)
# if(COMPILE_EXAMPLES)
	# add_subdirectory(examples)
# endif()

# # Install headers
file(GLOB headers "include/pega-texto-debugger/*.h")
configure_file("include/pega-texto-debugger.h" "pega-texto-debugger.h" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pega-texto-debugger.h" DESTINATION "include")
install(FILES ${headers} DESTINATION "include/${CMAKE_PROJECT_NAME}")

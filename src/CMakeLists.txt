include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
	pkg_check_modules(PEGA_TEXTO REQUIRED pega-texto>=2)
	link_libraries(${PEGA_TEXTO_LIBRARIES})
else()
	link_libraries(pega-texto readline)
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")

# library
set(libsrc command.c  match_actions.c  ptdb.c  shell.c  wrap.c)
add_library(pega-texto-debugger ${libsrc})
set_target_properties(pega-texto-debugger PROPERTIES
	VERSION "${PROJECT_VERSION}"
	SOVERSION "${PROJECT_SOVERSION}")

if(UNIX AND BUILD_SHARED_LIBS AND LINK_SONAME_FILE)
	if(APPLE)
		set(lib-ext dylib)
	else()
		set(lib-ext so)
	endif()
	set(link-name "libpega-texto-debugger${PROJECT_VERSION_MAJOR}.${lib-ext}")
	add_custom_command(TARGET pega-texto-debugger POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_SONAME_FILE_NAME:pega-texto-debugger> ${link-name})
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${link-name} DESTINATION lib)
endif()

# example test
add_executable(ptdb-test ${libsrc} main.c)
add_dependencies(pega-texto-debugger 3rd-party)
add_dependencies(ptdb-test 3rd-party)

# Install library
install(TARGETS pega-texto-debugger
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)


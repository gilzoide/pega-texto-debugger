link_libraries(${PEGA_TEXTO_LIBRARIES} ${PEGA_TEXTO_MEMORY_LIBRARIES} ${REPLXX_LIBRARIES})
include_directories("${PROJECT_SOURCE_DIR}/include"
	${PEGA_TEXTO_INCLUDE_DIRS} ${PEGA_TEXTO_MEMORY_INCLUDE_DIRS} ${REPLXX_INCLUDE_DIRS})

# library
set(libsrc command.c  match_actions.c  print_expr.c ptdb.c  shell.c  wrap.c)
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
	add_custom_command(TARGET pega-texto-debugger POST_BUILD COMMAND
		${CMAKE_COMMAND} -E create_symlink
		$<TARGET_SONAME_FILE_NAME:pega-texto-debugger> ${link-name})
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${link-name} DESTINATION lib)
endif()

# example test
add_executable(ptdb-test main.c)
target_link_libraries(ptdb-test pega-texto-debugger)

# Install library
install(TARGETS pega-texto-debugger
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)


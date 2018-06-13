include(ExternalProject)
set(replxx-bindir ${CMAKE_BINARY_DIR}/replxx)
ExternalProject_add(replxx
	PREFIX replxx
	GIT_REPOSITORY https://github.com/gilzoide/replxx
	GIT_TAG 34a02e70aba04fbf0a5fbb9ecb6e3229d006e0e3
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${replxx-bindir}
	           -DREPLXX_BuildExamples=OFF
	           -DBUILD_SHARED_LIBS=ON
	)
link_directories(${replxx-bindir}/lib)
link_libraries(replxx)
include_directories(${replxx-bindir}/include)

add_custom_target(3rd-party)
add_dependencies(3rd-party replxx)


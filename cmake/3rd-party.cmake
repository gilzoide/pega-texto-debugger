include(ExternalProject)
set(replxx-bindir ${CMAKE_BINARY_DIR}/replxx)
ExternalProject_add(replxx
	PREFIX replxx
	GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx
	GIT_TAG 9da16bb45d5c22224cffac40c9b52352f98be8f1
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${replxx-bindir}
	           -DREPLXX_BuildExamples=OFF
	           -DBUILD_SHARED_LIBS=ON
	)
link_directories(${replxx-bindir}/lib)
link_libraries(replxx)
include_directories(${replxx-bindir}/include)

add_custom_target(3rd-party)
add_dependencies(3rd-party replxx)


cmake_minimum_required(VERSION 3.14)

project(lmax_cli)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	lmax_cli
	${src}
)

target_include_directories(lmax_cli PUBLIC ${include_dirs} ${PROJECT_SOURCE_DIR})
target_link_libraries(lmax_cli PUBLIC fix)
cmake_minimum_required(VERSION 3.14)

project(fix_server)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	fix_server
	${src}
)

target_include_directories(fix_server PUBLIC ${include_dirs})

target_link_libraries(fix_server PUBLIC fix)
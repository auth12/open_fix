cmake_minimum_required(VERSION 3.14)

project(fix_cli)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	fix_cli
	${src}
)

target_include_directories(fix_cli PUBLIC ${include_dirs})

target_link_libraries(fix_cli PUBLIC fix)
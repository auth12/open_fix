cmake_minimum_required(VERSION 3.14)

project(fix_parse)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	fix_parse
	${src}
)

target_include_directories(fix_parse PUBLIC ${include_dirs})

target_link_libraries(fix_parse PUBLIC spdlog fix)
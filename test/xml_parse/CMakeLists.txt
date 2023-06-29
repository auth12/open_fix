cmake_minimum_required(VERSION 3.14)

project(xml_parse)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	xml_parse
	${src}
)

target_include_directories(xml_parse PUBLIC ${include_dirs})

target_link_libraries(xml_parse PUBLIC fix)
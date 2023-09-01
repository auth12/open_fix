cmake_minimum_required(VERSION 3.14)

project(tcp_server)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	tcp_server
	${src}
)

target_include_directories(tcp_server PUBLIC ${include_dirs} ${PROJECT_SOURCE_DIR})
target_link_libraries(tcp_server PUBLIC fix)
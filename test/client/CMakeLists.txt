cmake_minimum_required(VERSION 3.14)

project(tcp_client)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	tcp_client
	${src}
)

target_include_directories(tcp_client PUBLIC ${include_dirs})

target_link_libraries(tcp_client PUBLIC fix)
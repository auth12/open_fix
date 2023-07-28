cmake_minimum_required(VERSION 3.14)

project(metrics_cli)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	metrics_cli
	${src}
)

target_include_directories(metrics_cli PUBLIC ${include_dirs})

target_link_libraries(metrics_cli PUBLIC fix prometheus-cpp::pull spdlog)
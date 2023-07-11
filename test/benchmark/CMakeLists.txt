cmake_minimum_required(VERSION 3.14)

project(bench)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	bench
	${src}
)

target_include_directories(bench PUBLIC ${include_dirs} "${PROJECT_SOURCE_DIR}/../../CppBenchmark/include/")

target_link_libraries(bench PUBLIC cppbenchmark absl::base absl::algorithm absl::strings)
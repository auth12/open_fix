cmake_minimum_required(VERSION 3.14)

project(fix_cli)

file(GLOB_RECURSE src "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(
	fix_cli
	${src}
)

# Linking openssl
find_package(OpenSSL REQUIRED)
message("${OPENSSL_LIBRARIES}")
target_include_directories(fix_cli PUBLIC ${include_dirs} ${PROJECT_SOURCE_DIR} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(fix_cli PUBLIC fix cppbenchmark ${OPENSSL_LIBRARIES})
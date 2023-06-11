cmake_minimum_required(VERSION 3.17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(modules/spdlog)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTS OFF)
add_subdirectory(modules/libuv)

set(ENABLE_PROGRAMS OFF)
set(ENABLE_TESTING OFF)
add_subdirectory(modules/mbedtls)

set(TBB_TEST OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(modules/oneTBB)

project(fix_lib CXX)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

file(GLOB_RECURSE lib_src "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE hdr "${PROJECT_SOURCE_DIR}/include/*.h")

set(CPPBENCHMARK_MODULE TRUE)
add_subdirectory(modules/CppBenchmark)

add_library(
	fix STATIC
	${lib_src}
    ${hdr}
)

set(include_dirs "${PROJECT_SOURCE_DIR}/modules/spdlog/include/"
"${PROJECT_SOURCE_DIR}/modules/libuv/include/"
"${PROJECT_SOURCE_DIR}/modules/mbedtls/include/"
"${PROJECT_SOURCE_DIR}/modules/oneTBB/include/"
"${PROJECT_SOURCE_DIR}/modules/hffix/include/"
"${PROJECT_SOURCE_DIR}/modules/atomic_queue/include/"
"${PROJECT_SOURCE_DIR}/modules/cppbenchmark/include/"
"${PROJECT_SOURCE_DIR}/include/")

target_include_directories(fix PUBLIC ${include_dirs})

target_precompile_headers(fix PUBLIC "${PROJECT_SOURCE_DIR}/modules/hffix/include/hffix.hpp" "${PROJECT_SOURCE_DIR}/modules/hffix/include/hffix_fields.hpp" "${PROJECT_SOURCE_DIR}/modules/hffix/include/hffix.hpp" "${PROJECT_SOURCE_DIR}/include/fix_spec.h" )

target_link_libraries(fix PUBLIC uv_a spdlog mbedtls mbedcrypto mbedx509 tbb tbbmalloc cppbenchmark) 

add_subdirectory(test)
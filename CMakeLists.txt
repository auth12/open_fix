cmake_minimum_required(VERSION 3.17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(spdlog)

set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTS OFF)
add_subdirectory(libuv)

set(ENABLE_PROGRAMS OFF)
set(ENABLE_TESTING OFF)
add_subdirectory(mbedtls)

set(TBB_TEST OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(onetbb)

project(fix_lib CXX)

file(GLOB_RECURSE lib_src "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE hdr "${PROJECT_SOURCE_DIR}/include/*.h")

add_library(
	fix STATIC
	${lib_src}
    ${hdr}
)

set(include_dirs "${PROJECT_SOURCE_DIR}/spdlog/include/"
"${PROJECT_SOURCE_DIR}/libuv/include/"
"${PROJECT_SOURCE_DIR}/mbedtls/include/"
"${PROJECT_SOURCE_DIR}/onetbb/include/"
"${PROJECT_SOURCE_DIR}/hffix/include/"
"${PROJECT_SOURCE_DIR}/include/")

target_include_directories(fix PUBLIC ${include_dirs})

target_precompile_headers(fix PUBLIC "${PROJECT_SOURCE_DIR}/hffix/include/hffix.hpp" "${PROJECT_SOURCE_DIR}/hffix/include/hffix_fields.hpp" "${PROJECT_SOURCE_DIR}/hffix/include/hffix.hpp" "${PROJECT_SOURCE_DIR}/include/fix_spec.h" )

target_link_libraries(fix PUBLIC uv_a spdlog mbedtls mbedcrypto mbedx509 tbb tbbmalloc)

add_subdirectory(test)
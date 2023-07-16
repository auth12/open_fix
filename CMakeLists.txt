cmake_minimum_required(VERSION 3.17)

project(fix_lib CXX)

include(cmake/utils.cmake)

file(GLOB_RECURSE lib_src "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE hdr "${PROJECT_SOURCE_DIR}/include/*.h")

add_library(
	fix STATIC
	${lib_src}
    ${hdr}
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ABSL_PROPAGATE_CXX_STD ON)
add_subdirectory(modules/abseil)

add_subdirectory(modules/spdlog)

set(LIBUV_BUILD_SHARED OFF)
set(BUILD_TESTS OFF)
add_subdirectory(modules/libuv)

set(CPPBENCHMARK_MODULE TRUE)
add_subdirectory(modules/CppBenchmark)

set(include_dirs "${PROJECT_SOURCE_DIR}/modules/spdlog/include/"
"${PROJECT_SOURCE_DIR}/modules/libuv/include/"
"${PROJECT_SOURCE_DIR}/modules/hffix/include/"
"${PROJECT_SOURCE_DIR}/modules/atomic_queue/include/"
"${PROJECT_SOURCE_DIR}/modules/cppbenchmark/include/"
"${PROJECT_SOURCE_DIR}/modules/abseil/"
"${PROJECT_SOURCE_DIR}/include/")

target_include_directories(fix PUBLIC ${include_dirs})

target_precompile_headers(fix PUBLIC "${PROJECT_SOURCE_DIR}/modules/hffix/include/hffix.hpp" "${PROJECT_SOURCE_DIR}/modules/hffix/include/hffix_fields.hpp" "${PROJECT_SOURCE_DIR}/include/fix_spec.h" )

target_link_libraries(fix PUBLIC uv_a spdlog cppbenchmark absl::base absl::algorithm absl::strings)

add_subdirectory(test)


##copy configs
set(SOURCE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/configs/fix.json)
set(TARGET_CONFIG_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
file(COPY ${SOURCE_CONFIG} DESTINATION ${TARGET_CONFIG_FOLDER})
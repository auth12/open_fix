cmake_minimum_required(VERSION 3.17)

project(fix_lib CXX)

include(cmake/utils.cmake)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wno-strict-prototypes)

#set(ABSL_PROPAGATE_CXX_STD ON)
#add_subdirectory(modules/abseil)
add_subdirectory(modules/spdlog)

set(ENABLE_TESTING OFF)
set(ENABLE_PUSH OFF)
set(GENERATE_PKGCONFIG OFF)
add_subdirectory(modules/prometheuslib)

set(include_dirs "${PROJECT_SOURCE_DIR}/modules/spdlog/include/"
"${PROJECT_SOURCE_DIR}/modules/atomic_queue/include/"
"${PROJECT_SOURCE_DIR}/modules/prometheus/core/include"
"${PROJECT_SOURCE_DIR}/modules/prometheus/pull/include"
"${PROJECT_SOURCE_DIR}/include/")

file(GLOB_RECURSE hdr "${PROJECT_SOURCE_DIR}/include/*.h")

add_library(
	fix INTERFACE
    ${hdr}
)

target_include_directories(fix INTERFACE ${include_dirs})

target_precompile_headers(fix INTERFACE "${PROJECT_SOURCE_DIR}/include/fix_spec.h")
target_link_libraries(fix INTERFACE spdlog)

add_subdirectory(test)

##copy configs
set(SOURCE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/configs/fix.json)
set(TARGET_CONFIG_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
file(COPY ${SOURCE_CONFIG} DESTINATION ${TARGET_CONFIG_FOLDER})
cmake_minimum_required(VERSION 3.9.4)
project(ShaderView)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILDTYPE RELEASE)

find_package(OpenMP)

add_subdirectory(lib/glfw)
add_subdirectory(lib/freetype)
file(GLOB_RECURSE SRC src/*.cc thirdparty/*.cc)

add_executable(shaderview ${SRC})
target_include_directories(shaderview PUBLIC src)
target_include_directories(shaderview PUBLIC thirdparty)

if( supported )
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET shaderview PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()


set(FREETYPE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/freetype)

target_include_directories(shaderview PUBLIC ${FREETYPE_DIR}/include)
target_link_libraries(shaderview freetype)
target_link_libraries(shaderview GL glfw)

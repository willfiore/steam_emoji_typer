cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME key_replacer)
project(${PROJECT_NAME})

set(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)

set(SOURCES_CXX
	src/main.cpp
)

set(INCLUDE_DIRS
	src/
)

# VS targets globs
file(GLOB_RECURSE VS_ADDITIONAL_TARGETS_GLOB
	src/*.hpp
)

add_executable(${TARGET_NAME} WIN32 ${SOURCES_CXX} ${VS_ADDITIONAL_TARGETS_GLOB})
target_include_directories(${TARGET_NAME} PRIVATE ${INCLUDE_DIRS})

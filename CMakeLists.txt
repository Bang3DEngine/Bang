#=================================================================
# CMake options ==================================================
#=================================================================
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)
project(Bang C CXX)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Include guard ==================================================
#=================================================================
if (DEFINED BANG_BUILT)
    return()
endif()
set(BANG_BUILT ON)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Check input params =============================================
#=================================================================
message("CMAKE_CURRENT_LIST_DIR: " ${CMAKE_CURRENT_LIST_DIR})
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# General variables ==============================================
#=================================================================
set(BANG_ENGINE_ROOT          ${CMAKE_CURRENT_LIST_DIR})
set(BANG_SRC_DIR              ${BANG_ENGINE_ROOT}/src)
set(BANG_ENGINE_SRC_DIR       ${BANG_SRC_DIR}/Engine)
set(BANG_ENGINE_INCLUDE_DIR   ${BANG_ENGINE_ROOT}/include)
set(BANG_ENGINE_LIBRARIES_DIR ${BANG_ENGINE_ROOT}/Libraries)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(${BANG_ENGINE_ROOT}/Build/CMakeIncludes/CMakeCheckInputParams.cmake REQUIRED)
include(${BANG_ENGINE_ROOT}/Build/CMakeIncludes/CMakeMacros.cmake           REQUIRED)

# Output params
message("Bang ======================================")
message("CMAKE_BUILD_TYPE:          ${CMAKE_BUILD_TYPE}")
message("CMAKE_BINARY_DIR:          ${CMAKE_BINARY_DIR}")
message("BUILD_SHARED_LIBS:         ${BUILD_SHARED_LIBS}")
message("EXECUTABLE_OUTPUT_PATH:    ${EXECUTABLE_OUTPUT_PATH}")
message("PROJECT_SOURCE_DIR:        ${PROJECT_SOURCE_DIR}")
message("BANG_ENGINE_ROOT:          ${BANG_ENGINE_ROOT}")
message("BANG_SRC_DIR:              ${BANG_SRC_DIR}")
message("BANG_ENGINE_SRC_DIR:       ${BANG_ENGINE_SRC_DIR}")
message("BANG_ENGINE_LIBRARIES_DIR: ${BANG_ENGINE_LIBRARIES_DIR}")
message("===========================================")
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Profiling set up if needed =====================================
#=================================================================
if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_definitions(-DGPROF)
    link_libraries("profiler") # gperftools
endif()
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Add misc files =================================================
#=================================================================
set(BANG_ENGINE_ASSETS_DIR "${BANG_ENGINE_ROOT}/Assets")
file(GLOB_RECURSE OTHER_FILES "${BANG_ENGINE_ASSETS_DIR}/[^.]*.frag"
                              "${BANG_ENGINE_ASSETS_DIR}/[^.]*.geom"
                              "${BANG_ENGINE_ASSETS_DIR}/[^.]*.vert"
                              "${BANG_ENGINE_ASSETS_DIR}/[^.]*.png"
                              "${BANG_ENGINE_ASSETS_DIR}/[^.]*.svg"
                              "${BANG_ENGINE_ASSETS_DIR}/[^.]*.bmat"
                              "${BANG_ENGINE_ASSETS_DIR}/[^.]*.glsl"
                              "${BANG_ENGINE_ASSETS_DIR}/[^.]*.txt"
                              "${BANG_ENGINE_ROOT}/Build/BuildPackage/CMakeLists.txt"
                              "${BANG_ENGINE_ROOT}/Build/BuildDependencies/CMakeLists.txt"
                              "${BANG_ENGINE_ROOT}/Build/BuildDependencies/ThirdParty/CMakeLists.txt"
                              "${BANG_ENGINE_INCLUDE_DIR}/Bang/*.h"
                              "${BANG_ENGINE_INCLUDE_DIR}/Bang/*.tcc")
add_misc_files("${OTHER_FILES}")
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# BangEngine sources =============================================
#=================================================================
file(GLOB_RECURSE ENGINE_SRC_FILES "${BANG_ENGINE_SRC_DIR}/*.cpp")
#=================================================================
#=================================================================
#=================================================================


#=================================================================
# Dependencies ===================================================
#=================================================================
include("${CMAKE_CURRENT_LIST_DIR}/Build/BuildDependencies/CMakeLists.txt")

add_library(BangEngineObjects OBJECT ${ENGINE_SRC_FILES} ${OTHER_FILES})
if (${BUILD_SHARED_LIBS})
    add_library(BangLibStatic STATIC $<TARGET_OBJECTS:BangEngineObjects>)
    add_library(BangLib       SHARED $<TARGET_OBJECTS:BangEngineObjects>)
    set_target_properties(BangLibStatic PROPERTIES OUTPUT_NAME "Bang")
    set_target_properties(BangLib       PROPERTIES OUTPUT_NAME "Bang")
else()
    add_library(BangLib       STATIC $<TARGET_OBJECTS:BangEngineObjects>)
    add_library(BangLibShared SHARED $<TARGET_OBJECTS:BangEngineObjects>)
    set_target_properties(BangLib       PROPERTIES OUTPUT_NAME "Bang")
    set_target_properties(BangLibShared PROPERTIES OUTPUT_NAME "Bang")
endif()
add_bang_compilation_flags(BangEngineObjects)

target_include_directories(BangLib           PUBLIC ${BANG_ENGINE_INCLUDE_DIR})
target_include_directories(BangEngineObjects PUBLIC ${BANG_ENGINE_INCLUDE_DIR})
target_include_directories(BangEngineObjects PUBLIC ${DEPENDENCIES_INCLUDE_DIRS})
target_link_libraries(BangLib PRIVATE ${DEPENDENCIES_LIB_PATHS})
set_target_properties(BangLib PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(BangLib BuildDependencies)

#=================================================================
#=================================================================
#=================================================================

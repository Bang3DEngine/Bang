#=================================================================
# CMake options ==================================================
#=================================================================
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE CXX_STANDARD 11)
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
# General variables ==============================================
#=================================================================
set(BANG_ENGINE_ROOT ${CMAKE_CURRENT_LIST_DIR})
include(${BANG_ENGINE_ROOT}/Compile/CMakeIncludes/CMakeCheckInputParams.cmake REQUIRED)
include(${BANG_ENGINE_ROOT}/Compile/CMakeIncludes/CMakeMacros.cmake           REQUIRED)

set(BANG_SRC_DIR              ${BANG_ENGINE_ROOT}/src)
set(BANG_ENGINE_SRC_DIR       ${BANG_SRC_DIR}/Engine)
set(BANG_ENGINE_INCLUDE_DIR   ${BANG_ENGINE_ROOT}/include)
set(BANG_ENGINE_LIBRARIES_DIR "${BANG_ENGINE_ROOT}/Libraries/${LIBRARIES_SUBDIR_NAME}")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
# Add misc files =================================================
#=================================================================
option(ADD_MISC_FILES "Add misc files for IDE" ON)
if (ADD_MISC_FILES)
    set(BANG_ENGINE_ASSETS_DIR "${BANG_ENGINE_ROOT}/Assets")
    file(GLOB_RECURSE OTHER_FILES "${BANG_ENGINE_ASSETS_DIR}/[^.]*.frag"
                                  "${BANG_ENGINE_ASSETS_DIR}/[^.]*.geom"
                                  "${BANG_ENGINE_ASSETS_DIR}/[^.]*.vert"
                                  "${BANG_ENGINE_ASSETS_DIR}/[^.]*.bushader"
                                  "${BANG_ENGINE_ASSETS_DIR}/[^.]*.png"
                                  "${BANG_ENGINE_ASSETS_DIR}/*.meta"
                                  "${BANG_ENGINE_ASSETS_DIR}/[^.]*.svg"
                                  "${BANG_ENGINE_ASSETS_DIR}/[^.]*.bmat"
                                  "${BANG_ENGINE_ASSETS_DIR}/[^.]*.glsl"
                                  "${BANG_ENGINE_ASSETS_DIR}/[^.]*.txt"
                                  "${BANG_ENGINE_ROOT}/Compile/CompilePackage/CMakeLists.txt"
                                  "${BANG_ENGINE_ROOT}/Compile/CompileDependencies/CMakeLists.txt"
                                  "${BANG_ENGINE_ROOT}/Compile/CompileDependencies/ThirdParty/CMakeLists.txt"
                                  "${BANG_ENGINE_INCLUDE_DIR}/Bang/*.h"
                                  "${BANG_ENGINE_INCLUDE_DIR}/Bang/*.tcc")
    add_misc_files("${OTHER_FILES}")
endif()
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# BangEngine sources =============================================
#=================================================================
file(GLOB_RECURSE ENGINE_SRC_FILES "${BANG_ENGINE_SRC_DIR}/*.cpp")
add_misc_files("${ENGINE_SRC_FILES}")
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Dependencies ===================================================
#=================================================================
include("${CMAKE_CURRENT_LIST_DIR}/Compile/CompileDependencies/CMakeLists.txt")

if (${BUILD_SHARED_LIBS})
    # If shared building, split compilation into some different shared libs

    # Math lib
    set(BANG_MATH_LIB_SOURCES "")
    file(GLOB_RECURSE BANG_MATH_LIB_SOURCES "${BANG_ENGINE_SRC_DIR}/Math/*.cpp")
    add_library(BangMathLib ${BANG_MATH_LIB_SOURCES})
    target_include_directories(BangMathLib PUBLIC ${DEPENDENCIES_INCLUDE_DIRS} ${BANG_ENGINE_INCLUDE_DIR})
    add_bang_compilation_flags(BangMathLib)
    remove_from_list_another_list(ENGINE_SRC_FILES "${BANG_MATH_LIB_SOURCES}")

    # Components lib
    set(BANG_COMPONENTS_LIB_SOURCES "")
    file(GLOB_RECURSE BANG_COMPONENTS_LIB_SOURCES "${BANG_ENGINE_SRC_DIR}/Components/*.cpp"
                                                  "${BANG_ENGINE_SRC_DIR}/UIComponents/*.cpp")
    add_library(BangComponentsLib ${BANG_COMPONENTS_LIB_SOURCES})
    target_include_directories(BangComponentsLib PUBLIC ${DEPENDENCIES_INCLUDE_DIRS} ${BANG_ENGINE_INCLUDE_DIR})
    add_bang_compilation_flags(BangComponentsLib)
    remove_from_list_another_list(ENGINE_SRC_FILES "${BANG_COMPONENTS_LIB_SOURCES}")

    # Graphics lib
    set(BANG_GRAPHICS_LIB_SOURCES "")
    file(GLOB_RECURSE BANG_GRAPHICS_LIB_SOURCES "${BANG_ENGINE_SRC_DIR}/Graphics/*.cpp")
    add_library(BangGraphicsLib ${BANG_GRAPHICS_LIB_SOURCES})
    target_include_directories(BangGraphicsLib PUBLIC ${DEPENDENCIES_INCLUDE_DIRS} ${BANG_ENGINE_INCLUDE_DIR})
    add_bang_compilation_flags(BangGraphicsLib)
    remove_from_list_another_list(ENGINE_SRC_FILES "${BANG_GRAPHICS_LIB_SOURCES}")
endif()

add_library(BangEngineObjects OBJECT ${ENGINE_SRC_FILES} ${OTHER_FILES})
add_library(BangLib SHARED $<TARGET_OBJECTS:BangEngineObjects>)
add_library(BangStatic STATIC $<TARGET_OBJECTS:BangEngineObjects>)
set_target_properties(BangLib PROPERTIES OUTPUT_NAME "Bang")
set_target_properties(BangStatic PROPERTIES OUTPUT_NAME "Bang")
add_bang_compilation_flags(BangEngineObjects)

target_include_directories(BangEngineObjects PUBLIC ${BANG_ENGINE_INCLUDE_DIR})
target_include_directories(BangEngineObjects PUBLIC ${DEPENDENCIES_INCLUDE_DIRS})
set_target_properties(BangLib PROPERTIES LINKER_LANGUAGE CXX)

if (${BUILD_SHARED_LIBS})
    target_link_libraries(BangLib PUBLIC BangComponentsLib)
    target_link_libraries(BangLib PUBLIC BangGraphicsLib)
    target_link_libraries(BangLib PUBLIC BangMathLib)
endif()

target_link_libraries(BangLib PUBLIC ${DEPENDENCIES_LIBS})

add_dependencies(BangEngineObjects BuildDependencies)
add_dependencies(BangLib BuildDependencies)

#=================================================================
#=================================================================
#=================================================================

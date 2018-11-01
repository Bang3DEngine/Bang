#=================================================================
# Include guard ==================================================
#=================================================================
if (DEFINED BANG_CHECK_INPUT_PARAMS_BUILT)
    return()
endif()
set(BANG_CHECK_INPUT_PARAMS_BUILT ON)
#=================================================================
#=================================================================
#=================================================================

# BuildSharedLibs
if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS OFF)
    message("No BUILD_SHARED_LIBS specified. Assuming '${BUILD_SHARED_LIBS}'.")
endif()

# CMakeBuildType
if ((NOT DEFINED CMAKE_BUILD_TYPE) OR ("${CMAKE_BUILD_TYPE}" STREQUAL ""))
    set(CMAKE_BUILD_TYPE "Release")
    message("No CMAKE_BUILD_TYPE specified. Assuming '${CMAKE_BUILD_TYPE}'.")
endif()

if ( NOT("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")    AND
     NOT("${CMAKE_BUILD_TYPE}" STREQUAL "Release")  AND
     NOT("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
   )
    message(FATAL_ERROR "'${CMAKE_BUILD_TYPE}' is not a valid build type. Please specify on of the following -DCMAKE_BUILD_TYPE=(Debug|Release|RelWithDebInfo)")
endif()

#=================================================================
# CMake options ==================================================
#=================================================================
cmake_minimum_required(VERSION 2.8)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Include guard ==================================================
#=================================================================
if (DEFINED BANG_DEPENDENCIES_BUILT)
    return()
endif()
set(BANG_DEPENDENCIES_BUILT ON)
#=================================================================
#=================================================================
#=================================================================

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../CMakeIncludes")
add_custom_target(BuildDependencies)

if (${BUILD_SHARED_LIBS})

    find_package(GLEW REQUIRED)
    find_package(OpenAL REQUIRED)
    find_package(PNG REQUIRED)
    find_package(JPEG REQUIRED)
    find_package(MiniZip REQUIRED)

    find_library(ASSIMP_LIBRARY "assimp")

    find_package(SDL2 REQUIRED)
    string(STRIP "${SDL2_LIBRARY}" SDL2_LIBRARY)
    string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
    # string(REPLACE "-lpthread" "" SDL2_LIBRARY "${SDL2_LIBRARY}")

    find_package(SDL2_ttf REQUIRED)
    string(STRIP "${SDL2_TTF_LIBRARY}" SDL2_TTF_LIBRARY)
    string(STRIP "${SDL2_TTF_LIBRARIES}" SDL2_TTF_LIBRARIES)

    list(APPEND DEPENDENCIES_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIR})
    list(APPEND DEPENDENCIES_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIRS})
    list(APPEND DEPENDENCIES_INCLUDE_DIRS ${OPENAL_INCLUDE_DIRS})
    list(APPEND DEPENDENCIES_INCLUDE_DIRS ${GLEW_INCLUDE_DIRS})
    list(APPEND DEPENDENCIES_INCLUDE_DIRS ${PNG_INCLUDE_DIRS})
    list(APPEND DEPENDENCIES_INCLUDE_DIRS ${JPEG_INCLUDE_DIRS})
    list(APPEND DEPENDENCIES_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    list(APPEND DEPENDENCIES_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})

    list(APPEND DEPENDENCIES_LIB_PATHS ${PNG_LIBRARIES})
    list(APPEND DEPENDENCIES_LIB_PATHS ${OPENAL_LIBRARY})
    list(APPEND DEPENDENCIES_LIB_PATHS ${JPEG_LIBRARIES})
    list(APPEND DEPENDENCIES_LIB_PATHS ${GLEW_LIBRARIES})
    list(APPEND DEPENDENCIES_LIB_PATHS ${SDL2_LIBRARY})
    list(APPEND DEPENDENCIES_LIB_PATHS ${SDL2_TTF_LIBRARIES})
    list(APPEND DEPENDENCIES_LIB_PATHS ${ASSIMP_LIBRARY})
    list(APPEND DEPENDENCIES_LIB_PATHS "${MINIZIP_LIBRARY}")

else ()

    include("${CMAKE_CURRENT_LIST_DIR}/ThirdParty/CMakeLists.txt")
    list(APPEND DEPENDENCIES_INCLUDE_DIRS ${THIRD_PARTY_INCLUDE_DIRS})
    list(APPEND DEPENDENCIES_LIB_PATHS    ${THIRD_PARTY_LIB_PATHS})

    add_dependencies(BuildDependencies BuildThirdParty)

endif()

# Common shared dependencies ===================
set(SEARCH_LIB_PATHS "")
list(APPEND SEARCH_LIB_PATHS "/lib")
list(APPEND SEARCH_LIB_PATHS "/lib64")
list(APPEND SEARCH_LIB_PATHS "/usr/lib")
list(APPEND SEARCH_LIB_PATHS "/usr/lib64")
list(APPEND SEARCH_LIB_PATHS "/usr/lib/x86_64-linux-gnu")
list(APPEND SEARCH_LIB_PATHS "/usr/lib/i386-linux-gnu")

find_package(OpenGL REQUIRED)
if (UNIX)
    find_package(ZLIB REQUIRED) #  "zlib"    PATHS ${SEARCH_LIB_PATHS})
    find_library(DL_LIBRARY      REQUIRED "dl"      PATHS ${SEARCH_LIB_PATHS})
    find_library(BSD_LIBRARY     REQUIRED "bsd"     PATHS ${SEARCH_LIB_PATHS})
    find_library(SNDIO_LIBRARY   REQUIRED "sndio"   PATHS ${SEARCH_LIB_PATHS})
    find_library(SNDFILE_LIBRARY REQUIRED "sndfile" PATHS ${SEARCH_LIB_PATHS})
    find_library(PTHREAD_LIBRARY REQUIRED "pthread" PATHS ${SEARCH_LIB_PATHS})
else()

endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    if ("${USE_SANITIZER}")
        find_library(ASAN_LIBRARY  REQUIRED NAMES asan libasan.so.4 libasan.so.3 libasan.so.2 libasan.so.1 libasan.so)
        find_library(UBSAN_LIBRARY REQUIRED NAMES ubsan libubsan.so.2 libubsan.so.1 libubsan.so.0 libubsan.so)
        list(APPEND DEPENDENCIES_LIB_PATHS "${ASAN_LIBRARY}")
        list(APPEND DEPENDENCIES_LIB_PATHS "${UBSAN_LIBRARY}")
    endif()
endif()

list(APPEND DEPENDENCIES_INCLUDE_DIRS "${OPENGL_INCLUDE_DIRS}")

list(APPEND DEPENDENCIES_LIB_PATHS "${OPENGL_LIBRARY}")
if (UNIX)
    list(APPEND DEPENDENCIES_LIB_PATHS "${DL_LIBRARY}")
    list(APPEND DEPENDENCIES_LIB_PATHS "${BSD_LIBRARY}")
    list(APPEND DEPENDENCIES_LIB_PATHS "${ZLIB_LIBRARY}")
    list(APPEND DEPENDENCIES_LIB_PATHS "${SNDIO_LIBRARY}")
    list(APPEND DEPENDENCIES_LIB_PATHS "${SNDFILE_LIBRARY}")
    list(APPEND DEPENDENCIES_LIB_PATHS "${PTHREAD_LIBRARY}")
else()
endif()
# =================================================


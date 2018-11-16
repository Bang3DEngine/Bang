#=================================================================
# CMake options ==================================================
#=================================================================
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)
project(BangThirdParty C CXX)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Variables ======================================================
#=================================================================
# General
set(NUM_COMPILE_THREADS 4)

# Assimp
set(ASSIMP_DIR "${CMAKE_CURRENT_LIST_DIR}/assimp")
set(ASSIMP_INCLUDE_DIR_0 "${ASSIMP_DIR}/include")
set(ASSIMP_INCLUDE_DIR_1 "${ASSIMP_DIR}/build/include")
set(ASSIMP_LIB_OUTPUT_DIR  "${ASSIMP_DIR}/build/code")
set(ASSIMP_LIB_OUTPUT_NAME "libassimp.a")
set(ASSIMP_LIB_OUTPUT "${ASSIMP_LIB_OUTPUT_DIR}/${ASSIMP_LIB_OUTPUT_NAME}")
set(ASSIMP_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${ASSIMP_LIB_OUTPUT_NAME}")

# Minizip
set(MINIZIP_DIR "${CMAKE_CURRENT_LIST_DIR}/minizip")
set(MINIZIP_LIB_OUTPUT_DIR  "${MINIZIP_DIR}/build/")
set(MINIZIP_LIB_OUTPUT_NAME "libminizip.a")
set(MINIZIP_LIB_OUTPUT "${MINIZIP_LIB_OUTPUT_DIR}/${MINIZIP_LIB_OUTPUT_NAME}")
set(MINIZIP_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${MINIZIP_LIB_OUTPUT_NAME}")

# IRRXML (used by Assimp)
set(IRRXML_DIR "${ASSIMP_DIR}/build/contrib/irrXML")
set(IRRXML_LIB_OUTPUT_DIR  "${IRRXML_DIR}")
set(IRRXML_LIB_OUTPUT_NAME "libIrrXML.a")
set(IRRXML_LIB_OUTPUT "${IRRXML_LIB_OUTPUT_DIR}/${IRRXML_LIB_OUTPUT_NAME}")
set(IRRXML_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${IRRXML_LIB_OUTPUT_NAME}")

# Freetype2
set(FREETYPE_DIR "${CMAKE_CURRENT_LIST_DIR}/freetype-2.4.0")
set(FREETYPE_INSTALL_DIR_NAME "freetype-install")
set(FREETYPE_INSTALL_DIR "${FREETYPE_DIR}/${FREETYPE_INSTALL_DIR_NAME}")
set(FREETYPE_LIB_OUTPUT_DIR  "${FREETYPE_INSTALL_DIR}/lib*")
set(FREETYPE_LIB_OUTPUT_NAME "libfreetype.a")
set(FREETYPE_LIB_OUTPUT  "${FREETYPE_LIB_OUTPUT_DIR}/${FREETYPE_LIB_OUTPUT_NAME}")
set(FREETYPE_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FREETYPE_LIB_OUTPUT_NAME}")

# GLEW
set(GLEW_DIR "${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0")
set(GLEW_INCLUDE_DIR "${GLEW_DIR}/include")
set(GLEW_LIB_OUTPUT_DIR  "${GLEW_DIR}/lib*")
set(GLEW_LIB_OUTPUT_NAME "libGLEW.a")
set(GLEW_LIB_OUTPUT  "${GLEW_LIB_OUTPUT_DIR}/${GLEW_LIB_OUTPUT_NAME}")
set(GLEW_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${GLEW_LIB_OUTPUT_NAME}")

# JPEG
set(JPEG_DIR "${CMAKE_CURRENT_LIST_DIR}/libjpeg-turbo")
set(JPEG_INSTALL_DIR "${JPEG_DIR}/jpeg-install")
set(JPEG_INCLUDE_DIR_0 "${JPEG_DIR}")
set(JPEG_INCLUDE_DIR_1 "${JPEG_DIR}/build")
set(JPEG_LIB_OUTPUT_DIR  "${JPEG_INSTALL_DIR}/lib*")
set(JPEG_LIB_OUTPUT_NAME "libturbojpeg.a")
set(JPEG_LIB_OUTPUT  "${JPEG_LIB_OUTPUT_DIR}/${JPEG_LIB_OUTPUT_NAME}")
set(JPEG_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${JPEG_LIB_OUTPUT_NAME}")

# PNG
set(PNG_DIR "${CMAKE_CURRENT_LIST_DIR}/libpng-1.6.34")
set(PNG_INSTALL_DIR "${PNG_DIR}/png-install")
set(PNG_INCLUDE_DIR_0 "${PNG_DIR}")
set(PNG_INCLUDE_DIR_1 "${PNG_DIR}/build")
set(PNG_LIB_OUTPUT_DIR  "${PNG_INSTALL_DIR}/lib*")
set(PNG_LIB_OUTPUT_NAME "libpng.a")
set(PNG_LIB_OUTPUT  "${PNG_LIB_OUTPUT_DIR}/${PNG_LIB_OUTPUT_NAME}")
set(PNG_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PNG_LIB_OUTPUT_NAME}")

# SNDIO
set(SNDIO_DIR "${CMAKE_CURRENT_LIST_DIR}/sndio")
set(SNDIO_LIB_OUTPUT_DIR  "${SNDIO_DIR}/build")
set(SNDIO_LIB_OUTPUT_NAME "libsndio.a")
set(SNDIO_LIB_OUTPUT  "${SNDIO_LIB_OUTPUT_DIR}/${SNDIO_LIB_OUTPUT_NAME}")
set(SNDIO_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SNDIO_LIB_OUTPUT_NAME}")

# OPENAL
set(OPENAL_DIR "${CMAKE_CURRENT_LIST_DIR}/openal-soft")
set(OPENAL_INCLUDE_DIR "${OPENAL_DIR}/include")
set(OPENAL_LIB_OUTPUT_DIR  "${OPENAL_DIR}/build")
set(OPENAL_LIB_OUTPUT_NAME "libopenal.a")
set(OPENAL_LIB_OUTPUT  "${OPENAL_LIB_OUTPUT_DIR}/${OPENAL_LIB_OUTPUT_NAME}")
set(OPENAL_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${OPENAL_LIB_OUTPUT_NAME}")

# SDL2
set(SDL2_DIR "${CMAKE_CURRENT_LIST_DIR}/SDL2-2.0.8")
set(SDL2_INSTALL_DIR "${SDL2_DIR}/sdl2-install")
set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
set(SDL2_LIB_OUTPUT_DIR  "${SDL2_DIR}/build")
set(SDL2_LIB_OUTPUT_NAME "libSDL2.a")
set(SDL2_LIB_OUTPUT  "${SDL2_LIB_OUTPUT_DIR}/${SDL2_LIB_OUTPUT_NAME}")
set(SDL2_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SDL2_LIB_OUTPUT_NAME}")

# SDL2_ttf
set(SDL2TTF_DIR "${CMAKE_CURRENT_LIST_DIR}/SDL2_ttf-2.0.14")
set(SDL2TTF_INSTALL_DIR "${SDL2TTF_DIR}/sdl2-ttf-install")
set(SDL2TTF_INCLUDE_DIR "${SDL2TTF_DIR}")
set(SDL2TTF_LIB_OUTPUT_DIR  "${SDL2TTF_INSTALL_DIR}/lib*")
set(SDL2TTF_LIB_OUTPUT_NAME "libSDL2_ttf.a")
set(SDL2TTF_LIB_OUTPUT  "${SDL2TTF_LIB_OUTPUT_DIR}/${SDL2TTF_LIB_OUTPUT_NAME}")
set(SDL2TTF_LIB_FINAL_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SDL2TTF_LIB_OUTPUT_NAME}")
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Assimp Compile =================================================
#=================================================================
add_custom_command(
  OUTPUT ${ASSIMP_LIB_FINAL_OUTPUT}
  COMMAND
      echo "Executing ASSIMP CMake ------------" &&
      cd ${ASSIMP_DIR} &&
      mkdir -p build &&
      cd build &&
      cmake
        -DBUILD_SHARED_LIBS=OFF
        -DSYSTEM_IRRXML=OFF
        -DASSIMP_BUILD_ZLIB=OFF
        -DASSIMP_BUILD_TESTS=OFF
        -DASSIMP_BUILD_SAMPLES=OFF
        -DASSIMP_ASAN=OFF
        -DASSIMP_UBSAN=OFF
        -DASSIMP_NO_EXPORT=OFF
        -DASSIMP_BUILD_OFF_IMPORTER=OFF
        -DASSIMP_BUILD_OGRE_IMPORTER=OFF
        -DASSIMP_BUILD_OPENGEX_IMPORTER=OFF
        -DASSIMP_BUILD_Q3BSP_IMPORTER=OFF
        -DASSIMP_BUILD_Q3D_IMPORTER=OFF
        -DASSIMP_BUILD_RAW_IMPORTER=OFF
        -DASSIMP_BUILD_SMD_IMPORTER=OFF
        -DASSIMP_BUILD_SIB_IMPORTER=OFF
        -DASSIMP_BUILD_TERRAGEN_IMPORTER=OFF
        -DASSIMP_BUILD_AC_IMPORTER=OFF
        -DASSIMP_BUILD_AMF_IMPORTER=OFF
        -DASSIMP_BUILD_ASE_IMPORTER=OFF
        -DASSIMP_BUILD_ASSBIN_IMPORTER=OFF
        -DASSIMP_BUILD_ASSXML_IMPORTER=OFF
        -DASSIMP_BUILD_B3D_IMPORTER=OFF
        -DASSIMP_BUILD_COB_IMPORTER=OFF
        -DASSIMP_BUILD_CSM_IMPORTER=OFF
        -DASSIMP_BUILD_DFX_IMPORTER=OFF
        -DASSIMP_BUILD_GLTF_IMPORTER=OFF
        -DASSIMP_BUILD_HMP_IMPORTER=OFF
        -DASSIMP_BUILD_IFC_IMPORTER=OFF
        -DASSIMP_BUILD_IRRMESH_IMPORTER=OFF
        -DASSIMP_BUILD_IRR_IMPORTER=OFF
        -DASSIMP_BUILD_LWO_IMPORTER=OFF
        -DASSIMP_BUILD_LWS_IMPORTER=OFF
        -DASSIMP_BUILD_MD2_IMPORTER=OFF
        -DASSIMP_BUILD_MD3_IMPORTER=OFF
        -DASSIMP_BUILD_MD5_IMPORTER=OFF
        -DASSIMP_BUILD_MDL_IMPORTER=OFF
        -DASSIMP_BUILD_MMD_IMPORTER=OFF
        -DASSIMP_BUILD_MS3D_IMPORTER=OFF
        -DASSIMP_BUILD_NDO_IMPORTER=OFF
        -DASSIMP_BUILD_NDF_IMPORTER=OFF
        -DASSIMP_BUILD_X3D_IMPORTER=OFF
        -DASSIMP_BUILD_XGL_IMPORTER=OFF
        -DASSIMP_BUILD_X_IMPORTER=OFF
        -DBUILD_FRAMEWORK=OFF
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
        .. &&
        make -j${NUM_COMPILE_THREADS} &&
        /bin/bash -c "cp ${ASSIMP_LIB_OUTPUT} ${ASSIMP_LIB_FINAL_OUTPUT}"
  COMMENT "Building Assimp"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# MINIZIP Compile ================================================
#=================================================================
add_custom_command(
  OUTPUT ${MINIZIP_LIB_FINAL_OUTPUT}
  COMMAND
      echo "Executing MINIZIP CMake ------------" &&
      cd ${MINIZIP_DIR} &&
      mkdir -p build &&
      cd build &&
      cmake -DBUILD_SHARED_LIBS=OFF
            -DUSE_AES=OFF
            -DCMAKE_C_FLAGS=-fPIC\ -DNOCRYPT
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            .. &&
      make -j${NUM_COMPILE_THREADS} &&
      /bin/bash -c "cp ${MINIZIP_LIB_OUTPUT} ${MINIZIP_LIB_FINAL_OUTPUT}"
  COMMENT "Building MINIZIP"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# IRRXML Copy when Assimp has built it ===========================
#=================================================================
add_custom_command(
  OUTPUT ${IRRXML_LIB_FINAL_OUTPUT}
  DEPENDS ${ASSIMP_LIB_FINAL_OUTPUT}
  COMMAND /bin/bash -c "cp ${IRRXML_LIB_OUTPUT} ${IRRXML_LIB_FINAL_OUTPUT}"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Freetype Compile ===============================================
#=================================================================
add_custom_command(
  OUTPUT ${FREETYPE_LIB_FINAL_OUTPUT}
  COMMAND
      echo "Configuring FREETYPE ------------" &&
      cd ${FREETYPE_DIR} &&
      sh autogen.sh &&
      mkdir -p ${FREETYPE_INSTALL_DIR} &&
      mkdir -p ${FREETYPE_INSTALL_DIR}/include/freetype2/freetype/internal/ &&
      ./configure
            --prefix=${FREETYPE_INSTALL_DIR}
            --with-pic &&
      make -j${NUM_COMPILE_THREADS} &&
      make install &&
      /bin/bash -c "cp ${FREETYPE_LIB_OUTPUT} ${FREETYPE_LIB_FINAL_OUTPUT}"
  COMMENT "Building Freetype"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# GLEW Compile ===================================================
#=================================================================
add_custom_command(
  OUTPUT ${GLEW_LIB_FINAL_OUTPUT}
  COMMAND
      cd ${GLEW_DIR} &&
      make -j${NUM_COMPILE_THREADS} &&
      /bin/bash -c "cp ${GLEW_LIB_OUTPUT} ${GLEW_LIB_FINAL_OUTPUT}"
  COMMENT "Building GLEW"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# JPEG Compile ===================================================
#=================================================================
add_custom_command(
  OUTPUT ${JPEG_LIB_FINAL_OUTPUT}
  COMMAND
      echo "Executing JPEG CMake ------------" &&
      cd ${JPEG_DIR} &&
      mkdir -p build &&
      cd build &&
      export PATH=$ENV{PATH}:/usr/lib/x86_64-linux-gnu/ &&
      cmake
            -DCMAKE_INSTALL_PREFIX=${JPEG_INSTALL_DIR}
            -DBUILD_SHARED_LIBS=OFF -DWITH_SIMD=OFF
            -DREQUIRE_SIMD=OFF
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            ..  &&
      export PATH=$ENV{PATH}:/usr/lib/x86_64-linux-gnu/ &&
      make -j${NUM_COMPILE_THREADS} &&
      make install &&
      /bin/bash -c "cp ${JPEG_LIB_OUTPUT} ${JPEG_LIB_FINAL_OUTPUT}"
  COMMENT "Building JPEG"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# PNG Compile ====================================================
#=================================================================
add_custom_command(
  OUTPUT ${PNG_LIB_FINAL_OUTPUT}
  COMMAND
      echo "Executing PNG CMake ------------" &&
      cd ${PNG_DIR} &&
      mkdir -p build &&
      cd build &&
      export PATH=$ENV{PATH}:/usr/lib/x86_64-linux-gnu/ &&
      cmake
            -DCMAKE_INSTALL_PREFIX=${PNG_INSTALL_DIR}
            -DBUILD_SHARED_LIBS=OFF
            -DCMAKE_C_FLAGS="-fPIC"
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            ..  &&
      export PATH=$ENV{PATH}:/usr/lib/x86_64-linux-gnu/ &&
      make -j${NUM_COMPILE_THREADS} &&
      make install &&
      /bin/bash -c "cp ${PNG_LIB_OUTPUT} ${PNG_LIB_FINAL_OUTPUT}"
  COMMENT "Building PNG"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# SNDIO Compile =================================================
#=================================================================
add_custom_command(
  OUTPUT ${SNDIO_LIB_FINAL_OUTPUT}
  COMMAND
      echo "Executing SNDIO CMake ------------" &&
      cd ${SNDIO_DIR} &&
      mkdir -p build &&
      cd build &&
      cmake -DCMAKE_CXX_FLAGS=-fPIC
            -DCMAKE_C_FLAGS=-fPIC
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            .. &&
      make -j${NUM_COMPILE_THREADS} &&
      /bin/bash -c "cp ${SNDIO_LIB_OUTPUT} ${SNDIO_LIB_FINAL_OUTPUT}"
  COMMENT "Building SNDIO"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# OPENAL Compile =================================================
#=================================================================
add_custom_command(
  OUTPUT ${OPENAL_LIB_FINAL_OUTPUT}
  DEPENDS ${SNDIO_LIB_FINAL_OUTPUT}
  COMMAND
      echo "Executing OPENAL CMake ------------" &&
      cd ${OPENAL_DIR} &&
      mkdir -p build &&
      cd build &&
      cmake -DBUILD_SHARED_LIBS=OFF
            -DLIBTYPE=STATIC
            -DCMAKE_CXX_FLAGS=-fPIC
            -DCMAKE_C_FLAGS=-fPIC
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            ..   &&
      make -j${NUM_COMPILE_THREADS} &&
      /bin/bash -c "cp ${OPENAL_LIB_OUTPUT} ${OPENAL_LIB_FINAL_OUTPUT}"
  COMMENT "Building OpenAL"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# SDL2 Compile ===================================================
#=================================================================
add_custom_command(
  OUTPUT ${SDL2_LIB_FINAL_OUTPUT}
  COMMAND
      echo "Executing SDL2 CMake ------------" &&
      cd ${SDL2_DIR} &&
      mkdir -p ${SDL2_INSTALL_DIR} &&
      mkdir -p build &&
      cd build &&
      cmake
          -DSDL_STATIC_PIC=ON
          -DSNDIO=OFF
          -DBUILD_SHARED_LIBS=OFF
          -DCMAKE_INSTALL_PREFIX=${SDL2_INSTALL_DIR}
          -DCMAKE_CXX_FLAGS=-fPIC
          -DCMAKE_C_FLAGS=-fPIC
          -DCMAKE_POSITION_INDEPENDENT_CODE=ON
          ..  &&
      make -j${NUM_COMPILE_THREADS} &&
      make install &&
      /bin/bash -c "cp ${SDL2_LIB_OUTPUT} ${SDL2_LIB_FINAL_OUTPUT}"
  COMMENT "Building SDL2"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# SDL2TTF Compile ================================================
#=================================================================
add_custom_command(
  OUTPUT ${SDL2TTF_LIB_FINAL_OUTPUT}
  DEPENDS ${SDL2_LIB_FINAL_OUTPUT} ${FREETYPE_LIB_FINAL_OUTPUT}
  COMMAND
      export PATH=$ENV{PATH}:${SDL2_INSTALL_DIR}:${SDL2TTF_INSTALL_DIR} &&
      export SDL2_CONFIG=${SDL2_INSTALL_DIR}/bin/sdl2-config &&
      /bin/bash -c "cp ${FREETYPE_INSTALL_DIR}/lib*/libfreetype.so.6 ${SDL2_INSTALL_DIR}/lib*/" &&
      cd ${SDL2TTF_DIR} &&
      sh autogen.sh &&
      ./configure CPPFLAGS=-I${SDL2_INCLUDE_DIR}
                  --with-freetype-prefix=${FREETYPE_INSTALL_DIR}
                  --prefix=${SDL2TTF_INSTALL_DIR}
                  --enable-static=yes
                  --with-pic &&
      make -j${NUM_COMPILE_THREADS} &&
      make install &&
      /bin/bash -c "cp ${SDL2TTF_LIB_OUTPUT} ${SDL2TTF_LIB_FINAL_OUTPUT}"
  COMMENT "Building SDL2TTF"
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Create output lists ============================================
#=================================================================
list(APPEND THIRD_PARTY_LIB_PATHS ${ASSIMP_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${JPEG_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${PNG_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${MINIZIP_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${OPENAL_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${SNDIO_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${SDL2_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${SDL2TTF_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${GLEW_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${IRRXML_LIB_FINAL_OUTPUT})
list(APPEND THIRD_PARTY_LIB_PATHS ${FREETYPE_LIB_FINAL_OUTPUT})

list(APPEND THIRD_PARTY_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
list(APPEND THIRD_PARTY_INCLUDE_DIRS ${ASSIMP_INCLUDE_DIR_0})
list(APPEND THIRD_PARTY_INCLUDE_DIRS ${ASSIMP_INCLUDE_DIR_1})
list(APPEND THIRD_PARTY_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
list(APPEND THIRD_PARTY_INCLUDE_DIRS ${JPEG_INCLUDE_DIR_0})
list(APPEND THIRD_PARTY_INCLUDE_DIRS ${JPEG_INCLUDE_DIR_1})
list(APPEND THIRD_PARTY_INCLUDE_DIRS ${PNG_INCLUDE_DIR_0})
list(APPEND THIRD_PARTY_INCLUDE_DIRS ${PNG_INCLUDE_DIR_1})
list(APPEND THIRD_PARTY_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
list(APPEND THIRD_PARTY_INCLUDE_DIRS ${SDL2TTF_INCLUDE_DIR})

add_custom_target(BuildThirdParty ALL
    DEPENDS ${ASSIMP_LIB_FINAL_OUTPUT}
    DEPENDS ${PNG_LIB_FINAL_OUTPUT}
    DEPENDS ${MINIZIP_LIB_FINAL_OUTPUT}
    DEPENDS ${IRRXML_LIB_FINAL_OUTPUT}
    DEPENDS ${JPEG_LIB_FINAL_OUTPUT}
    DEPENDS ${OPENAL_LIB_FINAL_OUTPUT}
    DEPENDS ${SDL2_LIB_FINAL_OUTPUT}
    DEPENDS ${SDL2TTF_LIB_FINAL_OUTPUT}
    DEPENDS ${GLEW_LIB_FINAL_OUTPUT}
    DEPENDS ${FREETYPE_LIB_FINAL_OUTPUT}
    DEPENDS ${THIRD_PARTY_LIB_PATHS})
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Copy all libraries to BANG_ENGINE_LIBRARIES_DIR ================
#=================================================================
set(ALL_NEW_OUTPUT_LIB_PATHS "")

foreach(OUTPUT_LIB_PATH ${THIRD_PARTY_LIB_PATHS})
    get_filename_component(OUTPUT_LIB_NAME ${OUTPUT_LIB_PATH} NAME)
    set(NEW_OUTPUT_LIB_PATH "${BANG_ENGINE_LIBRARIES_DIR}/${OUTPUT_LIB_NAME}")
    add_custom_command(
      OUTPUT ${NEW_OUTPUT_LIB_PATH}
      COMMAND
        ${CMAKE_COMMAND} -E copy ${OUTPUT_LIB_PATH} ${NEW_OUTPUT_LIB_PATH}
      DEPENDS ${OUTPUT_LIB_PATH}
      VERBATIM
    )
    list(APPEND ALL_NEW_OUTPUT_LIB_PATHS ${NEW_OUTPUT_LIB_PATH})
endforeach()

add_custom_target(CopyThirdPartyLibraries ALL
                     DEPENDS BuildThirdParty
                     DEPENDS ${ALL_NEW_OUTPUT_LIB_PATHS})
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Check Force rebuild ============================================
#=================================================================
# if (${FORCE_REBUILD}) # If force rebuild, remove all outputlibs
#     foreach(OUTPUT_LIB_FILE ${THIRD_PARTY_LIB_PATHS})
#         file(REMOVE ${OUTPUT_LIB_FILE})
#     endforeach()
# endif()
#=================================================================
#=================================================================
#=================================================================


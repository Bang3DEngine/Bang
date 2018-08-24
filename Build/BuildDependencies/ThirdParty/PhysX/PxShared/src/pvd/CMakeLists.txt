cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

# -----------------------------------------------------------------------------------------------------------------------------
# FLAGS
# -----------------------------------------------------------------------------------------------------------------------------

if(APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPxShared_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPxShared_STATIC_LIB -DNDEBUG -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -g3 -gdwarf-2 -O0")
elseif(WIN32)
    # TODO: Set PX_PHYSX_CORE_EXPORTS flag and enable delay loads.
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc:wchar_t /D \"PX_PVDSDK_DLL=1\" /D \"PX_FOUNDATION_DLL=1\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /D \"_WINDLL\" /GF /Zc:forScope /GR- /Gd /MT /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+ /Zc:inline /fp:fast")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zc:wchar_t /D \"PX_PVDSDK_DLL=1\" /D \"PX_FOUNDATION_DLL=1\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"_DEBUG\" /D \"PX_DEBUG=1\" /D \"PX_CHECKED=1\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /D \"PX_PHYSX_DLL_NAME_POSTFIX=DEBUG\" /D \"_WINDLL\" /GF /Zc:forScope /RTCu /GR- /Gd /MTd /Zc:inline /fp:fast /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+")
elseif(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPxShared_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -pipe -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPxShared_STATIC_LIB -DNDEBUG -pipe -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -g3 -O0")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -DPX_PHYSX_STATIC_LIB -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -msse2 -ffast-math -fno-exceptions -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -Wno-strict-aliasing -g3 -O3 -std=c++11 -fPIC ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC -DPX_PHYSX_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -msse2 -ffast-math -fno-exceptions -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -Wno-strict-aliasing -g3 -O0 -std=c++11 -fPIC ")
endif()

# -----------------------------------------------------------------------------------------------------------------------------
# FILES
# -----------------------------------------------------------------------------------------------------------------------------

# Headers
file(GLOB_RECURSE PXPVDSDK_HEADERS ${PHYSX_ROOT_DIR}/PxShared/include/pvd/*.h ${PROJECT_SOURCE_DIR}/src/PxShared/src/pvd/include/*.h ${PROJECT_SOURCE_DIR}/src/PxShared/src/pvd/src/*.h)

# Sources
file(GLOB_RECURSE PXPVDSDK_SOURCES ${PHYSX_ROOT_DIR}/PxShared/src/pvd/src/*.cpp)

if(UNIX OR APPLE)
    list(REMOVE_ITEM PXPVDSDK_HEADERS "${PHYSX_ROOT_DIR}/PxShared/include/pvd/windows/PxWindowsPvdDelayLoadHook.h")
    list(REMOVE_ITEM PXPVDSDK_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/pvd/src/windows/PxWindowsPvdDelayLoadHook.cpp")
endif()

# Source groups
source_group("Headers" FILES ${PXPVDSDK_HEADERS})
source_group("Sources" FILES ${PXPVDSDK_SOURCES})

# -----------------------------------------------------------------------------------------------------------------------------
# TARGETS
# -----------------------------------------------------------------------------------------------------------------------------

if(WIN32)
    add_library(PxPvdSDK SHARED ${PXPVDSDK_HEADERS} ${PXPVDSDK_SOURCES})
    target_link_libraries(PxPvdSDK PxFoundation)
    target_link_libraries(PxPvdSDK "${PHYSX_ROOT_DIR}/Externals/nvToolsExt/1/lib/x64/nvToolsExt64_1.lib")
else()
    add_library(PxPvdSDK ${PXPVDSDK_HEADERS} ${PXPVDSDK_SOURCES})
endif()

set_target_properties(PxPvdSDK PROPERTIES FOLDER "PhysX")

target_include_directories(PxPvdSDK PUBLIC "${PHYSX_ROOT_DIR}/PxShared/include")
target_include_directories(PxPvdSDK PUBLIC "${PHYSX_ROOT_DIR}/PxShared/src/pvd/include")
target_include_directories(PxPvdSDK PUBLIC "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include")
target_include_directories(PxPvdSDK PUBLIC "${PHYSX_ROOT_DIR}/PxShared/src/filebuf/include")
target_include_directories(PxPvdSDK PUBLIC "${PHYSX_ROOT_DIR}/Externals/nvToolsExt/1/include")

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

# -----------------------------------------------------------------------------------------------------------------------------
# FLAGS
# -----------------------------------------------------------------------------------------------------------------------------

if(APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPX_PHYSX_STATIC_LIB -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPX_PHYSX_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -g3 -gdwarf-2 -O0")
elseif(WIN32)
    # TODO: Set PX_PHYSX_CORE_EXPORTS flag and enable delay loads.
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc:wchar_t /D \"PX_FOUNDATION_DLL=1\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /D \"_WINDLL\" /GF /Zc:forScope /GR- /Gd /MT /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+ /Zc:inline /fp:fast")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zc:wchar_t /D \"PX_FOUNDATION_DLL=1\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"_DEBUG\" /D \"PX_DEBUG=1\" /D \"PX_CHECKED=1\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /D \"PX_PHYSX_DLL_NAME_POSTFIX=DEBUG\" /D \"_WINDLL\" /GF /Zc:forScope /RTCu /GR- /Gd /MTd /Zc:inline /fp:fast /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+")
elseif(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPX_PHYSX_STATIC_LIB -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPX_PHYSX_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything -Wno-documentation-deprecated-sync -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -g3 -O0")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -DPX_PHYSX_STATIC_LIB -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -msse2 -ffast-math -fno-exceptions -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -Wno-strict-aliasing -g3 -O3 -std=c++11 -fPIC ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC -DPX_PHYSX_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -msse2 -ffast-math -fno-exceptions -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -Wno-strict-aliasing -g3 -O0 -std=c++11 -fPIC ")
endif()

# -----------------------------------------------------------------------------------------------------------------------------
# FILES
# -----------------------------------------------------------------------------------------------------------------------------

# Headers
file(GLOB_RECURSE PXFOUNDATION_HEADERS ${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/*.h ${PROJECT_SOURCE_DIR}/src/PxShared/src/foundation/src/*.h)

# Sources
file(GLOB_RECURSE PXFOUNDATION_SOURCES ${PHYSX_ROOT_DIR}/PxShared/src/foundation/*.cpp
                                       ${PHYSX_ROOT_DIR}/PxShared/src/foundation/unix/*.cpp)

if(WIN32)
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/unix/neon/PsUnixNeonAoS.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/unix/neon/PsUnixNeonInlineAoS.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/unix/PsUnixAoS.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/unix/PsUnixFPU.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/unix/PsUnixInlineAoS.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/unix/PsUnixIntrinsics.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/unix/PsUnixTrigConstants.h")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixAtomic.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixCpu.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixFPU.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixMutex.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixPrintString.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixSList.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixSocket.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixSync.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixThread.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/unix/PsUnixTime.cpp")
elseif(UNIX OR APPLE OR EMSCRIPTEN)
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/windows/PsWindowsAoS.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/windows/PsWindowsFPU.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/windows/PsWindowsInclude.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/windows/PsWindowsInlineAoS.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/windows/PsWindowsLoadLibrary.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/windows/PsWindowsIntrinsics.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/windows/PsWindowsTrigConstants.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/unix/neon/PsUnixNeonAoS.h")
    list(REMOVE_ITEM PXFOUNDATION_HEADERS "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include/unix/neon/PsUnixNeonInlineAoS.h")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsAtomic.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsCpu.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsFPU.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsMutex.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsPrintString.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsSList.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsSocket.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsSync.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsThread.cpp")
    list(REMOVE_ITEM PXFOUNDATION_SOURCES "${PHYSX_ROOT_DIR}/PxShared/src/foundation/src/windows/PsWindowsTime.cpp")
endif()

# Source groups
source_group("Headers" FILES ${PXFOUNDATION_HEADERS})
source_group("Sources" FILES ${PXFOUNDATION_SOURCES})

# -----------------------------------------------------------------------------------------------------------------------------
# TARGETS
# -----------------------------------------------------------------------------------------------------------------------------

if(WIN32)
    add_library(PxFoundation SHARED ${PXFOUNDATION_HEADERS} ${PXFOUNDATION_SOURCES})
else()
    add_library(PxFoundation ${PXFOUNDATION_HEADERS} ${PXFOUNDATION_SOURCES})
endif()

set_target_properties(PxFoundation PROPERTIES FOLDER "PhysX")

target_include_directories(PxFoundation PUBLIC "${PHYSX_ROOT_DIR}/PxShared/include")
target_include_directories(PxFoundation PUBLIC "${PHYSX_ROOT_DIR}/PxShared/src/foundation/include")

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

# -----------------------------------------------------------------------------------------------------------------------------
# FLAGS
# -----------------------------------------------------------------------------------------------------------------------------

if(APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPX_BUILD_NUMBER=22017532 -fPIC -DPX_PHYSX_STATIC_LIB -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPX_BUILD_NUMBER=22017532 -fPIC -DPX_PHYSX_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -mmacosx-version-min=10.7 -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -g3 -gdwarf-2 -O0")
elseif(WIN32)
    # TODO: Set PX_PHYSX_CORE_EXPORTS flag and enable delay loads.
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D \"PX_BUILD_NUMBER=22017532\" /D \"PX_PHYSX_STATIC_LIB\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /D \"_WINDLL\" /GF /Zc:forScope /GR- /Gd /MT /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+ /Zc:inline /fp:fast")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D \"PX_BUILD_NUMBER=22017532\" /D \"PX_PHYSX_STATIC_LIB\" /D \"WIN32\" /D \"WIN64\" /D \"_CRT_SECURE_NO_DEPRECATE\" /D \"_CRT_NONSTDC_NO_DEPRECATE\" /D \"_WINSOCK_DEPRECATED_NO_WARNINGS\" /D \"_DEBUG\" /D \"PX_DEBUG=1\" /D \"PX_CHECKED=1\" /D \"PX_NVTX=1\" /D \"PX_SUPPORT_PVD=1\" /D \"PX_PHYSX_DLL_NAME_POSTFIX=DEBUG\" /D \"_WINDLL\" /GF /Zc:forScope /RTCu /GR- /Gd /MTd /Zc:inline /fp:fast /MP /Wall /wd4514 /wd4820 /wd4127 /wd4710 /wd4711 /wd4435 /wd4577 /wd4464 /d2Zi+")
elseif(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DPX_BUILD_NUMBER=22017532 -fPIC -DPX_PHYSX_STATIC_LIB -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPX_BUILD_NUMBER=22017532 -fPIC -DPX_PHYSX_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -msse2 -ffast-math -fno-exceptions -fvisibility=hidden -fvisibility-inlines-hidden -Wno-return-type-c-linkage -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic -Weverything  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -g3 -O0")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -DPX_PHYSX_STATIC_LIB -DNDEBUG -DPX_SUPPORT_PVD=0 -pipe -msse2 -ffast-math -fno-exceptions -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -Wno-strict-aliasing -g3 -O3 -std=c++11")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC -DPX_PHYSX_STATIC_LIB -D_DEBUG -DPX_DEBUG=1 -DPX_CHECKED=1 -DPX_SUPPORT_PVD=1 -pipe -msse2 -ffast-math -fno-exceptions -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing=2 -pedantic  -Wno-documentation-unknown-command -Wno-float-equal -Wno-padded -Wno-weak-vtables -Wno-cast-align -Wno-conversion -Wno-missing-variable-declarations -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-unreachable-code -Wno-unused-macros -Wno-used-but-marked-unused -Wno-weak-template-vtables -Wno-invalid-offsetof -Wno-c++11-extensions -Wno-strict-aliasing -g3 -O0 -std=c++11")
endif()

# -----------------------------------------------------------------------------------------------------------------------------
# FILES
# -----------------------------------------------------------------------------------------------------------------------------

# Headers
file(GLOB_RECURSE PHYSX_EXTENSIONS_HEADERS ${PHYSX_ROOT_DIR}/PhysX_3.4/Include/*.h ${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXExtensions/*.h)

# Sources
file(GLOB_RECURSE PHYSX_EXTENSIONS_SOURCES ${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXExtensions/*.cpp)

list(APPEND PHYSX_EXTENSIONS_SOURCES ${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXMetaData/extensions/src/PxExtensionAutoGeneratedMetaDataObjects.cpp)

if(UNIX OR APPLE)
    list(REMOVE_ITEM PHYSX_EXTENSIONS_SOURCES "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysX/windows/NpWindowsDelayLoadHook.cpp")
    list(REMOVE_ITEM PHYSX_EXTENSIONS_SOURCES "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysX/device/windows/PhysXIndicatorWindows.cpp")
    list(REMOVE_ITEM PHYSX_EXTENSIONS_SOURCES "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysX/device/windows/PhysXIndicatorWindows.cpp")
endif()

if(WIN32 OR APPLE)
    list(REMOVE_ITEM PHYSX_EXTENSIONS_SOURCES "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysX/device/linux/PhysXIndicatorLinux.cpp")
endif()

# Source groups
source_group("Headers" FILES ${PHYSX_EXTENSIONS_HEADERS})
source_group("Sources" FILES ${PHYSX_EXTENSIONS_SOURCES})

# -----------------------------------------------------------------------------------------------------------------------------
# TARGETS
# -----------------------------------------------------------------------------------------------------------------------------

add_library(PhysX3Extensions ${PHYSX_EXTENSIONS_HEADERS} ${PHYSX_EXTENSIONS_SOURCES})
target_link_libraries(PhysX3Extensions PsFastXml)

set_target_properties(PhysX3Extensions PROPERTIES FOLDER "PhysX")

target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/Common/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PxShared/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PxShared/foundation/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PxShared/fastxml/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PxShared/pvd/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/characterkinematic")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/common")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/extensions")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/geometry")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/GeomUtils")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/pvd")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/particles")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/cloth")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/gpu")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Include/vehicle")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/Common/src")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/headers")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/src")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/contact")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/common")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/convex")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/distance")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/sweep")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/gjk")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/mesh")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/hf")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/pcm")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/ccd")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/GeomUtils/intersection")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/LowLevel/API/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/LowLevel/software/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/LowLevel/common/include/pipeline")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/LowLevelAABB/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/LowLevelDynamics/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/LowLevelDynamics/src")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/LowLevelParticles/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysX/src")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysX/buffering")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysX/particles")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysX/cloth")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/SimulationController/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/SimulationController/src")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/SimulationController/particles")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXCooking/src")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXCooking/mesh")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXCooking/convex")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/SceneQuery/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXGpu/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXMetaData/core/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXVehicle/src")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXVehicle/PhysXMetaData/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXMetaData/extensions/include")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXExtensions/serialization/Xml")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXExtensions/serialization/File")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXExtensions/serialization/Binary")
target_include_directories(PhysX3Extensions PUBLIC "${PHYSX_ROOT_DIR}/PhysX_3.4/Source/PhysXExtensions/src")

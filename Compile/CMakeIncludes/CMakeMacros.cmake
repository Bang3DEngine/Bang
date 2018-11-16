
#=================================================================
# Include guard ==================================================
#=================================================================
if (DEFINED BANG_CMAKE_MACROS_BUILT)
    return()
endif()
set(BANG_CMAKE_MACROS_BUILT ON)
#=================================================================
#=================================================================
#=================================================================

# Some global vars
set(LIBRARIES_SUBDIR_NAME "${CMAKE_BUILD_TYPE}")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    if (${USE_SANITIZER})
        set(LIBRARIES_SUBDIR_NAME "DebugWithSanitizer")
    endif()
endif()

#=================================================================
# Macros =========================================================
#=================================================================
macro(list_replace LIST INDEX NEWVALUE)
    list(INSERT ${LIST} ${INDEX} ${NEWVALUE})
    MATH(EXPR __INDEX "${INDEX} + 1")
    list (REMOVE_AT ${LIST} ${__INDEX})
endmacro()

macro(get_soname __SONAME __SOPATH)
    execute_process(
      COMMAND /bin/sh -c "objdump -p ${__SOPATH} | grep SONAME | sed 's/.*SONAME.*\\s//g'"
      OUTPUT_VARIABLE ${__SONAME}
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro()

macro(get_list_sonames __SOPATH)
    set(__IDX 0)
    list(LENGTH ${__LIST} __COUNT)
    while(${__IDX} LESS ${__COUNT})
      list(GET ${__LIST} ${__IDX} __PATH)
      get_soname(${__PATH})
      list_replace(${__LIST} ${__IDX} ${__PATH})
      math(EXPR __IDX "${__IDX} + 1")
    endwhile()
endmacro()

macro(get_realpath __PATH)
    get_filename_component(__PATH ${__PATH} REALPATH)
endmacro()

macro(get_list_realpaths __LIST)
    set(__IDX 0)
    list(LENGTH ${__LIST} __COUNT)
    while(${__IDX} LESS ${__COUNT})
      list(GET ${__LIST} ${__IDX} __PATH)
      get_realpath(${__PATH})
      list_replace(${__LIST} ${__IDX} ${__PATH})
      math(EXPR __IDX "${__IDX} + 1")
    endwhile()
endmacro()

macro(add_misc_files)

endmacro()

macro(remove_from_list_another_list __LIST_TO_REMOVE_FROM __LIST_OF_ITEMS_TO_REMOVE)
    foreach(ITEM_TO_REMOVE ${__LIST_OF_ITEMS_TO_REMOVE})
        list(REMOVE_ITEM ${__LIST_TO_REMOVE_FROM} ${ITEM_TO_REMOVE})
    endforeach()
endmacro()

macro(add_bang_compilation_flags __TARGET)

    target_compile_options(${__TARGET} PUBLIC -std=c++11)
    target_compile_options(${__TARGET} PUBLIC -Wall)
    target_compile_options(${__TARGET} PUBLIC -fPIC)
    target_compile_options(${__TARGET} PUBLIC -Wint-to-pointer-cast)
    target_compile_options(${__TARGET} PUBLIC -Wno-unused-parameter)
    target_compile_options(${__TARGET} PUBLIC -Wno-sign-compare)
    target_compile_options(${__TARGET} PUBLIC -Wswitch)
    target_compile_options(${__TARGET} PUBLIC -Wl,--export-dynamic)
    target_compile_options(${__TARGET} PUBLIC -pthread)
    target_compile_definitions(${__TARGET} PUBLIC -DBANG_BUILD_TYPE="${CMAKE_BUILD_TYPE}")

    if (NOT ${BUILD_SHARED_LIBS})
        target_compile_definitions(${__TARGET} PUBLIC -DBUILD_STATIC)
    endif()

    if (DEFINED FROM_TRAVIS)
        target_compile_definitions(${__TARGET} PUBLIC -DFROM_TRAVIS=${FROM_TRAVIS})
    endif()

    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        if ("${USE_SANITIZER}")
            target_compile_options(${__TARGET} PUBLIC -fsanitize=undefined)
            target_compile_options(${__TARGET} PUBLIC -fno-sanitize-recover)
            target_compile_options(${__TARGET} PUBLIC -fsanitize=address)
            target_compile_options(${__TARGET} PUBLIC -fsanitize=leak)
            target_compile_options(${__TARGET} PUBLIC -fsanitize=float-divide-by-zero)
            target_compile_options(${__TARGET} PUBLIC -fsanitize=integer-divide-by-zero)
            target_compile_options(${__TARGET} PUBLIC -fsanitize=return)
            target_compile_options(${__TARGET} PUBLIC -fsanitize=null)

            # When executing set these environment variables:
            # ASAN_OPTIONS  = "abort_on_error=1"
            # UBSAN_OPTIONS = "print_stacktrace=1:halt_on_error=1"
            # LD_PRELOAD    = /path/to/libasan.x.so
        endif()

        add_definitions(-DDEBUG)
        add_definitions(-D_DEBUG)
        target_compile_options(${__TARGET} PUBLIC -g3)
        target_compile_options(${__TARGET} PUBLIC -O0)
        target_compile_options(${__TARGET} PUBLIC -ggdb)
        target_compile_options(${__TARGET} PUBLIC -Wl,-O0)
        target_compile_options(${__TARGET} PUBLIC -fno-omit-frame-pointer)

    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")

        add_definitions(-DGPROF)
        add_definitions(-DNDEBUG)
        link_libraries("profiler") # gperftools

    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")

        target_compile_options(${__TARGET} PUBLIC -O3)
        target_compile_options(${__TARGET} PUBLIC -Wl,-O3)

    endif()

endmacro()

if (USE_CLANG_TIDY)
    find_program (
      CLANG_TIDY_EXE
      NAMES "clang-tidy"
      DOC "Path to clang-tidy executable"
    )

    if(NOT CLANG_TIDY_EXE)
        message(STATUS "clang-tidy not found.")
    else()
          message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
          set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}"
              "-checks=*,-clang-analyzer-alpha.*")
    endif()
endif()

macro(add_clang_tidy __TARGET)
    if (USE_CLANG_TIDY)
        if(CLANG_TIDY_EXE)
            set_target_properties( ${__TARGET} PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}" )
        endif()
    endif()
endmacro()

#=================================================================
#=================================================================
#=================================================================

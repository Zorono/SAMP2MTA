#==============================================================================#
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0115 NEW)
include(CheckIncludeFiles)
#==============================================================================#
# Settings                                                                     #
#==============================================================================#
set(MODULE_NAME "SAMP2MTA")
set(MODULE_VERSION_MAJOR 1)
set(MODULE_VERSION_MINOR 1)
set(MODULE_VERSION_BUILD 0)

set(MODULE_LINK_DEPENDENCIES "pthread"
                             "stdc++"
                             "lua5.1"
                             "sqlite3"
                             "stdc++fs")
#==============================================================================#
project(${MODULE_NAME}
	LANGUAGES CXX
	VERSION ${MODULE_VERSION_MAJOR}.${MODULE_VERSION_MINOR}.${MODULE_VERSION_BUILD}
)

# Check include files availability
set(REQUIRED_INCLUDE_FILES
	"inttypes.h"
	"stdint.h"
	"alloca.h"
    "stdio.h"
    "string.h"
    "malloc.h"
    "map.h"
)
foreach(INCLUDE_FILE ${REQUIRED_INCLUDE_FILES})
	string(REGEX REPLACE "\\.|/" "_" DEFINITION_NAME "HAVE_${INCLUDE_FILE}")
	string(TOUPPER ${DEFINITION_NAME} DEFINITION_NAME)
	check_include_files("${INCLUDE_FILE}" ${DEFINITION_NAME})
	if(${DEFINITION_NAME})
        add_definitions(-D${DEFINITION_NAME})
	endif()
endforeach()

string(TOLOWER ${MODULE_NAME} MODULE_NAME_LOWERCASE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MODULE_SRC
    "${CMAKE_SOURCE_DIR}/StdInc.cpp"
    "${CMAKE_SOURCE_DIR}/amx/amxargs.c"
    "${CMAKE_SOURCE_DIR}/amx/amxaux.c"
    "${CMAKE_SOURCE_DIR}/amx/amx.c"
    "${CMAKE_SOURCE_DIR}/amx/amxcons.c"
    "${CMAKE_SOURCE_DIR}/amx/amxcore.c"
    "${CMAKE_SOURCE_DIR}/amx/amxdbg.c"
    "${CMAKE_SOURCE_DIR}/amx/amxdgram.c"
    "${CMAKE_SOURCE_DIR}/amx/amx_exec.c"
    "${CMAKE_SOURCE_DIR}/amx/amxexec_gcc.c"
    "${CMAKE_SOURCE_DIR}/amx/amxfile.c"
    "${CMAKE_SOURCE_DIR}/amx/amxfixed.c"
    "${CMAKE_SOURCE_DIR}/amx/amxfloat.c"
    "${CMAKE_SOURCE_DIR}/amx/amxgc.c"
    "${CMAKE_SOURCE_DIR}/amx/amxpool.c"
    "${CMAKE_SOURCE_DIR}/amx/amxprocess.c"
    "${CMAKE_SOURCE_DIR}/amx/amxstring.c"
    "${CMAKE_SOURCE_DIR}/amx/amxtime.c"
    "${CMAKE_SOURCE_DIR}/amx/amx_verifier.c"
    "${CMAKE_SOURCE_DIR}/amx/dllmain.c"
    "${CMAKE_SOURCE_DIR}/amx/fixed.c"
    "${CMAKE_SOURCE_DIR}/amx/float.c"
    "${CMAKE_SOURCE_DIR}/amx/fpattern.c"
    "${CMAKE_SOURCE_DIR}/amx/keeloq.c"
    "${CMAKE_SOURCE_DIR}/amx/minIni.c"
    "${CMAKE_SOURCE_DIR}/amx/pawndbg.c"
    "${CMAKE_SOURCE_DIR}/amx/pawnrun.c"
    "${CMAKE_SOURCE_DIR}/amx/term_ga.c"
    "${CMAKE_SOURCE_DIR}/amx/termwin.c"
    "${CMAKE_SOURCE_DIR}/lua/lapi.c"
    "${CMAKE_SOURCE_DIR}/lua/lauxlib.c"
    "${CMAKE_SOURCE_DIR}/lua/lbaselib.c"
    "${CMAKE_SOURCE_DIR}/lua/lcode.c"
    "${CMAKE_SOURCE_DIR}/lua/ldblib.c"
    "${CMAKE_SOURCE_DIR}/lua/ldebug.c"
    "${CMAKE_SOURCE_DIR}/lua/ldo.c"
    "${CMAKE_SOURCE_DIR}/lua/ldump.c"
    "${CMAKE_SOURCE_DIR}/lua/lfunc.c"
    "${CMAKE_SOURCE_DIR}/lua/lgc.c"
    "${CMAKE_SOURCE_DIR}/lua/linit.c"
    "${CMAKE_SOURCE_DIR}/lua/liolib.c"
    "${CMAKE_SOURCE_DIR}/lua/llex.c"
    "${CMAKE_SOURCE_DIR}/lua/lmathlib.c"
    "${CMAKE_SOURCE_DIR}/lua/lmem.c"
    "${CMAKE_SOURCE_DIR}/lua/loadlib.c"
    "${CMAKE_SOURCE_DIR}/lua/lobject.c"
    "${CMAKE_SOURCE_DIR}/lua/lopcodes.c"
    "${CMAKE_SOURCE_DIR}/lua/loslib.c"
    "${CMAKE_SOURCE_DIR}/lua/lparser.c"
    "${CMAKE_SOURCE_DIR}/lua/lstate.c"
    "${CMAKE_SOURCE_DIR}/lua/lstring.c"
    "${CMAKE_SOURCE_DIR}/lua/lstrlib.c"
    "${CMAKE_SOURCE_DIR}/lua/ltable.c"
    "${CMAKE_SOURCE_DIR}/lua/ltablib.c"
    "${CMAKE_SOURCE_DIR}/lua/ltm.c"
    "${CMAKE_SOURCE_DIR}/lua/lundump.c"
    "${CMAKE_SOURCE_DIR}/lua/lutf8lib.c"
    "${CMAKE_SOURCE_DIR}/lua/lvm.c"
    "${CMAKE_SOURCE_DIR}/lua/lzio.c"
    "${CMAKE_SOURCE_DIR}/lua/print.c"
    "${CMAKE_SOURCE_DIR}/sha256/sha256.cpp"
    "${CMAKE_SOURCE_DIR}/sqlite/sqlite3.c"
    "${CMAKE_SOURCE_DIR}/util.cpp"
	"${CMAKE_SOURCE_DIR}/ml_base.cpp"
	"${CMAKE_SOURCE_DIR}/CFunctions.cpp"
)
if(UNIX)
    set(MODULE_SRC
        ${MODULE_SRC}
        "${CMAKE_SOURCE_DIR}/linux/binreloc.c"
        "${CMAKE_SOURCE_DIR}/linux/getch.c"
    )
    add_definitions(-DLINUX -DENABLE_BINRELOC -g)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/linux)
endif()

# Make sure the target is 32-bit
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -O3")
    endif()
    if(CMAKE_COMPILER_IS_GNUCC
     AND NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.0)
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-attributes")
    elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-ignored-attributes")
    endif()

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
elseif(MSVC)
    target_compile_definitions(${MODULE_NAME_LOWERCASE} PRIVATE _CRT_SECURE_NO_WARNINGS HAVE_STDINT_H)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-noexcept-type -pthread -O3")
  if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
  endif()
  if(CMAKE_COMPILER_IS_GNUCXX
     AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-attributes")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
elseif (CMAKE_CXX_STANDARD LESS 17)
    message(WARNING "CMAKE_CXX_STANDARD has been set to '${CMAKE_CXX_STANDARD}' which is lower than the minimum required standard (c++17).")
endif ()

add_library(${MODULE_NAME_LOWERCASE} SHARED ${MODULE_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/amx ${CMAKE_CURRENT_SOURCE_DIR}/lua ${CMAKE_CURRENT_SOURCE_DIR}/sha256 ${CMAKE_CURRENT_SOURCE_DIR}/sqlite)
foreach(LINK_DEPENDENCY ${MODULE_LINK_DEPENDENCIES})
    link_libraries(${LINK_DEPENDENCY})
endforeach()
set_property(TARGET ${MODULE_NAME_LOWERCASE} PROPERTY PREFIX "")
set_property(TARGET ${MODULE_NAME_LOWERCASE} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${MODULE_NAME_LOWERCASE} PROPERTY POSITION_INDEPENDENT_CODE Off)

if(MINGW)
	set_property(TARGET ${MODULE_NAME_LOWERCASE}
		APPEND_STRING PROPERTY LINK_FLAGS "-Wl,-k,--enable-stdcall-fixup"
	)
endif()

set(CPACK_PACKAGE_NAME "${MODULE_NAME_LOWERCASE}")
set(CPACK_PACKAGE_VERSION_MAJOR ${MODULE_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${MODULE_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${MODULE_VERSION_BUILD})
include(CPack)

# Print CXX compiler information
message (STATUS "CMAKE_CXX_COMPILER '${CMAKE_CXX_COMPILER}' '${CMAKE_CXX_COMPILER_ID}' '${CMAKE_CXX_COMPILER_VERSION}'")

# Print CXX FLAGS
message (STATUS "CMAKE_CXX_FLAGS                '${CMAKE_CXX_FLAGS}'")
if (MSVC)
    message (STATUS "CMAKE_CXX_FLAGS_DEBUG          '${CMAKE_CXX_FLAGS_DEBUG}'")
    message (STATUS "CMAKE_CXX_FLAGS_RELEASE        '${CMAKE_CXX_FLAGS_RELEASE}'")
    message (STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO '${CMAKE_CXX_FLAGS_RELWITHDEBINFO}'")
    message (STATUS "CMAKE_CXX_FLAGS_MINSIZEREL     '${CMAKE_CXX_FLAGS_MINSIZEREL}'")
else (NOT MSVC)
    if     (CMAKE_BUILD_TYPE STREQUAL Debug)
        message (STATUS "CMAKE_CXX_FLAGS_DEBUG          '${CMAKE_CXX_FLAGS_DEBUG}'")
    elseif (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
        message (STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO '${CMAKE_CXX_FLAGS_RELWITHDEBINFO}'")
    elseif (CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
        message (STATUS "CMAKE_CXX_FLAGS_MINSIZEREL     '${CMAKE_CXX_FLAGS_MINSIZEREL}'")
    else ()
        message (STATUS "CMAKE_CXX_FLAGS_RELEASE        '${CMAKE_CXX_FLAGS_RELEASE}'")
    endif ()
endif ()

message (STATUS "Using c++ standard c++${CMAKE_CXX_STANDARD}")
message (STATUS "CMake version: ${CMAKE_VERSION}")
message (STATUS "Module version: ${PROJECT_VERSION}")

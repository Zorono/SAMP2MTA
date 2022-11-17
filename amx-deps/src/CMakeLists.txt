#==============================================================================#
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0115 NEW)
include(CheckIncludeFiles)
#==============================================================================#
# Settings                                                                     #
#==============================================================================#
set(MODULE_NAME "SAMP2MTA")
set(MODULE_VERSION_MAJOR 0)
set(MODULE_VERSION_MINOR 0)
set(MODULE_VERSION_BUILD 1)

set(MODULE_LINK_DEPENDENCIES "")
set(MODULE_COMPILE_DEFINITIONS "")
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
)
foreach(INCLUDE_FILE ${REQUIRED_INCLUDE_FILES})
	string(REGEX REPLACE "\\.|/" "_" DEFINITION_NAME "HAVE_${INCLUDE_FILE}")
	string(TOUPPER ${DEFINITION_NAME} DEFINITION_NAME)
	check_include_files("${INCLUDE_FILE}" ${DEFINITION_NAME})
	if(${DEFINITION_NAME})
		set(MODULE_COMPILE_DEFINITIONS ${MODULE_COMPILE_DEFINITIONS} "${DEFINITION_NAME}=1")
	endif()
endforeach()

string(TOLOWER ${MODULE_NAME} MODULE_NAME_LOWERCASE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MODULE_SRC
	"${CMAKE_SOURCE_DIR}/ml_base.cpp"
	"${CMAKE_SOURCE_DIR}/CFunctions.cpp"
	"${CMAKE_SOURCE_DIR}/util.cpp"
    "${CMAKE_SOURCE_DIR}/sha256/sha256.cpp"
    "${CMAKE_SOURCE_DIR}/sqlite/sqlite3.c"
)
if(UNIX)
	set(MODULE_COMPILE_DEFINITIONS ${MODULE_COMPILE_DEFINITIONS} "LINUX")
endif()
if(MINGW)
	set(MODULE_COMPILE_DEFINITIONS ${MODULE_COMPILE_DEFINITIONS} "HAVE_STDINT_H=1" "__need_size_t")
endif()

# Make sure the target is 32-bit
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
	if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    endif()
    if(CMAKE_COMPILER_IS_GNUCC
     AND NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 4.0)
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-attributes")
    elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-ignored-attributes")
    endif()

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-noexcept-type -pthread -lrt")
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

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
elseif (CMAKE_CXX_STANDARD LESS 14)
    message(WARNING "CMAKE_CXX_STANDARD has been set to '${CMAKE_CXX_STANDARD}' which is lower than the minimum required standard (c++14).")
endif ()

message(STATUS "Using c++ standard c++${CMAKE_CXX_STANDARD}")
message (STATUS "CMake version: ${CMAKE_VERSION}")
message (STATUS "Module version: ${PROJECT_VERSION}")

add_library(${MODULE_NAME_LOWERCASE} SHARED ${MODULE_SRC})
target_include_directories(${MODULE_NAME_LOWERCASE} PRIVATE linux amx lua sha256 sqlite)
target_link_libraries(${MODULE_NAME_LOWERCASE} ${MODULE_LINK_DEPENDENCIES})
set_property(TARGET ${MODULE_NAME_LOWERCASE} APPEND_STRING PROPERTY "COMPILE_DEFINITIONS" ${MODULE_COMPILE_DEFINITIONS})
set_property(TARGET ${MODULE_NAME_LOWERCASE} PROPERTY PREFIX "")
set_property(TARGET ${MODULE_NAME_LOWERCASE} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${MODULE_NAME_LOWERCASE} PROPERTY CXX_STANDARD_REQUIRED ON)

if(MINGW)
	set_property(TARGET ${MODULE_NAME_LOWERCASE}
		APPEND_STRING PROPERTY LINK_FLAGS "-Wl,-k"
	)
endif()

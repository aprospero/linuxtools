cmake_minimum_required(VERSION 3.20)
project(linuxtools C)

# Custom Command: Version Header vor Build generieren
add_custom_target(linuxtools_version ALL
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_git_desc.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating version.h from git description"
)

# Alle .c Dateien unter src/ sammeln (ausser mqtt_subs_test/src/main.c)
file(GLOB_RECURSE LINUXTOOLS_SOURCES 
    "src/*.c"
    "src/**/*.c"
    EXCLUDE "test/main.c"
)

# Alle .h Dateien für PUBLIC Header
file(GLOB_RECURSE LINUXTOOLS_HEADERS 
    "src/*.h"
    "src/**/*.h"
)

add_library(linuxtools SHARED ${LINUXTOOLS_SOURCES})

# Version Header muss vor Compilation erstellt werden
add_dependencies(linuxtools linuxtools_version)

# Public Headers exposen
target_include_directories(linuxtools
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

# Externe Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)
target_link_libraries(linuxtools PUBLIC ${MOSQUITTO_LIBRARIES})
target_include_directories(linuxtools PUBLIC ${MOSQUITTO_INCLUDE_DIRS})

# Compile-Definitionen
target_compile_definitions(linuxtools PRIVATE
    LINUXTOOLS_BUILD=1
)
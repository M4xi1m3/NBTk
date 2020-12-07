cmake_minimum_required(VERSION 3.16)
project(NBTk)

# Grab all sources
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

# Use standard install dirs
include(GNUInstallDirs)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0)

# Setup CMake to use GTKMM, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTKMM_CFLAGS_OTHER})

add_executable(NBTk ${SOURCES})

# Link the target to the GTKMM libraries
target_link_libraries(NBTk ${GTKMM_LIBRARIES})

# Add nbtpp
add_subdirectory(external/nbtpp)
target_link_libraries(NBTk nbtpp)

target_include_directories(NBTk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/nbtpp/include>
    PRIVATE src)

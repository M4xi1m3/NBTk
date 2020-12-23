cmake_minimum_required(VERSION 3.16)
project(NBTk)

# Grab all sources
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

add_executable(NBTk ${SOURCES})

# Use standard install dirs
include(GNUInstallDirs)

# Add gtkmm
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0)
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
add_definitions(${GTKMM_CFLAGS_OTHER})
target_link_libraries(NBTk ${GTKMM_LIBRARIES})

# Add stde
message(WARNING "Downloading stde...")
configure_file(CMakeLists.txt.in stde-download/CMakeLists.txt)

execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/stde-download"
)
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/stde-download"
)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/stde)
set(STDE_HEADERS ${CMAKE_SOURCE_DIR}/external/stde/include)

# Add nbtpp
set(HAS_STDE true)
message(WARNING "Downloading nbtpp...")

execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/nbtpp-download"
)
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/nbtpp-download"
)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/nbtpp)
set(NBTPP_HEADERS ${CMAKE_SOURCE_DIR}/external/nbtpp/include)

# Copy ui and imgs
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ui DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgs DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(NBTk nbtpp)

target_include_directories(NBTk PUBLIC
    ${NBTPP_HEADERS}
    PRIVATE src)

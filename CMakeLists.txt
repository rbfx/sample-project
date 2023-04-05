cmake_minimum_required(VERSION 3.14)
project (SampleProject)

set (PROJECT_UWP_GUID "D84454DA-854B-3661-9EDA-0B587569493A")

# C++17 is required for this project.
set (CMAKE_CXX_STANDARD 17)

# Setup output direcotries.
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Generate Urho3DAll.h with all engine includes.
# It is not required, but it may be useful.
set (URHO3D_MONOLITHIC_HEADER ON CACHE BOOL "")

# Disable unused features to speed up build
set (URHO3D_CSHARP OFF CACHE BOOL "")
set (URHO3D_GLOW OFF CACHE BOOL "")
set (URHO3D_IK OFF CACHE BOOL "")
set (URHO3D_NETWORK OFF CACHE BOOL "")
set (URHO3D_URHO2D OFF CACHE BOOL "")
set (URHO3D_PHYSICS2D OFF CACHE BOOL "")
set (URHO3D_SAMPLES OFF CACHE BOOL "")
set (URHO3D_PROFILING OFF CACHE BOOL "")
set (URHO3D_TESTING OFF CACHE BOOL "")

# Disable default Player and Editor to inject static plugins later.
# Dynamically linked builds don't need it since default Player and Editor can load *.dll/*.so just fine.
# But static builds require this to statically inject user code into Player and Editor.
set (URHO3D_PLAYER OFF CACHE BOOL "")
set (URHO3D_EDITOR OFF CACHE BOOL "")

# Link project plugin to Player and Editor.
set (URHO3D_PLUGIN_LIST ${CMAKE_PROJECT_NAME} CACHE BOOL "")

# Include common build scripts.
include (${CMAKE_SOURCE_DIR}/3rdParty/rbfx/CMake/Modules/UrhoCommon.cmake)

# Include the Framework.
add_subdirectory (${CMAKE_SOURCE_DIR}/3rdParty/rbfx)

# Add project source files.
add_subdirectory (${CMAKE_SOURCE_DIR}/Source)

# Add launcher source files.
add_subdirectory (${CMAKE_SOURCE_DIR}/Launcher)

# Add custom player and editor with injected plugins.
if (NOT ANDROID)
    add_subdirectory (${CMAKE_SOURCE_DIR}/3rdParty/rbfx/Source/Player)
    add_subdirectory (${CMAKE_SOURCE_DIR}/3rdParty/rbfx/Source/Editor)
endif()

# Package resources from Project directory.
if (WEB OR MOBILE)
    create_pak("${CMAKE_SOURCE_DIR}/Project/Data"     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Data.pak")
    create_pak("${CMAKE_SOURCE_DIR}/Project/CoreData" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CoreData.pak")
endif ()

# Link resources to Player for Web.
if (WEB)
    web_executable (Player)
    package_resources_web(
        FILES        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Data.pak"
                     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CoreData.pak"
        RELATIVE_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        OUTPUT       "Resources.js"
        INSTALL_TO   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )
    web_link_resources(Player Resources.js)
    target_link_libraries(Player PRIVATE "--shell-file ${CMAKE_SOURCE_DIR}/3rdParty/rbfx/bin/shell.html")
endif ()

cmake_minimum_required(VERSION 3.14)
project (SampleProject)

set (PROJECT_UWP_GUID "D84454DA-854B-3661-9EDA-0B587569493A")

# C++17 is required for this project.
set (CMAKE_CXX_STANDARD 17)

# Setup output directories.
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# It is possible to use pre-built SDK or build the Framework from source.
# In your project, you can choose either option or keep both.
set (REBELFORK_SDK "" CACHE STRING "Path to pre-built SDK")
if ("${REBELFORK_SDK}" STREQUAL "")
    # Include the Framework to build.
    add_subdirectory (../rbfx ${CMAKE_BINARY_DIR}/3rdParty/rbfx)

    # Set path to engine resources.
    set (REBELFORK_BIN_DIR "${CMAKE_SOURCE_DIR}/../rbfx/bin")
    set (REBELFORK_CMAKE_DIR "${CMAKE_SOURCE_DIR}/../rbfx/CMake")
    set (REBELFORK_SHELL_DIR "${REBELFORK_BIN_DIR}")
else ()
    # Include pre-built SDK.
    # CMAKE_FIND_ROOT_PATH is required for Web builds because EMSDK ignores CMAKE_PREFIX_PATH by default.
    set (CMAKE_PREFIX_PATH ${REBELFORK_SDK}/share)
    list (APPEND CMAKE_FIND_ROOT_PATH ${REBELFORK_SDK}/share)
    find_package (Urho3D REQUIRED)

    # Set path to engine resources.
    set (REBELFORK_BIN_DIR "${REBELFORK_SDK}/bin")
    set (REBELFORK_CMAKE_DIR "${REBELFORK_SDK}/share/CMake")
    set (REBELFORK_SHELL_DIR "${REBELFORK_BIN_DIR}/Debug")
endif ()

# Include common build scripts.
include (${REBELFORK_CMAKE_DIR}/Modules/UrhoCommon.cmake)

# Add plugins (optional).
include_directories (${CMAKE_SOURCE_DIR}/Plugins)
add_subdirectory (${CMAKE_SOURCE_DIR}/Plugins/Core.SamplePlugin)

# List all plugins used by the application.
set (APP_PLUGIN_NAME "App.Main")
set (APP_PLUGIN_LIST "Plugin.Core.SamplePlugin;${APP_PLUGIN_NAME}")

# Add main application library with all user code.
add_subdirectory (${CMAKE_SOURCE_DIR}/Source/Application)

# Add custom editor linked with user code.
# With dynamic linking, pre-built editor can be used.
add_subdirectory (${CMAKE_SOURCE_DIR}/Source/Editor)

# Add custom player.
add_subdirectory (${CMAKE_SOURCE_DIR}/Source/Launcher)

# Package resources from Project directory.
if (WEB OR MOBILE)
    create_pak("${CMAKE_SOURCE_DIR}/Project/Data"   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Data.pak")
    create_pak("${REBELFORK_BIN_DIR}/CoreData"      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CoreData.pak")
endif ()

# Link resources to Player for Web.
if (WEB)
    web_executable (${CMAKE_PROJECT_NAME})
    package_resources_web(
        FILES        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Data.pak"
                     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CoreData.pak"
        RELATIVE_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        OUTPUT       "Resources.js"
        INSTALL_TO   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )
    web_link_resources(${CMAKE_PROJECT_NAME} Resources.js)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "--shell-file ${REBELFORK_SHELL_DIR}/shell.html")
endif ()

# Generate Visual Studio props file for csproj.
rbfx_configure_cmake_props()

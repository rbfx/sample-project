cmake_minimum_required(VERSION 3.14)
project (SampleProject)

set (PROJECT_UWP_GUID "D84454DA-854B-3661-9EDA-0B587569493A")

# C++17 is required for this project.
set (CMAKE_CXX_STANDARD 17)

# Setup output directories.
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Configure engine options. This is required in-source engine builds.
set(URHO3D_SAMPLES OFF)

# Engine must be available on CMAKE_PREFIX_PATH:
# Option 1 - prebuilt SDK:
#   - Download a prebuilt SDK and extract it somewhere
#   - Configure with `-DCMAKE_PREFIX_PATH=/path/to/your/sdk -DCMAKE_CONFIGURATION_TYPES="Debug;RelWithDebInfo"`
#     - CMAKE_CONFIGURATION_TYPES may or may not be necessary, depending on your SDK configuration
# Option 2 - in-source build:
#   - Clone https://github.com/rbfx/rbfx somewhere
#   - Configure with `-DCMAKE_PREFIX_PATH=/path/to/rbfx/CMake`
find_package(Urho3D REQUIRED)

# Check if Core.SamplePlugin exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Plugins/Core.SamplePlugin/CMakeLists.txt")
    message(FATAL_ERROR
        "Missing required file: ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/Core.SamplePlugin/CMakeLists.txt\n"
        "It looks like git submodules are not initialized.\n"
        "Please run the following command to fetch them:\n"
        "  git submodule update --init --recursive\n"
    )
endif()

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
    create_pak("${CMAKE_SOURCE_DIR}/Project/Data"    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Data.pak")
    create_pak("${Urho3D_PACKAGE_ROOT}/bin/CoreData" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CoreData.pak")
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
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "--shell-file ${Urho3D_PACKAGE_ROOT}/bin/shell.html")
endif ()

# Generate Visual Studio props file for csproj.
rbfx_configure_cmake_props()

# Include source files.
file (GLOB_RECURSE SOURCE_FILES *.h *.cpp)

# Add dynamic or static library.
set (TARGET_NAME ${CMAKE_PROJECT_NAME})
add_library(${TARGET_NAME} ${SOURCE_FILES})

# Link the engine and dependencies.
target_link_libraries (${TARGET_NAME} PRIVATE
    Urho3D
    Plugin.Core.SamplePlugin
)

# Declare that this library could be a static plugin.
define_static_plugin (${TARGET_NAME} ${CMAKE_PROJECT_NAME})

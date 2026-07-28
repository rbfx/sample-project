# Include source files.
file (GLOB_RECURSE SOURCE_FILES *.h *.cpp)

if (ANDROID)
    add_library(${APP_PLUGIN_NAME} STATIC ${SOURCE_FILES})
    target_compile_definitions(${APP_PLUGIN_NAME} PRIVATE URHO3D_STATIC=1)
else()
    add_library(${APP_PLUGIN_NAME} ${SOURCE_FILES})
endif ()

# Link the engine and dependencies.
target_link_libraries (${APP_PLUGIN_NAME} PRIVATE
    Urho3D
    Plugin.Core.SamplePlugin
)

# Declare that this library could be a static plugin.
setup_plugin_target (${APP_PLUGIN_NAME} ${APP_PLUGIN_NAME})

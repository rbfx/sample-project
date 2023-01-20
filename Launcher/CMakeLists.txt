# Include source files.
file (GLOB_RECURSE SOURCE_FILES *.h *.cpp)

if(MSVC)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_SOURCE_DIR}/StoreArt/Windows/appicon.rc")
    list(APPEND SOURCE_FILES ${APP_ICON_RESOURCE_WINDOWS})
endif()

# Add launcher.
set (TARGET_NAME "${CMAKE_PROJECT_NAME}Launcher")
add_executable(${TARGET_NAME} ${SOURCE_FILES})
target_link_libraries (${TARGET_NAME} PRIVATE Urho3D "${CMAKE_PROJECT_NAME}")
link_static_plugins (${TARGET_NAME} "${URHO3D_PLUGIN_LIST}")

if(MSVC)
    set_target_properties(${TARGET_NAME}
        PROPERTIES
            VS_DEBUGGER_COMMAND_ARGUMENTS "--pp ${CMAKE_SOURCE_DIR}/Project"
    )
endif()

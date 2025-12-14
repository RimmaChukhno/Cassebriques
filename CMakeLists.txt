cmake_minimum_required(VERSION 3.16)
project(CasseBriques)

# Configuration C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put built binaries into a predictable folder (helps running the correct exe)
# Multi-config generators (Visual Studio): bin/<Config>/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
foreach(OUTPUTCONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG})
endforeach()

# Build type (only meaningful for single-config generators like Ninja/Makefiles).
# Visual Studio is multi-config and ignores CMAKE_BUILD_TYPE.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# Chemins SFML
# Option 1 : Si SFML est installé dans un dossier spécifique
# set(SFML_DIR "C:/SFML/lib/cmake/SFML")
# Option 2 : Si SFML est installé via vcpkg
# set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Recherche de SFML (Config package or Find module, depending on install)
find_package(SFML 2.6 COMPONENTS system window graphics REQUIRED)

# --- Guard: MSVC can't link MinGW (.dll.a) import libraries ---
option(CASSEBRIQUES_ERROR_ON_SFML_MISMATCH "Stop CMake configuration if MSVC is paired with a MinGW SFML install" OFF)
if(MSVC)
    # Common with MSYS2: SFML_DIR points into .../mingw64/... and libs end with .dll.a
    set(_sfml_msvc_mismatch FALSE)
    if(DEFINED SFML_DIR AND SFML_DIR MATCHES "/mingw64/|\\\\mingw64\\\\")
        set(_sfml_msvc_mismatch TRUE)
    endif()
    if(DEFINED SFML_LIBRARIES AND SFML_LIBRARIES MATCHES "\\.dll\\.a(;|$)")
        set(_sfml_msvc_mismatch TRUE)
    endif()
    if(_sfml_msvc_mismatch)
        set(_sfml_mismatch_msg
            "SFML detected from MinGW/MSYS2 (e.g. '.../mingw64/...', '.dll.a' import libs), but you are building with MSVC.\n"
            "MSVC cannot link MinGW import libraries.\n\n"
            "Fix options:\n"
            "  A) Install SFML built for MSVC (or via vcpkg x64-windows) and reconfigure, e.g. set SFML_DIR accordingly.\n"
            "  B) Build this project with MinGW instead (recommended if you already have MSYS2).\n"
            "     Use scripts/build_mingw.ps1 or scripts/build_mingw.bat.\n"
        )
        if(CASSEBRIQUES_ERROR_ON_SFML_MISMATCH)
            message(FATAL_ERROR "${_sfml_mismatch_msg}")
        else()
            message(WARNING "${_sfml_mismatch_msg}")
        endif()
    endif()
endif()

# --- Sources ---
# IMPORTANT: we list sources explicitly to avoid CMake glob not picking up new files
# unless you re-run configuration.
set(SOURCES
    main.cpp

    # app
    src/app/App.cpp
    src/app/Assets.cpp
    src/app/Settings.cpp

    # ui
    src/ui/Button.cpp

    # scenes
    src/scenes/MainMenuScene.cpp
    src/scenes/SettingsScene.cpp
    src/scenes/ClassicGameScene.cpp
    src/scenes/RebornGameScene.cpp

    # core
    src/core/AABB.cpp
    src/core/GameObject.cpp
    src/core/InputManager.cpp

    # classic gameplay objects
    src/game/Ball.cpp
    src/game/Brick.cpp
    src/game/Paddle.cpp

    # reborn gameplay objects
    src/game_reborn/Brick.cpp
    src/game_reborn/Cannon.cpp
    src/game_reborn/Projectile.cpp
)

set(HEADERS
    # app
    src/app/App.hpp
    src/app/Assets.hpp
    src/app/Scene.hpp
    src/app/Settings.hpp

    # ui
    src/ui/Button.hpp

    # core
    src/core/AABB.hpp
    src/core/GameObject.hpp
    src/core/InputManager.hpp

    # classic
    src/game/Ball.hpp
    src/game/Brick.hpp
    src/game/Paddle.hpp

    # reborn
    src/game_reborn/Brick.hpp
    src/game_reborn/Cannon.hpp
    src/game_reborn/Projectile.hpp
)

# Exécutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    src/
)

# Lier SFML (supports both config targets and legacy module variables)
if(TARGET SFML::Graphics)
    target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)
elseif(TARGET sfml-graphics)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
else()
    # Fallback for FindSFML.cmake style
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SFML_LIBRARIES})
endif()

# Optional: SFML main helper (usually only needed for WIN32 subsystem apps)
if(TARGET SFML::Main)
    target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Main)
elseif(TARGET sfml-main)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-main)
endif()

# Copier les DLL SFML (Windows) when SFML provides imported targets we can query.
if(WIN32)
    if(TARGET sfml-graphics AND TARGET sfml-window AND TARGET sfml-system)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    elseif(TARGET SFML::Graphics AND TARGET SFML::Window AND TARGET SFML::System)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SFML::Graphics>
            $<TARGET_FILE:SFML::Window>
            $<TARGET_FILE:SFML::System>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    else()
        message(STATUS "SFML DLL copy step skipped (no known SFML imported targets found).")
    endif()
endif()

# Copy runtime assets next to the executable so relative paths always work
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/levels
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/levels
)

# Messages informatifs
message(STATUS "SFML found: ${SFML_FOUND}")
message(STATUS "SFML version: ${SFML_VERSION}")
message(STATUS "SFML include dir: ${SFML_INCLUDE_DIR}")


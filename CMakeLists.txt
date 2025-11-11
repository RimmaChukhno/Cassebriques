cmake_minimum_required(VERSION 3.10)
project(CasseBriques)

# Configuration C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options de build
set(CMAKE_BUILD_TYPE Release)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# Chemins SFML
# Option 1 : Si SFML est installé dans un dossier spécifique
# set(SFML_DIR "C:/SFML/lib/cmake/SFML")
# Option 2 : Si SFML est installé via vcpkg
# set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Recherche de SFML
find_package(SFML 2.6 COMPONENTS system window graphics REQUIRED)

# Fichiers sources
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*/*.cpp"
    "main.cpp"
)

# Fichiers headers (optionnel, pour l'IDE)
file(GLOB_RECURSE HEADERS 
    "src/*.h"
    "src/*/*.h"
    "src/*.hpp"
    "src/*/*.hpp"
)

# Vérifier que des sources ont été trouvées
if(NOT SOURCES)
    message(FATAL_ERROR "Aucun fichier source trouvé ! Vérifiez que les fichiers .cpp existent dans src/")
endif()

message(STATUS "Sources trouvées: ${SOURCES}")

# Exécutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    src/
)

# Lier SFML (méthode moderne avec targets)
target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
)

# Copier les DLL SFML (Windows)
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

# Messages informatifs
message(STATUS "SFML found: ${SFML_FOUND}")
message(STATUS "SFML version: ${SFML_VERSION}")
message(STATUS "SFML include dir: ${SFML_INCLUDE_DIR}")


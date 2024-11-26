cmake_minimum_required(VERSION 3.15)
project(MainProject)

# Utiliser le compilateur Clang++
set(CMAKE_CXX_COMPILER clang++)

# Définir le standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Définir les options de compilation
set(CMAKE_CXX_FLAGS "-g -Wno-everything")

# Collecter automatiquement tous les fichiers source et d'en-tête
file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "*.h")

# Exclure les fichiers dans .ccls-cache
list(FILTER SRCS EXCLUDE REGEX ".*/\\.ccls-cache/.*")
list(FILTER HEADERS EXCLUDE REGEX ".*/\\.ccls-cache/.*")

# Ajouter l'exécutable principal
add_executable(main ${SRCS} ${HEADERS})

# Configuration pour un mode de débogage
add_executable(main-debug ${SRCS} ${HEADERS})
target_compile_options(main-debug PRIVATE -O0)

# Ajouter une cible pour nettoyer les fichiers générés
add_custom_target(clean
        COMMAND ${CMAKE_COMMAND} -E remove main main-debug
        COMMENT "Nettoyage des fichiers générés"
)



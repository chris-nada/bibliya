cmake_minimum_required(VERSION 3.9)
project(bibliya)

set(CMAKE_CXX_STANDARD 17)

# Global
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=core2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld")

# Integrierte Bibliotheken (im Ordner /libs/)   # Version   | Datum als aktuell | Beschreibung
include_directories(SYSTEM libs/imgui)          # 1.78          29 09 2020          GUI
include_directories(SYSTEM libs/imgui-sfml)     # 5ab660a       29 09 2020          SFML Schnittstelle für ImGui
include_directories(SYSTEM libs/implot)         # da5b4ab       29 09 2020          Plot-Addons für ImGui

# Build Config
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -m64 -mtune=generic") # Kompileroptimierungen
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
    endif()
    if (UNIX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
    endif ()
    add_subdirectory(libs)
else () # Debug build
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    add_subdirectory(libs)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -coverage")
endif ()

# Gemeinsame Bibliotheken
find_package(SFML 2 REQUIRED graphics window system) # audio

#find_package(Boost REQUIRED)
#include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# Plattformabhängige Bibliotheken
set(LIBS bibliotheken sfml-system sfml-window sfml-graphics)

### Debian 10
if (UNIX)
    set(LIBS ${LIBS} OpenGL GL pthread stdc++fs)
### Windows mit MSYS2
elseif (WIN32)
    find_package(CEREAL REQUIRED)
    find_package(OpenGL REQUIRED)
    set(LIBS ${LIBS} ${OPENGL_LIBRARIES} stdc++fs)
    include_directories(SYSTEM ${CEREAL_INCLUDE_DIRS})
endif ()

# Build
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/)
file(GLOB_RECURSE QUELLTEXT src/*.hpp src/*cpp)
add_executable(bibliya ${QUELLTEXT})
target_link_libraries (bibliya ${LIBS})

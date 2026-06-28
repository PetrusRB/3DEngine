cmake_minimum_required(VERSION 3.10)

project(3DEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/arena)
include_directories(${CMAKE_SOURCE_DIR}/include/arena/internal)
include_directories(${CMAKE_SOURCE_DIR}/include/arena/core)
include_directories(${CMAKE_SOURCE_DIR}/include/arena/core/allocation)
include_directories(C:/msys64/mingw64/include)

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.c)
list(FILTER SRC_FILES EXCLUDE REGEX "tabs\\.cpp$")
list(FILTER SRC_FILES EXCLUDE REGEX "obj\\.c$")
list(FILTER SRC_FILES EXCLUDE REGEX "arena_visualizer\\.c$")
file(GLOB IMGUI_SOURCES
    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui_impl_glfw.cpp
    include/imgui/imgui_impl_opengl3.cpp
)
add_executable(3DEngine ${SRC_FILES} ${IMGUI_SOURCES})

target_link_directories(3DEngine PRIVATE C:/msys64/mingw64/lib fmt::fmt)

target_link_libraries(3DEngine
    "C:/GLFW_mingw64/lib-mingw-w64/libglfw3.a"
    opengl32
    gdi32
    fmt
    user32
    shell32
    assimp
    openal
)

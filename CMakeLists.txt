cmake_minimum_required(VERSION 3.16)
project (daemon C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
include_directories(
  ${SDL2_INCLUDE_DIRS}
  external/glad/include 
)

add_executable(daemon
  src/main.c
  src/renderer.c 
  src/time.c 
  src/input.c
  src/camera.c
  src/gfx/shader.c
  src/map/map.c
  src/geom/sector_mesh.c
  src/geom/wall_mesh.c
  external/glad/src/glad.c
)

if (WIN32)
  target_link_libraries(daemon PRIVATE ${SDL2_LIBRARIES} opengl32)
elseif(APPLE)
  find_library(OpenGL_FRAMEWORK OpenGL REQUIRED)
  target_link_libraries(daemon PRIVATE ${SDL2_LIBRARIES} ${OpenGL_FRAMEWORK})
else()
  find_package(OpenGL REQUIRED)
  target_link_libraries(daemon PRIVATE ${SDL2_LIBRARIES} OpenGL::GL m)
endif()

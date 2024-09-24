cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if(NOT CMAKE_BUILD_TYPE)
  message(WARNING "Default build type set to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

project(placeholder)

find_package(SDL2 REQUIRED PATHS external/SDL2-2.30.7)

add_executable(
  placeholder
  src/main.cpp
  external/imgui-1.91.2/imgui.cpp
  external/imgui-1.91.2/imgui_demo.cpp
  external/imgui-1.91.2/imgui_draw.cpp
  external/imgui-1.91.2/imgui_tables.cpp
  external/imgui-1.91.2/imgui_widgets.cpp
  external/imgui-1.91.2/backends/imgui_impl_sdl2.cpp
  external/imgui-1.91.2/backends/imgui_impl_sdlrenderer2.cpp
)

target_compile_options(placeholder PRIVATE /W4)

target_include_directories(
  placeholder PRIVATE include
   external/imgui-1.91.2 external/imgui-1.91.2/backends
                 ${SDL2_INCLUDE_DIRS})

target_link_libraries(placeholder PRIVATE ${SDL2_LIBRARIES})

if(MSVC)
  add_custom_command(
    TARGET placeholder
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${SDL2_BINDIR}/SDL2.dll"
            $<TARGET_FILE_DIR:placeholder>)

#   add_custom_command(
#     TARGET placeholder
#     POST_BUILD
#     COMMAND
#       "${CMAKE_COMMAND}" -E copy_if_different
#       "${CMAKE_SOURCE_DIR}/JetBrainsMonoNL-Regular.ttf"
#       $<TARGET_FILE_DIR:placeholder>)
endif()
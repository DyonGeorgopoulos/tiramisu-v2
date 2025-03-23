cmake_minimum_required(VERSION 3.30)
cmake_policy(SET CMP0135 NEW)
project(tiramisu LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)

# Include the command that downloads libraries
include(FetchContent)

# define a function for adding git dependencies
function(include_dependency libName gitURL gitTag)
    # setup the declare
    FetchContent_Declare(${libName}
            GIT_REPOSITORY ${gitURL}
            GIT_TAG        ${gitTag}
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
    )

    FetchContent_MakeAvailable(${libName})
endfunction()

# add freetype support
# find_package(Freetype QUIET)
# if (NOT Freetype_FOUND)
#     message(STATUS "Getting freetype2 from Github")
#     include_dependency(Freetype https://github.com/freetype/freetype 5d4e649f740c675426fbe4cdaffc53ee2a4cb954)
# else()
#     message(STATUS "using local Freetype")
# endif()


# add cglm support
message(STATUS "Getting cglm from Github")
include_dependency(cglm https://github.com/recp/cglm v0.9.6)

# add SDL3 support
find_package(SDL3 QUIET)
if (NOT SDL3_FOUND)
    message(STATUS "Getting SDL3 from Github")
    include_dependency(SDL3 https://github.com/libsdl-org/SDL.git release-3.2.0)
else()
    message(STATUS "Using local SDL3")
endif()


# === LIBRARY: sokol ===
# add headers to the the file list because they are useful to have in IDEs
set(SOKOL_HEADERS
    sokol/sokol_gfx.h
    sokol/sokol_imgui.h
    sokol/sokol_glue.h
    sokol/sokol_gp.h)
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    add_library(sokol STATIC sokol/sokol.c ${SOKOL_HEADERS})
    target_compile_options(sokol PRIVATE -x objective-c)
    target_link_libraries(sokol PUBLIC
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework MetalKit"
        "-framework Metal")
else()
    add_library(sokol STATIC sokol/sokol.c ${SOKOL_HEADERS})
    if (CMAKE_SYSTEM_NAME STREQUAL Linux)
        target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL dl m)
        target_link_libraries(sokol PUBLIC Threads::Threads)
    endif()
endif()
# target_link_libraries(sokol PUBLIC cimgui)
target_include_directories(sokol INTERFACE sokol)

# === LIBRARY: cimgui ===
# cimgui -- this needs work as sdl3 bindings are (shrug) atm.
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/backends)
  set(BACKENDS_FOLDER "cimgui/imgui/backends/")
else()
  set(BACKENDS_FOLDER "cimgui/imgui/examples/")
endif()

if (EXISTS  ${CMAKE_CURRENT_SOURCE_DIR}/cimgui/imgui/imgui_tables.cpp)
  set(TABLES_SOURCE "cimgui/imgui/imgui_tables.cpp")
else()
  set(TABLES_SOURCE "")
endif()

include_directories(cimgui/imgui)
include_directories(cimgui)
add_definitions("-DIMGUI_USER_CONFIG=\"../cimconfig.h\"")
add_definitions("-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1")

set(IMGUI_SOURCES
  cimgui/cimgui.cpp 
  cimgui/imgui/imgui.cpp 
  cimgui/imgui/imgui_draw.cpp 
  cimgui/imgui/imgui_demo.cpp 
  cimgui/imgui/imgui_widgets.cpp
  ${TABLES_SOURCE})

set(IMGUI_SOURCES_sdl)
set(IMGUI_LIBRARIES)

if (WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)

add_compile_definitions("IMGUI_IMPL_OPENGL_LOADER_GL3W")

#opengl3
list(APPEND IMGUI_SOURCES ${BACKENDS_FOLDER}imgui_impl_opengl3.cpp)
include_directories(cimgui/imgui/examples/libs/gl3w)
if(WIN32)
       list(APPEND IMGUI_LIBRARIES opengl32)
else(WIN32)#Unix
       list(APPEND IMGUI_LIBRARIES GL)
endif(WIN32)

list(APPEND IMGUI_SOURCES ${BACKENDS_FOLDER}imgui_impl_sdl3.cpp)
list(APPEND IMGUI_SOURCES ${BACKENDS_FOLDER}imgui_impl_dx11.cpp)

#as c math
message(STATUS "Getting as-c-math from Github")
include_dependency(as-c-math https://github.com/pr0g/as-c-math.git 024955c7538d56272c696c4b2d0cdd681b9d0d50)

option(IMGUI_STATIC "" ON)

add_executable(${PROJECT_NAME})
target_sources(
  ${PROJECT_NAME}
  PRIVATE src/main.c
          src/map.c
          src/global.c
          src/entity.c
          src/update.c
          src/render.c
          src/iterate.c
          src/player.c
          src/camera.c
          src/common.c
          sokol/sokol.c)

# set the include directory
target_include_directories(tiramisu PUBLIC ${SDL3_INCLUDE_DIRS} include)


target_compile_definitions(
  ${PROJECT_NAME}
  PRIVATE $<$<BOOL:${SOKOL_EXPERIMENT_GL}>:SOKOL_EXPERIMENT_GL>
          $<$<BOOL:${SOKOL_EXPERIMENT_D3D}>:SOKOL_EXPERIMENT_D3D>
          SDL_MAIN_USE_CALLBACKS)

if (NOT STATIC_BUILD)
  add_library(cimgui SHARED ${IMGUI_SOURCES})
  target_include_directories(cimgui PUBLIC ${SDL3_INCLUDE_DIRS})
  target_link_libraries(cimgui PRIVATE SDL3::SDL3)
else()
  add_library(cimgui STATIC ${IMGUI_SOURCES})
endif()

# link all libraries to the project
target_link_libraries(tiramisu PRIVATE SDL3::SDL3 sokol as-c-math cglm cimgui ${IMGUI_LIBRARIES})

# set compile_commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # does not produce the json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works

if(SOKOL_EXPERIMENT_GL)
  FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG 7acc116b06f40508b82fd90fc58d181be7e65452
    SOURCE_SUBDIR cmake)
  FetchContent_MakeAvailable(glad)
  list(APPEND CMAKE_MODULE_PATH ${glad_SOURCE_DIR})
  glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)
  target_link_libraries(${PROJECT_NAME} PRIVATE glad_gl_core_33)
  target_sources(${PROJECT_NAME} PRIVATE src/sokol-sdl-graphics-backend-gl.c)
  target_compile_definitions(tiramisu PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_SDL3)
elseif(SOKOL_EXPERIMENT_D3D)
  target_link_libraries(${PROJECT_NAME} PRIVATE dxguid.lib)
  target_sources(${PROJECT_NAME} PRIVATE src/sokol-sdl-graphics-backend-d3d.c)
  target_compile_definitions(tiramisu PUBLIC -DCIMGUI_USE_DX11 -DCIMGUI_USE_SDL3)
endif()

if (WIN32)
    add_custom_command(
            TARGET tiramisu POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:tiramisu>"
            VERBATIM
    )
endif()
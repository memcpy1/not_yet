cmake_minimum_required(VERSION 3.5)
project(gameEngine2D VERSION 0.0.1 LANGUAGES CXX)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED true)

#Include External Libraries
add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)
add_subdirectory(external/SDL_mixer)
add_subdirectory(external/SDL_ttf)
add_subdirectory(external/box2D)

add_library(${PROJECT_NAME} STATIC
        source/engine.cpp       header/Engine.h
        source/text.cpp         header/Text.h
        source/timer.cpp        header/Timer.h
        source/ecs.cpp          header/ECS.h
        source/debug.cpp        header/Debug.h
        source/event.cpp        header/Event.h
        source/collision.cpp    header/Collision.h

        source/vendor/SDL_Primitives/SDL_prims.cpp  header/vendor/SDL_Primitives/SDL_prims.h
)
target_include_directories( ${PROJECT_NAME} 
    PUBLIC
        $<INSTALL_INTERFACE:header>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/header>
    PUBLIC
        external/SDL/include
        external/SDL_image/include
        external/SDL_mixer/include
        external/SDL_ttf
        external/Box2D/include
)

target_link_libraries( ${PROJECT_NAME}
    PUBLIC
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
        box2d
)
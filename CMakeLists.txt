cmake_minimum_required(VERSION 3.5)
project(not_yet VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED true)

add_subdirectory(external/engine)

add_executable(${PROJECT_NAME} source/game.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE gameEngine2D)
target_link_directories(${PROJECT_NAME} PRIVATE build/external/engine/Debug)
target_link_libraries(${PROJECT_NAME} gameEngine2D)


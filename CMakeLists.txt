cmake_minimum_required(VERSION 3.0.0)
project(TicTacToe VERSION 0.1.0 LANGUAGES C CXX)

include_directories(include)

add_executable(TicTacToe src/main.cpp)

target_link_libraries(TicTacToe SDL2main SDL2 SDL2_ttf)

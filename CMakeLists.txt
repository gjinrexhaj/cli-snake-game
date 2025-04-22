# set project attributes
cmake_minimum_required(VERSION 3.30)
project(cli-snake-game C)
set(CMAKE_C_STANDARD 11)

# add Curses as a dependency and compile single source file
find_package(Curses REQUIRED)
add_executable(cli-snake-game game.c)
target_link_libraries(cli-snake-game ${CURSES_LIBRARIES})

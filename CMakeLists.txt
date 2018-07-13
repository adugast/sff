#########
# CMAKE #
#########

cmake_minimum_required(VERSION 2.6.2)

project(sff)

## set compilation flags
set(CMAKE_C_FLAGS "-W -Wall -pedantic -std=c99 -std=gnu99 -g")

## set the target name and source
add_executable(
    sff
    src/main.c
    src/argument_manager.c
    src/process_manager.c
    src/server_manager.c
    src/daemon_manager.c
)


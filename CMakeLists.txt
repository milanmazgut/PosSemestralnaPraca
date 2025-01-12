cmake_minimum_required(VERSION 3.10)

project(SemPos C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(program
    main.c
    server.c
    client.c
    pipe.c
    game.c
    player.c
    dice.c
    shop.c
    enums.c
    syn_game.c
    shm.c
)

target_link_libraries(program pthread)

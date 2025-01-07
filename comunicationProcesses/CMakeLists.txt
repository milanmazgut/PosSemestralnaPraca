cmake_minimum_required(VERSION 3.10)

project(SemPos C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(program
    main.c
    server.c
    client.c
    pipe.c
)

target_link_libraries(program pthread)

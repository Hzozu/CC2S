cmake_minimum_required(VERSION 3.31)
project(CC2S)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(CLionProjects
        main.cpp
        config.cpp
)

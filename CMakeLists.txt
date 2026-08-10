cmake_minimum_required(VERSION 4.2)
project(CButter C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(CButter
        consts.h
        functions.h
        main.c
        consts.h
        compile.h
        functions.c
        consts.c
        hashmap.h
        hashmap.c
        compile.c)

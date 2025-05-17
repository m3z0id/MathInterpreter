cmake_minimum_required(VERSION 3.30)
project(MathInterpreter C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(MathInterpreter
        Helpers/DataTypes.h
    MathEval.c
        Helpers/CustomMath.h
        Lexer.h
        Input.h
        Helpers/AdditionalString.h)

set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -march=native -ffast-math")

target_link_libraries(MathInterpreter m)

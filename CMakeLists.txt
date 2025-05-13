cmake_minimum_required(VERSION 3.30)
project(MathInterpreter C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(MathInterpreter
    DataTypes.h
    MathEval.c
    CustomMath.h)

target_link_libraries(MathInterpreter m)

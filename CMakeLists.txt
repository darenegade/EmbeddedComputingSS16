cmake_minimum_required(VERSION 3.4)
project(EmbeddedComputingSS16)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES Aufgabe1/Aufgabe1.c)
add_executable(Aufgabe1 ${SOURCE_FILES})

set(SOURCE_FILES Aufgabe2/Aufgabe2.c)
add_executable(Aufgabe2 ${SOURCE_FILES})
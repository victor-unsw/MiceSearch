cmake_minimum_required(VERSION 3.2)
project(bazinga)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(bazinga ${SOURCE_FILES} inc/proceeding.h src/proceeding.cpp inc/posting.h src/posting.cpp inc/dictionary.h src/dictionary.cpp inc/indexer.h src/indexer.cpp inc/askMe.h src/askMe.cpp)
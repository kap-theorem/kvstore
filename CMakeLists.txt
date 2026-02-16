cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER clang++)

project(
    KVStore
    VERSION 1.0
    LANGUAGES CXX
)

include_directories(include)

add_executable(kv_run src/main.cpp src/kvstore.cpp)
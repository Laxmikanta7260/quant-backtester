cmake_minimum_required(VERSION 3.10)
project(quant_backtester)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(quant_backtester
    src/main.cpp
    src/data_loader.cpp
    src/strategy.cpp
    src/backtester.cpp
    src/metrics.cpp
    src/tuner.cpp
)
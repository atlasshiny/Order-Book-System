cmake_minimum_required(VERSION 3.20)

project(OrderBookSimulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    asio
    URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-38-0.tar.gz
)
FetchContent_MakeAvailable(asio)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(OrderBookSimulator ${SOURCES})

target_include_directories(OrderBookSimulator PRIVATE include)

find_package(Threads REQUIRED)
target_link_libraries(OrderBookSimulator PRIVATE asio::asio Threads::Threads)
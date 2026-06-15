cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0135 NEW)

project(OrderBookSimulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    asio
    URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-38-0.tar.gz
)
FetchContent_MakeAvailable(asio)

add_library(asio_target INTERFACE)
target_include_directories(asio_target INTERFACE 
    ${asio_SOURCE_DIR}/asio/include
    ${asio_SOURCE_DIR}/include
)
target_compile_definitions(asio_target INTERFACE ASIO_STANDALONE)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(OrderBookSimulator ${SOURCES})

target_include_directories(OrderBookSimulator PRIVATE include)

find_package(Threads REQUIRED)

target_link_libraries(OrderBookSimulator PRIVATE 
    asio_target 
    Threads::Threads
)
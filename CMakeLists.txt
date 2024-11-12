set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR/toolchain.cmake})

cmake_minimum_required(VERSION 3.5.0)
project(clash_to_singbox VERSION 0.1.0 LANGUAGES CXX)

add_executable(clash_to_singbox 
    src/main.cpp
    src/proxy/proxy.cpp)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(yaml-cpp REQUIRED)
target_link_libraries(clash_to_singbox yaml-cpp)

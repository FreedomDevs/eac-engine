cmake_minimum_required(VERSION 3.10)

project(eac-engine)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SRC_FILES
    "main.cpp"
    "network/*.cpp"
)

add_executable(eac-engine ${SRC_FILES})

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_MakeAvailable(json)

target_link_libraries(eac-engine PRIVATE nlohmann_json::nlohmann_json)

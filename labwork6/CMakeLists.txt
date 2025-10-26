cmake_minimum_required(VERSION 3.12)

project(
    labwork6
    VERSION 1.0
    DESCRIPTION "C++ Year 2025 labwork6"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)


add_subdirectory(lib)
add_subdirectory(bin)


include(FetchContent)
FetchContent_Declare(
    cpr 
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG dec9422db3af470641f8b0d90e4b451c4daebf64
) 

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
        GIT_TAG main
)

FetchContent_MakeAvailable(ftxui)

FetchContent_MakeAvailable(cpr)
FetchContent_MakeAvailable(nlohmann_json)

# enable_testing()
# add_subdirectory(tests)
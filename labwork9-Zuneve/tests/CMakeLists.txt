include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        lab9_tests
        my_tests.cpp
)

target_link_libraries(
        lab9_tests
        GTest::gtest_main
        GTest::gmock_main
)

target_include_directories(lab9_tests PUBLIC ..)

include(GoogleTest)

gtest_discover_tests(lab9_tests)

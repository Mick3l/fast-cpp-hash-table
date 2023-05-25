include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

add_executable(Tests_hash tests_hash.cpp)
target_link_libraries(Tests_hash PRIVATE GTest::gtest_main)
target_include_directories(Tests_hash PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(Tests_hash)


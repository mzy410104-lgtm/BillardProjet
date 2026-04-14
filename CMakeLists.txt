cmake_minimum_required(VERSION 3.14)

project(BilliardProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(TEST_SOURCES ${SOURCES})
list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

include(FetchContent)
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)
FetchContent_MakeAvailable(sfml)

add_executable(BilliardSim ${SOURCES})

target_link_libraries(BilliardSim PRIVATE sfml-graphics sfml-window sfml-system)

if(WIN32)
    add_custom_command(TARGET BilliardSim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics> $<TARGET_FILE:sfml-window> $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:BilliardSim>
    )
endif()

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(UnitTests
    tests/test_math.cpp
    tests/test_collision.cpp 
    ${TEST_SOURCES}
)

target_link_libraries(UnitTests 
    PRIVATE 
    GTest::gtest_main
    sfml-graphics sfml-window sfml-system # <--- 加上这一行！
)

if(WIN32)
    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics> $<TARGET_FILE:sfml-window> $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:UnitTests>
    )
endif()

include(GoogleTest)
gtest_discover_tests(UnitTests)

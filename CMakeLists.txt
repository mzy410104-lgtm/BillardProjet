cmake_minimum_required(VERSION 3.14)

# 项目名称
project(BilliardProject VERSION 1.0 LANGUAGES CXX)

# 设置 C++ 标准 (C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# --- 1. 准备源文件 ---
# 自动查找 src 下的所有 .cpp 文件
file(GLOB SOURCES "src/*.cpp")

# 创建一个不包含 main.cpp 的源文件列表 (用于测试)
# 因为测试程序有自己的 main 函数，不能和游戏的 main 函数冲突
set(TEST_SOURCES ${SOURCES})
list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# --- 2. 配置 SFML (图形库) ---
include(FetchContent)
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)
FetchContent_MakeAvailable(sfml)

# --- 3. 生成主游戏 (BilliardSim) ---
# 注意：先 add_executable，再 target_link_libraries
add_executable(BilliardSim ${SOURCES})

# 链接 SFML 库
target_link_libraries(BilliardSim PRIVATE sfml-graphics sfml-window sfml-system)

# (Windows 专用) 自动复制 DLL 到输出目录
if(WIN32)
    add_custom_command(TARGET BilliardSim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics> $<TARGET_FILE:sfml-window> $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:BilliardSim>
    )
endif()

# --- 4. 配置 Google Test (单元测试) ---
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# 强制使用共享 CRT (防止 Windows 编译报错)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# --- 5. 生成测试程序 (UnitTests) ---
# 注意：这里我们要把刚才写的 tests/test_collision.cpp 也加进去
# 如果你有 tests/test_math.cpp 也要加在里面
add_executable(UnitTests
    tests/test_math.cpp
    tests/test_collision.cpp 
    ${TEST_SOURCES}
)

# 链接 Google Test 库
target_link_libraries(UnitTests GTest::gtest_main)

# 让 CMake 发现测试
include(GoogleTest)
gtest_discover_tests(UnitTests)
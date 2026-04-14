cmake_minimum_required(VERSION 3.14)

# 1. 项目设置
project(BilliardProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含项目自己的头文件目录 
include_directories(${CMAKE_SOURCE_DIR}/include)

# --- 2. 准备源文件 ---
# 自动查找 src 下的所有 .cpp 文件 (返回的是绝对路径)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 创建一个不包含 main.cpp 的源文件列表 (用于测试)
set(TEST_SOURCES ${SOURCES})
list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# --- 3. 配置 SFML (图形库) ---
# CMake 会自动下载并管理路径，不需要你手动指定 include/lib
include(FetchContent)
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)
FetchContent_MakeAvailable(sfml)

# --- 4. 生成主游戏 (BilliardSim) ---
add_executable(BilliardSim ${SOURCES})

# 【关键】链接 SFML 到主游戏
target_link_libraries(BilliardSim PRIVATE sfml-graphics sfml-window sfml-system)

# (Windows 专用) 自动复制 DLL 到输出目录
if(WIN32)
    add_custom_command(TARGET BilliardSim POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics> $<TARGET_FILE:sfml-window> $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:BilliardSim>
    )
endif()

# --- 5. 配置 Google Test (单元测试) ---
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# 强制使用共享 CRT (防止 Windows 编译报错)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# --- 6. 生成测试程序 (UnitTests) ---
add_executable(UnitTests
    tests/test_math.cpp
    tests/test_collision.cpp 
    ${TEST_SOURCES}
)

# 【这里是修复的核心！】
# 测试程序也引用了 Renderer.hpp，所以它也必须链接 SFML，否则找不到头文件
target_link_libraries(UnitTests 
    PRIVATE 
    GTest::gtest_main
    sfml-graphics sfml-window sfml-system # <--- 加上这一行！
)

# (可选) 如果运行测试报错缺dll，也给测试程序加上复制DLL的命令
if(WIN32)
    add_custom_command(TARGET UnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics> $<TARGET_FILE:sfml-window> $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:UnitTests>
    )
endif()

include(GoogleTest)
gtest_discover_tests(UnitTests)
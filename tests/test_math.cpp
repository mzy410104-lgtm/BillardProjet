// 文件：src/test_math.cpp (或者直接写在 main.cpp 里)
#include <iostream>
#include <cassert>  // 引入断言库
#include "Vector2D.hpp"

int main() {
    std::cout << "Running Vector2D tests..." << std::endl;

    // --- 测试案例 1: 向量加法 ---
    // 你的具体需求：验证 (1,0) + (0,1)
    Vector2D v1(1.0f, 0.0f);
    Vector2D v2(0.0f, 1.0f);
    
    Vector2D sum = v1 + v2;

    // 输出结果看看
    std::cout << "v1 + v2 = (" << sum.x << ", " << sum.y << ")" << std::endl;

    // 核心验证代码：
    // assert 的作用是：如果括号里的条件为假，程序会立刻报错并终止。
    // 如果条件为真，程序继续运行，什么都不发生。
    assert(sum.x == 1.0f && sum.y == 1.0f);
    
    // 或者使用我们在类里重载的 == 运算符
    assert(sum == Vector2D(1.0f, 1.0f));

    std::cout << "[PASS] Vector Addition Test Passed!" << std::endl;


    // --- 测试案例 2: 点积 (额外测试) ---
    // (1, 0) 和 (0, 1) 是垂直的，点积应该为 0
    float dotProduct = v1.dot(v2);
    assert(dotProduct == 0.0f);
    std::cout << "[PASS] Dot Product Test Passed!" << std::endl;

    // --- 测试案例 3: 模长 ---
    // (3, 4) 的长度应该是 5
    Vector2D v3(3.0f, 4.0f);
    assert(v3.length() == 5.0f);
    std::cout << "[PASS] Length Test Passed!" << std::endl;

    std::cout << "All tests passed successfully." << std::endl;
    
    return 0; // 返回 0 代表程序正常结束
}
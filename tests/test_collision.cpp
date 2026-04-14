#include <gtest/gtest.h>
#include "../include/Boule.hpp"

// 测试场景：完全弹性碰撞的速度交换
// 就像牛顿摆：左边的球撞右边的球，左边停下，右边飞出
TEST(PhysicsTest, ElasticCollisionSwap) {
    // 1. 准备两个质量相等 (m=1.0) 的球
    // 初始位置：(0,0) 和 (15,0)。半径都是10，所以它们重叠了5个像素，必然触发碰撞。
    Boule b1(Vector2D(0.0f, 0.0f), 10.0f, 1.0f);
    Boule b2(Vector2D(15.0f, 0.0f), 10.0f, 1.0f);

    // 2. 设置初速度 (头对头撞击)
    // b1 向右飞 (速度 100)
    b1.setVelocity(Vector2D(100.0f, 0.0f));
    // b2 向左飞 (速度 -100)
    b2.setVelocity(Vector2D(-100.0f, 0.0f));

    // 3. 执行碰撞逻辑 (调用成员 A 写的函数)
    // 注意：确保 Boule::resolveCollision 是 public 的
    b1.resolveCollision(b2);

    // 4. 验证结果
    // 预期：速度完全交换
    // b1 原来是 100，现在应该变成 -100 (被弹回去)
    // b2 原来是 -100，现在应该变成 100
    
    // 获取新速度
    Vector2D v1 = b1.getVelocity();
    Vector2D v2 = b2.getVelocity();

    // Google Test 浮点数断言
    EXPECT_NEAR(v1.x, -100.0f, 0.1f); // 允许 0.1 的误差
    EXPECT_NEAR(v2.x, 100.0f, 0.1f);
    
    // Y 轴速度应该还是 0
    EXPECT_NEAR(v1.y, 0.0f, 0.1f);
    EXPECT_NEAR(v2.y, 0.0f, 0.1f);
}

// 附加测试：一动一静
TEST(PhysicsTest, MovingHitsStationary) {
    Boule moving(Vector2D(0,0), 10, 1);
    Boule stationary(Vector2D(15,0), 10, 1);
    
    moving.setVelocity(Vector2D(50, 0));
    stationary.setVelocity(Vector2D(0, 0)); // 静止
    
    moving.resolveCollision(stationary);
    
    // 预期：动的停下，静的飞出 (速度交换)
    EXPECT_NEAR(moving.getVelocity().x, 0.0f, 0.1f);
    EXPECT_NEAR(stationary.getVelocity().x, 50.0f, 0.1f);
}
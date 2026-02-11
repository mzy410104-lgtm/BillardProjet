#pragma once
#include "vector2D.hpp"

class Boule{
private:
    //位置和速度
    Vector2D position;
    Vector2D velocity;
    //半径和质量
    float radius;
    float mass;
    //摩擦系数和弹性系数
    float frictionCoeff;
    float restitutionCoeff;
    
public:
    Boule(Vector2D pos,float r,float m);

    //设置获取位置速度
    void setVelocity(Vector2D v);
    Vector2D getVelocity() const;
    Vector2D getPosition() const;
    float getRadium() const;

    //欧拉积分处理位置和摩擦
    void update(float dt);

    //墙壁碰撞检测
    void checkWallCollision(float tableWidth,float tableHeight);

    //球之间的碰撞
    void resolveCollision(Boule& other);
};

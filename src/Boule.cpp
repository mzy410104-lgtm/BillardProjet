#include "Boule.hpp"
#include <cmath>
#include <algorithm>

Boule::Boule(Vector2D pos,float r,float m) : position(pos) ,radius(r) ,mass(m) ,velocity(0,0) {
    frictionCoeff = 0.5f;
    restitutionCoeff = 0.9f;
}

void Boule::setVelocity(Vector2D v){
    velocity = v;
}

Vector2D Boule::getVelocity() const{
    return velocity;
}

Vector2D Boule::getPosition() const{
    return position;
}

float Boule::getRadium() const{
    return radius;
}

void Boule::update(float dt){
    //位置改变
    position = position + (velocity * dt);

    //摩擦力
    float decay = 1.0f - (frictionCoeff * dt);
    //防止摩擦力或时间步长过大
    if (decay < 0.0f) decay = 0.0f;
    //更新速度
    velocity = velocity * decay;
    //如果速度很小，直接停下
    if (velocity.length() < 1.0f) velocity = Vector2D(0,0);

}

void Boule::checkWallCollision(float tableWidth,float tableHeight){
    //如果上下边沿，y轴方向反向
    if ((position.y - radius) < 0 ){
        position.y = radius;
        velocity.y = - velocity.y * restitutionCoeff;
    }

    else if ((position.y + radius) > tableHeight){
        position.y = tableHeight - radius;
        velocity.y = - velocity.y * restitutionCoeff;
    }

    //如果是左右边缘，x轴方向反向
    if((position.x - radius) < 0 ){
        position.x = radius;
        velocity.x = -velocity.x * restitutionCoeff;
    }

    else if((position.x + radius) > tableWidth ){
        position.x = tableWidth - radius;
        velocity.x = -velocity.x * restitutionCoeff;
    }
}

void Boule::resolveCollision(Boule& other){
    Vector2D delta = position - other.position;
    float dist= delta.length();
    float totalRadius = radius + other.radius;

    //如果距离大于半径，则没撞上
    if (dist > totalRadius){
        return;
    }
    //防止球黏在一起
    Vector2D n = delta.normalize();
    float overlap = totalRadius - dist;
    //质量相等，各退一半
    Vector2D correction = n *(overlap / 2.0f);
    this->position = this ->position + correction;
    other.position = other.position - correction;

    //根据物理定律改变速度
    //计算相对速度
    Vector2D relativeVelocity = this ->velocity - other.velocity;
    //计算在法向的分量
    float velocityAlongNormal = relativeVelocity.dot(n);
    if (velocityAlongNormal > 0){
        return;
    }
    //弹性系数 e
    float e = 1.0f;

    //动量守恒
    float j = -(1.0f + e) * velocityAlongNormal;
    j /= (1.0f/this ->mass + 1.0f/other.mass);
    //计算冲量
    Vector2D impluse = n *j;
    //冲量改变速度
    this ->velocity = this ->velocity +(impluse * (1.0f/this ->mass));
    other.velocity = other.velocity - (impluse * (1.0f/other.mass));
    
}
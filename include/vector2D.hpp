#pragma once
#include <cmath>

class Vector2D{
public:
    float x,y;
    //构建函数
    Vector2D(float x =0.0f,float y =0.0f);
    
    //向量加法(+)
    Vector2D operator+(const Vector2D& other) const;

    //向量减法(-)
    Vector2D operator-(const Vector2D& other) const;

    //乘法运算
    Vector2D operator*(float scale) const;

    //点乘--计算角度和投影
    float dot(const Vector2D& other) const;

    //模长--计算向量长度
    float length() const;

    //判断向量是否一致
    bool operator==(const Vector2D& other) const;

    //归一化
    Vector2D normalize() const;
};

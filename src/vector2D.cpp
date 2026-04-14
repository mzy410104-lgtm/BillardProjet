#include "vector2D.hpp"
#include <cmath>

Vector2D::Vector2D(float x,float y) : x(x),y(y) {};

Vector2D Vector2D::operator+(const Vector2D& other) const{
    return Vector2D(x + other.x,y + other.y);
}

Vector2D Vector2D::operator-(const Vector2D& other) const{
    return Vector2D(x - other.x,y - other.y);
}

Vector2D Vector2D::operator*(float scale) const{
    return Vector2D(x * scale,y * scale);
}

float Vector2D::dot(const Vector2D& other) const{
    return (x * other.x + y * other.y);
}

float Vector2D::length() const{
    return std::sqrt(x * x + y * y);
}

bool Vector2D::operator==(const Vector2D& other) const{
    return ((x == other.x) && (y  == other.y));
}

Vector2D Vector2D::normalize() const{
    float len=length();
    if (len != 0){
        return Vector2D(x/len,y/len);
    }
    return Vector2D(0.0f,0.0f);
}
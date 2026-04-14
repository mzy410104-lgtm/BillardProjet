#pragma once
#include <cmath>

class Vector2D{
public:
    float x,y;
    
    // Constructeur
    Vector2D(float x =0.0f,float y =0.0f);
    
    // Addition vectorielle
    Vector2D operator+(const Vector2D& other) const;

    // Soustraction vectorielle
    Vector2D operator-(const Vector2D& other) const;

    // Multiplication par un scalaire
    Vector2D operator*(float scale) const;

    // Produit scalaire
    float dot(const Vector2D& other) const;

    // Norme du vecteur
    float length() const;

    // Test d'égalité
    bool operator==(const Vector2D& other) const;

    // Normalisation
    Vector2D normalize() const;
};
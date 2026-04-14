#pragma once
#include "vector2D.hpp"

// Définition des types de billes
enum BallType{
    WHITE_BALL, // Bille blanche
    BLACK_BALL, // Bille noire
    RED_BALL,   // Bille rouge
    BLUE_BALL   // Bille bleue
};


class Boule{
private:
    // Position et vitesse
    Vector2D position;
    Vector2D velocity;
    
    // Rayon et masse
    float radius;
    float mass;
    
    // Coefficients de friction et de restitution
    float frictionCoeff;
    float restitutionCoeff;

    // État de la bille (active sur la table ou empochée)
    bool isActive;

    // Type de la bille
    BallType type;

    // --- Vitesse angulaire ---
    // Rotation autour des axes X et Y (roulement et effets longitudinaux)
    Vector2D spinXY; 
    // Rotation autour de l'axe Z (effet latéral / side-spin)
    float spinZ;

    float rotationAngle;
    
public:
    // Constructeur
    Boule(Vector2D pos,float r,float m, BallType t);

    // Accesseurs et mutateurs (Position et Vitesse)
    void setVelocity(Vector2D v);
    void setPosition(Vector2D pos);
    Vector2D getVelocity() const;
    Vector2D getPosition() const;
    float getRadium() const;

    // Gestion de l'état d'activité
    bool isBallActive() const{return isActive; }
    void setInactive() {isActive = false; }

    // --- Réactivation de la bille ---
    void revive() { isActive = true; }

    // Accesseur du type de bille
    BallType getType() const{return type; }

    // Mise à jour physique (Intégration d'Euler et friction)
    void update(float dt);

    // Gestion des collisions avec les bandes
    void checkWallCollision(float tableWidth,float tableHeight);

    // Résolution des collisions inter-billes
    void resolveCollision(Boule& other);

    // Accesseurs et mutateurs (Rotation)
    void setSpin(Vector2D sXY, float sZ) { spinXY = sXY; spinZ = sZ; }
    Vector2D getSpinXY() const { return spinXY; }
    float getSpinZ() const { return spinZ; }

    float getRotationAngle() const { return rotationAngle; }
};
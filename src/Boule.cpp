#include "Boule.hpp"
#include <cmath>

Boule::Boule(Vector2D pos,float r,float m, BallType t) : position(pos) ,radius(r) ,mass(m) ,velocity(0,0), isActive(true), type(t), spinXY(0,0), spinZ(0.0f), rotationAngle(0.0f){
    frictionCoeff = 0.5f;
    restitutionCoeff = 0.9f;
}

void Boule::setVelocity(Vector2D v){
    velocity = v;
}

void Boule::setPosition(Vector2D pos){
    position = pos;
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
    // Mise à jour de la position
    position = position + (velocity * dt);

    // --- Transition du glissement au roulement ---
    // Calcul de la vitesse de glissement par rapport à l'état de roulement pur
    Vector2D slipVelocity = spinXY - velocity; 
    float slipSpeed = slipVelocity.length();

    if (slipSpeed > 0.1f) {
        // Paramètre de friction de glissement
        float frictionStep = 250.0f * dt; 
        
        // Transition fluide vers le roulement pur
        if (frictionStep * 3.5f > slipSpeed) {
            float F = slipSpeed / 3.5f;
            Vector2D slipDir = slipVelocity.normalize();
            velocity = velocity + (slipDir * F);
            spinXY = velocity; // Synchronisation (roulement pur)
        } else {
            Vector2D slipDir = slipVelocity.normalize();
            velocity = velocity + (slipDir * frictionStep);
            // Atténuation de la rotation due à la friction
            spinXY = spinXY - (slipDir * frictionStep * 3.0f); 
        }
    }

    // --- Friction de roulement (décélération linéaire) ---
    float speed = velocity.length();
    if (speed > 0.0f) {
        // Décélération constante par seconde
        float deceleration = 120.0f * dt; 
        float newSpeed = speed - deceleration;

        if (newSpeed <= 0.0f) {
            velocity = Vector2D(0, 0); // Arrêt de la bille
        } else {
            // Réduction de la vitesse avec conservation de la direction
            velocity = velocity * (newSpeed / speed); 
        }
    }
    
    // Atténuation progressive du spin latéral
    spinZ *= (1.0f - (frictionCoeff * 0.5f * dt));
    
    // --- Conversion de la vitesse en roulement visuel ---
    float currentSpeed = velocity.length();
    if (currentSpeed > 0.1f) {
        // Calcul de la distance parcourue
        float distance = currentSpeed * dt;
        // Conversion en degrés
        float rollAngle = (distance / radius) * (180.0f / 3.14159265f);
        
        // Détermination de la direction de rotation
        float sign = (velocity.x + velocity.y > 0) ? 1.0f : -1.0f;
        
        rotationAngle += rollAngle * sign; 
    }

    // Ajout de l'effet visuel du spin latéral
    rotationAngle += spinZ * 100.0f * dt; 

    // Maintien de l'angle entre 0 et 360 degrés
    if (rotationAngle > 360.0f) rotationAngle -= 360.0f;
    if (rotationAngle < 0.0f)  rotationAngle += 360.0f;
    
    
    // Seuils d'arrêt
    if (spinXY.length() < 1.0f) spinXY = Vector2D(0,0);
    if (std::abs(spinZ) < 0.01f) spinZ = 0.0f;
}

void Boule::checkWallCollision(float tableWidth,float tableHeight){
    // Ignorer la collision si la bille est inactive (empochée)
    if (!isActive) return;
    
    // Coefficient de friction de la bande
    float cushionFriction = 0.2f; 
    float spinTransfer = 0.5f;    

    // --- Bandes supérieure et inférieure ---
    if ((position.y - radius) < 0 || (position.y + radius) > tableHeight) {
        if (position.y - radius < 0) {
            position.y = radius; // Bande supérieure
        } else {
            position.y = tableHeight - radius; // Bande inférieure
        }
        
        // Inversion et atténuation de la vitesse normale
        velocity.y = -velocity.y * restitutionCoeff;
        
        // Application de la friction tangentielle et effet du spin latéral
        float relativeTangentialVel = velocity.x + (spinZ * radius);
        
        velocity.x -= relativeTangentialVel * cushionFriction;
        
        // Perte d'une partie du spin latéral après le contact
        spinZ -= spinZ * cushionFriction;
    }

    // --- Bandes gauche et droite ---
    if ((position.x - radius) < 0 || (position.x + radius) > tableWidth) {
        if (position.x - radius < 0) {
            position.x = radius; // Bande gauche
        } else {
            position.x = tableWidth - radius; // Bande droite
        }
        
        // Inversion et atténuation de la vitesse normale
        velocity.x = -velocity.x * restitutionCoeff;
        
        // Application de la friction tangentielle et effet du spin latéral
        float relativeTangentialVel = velocity.y - (spinZ * radius);
        
        velocity.y -= relativeTangentialVel * cushionFriction;
        spinZ -= spinZ * cushionFriction;
    }
}

void Boule::resolveCollision(Boule& other){
    Vector2D delta = position - other.position;
    float dist= delta.length();
    float totalRadius = radius + other.radius;

    // Vérification de l'absence de collision
    if (dist > totalRadius){
        return;
    }
    
    // Correction statique pour éviter le chevauchement (overlap)
    Vector2D n = delta.normalize();
    float overlap = totalRadius - dist;
    
    // Séparation proportionnelle à la pénétration
    Vector2D correction = n *(overlap / 2.0f);
    this->position = this ->position + correction;
    other.position = other.position - correction;

    // Calcul de la vitesse relative
    Vector2D relativeVelocity = this ->velocity - other.velocity;
    
    // Calcul de la vitesse le long de la normale
    float velocityAlongNormal = relativeVelocity.dot(n);
    if (velocityAlongNormal > 0){
        return;
    }
    
    // Coefficient de restitution
    float e = 1.0f;

    // Calcul de l'impulsion (Conservation de la quantité de mouvement)
    float j = -(1.0f + e) * velocityAlongNormal;
    j /= (1.0f/this ->mass + 1.0f/other.mass);
    
    Vector2D impluse = n *j;
    
    // Application de l'impulsion pour modifier les vitesses
    this ->velocity = this ->velocity +(impluse * (1.0f/this ->mass));
    other.velocity = other.velocity - (impluse * (1.0f/other.mass));
    
}
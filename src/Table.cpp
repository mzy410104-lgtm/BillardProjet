#include "Table.hpp"
#include <cmath>

Table::Table(float w,float h) : width(w),height(h){};

void Table::addBall(const Boule& ball){
    balls.push_back(ball);
}

const std::vector<Boule>& Table::getBalls() const{
    return balls;
}

void Table::update(float dt){
    for (auto& ball : balls){
        ball.update(dt);
        ball.checkWallCollision(width,height);
    }
    for (size_t i = 0; i < balls.size(); ++i){
        for (size_t j = i + 1; j < balls.size(); ++j){
            balls[i].resolveCollision(balls[j]);
        }
        
    }       
}

bool Table::allBallsStopped() const{
    const float STOP_THRESHOLD = 3.0f;

    for (const auto& ball: balls){
        if(ball.getVelocity().length() > STOP_THRESHOLD){
            return false;
        }
    }
    return true;
}